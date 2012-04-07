#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include"world.h"
#include"loadase.h"
#include"bmplib.h"
#include"jmalloc.h"
#include"r3bsputil.h"
#include"JError.h"

#define _READ_Y	2	//읽을때 바꾸려면...
#define _READ_Z	1

#define _QUAT_SIGN	+

#define _MAX_LIMIT_NUM			20000	//최대 수치를 만개로 해놓는다.

static int Col_object_num=0;
static int Geo_object_num=0;
static int Geo_light_num=0;
static int Geo_camera_num=0;
static float NowTickFrame=0;
static BOOL Now_bmp_load=FALSE;

//--------------------------get scene ----------------------------------//
static void GetSCENE(FILE *fp,_SCENE *scene);

//--------------------------get geo objects ----------------------------------//
static void GetGEOMOBJECT(FILE *fp,_OBJECT *object);
static void GetMESH(FILE *fp,_OBJECT *object);
static void GetMESH_VERTEX_LIST(FILE *fp,_OBJECT *object);
static void GetMESH_FACE_LIST(FILE *fp,_OBJECT *object);
static void GetMESH_TVERTLIST(FILE *fp,_OBJECT *object);
static void GetMESH_TVFACELIST(FILE *fp,_OBJECT *object);
static void GetMESH_TVFACELIST2(FILE *fp,_OBJECT *object);
static void GetMESH_NORMALS(FILE *fp,_OBJECT *object);
static void GetNODE_TM(FILE *fp,_OBJECT *object);
static void GetMAP_DIFFUSE(FILE *fp,_MATERIAL *material);
static void GetMATERIAL(FILE *fp,_MATERIAL *material);
static _MATERIAL *GetMATERIAL_LIST(FILE *fp);
static void GetImsi(FILE *fp,_MESH *mesh);


//--------------------------------- 시작 
void GetImsi(FILE *fp)
{
	char hole[256];

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;
    }
}

void SkipNowGroup(FILE *fp)		//{}괄호 날리기....
{
	char hole[256];

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;
        if (!strcmp(hole,"{"))
		{
			SkipNowGroup(fp);
		}
    }
}

void GetName(FILE *fp,char *hole)	// fscanf로 읽기 지저분한 nodename parentname을 읽어준다.
{
	char name[256];
	int i;

	while(1)
	{
		if( fscanf(fp,"%s",&hole[0])==EOF)
			break;
		if (hole[0] == '"' && hole[strlen(hole)-1] == '"')
		{
			hole[strlen(hole)-1] =NULL;
			strcpy(&hole[0],&hole[1]);
			return;
		}
		if (hole[0] == '"')
		{
			for(i=0;i<200;i++)
			{
				name[i]=fgetc(fp);
				if(name[i] == ' ')
					name[i]='%';
				if(name[i] == '"')
				{
					name[i+1]=NULL;
					break;
				}
			}
			strcat(hole,name);
			hole[strlen(hole)-1] =NULL;
			strcpy(&hole[0],&hole[1]);
			return;
		}
	}
}
void GetNameBmp(FILE *fp,char *hole)	// fscanf로 읽기 지저분한 nodename parentname을 읽어준다.
{
	char name[256];
	int i;

	while(1)
	{
		if( fscanf(fp,"%s",&hole[0])==EOF)
			break;
		if (hole[0] == '"' && hole[strlen(hole)-1] == '"')
		{
			hole[strlen(hole)-1] =NULL;
			strcpy(&hole[0],&hole[1]);
			return;
		}
		if (hole[0] == '"')
		{
			for(i=0;i<200;i++)
			{
				name[i]=fgetc(fp);
				if(name[i] == ' ')
					name[i]=' ';
				if(name[i] == '"')
				{
					name[i+1]=NULL;
					break;
				}
			}
			strcat(hole,name);
			hole[strlen(hole)-1] =NULL;
			strcpy(&hole[0],&hole[1]);
			return;
		}
	}
}

static _MESH *mesh;

_MESH *LoadMeshASE(char *filename)
{
	FILE *fp;
	char hole[256];
    int i,j,k;

	if( (fp=fopen(filename,"rt"))==NULL)
	{
		Error(filename," 요파일이 없다!");
	}
    Geo_object_num=0;
	Col_object_num=0;
	while( 1 )
	{
		if( fscanf(fp,"%s",&hole[0])==EOF)
			break;
        if (!strcmp(hole,"*GEOMOBJECT"))
		{
			Geo_object_num++;
		}
        if (!strcmp(hole,"*COLLISIONOBJECT"))
		{
			Col_object_num++;
		}
	}
    fclose(fp);
    if(Geo_object_num==0)
	{
		Error("","Geo object 가 한개도 없다.");
	}

    fp=fopen(filename,"rt");
    mesh=(_MESH *)(Dmalloc(sizeof(_MESH)));
    memset(mesh,0,sizeof(_MESH));
    mesh->objectnum=Geo_object_num;
    mesh->object=(_OBJECT *)Dmalloc(sizeof(_OBJECT)*Geo_object_num);
    memset(mesh->object,0,sizeof(_OBJECT)*Geo_object_num);
	int now_geo_object_num=0;
    while( 1 )
	{
		if( fscanf(fp,"%s",&hole[0])==EOF)
			break;
        if (!strcmp(hole,"*GEOMOBJECT"))
		{
            mesh->object[now_geo_object_num].material_ref=-1;
            GetGEOMOBJECT(fp,&mesh->object[now_geo_object_num]);
            now_geo_object_num++;
		}
	}

//extern void GetLocalMatrix(_MESH *mesh);
//	GetLocalMatrix(mesh);	//애니에서 쓸 로컬 매트릭스를 구한다.

	//평면노멀과 맵핑 정리...
    for(i=0;i<mesh->objectnum;i++)
    {
        for(j=0;j<mesh->object[i].facenum;j++)
        {
            _VERTEX V[3],N;
            //double mag;
            for(k=0;k<3;k++)
            {
                if(0 > mesh->object[i].tvertexnum &&
                    mesh->object[i].face[j].tv[k] <mesh->object[i].tvertexnum)
                {
                    mesh->object[i].face[j].fu[k]=mesh->object[i].tvertex[mesh->object[i].face[j].tv[k]].uv[0];
                    mesh->object[i].face[j].fv[k]=mesh->object[i].tvertex[mesh->object[i].face[j].tv[k]].uv[1];
//                    mesh->object[i].face[j].fw[k]=mesh->object[i].tvertex[mesh->object[i].face[j].tv[k]].w;
                }
            }
            memcpy(&V[0],&mesh->object[i].vertex[mesh->object[i].face[j].v[0]],sizeof(_VERTEX));
            memcpy(&V[1],&mesh->object[i].vertex[mesh->object[i].face[j].v[1]],sizeof(_VERTEX));
            memcpy(&V[2],&mesh->object[i].vertex[mesh->object[i].face[j].v[2]],sizeof(_VERTEX));
            N.local[0]=V[0].local[1]*(V[1].local[2]-V[2].local[2])+V[1].local[1]*(V[2].local[2]-V[0].local[2])+V[2].local[1]*(V[0].local[2]-V[1].local[2]);
            N.local[1]=V[0].local[2]*(V[1].local[0]-V[2].local[0])+V[1].local[2]*(V[2].local[0]-V[0].local[0])+V[2].local[2]*(V[0].local[0]-V[1].local[0]);
            N.local[2]=V[0].local[0]*(V[1].local[1]-V[2].local[1])+V[1].local[0]*(V[2].local[1]-V[0].local[1])+V[2].local[0]*(V[0].local[1]-V[1].local[1]);
			double mind=-1.0;
			mind=max( (N.local[0] < 0 ) ? -N.local[0] : N.local[0] ,mind);
			mind=max( (N.local[1] < 0 ) ? -N.local[1] : N.local[1] ,mind);
			mind=max( (N.local[2] < 0 ) ? -N.local[2] : N.local[2] ,mind);
			N.local[0]=(float)(N.local[0]/mind)*512;
			N.local[1]=(float)(N.local[1]/mind)*512;
			N.local[2]=(float)(N.local[2]/mind)*512;
			N.local[0]=(float)((int)N.local[0]);
			N.local[1]=(float)((int)N.local[1]);
			N.local[2]=(float)((int)N.local[2]);
			float mag=(float)sqrt(N.local[0]*N.local[0]+N.local[1]*N.local[1]+N.local[2]*N.local[2]);
			if(mag!=0.0)
				N.local[0]/=mag, N.local[1]/=mag, N.local[2]/=mag;
			else
				N.local[0]=N.local[1]=N.local[2]=0.0;
            memcpy(&mesh->object[i].fnormal[j],&N,sizeof(_VERTEX));
        }
    }
	fclose(fp);
    return(mesh);
}

_MESH *LoadCollisionMeshASE(char *filename)
{
	FILE *fp;
	char hole[256];
    int i,j;

	if( (fp=fopen(filename,"rt"))==NULL)
	{
		Error(filename," 요파일이 없다!");
	}
    mesh=(_MESH *)(Dmalloc(sizeof(_MESH)));
    memset(mesh,0,sizeof(_MESH));
    mesh->objectnum=Col_object_num;
	if( Col_object_num )
		return mesh;
    mesh->object=(_OBJECT *)Dmalloc(sizeof(_OBJECT)*Col_object_num);
    memset(mesh->object,0,sizeof(_OBJECT)*Col_object_num);
	int now_geo_object_num=0;
    while( 1 )
	{
		if( fscanf(fp,"%s",&hole[0])==EOF)
			break;
        if (!strcmp(hole,"*COLLISIONOBJECT"))
		{
            mesh->object[now_geo_object_num].material_ref=-1;
            GetGEOMOBJECT(fp,&mesh->object[now_geo_object_num]);
            now_geo_object_num++;
		}
	}

//extern void GetLocalMatrix(_MESH *mesh);
//	GetLocalMatrix(mesh);	//애니에서 쓸 로컬 매트릭스를 구한다.

	//평면노멀과 맵핑 정리...
    for(i=0;i<mesh->objectnum;i++)
    {
        for(j=0;j<mesh->object[i].facenum;j++)
        {
            _VERTEX V[3],N;

            memcpy(&V[0],&mesh->object[i].vertex[mesh->object[i].face[j].v[0]],sizeof(_VERTEX));
            memcpy(&V[1],&mesh->object[i].vertex[mesh->object[i].face[j].v[1]],sizeof(_VERTEX));
            memcpy(&V[2],&mesh->object[i].vertex[mesh->object[i].face[j].v[2]],sizeof(_VERTEX));
            N.local[0]=V[0].local[1]*(V[1].local[2]-V[2].local[2])+V[1].local[1]*(V[2].local[2]-V[0].local[2])+V[2].local[1]*(V[0].local[2]-V[1].local[2]);
            N.local[1]=V[0].local[2]*(V[1].local[0]-V[2].local[0])+V[1].local[2]*(V[2].local[0]-V[0].local[0])+V[2].local[2]*(V[0].local[0]-V[1].local[0]);
            N.local[2]=V[0].local[0]*(V[1].local[1]-V[2].local[1])+V[1].local[0]*(V[2].local[1]-V[0].local[1])+V[2].local[0]*(V[0].local[1]-V[1].local[1]);
			double mind=-1.0;
			mind=max( (N.local[0] < 0 ) ? -N.local[0] : N.local[0] ,mind);
			mind=max( (N.local[1] < 0 ) ? -N.local[1] : N.local[1] ,mind);
			mind=max( (N.local[2] < 0 ) ? -N.local[2] : N.local[2] ,mind);
			N.local[0]=(float)(N.local[0]/mind)*512;
			N.local[1]=(float)(N.local[1]/mind)*512;
			N.local[2]=(float)(N.local[2]/mind)*512;
			N.local[0]=(float)((int)N.local[0]);
			N.local[1]=(float)((int)N.local[1]);
			N.local[2]=(float)((int)N.local[2]);
			float mag=(float)sqrt(N.local[0]*N.local[0]+N.local[1]*N.local[1]+N.local[2]*N.local[2]);
			if(mag!=0.0)
				N.local[0]/=mag, N.local[1]/=mag, N.local[2]/=mag;
			else
				N.local[0]=N.local[1]=N.local[2]=0.0;
            memcpy(&mesh->object[i].fnormal[j],&N,sizeof(_VERTEX));
        }
    }
	fclose(fp);
    return(mesh);
}


