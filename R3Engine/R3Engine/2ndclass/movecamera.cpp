#include "MoveCamera.h"
#include <math.h>
//#include <d3d8.h>
#include "d3dutil.h"
#include "D3dx8math.h"

#define ConvAngle(a)	(180.f*(a)/3.141592f)

void CalculateMoveCamera(_MOVE_CAMERA *camera)
{
    double aa = (double)-camera->rho*(sin(camera->ph));

    camera->pos[0] = (float)(aa*(cos(camera->th))                                 +camera->point[0]);
    camera->pos[1] = (float)((double)-camera->rho*sin(-camera->th)                +camera->point[1]);
    camera->pos[2] = (float)((double)-camera->rho*cos( camera->th)*cos(camera->ph)+camera->point[2]);
}

void ReMoveCamera(_MOVE_CAMERA *cam)
{
//	GLfloat matrix[16];
	float a[3];
	D3DXMATRIX mat,tmat,mat1;

	D3DXMatrixIdentity( &mat );
	D3DXMatrixIdentity( &mat1 );
	
	if(cam->speed!=0)
	{
		a[0] = 0;
		a[1] = cam->speed;
		a[2] = 0;

		cam->rho=1;
		D3DXMatrixRotationX( &mat1, cam->th );
		D3DXMatrixIdentity( &tmat );
		D3DXMatrixRotationY( &tmat, cam->ph );
		D3DXMatrixMultiply( &mat, &mat1,&tmat );
//		D3DXMatrixMultiply( &mat, &tmat,&mat1 );

		cam->point[0] += a[1]*mat._31;
		cam->point[1] += a[1]*mat._32;
		cam->point[2] += a[1]*mat._33;
		cam->pos[0] += a[1]*mat._31;
		cam->pos[1] += a[1]*mat._32;
		cam->pos[2] += a[1]*mat._33;
		cam->speed=0;

	}
	if(cam->step!=0)
	{
		a[0] = 0;
		a[1] = cam->step;
		a[2] = 0;

		cam->rho=1;
		D3DXMatrixRotationY( &mat, (float)(cam->ph+3.141592/2.0f) );

		cam->point[0] += a[1]*mat._31;
		cam->point[1] += a[1]*mat._32;
		cam->point[2] += a[1]*mat._33;
		cam->pos[0] += a[1]*mat._31;
		cam->pos[1] += a[1]*mat._32;
		cam->pos[2] += a[1]*mat._33;
		cam->step=0;
	}
}
