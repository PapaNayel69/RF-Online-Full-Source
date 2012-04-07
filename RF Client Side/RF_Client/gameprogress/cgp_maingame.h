////////////////////////////////////////////////////////////////////////////////
//
// CGP_MainGame Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __GP_MAINGAME_H__
#define __GP_MAINGAME_H__

#include "CGameProgress.h"

//------------------------------------------------------------------------------
// User Interface
// GUI header
#include "../UserInterface/SpriteMgr.h"
#include "../UserInterface/InputMgr.h"
#include "../UserInterface/GameWindow/GUIGaugeExpWindow.h"
#include "../UserInterface/GameWindow/GUIChatWindow.h"
#include "../UserInterface/GameWindow/GUIHotKeyBoard.h"
#include "../UserInterface/GameWindow/GUICharInfoWindow.h"
#include "../UserInterface/GameWindow/GUIAttackPartWindow.h"
#include "../UserInterface/GameWindow/Recall/GUIMobControl.h"
#include "../UserInterface/GameWindow/Recall/GUIRecallWindow.h"
#include "../UserInterface/GameWindow/Npc/GUINpcSelectWindow.h"
#include "../UserInterface/GameWindow/GUISelectedCharWin.h"
#include "../UserInterface/GameWindow/Group/GUIPartyWindow.h"
#include "../UserInterface/GameWindow/GUISelectedCharMenu.h"
#include "../UserInterface/GameWindow/EtcWin/GUIMapMoveWindow.h"
#include "../UserInterface/GameWindow/SkillForce/GUISkillForceQueue.h"
#include "../UserInterface/GameWindow/EtcWin/GUIRadarWindow.h"
#include "../UserInterface/GameWindow/EtcWin/GUIMiniMapWindow.h"
#include "../UserInterface/GameWindow/EtcWin/GUIMapLoadingWindow.h"
#include "../UserInterface/GameWindow/EtcWin/GUIOptionWindow.h"

#include "../UserInterface/GUIController/GUIScreen.h"
#include "../UserInterface/ConsoleWin.h"
#include "../UserInterface/MouseDev.h"
#include "../UserInterface/GUIController/GUIMouse.h"
// User Interface
//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define MAX_DISPLAY_INFO_NUM	0x4

#define	DISPLAY_INFO_NONE		0x0
#define	DISPLAY_INFO_AVATAR		0x1
#define	DISPLAY_INFO_PLAYER		0x2
#define	DISPLAY_INFO_MONSTER	0x3

//------------------------------------------------------------------------------

const UINT MOUSE_BD_MSG[4]	= { 0, WM_LBUTTONDOWN, WM_RBUTTONDOWN, WM_MBUTTONDOWN };
const UINT MOUSE_BU_MSG[4]	= { 0, WM_LBUTTONUP, WM_RBUTTONUP, WM_MBUTTONUP };
const UINT MOUSE_BUTTON[4]	= { 0, MK_LBUTTON, MK_RBUTTON, MK_MBUTTON };

/*//////////////////////////////////////////////////////////////////////////////

[ CGP_MainGame ]

//////////////////////////////////////////////////////////////////////////////*/

class CInputProcessMgr;

class CHintMgr;
class CUIHelpMgr;
class CGameEventBoard;
class CPartyBoard;
class CHelpBoard;

class CTargetMgr;
class CNextActionMgr;

//------------------------------------------------------------------------------

class CGP_MainGame : public CGameProgress
{
// < Data Member > ---------------------------------------------------------
private :

	BOOL	m_bIsCompleteDataDownload;		// 메인게임 시작전 데이터를 모두 다운로드 받았는가?
	BOOL	m_bIsCreated_UI;
	BOOL	m_bIsCreated_Board;
	BOOL	m_bIsCreated_Avatar;

