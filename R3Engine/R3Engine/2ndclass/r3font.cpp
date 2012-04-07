#include <stdio.h>
#include <D3DX8.h>
#include "R3Font.h"
#include "jerror.h"
#include "jmalloc.h"

#define N_SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }


#define FONT_WIDTH			6//--9포인트 폰트일 경우 6픽셀이다.
#define FONT_HEIGHT			12//--9포인트 폰트일 경우 12픽셀이다.


CR3Font::CR3Font()
{
	strcpy(m_strFontName, "굴림체");
	m_dwFontHeight = 9;//--D3D예의상 초기화
	m_dwFontFlags = 0L;
	m_pd3dDevice = NULL;
	m_pTexture = NULL;
	m_dwSavedStateBlock = 0L;
	m_dwDrawTextStateBlock = 0L;
	mMemPtr=0;
}

CR3Font::~CR3Font()
{
}

void CR3Font::PrivateInit()
{

	m_dwOutLineColor=0;

	m_hbmBitmap=0;;
	m_hDC=0;
	m_hFont=0;
	m_pBitmapBits=0;
	mDeleteLine=0;

	BITMAPINFO bmi;
	ZeroMemory( &bmi.bmiHeader, sizeof(BITMAPINFOHEADER) );
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = (int)mMaxTextureXSize;//512
	bmi.bmiHeader.biHeight = -12;//--9포인트는 12픽셀
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biBitCount = 32;
	//--호환DC 생성
	m_hDC = CreateCompatibleDC( NULL );
	//--비트맵에 직접 쓰기 위해서 비트포인터를 설정
	m_hbmBitmap = CreateDIBSection( m_hDC, &bmi, DIB_RGB_COLORS, (void**)&m_pBitmapBits, NULL, 0 );
	if( m_hbmBitmap == NULL )
	{
		Warning("폰트초기화 실패","");
		return;
	}
	SetMapMode( m_hDC, MM_TEXT );

	int nHeight = -(int)(MulDiv( m_dwFontHeight, (int)(GetDeviceCaps(m_hDC, LOGPIXELSY)*m_fTextScale), 72 ));
	DWORD dwBold   = (m_dwFontFlags&D3DFONT_BOLD)   ? FW_BOLD : FW_NORMAL;
	DWORD dwItalic = (m_dwFontFlags&D3DFONT_ITALIC) ? TRUE    : FALSE;
	m_hFont = CreateFont( nHeight, 0, 0, 0, dwBold, dwItalic, FALSE, FALSE, DEFAULT_CHARSET
		, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, m_strFontName );
	if( m_hFont == NULL )//--폰트생성안되면 그냥리턴
		return;// E_FAIL;
	SelectObject( m_hDC, m_hbmBitmap );
	SelectObject( m_hDC, m_hFont );
	SetTextColor( m_hDC, RGB(255,255,255) );//--흰색
	SetBkColor( m_hDC, 0x00000000 );//--까만색
	SetTextAlign( m_hDC, TA_TOP );

	MemAllocate();
	ClearCache();
}

void CR3Font::MemAllocate()
{
	// 우린 인생한방이라 멜록도 한번에 하지..
	DWORD total_size = sizeof(DWORD)*(
		mFontMaxLines*mFontMaxStringLength		//시작점 
		+mFontMaxLines*mFontMaxStringLength		//문자열 길이.
		+mFontMaxLines							//해당라인별루 카운팅.
		+mFontMaxLines)						//해당 라인의 여분.
		+sizeof(char)*(mFontMaxLines*mFontMaxStringLength);	//실제 비교 문자닷.
	
	mMemPtr=(BYTE *)Dmalloc(total_size);		//대표메모리 얼로케이터.
	BYTE *temp = mMemPtr;
	m_Start=(DWORD *)temp;
	temp+=sizeof(DWORD)*mFontMaxLines*mFontMaxStringLength;
	m_Cnt=(DWORD *)temp;
	temp+=sizeof(DWORD)*mFontMaxLines*mFontMaxStringLength;
	m_StrCnt=(DWORD *)temp;
	temp+=sizeof(DWORD)*mFontMaxLines;
	m_LineCnt=(DWORD *)temp;
	temp+=sizeof(DWORD)*mFontMaxLines;
	m_CacheString=(char*)temp;

	/*
	DWORD m_Start[mFontMaxLines][mFontMaxStringLength];	//시작점.
	DWORD m_Cnt[mFontMaxLines][mFontMaxStringLength];		//문자열 길이.
	DWORD m_StrCnt[mFontMaxLines];	//해당라인별루 카운팅.
	DWORD m_LineCnt[mFontMaxLines];	//해당 라인의 여분.
	char m_CacheString[mFontMaxLines][mFontMaxStringLength];	//실제 비교 문자닷.
	*/
}

