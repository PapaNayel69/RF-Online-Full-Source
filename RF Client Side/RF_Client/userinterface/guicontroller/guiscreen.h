////////////////////////////////////////////////////////////////////////////
//
// CGUIScreen Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUIISCREEN_H__
#define __GUIISCREEN_H__

#pragma warning( disable : 4786 )

#include "GUIContainer.h"
#include "GUIWindow.h"
#include "GUIMouse.h"
#include "GUIItem.h"
#include "../../input/IMEEdit.h"
#include "GUITooltip.h"
#include <list>


extern	DWORD	g_dwGUIInputID;	// GUI input과 다른 input을 구별하기 위해
								// GUI input이 일어난 Object의 ID를 가지고 있는다.

#define	NONE_INPUT_ID					0xffffffff

#define	DEFAULT_ID_DIALOG_BOX			0x0000ffff	// dialogBox가 떳을 때는 
													// active window를 외부에서 조작할 수 없게 하기 위해
													// 현재 dialogbox가 떴는지 알아야한다.

// -----------------------------------------------------------------
// MessageBox USER DEFINED ID
// by J.S.H
#define MSGBOX_ID_REBIRTH				0x1000		// 부활 다이얼로그 박스의 ID
#define MSGBOX_ID_REQUEST_TRADE			0x2000		// 아이템 트레이드 요청
#define MSGBOX_ID_DISCONNECT_SERVER		0xF000		// 서버와의 접속을 끊음
#define MSGBOX_ID_QUIT_PROGRAM			0xF001		// 프로그램 종료
#define MSGBOX_ID_DISCONNECT_ACCOUNT	0xF002		// 기존의 접속을 끊음 => 로그인에서 사용

#define MSGBOX_ID_GAMEPROGRESS_LOGIN	0xF100		// 로그인 전체 부분에서 사용됨
#define MSGBOX_ID_GAMEPROGRESS_MAINGAME	0xF101		// 메인 게임 전체 부분에서 사용됨

#define MSGBOX_ID_CONDITION_INFO		0x3001		// 캐릭터의 상태 정보창

#define MSGBOX_ID_UNIT_DELIVERY			0x5001		// 유닛 출고
// by J.S.H

#define	MSGBOX_ID_CRITICAL_ERROR		0xFFF1		// 에러 메시지


// -----------------------------------------------------------------
// tooltip 관련 
#define	TOOLTIP_TYPE_DEFAULT			0	// 일반 툴팁
#define	TOOLTIP_TYPE_ITEM				1	// 아이템 툴팁

#define TOOLTIP_STR_LENGTH				1280

#define	TOOLTIP_MAX_LINE				32
#define	TOOLTIP_MAX_STR					64

#define	TOOLTIP_MAX_COLOR_STR_NUM		32

struct TOOLTIP_COLOR_STR_INFO
{
	BYTE	byLineIndex;
	BYTE	byCharIndex;

	DWORD	dwColor;

	char	pString[TOOLTIP_MAX_STR];
};
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// 업그레이드 정보
#define	UPGRADE_SOCKET_NUM				7

struct UPGRADE_ITEM_INFO
{
	BYTE	byTotalSocketNum;
	BYTE	byTotalItemNum;
	
	DWORD 	dwSpriteIDList[UPGRADE_SOCKET_NUM];

	BYTE	byUpgradeSocketLineIdx;
	BYTE	byUpgradeSocketCharIdx;
};

/*//////////////////////////////////////////////////////////////////////////

[ CGUIScreen ]

  - 화면상의 윈도우를 관리하는 가상의 screen이다. 

  - CGUIScreen에 등록된 window만이 
    다른 window와의 interaction(object 주고받기 등)이 가능하다.
	CGUIScreen을 통해서 각 윈도우에 input message를 전달하고.. 
    윈도우간에 Item이나 기타 object를 주고 받을 때 CGUIScreen을 
	통해서만 가능하다.

  - Mouse cursor도 여기에 등록되어야 한다. 
  
//////////////////////////////////////////////////////////////////////////*/
class CGUIScreen : public CGUIWindow,				
				   public CSingleton<CGUIScreen>
				   
