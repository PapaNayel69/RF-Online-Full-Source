#ifndef __LOADASE_H__
#define __LOADASE_H__

#include"world.h"

_MESH *LoadMeshASE(char *filename);
_CAMERA *LoadCameraASE(char *filename);
_MATERIAL *LoadMaterialASE(char *filename);
_LIGHTSOURCE *LoadLightASE(char *filename);
_HELPER *LoadHelperASE(char *filename);
_SCENE *LoadSceneASE(char *filename);
//_MESH *LoadCollisionMeshASE(char *filename);	충돌메쉬를 안쓴다.
//_2D_SHAPE *LoadShapeASE(char *filename);
//void SaveShape(char *filename,_2D_SHAPE *shape);
//int FindHelperObjectNumber(_HELPER *helper,char *objectname);


void AddTexturePath(char *name);
char *GetFullTexPath(char *name);

void SetAnimation(_MESH *mesh,int f_num);
int FindObjectNumber(_MESH *mesh,char *objectname);
void LoadVue(_MESH *mesh,char *filename,int actnum);

#endif