void CR3Font::MemFree()
{
	if( mMemPtr )
	{
		Dfree(mMemPtr);
		mMemPtr=0;
	}
}

void CR3Font::PrivateRelease()
{
	if( m_hbmBitmap )
		DeleteObject( m_hbmBitmap );
	if( m_hDC )
		DeleteDC( m_hDC );
	if( m_hFont )
		DeleteObject( m_hFont );
	ClearCache();
	MemFree();
}

HRESULT CR3Font::InitDeviceObjects( LPDIRECT3DDEVICE8 pd3dDevice,DWORD tex_xl,DWORD tex_yl )
{
	//--이 함수는 더이상 건드리지 말자
	HRESULT hr;//--리턴값체크용
	m_pd3dDevice = pd3dDevice;//--로칼디바이스를 복사한다.
	m_fTextScale = 1.0f;//--현재 지원 안하는 스케일
	D3DCAPS8 d3dCaps;
	m_pd3dDevice->GetDeviceCaps( &d3dCaps );
	/*
	if( m_dwTexWidth > d3dCaps.MaxTextureWidth )//--검사해서 현재 텍스처 지원안하면 낮춘다
	{
		m_fTextScale = (FLOAT)d3dCaps.MaxTextureWidth / (FLOAT)m_dwTexWidth;
		m_dwTexWidth = m_dwTexHeight = d3dCaps.MaxTextureWidth;
	}
	*/	
	//--텍스처 생성
	mMaxTextureXSize=tex_xl;
	mMaxTextureYSize=tex_yl;
	mFontMaxLines	=			((int)mMaxTextureYSize/(int)FONT_HEIGHT);
	mFontMaxStringLength	=	((int)mMaxTextureXSize/(int)FONT_WIDTH);

	hr = m_pd3dDevice->CreateTexture( mMaxTextureXSize, mMaxTextureYSize, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, &m_pTexture );
	if( FAILED( hr ) )
		return hr;
	return S_OK;
}
HRESULT CR3Font::RestoreDeviceObjects()
{
	for( int which=0; which<2; which++ )
	{
		m_pd3dDevice->BeginStateBlock();
		m_pd3dDevice->SetTexture( 0, m_pTexture );
//		if ( D3DFONT_ZENABLE & m_dwFontFlags )
//			m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
//		else
//			m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
		//--렌더상태 셋업
		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
		m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA );
		m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
		m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,        FALSE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		//--블로킹셋업
		if( which==0 )
			m_pd3dDevice->EndStateBlock( &m_dwSavedStateBlock );
		else
			m_pd3dDevice->EndStateBlock( &m_dwDrawTextStateBlock );
    }
	PrivateRelease();
	PrivateInit();
	return S_OK;
}

void CR3Font::PrepareDrawText()
{
	//--렌더상태 셋업
//	m_pd3dDevice->CaptureStateBlock( m_dwSavedStateBlock );
//	m_pd3dDevice->ApplyStateBlock( m_dwDrawTextStateBlock );

		m_pd3dDevice->SetTexture( 0, m_pTexture );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
		m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA );
		m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
		m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,        FALSE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	m_pd3dDevice->SetVertexShader( D3DFVF_FONT2DVERTEX );
	//--필터셋업
	if( m_dwFontFlags & D3DFONT_FILTERED )
	{
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	}
	m_dwFontVertexNum=0;
}

