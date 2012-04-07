#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <gl\glut.h>
#include <math.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "C_vector.h"
#include "World.h"
#include "Loadase.h"
#include "Jmalloc.h"
#include "R3BspUtil.h"
#include "R3Bsp.h"
#include "Rad.h"
#include "R3BspTool.h"
#include "MeshUtil.h"
#include "Material.h"
#include "MoveCamera.h"
#include "jerror.h"
#include "Patch.h"
#include "r3blockMat.h"
#include "Lightmap.h"
#include "LoadPreScript.h"
#include "DDSExport.h"

#define _READ_Y	1	//읽을때 바꾸려면...
#define _READ_Z	2

Vector3f camera_pos, camera_lookat, camera_up;
char Wintitle[]="R3Bsp ver 0.81";

double near_plane=1, far_plane=6000, camera_fov;
enum {STOP,FILTER,MOVE_CAMERA,CAMERA}; // menu ids
enum {LIT_MODEL=1, INDEX_MODEL}; // display lists

GLfloat viewanglex,viewangley, viewanglez, orgviewanglex,orgviewangley, orgviewanglez;

_CAMERA *Camera;
_MOVE_CAMERA MoveCamera;
static _MESH *RenderMesh;

//---------------------------------- open-gl을사용하는것에 관한 것 
void InitRender3DSMAXModel(_MESH *mesh)	//모델을 랜덤한 색으로 뿌린다. 
{
	Vector3f c0;
	int i,j;


	glNewList(LIT_MODEL, GL_COMPILE);
        glShadeModel(GL_SMOOTH);
		glBegin(GL_TRIANGLES);

		if(mesh->poly_type == TRUE)
		{
			for(i=0; i<mesh->objectnum ; i++)
			{
				for(j=0; j<mesh->object[i].facenum; j++)
				{
					c0[0]=(rand()%128)/256.0+.5;	c0[1]=(rand()%128)/256.0+.5;	c0[2]=(rand()%128)/256.0+.5;
					int v0=mesh->object[i].p_face[j].vertex_id[0];
					int v1=mesh->object[i].p_face[j].vertex_id[1];
					int v2=mesh->object[i].p_face[j].vertex_id[2];

					glColor3f(c0[0], c0[1], c0[2]);
					glVertex3f(mesh->object[i].vertex[v0].local[0], mesh->object[i].vertex[v0].local[1],mesh->object[i].vertex[v0].local[2]);
					glVertex3f(mesh->object[i].vertex[v1].local[0], mesh->object[i].vertex[v1].local[1],mesh->object[i].vertex[v1].local[2]);
					glVertex3f(mesh->object[i].vertex[v2].local[0], mesh->object[i].vertex[v2].local[1],mesh->object[i].vertex[v2].local[2]);
				}
			}
		}
		else
		{
			for(i=0; i<mesh->objectnum ; i++)
			{
				for(j=0; j<mesh->object[i].facenum; j++)
				{
					c0[0]=(rand()%128)/256.0+.5;	c0[1]=(rand()%128)/256.0+.5;	c0[2]=(rand()%128)/256.0+.5;
					int v0=mesh->object[i].face[j].v[0];
					int v1=mesh->object[i].face[j].v[1];
					int v2=mesh->object[i].face[j].v[2];

					glColor3f(c0[0], c0[1], c0[2]);
					glVertex3f(mesh->object[i].vertex[v0].local[0], mesh->object[i].vertex[v0].local[1],mesh->object[i].vertex[v0].local[2]);
					glVertex3f(mesh->object[i].vertex[v1].local[0], mesh->object[i].vertex[v1].local[1],mesh->object[i].vertex[v1].local[2]);
					glVertex3f(mesh->object[i].vertex[v2].local[0], mesh->object[i].vertex[v2].local[1],mesh->object[i].vertex[v2].local[2]);
				}
			}
		}
		glEnd();
	glEndList();
}
void DeleteLightTexture(void)
{
	CGenLightMap *gen_map = GetLightmap();
	if( gen_map==NULL )
		return;//		Error("먼저 라이트맵을 만들어라.","");
	int lgt_cnt=GetLightmapCnt();
	for(GLuint i=0; i<(GLuint)lgt_cnt; i++)
	{
		_LIGHT_MAP *lgt_map = &gen_map->mLgtMap;
		glBindTexture(GL_TEXTURE_2D, i+1);
		glDeleteTextures (lgt_map->xl*lgt_map->yl*3,&i);
	}
}