{
// < Data Member > ---------------------------------------------------------
private :
	HWND				m_hWnd;
	//--------------------------------------------------------------------------
	// mouse cursor
	CGUIMouse			m_cMouse;	
	
	//--------------------------------------------------------------------------
	// 마우스가 잡고 있는 아이템
	CGUIItem			m_cCatchedItem;

	//--------------------------------------------------------------------------
	// Tool tip
	BYTE				m_byTooltipType;

	BYTE				m_byTooltipStringNum;
	char				m_pTooltipString[TOOLTIP_MAX_LINE][TOOLTIP_MAX_STR];
	POINT				m_ptTooltipBoardPos;
	POINT				m_ptTooltipBoardSize;

	BYTE					m_byTooltipColorStrNum;
	TOOLTIP_COLOR_STR_INFO	m_pTooltipColorStrInfo[TOOLTIP_MAX_COLOR_STR_NUM];

	CGUIItem			m_cTooltipItem;
	CGUIObject			m_cTooltipItemBase;
	
	CGUIObject			m_cUpgradeSocket;
	CGUIItem			m_cUpgradeItem[UPGRADE_SOCKET_NUM];

	BYTE				m_byTotalUpgradeSocketNum;
	BYTE				m_byTotalUpgradeItemNum;
	BYTE				m_byUpgradeSocketLineIdx;
	BYTE				m_byUpgradeSocketCharIdx;
	
	DWORD				m_dwTooltipDelayTime;	// 툴팁 뜨는 딜레이
	DWORD				m_dwTooltipStartTime;	

	BOOL				m_bCreated;
	

// < Member Function > -----------------------------------------------------
public :

	CGUIScreen();
	~CGUIScreen();
	
			void	Init( void );

			void	RestoreSprite( void );
			void	SetSprite( void );

	inline	void	SetParent( CGUIWindow * pi_pParent ) {;}

			void	Show( BOOL pi_bShow );

	//--------------------------------------------------------------------------
	// mouse cursor
			void		SetMouseState( CGUIMouse::CursorState pi_eState );
	inline	CGUIMouse * GetMouse( void )				{ return &m_cMouse; }

	//--------------------------------------------------------------------------
	// window 등록, 삭제
			void	Add( CGUIWindow * pi_pWindow );
			void	Remove( CGUIWindow * pi_pWindow );
			void	RemoveAll( void );

	//--------------------------------------------------------------------------
	// event process
			BOOL	InputProc( void );	
			LRESULT	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

			void	ActiveWindow( CGUIWindow * pi_pWindow );
			void	InActiveWindow( CGUIWindow * pi_pWindow );

			void	MouseEvent( _mouse_event & event );
			void	MouseMotionEvent( _mouse_motion_event & event );

			BOOL	Draw( void );
			void	Update( void );
			void	ClosingDialogBox( CGUIDialogBox * pi_pDlg );
	
	//--------------------------------------------------------------------------
	// catched object
	inline	CGUIItem *	GetObject( void )				{ return &m_cCatchedItem; }
	inline	BOOL	ExistObject( void )					{ return !m_cCatchedItem.IsEmpty(); }
			void	AddObject( CGUIItem * pi_pItem );
			void	ResizeObject( int pi_nWidth, int pi_nHeight );
			void	RemoveObject( void );
			void	DeleteObject( void );
			
	//--------------------------------------------------------------------------
	// ToolTip
			void	SetTooltip( BYTE pi_byLineNum, char pi_pStrLine[][TOOLTIP_MAX_STR], BYTE pi_byColorStrNum = 0, TOOLTIP_COLOR_STR_INFO * pi_pColorStr = NULL );
			void	SetTooltip_Item( CGUIItem *	pi_pItem,
									 BYTE pi_byLineNum, char pi_pStrLine[][TOOLTIP_MAX_STR],
									 BYTE pi_byColorStrNum = 0, TOOLTIP_COLOR_STR_INFO * pi_pColorStr = NULL,
									 UPGRADE_ITEM_INFO * pi_pUpgradeInfo = NULL );
			void	RemoveTooltip( void );
	
	inline	void	SetTooltipDelay( DWORD pi_dwDelay )	{ m_dwTooltipDelayTime = pi_dwDelay; }

private :
			void	DrawTooltip( void );

public :

	inline	void	SetHWND( HWND pi_hWnd )				{ m_hWnd = pi_hWnd; }
	inline	HWND	GetHWND( void )						{ return m_hWnd; }
};

#define	gScreen		CGUIScreen::GetSingleton()

#endif // __GUIISCREEN_H__