static _SCENE Scene;
_SCENE *LoadSceneASE(char *filename)
{
	FILE *fp;
	char hole[256];

	if( (fp=fopen(filename,"rt"))==NULL)
	{
		Error(filename," 요파일이 없다!");
	}
    while( 1 )
	{
		if( fscanf(fp,"%s",&hole[0])==EOF)
			break;
        if (!strcmp(hole,"*SCENE"))
		{
            GetSCENE(fp,&Scene);
			break;
		}
	}
	fclose(fp);
	return &Scene;
}
_MATERIAL *LoadMaterialASE(char *filename)
{
	_MATERIAL *mat=NULL;
	FILE *fp;
	char hole[256];

	if( (fp=fopen(filename,"rt"))==NULL)
	{
		Error(filename," 요파일이 없다!");
	}
//	mat=(_MATERIAL *)Dmalloc(sizeof(_MATERIAL));
//	memset(mat,0,sizeof(_MATERIAL));
    while( 1 )
	{
		if( fscanf(fp,"%s",&hole[0])==EOF)
			break;
        if (!strcmp(hole,"*MATERIAL_LIST"))
		{
            mat = GetMATERIAL_LIST(fp);
		}
	}
	fclose(fp);
	if( mat == NULL )
	{
		Warning("텍스쳐가 1개도 없습니다.","");
	}
	return mat;
}

void GetSCENE(FILE *fp,_SCENE *scene)
{
    char hole[256];

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;
        if (!strcmp(hole,"*SCENE_FIRSTFRAME"))
		{
	        fscanf(fp,"%s",&hole[0]);
			scene->StartFrame = atoi(hole);
		}
        if (!strcmp(hole,"*SCENE_LASTFRAME"))
		{
	        fscanf(fp,"%s",&hole[0]);
			scene->EndFrame = atoi(hole);
		}
        if (!strcmp(hole,"*SCENE_FRAMESPEED"))
		{
	        fscanf(fp,"%s",&hole[0]);
			scene->FrameSpeed = atoi(hole);
		}
        if (!strcmp(hole,"*SCENE_TICKSPERFRAME"))
		{
	        fscanf(fp,"%s",&hole[0]);
			scene->TickPerFrame = atoi(hole);
			NowTickFrame=(float)atoi(hole);
		}
	}
}

void GetCONTROL_POS_TRACK(FILE *fp,_POS_TRACK *im_pos,int *pos_cnt)
{
    char hole[256];
	int i;

	*pos_cnt=0;
    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;
        if (!strcmp(hole,"*CONTROL_POS_SAMPLE"))
		{
	        fscanf(fp,"%s",&hole[0]);		//프래임과 x,y,z 이동값을 차례로 읽는다.
			im_pos[*pos_cnt].frame = (float)atof(hole)/NowTickFrame;
	        fscanf(fp,"%s",&hole[0]);
			im_pos[*pos_cnt].x = (float)atof(hole);	
			fscanf(fp,"%s",&hole[0]);
			im_pos[*pos_cnt].y = (float)atof(hole);
			fscanf(fp,"%s",&hole[0]);
			im_pos[*pos_cnt].z = (float)atof(hole);
			(*pos_cnt)++;
			if( *pos_cnt >= 5256 )
				Error("pos 트랙이 너무 많은데..","");
		}
	}
	for(i=1;i<*pos_cnt;i++)	//이동이 다 똑같으면 결국 이동 애니메이션이 없다. 
	{
		if(im_pos[0].x != im_pos[i].x 
			|| im_pos[0].y != im_pos[i].y 
			|| im_pos[0].z != im_pos[i].z )
		{
			break;
		}
	}
	if( i == *pos_cnt )	//따라서 하나로 만들어 정리해준다.
		*pos_cnt=1;
}

void GetCONTROL_POS_TRACK(FILE *fp,_OBJECT *object)
{
	_POS_TRACK im_pos[5256];
	int pos_cnt=0;

	GetCONTROL_POS_TRACK(fp,im_pos,&pos_cnt);
	object->Pos = (_POS_TRACK *)Dmalloc(sizeof(_POS_TRACK)*pos_cnt);
	object->Pos_cnt=pos_cnt;
	memcpy(object->Pos,im_pos,sizeof(_POS_TRACK)*pos_cnt);
}

void GetCONTROL_POS_TRACK(FILE *fp,_HELPER *helper)
{
	_POS_TRACK im_pos[5256];
	int pos_cnt=0;

	GetCONTROL_POS_TRACK(fp,im_pos,&pos_cnt);
	helper->Pos = (_POS_TRACK *)Dmalloc(sizeof(_POS_TRACK)*pos_cnt);
	helper->Pos_cnt=pos_cnt;
	memcpy(helper->Pos,im_pos,sizeof(_POS_TRACK)*pos_cnt);
}
void GetCONTROL_POS_TRACK(FILE *fp,_CAMERA *camera)
{
	_POS_TRACK im_pos[5256];
	int pos_cnt=0;

	GetCONTROL_POS_TRACK(fp,im_pos,&pos_cnt);
	camera->Pos = (_POS_TRACK *)Dmalloc(sizeof(_POS_TRACK)*pos_cnt);
	camera->Pos_cnt=pos_cnt;
	memcpy(camera->Pos,im_pos,sizeof(_POS_TRACK)*pos_cnt);
}

void GetCONTROL_ROT_TRACK(FILE *fp,_ROT_TRACK *im_rot,int *rot_cnt)
{
    char hole[256];
	int i;

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;
        if (!strcmp(hole,"*CONTROL_ROT_SAMPLE"))
		{
	        fscanf(fp,"%s",&hole[0]);		//프래임과 axis,angular값을 차례로 읽는다.
			im_rot[*rot_cnt].frame = (float)atof(hole)/NowTickFrame;
	        fscanf(fp,"%s",&hole[0]);
			im_rot[*rot_cnt].x = (float)atof(hole);
	        fscanf(fp,"%s",&hole[0]);
			im_rot[*rot_cnt].y = (float)atof(hole);
	        fscanf(fp,"%s",&hole[0]);
			im_rot[*rot_cnt].z = (float)atof(hole);
	        fscanf(fp,"%s",&hole[0]);
			im_rot[*rot_cnt].w = (float)atof(hole);
			(*rot_cnt)++;
			if( *rot_cnt >= 5256 )
				Error("회전트랙이 너무 많은데..","");
		}
	}
//------------------ ase의 회전트랙은 axis,angular다 쿼터니온으로 바꾸자
//	float im_mat[4][4];
	if(rot_cnt==0)
		return;
	union{
	Vector3f va;
	D3DVECTOR a;
	};
	a.x=a.y=a.z=0;
	float aw=0;
	float qx,qy,qz,qw;

	for(i=0;i<*rot_cnt;i++)
	{
		a.x=im_rot[i].x;
		a.y=im_rot[i].y;
		a.z=im_rot[i].z;
		aw=im_rot[i].w;
//		MatrixFromRotateVector(Matrix,im_rot[i].w,im_rot[i].x,im_rot[i].y,im_rot[i].z);
//		QuaternionFromMatrix( im_rot[i].x,im_rot[i].y,im_rot[i].z
//			,im_rot[i].w,Matrix );

//		Normalize(va);
		D3DMath_QuaternionFromRotation(qx,qy,qz,qw,a,aw);
		im_rot[i].x = qx;
		im_rot[i].y = qy;
		im_rot[i].z = qz;
		im_rot[i].w = qw;
	}
/*
	qx=qy=qz=qw=0;
	float aqx,aqy,aqz,aqw;
	aqx=aqy=aqz=0;
	aqw=1;
	for(i=0;i<*rot_cnt;i++) //쿼터니온 누적
	{

		D3DMath_QuaternionMultiply( aqx,aqy, aqz, aqw
								   ,im_rot[i].x,im_rot[i].y,im_rot[i].z,im_rot[i].w
                                   ,aqx, aqy, aqz, aqw
								   );
		im_rot[i].x=aqx;
		im_rot[i].y=aqy;
		im_rot[i].z=aqz;
		im_rot[i].w=aqw;

		D3DMATRIX qmat;
		D3DMath_MatrixFromQuaternion(qmat,im_rot[i].x,im_rot[i].y,im_rot[i].z,im_rot[i].w);

		a.x=-0.8405;
		a.y=	0.0503;
		a.z=-0.5395;
		aw=2.8279;
		D3DMath_QuaternionFromRotation(qx,qy,qz,qw,a,aw);
		D3DMath_MatrixFromQuaternion(qmat,qx,qy,qz,qw);

	}
*/	
	for(i=1;i<*rot_cnt;i++)	//회전이 다 똑같으면 결국 회전 애니메이션이 없다. 
	{
		if(im_rot[0].x != im_rot[i].x 
			|| im_rot[0].y != im_rot[i].y 
			|| im_rot[0].z != im_rot[i].z 
			|| im_rot[0].w != im_rot[i].w )
		{
			break;
		}
	}
	if( i == *rot_cnt )	//따라서 하나로 만들어 정리해준다.
		*rot_cnt=1;
}
void GetCONTROL_ROT_TRACK(FILE *fp,_OBJECT *object)
{
	_ROT_TRACK im_rot[5256];
	int rot_cnt=0;

	GetCONTROL_ROT_TRACK(fp,im_rot,&rot_cnt);
	object->Rot = (_ROT_TRACK *)Dmalloc(sizeof(_ROT_TRACK)*rot_cnt);
	object->Rot_cnt=rot_cnt;
	memcpy(object->Rot,im_rot,sizeof(_ROT_TRACK)*rot_cnt);
}
void GetCONTROL_ROT_TRACK(FILE *fp,_HELPER *helper)
{
	_ROT_TRACK im_rot[5256];
	int rot_cnt=0;

	GetCONTROL_ROT_TRACK(fp,im_rot,&rot_cnt);
	helper->Rot = (_ROT_TRACK *)Dmalloc(sizeof(_ROT_TRACK)*rot_cnt);
	helper->Rot_cnt=rot_cnt;
	memcpy(helper->Rot,im_rot,sizeof(_ROT_TRACK)*rot_cnt);
}
void GetCONTROL_ROT_TRACK(FILE *fp,_CAMERA *camera)
{
	_ROT_TRACK im_rot[5256];
	int rot_cnt=0;

	GetCONTROL_ROT_TRACK(fp,im_rot,&rot_cnt);
	camera->Rot = (_ROT_TRACK *)Dmalloc(sizeof(_ROT_TRACK)*rot_cnt);
	camera->Rot_cnt=rot_cnt;
	memcpy(camera->Rot,im_rot,sizeof(_ROT_TRACK)*rot_cnt);
}