void MakeLightTextureFromLightmap(void)
{

    glDisable(GL_LIGHTING);
    glDisable(GL_DITHER);
    glShadeModel(GL_FLAT);
	glEnable(GL_TEXTURE_2D);	//1번만..

//	glBindTexture(GL_TEXTURE_2D, i+1);
//	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_DEPTH_TEST);

	CGenLightMap *gen_map = GetLightmap();
	if( gen_map==NULL )
	{
		Warning("먼저 라이트맵을 만들어라.(또는 전체가 버텍스라이트일수있음)","");
		return;
	}

	int lgt_cnt=GetLightmapCnt();
	for(GLuint i=0; i<(GLuint)lgt_cnt; i++)
	{
		_LIGHT_MAP *lgt_map = &gen_map->mLgtMap;
		glPixelStorei(GL_UNPACK_ALIGNMENT,1);
		glBindTexture(GL_TEXTURE_2D, i+1);
//WINGDIAPI GLboolean APIENTRY glAreTexturesResident (GLsizei n, const GLuint *textures, GLboolean *residences);
//WINGDIAPI void APIENTRY glDeleteTextures (GLsizei n, const GLuint *textures);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);	//필터링no
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	//필터링no
		GLubyte *g_buf = (GLubyte *)Dmalloc(lgt_map->xl*lgt_map->yl*3);

		for(int j=0; j<lgt_map->xl*lgt_map->yl; j++)
		{
			g_buf[3*j+0]=lgt_map->bitmap[j].R;
			g_buf[3*j+1]=lgt_map->bitmap[j].G;
			g_buf[3*j+2]=lgt_map->bitmap[j].B;
		}
		glTexImage2D(GL_TEXTURE_2D,0,3,lgt_map->xl,lgt_map->yl,0,
			GL_RGB,GL_UNSIGNED_BYTE,g_buf);
		Dfree(g_buf);
	}
//	glBindTexture(GL_TEXTURE_2D, 4);
}
void InitRenderLightmapFrom3DSMAXModel(_MESH *mesh)
{
	int i,j,k,cnt=0;

//	int test[10];
//	test[1]=0;
//	test[2]=0;
//	test[3]=0;

	glNewList(LIT_MODEL, GL_COMPILE);
//        glShadeModel(GL_SMOOTH);
	for(k=0; k<mesh->objectnum; k++)
	{
		for(j=0; j<mesh->object[k].facenum; j++)
		{
			glBegin(GL_TRIANGLES);
			for(i=0; i<mesh->object[k].p_face[j].vertex_num-2; i++)
			{
				int v0=mesh->object[k].p_face[j].vertex_id[0];
				int v1=mesh->object[k].p_face[j].vertex_id[i+1];
				int v2=mesh->object[k].p_face[j].vertex_id[i+2];

				glNormal3f(0.0, 1.0, 0.0);
//				glBindTexture(GL_TEXTURE_2D, mesh->object[k].p_face[j].mtlid+1);
				glBindTexture(GL_TEXTURE_2D, mesh->object[k].p_face[j].l_mtlid+1);
//				test[mesh->object[k].p_face[j].l_mtlid+1]++;
				glTexCoord2f(mesh->object[k].p_face[j].lu[0],mesh->object[k].p_face[j].lv[0]);
				glVertex3f(mesh->object[k].vertex[v0].local[0], mesh->object[k].vertex[v0].local[_READ_Y],mesh->object[k].vertex[v0].local[_READ_Z]);
				glTexCoord2f(mesh->object[k].p_face[j].lu[i+1],mesh->object[k].p_face[j].lv[i+1]);
				glVertex3f(mesh->object[k].vertex[v1].local[0], mesh->object[k].vertex[v1].local[_READ_Y],mesh->object[k].vertex[v1].local[_READ_Z]);
				glTexCoord2f(mesh->object[k].p_face[j].lu[i+2],mesh->object[k].p_face[j].lv[i+2]);
				glVertex3f(mesh->object[k].vertex[v2].local[0], mesh->object[k].vertex[v2].local[_READ_Y],mesh->object[k].vertex[v2].local[_READ_Z]);
				cnt++;
			}
			glEnd();
		}
	}
	glEndList();
}

