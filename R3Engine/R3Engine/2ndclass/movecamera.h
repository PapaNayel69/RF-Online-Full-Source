#ifndef __MOVE_CAMERA_H__
#define __MOVE_CAMERA_H__

typedef struct{
	float pos[3],point[3];
	float th,ph,rho;
	float speed;
	float step;
	int free_camera;
}_MOVE_CAMERA;

void CalculateMoveCamera(_MOVE_CAMERA *camera);
void ReMoveCamera(_MOVE_CAMERA *cam);


#endif