void GetCONTROL_SCALE_TRACK(FILE *fp,_OBJECT *object)
{
    char hole[256];
	_SCALE_TRACK im_scale[5256];
	int i;
	int scale_cnt=0;

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;
        if (!strcmp(hole,"*CONTROL_SCALE_SAMPLE"))
		{
	        fscanf(fp,"%s",&hole[0]);		//프래임과 x,y,z 스케일값을 차례로 읽는다.
			im_scale[scale_cnt].frame = (float)atof(hole)/NowTickFrame;
	        fscanf(fp,"%s",&hole[0]);
			im_scale[scale_cnt].x = (float)atof(hole);
	        fscanf(fp,"%s",&hole[0]);
			im_scale[scale_cnt].y = (float)atof(hole);
	        fscanf(fp,"%s",&hole[0]);
			im_scale[scale_cnt].z = (float)atof(hole);
	        fscanf(fp,"%s",&hole[0]);
			im_scale[scale_cnt].qx = (float)atof(hole);
	        fscanf(fp,"%s",&hole[0]);
			im_scale[scale_cnt].qy = (float)atof(hole);
	        fscanf(fp,"%s",&hole[0]);
			im_scale[scale_cnt].qz = (float)atof(hole);
	        fscanf(fp,"%s",&hole[0]);
			im_scale[scale_cnt].qw = (float)atof(hole);

			union{
			Vector3f va;
			D3DVECTOR a;
			};
			a.x=a.y=a.z=0;
			float aw=0;
			float qx,qy,qz,qw;

			a.x=im_scale[scale_cnt].qx;
			a.y=im_scale[scale_cnt].qy;
			a.z=im_scale[scale_cnt].qz;
			aw=im_scale[scale_cnt].qw;
			D3DMath_QuaternionFromRotation(qx,qy,qz,qw,a,aw);
			im_scale[scale_cnt].qx = qx;
			im_scale[scale_cnt].qy = qy;
			im_scale[scale_cnt].qz = qz;
			im_scale[scale_cnt].qw = qw;

			scale_cnt++;
		}
	}
	for(i=1;i<scale_cnt;i++)	//스케일이 다 똑같으면 결국 스케일애니메이션이 없다. 
	{
		if(im_scale[0].x != im_scale[i].x 
			|| im_scale[0].y != im_scale[i].y 
			|| im_scale[0].z != im_scale[i].z )
		{
			break;
		}
	}
	//	스케일애니는 안한다.
	if( i == scale_cnt )	//따라서 하나로 만들어 정리해준다.
		scale_cnt=1;
	object->Scale = (_SCALE_TRACK *)Dmalloc(sizeof(_SCALE_TRACK)*scale_cnt);
	object->Scale_cnt=scale_cnt;
	memcpy(object->Scale,im_scale,sizeof(_SCALE_TRACK)*scale_cnt);
}

void GetTM_ANIMATION(FILE *fp,_OBJECT *object)
{
    char hole[256];
    char old_hole[256];

	object->Pos_cnt=0;
	object->Rot_cnt=0;
	object->Rot=NULL;
	object->Pos=NULL;
	if( NowTickFrame == 0)
		Error("NowTickFrame is 0","");
    fscanf(fp,"%s",&hole[0]);
    while(1)
    {
		strcpy(old_hole,hole);
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;
        if (!strcmp(hole,"{"))
		{
//			Error(old_hole," <- can't surport it");
			Warning(old_hole," <- can't surport it");
			GetImsi(fp);
		}
        if (!strcmp(hole,"*CONTROL_POS_TRACK"))
		{
			GetCONTROL_POS_TRACK(fp,object);
		}
        if (!strcmp(hole,"*CONTROL_ROT_TRACK"))
		{
			GetCONTROL_ROT_TRACK(fp,object);
		}
        if (!strcmp(hole,"*CONTROL_SCALE_TRACK"))
		{
			GetCONTROL_SCALE_TRACK(fp,object);
		}
	}
}
void GetTM_ANIMATION(FILE *fp,_HELPER *dummy)
{
    char hole[256];
    char old_hole[256];

	dummy->Pos_cnt=0;
	dummy->Rot_cnt=0;
	dummy->Rot=NULL;
	dummy->Pos=NULL;
//	dummy->Scale_cnt=0;
//	dummy->Scale=NULL;
	if( NowTickFrame == 0)
		Error("NowTickFrame is 0","");
    fscanf(fp,"%s",&hole[0]);
    while(1)
    {
		strcpy(old_hole,hole);
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;
        if (!strcmp(hole,"{"))
		{
//			Error(old_hole," <- can't surport it");
			Warning(old_hole," <- can't surport it");
			GetImsi(fp);
		}
        if (!strcmp(hole,"*CONTROL_POS_TRACK"))
		{
			GetCONTROL_POS_TRACK(fp,dummy);
		}
        if (!strcmp(hole,"*CONTROL_ROT_TRACK"))
		{
			GetCONTROL_ROT_TRACK(fp,dummy);
		}
        if (!strcmp(hole,"*CONTROL_SCALE_TRACK"))
		{
//			GetCONTROL_SCALE_TRACK(fp,dummy);
		}
	}
}
void GetTM_ANIMATION(FILE *fp,_CAMERA *camera)
{
    char hole[256];
    char old_hole[256];

	camera->Pos_cnt=0;
	camera->Rot_cnt=0;
	camera->Rot=NULL;
	camera->Pos=NULL;
//	camera->Scale_cnt=0;
//	camera->Scale=NULL;
	if( NowTickFrame == 0)
		Error("NowTickFrame is 0","");
    fscanf(fp,"%s",&hole[0]);
    while(1)
    {
		strcpy(old_hole,hole);
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;
        if (!strcmp(hole,"{"))
		{
//			Error(old_hole," <- can't surport it");
			Warning(old_hole," <- can't surport it");
			GetImsi(fp);
		}
        if (!strcmp(hole,"*CONTROL_POS_TRACK"))
		{
			GetCONTROL_POS_TRACK(fp,camera);
		}
        if (!strcmp(hole,"*CONTROL_ROT_TRACK"))
		{
			GetCONTROL_ROT_TRACK(fp,camera);
		}
        if (!strcmp(hole,"*CONTROL_SCALE_TRACK"))
		{
//			GetCONTROL_SCALE_TRACK(fp,camera);
		}
	}
}

void OutQuatStack(FILE *fp)
{
	char hole[256];

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;		
        if (!strcmp(hole,"{"))
		{
			OutQuatStack(fp);
		}
	}
}

void GetGEOMOBJECT(FILE *fp,_OBJECT *object)
{
    char hole[256];

    fscanf(fp,"%s",&hole[0]);	// {
	object->parentname[0]=NULL;
    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;
        if (!strcmp(hole,"{"))
		{
			OutQuatStack(fp);
		}
        if (!strcmp(hole,"*NODE_NAME"))
		{
            GetName(fp,hole);
            strcpy(object->objectname,hole);
		}
        if (!strcmp(hole,"*NODE_TM"))
		{
            GetNODE_TM(fp,object);
		}
        if (!strcmp(hole,"*MESH"))
		{
            GetMESH(fp,object);
		}
        if (!strcmp(hole,"*NODE_PARENT"))
		{
            GetName(fp,hole);
			strcpy(object->parentname,hole);
		}
        if (!strcmp(hole,"*TM_ANIMATION"))
		{
            GetTM_ANIMATION(fp,object);
		}
        if (!strcmp(hole,"*MATERIAL_REF"))
		{
            fscanf(fp,"%s",&hole[0]);
            object->material_ref=atoi(hole);
		}
        if (!strcmp(hole,"*PROP_CASTSHADOW"))
		{
            fscanf(fp,"%s",&hole[0]);
			if( !strcmp(hole,"0" ))
				object->type |=_SHADOW_CAST_OFF;
		}
        if (!strcmp(hole,"*PROP_RECVSHADOW"))
		{
            fscanf(fp,"%s",&hole[0]);
			if( !strcmp(hole,"0" ))
				object->type |=_SHADOW_RECEIVE_OFF;
		}
        if (!strcmp(hole,"*MESH_ANIMATION"))
		{
            fscanf(fp,"%s",&hole[0]);	// {
			SkipNowGroup(fp);
		}		
    }
}

void GetMESH_MAPPINGCHANNEL_MESH_TVERT(FILE *fp,_OBJECT *object)
{
	char hole[256];
    int num;

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;
        if (!strcmp(hole,"*MESH_TVERT"))
		{
            fscanf(fp,"%s",&hole[0]);
            num=atoi(hole);
            fscanf(fp,"%s",&hole[0]);
            object->tvertex_ch2[num].uv[0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            object->tvertex_ch2[num].uv[1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
        }
    }
}
void GetMESH_MAPPINGCHANNEL(FILE *fp,_OBJECT *object)
{
	char hole[256];
//    int num;

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;
        if (!strcmp(hole,"{"))
		{
			SkipNowGroup(fp);
		}
        if (!strcmp(hole,"*MESH_NUMTVERTEX"))
		{
            fscanf(fp,"%s",&hole[0]);
            object->tvertexnum_ch2=atoi(hole);
/*
			if( temp != object->tvertexnum_ch2 )
			{
				Warning("채널매핑개수가 오브젝트 매핑갯수와 일치하지 않습니다.(장언일문의)","");
				SkipNowGroup(fp);
				break;
			}*/
			object->tvertex_ch2 = (_TVERTEX *)Dmalloc(object->tvertexnum_ch2*(sizeof(_TVERTEX)));
		}
        if (!strcmp(hole,"*MESH_TFACELIST"))
		{
            GetMESH_TVFACELIST2(fp,object);
		}
        if (!strcmp(hole,"*MESH_TVERTLIST"))
		{
			GetMESH_MAPPINGCHANNEL_MESH_TVERT(fp,object);
		}
    }
}

