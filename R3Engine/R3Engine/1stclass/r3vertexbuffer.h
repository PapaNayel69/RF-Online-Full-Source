#ifndef __R3VERTEXBUFFER_H__
#define __R3VERTEXBUFFER_H__

#include "R3d3d8.h"


#define _USE_DYNAMIC_VP		0x00000001
#define _USE_STATIC_VP		0x00000002
#define _USE_MEMORY_VP		0x00000003		//그냥 메모리에 얹는다.

//---------------------버텍스 버퍼 관련 클래스...
class CVertexBuffer{
private:
	DWORD m_Flag;
	DWORD m_Size;	//버텍스 버퍼 총사이즈
public:
	union{
	LPDIRECT3DVERTEXBUFFER8 m_lpVertexBuffer;
	_D3DR3VERTEX_TEX1 *m_VertexBufferTex1;	//메모리상의 버텍스 버퍼.
	_D3DR3VERTEX_TEX2 *m_VertexBufferTex2;	//메모리상의 버텍스 버퍼.
	};
	CVertexBuffer();
	~CVertexBuffer();
//	void *GetVertexBuffer();
	void InitVertexBuffer(int size,int flag,DWORD fvf=0);
	void ReleaseVertexBuffer();
	BYTE* VPLock(int start=0,int size=0,DWORD option=0);
	void VPUnLock();
	DWORD GetVertexBufferSize()		{	return m_Size;	};
};

class CIndexBuffer{
private:
	DWORD m_Flag;
	DWORD m_Size;	//인덱스 버퍼 총사이즈
public:
	union{
	LPDIRECT3DINDEXBUFFER8 m_lpIndexBuffer;
	WORD *m_IndexBuffer;	//메모리상의 버텍스 버퍼.
	};
	CIndexBuffer();
	~CIndexBuffer();
	void InitIndexBuffer(int size,int flag=_USE_STATIC_VP);
	void ReleaseIndexBuffer();
	BYTE* VPLock(int start=0,int size=0,DWORD option=0);
	void VPUnLock();
	DWORD GetVertexBufferSize()		{	return m_Size;	};
};

//버텍스 버퍼불필요한 셋팅을 막기위한 캐쉬.
void ResetVertexBufferCache();	//렌더링 구조에서 시작할때 반드시 호출하자.. 
void SetVPIPTex1(void *vb,void *ib);
void SetVPIPTex2(void *vb,void *ib);
void SetVPIPTex1IndexPrimitiveTL(void *vb,void *ib);
void SetVPIPTex1IndexPrimitive(void *vb,void *ib);


//버텍스 버퍼 정보.
void ResetTotalVertexBufferInfo();
DWORD GetTotalVertexBufferSize();	//인덱스버퍼도 포함.
DWORD GetTotalVertexBufferCnt();		//인덱스 버퍼포함.


#endif