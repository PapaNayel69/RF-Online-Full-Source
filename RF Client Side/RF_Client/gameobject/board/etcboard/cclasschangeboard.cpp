////////////////////////////////////////////////////////////////////////////////
//
// CClassChangeBoard Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../../Manager/CCharacterMgr.h"
#include "../Common/CBoardManager.h"
#include "../General/CGameEventBoard.h"

#include "../../../Network/NetMsgProcessor/Event/CNetMsgProcessor_Event.h"

#include "CClassChangeBoard.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CClassChangeBoard::CClassChangeBoard()
{
	m_bIsEnableClassChange	= FALSE;

	m_pSelectClass			= NULL;
	ZeroMemory( m_listChildClass, sizeof( CPlayerClass * ) * CPlayerClass::MAX_CHILD_CLASS_NUM );

	m_pNMP_Event			= static_cast< CNetMsgProcessor_Event * >( _GetNetworkMgr()->GetNetMsgProcessor( NMP_EVENT ) );
}

CClassChangeBoard::~CClassChangeBoard()
{
}

void
CClassChangeBoard::InitWindow( DWORD pi_dwWindowID, int pi_nPosX, int pi_nPosY )
{
	m_uiWindow = &m_uiClassChangeWindow;

	CBaseBoard::InitWindow( pi_dwWindowID, pi_nPosX, pi_nPosY );
}

void
CClassChangeBoard::OpenWindow( void )
{
	// check
	if( !_GetAvatar()->IsChangableClass() )
	{
		_SetSystemMsg( "이미 최종 단계의 클래스까지 전직을 마쳤습니다." );
		 return;
	}

	if( !m_bIsEnableClassChange )
	{
		_SetSystemMsg( "아직 전직할 수 있는 자격이 없습니다." );
		 return;
	}

	// Get Current Avatar's Class Info
	CPlayerClass * pCurClass = _GetCharDataMgr()->GetClassDataByDTIndex( _GetAvatar()->GetClassDTIndex( 0 ) );
	if( !pCurClass )
		return;

	// Clear previous list
	ZeroMemory( m_listChildClass, sizeof( CPlayerClass * ) * CPlayerClass::MAX_CHILD_CLASS_NUM );

	// Get Child Class Info
	CPlayerClass *	pChildClass = NULL;
	BYTE 			pUIIconList[CPlayerClass::MAX_CHILD_CLASS_NUM];
	ZeroMemory( pUIIconList, CPlayerClass::MAX_CHILD_CLASS_NUM );

	for( int i = 0, nChildClassNum = 0; i < CPlayerClass::MAX_CHILD_CLASS_NUM; ++i )
	{
		pChildClass = pCurClass->GetChildClass( i );
		if( !pChildClass )
			continue;

		m_listChildClass[nChildClassNum]	= pChildClass;
		pUIIconList[nChildClassNum]			= pChildClass->GetIconID();
		++nChildClassNum;
	}

	// Set UI
	if( nChildClassNum <= 0 )
		return;

	m_uiClassChangeWindow.SetTotalClassNum( nChildClassNum, pUIIconList );
	UI_SelectClass( 0 );

	CBaseBoard::OpenWindow();
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CClassChangeBoard::UI_SelectClass( BYTE pi_bySocketNo )
{
	if( pi_bySocketNo >= CPlayerClass::MAX_CHILD_CLASS_NUM )
		return;
	if( !m_listChildClass[pi_bySocketNo] )
		return;

	// set class name
	m_uiClassChangeWindow.SetClassName( m_listChildClass[pi_bySocketNo]->GetClassName() );

	// set class description
	m_uiClassChangeWindow.SetDescription( m_listChildClass[pi_bySocketNo]->GetDescription() );

	// set class skill
	SF_DATA *	pClassSkill = NULL;
	DWORD 		pUIIconList[CPlayerClass::MAX_SKILL_NUM];
	ZeroMemory( pUIIconList, sizeof( DWORD ) * CPlayerClass::MAX_SKILL_NUM );

	for( int i = 0, cnt = 0; i < CPlayerClass::MAX_SKILL_NUM; ++i )
	{
		pClassSkill = m_listChildClass[pi_bySocketNo]->GetClassSkill( i );
		if( !pClassSkill )
			continue;

		pUIIconList[cnt] = pClassSkill->dwIconID;
		++cnt;
	}

	m_uiClassChangeWindow.SetSkillIcon( cnt, pUIIconList );

	m_uiClassChangeWindow.SelectClass( pi_bySocketNo );

	m_pSelectClass = m_listChildClass[pi_bySocketNo];
}

void
CClassChangeBoard::UI_ClassChangeRequest( void )
{
	if( !m_bIsEnableClassChange || !m_pSelectClass )
		return;

	if( _GetGBM()->IsRequestProcess() )
		return;

	m_pNMP_Event->SelectClassRequest_clzo( m_pSelectClass->GetDTIndex() );
}

void
CClassChangeBoard::UI_ClassChangeResult( void )
{
	// change class
	_GetAvatar()->ChangeClass( m_pSelectClass->GetDTIndex() );
	_SetSystemMsg( "아바타의 클래스가 %s(으)로 전직 되었습니다.", _GetAvatar()->GetClassName() );

	// add new skill to skill board
#ifdef _DEVELOPER_
	_GetSkillBoard()->CreateClassSkill( m_pSelectClass->GetDTIndex() );
	_SetSystemMsg( "새로운 클래스 스킬이 추가 되었습니다." );
#endif

	// init
	m_bIsEnableClassChange	= FALSE;
	m_pSelectClass			= NULL;
	_GetGameEventBoard()->ClearEvent( GET_CLASS, CClassEvent::RECEIVE_CLASS_CHANGE );

	CloseWindow();
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CClassChangeBoard::FrameMove( void )
{
}

LRESULT
CClassChangeBoard::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
		case WM_ENABLE_CLASS_CHANGE :
			m_bIsEnableClassChange	= TRUE;
			break;

		case WM_BOARD_HOVER_OBJECT :
			//if( HIWORD( lParam ) == 1 )	// 클래스는 0, 클래스 스킬은 1
				//UI_HoverOnClassSkill( LOWORD( lParam ) );
			break;

		case WM_BOARD_CLICK_OBJECT :
			if( HIWORD( lParam ) == 0 )	// 클래스는 0, 클래스 스킬은 1
				UI_SelectClass( LOWORD( lParam ) );
			break;

		case WM_BOARD_OK :
			UI_ClassChangeRequest();
			break;

		case WM_BOARD_CANCEL :
			CloseWindow();
	}

	return CBaseBoard::MsgProc( hWnd, uMsg, wParam, lParam );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////