void CR3Font::DrawFullText(FONT2DVERTEX *pm_Vertices)
{
	m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, m_dwFontVertexNum/3, pm_Vertices, sizeof(FONT2DVERTEX) );
//	m_pd3dDevice->ApplyStateBlock( m_dwSavedStateBlock );//--저장된 상태를 로딩한다.

	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
}


HRESULT CR3Font::DrawText(float xyzw[4], DWORD dwColor, char* strText, DWORD dwFlags)
{
	if(!strcmp( strText, "" ))
		return S_OK;
	//--디바이스랑 텍스쳐가 없으면 볼일없다
	if( m_pd3dDevice == NULL || m_pTexture == NULL )
		return E_FAIL;

//	dwFlags |= D3DFONT_OUTLINE;
	char *ch_temp,ch_temp_buf[256];

	if( strlen(strText) >= mFontMaxStringLength )	//문자열 최대길이 제한
	{
		strncpy(ch_temp_buf,strText,mFontMaxStringLength);
		ch_temp_buf[mFontMaxStringLength]=NULL;
		ch_temp=ch_temp_buf;
	}
	else
		ch_temp = strText;

	PrepareDrawText();//미리 준비.

	m_dwFontFlags = dwFlags;//--플래그값 대입
	DWORD alpha_black_color = (dwColor&0xff000000)|m_dwOutLineColor;
	float sx  = xyzw[0];
	float sy  = xyzw[1];
	if( m_dwFontFlags & D3DFONT_SHADOW )
	{
		xyzw[0]=sx+1;		xyzw[1]=sy+1;
		FillIt(m_Vertices,xyzw, 0xff000000, ch_temp, m_dwFontFlags);
		xyzw[0]=sx;		xyzw[1]=sy;
		FillIt(m_Vertices,xyzw, dwColor, ch_temp, m_dwFontFlags);
	}
	else if( m_dwFontFlags & D3DFONT_OUTLINE )
	{
		xyzw[0]=sx+0;		xyzw[1]=sy+0;
		FillIt( m_Vertices,xyzw, alpha_black_color, ch_temp, m_dwFontFlags);
		xyzw[0]=sx+0;		xyzw[1]=sy+1;
		FillIt( m_Vertices,xyzw, alpha_black_color, ch_temp, m_dwFontFlags);
		xyzw[0]=sx+0;		xyzw[1]=sy+2;
		FillIt( m_Vertices,xyzw, alpha_black_color, ch_temp, m_dwFontFlags);
		xyzw[0]=sx+1;		xyzw[1]=sy+0;
		FillIt( m_Vertices,xyzw, alpha_black_color, ch_temp, m_dwFontFlags);
		xyzw[0]=sx+1;		xyzw[1]=sy+2;
		FillIt( m_Vertices,xyzw, alpha_black_color, ch_temp, m_dwFontFlags);
		xyzw[0]=sx+2;		xyzw[1]=sy+0;
		FillIt( m_Vertices,xyzw, alpha_black_color, ch_temp, m_dwFontFlags);
		xyzw[0]=sx+2;		xyzw[1]=sy+1;
		FillIt( m_Vertices,xyzw, alpha_black_color, ch_temp, m_dwFontFlags);
		xyzw[0]=sx+2;		xyzw[1]=sy+2;
		FillIt( m_Vertices,xyzw, alpha_black_color, ch_temp, m_dwFontFlags);
		xyzw[0]=sx+1;		xyzw[1]=sy+1;
		FillIt( m_Vertices,xyzw, dwColor, ch_temp, m_dwFontFlags);
	}
	else
	{
		FillIt( m_Vertices,xyzw, dwColor, ch_temp, m_dwFontFlags);
	}

	DrawFullText(m_Vertices);
	return S_OK;
}
                                                                                