//--------------------------------------------------------------------------------------

static int Rad_cnt=0;

void Step() // the main radiosity step
{
/*
	int gop=(GetTickCount()-Start_time)/(1000.0);
	PutR3BspInfoText("all process finished!\n");
	PutR3BspInfoText("RenderTime %2d:%2d:%2d seconds!\n",(gop/3600),(gop%3600)/60,gop%60);
*/
}

void SetGlCamera(_CAMERA *cam)
{
	memset(&MoveCamera,0,sizeof(_MOVE_CAMERA));
	camera_fov=(30*cam->fov)/0.5236;
	viewanglex=0;
	viewanglez=0;
	if( cam->type == 1 ) //free
	{
		MoveCamera.is_free = TRUE;
		memcpy(MoveCamera.tr_matrix,cam->m_matrix,sizeof(float)*16);
		camera_pos[0]=cam->m_matrix[3][0];
		camera_pos[1]=cam->m_matrix[3][1];
		camera_pos[2]=cam->m_matrix[3][2];
		camera_up[0]=0.0;
		camera_up[1]=1.0;
		camera_up[2]=0.0;
	}
	else
	{
		camera_pos[0]=cam->pos[0];
		camera_pos[1]=cam->pos[1];
		camera_pos[2]=cam->pos[2];

		camera_lookat[0]=cam->target[0];
		camera_lookat[1]=cam->target[1];
		camera_lookat[2]=cam->target[2];

		viewanglex=0.0;
		viewanglez=0.0;
		camera_up[0]=0.0;
		camera_up[1]=1.0;
		camera_up[2]=0.0;

		MoveCamera.rho=1;
		MoveCamera.point[0] = camera_lookat[0];
		MoveCamera.point[1] = camera_lookat[1];
		MoveCamera.point[2] = camera_lookat[2];
	}
}

void DrawDebugPatch(int ob_num,int num)	//해당 팻치의 점을 찍어준다.
{
	_LFACE **lf = GetLFace();
	if( lf==NULL )
		return;

	glPointSize(5);
  	glBegin(GL_POINTS);

	glColor3f(0,0,0);
	for(int i=0; i< lf[ob_num][num].patch->yl; i++)
		for(int j=0; j< lf[ob_num][num].patch->xl; j++)
		{
//			if( !lf[ob_num][num].patch->winding[lf[ob_num][num].patch->xl*i+j].is_out_face )
				glVertex3fv(lf[ob_num][num].patch->winding[lf[ob_num][num].patch->xl*i+j].pos);
		}

	glEnd();
	glPointSize(1);

	Vector3f temp;
	glBegin(GL_LINES);
	glColor3f(1,0,0);
	for(i=0; i< lf[ob_num][num].patch->yl; i++)
		for(int j=0; j< lf[ob_num][num].patch->xl; j++)
		{
//			if( !lf[ob_num][num].patch->winding[lf[ob_num][num].patch->xl*i+j].is_out_face )
			{
				glVertex3fv(lf[ob_num][num].patch->winding[lf[ob_num][num].patch->xl*i+j].pos);

				Vector3fScale(lf[ob_num][num].patch->winding[lf[ob_num][num].patch->xl*i+j].normal,4,temp);
				Vector3fAdd(lf[ob_num][num].patch->winding[lf[ob_num][num].patch->xl*i+j].pos,temp,temp);
				glVertex3fv(temp);
			}
		}
	glEnd();

	glBegin(GL_LINES);
	Vector3f pos;
	for(i=0; i< RenderMesh->object[ob_num].p_face[num].vertex_num; i++)
	{
		glColor4f(.9f,.9f,.9f,1);
		Vector3fCopy(RenderMesh->object[ob_num].vertex[RenderMesh->object[ob_num].p_face[num].vertex_id[i]].local,pos);
		glVertex3fv(pos);
		Vector3fScale(RenderMesh->object[ob_num].p_face[num].v_normal[i],6,temp);
		Vector3fAdd(pos,temp,temp);
		glVertex3fv(temp);
	}
	glEnd();
	float aa[4];
	glGetFloatv(GL_CURRENT_COLOR,aa);
}


