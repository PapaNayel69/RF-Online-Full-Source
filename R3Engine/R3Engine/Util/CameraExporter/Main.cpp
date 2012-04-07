#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include "C_vector.h"
#include "World.h"
#include "Loadase.h"
#include "Jmalloc.h"
#include "R3BspUtil.h"
#include "MeshUtil.h"
#include "jerror.h"


_HELPER *Helper;
_CAMERA *Camera;
_SCENE *Scene;


int GetParentsNum(char *p_name,_HELPER *helper)
{
	int cnt=0;
	char s_name[256];

	if( p_name == NULL || helper == NULL )
		return 0;

	strcpy(s_name,p_name);
	while( 1 )
	{
		int is_found=0;

		for(int i=0; i<helper->num; i++)
		{
			if( !strcmp(helper[i].objectname,s_name ))
			{
				is_found=1;
				cnt++;
				strcpy(s_name,helper[i].parentname);
				break;
			}
		}
		if( is_found==0 )
			break;
	}
	return cnt;
}

void SaveHelperParent(FILE *fp,char *p_name,_HELPER *helper)
{
	int is_found=0;
	char s_name[256]="";

	if( helper == NULL )
		return;
	for(int i=0; i<helper->num; i++)
	{
		if( !strcmp(helper[i].objectname,p_name ))
		{
			is_found=1;
			strcpy(s_name,helper[i].parentname);
			break;
		}
	}
	if( i == helper->num )
		return;
	if( p_name[0] != NULL )
	{
		SaveHelperParent(fp,s_name,helper);
		fwrite(helper[i].s_matrix,sizeof(D3DMATRIX),1,fp);
		fwrite(helper[i].pos,sizeof(Vector3f),1,fp);
		fwrite(helper[i].quat,sizeof(Vector4f),1,fp);
		fwrite(&helper[i].Pos_cnt,4,1,fp);
		fwrite(&helper[i].Rot_cnt,4,1,fp);
		fwrite(helper[i].Pos,sizeof(_POS_TRACK)*helper[i].Pos_cnt,1,fp);
		fwrite(helper[i].Rot,sizeof(_ROT_TRACK)*helper[i].Rot_cnt,1,fp);

		D3DMATRIX temp;
		D3DMath_MatrixFromQuaternion(temp,helper[i].quat[0],helper[i].quat[1],helper[i].quat[2],helper[i].quat[3]);
	}
}

void GetCameraFactorFromHelper(_CAMERA *camera,_HELPER *helper)	//pos값을 부모 인버스 자기행렬 곱해서 얻어라...
{
	int i,j;
	D3DMATRIX invmat,temp;

	for(i=0; i<camera->num; i++)
	{
		memcpy(&temp,&camera[i].d3d_trmatrix,sizeof(D3DMATRIX));
		if( camera[i].parentname[0]!= NULL)
		{
			if( helper == NULL )
				Error("카메라 부모가 헬퍼가 아님","");
			for(j=0 ;j<helper->num; j++)
			{
				if(! strcmp(helper[j].objectname,camera[i].parentname))
				{					
					D3DMath_MatrixInvert( invmat, helper[j].d3d_trmatrix );
					D3DMath_MatrixMultiply( temp,invmat, camera[i].d3d_trmatrix );
				}
			}
		}

		camera[i].pos[0] = temp._41;
		camera[i].pos[1] = temp._42;
		camera[i].pos[2] = temp._43;

		//D3DMath_QuaternionFromMatrix(camera[i].quat[0],camera[i].quat[1],camera[i].quat[2],camera[i].quat[3],temp);

		D3DMATRIX rot_m,inv_rot;
		D3DMath_MatrixFromQuaternion(rot_m,camera[i].quat[0],camera[i].quat[1],camera[i].quat[2],camera[i].quat[3]);
		//D3DMath_MatrixFromQuaternion(rot_m,helper[i].quat[0],helper[i].quat[1],helper[i].quat[2],helper[i].quat[3]);

		D3DMath_MatrixInvert( inv_rot, rot_m );
		D3DMath_MatrixMultiply( camera[i].d3d_s_matrix ,temp,inv_rot );	//스케일 로컬 매트릭스
//		D3DMath_MatrixMultiply( camera[i].d3d_s_matrix ,inv_rot,temp );	//스케일 로컬 매트릭스
//		camera[i].d3d_s_matrix._41=0;
//		camera[i].d3d_s_matrix._42=0;
//		camera[i].d3d_s_matrix._43=0;
//		camera[i].d3d_s_matrix._44=1;
		D3DMath_MatrixMultiply( temp, camera[i].d3d_s_matrix ,rot_m );	//테스트
	}
}

void GetHelperFactor(_HELPER *helper)	//값을 부모 인버스 자기행렬 곱해서 얻어라...
{
	int i,j;
	D3DMATRIX invmat,temp;

	if( helper == NULL )
		return;
	for(i=0; i<helper->num; i++)
	{
		if( helper[i].parentname[0]!= NULL)
		{
			for(j=0 ;j<helper->num; j++)
			{
				if(! strcmp(helper[j].objectname,helper[i].parentname))
				{
					
					D3DMath_MatrixInvert( invmat, helper[j].d3d_trmatrix );
					D3DMath_MatrixMultiply( temp,invmat, helper[i].d3d_trmatrix );
					break;
					//D3DMath_QuaternionFromMatrix(camera[i].quat[0],camera[i].quat[1],camera[i].quat[2],camera[i].quat[3],temp);
				}
			}
		}
		else
			memcpy(&temp,&helper[i].d3d_trmatrix,sizeof(D3DMATRIX));

		D3DMATRIX rot_m,inv_rot;
		D3DMath_MatrixFromQuaternion(rot_m,helper[i].quat[0],helper[i].quat[1],helper[i].quat[2],helper[i].quat[3]);

		D3DMath_MatrixInvert( inv_rot, rot_m );
		D3DMath_MatrixMultiply( helper[i].d3d_s_matrix ,inv_rot,temp );	//스케일 로컬 매트릭스
		helper[i].d3d_s_matrix._41=0;
		helper[i].d3d_s_matrix._42=0;
		helper[i].d3d_s_matrix._43=0;
		helper[i].d3d_s_matrix._44=1;
		//D3DMath_MatrixMultiply( temp,rot_m ,helper[i].d3d_s_matrix);	//스케일 로컬 매트릭스

		helper[i].pos[0] = temp._41;
		helper[i].pos[1] = temp._42;
		helper[i].pos[2] = temp._43;
	}
}