void GetMESH(FILE *fp,_OBJECT *object)
{
	char hole[256];

    fscanf(fp,"%s",&hole[0]);	// {
	BOOL mesh_ani=FALSE;
    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
		{
			break;
		}
        if (!strcmp(hole,"{"))	//불필요한 {생길경우...
		{
			OutQuatStack(fp);
		}
        if (!strcmp(hole,"*MESH_NUMVERTEX"))
		{
            fscanf(fp,"%s",&hole[0]);
            object->vertexnum=atoi(hole);
            object->vertex=(_VERTEX *)Dmalloc(sizeof(_VERTEX)*object->vertexnum);
            memset(object->vertex,0,sizeof(_VERTEX)*object->vertexnum);
			if(object->vertexnum >= _MAX_LIMIT_NUM)
				Error(object->objectname,"<-이 오브젝트가 vertex 20000 이넘습니다.");
//            object->vnormal=(_VERTEX *)Dmalloc(sizeof(_VERTEX)*object->vertexnum);
  //          memset(object->vnormal,0,sizeof(_VERTEX)*object->vertexnum);
        }
        if (!strcmp(hole,"*MESH_NUMFACES"))
		{
            fscanf(fp,"%s",&hole[0]);
            object->facenum=atoi(hole);
			if(object->facenum >= _MAX_LIMIT_NUM)
				Error(object->objectname,"이 오브젝트가 face 20000 이상입니다.");
            object->face=(_FACE *)Dmalloc(sizeof(_FACE)*object->facenum);
            memset(object->face,0,sizeof(_FACE)*object->facenum);
            object->fnormal=(Vector3f *)Dmalloc(sizeof(Vector3f)*object->facenum);
  //          memset(object->fnormal,0,sizeof(_VERTEX)*object->facenum);
		}
        if (!strcmp(hole,"*MESH_VERTEX_LIST"))
		{
            GetMESH_VERTEX_LIST(fp,object);
		}
        if (!strcmp(hole,"*MESH_FACE_LIST"))
		{
            GetMESH_FACE_LIST(fp,object);
		}
        if (!strcmp(hole,"*MESH_NUMTVERTEX"))
		{
            fscanf(fp,"%s",&hole[0]);
            object->tvertexnum=atoi(hole);
//			if(object->tvertexnum >= _MAX_LIMIT_NUM)
//				Error("하나의 오브젝트에 텍스쳐 버텍스가 20000개 이상이 있으면 안됩니다.","");
            object->tvertex=(_TVERTEX *)Dmalloc(sizeof(_TVERTEX)*object->tvertexnum);
            memset(object->tvertex,0,sizeof(_TVERTEX)*object->tvertexnum);
		}
        if (!strcmp(hole,"*MESH_TVERTLIST"))
		{
            GetMESH_TVERTLIST(fp,object);
		}
        if (!strcmp(hole,"*MESH_NUMTVFACES"))
		{
            fscanf(fp,"%s",&hole[0]);
            object->tvfacenum=atoi(hole);
//			if(object->tvfacenum >= _MAX_LIMIT_NUM)
//				Error("하나의 오브젝트에 텍스쳐 페이스가 20000개 이상이 있으면 안됩니다.","");
            if( object->tvfacenum != 0 && object->tvfacenum!=object->facenum)
            {
                Error("not match tvfacenum!","tv 점이 틀리다!");
            }
//            object->tvface=(_TVFACE *)(Dmalloc(sizeof(_TVFACE)*object->tvfacenum));
		}
        if (!strcmp(hole,"*MESH_TFACELIST"))
		{
            GetMESH_TVFACELIST(fp,object);
		}
        if (!strcmp(hole,"*MESH_NORMALS"))
		{
            GetMESH_NORMALS(fp,object);
		}
        if (!strcmp(hole,"*MESH_MAPPINGCHANNEL"))
		{
            fscanf(fp,"%s",&hole[0]);
            int temp=atoi(hole);
            fscanf(fp,"%s",&hole[0]);
//			if( temp !=2 )
//			{
//				Warning("매핑채널은 인공 라이트맵 생성시 사용하십시요.","");
//				SkipNowGroup(fp);
//			}
			GetMESH_MAPPINGCHANNEL(fp,object);
		}
    }
}

static void FixVertexEpsilon( Vector3f v)	//소수점 짜잘한것은 버린다. 현재 둘째자리 
{
	int i;

#define _FIX_VERTEX_EPSILON		0.005
	for(i=0 ;i<3; i++)
	{
//		v[i] = ((int)((v[i]+_FIX_VERTEX_EPSILON)*100))/100.f;
	}
}

void GetMESH_VERTEX_LIST(FILE *fp,_OBJECT *object)
{
	char hole[256];
    int num,i;
	float x,y,z;

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;
        if (!strcmp(hole,"*MESH_VERTEX"))
		{
            fscanf(fp,"%s",&hole[0]);
            num=atoi(hole);
            fscanf(fp,"%s",&hole[0]);
            object->vertex[num].local[0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            object->vertex[num].local[_READ_Y]=(float)(atof(hole));	//좌표계를 맞춘다.
            fscanf(fp,"%s",&hole[0]);
            object->vertex[num].local[_READ_Z]=(float)atof(hole);
        }
    }
/*
	float revmat[12];
    float scmat[12];
    double a;

    scmat[0] = object->trmatrix[0][0];
    scmat[1] = object->trmatrix[0][1];
    scmat[2] = object->trmatrix[0][2];
    scmat[3] = object->trmatrix[1][0];
    scmat[4] = object->trmatrix[1][1];
    scmat[5] = object->trmatrix[1][2];
    scmat[6] = object->trmatrix[2][0];
    scmat[7] = object->trmatrix[2][1];
    scmat[8] = object->trmatrix[2][2];

    a=(double)(scmat[0]*(scmat[4]*scmat[8]-scmat[7]*scmat[5])
        -scmat[1]*(scmat[3]*scmat[8]-scmat[6]*scmat[5])
        +scmat[2]*(scmat[3]*scmat[7]-scmat[6]*scmat[4]));
    a=1.0/a;
    revmat[0]=(float)((double)(scmat[4]*scmat[8]-scmat[7]*scmat[5])*a);
    revmat[1]=-(float)((double)(scmat[3]*scmat[8]-scmat[5]*scmat[6])*a);
    revmat[2]=(float)((double)(scmat[3]*scmat[7]-scmat[6]*scmat[4])*a);
    revmat[3]=-(float)((double)(scmat[8]*scmat[1]-scmat[2]*scmat[7])*a);
    revmat[4]=(float)((double)(scmat[0]*scmat[8]-scmat[2]*scmat[6])*a);
    revmat[5]=-(float)((double)(scmat[0]*scmat[7]-scmat[6]*scmat[1])*a);
    revmat[6]=(float)((double)(scmat[5]*scmat[1]-scmat[4]*scmat[2])*a);
    revmat[7]=-(float)((double)(scmat[0]*scmat[5]-scmat[3]*scmat[2])*a);
    revmat[8]=(float)((double)(scmat[0]*scmat[4]-scmat[3]*scmat[1])*a);

    object->revmatrix[0][0] = revmat[0];
    object->revmatrix[0][1] = revmat[1];
    object->revmatrix[0][2] = revmat[2];
    object->revmatrix[1][0] = revmat[3];
    object->revmatrix[1][1] = revmat[4];
    object->revmatrix[1][2] = revmat[5];
    object->revmatrix[2][0] = revmat[6];
    object->revmatrix[2][1] = revmat[7];
    object->revmatrix[2][2] = revmat[8];
    object->revmatrix[3][0] = -(object->trmatrix[3][0]*revmat[0]+object->trmatrix[3][1]*revmat[3]+object->trmatrix[3][2]*revmat[6]);
    object->revmatrix[3][1] = -(object->trmatrix[3][0]*revmat[1]+object->trmatrix[3][1]*revmat[4]+object->trmatrix[3][2]*revmat[7]);
    object->revmatrix[3][2] = -(object->trmatrix[3][0]*revmat[2]+object->trmatrix[3][1]*revmat[5]+object->trmatrix[3][2]*revmat[8]);
    object->revmatrix[3][3] = 1.0;
    object->revmatrix[0][3] = 0;
    object->revmatrix[1][3] = 0;
    object->revmatrix[2][3] = 0;
    for(i=0;i< object->vertexnum;i++)
    {
        x=object->vertex[i].local[0]-object->trmatrix[3][0];
        y=object->vertex[i].local[1]-object->trmatrix[3][1];
        z=object->vertex[i].local[2]-object->trmatrix[3][2];
        object->vertex[i].local[0]=(x*revmat[0] + y*revmat[1] + z*revmat[2]);
        object->vertex[i].local[1]=(x*revmat[3] + y*revmat[4] + z*revmat[5]);
        object->vertex[i].local[2]=(x*revmat[6] + y*revmat[7] + z*revmat[8]);
    }
*/
	union {
	float fmat[4][4];
	D3DMATRIX mat;
	};

    D3DMath_MatrixInvert(mat, object->d3d_trmatrix);
	memcpy(object->revmatrix,fmat,sizeof(D3DMATRIX));
    for(i=0;i< object->vertexnum;i++)
    {                                                       
        x = object->vertex[i].local[0] * mat._11 +
            object->vertex[i].local[1] * mat._21 +
            object->vertex[i].local[2] * mat._31 + mat._41;
        y = object->vertex[i].local[0] * mat._12 +
            object->vertex[i].local[1] * mat._22 +
            object->vertex[i].local[2] * mat._32 + mat._42;
        z = object->vertex[i].local[0] * mat._13 +
            object->vertex[i].local[1] * mat._23 +
            object->vertex[i].local[2] * mat._33 + mat._43;

        object->vertex[i].local[0] = x;
        object->vertex[i].local[1] = y;
        object->vertex[i].local[2] = z;
		FixVertexEpsilon( object->vertex[i].local);
    }
}


void GetMESH_FACE_LIST(FILE *fp,_OBJECT *object)
{
	char hole[256];
	char temp[256];
    int i,num;

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;
REPARSE:
        if (!strcmp(hole,"*MESH_FACE"))
		{
            fscanf(fp,"%s",&hole[0]);
            for(i=0;i<256;i++)
            {
                if(hole[i]==':')
                    break;
            }
            hole[i]=NULL;
            num=atoi(hole);

            fscanf(fp,"%s",&hole[0]);
            fscanf(fp,"%s",&hole[0]);
            object->face[num].v[0]=atoi(hole);
            fscanf(fp,"%s",&hole[0]);
            fscanf(fp,"%s",&hole[0]);
            object->face[num].v[_READ_Y]=atoi(hole);
            fscanf(fp,"%s",&hole[0]);
            fscanf(fp,"%s",&hole[0]);
            object->face[num].v[_READ_Z]=atoi(hole);
			object->face[num].smooth_group=0;
            //object->face[num].color[0]=rand()%32768;
        }
        if (!strcmp(hole,"*MESH_MTLID"))
        {
            fscanf(fp,"%s",&hole[0]);
            object->face[num].mtlid=atoi(hole);
        }
        if (!strcmp(hole,"*MESH_SMOOTHING"))
        {
            fscanf(fp,"%s",&hole[0]);
	        if (!strcmp(hole,"*MESH_MTLID"))
				goto REPARSE;
			int cnt=0;
			for(int i=0; i<(int )strlen(hole)+1; i++)
			{
				temp[cnt] = hole[i];
				if(temp[cnt]==','||temp[cnt]==NULL)
				{
					temp[cnt]=NULL;
					int bit = atoi(temp)-1;
					if( bit != -1)
						object->face[num].smooth_group |=(1<<bit);
					cnt=-1;
				}
				cnt++;
			}
        }
    }
}

