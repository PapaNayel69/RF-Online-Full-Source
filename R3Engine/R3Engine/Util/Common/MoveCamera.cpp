#include <windows.h>
#include "MoveCamera.h"
#include <math.h>
#include <gl\glut.h>
#include <gl\gl.h>

#define ConvAngle(a)	(180.f*(a)/3.141592f)

void CalculateMoveCamera(_MOVE_CAMERA *camera)
{
/*
    double aa=(double)-camera->rho*(sin(camera->ph));
    camera->x=(float)(aa*(cos(camera->th))+camera->pointx);
    camera->z=(float)((double)-camera->rho*sin(-camera->th)+camera->pointz);
    camera->y=(float)((double)-camera->rho*cos(camera->th)
        *cos(camera->ph)+camera->pointy);
*/

    double aa=(double)-camera->rho*(sin(camera->ph));
    camera->pos[0]=(float)(aa*(cos(camera->th))+camera->point[0]);
    camera->pos[1]=(float)((double)-camera->rho*sin(-camera->th)+camera->point[1]);
    camera->pos[2]=(float)((double)-camera->rho*cos(camera->th)
        *cos(camera->ph)+camera->point[2]);

}

void ReMoveCamera(_MOVE_CAMERA *cam)
{
	GLfloat matrix[16];
	float a[3];
/*
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	if(cam->speed!=0)
	{
		a[0] = 0;
		a[1] = cam->speed;
		a[2] = 0;

		cam->rho=1;
		glRotatef(ConvAngle(cam->ph), 0.f, 1.0f, 0.f);
		glRotatef(ConvAngle(cam->th), 1.f, 0.f, 0.f);
		glGetFloatv(GL_MODELVIEW_MATRIX,matrix);

		cam->pointx += a[1]*matrix[8];
		cam->pointy += a[1]*matrix[10];
		cam->pointz += a[1]*matrix[9];
		cam->x += a[1]*matrix[8];
		cam->y += a[1]*matrix[10];
		cam->z += a[1]*matrix[9];
		cam->speed=0;

	}
	if(cam->step!=0)
	{
		a[0] = 0;
		a[1] = cam->step;
		a[2] = 0;

		cam->rho=1;
//		RotatePi(0,cam->ph+_PI/2.0,0);
		glRotatef(ConvAngle(cam->ph+3.141592/2.0f), 0.f, 1.f, 0.f);
		glGetFloatv(GL_MODELVIEW_MATRIX,matrix);

		cam->pointx += a[1]*matrix[8];
		cam->pointy += a[1]*matrix[10];
		cam->pointz += a[1]*matrix[9];
		cam->x += a[1]*matrix[8];
		cam->y += a[1]*matrix[10];
		cam->z += a[1]*matrix[9];
		cam->step=0;
	}
	glPopMatrix();
*/
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	if(cam->speed!=0)
	{
		a[0] = 0;
		a[1] = cam->speed;
		a[2] = 0;

		cam->rho=1;
		glRotatef(ConvAngle(cam->ph), 0.f, 1.0f, 0.f);
		glRotatef(ConvAngle(cam->th), 1.f, 0.f, 0.f);
		glGetFloatv(GL_MODELVIEW_MATRIX,matrix);

		cam->point[0] += a[1]*matrix[8];
		cam->point[1] += a[1]*matrix[9];
		cam->point[2] += a[1]*matrix[10];
		cam->pos[0] += a[1]*matrix[8];
		cam->pos[1] += a[1]*matrix[9];
		cam->pos[2] += a[1]*matrix[10];
		cam->speed=0;

	}
	if(cam->step!=0)
	{
		a[0] = 0;
		a[1] = cam->step;
		a[2] = 0;

		cam->rho=1;
//		RotatePi(0,cam->ph+_PI/2.0,0);
		glRotatef(ConvAngle(cam->ph+3.141592/2.0f), 0.f, 1.f, 0.f);
		glGetFloatv(GL_MODELVIEW_MATRIX,matrix);

		cam->point[0] += a[1]*matrix[8];
		cam->point[1] += a[1]*matrix[9];
		cam->point[2] += a[1]*matrix[10];
		cam->pos[0] += a[1]*matrix[8];
		cam->pos[1] += a[1]*matrix[9];
		cam->pos[2] += a[1]*matrix[10];
		cam->step=0;
	}
	glPopMatrix();

}
