////////////////////////////////////////////////////////////////////////////
//
// Global data and function Implementation
//
////////////////////////////////////////////////////////////////////////////

#include "UIGlobal.h"
#include "GUIController/GUIObject.h"
#include "GameWindow/GUICharInfoWindow.h"

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////        Boundary          ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////
// 아바타 정보 전역 함수
CGUICharInfoWindow *	g_pCharInfoWindow = NULL;
BYTE g_byAvatarRaceType = 0;

void
_SetUIAvatarRace( BYTE pi_byRaceType )
{
	g_byAvatarRaceType = pi_byRaceType;
}

void	
_SetCharInfoWindow( CGUICharInfoWindow * pi_pWin )
{
	g_pCharInfoWindow = pi_pWin;
}

BYTE
_GetAvatarRace( void )
{
	return g_byAvatarRaceType;
}

WORD
_GetAvatarLevel( void )
{
	if( g_pCharInfoWindow )
		return g_pCharInfoWindow->GetLevel();
	
	return 0;
}

static char s_pMoneyName[3][8] = { "달란트", "디세나", "CP" };

char *
_GetAvatarMoneyName( void )
{
	BYTE byRace = g_byAvatarRaceType / 2;
	if( byRace > 2 )
		byRace = 0;

	return s_pMoneyName[byRace];
}