void GetMESH_TVERTLIST(FILE *fp,_OBJECT *object)
{
	char hole[256];
    int num;

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;
        if (!strcmp(hole,"*MESH_TVERT"))
		{
            fscanf(fp,"%s",&hole[0]);
            num=atoi(hole);
            fscanf(fp,"%s",&hole[0]);
            object->tvertex[num].uv[0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            object->tvertex[num].uv[1]=1.0f-(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
        }
    }
}

void GetMESH_TVFACELIST(FILE *fp,_OBJECT *object)
{
	char hole[256];
    int num;

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;
        if (!strcmp(hole,"*MESH_TFACE"))
		{
            fscanf(fp,"%s",&hole[0]);
            num=atoi(hole);
            fscanf(fp,"%s",&hole[0]);
            object->face[num].tv[0]=atoi(hole);
            fscanf(fp,"%s",&hole[0]);
            object->face[num].tv[_READ_Y]=atoi(hole);
            fscanf(fp,"%s",&hole[0]);
            object->face[num].tv[_READ_Z]=atoi(hole);
        }
    }
}
void GetMESH_TVFACELIST2(FILE *fp,_OBJECT *object)
{
	char hole[256];
    int num;

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;
        if (!strcmp(hole,"*MESH_TFACE"))
		{
            fscanf(fp,"%s",&hole[0]);
            num=atoi(hole);
            fscanf(fp,"%s",&hole[0]);
            object->face[num].tv2[0]=atoi(hole);
            fscanf(fp,"%s",&hole[0]);
            object->face[num].tv2[_READ_Y]=atoi(hole);
            fscanf(fp,"%s",&hole[0]);
            object->face[num].tv2[_READ_Z]=atoi(hole);
        }
    }
}

void GetMESH_NORMALS(FILE *fp,_OBJECT *object)
{
	char hole[256];
//    int num;

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;
        if (!strcmp(hole,"*MESH_FACENORMAL"))
		{
/*
            fscanf(fp,"%s",&hole[0]);
            num=atoi(hole);
            fscanf(fp,"%s",&hole[0]);
            object->fnormal[num][0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            object->fnormal[num][1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            object->fnormal[num][2]=(float)atof(hole);
*/
        }
        if (!strcmp(hole,"*MESH_VERTEXNORMAL"))
		{
/*
            fscanf(fp,"%s",&hole[0]);
            num=atoi(hole);
            fscanf(fp,"%s",&hole[0]);
            object->vnormal[num][0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            object->vnormal[num][1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            object->vnormal[num][2]=(float)atof(hole);
*/
        }
    }
}

void GetMatrixFrom3DSMAXMatrix( float matrix[4][4])
{
/*
	for(int k=0;k<4;k++)
	{
		float imsi=matrix[k][1];
//		matrix[k][0]=-matrix[k][0];	//x도 -를
		matrix[k][1]=-matrix[k][2];
//		matrix[k][1]=matrix[k][2];
		matrix[k][2]=imsi;
	}
*/

	for(int k=0;k<4;k++)
	{
		float imsi=matrix[k][1];
		matrix[k][0]=matrix[k][0];	//
		matrix[k][1]=matrix[k][2];
//		matrix[k][1]=matrix[k][2];
		matrix[k][2]=imsi;
	}
}

void GetNODE_TM(FILE *fp,_OBJECT *object)
{
	char hole[256];

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;
        if (!strcmp(hole,"*TM_ROW0"))
		{
            fscanf(fp,"%s",&hole[0]);
            object->trmatrix[0][0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            object->trmatrix[0][1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            object->trmatrix[0][2]=(float)atof(hole);
            object->trmatrix[0][3]=0.0;
        }
        if (!strcmp(hole,"*TM_ROW1"))
		{
            fscanf(fp,"%s",&hole[0]);
            object->trmatrix[1][0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            object->trmatrix[1][1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            object->trmatrix[1][2]=(float)atof(hole);
            object->trmatrix[1][3]=0.0f;
        }
        if (!strcmp(hole,"*TM_ROW2"))
		{
            fscanf(fp,"%s",&hole[0]);
            object->trmatrix[2][0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            object->trmatrix[2][1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            object->trmatrix[2][2]=(float)atof(hole);
            object->trmatrix[2][3]=0.0;
        }
        if (!strcmp(hole,"*TM_ROW3"))
		{
            fscanf(fp,"%s",&hole[0]);
            object->trmatrix[3][0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            object->trmatrix[3][1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            object->trmatrix[3][2]=(float)atof(hole);
            object->trmatrix[3][3]=1.0;
        }
        if (!strcmp(hole,"*TM_SCALE"))	//스케일
        {
            fscanf(fp,"%s",&hole[0]);
            object->scale[0]=(float)atof(hole);	//로컬 스케일값.
            fscanf(fp,"%s",&hole[0]);
            object->scale[1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            object->scale[2]=(float)atof(hole);
        }
        if (!strcmp(hole,"*TM_SCALEAXIS"))	//스케일
        {
            fscanf(fp,"%s",&hole[0]);
            object->scale_quat[0]=(float)atof(hole);	//로컬 스케일값.
            fscanf(fp,"%s",&hole[0]);
            object->scale_quat[1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            object->scale_quat[2]=(float)atof(hole);
        }
        if (!strcmp(hole,"*TM_SCALEAXISANG"))	//스케일
        {
            fscanf(fp,"%s",&hole[0]);
            object->scale_quat[3]=(float)atof(hole);	//로컬 스케일값.
			
			//스케일 
			D3DVECTOR a={object->scale_quat[0],object->scale_quat[1],object->scale_quat[2]};
			D3DMath_QuaternionFromRotation(object->scale_quat[0],object->scale_quat[1]
				,object->scale_quat[2],object->scale_quat[3], a, object->scale_quat[3]);
        }		 
        if (!strcmp(hole,"*TM_POS"))	//포지션 
        {
            fscanf(fp,"%s",&hole[0]);
            object->pos[0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            object->pos[1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            object->pos[2]=(float)atof(hole);
       }
        if (!strcmp(hole,"*TM_ROTAXIS"))	//axis 
        {
            fscanf(fp,"%s",&hole[0]);
            object->quat[0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);	
            object->quat[1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            object->quat[2]=(float)atof(hole);
        }
        if (!strcmp(hole,"*TM_ROTANGLE"))	//angle 
        {
            fscanf(fp,"%s",&hole[0]);
            object->quat[3]=(float)atof(hole);
			//-------- 쿼터니온으로 바꾸자.
			//MatrixFromRotateVector(Matrix,object->qw,object->qx,object->qy,object->qz);
			//QuaternionFromMatrix( object->qx,object->qy,object->qz
			//	,object->qw,Matrix );
			D3DVECTOR a={object->quat[0],object->quat[1],object->quat[2]};
			D3DMath_QuaternionFromRotation(object->quat[0],object->quat[1],object->quat[2],object->quat[3],a,object->quat[3]);
	    }
    }
//	GetMatrixFrom3DSMAXMatrix(object->trmatrix);
}
void MatrixFromQuaternion( D3DMATRIX *mat, FLOAT x, FLOAT y, FLOAT z,
                                   FLOAT w )
{
    FLOAT xx = x*x; FLOAT yy = y*y; FLOAT zz = z*z;
    FLOAT xy = x*y; FLOAT xz = x*z; FLOAT yz = y*z;
    FLOAT wx = w*x; FLOAT wy = w*y; FLOAT wz = w*z;

    mat->_11 = 1 - 2 * ( yy + zz );
    mat->_12 =     2 * ( xy - wz );
    mat->_13 =     2 * ( xz + wy );

    mat->_21 =     2 * ( xy + wz );
    mat->_22 = 1 - 2 * ( xx + zz );
    mat->_23 =     2 * ( yz - wx );

    mat->_31 =     2 * ( xz - wy );
    mat->_32 =     2 * ( yz + wx );
    mat->_33 = 1 - 2 * ( xx + yy );

    mat->_14 = mat->_24 = mat->_34 = 0.0f;
    mat->_41 = mat->_42 = mat->_43 = 0.0f;
    mat->_44 = 1.0f;
    
}

//---머터리얼은 틀려도 텍스쳐는 공유할수 있게...
int Texture_num=0;
char *Tex_name[1024];	//최대 텍스쳐는 1024개..
int Oppa_num=0;
char *Oppa_name[1024];
_TEXTURE *R_Tex[1024];
_TEXTURE *R_Oppa[1024];

_TEXTURE *GetTexturePtr(char *name)
{
	int i;

	if(name[0]==NULL )
		return NULL;
	for(i=0; i<Texture_num; i++)
	{
		if( strcmp(name,Tex_name[i])==0 )
			return R_Tex[i];		
	}
	_TEXTURE *tex = (_TEXTURE*)Dmalloc(sizeof(_TEXTURE));
	memset(tex,0,sizeof(_TEXTURE));

	Tex_name[Texture_num]=(char *)Dmalloc(256);
	strcpy(Tex_name[Texture_num],name);
	R_Tex[Texture_num]=tex;
	Texture_num++;
	if( Texture_num>= 1024 )
		Error("이거 텍스쳐 갯수가 1024개를 넘겼는데... 이렇게 많게 작업함 안되요 ㅡㅡ","");
	return tex;
}
_TEXTURE *GetOppaPtr(char *name)
{
	int i;

	for(i=0; i<Oppa_num; i++)
	{
		if( strcmp(name,Oppa_name[i])==0 )
			return R_Oppa[i];		
	}
	_TEXTURE *oppa = (_TEXTURE*)Dmalloc(sizeof(_TEXTURE));
	memset(oppa,0,sizeof(_TEXTURE));

	Oppa_name[Oppa_num]=(char *)Dmalloc(256);
	strcpy(Oppa_name[Oppa_num],name);
	R_Oppa[Oppa_num]=oppa;
	Oppa_num++;
	return oppa;
}



void GetMAP_DIFFUSE(FILE *fp,_MATERIAL *material)
{
    char hole[256];

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;
        if (!strcmp(hole,"*BITMAP"))
        {
			GetNameBmp(fp,&hole[0]);
			
			_TEXTURE *tex=GetTexturePtr(hole);
            if( tex!=NULL )
			{
				strcpy(tex->mapname,&hole[0]);
				if( tex->mapname[0]!=NULL )
				{
					char *full_path=GetFullTexPath(tex->mapname);
					if( IsExistFile(full_path) != NULL )
						material->tex = tex;
				}
				if(Now_bmp_load==TRUE)
				{
					char *full_path=GetFullTexPath(tex->mapname);
					if( IsExistFile(full_path) != NULL )
					{
						tex->buffer=LoadBmp(full_path,&tex->mapxl,&tex->mapyl);
						tex->mapxl++;
						tex->mapyl++;
						CheckBmpExp(tex);
					}
				}
			}
            else
			{
                material->tex=NULL;
                material->oppa=NULL;
			}
        }
    }
}
void GetMAP_AMBIENT(FILE *fp,_MATERIAL *material)
{
    char hole[256];

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;
    }
}
void GetMAP_OPACITY(FILE *fp,_MATERIAL *material)
{
    char hole[256];

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"*MAP_AMOUNT"))
        {/*
			float num;

	        fscanf(fp,"%s",&hole[0]);
			material->attr|=_NORMAL_ALPHA;
			num=atof(hole);
			material->attr|=(int)(num*255);
			*/
		}
        if (!strcmp(hole,"*BITMAP"))
        {
			material->attr|=_MAT_NORMAL_ALPHA;

			GetNameBmp(fp,&hole[0]);
			_TEXTURE *oppa=GetOppaPtr(hole);
            if( oppa != NULL )
			{
	            strcpy(oppa->mapname,&hole[0]);
				char *full_path=GetFullTexPath(oppa->mapname);
				if( IsExistFile(full_path) != NULL )
				{
					material->oppa = oppa;
	                oppa->buffer=LoadBmp(full_path,&oppa->mapxl,&oppa->mapyl);
					oppa->mapxl++;
					oppa->mapyl++;
					CheckBmpExp(oppa);
				}
			}
            else
			{
                material->oppa=NULL;
			}
		}
        if (!strcmp(hole,"}"))
			break;
    }
}
void GetMAP_SHINE(FILE *fp,_MATERIAL *material)
{
    char hole[256];

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"*MAP_AMOUNT"))
        {
//			float num;
//	        fscanf(fp,"%s",&hole[0]);
//			material->attr|=_PLUS_ALPHA;
//			num=atof(hole);
//			material->attr|=(int)(num*255);
		}
        if (!strcmp(hole,"}"))
			break;
    }
}
void GetMAP_FILTERCOLOR(FILE *fp,_MATERIAL *material)
{
    char hole[256];

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"*MAP_AMOUNT"))
        {
//			float num;

	        fscanf(fp,"%s",&hole[0]);
//			material->attr|=_TRANSPARENT;
//			num=atof(hole);
//			material->attr|=(int)(num*255);
		}
        if (!strcmp(hole,"}"))
			break;
    }
}
void GetMAP_MAP_REFRACT(FILE *fp,_MATERIAL *material)
{
    char hole[256];

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"*MAP_AMOUNT"))
        {
//			float num;

	        fscanf(fp,"%s",&hole[0]);
//			material->attr|=_WATER;
//			num=atof(hole);
		}
        if (!strcmp(hole,"}"))
			break;
    }
}