HRESULT CR3Font::DrawText(float sx, float sy, DWORD dwColor, char* strText, DWORD dwFlags)
{	
	float xyzw[4];
	xyzw[0] = sx;
	xyzw[1] = sy;
	xyzw[2] = 0.99f;
	xyzw[3] = 1.0f;
	
	return DrawText( xyzw,  dwColor, strText, dwFlags);
}

HRESULT CR3Font::DrawText(float sx, float sy, DWORD dwColor, char* strText, float clipping[2][2], DWORD dwFlags)
{

	float xyzw[4];
	xyzw[0] = sx;
	xyzw[1] = sy;
	xyzw[2] = 0.99f;
	xyzw[3] = 1.0f;

	dwFlags |= D3DFONT_CLIPPING;
	m_fClipping[0][0] = clipping[0][0];
	m_fClipping[0][1] = clipping[0][1];
	m_fClipping[1][0] = clipping[1][0];
	m_fClipping[1][1] = clipping[1][1];
	return DrawText( xyzw,  dwColor, strText, dwFlags);
}

HRESULT CR3Font::DeleteDeviceObjects()
{
	//--생성된 오브젝트 지움
	N_SAFE_RELEASE( m_pTexture );
	m_pd3dDevice = NULL;
	return S_OK;
}

HRESULT CR3Font::InvalidateDeviceObjects()
{
//	N_SAFE_RELEASE( m_pVB );
	//--상태 블록을 지우자
	if( m_pd3dDevice )
	{
		if( m_dwSavedStateBlock )
			m_pd3dDevice->DeleteStateBlock( m_dwSavedStateBlock );
		if( m_dwDrawTextStateBlock )
			m_pd3dDevice->DeleteStateBlock( m_dwDrawTextStateBlock );
	}
	m_dwSavedStateBlock    = 0L;
	m_dwDrawTextStateBlock = 0L;
	PrivateRelease();
	return S_OK;
}



void CR3Font::ClearCache()
{
	if( !mMemPtr )	//메모리 할당이 안되있음 안되지..
		return;
	memset(m_StrCnt,0,sizeof(DWORD)*mFontMaxLines);
	memset(m_LineCnt,0,sizeof(DWORD)*mFontMaxLines);
	memset(m_Cnt,0,sizeof(DWORD)*mFontMaxStringLength*mFontMaxLines);
}

//str_leng은 NULL을 제외한 숫자
void CR3Font::GetBestPosCache(DWORD str_leng,DWORD *x_index,DWORD *y,DWORD *start)
{
	DWORD i,j,cnt;

	for(j=0; j<mFontMaxLines; j++)		//일반적인 공백을 찾는다.
	{
		cnt=0;
		for(i=0; i<m_StrCnt[j]; i++)		//일반적인 공백을 찾는다.
		{
//			cnt += m_Cnt[j*mFontMaxLines+i];
			cnt += m_Cnt[j*mFontMaxStringLength+i];
		}
		//
		if( (int)(mFontMaxStringLength - cnt) >= str_leng )	//넣는 공간이 된다. 찾았다.
		{
			*y = j;
			*x_index = i;
			*start = cnt;
			return;
		}
	}
	//넣을만한곳이 없다.꽉찻으니 순서대로 한 라인을 날린다.
	memset(m_Cnt+mDeleteLine*mFontMaxStringLength,0,sizeof(DWORD)*mFontMaxStringLength);
	m_StrCnt[mDeleteLine]=0;		//카운팅을 날리고.
	m_LineCnt[mDeleteLine]=0;		//라인의 문자열수 날리고 
	*start = 0;			//시작점도 당연히 처음이고..

//	for(i=0; i<m_StrCnt[mDeleteLine]; i++)
//		m_Cnt[mDeleteLine][i]=0;
	*x_index = 0;
	*y = mDeleteLine;
	mDeleteLine++;
	mDeleteLine %= mFontMaxLines;
}

inline static BOOL _StrCmp(char *src,char *tar,DWORD cnt)
{
	DWORD i;
	for(i=0; i<cnt; i++)
	{
		if( src[i] != tar[i] )
			return TRUE;
	}
	return FALSE;	//strcmp하고 같게 하기위해서
}