// GLUT callbacks /////////////////////////////////////////////////////////////////////////
void redraw(void)
{
	glClearColor(248/255.0f,193/255.0f,7/255.0f,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION); // put camera in the correct position
	glLoadIdentity();
	gluPerspective(camera_fov, 1, near_plane, far_plane);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_CULL_FACE);
	if( MoveCamera.move_camera )
	{
		gluLookAt(MoveCamera.pos[0], MoveCamera.pos[1]
			, MoveCamera.pos[2], MoveCamera.point[0]
			, MoveCamera.point[1], MoveCamera.point[2]
			, camera_up[0], camera_up[1], camera_up[2]); 
	}
	else
	{
		if( MoveCamera.is_free )
		{
			glDisable(GL_CULL_FACE);
			D3DMATRIX d3dmatrix;
			//float matrix[16];
			glRotatef(viewanglex, 1.f, 0.f, 0.f);
			glRotatef(viewanglez, 0.f, 0.f, 1.f);
			//glGetFloatv(GL_MODELVIEW_MATRIX,matrix);
			//glTranslatef(-MoveCamera.tr_matrix[3][0],-MoveCamera.tr_matrix[3][1],-MoveCamera.tr_matrix[3][2]);
			
			D3DMath_MatrixInvert(d3dmatrix,*(D3DMATRIX *)MoveCamera.tr_matrix);
			glMultMatrixf((float*)&d3dmatrix);
  			//glLoadMatrixf((float*)MoveCamera.tr_matrix);
		}
		else
		{
			glRotatef(viewanglex, 1.f, 0.f, 0.f);
			glRotatef(viewanglez, 0.f, 1.f, 0.f);
	//		glRotatef(viewanglez, 0.f, 0.f, 1.f);
			gluLookAt(camera_pos[0], camera_pos[1], camera_pos[2], camera_lookat[0], camera_lookat[1], camera_lookat[2], camera_up[0], camera_up[1], camera_up[2]); 
			float matrix[16];
			glGetFloatv(GL_MODELVIEW_MATRIX,matrix);
		}
	}

	glCallList(LIT_MODEL);	//컴파일된 모델을 뿌린다.

//	DrawDebugPatch(0,0);
//	DrawDebugPatch(0,1);
//	DrawDebugPatch(0,2);
//	DrawDebugPatch(1,0);
//	DrawDebugPatch(2,0);
//	DrawDebugPatch(0,2);
//	DrawDebugPatch(0,3);
//	DrawDebugPatch(0,4);
//	DrawDebugPatch(0,5);
//	DrawDebugPatch(0,6);

	//	DrawBspSplitPolygon();	//잘려진 폴리곤을 뿌린다.
	DrawBspLeafBBox();		//바운딩 박스를 뿌린다.

	glutSwapBuffers();
}

void special(int key, int x, int y)
{
	switch (key) 
	{
	case GLUT_KEY_LEFT:
		viewanglez += 10.f; glutPostRedisplay(); break;
	case GLUT_KEY_RIGHT:
		viewanglez -= 10.f; glutPostRedisplay(); break;
	case GLUT_KEY_UP:
		viewanglex -= 10.f; glutPostRedisplay(); break;
	case GLUT_KEY_DOWN:
		viewanglex += 10.f; glutPostRedisplay(); break;
	break;
	}
}

