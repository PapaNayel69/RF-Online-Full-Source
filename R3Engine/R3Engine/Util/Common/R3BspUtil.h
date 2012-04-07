#ifndef _R3BSPUTIL_H_
#define _R3BSPUTIL_H_

#include "world.h"

#define _R3BSP_OPTIMIZE_NONE	0
#define _R3BSP_OPTIMIZE_HIGH	1

#define _R_OPTIMIZE_FASTEST		0
#define _R_OPTIMIZE_FAST		1
#define _R_OPTIMIZE_RADIOSITY	2

typedef struct{
	char In_file[256];
	char Out_file[256];
	int R3BspOptimize;		//bsp최적화 옵션.
	float EnergyLimit;
	Vector3f Ambient;	
	Vector3f Reflection;
	float PatchSize;
	int RenderState;
	int CPUNum;		//쓰레드에 사용될예정...
	int LightMapBits;		//라이트맵 비트수.
	BOOL IsExportLightmapBMP;	//라이트맵을 bmp로 export할거냐...

	_MESH *load_mesh;		//메쉬
	_MESH *w_mesh;			//메쉬
//	_MESH *o_mesh;			//통자메쉬..
	_MESH *p_mesh;			//메쉬의 다각형메쉬..

	int finish;

	char MapPath[256];	//맵 패스
}_INIFILE;

void R3BspInfoTextState(int option);	// 0=화면,프린트 둘다 ,1=프린트만 
void PutR3BspInfoText(const char *fmt, ... );
void SetR3BspInfoFileName(char *name);
void GetR3BspOutFileName(char *name,char *t_name);
_INIFILE *GetIniFile();

void StripEXT(char *name);
void StripPath(char *name);
void StripName(char *name);
void RemoveSpace(char *name);	//이름에 스페이스를 없앤다.
void MakeDir(char *name);	//디렉토리를 만들어준다.


void AddTexturePath(char *name);
char *GetFullTexPath(char *name);
BOOL IsExistFile(char *name);
void CheckBmpExp(_TEXTURE *tex);

//int FixFloatToInt(float su);	//float를 int로 올림해서 리턴한다.
int MaxFixFloatToInt(float su);
int MinFixFloatToInt(float su);

void StartTimer(void);
void EndTimer(void);
DWORD GetTimer(void);

void TestDebugOutPut(int num);
void TestDebugOutPutString(const char *fmt, ... );

#endif