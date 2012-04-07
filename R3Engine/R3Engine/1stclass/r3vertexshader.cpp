#include "d3d8.h"
#include "d3dx8.h"
#include <stdio.h>

#include "CommonUtil.h"
#include "Core.h"
#include "d3dutil.h"
#include "jerror.h"

//--------------------------------------------------------------------------------------
//버텍스 쉐이더 레지스터 inupt 구조.   쉐이더에서 안쓰면 넣지 않는게 기본...
// c0-c3 매트릭스 1 
// c4-c7 매트릭스 2
// c8	포그
// c12- 버텍스 쉐이더의 상황에 따른 셋팅.

DWORD stGrassVertexShader;
DWORD stEntityVertexDecl[] =
{
    D3DVSD_STREAM( 0 ),
    D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // Position of first mesh
    D3DVSD_REG( 3, D3DVSDT_D3DCOLOR ), // DWORD color
    D3DVSD_REG( 4, D3DVSDT_FLOAT2 ), // Tex coords
    D3DVSD_END()
};
DWORD stDetailTexVertexShader;
DWORD stDetailTexVertexDecl[] =
{
    D3DVSD_STREAM( 0 ),
    D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // Position of first mesh
    D3DVSD_REG( 3, D3DVSDT_FLOAT3 ), // Normal
    D3DVSD_REG( 6, D3DVSDT_D3DCOLOR ), // DWORD color
    D3DVSD_REG( 7, D3DVSDT_FLOAT2 ), // Tex coords
    D3DVSD_REG( 9, D3DVSDT_FLOAT2 ), // Tex coords 2
    D3DVSD_END()
};


#define SHADERTYPE_VERTEX		1
#define SHADERTYPE_PIXEL		2

HRESULT LoadCreateShader(char *strFilePath, const DWORD* pDeclaration, DWORD Usage, int ShaderType, DWORD* pHandle)
{
	if( !GetD3dDevice() )
		return E_FAIL;
	try
	{
		HANDLE hFile;
		HRESULT hr;

		hFile = CreateFile(strFilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			Warning("Could not find file ",strFilePath);
			return E_FAIL;
		}

		DWORD dwFileSize = GetFileSize(hFile, NULL);

		const DWORD* pShader = (DWORD*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwFileSize);
		if (!pShader)
		{
			Warning("Failed to allocate memory to load shader ", strFilePath);
			return E_FAIL;
		}

		ReadFile(hFile, (void*)pShader, dwFileSize, &dwFileSize, NULL);

		CloseHandle(hFile);

		if (ShaderType == SHADERTYPE_VERTEX)
		{
			hr = GetD3dDevice()->CreateVertexShader(pDeclaration, pShader, pHandle, Usage);
		}
		else if (ShaderType == SHADERTYPE_PIXEL)
		{
			hr = GetD3dDevice()->CreatePixelShader(pShader, pHandle);
		}

		HeapFree(GetProcessHeap(), 0, (void*)pShader);

		if (FAILED(hr))
		{
			Warning("Failed to create shader ", strFilePath);; 
			return E_FAIL;
		}
	}
	catch(...)
	{
		Warning("Error opening file ", strFilePath);
		return E_FAIL;
	}

	return S_OK;
}

BOOL LoadVertexShaderList()
{
	HRESULT hr;


	if( IsExistFile(".\\System\\shader\\Grass1.vso") )
	{
		HRESULT result = LoadCreateShader(".\\System\\shader\\Grass1.vso"
			,stEntityVertexDecl,0,SHADERTYPE_VERTEX, &stGrassVertexShader);
		if( result != S_OK )
			return FALSE;
	}
	else
    if( FAILED( hr = D3DUtil_CreateVertexShader( GetD3dDevice(), ".\\System\\shader\\Grass1.vsh",
                                                 stEntityVertexDecl,
                                                 &stGrassVertexShader ) ) )
    {
        return FALSE;
    }

	if( IsExistFile(".\\System\\shader\\DetailTex.vso") )
	{
		HRESULT result = LoadCreateShader(".\\System\\shader\\DetailTex.vso"
			,stDetailTexVertexDecl,0,SHADERTYPE_VERTEX, &stDetailTexVertexShader);
		if( result != S_OK )
			return FALSE;
	}
	return TRUE;
}