void key(unsigned char key, int x, int y)
{
	switch (key) 
	{
	case ' ':
		break;
	case '\033':
		exit(0);
		break;

	case 'w':
		MoveCamera.speed=1;
		ReMoveCamera(&MoveCamera);
	break;
	case 's':
		MoveCamera.speed=-1;
		ReMoveCamera(&MoveCamera);
	break;
	case 'd':
		MoveCamera.step=-1;
		ReMoveCamera(&MoveCamera);
	break;
	case 'a':
		MoveCamera.step=1;
		ReMoveCamera(&MoveCamera);
	break;

	}
	glutPostRedisplay();
}

static int startx, starty; // used for dragging
void mouse(int button, int state, int x, int y)
{
	if (state == GLUT_UP) 
	{
		viewanglex=orgviewanglex+(starty-y); viewanglez=orgviewanglez+(startx-x);
	} 
	else // GLUT_DOWN 
	{              
		startx = x; starty = y;
		orgviewanglex=viewanglex; orgviewanglez=viewanglez;
	}
	if(viewanglex<-179)
		viewanglex=-179;
	if(viewanglex>179)
		viewanglex=179;
	MoveCamera.ph = (viewanglez*_PI)/360.0f;
	MoveCamera.th = -(viewanglex*_PI)/360.0f;
	CalculateMoveCamera(&MoveCamera);
	glutPostRedisplay();
}

void motion(int x, int y)
{
	viewanglex=orgviewanglex+(starty-y);
	viewanglez=orgviewanglez+(startx-x);
	if(viewanglex<-179)
		viewanglex=-179;
	if(viewanglex>179)
		viewanglex=179;
		
	MoveCamera.ph = (viewanglez*_PI)/360.0f;
	MoveCamera.th = -(viewanglex*_PI)/360.0f;
	CalculateMoveCamera(&MoveCamera);
	glutPostRedisplay();
}

static int Spotnum[256];
static int Spotcnt=0;

void menu(int sel)
{
	if(sel==MOVE_CAMERA)
	{
		MoveCamera.move_camera=1;
		glutPostRedisplay();
	}
	if(sel==STOP)
	{
	}
	if(sel==FILTER)
	{
		static int filter_on=0;
		filter_on=~filter_on;
	}
    if(sel>=CAMERA && sel < CAMERA+Camera->num)
	{
		SetGlCamera(&Camera[sel-CAMERA]);
    }
	glutPostRedisplay();
}

void reshape(int w, int h)
{
// dummy to prevent the viewport from resizing
}