	DWORD	m_dwDisplayInfomationMode;		// 게임 정보 디스플레이 모드
	BOOL	m_bIsDisplayNetworkMessage;		// 네트웍 메세지를 출력할 것인가?
	BOOL	m_bIsRenderCollisionMap;		// 충돌맵을 렌더링 할 것인가?
	BOOL	m_bIsRenderBoundBox;			// 바운딩 박스를 렌더링 할 것인가?
	BOOL	m_bIsDisplayHelpText;			// 도움말을 렌더링 할 것인가?

	BOOL	m_bIsEnableInput;				// 키보드와 마우스 입력이 가능한가?

	DWORD	m_dwRequestDataUploadTime;		// 데이터 업로드를 요청한 시간
	DWORD	m_dwRequestLogoutTime;			// 로그 아웃을 요청한 시간

	DWORD	m_dwHoverMouseUI;				// 현재 Mouse가 Hover 상태인 UI의 ID
	BOOL	m_bIsOpenedWindow[MAX_UI];		// 현재 UI창이 열린 상태인가?

	BOOL	m_bIsChatMode;					// 채팅 모드인가?
	BOOL	m_bIsMoveByDrag;				// 아바타가 드래그로 움직이는 중인가?
	BOOL	m_bIsAutoMouse;					// 오토마우스 모드인가?
	BOOL	m_bIsAutoTargeting;				// 오토타겟팅 모드인가?
	BOOL	m_bIsFreeTargetCombatMode;		// 타겟이 없는 전투모드
	BOOL	m_bIsPlayMovie;					// 리얼타임 무비 플레이
	DWORD	m_dwMovieFlag;					// 리얼타임 무비 플래그
	BOOL	m_bIsHideUI;					// UI 숨김

	BOOL	m_bIsRequestLogout;				// 로그 아웃을 요청했는가?

	BYTE	m_byOldCameraMode;				// old 카메라모드 저장. by J.U.I

	//--------------------------------------------------------------------------

	CInputProcessMgr *	m_pInputProcessMgr;

	CHintMgr *			m_pHintMgr;
	CUIHelpMgr *		m_pUIHelpMgr;

	CGameEventBoard *	m_pGameEventBoard;
	CPartyBoard *		m_pPartyBoard;
	CHelpBoard *		m_pHelpBoard;

	CTargetMgr *		m_pTargetMgr;
	CNextActionMgr *	m_pNextActionMgr;

	//--------------------------------------------------------------------------

	BOOL		m_bIsMove_Cursor;
	float		m_fCursorAngle_PI;
	Vector3f	m_vecCursorOrgPos;
	Vector3f	m_vecCursorTarPos;
	Vector3f	m_vecCursorCurPos;
	DWORD		m_dwMouseButton;

	//--------------------------------------------------------------------------
	// User Interface
	CGUIGaugeExpWindow		m_uiGaugeWindow;	
	CGUIChatWindow			m_uiChatWindow;
	CGUIChatInputWindow		m_uiChatInputWindow;		
	CGUICharInfoWindow		m_uiCharInfoWindow;
	CGUIHotKeyBoard			m_uiFunctionKeyWindow;
	CGUIAttackPartWindow	m_uiSelectAttackPartWindow;	
	CGUIRecallWindow		m_uiSummonsSelectWindow;
	CGUIMobControl			m_uiSummonsControlWindow;
	CGUINpcSelectWindow		m_uiNpcIntroduceWindow;		
	CGUIPartyWindow			m_uiPartyWindow;
	CGUIMapMoveWindow		m_uiMoveMapWindow;
	CGUISkillForceQueue		m_uiAppliedSFWindow;
	CGUIRadarWindow			m_uiRadarWindow;
	CGUIMiniMapWindow		m_uiMinimapWindow;
	CGUIMapLoadingWindow	m_uiMapLoadingWindow;
	CGUIOptionWindow		m_uiOptionWindow;

// < Member Function > -----------------------------------------------------
public :
	CGP_MainGame();
	~CGP_MainGame();

	void		Init( void );
	BOOL		Create( void );
	BOOL		Destroy( void );

	void		RequestLogout( void );

protected :

	void		Create_UI( void );
	void		Create_Board( void );
	void		Create_Avatar( void );

