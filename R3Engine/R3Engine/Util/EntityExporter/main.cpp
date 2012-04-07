#include <stdio.h>
#include <conio.h>
#include "loadase.h"
#include "r3bsputil.h"
#include "meshutil.h"
#include "patch.h"
#include "rad.h"
#include "jmalloc.h"
#include "ddsexport.h"
#include "material.h"
#include "entity.h"
#include "jerror.h"
#include "loadprescript.h"


void main(void)
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

	strcpy(GetIniFile()->In_file,ase_name);

	_INIFILE *Ini=GetIniFile();
	SetR3BspInfoFileName(Ini->In_file);	//로그파일 초기화
	Ini->CPUNum=1;
	PutR3BspInfoText("Start Entity Generator!\n");

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
		if( load_mesh->object[i].Pos_cnt || load_mesh->object[i].Rot_cnt || load_mesh->object[i].Scale_cnt)
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
	Ini->w_mesh = w_mesh;
	SetMeshFaceNormal(w_mesh,load_mesh);			// 삼각메쉬의 normal을 구하자. 
	p_mesh=ConvMeshTriToPoly(w_mesh);  // 하나의 삼각메쉬를 폴리곤메쉬로

	mat = ConvOneMaterial(org_mat);	//반드시 ConvOneMesh 다음에 호출할것...오리지날 메쉬를 하나로...
	AdjustMeshMaterial(p_mesh,mat);	//머터리얼 정리.....

	DWORD p_num=0;
	for( i=0; i< p_mesh->objectnum; i++ )
		p_num += p_mesh->object[i].facenum;

	//-----블록 설정을 준비한다.
	R3BlockBsp CBlockBsp(p_num);	//블록설정 준비..
	CBlockBsp.PrepareBlockBsp(p_mesh,mat);			//블록들을 합쳐놓는다.
	//------------팻치생성
	MakeVertexPatchAndWinding(p_mesh);	// 통자메쉬를 인자로....

	//------------라이트계산

    No_warning=1;	//워닝 끄자...-.-
	_LIGHTSOURCE *lgt_source=LoadLightASE(ase_name);	//라이트로드
    No_warning=0;	//워닝 키고...-.-
	if( lgt_source )
	{
		for(int i=0; i<lgt_source->light_num; i++)
		{
			lgt_source[i].is_shadow =FALSE;		//스카이박스는 버텍스 라이팅이므로 그림자는 끈다.
		}
	}

	GetLightRadiosity(p_mesh,mat,lgt_source);		// 라이트에 대한 래디오시티를 구한다.


	int j,k;
	_LFACE **lf = GetLFace();
	if( lgt_source )
	{
		//---------엔티티의 버텍스 컬러를...이놈은 multiply 2를 지원한다.
		for(k=0; k<p_mesh->objectnum; k++)
		{
			for(i=0; i<p_mesh->object[k].facenum; i++)
			{
				for(j=0; j<p_mesh->object[k].p_face[i].vertex_num; j++)
				{
					_WINDING *winding=&lf[k][i].patch->winding[j];
		//			winding->energy[0] = min(1.0f,winding->energy[0]);
		//			winding->energy[1] = min(1.0f,winding->energy[1]);
		//			winding->energy[2] = min(1.0f,winding->energy[2]);
					winding->energy[0] = min(255,winding->energy[0]);
					winding->energy[1] = min(255,winding->energy[1]);
					winding->energy[2] = min(255,winding->energy[2]);
				}
			}
		}
	}
	else
	{
		//---------라이트가 하나도 없으면 엔티티의 버텍스 컬러를...
		for(k=0; k<p_mesh->objectnum; k++)
		{
			for(i=0; i<p_mesh->object[k].facenum; i++)
			{
				for(j=0; j<p_mesh->object[k].p_face[i].vertex_num; j++)
				{
					_WINDING *winding=&lf[k][i].patch->winding[j];
		//			winding->energy[0] = min(1.0f,winding->energy[0]);
		//			winding->energy[1] = min(1.0f,winding->energy[1]);
		//			winding->energy[2] = min(1.0f,winding->energy[2]);
					winding->energy[0]=127;
					winding->energy[1]=127;
					winding->energy[2]=127;
				}
			}
		}
	}

	char entityname[256],buf[256];
	char entitypath[256];

	strcpy(entitypath,ase_name);
	StripName(entitypath);

	strcpy(entityname,ase_name);
	StripEXT(entityname);
	StripPath(entityname);

	if( strlen(entityname) > 15)
		Error(entityname,"파일이름이 너무 깁니다. 15자이내로 줄여주세요.");

	strcat(entitypath,entityname);
	strcat(entitypath,"\\");

	//-------------------- 텍스쳐를 DDS로 컨버트 
	PutR3BspInfoText("Material and DDS Texture Export!\n");
	for( i=0; i<mat->all_num; i++)
		mat[i].lgt_id= -1;
	char mdpath[256];
	sprintf(mdpath,"md %s",entitypath);
	system(mdpath);
	
	MaterialExport(mat,entitypath);		//머터리얼 스크립터로 출력한다.

	DDSTextureExport(p_mesh,mat,entitypath);		//dds 텍스쳐로 컨버트한다.

	strcat(entitypath,entityname);
	strcat(entitypath,".R3E");
	EntityExport(entitypath,p_mesh,mat,&CBlockBsp);

	PutR3BspInfoText("Entity Export Complete!\n");
	PutR3BspInfoText("Press any key to continue\n");
	getch();
}

// 내일 당장할일.... 
// 라이트가 없을시 0xffffffff  작업을....
// 그리고 파일포맷을 만들어라..... 이전 파일포맷을 호환가능하게.....