void ReadObject(char* asefile)
{
	_MESH *load_mesh;	//첨 로딩했던 매쉬
	_MESH *w_mesh;		//월드로 고친메쉬
	_MESH *p_mesh;		//폴리곤 메쉬
	_MESH *col_one_mesh;	//충돌 폴리곤 메쉬...
	_MESH *col_p_mesh;	//충돌 폴리곤 메쉬...
	_MATERIAL *org_mat,*one_mat,*composite_mat;

	_INIFILE *Ini=GetIniFile();
	//---------- 출력할 BSP파일이름을 얻어낸다.
	GetR3BspOutFileName(Ini->In_file,Ini->Out_file);	//출력할 BSP파일이름을 얻어낸다.

	PutR3BspInfoText("Loading!...\n");
#ifdef 	_DEBUG
//	No_warning=1;	//워닝 끄자...-.-
#endif
    _SCENE *scene=LoadSceneASE(asefile);
    load_mesh = LoadMeshASE(asefile);
    GetInvScaleMatrix(load_mesh);		//애니메이션을위한 inv스케일매트릭스를 구한다.
	
	GetPreScriptAtt(asefile,load_mesh);	//프리 스크립트의 내용을 얻어온다.

	if(load_mesh==NULL)
    {
        puts("object not found!");
        exit(0);
    }
	org_mat = LoadMaterialASE(asefile);
	one_mat = ConvOneMaterial(org_mat);	//반드시 ConvOneMesh 다음에 호출할것...오리지날 메쉬를 하나로...

	GetCompositeMat(one_mat,&composite_mat);	// 머터리얼에서 composite머터리얼만 추출해낸다...
    Camera=LoadCameraASE(asefile);		// 카메라 로드

	/*	충돌 폴리곤을 따로 안두기로 결정.
	col_mesh = load_mesh;
    col_mesh = LoadCollisionMeshASE(asefile);		//충돌 폴리곤메쉬...
	if( col_mesh->objectnum == 0 )
	{
		//Warning("충돌폴리곤이 없어서 그냥폴리곤들을 충돌 폴리곤으로 사용합니다.");
		col_mesh = load_mesh;
	}
	else
		SetWorldMesh(load_mesh,org_mat);  // wx,wy,wz를 구하기위해. 충돌매쉬와 월드매쉬가 같으면 한번만 호출하자...
	*/
	SetWorldMesh(load_mesh,org_mat);  // wx,wy,wz를 구하기위해. 충돌매쉬와 월드매쉬가 같으면 한번만 호출하자...

	w_mesh = load_mesh;
	GetIniFile()->w_mesh = w_mesh;

	//볼륨 포그영역의 폴리곤들을 생성한다. 주의할점은 볼륨포그하나당 머터리얼이 하나씩 늘어간다.
//	one_mat = MakeVolumeFogObject(w_mesh,one_mat);

	SetMeshFaceNormal(w_mesh,w_mesh);			// 삼각메쉬의 normal을 구하자. 
	p_mesh=ConvMeshTriToPoly(w_mesh);  // 삼각메쉬를 폴리곤메쉬로

	CalculateMeshSmooth(p_mesh);	// 스무스그룹 normal을 구하자. 
	SetMeshCompositeMat(p_mesh,org_mat);	//Composite오브젝트의 경우. 총 개수와 오브젝트에 속성을 넣어준다.

	GetIniFile()->p_mesh = p_mesh;

	AdjustMeshMaterial(p_mesh,one_mat);	//머터리얼 정리.....

	if( Camera != NULL )
		SetGlCamera(&Camera[0]);
	else
	{
		camera_pos[0]=0;
		camera_pos[1]=0;
		camera_pos[2]=50;

		camera_lookat[0]=0;
		camera_lookat[1]=0;
		camera_lookat[2]=0;

		camera_fov=(30*0.78)/0.5236;
		viewanglex=0.0;
		viewanglez=0.0;
		camera_up[0]=0.0;
		camera_up[1]=0.0;
		camera_up[2]=1.0;
	}
	
	//------------팻치생성
	PutR3BspInfoText("Make Patch!\n");
	MakePatchAndWinding(p_mesh);	// 멀티오브젝트 메쉬를 인자로....
//	MakePatchAndWinding(p_mesh,1);	// 멀티오브젝트를 인자로....

	//------------라이트계산및 라이트맵 생성
    _LIGHTSOURCE *lgt_source=LoadLightASE(asefile);	//라이트로드

	char buf[512];
	sprintf(buf,"Dmalloc size = %4.2fMByte\n",GetDmallocSize()/1000000.0f);
	PutR3BspInfoText(buf);

	GetLightRadiosity(p_mesh,one_mat,lgt_source);		// 라이트에 대한 래디오시티를 구한다.

	PutR3BspInfoText("Make Material Block...\n");
	//-----블록 설정을 준비한다.
	DWORD p_face_num = GetFaceNumFromMesh(p_mesh);

	R3BlockBsp CBlockBsp(p_face_num);						//블록설정 준비..
	CBlockBsp.PrepareBlockBsp(p_mesh,one_mat);				//블록들을 합쳐놓는다.
	
	//-----머지된 오브젝트로 bsp를 만든다.
	col_one_mesh=ConvOneMesh(w_mesh,org_mat);  // 정적인것은 하나의 메쉬로
//	SetMeshFaceNormal(col_one_mesh,w_mesh);			// 삼각메쉬의 normal을 구하자. 
	_MESH *temp_p_mesh=ConvMeshTriToPoly(col_one_mesh);  // 하나의 삼각메쉬를 폴리곤메쉬로

	col_p_mesh = CBlockBsp.MakePMeshBlockBsp(temp_p_mesh,p_mesh);

//	if( col_p_mesh->object->facenum

	PutR3BspInfoText("Now Bsp Compile...\n");
	MakeBSPTree(col_p_mesh);	//bsp-tree를 생성.. 충돌메쉬를 줘서...

	PutR3BspInfoText("Ok end!\n");
	BspPrint();	//bsp프린팅

	sprintf(buf,"Dmalloc size = %4.2fMByte\n",GetDmallocSize()/1000000.0f);

	MakeLightmapsFromPatchs(p_mesh,one_mat,&CBlockBsp,composite_mat);				// 패치로부터 라이트맵을 생성한다.
	SaveLightMap(p_mesh,composite_mat);


	RenderMesh = p_mesh;

	PutR3BspInfoText("Bsp Export!\n");
	BspExport(col_p_mesh,p_mesh,Ini->Out_file,&CBlockBsp);	//bsp 파일 생성.
	//-------------------- 텍스쳐를 DDS로 컨버트 
	PutR3BspInfoText("DDS Texture Export!\n");

	//--머터리얼 넣을 패스 생성.
	char mat_path[256]=".\\Map\\";
	char m_name[256];
	strcpy(m_name,Ini->In_file);
	StripPath(m_name);
	StripEXT(m_name);
	if( mat_path != NULL )
	{
		strcat(mat_path,m_name);
		strcat(mat_path,"\\tex");
		MakeDir(mat_path);
	}
//	printf("%s",mat_path);
	DDSTextureExport(p_mesh,one_mat);		//dds 텍스쳐로 컨버트한다.

	//dds포팅을 먼저해야 이전에 export되었던 파일을 포팅하지 않는다.
	MaterialExport(one_mat);		//머터리얼 스크립터로 출력한다.

//	if( org_mat )
//		MakeLightTextureFromLightmap();					//라이트맵을 open-gl텍스쳐에 올린다.

//	InitRenderLightmapFrom3DSMAXModel(p_mesh);		//open-gl로 렌더링할 준비를 한다.

	EndTimer();
	int gop=GetTimer()/1000;
	PutR3BspInfoText("****************************************\n");
	sprintf(buf,"Render Time %4d:%2d:%2d Seconds!\n",(gop/3600),(gop%3600)/60,gop%60);
	PutR3BspInfoText(buf);
	PutR3BspInfoText("****************************************\n");
	PutR3BspInfoText("렌더링이 끝났습니다. 아무키나 누르시오\n");
	getch();
	exit(0);
}
void ReadR3EngineIniFile(const char* inifile)
{
	char entry[256];

	_INIFILE *Ini=GetIniFile();

    GetPrivateProfileString("Path", "MapPath", "0", entry, 256, inifile);
	strcpy(Ini->MapPath,entry);
}
void ReadR3BspIniFile(const char* inifile)
{
	char entry[256];

	_INIFILE *Ini=GetIniFile();
	SetR3BspInfoFileName(Ini->In_file);	//로그파일 초기화

    GetPrivateProfileString("R3BSP", "R3BSPOptimize", "0", entry, 256, inifile);
    GetPrivateProfileString("ExportLightmapBMP", "FALSE", "0", entry, 256, inifile);
	strlwr(entry);
	if( !strcmp(entry,"true"))
		Ini->IsExportLightmapBMP = TRUE;
	else
		Ini->IsExportLightmapBMP = FALSE;

	if( !strcmp(entry,"NONE") )
		Ini->R3BspOptimize=_R3BSP_OPTIMIZE_NONE;
	else
		Ini->R3BspOptimize=_R3BSP_OPTIMIZE_HIGH;


	GetPrivateProfileString("RenderState","LightMapBits","16",entry, 256, inifile);
	Ini->LightMapBits=atoi(entry);	//라이트맵	비트수..

	GetPrivateProfileString("RenderState","RenderSpeed","FASTEST",entry, 256, inifile);
	if(!strcmp("RADIOSITY",entry))
		Ini->RenderState = _R_OPTIMIZE_RADIOSITY;
	else
	if(!strcmp("FAST",entry))
		Ini->RenderState = _R_OPTIMIZE_FAST;
	else
		Ini->RenderState = _R_OPTIMIZE_FASTEST;

	GetPrivateProfileString("RenderState","CPUNum","1",entry, 256, inifile);
	Ini->CPUNum=atoi(entry);	//시피유갯수 쓰레드에 쓸예정.

	GetPrivateProfileString("RenderState","PatchSize","4.0",entry, 256, inifile);
	Ini->PatchSize=atof(entry);

//------------------------
	Ini->Ambient[0]=GetPrivateProfileInt("RenderEnv","AmbientRed", 0, inifile)/255.0f;
	Ini->Ambient[1]=GetPrivateProfileInt("RenderEnv","AmbientGreen", 0, inifile)/255.0f;
	Ini->Ambient[2]=GetPrivateProfileInt("RenderEnv","AmbientBlue", 0, inifile)/255.0f;
/*
	Ini->Reflection[0]=GetPrivateProfileInt("RenderEnv","ReflectionRed", 0, inifile)/255.0f;
	Ini->Reflection[1]=GetPrivateProfileInt("RenderEnv","ReflectionGreen", 0, inifile)/255.0f;
	Ini->Reflection[2]=GetPrivateProfileInt("RenderEnv","ReflectionBlue", 0, inifile)/255.0f;
	GetPrivateProfileString("RenderEnv","EnergyLimit","0.01",entry, 256, inifile);
	Ini->EnergyLimit=atof(entry);
//	Ini.res=GetPrivateProfileInt("Settings", "resolution", 256, inifile);
*/
	FILE *fp;
	fp = fopen(inifile,"rt");
	if( fp == NULL)
	{		
		Error((char*)inifile,"파일이 없음.");
	}
	char hole[256];

	while(1)
	{
		if( fscanf(fp,"%s",&hole[0])==EOF)
			break;
		if( !strcmp(hole,"[TexturePath]"))
		{
			while(1)
			{
				if( fscanf(fp,"%s",&hole[0])==EOF)
					break;
				AddTexturePath(hole);
			}
			break;
		}
	}	
	fclose(fp);
}