void GetMAP_MAP_REFLECT(FILE *fp,_MATERIAL *material)
{
    char hole[256];

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"*MAP_AMOUNT"))
        {
	        fscanf(fp,"%s",&hole[0]);
			material->attr|=_MAT_REFLECT;
			material->reflect=(float)atof(hole);
		}
        if (!strcmp(hole,"}"))
			break;
    }
}
 
void GetMATERIAL(FILE *fp,_MATERIAL *material)
{
	char hole[256];
    int num;

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;
        if (!strcmp(hole,"*MATERIAL_NAME"))
		{
            GetName(fp,hole);
			strcpy(material->name,hole);
        }
        if (!strcmp(hole,"*MATERIAL_CLASS"))
		{
            GetName(fp,hole);
			strlwr(hole);
			if(strcmp("multi/sub-object",hole))	//멀티서브가 아니면...
			{
				if(!strcmp("standard",hole))	//스탠다드 .
				{
					material->type = _MAT_TYPE_STANDARD;
				}
				else
				if(!strcmp("composite",hole)  || !strcmp("shell%material",hole) )	//멀티서브가 아니면...
				{
					material->type = _MAT_TYPE_COMPOSITE;
				}
				else
				{
					material->type = _MAT_TYPE_UNKNOWN;
					Warning(material->name,"<-이 머터리얼은 지원하지 않는 타입입니다.");
					SkipNowGroup(fp);	//{}괄호 날리자.
				}
			}
			else
				material->type = _MAT_TYPE_MUTISUBOBJECT;
        }
        if (!strcmp(hole,"*MATERIAL_AMBIENT"))
		{/*
            fscanf(fp,"%s",&hole[0]);
            material->ambient[0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            material->ambient[1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            material->ambient[2]=(float)atof(hole);
			*/
        }
        if (!strcmp(hole,"*MATERIAL_DIFFUSE"))
		{/*
            fscanf(fp,"%s",&hole[0]);
            material->diffuse[0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            material->diffuse[1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            material->diffuse[2]=(float)atof(hole);
			*/
        }
        if (!strcmp(hole,"*MATERIAL_SPECULAR"))
		{
			/*
            fscanf(fp,"%s",&hole[0]);
            material->specular[0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            material->specular[1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            material->specular[2]=(float)atof(hole);
			*/
        }
        if (!strcmp(hole,"*MATERIAL_SHINE"))
		{/*
            fscanf(fp,"%s",&hole[0]);
            material->shine=(float)atof(hole);
			*/
        }
        if (!strcmp(hole,"*MATERIAL_SHINESTRENGTH"))
		{/*
            fscanf(fp,"%s",&hole[0]);
            material->shine_st=(float)atof(hole);
			*/
        }
        if (!strcmp(hole,"*MATERIAL_TRANSPARENCY"))
        {
            fscanf(fp,"%s",&hole[0]);
            material->transparency = 1.f-(float)atof(hole);
		}
        if (!strcmp(hole,"*MAP_DIFFUSE"))
        {
            GetMAP_DIFFUSE(fp,material);
        }
        if (!strcmp(hole,"*MAP_AMBIENT"))
        {
            GetMAP_AMBIENT(fp,material);
        }
        if (!strcmp(hole,"*MATERIAL_SELFILLUM"))
        {
            fscanf(fp,"%s",&hole[0]);
			material->self_illum = (float)atof(hole);
			if( material->self_illum == 0.0f)
				Now_bmp_load=FALSE;
			else
				Now_bmp_load=TRUE;
        }
        if (!strcmp(hole,"*NUMSUBMTLS"))
		{
            fscanf(fp,"%s",&hole[0]);
            material->sbnum=atoi(hole);
            material->submaterial=(_MATERIAL *)Dmalloc(sizeof(_MATERIAL)*material->sbnum);
            memset(material->submaterial,0,sizeof(_MATERIAL)*material->sbnum);
        }
        if (!strcmp(hole,"*SUBMATERIAL"))
		{
            fscanf(fp,"%s",&hole[0]);
            num=atoi(hole);
            GetMATERIAL(fp,&material->submaterial[num]);
        }
//-----------------------
        if (!strcmp(hole,"*MAP_SHINE"))
		{
            GetMAP_SHINE(fp,material);
		}
        if (!strcmp(hole,"*MAP_OPACITY"))
        {
            GetMAP_OPACITY(fp,material);
        }
        if (!strcmp(hole,"*MAP_FILTERCOLOR"))
        {
            GetMAP_FILTERCOLOR(fp,material);
        }
        if (!strcmp(hole,"*MAP_REFRACT"))
        {
            GetMAP_MAP_REFRACT(fp,material);
        }
        if (!strcmp(hole,"*MAP_REFLECT"))
        {
            GetMAP_MAP_REFLECT(fp,material);
        }
        if (!strcmp(hole,"*MAP_SPECULAR"))
		{
			sprintf(hole,"%4d submat",num);
			Error(hole,"*MAP_SPECULAR exist!");
		}
        if (!strcmp(hole,"*MAP_SHINESTRENGTH"))
		{
			sprintf(hole,"%4d submat",num);
			Error(hole,"*MAP_SHINESTRENGTH exist!");
		}
        if (!strcmp(hole,"*MAP_SELFILLUM"))
		{
			sprintf(hole,"%4d submat",num);
			Error(hole,"*MAP_SELFILLUM exist!");
		}
/*
        if (!strcmp(hole,""))
		{
			sprint(hole,"%4d submat",num);
			error(hole," exist!");
		}
        if (!strcmp(hole,""))
		{
			sprint(hole,"%4d submat",num);
			error(hole," exist!");
		}
        if (!strcmp(hole,""))
		{
			sprint(hole,"%4d submat",num);
			error(hole," exist!");
		}
        if (!strcmp(hole,""))
		{
			sprint(hole,"%4d submat",num);
			error(hole," exist!");
		}
		*/
    }
}
_MATERIAL *GetMATERIAL_LIST(FILE *fp)
{
	char hole[256];
    int num;
	_MATERIAL *material=NULL;

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;
        if (!strcmp(hole,"*MATERIAL_COUNT"))
		{
            fscanf(fp,"%s",&hole[0]);
            int all_num=atoi(hole);
			if( all_num > 0)
			{
				material=(_MATERIAL *)Dmalloc(sizeof(_MATERIAL)*all_num);
				memset(material,0,sizeof(_MATERIAL)*all_num);
				material->all_num=all_num;
			}
        }

        if (!strcmp(hole,"*MATERIAL"))
		{
            fscanf(fp,"%s",&hole[0]);
            num=atoi(hole);
            GetMATERIAL(fp,&material[num]);
        }
    }
	return material;
}

//---------------------------------------------------------------------------//


//------------------------------ 라이트 구조...
static void GetLIGHTOBJECT(FILE *fp,_LIGHTSOURCE *lgt);
static void GetLIGHT_MAIN_TM(FILE *fp,_LIGHTSOURCE *lgt);
static void GetLIGHT_TARGET_TM(FILE *fp,_LIGHTSOURCE *lgt);
static void GetLIGHT_SETTINGS(FILE *fp,_LIGHTSOURCE *lgt);

_LIGHTSOURCE *LoadLightASE(char *filename)
{
	FILE *fp;
    char hole[256];
    _LIGHTSOURCE *lgt;
    int cnt;

	if( (fp=fopen(filename,"rt"))==NULL)
	{
        Error(filename," 요 라이트 파일이 없다!");
	}
    Geo_light_num=0;
	while( 1 )
	{
		if( fscanf(fp,"%s",&hole[0])==EOF)
			break;
        if (!strcmp(hole,"*LIGHTOBJECT"))
		{
            Geo_light_num++;
		}
	}
    fclose(fp);
    if(Geo_light_num==0)
	{
        Warning(filename,"라이트가 1개도 없다!");
		return NULL;
	}

    fp=fopen(filename,"rt");
    lgt=(_LIGHTSOURCE *)(Dmalloc(sizeof(_LIGHTSOURCE)*Geo_light_num));
    memset(lgt,0,sizeof(_LIGHTSOURCE)*Geo_light_num);
    lgt->light_num=Geo_light_num;

    cnt=0;
    while( 1 )
	{
		if( fscanf(fp,"%s",&hole[0])==EOF)
			break;
        if (!strcmp(hole,"*LIGHTOBJECT"))
		{
            GetLIGHTOBJECT(fp,&lgt[cnt]);
            cnt++;
		}
	}
	fclose(fp);
    return(lgt);
}