float version = 1.2f;

void CameraExport(char* asefile)
{
	FILE *fp;
	int i;
	char name[256]=".\\Map\\";
	char t_name[256];

	_INIFILE *Ini=GetIniFile();
	//---------- 출력할 BSP파일이름을 얻어낸다.
//	GetR3BspOutFileName(Ini->In_file,Ini->Out_file);	//출력할 BSP파일이름을 얻어낸다.

    Scene=LoadSceneASE(asefile);		// scene 로드
    Helper=LoadHelperASE(asefile);		// 더미 로드
    Camera=LoadCameraASE(asefile);		// 카메라 로드

	if( Camera == NULL )
	{
		Error("export할 카메라가 없습니다.","");
	}

	StripPath(asefile);
	StripEXT(asefile);
	strcat(name,asefile);
	strcpy(t_name,name);
	strcat(t_name,"\\");
	strcat(t_name,asefile);
	strcat(t_name,".cam");

	GetCameraFactorFromHelper(Camera,Helper);	//값을 부모 인버스 자기행렬 곱해서 얻어라...
	GetHelperFactor(Helper);					//값을 부모 인버스 자기행렬 곱해서 얻어라...

	fp =fopen(t_name,"wb");
	fwrite(&version,4,1,fp);	//항상 버젼부터..

	fwrite(&Camera->num,4,1,fp);
	fwrite(&Scene->StartFrame,4,1,fp);	//시작 프래임.
	fwrite(&Scene->EndFrame,4,1,fp);	//끝 프래임..
	// 최상의 부모를 찾아서 먼저 세이브
	for(i=0; i<Camera->num; i++)
	{
		strlwr(Camera[i].objectname);	//소문자로 포팅
		fwrite(&Camera[i].objectname,64,1,fp);
		fwrite(&Camera[i].fov,4,1,fp);
		fwrite(&Camera[i].tdist,4,1,fp);
		int num=GetParentsNum(Camera[i].parentname,Helper);
		num++;
		fwrite(&num,4,1,fp);
		SaveHelperParent(fp,Camera[i].parentname,Helper);	//부모먼저..

		fwrite(&Camera[i].d3d_s_matrix,sizeof(D3DMATRIX),1,fp);
		fwrite(Camera[i].pos,sizeof(Vector3f),1,fp);
		fwrite(Camera[i].quat,sizeof(Vector4f),1,fp);
		fwrite(&Camera[i].Pos_cnt,4,1,fp);
		fwrite(&Camera[i].Rot_cnt,4,1,fp);
		fwrite(Camera[i].Pos,sizeof(_POS_TRACK)*Camera[i].Pos_cnt,1,fp);
		fwrite(Camera[i].Rot,sizeof(_ROT_TRACK)*Camera[i].Rot_cnt,1,fp);
	}


	//더미 나머지들 저장 하자....
	DWORD h_sub_cnt;	//더미 나머지들...
	
	if( Helper == NULL )
		h_sub_cnt=0;
	else
		h_sub_cnt = (DWORD)Helper->num;

	fwrite(&h_sub_cnt,4,1,fp);
	for( i=0; i<(int)h_sub_cnt; i++)
	{
		fwrite(Helper[i].objectname,64,1,fp);	//자신의 이름과 부모의 이름을 넣는다.
		fwrite(Helper[i].parentname,64,1,fp);	
		fwrite(Helper[i].s_matrix,sizeof(D3DMATRIX),1,fp);
		fwrite(Helper[i].pos,sizeof(Vector3f),1,fp);
		fwrite(Helper[i].quat,sizeof(Vector4f),1,fp);
		fwrite(&Helper[i].Pos_cnt,4,1,fp);
		fwrite(&Helper[i].Rot_cnt,4,1,fp);
		fwrite(Helper[i].Pos,sizeof(_POS_TRACK)*Helper[i].Pos_cnt,1,fp);
		fwrite(Helper[i].Rot,sizeof(_ROT_TRACK)*Helper[i].Rot_cnt,1,fp);
	}

	fclose(fp);
}


void main(int argc, char **argv)
{
//-------------------- Ase를 파일창에서 불러온다.
	static OPENFILENAME ofn; // zero the ofn members out
    char ase_name[256]="1.map";

	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = ase_name;
	ofn.nMaxFile = sizeof(ase_name);
	ofn.lpstrFilter = "R3 MAP (*.MAP)\0*.MAP\0All (*.*)\0*.*\0";
	ofn.lpstrTitle = "Select R3 MAP file";
	ofn.Flags = OFN_NOCHANGEDIR | OFN_HIDEREADONLY |
               OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;


	if (!GetOpenFileName(&ofn))
		return;

	strcpy(GetIniFile()->In_file,ase_name);
//-------------------------------------------------
	CameraExport(ase_name);

	printf("Complete!\n");
	printf("Press any key to continue\n");
	getch();
}
