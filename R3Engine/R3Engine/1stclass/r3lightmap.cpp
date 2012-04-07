#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Jmalloc.h"
#include "R3d3dtex.h"
#include "R3Lightmap.h"

//#define _LIGHT_MAP_BMP

static int Lightmap_cnt;

_LIGHTMAP **stLightmap;
int *LightmapTexID;
static R3Texture *stLightmapTex=NULL;

void LoadLightMap(char *name)
{
	char temp[256];
	//char lgtfilepath[256]=".\\LightMap\\";
	int i;
	
	strcpy(temp,name);
//	strcat(temp,".r3t");

/*
#ifdef	_LIGHT_MAP_BMP
	FILE *fp;
	char filename[256];

	strcat(temp,".txt");
	if( ( fp = fopen(temp,"rt") )==NULL )
		return;
	fscanf(fp,"%s",temp);
	fscanf(fp,"%s",temp);
	Lightmap_cnt=atoi(temp);
	fclose(fp);

	Lightmap = (int*)Dmalloc(sizeof(int)*Lightmap_cnt);
	for(i=0; i<Lightmap_cnt; i++)
	{
		sprintf(filename,"%s%06d.bmp",lgtfilepath,i);
		Lightmap[i] = R3GetPreTextureId(filename);
	}
*/

	stLightmapTex = R3GetTexInfoR3T(temp,0);	//R3T텍스쳐 정보를 리턴한다.
	stLightmap=LoadR3TLightMap(stLightmapTex,D3DFMT_R5G6B5);
	if( stLightmap == NULL )
		return;
	Lightmap_cnt=stLightmapTex->mTexNum;
	LightmapTexID = (int*)Dmalloc(sizeof(int)*Lightmap_cnt);
	for(i=0; i<Lightmap_cnt; i++)
	{
		LightmapTexID[i] = stLightmapTex->mStartID+i;
	}
}

void ReleaseLightMap()
{
	if( LightmapTexID )
	{
		Dfree(LightmapTexID);
		LightmapTexID= NULL;
		stLightmapTex=NULL;
	}
	if( stLightmap )
	{
		for(int i=0; i<Lightmap_cnt; i++)
		{
			Dfree(stLightmap[i]->buf);
			Dfree(stLightmap[i]);
		}
		Lightmap_cnt=0;
		Dfree(stLightmap);
		stLightmap=NULL;
	}
}

DWORD GetLightMapColor(Vector2f lgt_uv, int id)
{
	lgt_uv[0] = max(lgt_uv[0],0);
	lgt_uv[1] = max(lgt_uv[1],0);
	lgt_uv[0] = min(lgt_uv[0],1);
	lgt_uv[1] = min(lgt_uv[1],1);

	if( Lightmap_cnt == 0 )
		return 0xffffffff;
	int xpos = (int)(stLightmap[id]->xl*lgt_uv[0]);
	int ypos = (int)(stLightmap[id]->yl*lgt_uv[1]);

	WORD color = stLightmap[id]->buf[xpos + ypos * stLightmap[id]->xl];
	DWORD r_color = (((color>>11))*8)<<0;	
	DWORD g_color = (((color>>5)&63)*4)<<0;
	DWORD b_color = (((color>>0)&31)*8)<<0;

/*
//multiply가 최대 2x래서..
	// 2x비율낮추자...
	DWORD max_color=max(g_color,r_color);
	max_color=max(max_color,b_color);
	if( max_color > 255 )
	{
		float div_rate = 255/(float)max_color;
		r_color = (DWORD)(r_color*div_rate);
		g_color = (DWORD)(g_color*div_rate);
		b_color = (DWORD)(b_color*div_rate);
	}
*/

	return 0xff000000|(r_color<<16)|(g_color<<8)|(b_color);
}

void *GetLightMapSurface(int i)
{
	if( LightmapTexID )
	{
		return R3GetSurface(LightmapTexID[i]);
	}
	else
		return NULL;
}