void SetWorldViewMatrixVS(float world_mat[4][4])
{
	_R3ENGINE_STATE *state = GetR3State();

	D3DXMATRIX matViewTranspose, matProjTranspose,temp;
	temp = *((D3DXMATRIX*)world_mat)*state->mMatView;
    D3DXMatrixTranspose(&matViewTranspose, &temp);
    GetD3dDevice()->SetVertexShaderConstant( 0, &matViewTranspose,  4 );
}

void SetGrassVS(float add_frame,float factor[2],float alpha,DWORD map_color)
{
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();
	_R3ENGINE_STATE *state = GetR3State();
	float FogArray[4] = {state->mFogStart,state->mFogEnd,1.0f/(state->mFogEnd-state->mFogStart)};

	D3DXMATRIX matViewTranspose, matProjTranspose,temp;
    D3DXMatrixTranspose(&matProjTranspose, &state->mMatProj);

    // Set the vertex shader constants
    pd3dDevice->SetVertexShaderConstant( 4, &matProjTranspose,  4 );
	pd3dDevice->SetVertexShader( stGrassVertexShader );

	pd3dDevice->SetVertexShaderConstant( 8, FogArray, 1 );

	float time = R3GetTime();
//	time =time-(DWORD)time;
	time=time+add_frame/10.f;
	time=(float)cos(time*factor[0])*0.2f*factor[1];

	float factor2[4],factor3[4];
	factor2[0]=time;
	factor2[1]=alpha;
	factor2[2]=0;
	factor2[3]=1;
	pd3dDevice->SetVertexShaderConstant( 9, factor2, 1 );
 
	factor3[3]=1;
	factor3[0]=((map_color>>16)&0xff)/128.0f;
	factor3[1]=((map_color>>8)&0xff)/128.0f;
	factor3[2]=((map_color>>0)&0xff)/128.0f;
	pd3dDevice->SetVertexShaderConstant( 10, factor3, 1 );	//바닥컬러.
}

void SetDetailTexVS(D3DXMATRIX *tex_mat)
{
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();
	_R3ENGINE_STATE *state = GetR3State();
	float FogArray[4] = {state->mFogStart,state->mFogEnd,1.0f/(state->mFogEnd-state->mFogStart)};

	D3DXMATRIX matViewTranspose, matProjTranspose,temp;

	pd3dDevice->SetVertexShader( stDetailTexVertexShader );
    
    D3DXMatrixTranspose(&matViewTranspose, &state->mMatView);
    GetD3dDevice()->SetVertexShaderConstant( 0, &matViewTranspose,  4 );

	D3DXMatrixTranspose(&matProjTranspose, &state->mMatProj);
    pd3dDevice->SetVertexShaderConstant( 4, &matProjTranspose,  4 );

/*
	float tex_mat[4][4];
	for(DWORD i=0; i<3; i++)
	{
		for(DWORD j=0; j<3; j++)
			tex_mat[i][j] = tex_mat[i][j];
		tex_mat[i][3] = 0;
	}
	for(DWORD i=0; i<3; i++)
		tex_mat[3][i] = 0;
	tex_mat[3][3] = 1;
    D3DXMatrixTranspose(&matProjTranspose, (D3DXMATRIX *)tex_mat);
*/
    D3DXMatrixTranspose(&matProjTranspose, tex_mat);
    pd3dDevice->SetVertexShaderConstant( 8, &matProjTranspose,  8 );

	float factor2[4];
	factor2[2]=0;
	factor2[3]=1;
	pd3dDevice->SetVertexShaderConstant( 13, factor2, 1 );
	pd3dDevice->SetVertexShaderConstant( 12, FogArray, 1 );
}

void ReleaseVertexShaderList()
{
    if( stGrassVertexShader != 0 )
    {
        GetD3dDevice()->DeleteVertexShader( stGrassVertexShader );
        stGrassVertexShader = 0;
    }

    if( stDetailTexVertexShader != 0 )
    {
        GetD3dDevice()->DeleteVertexShader( stDetailTexVertexShader );
        stDetailTexVertexShader = 0;
    }
}