void GetLIGHTOBJECT(FILE *fp,_LIGHTSOURCE *lgt)
{
    char hole[256];
    int num=0;

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;
        if (!strcmp(hole,"*LIGHT_TYPE"))
		{
            fscanf(fp,"%s",&hole[0]);
            if(!strcmp(hole,"Omni"))
                lgt->type=_OMNI;
			else
            if(!strcmp(hole,"Directional"))
                lgt->type=_DIRECTIONAL;
            else
                lgt->type=_SPOT;
        }
        if (!strcmp(hole,"*NODE_TM"))
		{
            if(num==0)
                GetLIGHT_MAIN_TM(fp,lgt);
            else
                GetLIGHT_TARGET_TM(fp,lgt);
            num++;
            if( lgt->type==_DIRECTIONAL)
				lgt->pos[2]+=4000.0;
        }
        if (!strcmp(hole,"*LIGHT_SETTINGS"))
		{
            GetLIGHT_SETTINGS(fp,lgt);
        }

        if (!strcmp(hole,"*LIGHT_SHADOWS"))			// light shadows
		{
            fscanf(fp,"%s",&hole[0]);
            if( !strcmp("Off",hole) )
				lgt->is_shadow=0;
			else
				lgt->is_shadow=1;
		}
    }
}

void GetLIGHT_MAIN_TM(FILE *fp,_LIGHTSOURCE *lgt)
{
	char hole[256];

//    memset(lgt->matrix,0,16*4);
//    lgt->matrix[3][3]=1.0;
    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;
        if (!strcmp(hole,"*TM_ROW3"))
		{
            fscanf(fp,"%s",&hole[0]);
            lgt->pos[0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            lgt->pos[_READ_Y]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            lgt->pos[_READ_Z]=(float)atof(hole);
        }
        if (!strcmp(hole,"*TM_SCALE"))
        {
        }
    }
}
void GetLIGHT_TARGET_TM(FILE *fp,_LIGHTSOURCE *lgt)
{
    char hole[256];

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;
        if (!strcmp(hole,"*TM_ROW3"))
		{
            fscanf(fp,"%s",&hole[0]);
            lgt->tpos[0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            lgt->tpos[_READ_Y]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            lgt->tpos[_READ_Z]=(float)atof(hole);
        }
    }
}

void GetLIGHT_SETTINGS(FILE *fp,_LIGHTSOURCE *lgt)
{
    char hole[256];

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;

        if (!strcmp(hole,"*LIGHT_HOTSPOT"))
        {
            fscanf(fp,"%s",&hole[0]);
            lgt->hotspot=(float)atof(hole);
        }
        if (!strcmp(hole,"*LIGHT_FALLOFF"))
        {
            fscanf(fp,"%s",&hole[0]);
            lgt->falloff=(float)atof(hole);
        }
        if (!strcmp(hole,"*LIGHT_ATTNSTART"))
        {
            fscanf(fp,"%s",&hole[0]);
            lgt->attnstart=(float)atof(hole);
            lgt->attn_faron=1;
        }
        if (!strcmp(hole,"*LIGHT_ATTNEND"))
        {
            fscanf(fp,"%s",&hole[0]);
            lgt->attnend=(float)atof(hole);
        }
        if (!strcmp(hole,"*LIGHT_NEAR_ATTNSTART"))
        {
            fscanf(fp,"%s",&hole[0]);
            lgt->near_attnstart=(float)atof(hole);
            lgt->attn_nearon=1;
        }
        if (!strcmp(hole,"*LIGHT_NEAR_ATTNEND"))
        {
            fscanf(fp,"%s",&hole[0]);
            lgt->near_attnend=(float)atof(hole);
        }
        if (!strcmp(hole,"*LIGHT_INTENS"))
        {
            fscanf(fp,"%s",&hole[0]);
            lgt->intens=(float)atof(hole);
        }
        if (!strcmp(hole,"*LIGHT_COLOR"))
        {
            fscanf(fp,"%s",&hole[0]);
            lgt->rgb[0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            lgt->rgb[1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            lgt->rgb[2]=(float)atof(hole);
        }
        if (!strcmp(hole,"*LIGHT_SHADOW_DENSITY"))	// shadow density
		{
            fscanf(fp,"%s",&hole[0]);
			lgt->shadow_density=(float)atof(hole);
		}
        if (!strcmp(hole,"*LIGHT_SHADOW_COLOR"))	// ambient
		{
            fscanf(fp,"%s",&hole[0]);
			lgt->shadow_color[0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
			lgt->shadow_color[1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
			lgt->shadow_color[2]=(float)atof(hole);
		}
        if (!strcmp(hole,"*LIGHT_MAPSIZE"))	// shadow amp size
		{
            fscanf(fp,"%s",&hole[0]);
			lgt->shadow_size=(float)atof(hole);
		}
        if (!strcmp(hole,"*LIGHT_MAPRANGE"))	// shadow amp size
		{
            fscanf(fp,"%s",&hole[0]);
			lgt->shadow_sample_range=(float)atof(hole);
		}
    }
}
//---------------------------------------------------------------------------------
//---------------------------- Camera reader --------------------------------//
static void GetCAMERAOBJECT(FILE *fp,_CAMERA *camera);
static void GetCAMERA_SETTINGS(FILE *fp,_CAMERA *camera);
static void GetCAMERA_MAIN_TM(FILE *fp,_CAMERA *camera);
static void GetCAMERA_TARGET_TM(FILE *fp,_CAMERA *camera);

_CAMERA *LoadCameraASE(char *filename)
{
	FILE *fp;
	char hole[256];
    _CAMERA *camera;
    int cnt;

	if( (fp=fopen(filename,"rt"))==NULL)
	{
        Error(filename,"요파일이 없다!");
	}
    Geo_camera_num=0;
	while( 1 )
	{
		if( fscanf(fp,"%s",&hole[0])==EOF)
			break;
        if (!strcmp(hole,"*CAMERAOBJECT"))
		{
            Geo_camera_num++;
		}
	}
    fclose(fp);
    if(Geo_camera_num==0)
    {
//        Warning("Camera not found!"," 카메라가 1개이상 있어야함");
        return NULL;
    }

    fp=fopen(filename,"rt");
    camera=(_CAMERA *)(Dmalloc(sizeof(_CAMERA)*Geo_camera_num));
    memset(camera,0,sizeof(_CAMERA)*Geo_camera_num);
    camera[0].num=Geo_camera_num;

    cnt=0;
    while( 1 )
	{
		if( fscanf(fp,"%s",&hole[0])==EOF)
			break;
        if (!strcmp(hole,"*CAMERAOBJECT"))
		{
            GetCAMERAOBJECT(fp,&camera[cnt]);
            cnt++;
		}
	}
	fclose(fp);
    return(camera);
}
void GetCAMERA_SETTING_ANIMATION(FILE *fp,_CAMERA *camera)
{
	char hole[256];
    int num=0;

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;
        if (!strcmp(hole,"*CAMERA_SETTINGS"))
		{
            GetCAMERA_SETTINGS(fp,camera);
        }
	}
}

void GetCAMERAOBJECT(FILE *fp,_CAMERA *camera)
{
	char hole[256];
    int num=0;

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;
        if (!strcmp(hole,"*CAMERA_TYPE"))
		{
            fscanf(fp,"%s",&hole[0]);
            if (!strcmp(hole,"Target"))
            {
//                Error("Camera is only support Target type ","");
            }
			else
				camera->type = 1;	//Free type
        }
        if (!strcmp(hole,"*NODE_TM"))
		{
            if(num==0)
                GetCAMERA_MAIN_TM(fp,camera);
            else
                GetCAMERA_TARGET_TM(fp,camera);
            num++;
        }
        if (!strcmp(hole,"*NODE_PARENT"))
		{
            GetName(fp,hole);
			strcpy(camera->parentname,hole);
		}
        if (!strcmp(hole,"*NODE_NAME"))
		{
            GetName(fp,hole);
			strcpy(camera->objectname,hole);
		}

        if (!strcmp(hole,"*CAMERA_SETTINGS"))
		{
            GetCAMERA_SETTINGS(fp,camera);
        }
        if (!strcmp(hole,"*CAMERA_ANIMATION"))
		{
            GetCAMERA_SETTING_ANIMATION(fp,camera);
        }
        if (!strcmp(hole,"*TM_ANIMATION"))
		{
            GetTM_ANIMATION(fp,camera);
		}
    }
}

void GetCAMERA_SETTINGS(FILE *fp,_CAMERA *camera)
{
	char hole[256];

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;

        if (!strcmp(hole,"*CAMERA_FOV"))
        {
            fscanf(fp,"%s",&hole[0]);
            camera->fov=(float)atof(hole);
        }
        if (!strcmp(hole,"*CAMERA_TDIST"))
        {
            fscanf(fp,"%s",&hole[0]);
            camera->tdist=(float)atof(hole);
        }
    }
}

