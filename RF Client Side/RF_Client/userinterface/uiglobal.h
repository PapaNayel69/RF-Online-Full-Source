////////////////////////////////////////////////////////////////////////////
//
// Global data and function Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __UIGLOBAL_H__
#define __UIGLOBAL_H__

#include <windows.h>
#include "Singleton.h"
#include <list>
using namespace std ;



// money type
#define	UI_IMT_DALANT	0
#define	UI_IMT_GOLD		1

// race type
#define	RT_BELLATO		0
#define	RT_CORA			1
#define	RT_ACCRETIA		2
#define	RT_NONE			3

// sex type
#define	ST_MALE			0
#define	ST_FEMALE		1
#define	ST_NONE			2

// continuous skill force type
#define	SF_CONT_DAMAGE	2
#define	SF_CONT_HELP	3

// sound id
#define	SND_ID_WINDOW_OPEN		273000
#define	SND_ID_WINDOW_CLOSE		273001
#define SND_ID_BUTTON_CLICK		273002
#define	SND_ID_ITEM_EQUIP		273003
#define	SND_ID_ITEM_PICKUP		273004
#define	SND_ID_BELT_EQUIP		273005
#define	SND_ID_NOT_USABLE		273006
#define	SND_ID_OK_BUTTON		273007
#define	SND_ID_ITEM_ADD_IN_PACK	273008	

#define	SND_ID_SUCC_UPGRADE		273009
#define	SND_ID_FAIL_UPGRADE		273010
#define	SND_ID_SUCC_MAKE		273011
#define	SND_ID_FAIL_MAKE		273012

#define	SND_ID_EVENT_ALARM		273013

#define	SND_ID_ADMIN_MSG		273014

#define	SND_ID_LOGIN_SELECT		273015
#define	SND_ID_LOGIN_OK			273016
#define	SND_ID_LOGIN_CANCEL		273017


// ---------------------------------------------------------------------------
// Global MessageBox USER DEFINED ID

#define	INPUT_DLG_ID			0x8000

#define	MSGBOX_ID_CLOSE			0x8001

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////        Boundary          ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////
class CGUICharInfoWindow;

void	_SetUIAvatarRace( BYTE pi_byRaceType );
void	_SetCharInfoWindow( CGUICharInfoWindow * pi_pWin );

BYTE	_GetAvatarRace( void );
WORD	_GetAvatarLevel( void );

char *	_GetAvatarMoneyName( void );	// 아바타 종족의 화폐 이름

char * _GetMoneyName( BYTE pi_byRaceType );

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////        Boundary          ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////
class CGUIItem;

struct TRADE_ITEM_INFO {
	CGUIItem	*	pItem;
	DWORD			dwListIndex;	
};

extern TRADE_ITEM_INFO		g_sHoverItemInfo;
extern BOOL					g_bIsQuickEvent_Equip;
extern BOOL					g_bIsQuickEvent_Trade;

void	SetQuickEvent( void );

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////        Boundary          ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 돈에 "," 표시
void	_GetPriceStr( DWORD pi_dwPrice, char * po_pPriceStr );
DWORD	_GetPriceNum( char * pi_pPriceStr );



////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////        Boundary          ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////
// 욕, 광고 필터링
BOOL	_LoadAbuseFilter( char * pi_pAbuseDataFilename );
BOOL	_LoadAdvFilter( char *pi_pAdvDataFilename );

BOOL	_IsAbuseStr( char * pi_pStr );
BOOL	_IsAdvStr( char * pi_pStr );


// 캐릭터 이름 필터링
BOOL	_IsAvailableCharName( char * pi_pName, BYTE pi_byUserDegree = 0 );



////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////        Boundary          ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

// moving object
enum MoveAxis {
	MOVE_AXIS_X,
	MOVE_AXIS_Y
};

class CGUIObject;

struct MoveObjectInfo
{
	CGUIObject	*	pObject;			// 이동할 object
	
	int				nInitPos;			// 처음 위치
	int				nLastPos;			// 이동할 위치
	int				nMidPos;
	MoveAxis		eAxis;				// 이동 축
	DWORD			dwMoveStartTime;	// 이동 시작 시간
	DWORD			dwVelocity;			// 이동 속도
	float			fTheta;
	BYTE			byWaveCnt;
	BYTE			byCurWaveCnt;
	WORD			wMaxWaveLength;

	BOOL			bIsFinished;		// 이동이 끝났을 때 true로
};

typedef list< MoveObjectInfo * >	CGUIMoveObjList;


class CGUIMoveObjectMgr : public CSingleton<CGUIMoveObjectMgr>
{
private:
	CGUIMoveObjList		m_listMoveObj;

public:
	CGUIMoveObjectMgr();
	~CGUIMoveObjectMgr();

	void	AddMoveObject( CGUIObject *	pi_pObj,
						   MoveAxis		pi_eAxis,
						   int			pi_nInitPos,
						   int			pi_nLastPos,
						   DWORD		pi_dwVelocity,
						   BYTE			pi_byWaveCnt = 0,
						   WORD			pi_wMaxWaveLength = 10 );

	void	StartMove( CGUIObject * pi_pObj );

	void	RemoveObject( CGUIObject * pi_pObj );
	void	RemoveAllObject( void );

	BOOL	IsFinishedMove( CGUIObject * pi_pObj );
	
	void	FrameMove( void );

private:
	MoveObjectInfo	*	GetMoveObject( CGUIObject * pi_pObj );
	MoveObjectInfo	*	GetMoveFinishedObject( void );
};

extern CGUIMoveObjectMgr	g_cMoveObjectMgr;

#endif //__UIGLOBAL_H__