BOOL CR3Font::IsExistCache(char *ch,DWORD str_leng,DWORD *x_index,DWORD *y)
{
	DWORD i,j,cnt;

	for(j=0; j<mFontMaxLines; j++)		//일반적인 공백을 찾는다.
	{
//		if( mFontMaxStringLength - m_LineCnt[j] < str_leng)
//			continue;
		cnt=0;
		for(i=0; i<m_StrCnt[j]; i++)		//일반적인 공백을 찾는다.
		{
			if( str_leng != m_Cnt[j*mFontMaxStringLength+i] )	//문자열개수랑 틀리면 아에 리턴
				continue;
			if( !_StrCmp(ch,&m_CacheString[j*mFontMaxStringLength+m_Start[j*mFontMaxStringLength+i]],m_Cnt[j*mFontMaxStringLength+i] ) )
			{                        
				*y = j;             
				*x_index = i;
				return TRUE;
			}
		}
	}
	return FALSE;
}

void CR3Font::SetCache(char *strText,DWORD str_leng,DWORD x_index,DWORD y,DWORD start)
{
	DWORD i,j;

	//캐쉬 버퍼및 문자열 갯수 넣어주고.
	m_Start[y*mFontMaxStringLength+x_index] = start;
	DWORD x = m_Start[y*mFontMaxStringLength+x_index];
	m_Cnt[y*mFontMaxStringLength+x_index] = str_leng;
	m_LineCnt[y]+=str_leng;			//해당 라인 문자열 개수누적
	m_StrCnt[y]++;		//카운팅을 늘려야지..
	for(i=0; i<str_leng; i++)
		m_CacheString[y*mFontMaxStringLength+x+i]=strText[i];

	ExtTextOut( m_hDC, 0, 0, ETO_OPAQUE, NULL, strText, str_leng, NULL );

	//--서페이스에 락 걸고 직접 픽셀값을 세팅한다.
	D3DLOCKED_RECT d3dlr;
	RECT rcLock;
	rcLock.left = x * 6;
	rcLock.top = y * 12;
	rcLock.right = (x + str_leng)*6;
	rcLock.bottom = ( y+1 )*12;
	
	//--락건다. rcLock영역 만큼. rcLock == NULL 이면 전체 영역
	HRESULT tt= m_pTexture->LockRect( 0, &d3dlr, &rcLock, 0 );
//	m_pTexture->LockRect( 0, &d3dlr, 0, 0 );
	if( tt != D3D_OK )
		return;

	BYTE* pDstRow = (BYTE*)d3dlr.pBits;//--락걸린 비트들. void* -> BYTE*형으로 캐스팅.
	WORD* pDst16;
	BYTE bAlpha;//--4비트 알파

	for( i=0; i < 12; i++ )
	{
		pDst16 = (WORD*)pDstRow+0;//--다시 WORD*(2바이트)로 캐스팅
		for( j=0; j < str_leng*6; j++ )
		{
			//--m_dwTexWidth로 해야한다. Bitmap크기가 m_dwTexWidth * 12이므로
			bAlpha = (BYTE)((m_pBitmapBits[ mMaxTextureXSize * i + j ] & 0xff) >> 4);//--알파값이 4비트 이므로
            if (bAlpha > 0)
            {
                *pDst16++ = (bAlpha << 12) | 0x0fff;
            }
            else
            {
				*pDst16++ = 0x0000;
            }
        }
        pDstRow += d3dlr.Pitch;
    }
    //--이제 세팅다했으니 지우자
    m_pTexture->UnlockRect(0);

}