char *
_GetMoneyName( BYTE pi_byRaceType )
{
	BYTE byRace = pi_byRaceType / 2;
	if( byRace > 2 )
		byRace = 0;

	return s_pMoneyName[byRace];
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////        Boundary          ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////
// 거래, 장착 관련 데이터
TRADE_ITEM_INFO		g_sHoverItemInfo;					// 거래같은 아이템 처리에서 마우스에 붙은 아이템의 정보가 필요하다.
BOOL				g_bIsQuickEvent_Equip	= FALSE;	// ctrl key를 눌렀을 때 마우스에 붙이지 않고 바로 처리하기 위해
BOOL				g_bIsQuickEvent_Trade	= FALSE;	// shitf key를 눌렀을 때 마우스에 붙이지 않고 바로 처리하기 위해

// 거래나 장착시 단축키를 눌렀는지 체크한다.
void
SetQuickEvent( void )
{
	if( GetKeyState( VK_CONTROL ) & 0xf000 )
		g_bIsQuickEvent_Equip = TRUE;
	else
		g_bIsQuickEvent_Equip = FALSE;

	if( GetKeyState( VK_SHIFT ) & 0xf000 )
		g_bIsQuickEvent_Trade = TRUE;					
	else
		g_bIsQuickEvent_Trade = FALSE;
}


////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////        Boundary          ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////


// 숫자를 가격표시 문자열로 바꾼다
void
_GetPriceStr( DWORD pi_dwPrice, char * po_pPriceStr )
{
	if( !po_pPriceStr )
		return;

	char buf[32];	

	_ultoa( pi_dwPrice, buf, 10 );		

	int len = strlen( buf );	
	int l_nUpperNum = len % 3; // 맨위 콤마(,) 앞자리 갯수
	int l_nCommaNum = len / 3; // 콤마 갯수

	// 콤마 앞자리 숫자 copy
	if( l_nUpperNum )
	{
		strncpy( po_pPriceStr, buf, l_nUpperNum );
		po_pPriceStr[l_nUpperNum] = '\0';
	}
	else
	{
		po_pPriceStr[0] = '\0';
	}

	
	// 3개씩 중간에 콤마를 삽입한다
	for( int i=0; i<l_nCommaNum; ++i )
	{		
		if( i != 0 || l_nUpperNum )
			strcat( po_pPriceStr, "," );
		strncat( po_pPriceStr, buf+l_nUpperNum+i*3, 3 );
	}	
}

// 가격표시 문자열을 숫자로
DWORD
_GetPriceNum( char * pi_pPriceStr )
{	
	char	buf[32];
	int		i, j;	

	// 콤마(,)를 뺀 숫자만 걸러낸다
	for( i=0, j=0; pi_pPriceStr[i] != '\0'; ++i )
	{
		if( pi_pPriceStr[i] != ',' )		
			buf[j++] = pi_pPriceStr[i];
	}
	buf[j] = '\0';
	
	return atol( buf );
}


////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////        Boundary          ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

#pragma comment ( lib, "ABuse.lib" )
#pragma comment ( lib, "Adv.lib" )

// 욕사전 함수
extern "C" __declspec(dllimport) int	ABuse_LoadDic(char *);		//욕사전 로딩
extern "C" __declspec(dllimport) BOOL	ABuse_InitDic();			//욕사전 초기화
extern "C" __declspec(dllimport) void	ABuse_EncodeDic();			//사전 엔코딩
extern "C" __declspec(dllimport) BOOL	ABuse_DecodeDic();			//사전 디코딩
extern "C" __declspec(dllimport) BOOL	ABuse_CheckAbuse(char *);	//욕포함 유뮤 
extern "C" __declspec(dllimport) double	ABuse_Emotion();			//감정 평가
extern "C" __declspec(dllimport) double	ABuse_Fuzzy();				//감정 평가용 퍼지


// 광고 사전 함수
extern "C" __declspec(dllimport) int	Adv_LoadDic(char *);		//광고사전 로딩
extern "C" __declspec(dllimport) BOOL	Adv_InitDic();				//광고사전 초기화
extern "C" __declspec(dllimport) void	Adv_EncodeDic();			//광고 엔코딩
extern "C" __declspec(dllimport) BOOL	Adv_DecodeDic();			//광고 디코딩
extern "C" __declspec(dllimport) BOOL	Adv_CheckAbuse(char *);		//광고포함 유뮤 
extern "C" __declspec(dllimport) double	Adv_Emotion();				//감정 평가
extern "C" __declspec(dllimport) double	Adv_Fuzzy();				//감정 평가용 퍼지

static BOOL s_bIsAbuseLoaded = FALSE;
// 욕 데이터 로딩
BOOL
_LoadAbuseFilter( char * pi_pAbuseDataFilename )
{
	if( s_bIsAbuseLoaded )
		return TRUE;

	// 사전 로딩
	ABuse_LoadDic( pi_pAbuseDataFilename );


	// 해킹 여부 판단
	if( ABuse_DecodeDic() ) 
	{
		// 사전 초기화
		ABuse_InitDic();

		s_bIsAbuseLoaded = TRUE;

		return TRUE;
	}
	else 
	{
		return FALSE;
	}
}

static BOOL s_bIsAdvLoaded = FALSE;
// 광고 데이터 로딩
BOOL
_LoadAdvFilter( char *pi_pAdvDataFilename )
{
	if( s_bIsAdvLoaded )
		return TRUE;

	// 사전 로딩
	Adv_LoadDic( pi_pAdvDataFilename );


	// 해킹 여부 판단
	if( Adv_DecodeDic() ) 
	{
		// 사전 초기화
		Adv_InitDic();

		s_bIsAdvLoaded = TRUE;

		return TRUE;
	}
	else 
	{
		return FALSE;
	}
}

// 문자열에 욕이 포함되었는지 체크한다.
BOOL
_IsAbuseStr( char * pi_pStr )
{
	if( !s_bIsAbuseLoaded || !pi_pStr )
		return FALSE;

	return ABuse_CheckAbuse( pi_pStr );
}

// 문자열에 광고가 포함되었는지 체크한다.
BOOL
_IsAdvStr( char * pi_pStr )
{
	if( !s_bIsAdvLoaded || !pi_pStr )
		return FALSE;

	return Adv_CheckAbuse( pi_pStr );
}


// 캐릭터 이름으로 사용할수 있는지 체크한다.
BOOL
_IsAvailableCharName( char * pi_pName, BYTE pi_byUserDegree )
{
	if( !strlen( pi_pName ) )
		return FALSE;

	// 욕 필터링
	if( _IsAbuseStr( pi_pName ) )
		return FALSE;

	// 특수 문자 필터링, '[' ']'를 제외한 특수문자	
	for( int i=0; pi_pName[i] != '\0'; ++i )
	{
		if( pi_pName[i] & 0x80 )
			continue;

		else if( isdigit( pi_pName[i] ) )
			continue;

		else if( isalpha( pi_pName[i] ) )
			continue;

		else if( pi_pName[i] == '[' || pi_pName[i] == ']' )
			continue;

		else
			return FALSE;
	}		

#define USER_DEGREE_STD			0		//유저등급-일반
#define USER_DEGREE_GID			1		//유저등급-가이드
#define	USER_DEGREE_MGR			2		//유저등급-운영자
#define USER_DEGREE_DEV			3		//유저등급-개발자

	// 운영자 이상이면 제한을 두지 않는다
	if( pi_byUserDegree >= USER_DEGREE_MGR )
		return TRUE;


	char l_pFilteringHanName[9][16] =
	{		
		"도우미",
		"고객센터",		
		"운영자",
		"운영팀",
		"관리자",
		"관리팀",
		"개발자",
		"개발사",
		"개발팀"
	};		
	// 한글 필터링은 문장이 포함되는지만 체크한다.
	for( i=0; i<9; ++i )
	{
		if( strstr( pi_pName, l_pFilteringHanName[i] ) )
			return FALSE;				
	}


	char l_pFilteringEngName[6][16] = 
	{
		"GM",
		"X2",
		"GAMEMASTER",		
		"CCR",		
		"ODIN",
		"RISINGFORCE"
	};

	// 영문은 대소문자 구분없이 문장이 포함되는지 체크한다.
	int ch;
	WORD l_nFilterStrlen;
	WORD l_nNameStrLen = strlen( pi_pName );
	for( i=0; i<6; ++i )
	{
		// "GM"은 전체문장이 GM일 때만 필터링한다.
		if( i == 0 )
		{
			if( !_stricmp( pi_pName, l_pFilteringEngName[0] ) )
				return FALSE;
		}
		else
		{
			l_nFilterStrlen = strlen( l_pFilteringEngName[i] );
			
			if( l_nNameStrLen < l_nFilterStrlen )
				continue;

			for( ch = 0; ch < l_nNameStrLen ; ++ch )
			{		
				if( l_nNameStrLen - ch < l_nFilterStrlen )
					break;

				if( !_strnicmp( pi_pName + ch, l_pFilteringEngName[i], l_nFilterStrlen ) )
					return FALSE;
			}		
		}
	}


	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////        Boundary          ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////


CGUIMoveObjectMgr::CGUIMoveObjectMgr()
{

}

CGUIMoveObjectMgr::~CGUIMoveObjectMgr()
{
	RemoveAllObject();
}

// 이동할 object와 이동할 위치를 등록한다.
void
CGUIMoveObjectMgr::AddMoveObject( CGUIObject *	pi_pObj,
								 MoveAxis		pi_eAxis,
								 int			pi_nInitPos,
								 int			pi_nLastPos,
								 DWORD			pi_dwVelocity,
								 BYTE			pi_byWaveCnt,
								 WORD			pi_wMaxWaveLength )
{
	// 이미 object가 등록되어있으면 update한다.
	MoveObjectInfo	* l_pMoveObjInfo;
	l_pMoveObjInfo =  GetMoveObject( pi_pObj );
	if( !l_pMoveObjInfo )
	{
		// 이미 이동이 끝난 object가 있으면 사용하고
		// 없으면 새로 만든다.
		l_pMoveObjInfo = GetMoveFinishedObject();
		if( !l_pMoveObjInfo )
		{		
			l_pMoveObjInfo = new MoveObjectInfo;
			
			m_listMoveObj.push_back( l_pMoveObjInfo );
		}
		
		l_pMoveObjInfo->pObject = pi_pObj;
	}
	
	l_pMoveObjInfo->eAxis			= pi_eAxis;
	l_pMoveObjInfo->nInitPos		= pi_nInitPos;
	l_pMoveObjInfo->nLastPos		= pi_nLastPos;
	l_pMoveObjInfo->dwMoveStartTime = 0;	
	l_pMoveObjInfo->dwVelocity		= pi_dwVelocity;
	l_pMoveObjInfo->byCurWaveCnt	= 0;
	l_pMoveObjInfo->byWaveCnt		= pi_byWaveCnt;
	l_pMoveObjInfo->wMaxWaveLength	= pi_wMaxWaveLength;

	l_pMoveObjInfo->bIsFinished		= FALSE;
}

// object를 움직인다.
void
CGUIMoveObjectMgr::StartMove( CGUIObject * pi_pObj )
{
	// moving 시작
	MoveObjectInfo * l_pMoveObjInfo = GetMoveObject( pi_pObj );
	if( l_pMoveObjInfo )
	{
		// set init pos
		if( l_pMoveObjInfo->eAxis == MOVE_AXIS_X )
			l_pMoveObjInfo->pObject->SetPos( l_pMoveObjInfo->nInitPos, l_pMoveObjInfo->pObject->m_ptPos.y );
		else
			l_pMoveObjInfo->pObject->SetPos( l_pMoveObjInfo->pObject->m_ptPos.x, l_pMoveObjInfo->nInitPos );

		l_pMoveObjInfo->byCurWaveCnt = 0;
		l_pMoveObjInfo->fTheta = D3DX_PI / 2;
		
		if( l_pMoveObjInfo->nLastPos > l_pMoveObjInfo->nInitPos )
			l_pMoveObjInfo->nMidPos = l_pMoveObjInfo->nLastPos + l_pMoveObjInfo->wMaxWaveLength * l_pMoveObjInfo->byWaveCnt;
		else
			l_pMoveObjInfo->nMidPos = l_pMoveObjInfo->nLastPos - l_pMoveObjInfo->wMaxWaveLength * l_pMoveObjInfo->byWaveCnt;

		// set start time
		l_pMoveObjInfo->dwMoveStartTime = timeGetTime();
	}
}

void
CGUIMoveObjectMgr::RemoveObject( CGUIObject * pi_pObj )
{
	CGUIMoveObjList::iterator	it;
	for( it=m_listMoveObj.begin(); it != m_listMoveObj.end(); ++it )
	{
		if( (*it)->pObject == pi_pObj )
		{
			delete *it;
			m_listMoveObj.erase( it );

			return;
		}		
	}
}

// 등록된 object를 모두 제거한다.
void
CGUIMoveObjectMgr::RemoveAllObject( void )
{
	CGUIMoveObjList::iterator	it;
	for( it=m_listMoveObj.begin(); it != m_listMoveObj.end(); ++it )
	{
		delete *it;		
	}

	m_listMoveObj.clear();
}

BOOL
CGUIMoveObjectMgr::IsFinishedMove( CGUIObject * pi_pObj )
{
	CGUIMoveObjList::iterator	it;
	MoveObjectInfo * l_pMoveObjInfo;

	for( it=m_listMoveObj.begin(); it != m_listMoveObj.end(); ++it )
	{
		l_pMoveObjInfo = *it;

		if( (*it)->pObject == pi_pObj )
		{
			return (*it)->bIsFinished;				
		}
	}

	return TRUE;	
}

void
CGUIMoveObjectMgr::FrameMove( void )
{
	CGUIMoveObjList::iterator	it;
	MoveObjectInfo * l_pMoveObjInfo;
	int l_nMovePos;
	float l_fTheta;

	for( it=m_listMoveObj.begin(); it != m_listMoveObj.end(); ++it )
	{
		l_pMoveObjInfo = *it;

		if( l_pMoveObjInfo->bIsFinished )
		{
			// erase
			// 나중에 한꺼번에 제거한다.
			continue;
		}		

		// move update
		l_fTheta = l_pMoveObjInfo->fTheta * (float)( timeGetTime() - l_pMoveObjInfo->dwMoveStartTime ) / l_pMoveObjInfo->dwVelocity;
		if( l_fTheta > l_pMoveObjInfo->fTheta )
			l_fTheta = l_pMoveObjInfo->fTheta;	

		l_nMovePos = l_pMoveObjInfo->nInitPos + ( l_pMoveObjInfo->nMidPos - l_pMoveObjInfo->nInitPos ) * sin( l_fTheta );


		if( l_pMoveObjInfo->eAxis == MOVE_AXIS_X )
			l_pMoveObjInfo->pObject->SetPos( l_nMovePos, l_pMoveObjInfo->pObject->m_ptPos.y );
		else
			l_pMoveObjInfo->pObject->SetPos( l_pMoveObjInfo->pObject->m_ptPos.x, l_nMovePos );

		if( l_fTheta == l_pMoveObjInfo->fTheta )
		{
			++l_pMoveObjInfo->byCurWaveCnt;

			// check if it's finished
			if( l_pMoveObjInfo->byCurWaveCnt > l_pMoveObjInfo->byWaveCnt )
			{
				l_pMoveObjInfo->bIsFinished = TRUE;
			}
			else
			{
	//			if( l_pMoveObjInfo->fTheta < D3DX_PI )
	//				l_pMoveObjInfo->fTheta = D3DX_PI;
	//			else
	//				l_pMoveObjInfo->fTheta = D3DX_PI / 2;

				if( l_pMoveObjInfo->eAxis == MOVE_AXIS_X )
				{
					l_pMoveObjInfo->nInitPos = l_pMoveObjInfo->pObject->m_ptPos.x;				
				}
				else
				{
					l_pMoveObjInfo->nInitPos = l_pMoveObjInfo->pObject->m_ptPos.y;				
				}

				if( l_pMoveObjInfo->byCurWaveCnt%2 == 0 )
					l_pMoveObjInfo->nMidPos = l_pMoveObjInfo->nLastPos + l_pMoveObjInfo->wMaxWaveLength * ( l_pMoveObjInfo->byWaveCnt - l_pMoveObjInfo->byCurWaveCnt );
				else
					l_pMoveObjInfo->nMidPos = l_pMoveObjInfo->nLastPos - l_pMoveObjInfo->wMaxWaveLength * ( l_pMoveObjInfo->byWaveCnt - l_pMoveObjInfo->byCurWaveCnt );
				

				l_pMoveObjInfo->dwMoveStartTime = timeGetTime();				
			}
		}		
		
	}
}

// 등록된 object를 리턴한다.
MoveObjectInfo	*
CGUIMoveObjectMgr::GetMoveObject( CGUIObject * pi_pObj )
{
	CGUIMoveObjList::iterator	it;
	for( it=m_listMoveObj.begin(); it != m_listMoveObj.end(); ++it )
	{
		if( (*it)->pObject == pi_pObj )
			return *it;		
	}
	
	return NULL;	
}

// 이동이 끝난 object중 맨 처음 것을 리턴한다.
MoveObjectInfo	*
CGUIMoveObjectMgr::GetMoveFinishedObject( void )
{
	CGUIMoveObjList::iterator	it;
	for( it=m_listMoveObj.begin(); it != m_listMoveObj.end(); ++it )
	{
		if( (*it)->bIsFinished )
			return *it;
	}

	return NULL;	
}

CGUIMoveObjectMgr	g_cMoveObjectMgr;