	void		LoadGameSetting( void );
	void		SaveGameSetting( void );

	void		Destroy_UI( void );
	void		Destroy_Board( void );

	HRESULT		FrameMove( void );
	HRESULT		Render( void );
	BOOL		InputProcess( void );

	BOOL		LoadData( void );
	BOOL		UnloadData( void );

	void		UI_LoadSprite( void );
	void		UI_UnloadSprite( void );

	//--------------------------------------------------------------------------
	// Event Procedual ( for UI )
	static	DWORD	EventProcChatWindow( void * pi_pObject );

	//--------------------------------------------------------------------------
	
	void		DrawAvatarPath( void );
	void		DisplayInformation( int pi_nX, int pi_nY );
	void		DisplayInformation_Avatar( int pi_nX, int pi_nY );
	void		DisplayInformation_Player( int pi_nX, int pi_nY );
	void		DisplayInformation_Monster( int pi_nX, int pi_nY );

	void		DisplayNetworkMessage( int pi_nX, int pi_nY );

	void		DisplayHelpText( void );

public :
	LRESULT		MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	HRESULT		RestoreDeviceObjects( void );
	HRESULT		InvalidateDeviceObjects( void );

	BOOL		InputProcess_Mouse( void );

	/*
	BOOL		InputProcess_Keyboard( void );
	BOOL		InputProcess_Keyboard_Normal( void );
	BOOL		InputProcess_Keyboard_WithShift( void );
	BOOL		InputProcess_Keyboard_WithControl( void );
	BOOL		InputProcess_Keyboard_WithControlAndNormal( void );
	BOOL		InputProcess_Keyboard_WithAlt( void );

	void		InputProcess_Keyboard_SetTarget( void );
	void		InputProcess_Keyboard_SelectTarget( void );
	void		InputProcess_Keyboard_DisplayInfo( void );
	*/

	BOOL		ProcessMousePicking( void );
	BOOL		ProcessMousePicking_ForCast( CCharacter * pi_pTarget );
	BOOL		ProcessMousePicking_Map( void );
	BOOL		ProcessMousePicking_Character( CCharacter * pi_pChar, BOOL pi_bIsByKeyboard );
	BOOL		ProcessMousePicking_Character_Player( CPlayer * pi_pPlayer, BOOL pi_bIsByKeyboard );
	BOOL		ProcessMousePicking_Character_Monster( CMonster * pi_pMonster, BOOL pi_bIsByKeyboard );
	BOOL		ProcessMousePicking_Character_Animus( CAnimus * pi_pAnimus, BOOL pi_bIsByKeyboard );
	BOOL		ProcessMousePicking_Character_GuardTower( CGuardTower * pi_pGuardTower, BOOL pi_bIsByKeyboard );
	BOOL		ProcessMousePicking_Character_Attack( CCharacter * pi_pEnemy, BOOL pi_bIsByKeyboard );
	BOOL		ProcessMousePicking_Character_Npc( CNpc * pi_pNpc, BOOL pi_bIsByKeyboard );
	BOOL		ProcessMousePicking_Character_Unit( CPlayer * pi_pUnit, BOOL pi_bIsByKeyboard );
	BOOL		ProcessMousePicking_Character_None( BYTE pi_byAttackType );		// 아바타가 단축키를 이용해서 타겟없이 공격할 경우 사용
	BOOL		ProcessMousePicking_Item( CGroundItem * pi_pGroundItem );

	void		ProcessMouseMove( void );
	void		ProcessMouseMove_Character( CCharacter * pi_pChar );
	void		ProcessMouseMove_Item( CGroundItem * pi_pGroundItem );

	BOOL		ProcessMouseClick_FreeTarget( BOOL pi_bLeft );

	CObject *	CollisionCheckWithBoundBox( BOOL pi_bIsPicking );

	//--------------------------------------------------------------------------

	void		MoveCursor( int pi_nX, int pi_nY, DWORD pi_dwButton );

	//--------------------------------------------------------------------------