HRESULT CR3Font::FillIt(FONT2DVERTEX *pm_Vertices,float xyzw[4], DWORD dwColor, char* strText, DWORD dwFlags)
{
	DWORD str_leng = strlen(strText);
	
	if( !str_leng )	//개수가 0도 성공은 성공이지.
		return S_OK;

	DWORD x,x_index,y;
	if( IsExistCache(strText,str_leng,&x_index,&y) ) //캐쉬에 있다면.
	{
	}
	else	//캐쉬에 없어서.
	{
		GetBestPosCache(str_leng,&x_index,&y,&x);	//가장 적당한 곳을 리턴받고
		SetCache(strText,str_leng,x_index,y,x);	//캐쉬에 쓰고 라이팅을 한다.
	}
	x = m_Start[y*mFontMaxStringLength+x_index];		//인덱스에 있는 실제 카운트


	float fDetCoord[4];
	fDetCoord[0] = ((x*6.0f))/(float)mMaxTextureXSize;
	fDetCoord[1] = ((y*12.0f))/(float)mMaxTextureYSize;
	fDetCoord[2] = ((x+str_leng)*6.0f)/(float)mMaxTextureXSize;
	fDetCoord[3] = ((y+1)*12.0f)/(float)mMaxTextureYSize;

	float fStartX = xyzw[0];
	
	DWORD dwNumTriangles = 0;

	float tx1, tx2, ty1, ty2, w, h;//--텍스처 좌표(Rect), Width, Height

	if( m_dwFontFlags & D3DFONT_CLIPPING )
	{
		tx1 = fDetCoord[0] + ( fDetCoord[2] - fDetCoord[0] ) * m_fClipping[0][0];
		ty1 = fDetCoord[1] + ( fDetCoord[3] - fDetCoord[1] ) * m_fClipping[1][0];
		tx2 = fDetCoord[0] + ( fDetCoord[2] - fDetCoord[0] ) * m_fClipping[0][1];
		ty2 = fDetCoord[1] + ( fDetCoord[3] - fDetCoord[1] ) * m_fClipping[1][1];
	}
	else
	{
		tx1 = fDetCoord[0];
		ty1 = fDetCoord[1];
		tx2 = fDetCoord[2];
		ty2 = fDetCoord[3];
	}
	
	w = (tx2-tx1)*(float)mMaxTextureXSize;
	h = (ty2-ty1)*(float)mMaxTextureYSize;

//--그냥 초기화 하는 함수
#define InitFont2DVertex(xx,yy,zz,ww,color1,tuu,tvv){		pm_Vertices[m_dwFontVertexNum].p.x = xx;		pm_Vertices[m_dwFontVertexNum].p.y = yy;		pm_Vertices[m_dwFontVertexNum].p.z = zz;	pm_Vertices[m_dwFontVertexNum].p.w = ww;		pm_Vertices[m_dwFontVertexNum].color = color1;		pm_Vertices[m_dwFontVertexNum].tu = tuu;		pm_Vertices[m_dwFontVertexNum].tv = tvv;}

	InitFont2DVertex( xyzw[0]+0-0.5f,xyzw[1]+h-0.5f,xyzw[2],xyzw[3], dwColor, tx1, ty2 );
	m_dwFontVertexNum++;
	InitFont2DVertex( xyzw[0]+0-0.5f,xyzw[1]+0-0.5f,xyzw[2],xyzw[3], dwColor, tx1, ty1 );
	m_dwFontVertexNum++;
	InitFont2DVertex( xyzw[0]+w-0.5f,xyzw[1]+h-0.5f,xyzw[2],xyzw[3], dwColor, tx2, ty2 );
	m_dwFontVertexNum++;
	InitFont2DVertex( xyzw[0]+w-0.5f,xyzw[1]+0-0.5f,xyzw[2],xyzw[3], dwColor, tx2, ty1 );
	m_dwFontVertexNum++;
	InitFont2DVertex( xyzw[0]+w-0.5f,xyzw[1]+h-0.5f,xyzw[2],xyzw[3], dwColor, tx2, ty2 );
	m_dwFontVertexNum++;
	InitFont2DVertex( xyzw[0]+0-0.5f,xyzw[1]+0-0.5f,xyzw[2],xyzw[3], dwColor, tx1, ty1 );
	m_dwFontVertexNum++;
	return S_OK;
}