void main(int argc, char **argv)
{
	/*
	HDC dc=GetDC(0); // desktop dc
	int bpp=GetDeviceCaps(dc, BITSPIXEL);
	ReleaseDC(0, dc);
	if(bpp<24)
	{
		MessageBox(0, "Please change to 24 or 32 bit colour depth", "Warning", MB_TASKMODAL);
		return;
	}*/

//	TestDebugOutPut(2);
//-------------------- Ase를 파일창에서 불러온다.
	static OPENFILENAME ofn; // zero the ofn members out
    char ase_name[256]="1.map";

	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = ase_name;
	ofn.nMaxFile = sizeof(ase_name);
	ofn.lpstrFilter = "R3MAP Map (*.MAP)\0*.MAP\0All (*.*)\0*.*\0";
	ofn.lpstrTitle = "Select MAP file";
	ofn.Flags = OFN_NOCHANGEDIR | OFN_HIDEREADONLY |
               OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (!GetOpenFileName(&ofn))
		return;

	strcpy(GetIniFile()->In_file,ase_name);
//-------------------------------------------------
	StartTimer();
	glutInit(&argc, argv);

    ReadR3BspIniFile(".\\r3bsp.ini");	//렌더링 각종 정보를 읽는다.
    ReadR3EngineIniFile(".\\r3engine.ini");	//엔진 각종 정보를 읽는다.

    glutInitWindowSize(512,512);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow(Wintitle);
	glutDisplayFunc(redraw);
	glutKeyboardFunc(key);
	glutSpecialFunc(special);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutIdleFunc(Step);
	glutReshapeFunc(reshape);

    ReadObject(GetIniFile()->In_file);

	glutCreateMenu(menu);
	glutAddMenuEntry("Stop", STOP);
	glutAddMenuEntry("Move Camera", MOVE_CAMERA);
	glutAddMenuEntry("Filtering", FILTER);

	if( Camera )
	{
		for(int i=0;i<Camera->num;i++)		//카메라 
		{
			char name[128];
			sprintf(name,"camera%3d",i+1);
			glutAddMenuEntry(name,CAMERA+i);
		}
	}
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	viewanglex=viewanglez=0;

	glutMainLoop(); // the action starts with the Step function above
}
