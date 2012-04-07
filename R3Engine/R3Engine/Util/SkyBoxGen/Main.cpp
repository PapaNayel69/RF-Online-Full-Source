#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include "C_vector.h"
#include "World.h"
#include "Loadase.h"
#include "Jmalloc.h"
//#include "Rad.h"
#include "MeshUtil.h"
#include "Material.h"
#include "jerror.h"
#include "patch.h"
#include "rad.h"
#include "R3bspUtil.h"
#include "DDSExport.h"
#include "entity.h"
#include "r3blockmat.h"
#include "loadprescript.h"



void StripSky(char *name)
{
	int s=strlen(name);
	if( name[s-3]=='s' && name[s-2]=='k' && name[s-1]=='y' )
	{
		name[s-3]=NULL;
	}
	else
	{
		Error(GetIniFile()->In_file," <- 이름끝에 sky를 붙여야합니다.");
	}
}


//--------------------------------------------------------------------------------------
R3BlockBsp *TempCBlockBsp;

void main(int argc, char **argv)
{
//-------------------- Map를 파일창에서 불러온다.
	static OPENFILENAME ofn; // zero the ofn members out
    char ase_name[256]="1.map";

	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = ase_name;
	ofn.nMaxFile = sizeof(ase_name);
	ofn.lpstrFilter = "R3MAP MAP (*.MAP)\0*.MAP\0All (*.*)\0*.*\0";
	ofn.lpstrTitle = "Select MAP file";
	ofn.Flags = OFN_NOCHANGEDIR | OFN_HIDEREADONLY |
               OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;


	if (!GetOpenFileName(&ofn))
		return;
	strlwr(ase_name);

	strcpy(GetIniFile()->In_file,ase_name);

	_INIFILE *Ini=GetIniFile();
	SetR3BspInfoFileName(Ini->In_file);	//로그파일 초기화
	Ini->CPUNum=1;
	PutR3BspInfoText("Start Skybox Generator!\n");

	//-------------------------------------------------
	_MESH *load_mesh;	//첨 로딩했던 매쉬
	_MESH *w_mesh;		//월드로 고친메쉬
	_MESH *p_mesh;
	_MATERIAL *org_mat,*mat;

    _SCENE *scene=LoadSceneASE(ase_name);
    load_mesh=LoadMeshASE(ase_name);
    if(load_mesh==NULL)
    {
        puts("object not found!");
        exit(0);
    }
    GetInvScaleMatrix(load_mesh);		//애니메이션을위한 inv스케일매트릭스를 구한다.

	int i;
	for(i=0; i<load_mesh->objectnum; i++)
	{
		if( load_mesh->object[i].Pos_cnt || load_mesh->object[i].Rot_cnt )
		{
			load_mesh->object[i].start_frame = 0;
			load_mesh->object[i].end_frame = scene->EndFrame;
			load_mesh->object[i].type|=_DYNAMIC_OBJECT;	//애니메이션 오브젝트
			load_mesh->object[i].type|=_NO_MERGE_OBJECT;	//애니메이션 오브젝트도 머지하면 안된다.
			FixTrack(&load_mesh->object[i]);
		}
	}
	
	
	org_mat = LoadMaterialASE(ase_name);

	SetWorldMesh(load_mesh,org_mat);  // wx,wy,wz를 구하기위해
	w_mesh = load_mesh;

//	one_mesh=ConvOneMesh(w_mesh,org_mat);  // 정적인것은 하나의 메쉬로
//	SetMeshFaceNormal(one_mesh,load_mesh);			// 삼각메쉬의 normal을 구하자. 
//	p_mesh=ConvMeshTriToPoly(one_mesh);  // 하나의 삼각메쉬를 폴리곤메쉬로

	SetMeshFaceNormal(w_mesh,load_mesh);			// 삼각메쉬의 normal을 구하자. 
	p_mesh=ConvMeshTriToPoly(w_mesh);  // 하나의 삼각메쉬를 폴리곤메쉬로

	mat = ConvOneMaterial(org_mat);	//반드시 ConvOneMesh 다음에 호출할것...오리지날 메쉬를 하나로...
	AdjustMeshMaterial(p_mesh,mat);	//머터리얼 정리.....

	//-----블록 설정을 준비한다.
	DWORD p_face_num = GetFaceNumFromMesh(p_mesh);

	R3BlockBsp CBlockBsp(p_face_num);						//블록설정 준비..
	CBlockBsp.PrepareBlockBsp(p_mesh,mat);			//블록들을 합쳐놓는다.
	TempCBlockBsp = &CBlockBsp;

	//------------팻치생성
	MakeVertexPatchAndWinding(p_mesh);	// 통자메쉬를 인자로....

	//------------라이트계산
    _LIGHTSOURCE *lgt_source=LoadLightASE(ase_name);	//라이트로드
	if( lgt_source )
	{
		for( i=0; i<lgt_source->light_num; i++)
		{
			lgt_source[i].is_shadow = FALSE;		//스카이박스는 버텍스 라이팅이므로 그림자는 끈다.
		}
	}

	GetLightRadiosity(p_mesh,mat,lgt_source);		// 라이트에 대한 래디오시티를 구한다.


	char t_path[256];
	char skyname[256];

	strcpy(skyname,ase_name);
	StripEXT(skyname);
	StripPath(skyname);
	char filename[256]=".\\Map\\";
	strcat(filename,skyname);
	StripSky(filename);
	strcat(filename,"\\");
	strcat(filename,"Sky\\");
	strcpy(t_path,filename);
	strcat(filename,skyname);
	strcat(filename,".R3E");

	MakeDir(t_path);
	if( !MaterialExport(mat,t_path) )		//머터리얼 스크립터로 출력한다.
	{
		StripSky(skyname);
		Error(skyname,"<-이맵이름의 폴더가 없습니다. 스카이맵을 만들기전에 맵부터 만들어주세요");
	}
	//-------------------- 텍스쳐를 DDS로 컨버트 
	PutR3BspInfoText("DDS Texture Export!\n");
	DDSTextureExport(p_mesh,mat,t_path);		//dds 텍스쳐로 컨버트한다.

	EntityExport(filename,p_mesh,mat,&CBlockBsp);

	PutR3BspInfoText("Complete!\n");
	PutR3BspInfoText("Press any key to continue\n");
	getch();
}