void GetCAMERA_MAIN_TM(FILE *fp,_CAMERA *camera)
{
	char hole[256];

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;
        if (!strcmp(hole,"*TM_ROW0"))
		{
            fscanf(fp,"%s",&hole[0]);
            camera->m_matrix[0][0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            camera->m_matrix[0][1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            camera->m_matrix[0][2]=(float)atof(hole);
            camera->m_matrix[0][3]=0.0;
        }
        if (!strcmp(hole,"*TM_ROW1"))
		{
            fscanf(fp,"%s",&hole[0]);
            camera->m_matrix[1][0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            camera->m_matrix[1][1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            camera->m_matrix[1][2]=(float)atof(hole);
            camera->m_matrix[1][3]=0.0;
        }
        if (!strcmp(hole,"*TM_ROW2"))
		{
            fscanf(fp,"%s",&hole[0]);
            camera->m_matrix[2][0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            camera->m_matrix[2][1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            camera->m_matrix[2][2]=(float)atof(hole);
            camera->m_matrix[2][3]=0.0;
        }
        if (!strcmp(hole,"*TM_ROW3"))
		{
            fscanf(fp,"%s",&hole[0]);
            camera->m_matrix[3][0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            camera->m_matrix[3][1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            camera->m_matrix[3][2]=(float)atof(hole);
            camera->m_matrix[3][3]=1.0;

//			camera->pos[0]=camera->m_matrix[3][0];
//			camera->pos[_READ_Y]=camera->m_matrix[3][1];
//			camera->pos[_READ_Z]=camera->m_matrix[3][2];
        }
        if (!strcmp(hole,"*TM_POS"))	//포지션 
        {
            fscanf(fp,"%s",&hole[0]);
            camera->pos[0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            camera->pos[1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            camera->pos[2]=(float)atof(hole);
		}
        if (!strcmp(hole,"*TM_ROTAXIS"))	//axis 
        {
            fscanf(fp,"%s",&hole[0]);
            camera->quat[0]=_QUAT_SIGN(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);	
            camera->quat[1]=_QUAT_SIGN(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            camera->quat[2]=_QUAT_SIGN(float)atof(hole);
        }
        if (!strcmp(hole,"*TM_ROTANGLE"))	//angle 
        {
            fscanf(fp,"%s",&hole[0]);
            camera->quat[3]=(float)atof(hole);
			//-------- 쿼터니온으로 바꾸자.
			D3DVECTOR a={camera->quat[0],camera->quat[1],camera->quat[2]};
			D3DMath_QuaternionFromRotation(camera->quat[0],camera->quat[1],camera->quat[2],camera->quat[3],a,camera->quat[3]);
	    }
        if (!strcmp(hole,"*TM_SCALE"))	//스케일
        {
            fscanf(fp,"%s",&hole[0]);
            //camera->scale[0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            //camera->scale[_READ_Y]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            //camera->scale[_READ_Z]=(float)atof(hole);
        }
        if (!strcmp(hole,"*TM_SCALEAXIS"))	//스케일
        {
            fscanf(fp,"%s",&hole[0]);
            //camera->s_quat[0]=_QUAT_SIGN(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            //camera->s_quat[_READ_Y]=_QUAT_SIGN(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            //camera->s_quat[_READ_Z]=_QUAT_SIGN(float)atof(hole);
        }
        if (!strcmp(hole,"*TM_SCALEAXISANG"))	//스케일
        {
            fscanf(fp,"%s",&hole[0]);
			/*
            camera->s_quat[3]=_QUAT_SIGN(float)atof(hole);
			//-------- 쿼터니온으로 바꾸자.
			D3DVECTOR a={camera->s_quat[0],camera->s_quat[1],camera->s_quat[2]};
			D3DMath_QuaternionFromRotation(camera->s_quat[0],camera->s_quat[1],camera->s_quat[2],camera->s_quat[3],a,camera->s_quat[3]);
			*/
		}
    }
//	GetMatrixFrom3DSMAXMatrix(camera->m_matrix);
/*
	float x,y,z,w;
	D3DMath_QuaternionFromMatrix(x,y,z,w,camera->d3d_trmatrix);
	camera->quat[0]=x;
	camera->quat[1]=y;
	camera->quat[2]=z;
	camera->quat[3]=w;*/
}

void GetCAMERA_TARGET_TM(FILE *fp,_CAMERA *camera)
{
    char hole[256];

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;
        if (!strcmp(hole,"*TM_ROW0"))
		{
            fscanf(fp,"%s",&hole[0]);
            camera->t_matrix[0][0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            camera->t_matrix[0][1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            camera->t_matrix[0][2]=(float)atof(hole);
            camera->t_matrix[0][3]=0.0;
        }
        if (!strcmp(hole,"*TM_ROW1"))
		{
            fscanf(fp,"%s",&hole[0]);
            camera->t_matrix[1][0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            camera->t_matrix[1][1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            camera->t_matrix[1][2]=(float)atof(hole);
            camera->t_matrix[1][3]=0.0;
        }
        if (!strcmp(hole,"*TM_ROW2"))
		{
            fscanf(fp,"%s",&hole[0]);
            camera->t_matrix[2][0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            camera->t_matrix[2][1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            camera->t_matrix[2][2]=(float)atof(hole);
            camera->t_matrix[2][3]=0.0;
        }
        if (!strcmp(hole,"*TM_ROW3"))
		{
            fscanf(fp,"%s",&hole[0]);
            camera->t_matrix[3][0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            camera->t_matrix[3][1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            camera->t_matrix[3][2]=(float)atof(hole);
            camera->t_matrix[3][3]=1.0;

			camera->target[0]=camera->t_matrix[3][0];
			camera->target[_READ_Y]=camera->t_matrix[3][1];
			camera->target[_READ_Z]=camera->t_matrix[3][2];
        }
        if (!strcmp(hole,"*TM_SCALE"))	//스케일
        {
            fscanf(fp,"%s",&hole[0]);
//            camera->scale[0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
  //          camera->scale[1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
    //        camera->scale[2]=(float)atof(hole);
        }
        if (!strcmp(hole,"*TM_POS"))	//포지션 
        {
            fscanf(fp,"%s",&hole[0]);
            //camera->pos[0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            //camera->pos[1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            //camera->pos[2]=(float)atof(hole);
		}
    }
//	GetMatrixFrom3DSMAXMatrix(camera->t_matrix);

}

//-----helper관련
void GetNODE_TM(FILE *fp,_HELPER *helper)
{
	char hole[256];

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;
        if (!strcmp(hole,"*TM_ROW0"))
		{
            fscanf(fp,"%s",&hole[0]);
            helper->trmatrix[0][0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            helper->trmatrix[0][1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            helper->trmatrix[0][2]=(float)atof(hole);
            helper->trmatrix[0][3]=0.0;
        }
        if (!strcmp(hole,"*TM_ROW1"))
		{
            fscanf(fp,"%s",&hole[0]);
            helper->trmatrix[1][0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            helper->trmatrix[1][1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            helper->trmatrix[1][2]=(float)atof(hole);
            helper->trmatrix[1][3]=0.0f;
        }
        if (!strcmp(hole,"*TM_ROW2"))
		{
            fscanf(fp,"%s",&hole[0]);
            helper->trmatrix[2][0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            helper->trmatrix[2][1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            helper->trmatrix[2][2]=(float)atof(hole);
            helper->trmatrix[2][3]=0.0;
        }
        if (!strcmp(hole,"*TM_ROW3"))
		{
            fscanf(fp,"%s",&hole[0]);
            helper->trmatrix[3][0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            helper->trmatrix[3][1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            helper->trmatrix[3][2]=(float)atof(hole);
            helper->trmatrix[3][3]=1.0;
        }
        if (!strcmp(hole,"*TM_SCALE"))	//스케일
        {
            fscanf(fp,"%s",&hole[0]);
            //helper->scale[0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            //helper->scale[_READ_Y]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            //helper->scale[_READ_Z]=(float)atof(hole);
        }
        if (!strcmp(hole,"*TM_POS"))	//포지션 
        {
            fscanf(fp,"%s",&hole[0]);
            helper->pos[0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            helper->pos[1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            helper->pos[2]=(float)atof(hole);
       }
        if (!strcmp(hole,"*TM_ROTAXIS"))	//axis 
        {
            fscanf(fp,"%s",&hole[0]);
            helper->quat[0]=_QUAT_SIGN(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);	
            helper->quat[1]=_QUAT_SIGN(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            helper->quat[2]=_QUAT_SIGN(float)atof(hole);
        }
        if (!strcmp(hole,"*TM_ROTANGLE"))	//angle 
        {
            fscanf(fp,"%s",&hole[0]);
			
            helper->quat[3]=(float)atof(hole);
			//-------- 쿼터니온으로 바꾸자.
//			MatrixFromRotateVector(Matrix,helper->qw,helper->qx,helper->qy,helper->qz);
//			QuaternionFromMatrix( helper->qx,helper->qy,helper->qz
//				,helper->qw,Matrix );
			D3DVECTOR a={helper->quat[0],helper->quat[1],helper->quat[2]};
			D3DMath_QuaternionFromRotation(helper->quat[0],helper->quat[1],helper->quat[2],helper->quat[3],a,helper->quat[3]);
	    }
        if (!strcmp(hole,"*TM_SCALE"))	//스케일
        {
            fscanf(fp,"%s",&hole[0]);
            //helper->scale[0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            //helper->scale[_READ_Y]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            //helper->scale[_READ_Z]=(float)atof(hole);
        }
        if (!strcmp(hole,"*TM_SCALEAXIS"))	//스케일
        {
            fscanf(fp,"%s",&hole[0]);
            //helper->s_quat[0]=_QUAT_SIGN(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            //helper->s_quat[_READ_Y]=_QUAT_SIGN(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            //helper->s_quat[_READ_Z]=_QUAT_SIGN(float)atof(hole);
        }
        if (!strcmp(hole,"*TM_SCALEAXISANG"))	//스케일
        {
            fscanf(fp,"%s",&hole[0]);
			/*
            helper->s_quat[3]=_QUAT_SIGN(float)atof(hole);
			//-------- 쿼터니온으로 바꾸자.
			D3DVECTOR a={helper->s_quat[0],helper->s_quat[1],helper->s_quat[2]};
			D3DMath_QuaternionFromRotation(helper->s_quat[0],helper->s_quat[1],helper->s_quat[2],helper->s_quat[3],a,helper->s_quat[3]);
			*/
		}
    }
}
void GetHELPEROBJECT(FILE *fp,_HELPER *obj)
{
    char hole[256];

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;
        if (!strcmp(hole,"*NODE_TM"))
		{
            GetNODE_TM(fp,obj);
		}
        if (!strcmp(hole,"*NODE_PARENT"))
		{
            GetName(fp,hole);
			strcpy(obj->parentname,hole);
		}
        if (!strcmp(hole,"*NODE_NAME"))
		{
            GetName(fp,hole);
			strcpy(obj->objectname,hole);
		}
        if (!strcmp(hole,"*BOUNDINGBOX_MIN"))
		{
            fscanf(fp,"%s",&hole[0]);
            obj->bb_min[0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            obj->bb_min[1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            obj->bb_min[2]=(float)atof(hole);
        }
        if (!strcmp(hole,"*BOUNDINGBOX_MAX"))
		{
            fscanf(fp,"%s",&hole[0]);
            obj->bb_max[0]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            obj->bb_max[1]=(float)atof(hole);
            fscanf(fp,"%s",&hole[0]);
            obj->bb_max[2]=(float)atof(hole);
        }
        if (!strcmp(hole,"*TM_ANIMATION"))
		{
            GetTM_ANIMATION(fp,obj);
		}
    }
}

_HELPER *LoadHelperASE(char *filename)
{
	FILE *fp;
    char hole[256];
    int cnt,helper_num=0;
	_HELPER *helper;

	if( (fp=fopen(filename,"rt"))==NULL)
	{
        Error(filename," not found : 요 파일이 없다!");
	}
	while( 1 )
	{
		if( fscanf(fp,"%s",&hole[0])==EOF)
			break;
        if (!strcmp(hole,"*HELPEROBJECT"))
		{
            helper_num++;
		}
	}
    fclose(fp);
    if(helper_num==0)
	{
//	    helper=(_HELPER *)(Dmalloc(sizeof(_HELPER)));
//		memset(helper,0,sizeof(_HELPER));
		return NULL;
	}
	else
	{
	    helper=(_HELPER *)(Dmalloc(sizeof(_HELPER)*helper_num));
		memset(helper,0,sizeof(_HELPER)*helper_num);
	}

	helper->num=helper_num;
	fp=fopen(filename,"rt");
    cnt=0;
    while( 1 )
	{
		if( fscanf(fp,"%s",&hole[0])==EOF)
			break;
        if (!strcmp(hole,"*HELPEROBJECT"))
		{
            GetHELPEROBJECT(fp,&helper[cnt]);
            cnt++;
		}
	}
	fclose(fp);

    return(helper);
}
