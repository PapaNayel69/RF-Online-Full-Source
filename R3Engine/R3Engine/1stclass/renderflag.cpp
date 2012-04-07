#include "RenderFlag.h"
#include "jmalloc.h"
#include "jerror.h"
#include <stdio.h>

#define _RENDERSTATE_SIZE	174	//최대 .
#define _RENDERSTAGE_SIZE	29	//최대 .

#define _MAX_TEXTURE_STAGE	8	//최대 8개.

//#define _OPTIMIZE
#ifndef _OPTIMIZE
#endif

static DWORD stTextureStage[_MAX_TEXTURE_STAGE][_RENDERSTAGE_SIZE]=
{
	0,4,2,1,2,2,1,0,0,0,0,0,0,1,1,0,1,1,0,0,0,1,0,0,0,1,1,1,1,
	0,1,2,1,1,2,1,0,0,0,0,1,0,1,1,0,1,1,0,0,0,1,0,0,0,1,1,1,1,
	0,1,2,1,1,2,1,0,0,0,0,2,0,1,1,0,1,1,0,0,0,1,0,0,0,1,1,1,1,
	0,1,2,1,1,2,1,0,0,0,0,3,0,1,1,0,1,1,0,0,0,1,0,0,0,1,1,1,1,
	0,1,2,1,1,2,1,0,0,0,0,4,0,1,1,0,1,1,0,0,0,1,0,0,0,1,1,1,1,
	0,1,2,1,1,2,1,0,0,0,0,5,0,1,1,0,1,1,0,0,0,1,0,0,0,1,1,1,1,
	0,1,2,1,1,2,1,0,0,0,0,6,0,1,1,0,1,1,0,0,0,1,0,0,0,1,1,1,1,
	0,1,2,1,1,2,1,0,0,0,0,7,0,1,1,0,1,1,0,0,0,1,0,0,0,1,1,1,1,
};

static DWORD stRenderState[_RENDERSTATE_SIZE]=
{
	0,0,0,0,0,0,0,1,3,2,0,0,0,0,1,0,1,0,0,2,1,0,3,4,0,8,0,0,0,0,0,0,
	0,0,0,0,0,1065353216,1065353216,0,0,0,-1163015426,0,0,0,0,0,0,0,-1163015426,0,0,1,1,1,8,0,-1,-1,-1,-1163015426,-1163015426,-1163015426,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	-1163015426,-1163015426,-1163015426,-1163015426,-1163015426,-1163015426,-1163015426,-1163015426,-1163015426,-1163015426,-1163015426,-1163015426,-1163015426,-1163015426,-1163015426,-1163015426,-1163015426,-1163015426,-1163015426,-1163015426,-1163015426,-1163015426,-1163015426,-1163015426,-1163015426,-1163015426,-1163015426,-1163015426,-1163015426,-1163015426,-1163015426,-1163015426,
	0,0,0,0,0,0,0,0,1,1,-1163015426,0,0,1,1,0,0,1,2,0,0,-1163015426,-1163015426,0,0,0,1065353216,0,0,0,1065353216,0,
	0,1,-1,0,1065353216,-1163015426,1115684864,0,15,-1163015426,0,1,3,1,
};




//static DWORD stRStateA[_RENDERSTATE_SIZE],stRStateB[_RENDERSTATE_SIZE];
void SaveRenderState()
{
	int i=0;

	DWORD *table = stRenderState;
/*
//	return;
	i=1;
	GetD3dDevice()->GetTextureStageState(0,D3DTSS_COLOROP               ,&table[i++]);
	GetD3dDevice()->GetTextureStageState(0,D3DTSS_COLORARG1             ,&table[i++]);
	GetD3dDevice()->GetTextureStageState(0,D3DTSS_COLORARG2             ,&table[i++]);
	GetD3dDevice()->GetTextureStageState(0,D3DTSS_ALPHAOP               ,&table[i++]);
	GetD3dDevice()->GetTextureStageState(0,D3DTSS_ALPHAARG1             ,&table[i++]);
	GetD3dDevice()->GetTextureStageState(0,D3DTSS_ALPHAARG2             ,&table[i++]);
//	i=1;
	GetD3dDevice()->GetTextureStageState(1,D3DTSS_COLOROP               ,&table[i++]);
	GetD3dDevice()->GetTextureStageState(1,D3DTSS_COLORARG1             ,&table[i++]);
	GetD3dDevice()->GetTextureStageState(1,D3DTSS_COLORARG2             ,&table[i++]);
	GetD3dDevice()->GetTextureStageState(1,D3DTSS_ALPHAOP               ,&table[i++]);
	GetD3dDevice()->GetTextureStageState(1,D3DTSS_ALPHAARG1             ,&table[i++]);
	GetD3dDevice()->GetTextureStageState(1,D3DTSS_ALPHAARG2             ,&table[i++]);
//	GetD3dDevice()->GetTextureStageState(0,D3DTSS_TEXCOORDINDEX         ,&table[i++]);
*/
	for(i=0; i<_RENDERSTATE_SIZE; i++)
	{
		GetD3dDevice()->GetRenderState((_D3DRENDERSTATETYPE)i,&table[i]);
	}
	FILE *fp;
	fp = fopen("renderstate.txt","wt");
	for(i=0; i<_RENDERSTATE_SIZE; i++)
	{
		fprintf(fp,"%d,",table[i]);
		if( (i%32) == 31 )
			fprintf(fp,"\n");
	}
//		fprintf(fp,"%d = %d\n",i,table[i]);

	fclose(fp);
}
void SaveTextureStage()
{
	int i,j;

	for(i=1; i<_RENDERSTAGE_SIZE; i++)
	{
		for(j=0; j<_MAX_TEXTURE_STAGE; j++)
			GetD3dDevice()->GetTextureStageState(j,(D3DTEXTURESTAGESTATETYPE)i,&stTextureStage[j][i]);
	}
	FILE *fp;
	fp = fopen("texturestage.txt","wt");

	for(j=0; j<_MAX_TEXTURE_STAGE; j++)
	{
		for(i=0; i<_RENDERSTAGE_SIZE; i++)
			fprintf(fp,"%d,",stTextureStage[j][i]);
		fprintf(fp,"\n");
	}
	fclose(fp);
}


HRESULT R3SetRenderState(D3DRENDERSTATETYPE type, DWORD state)
{
#ifndef _OPTIMIZE
	if( !GetD3dDevice() )
		return !S_OK;
#endif

	if( stRenderState[type] == state )
		return S_OK;
	stRenderState[type]=state;
	
	return GetD3dDevice()->SetRenderState( type, state );
}


HRESULT R3SetTextureStageState(DWORD stage,D3DTEXTURESTAGESTATETYPE type, DWORD state)
{
#ifndef _OPTIMIZE
	if( !GetD3dDevice() )
		return !S_OK;
#endif

	if( stTextureStage[stage][type] == state )
		return S_OK;
	stTextureStage[stage][type]=state;

	return GetD3dDevice()->SetTextureStageState( stage, type, state );
}