	inline	void	SetCompleteDataDownload( BOOL pi_bFlag )	{ m_bIsCompleteDataDownload = pi_bFlag; }

	inline	void	SetDisplayInfomationMode( DWORD pi_dwMode )	{ m_dwDisplayInfomationMode = pi_dwMode; }
	inline	DWORD	GetDisplayInfomationMode( void )			{ return m_dwDisplayInfomationMode; }
	inline	void	SetDisplayNetworkMessage( BOOL pi_bFlag )	{ m_bIsDisplayNetworkMessage = pi_bFlag; }
	inline	BOOL	IsDisplayNetworkMessage( void )				{ return m_bIsDisplayNetworkMessage; }
	inline	void	SetRenderCollisionMap( BOOL pi_bFlag )		{ m_bIsRenderCollisionMap = pi_bFlag; }
	inline	BOOL	IsRenderCollisionMap( void )				{ return m_bIsRenderCollisionMap; }
	inline	void	SetRenderBoundBox( BOOL pi_bFlag )			{ m_bIsRenderBoundBox = pi_bFlag; }
	inline	BOOL	IsRenderBoundBox( void )					{ return m_bIsRenderBoundBox; }
	inline	void	SetDisplayHelpText( DWORD pi_bFlag )		{ m_bIsDisplayHelpText = pi_bFlag; }
	inline	BOOL	IsDisplayHelpText( void )					{ return m_bIsDisplayHelpText; }

	inline	void	EnableInput( BOOL pi_bFlag )				{ m_bIsEnableInput = pi_bFlag; }
	inline	BOOL	IsEnableInput( void )						{ return m_bIsEnableInput; }
	inline	void	SetHoverMouseUI( DWORD pi_dwUIID )			{ m_dwHoverMouseUI = pi_dwUIID; }
	inline	BOOL	GetHoverMouseUI( void )						{ return m_dwHoverMouseUI; }

	inline	void	SetChatMode( BOOL pi_bFlag )				{ m_bIsChatMode = pi_bFlag; }
	inline	BOOL	IsChatMode( void )							{ return m_bIsChatMode; }
	inline	void	SetMoveByDrag( BOOL pi_bFlag )				{ m_bIsMoveByDrag = pi_bFlag; }
	inline	BOOL	IsMoveByDrag( void )						{ return m_bIsMoveByDrag; }
	inline	void	SetAutoMouse( BOOL pi_bFlag )				{ m_bIsAutoMouse = pi_bFlag; }
	inline	BOOL	IsAutoMouse( void )							{ return m_bIsAutoMouse; }
	inline	void	SetAutoTargeting( BOOL pi_bFlag )			{ m_bIsAutoTargeting = pi_bFlag; }
	inline	BOOL	IsAutoTargeting( void )						{ return m_bIsAutoTargeting; }
	inline	void	SetFreeTargetCombatMode( BOOL pi_bFlag )	{ m_bIsFreeTargetCombatMode = pi_bFlag; }
	inline	BOOL	IsFreeTargetCombatMode( void )				{ return m_bIsFreeTargetCombatMode; }
	inline	void	SetPlayMovie( BOOL pi_bFlag )				{ m_bIsPlayMovie = pi_bFlag; }
	inline	BOOL	IsPlayMovie( void )							{ return m_bIsPlayMovie; }
	inline	void	SetMovieFlag( DWORD pi_bFlag )				{ m_dwMovieFlag = pi_bFlag; }
	inline	DWORD	GetMovieFlag( void )						{ return m_dwMovieFlag; }
	inline	void	SetHideUI( BOOL pi_bFlag )					{ m_bIsHideUI = pi_bFlag; }
	inline	BOOL	IsHideUI( void )							{ return m_bIsHideUI; }

			void	ToggleHint( void );
			void	EnableHint( BOOL pi_bFlag );
			BOOL	IsEnableHint( void );

	inline	BOOL	IsRequestLogout( void )						{ return m_bIsRequestLogout; }
};

#endif // __GP_MAINGAME_H__