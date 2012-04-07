////////////////////////////////////////////////////////////////////////////////
//
// CGP_MainGame Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../GameObject/Manager/CCharacterMgr.h"
#include "../GameObject/Manager/CTargetMgr.h"
#include "../GameObject/Manager/CGroundItemManager.h"
#include "../GameObject/Board/Common/CBoardManager.h"
#include "../Character/Avatar/CAvatar_Quest.h"

#include "../Network/NetMsgProcessor/Combat/CNetMsgProcessor_Combat.h"

#include "../CLand.h"
#include "../CCamera.h"
#include "../Item/CItemMgr.h"
#include "../DataTable/CResourceDataMgr.h"

#include "CGP_MainGame.h"
#include "../MainApp.h"
#include "../addcodebyjang/effectutil.h"

#include "CInputProcessMgr.h"

#ifdef _DEVELOPER_
#include "RTMovie.h"
#endif

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
EP_Logout( void )
{
	_GetOptionWindow()->Close();
	_GetMainGame()->RequestLogout();
}

void
EP_QuitProgram( void )
{
	_GetOptionWindow()->Close();
	_GetMainApp()->RequestQuitProgram();
}

//------------------------------------------------------------------------------

extern BOOL	g_bWriteTestLog;	// 테스트 코드

#define CAM_MIN_DIST	10		// 장언일 추가코드(기획쪽과 상의해서 카메라조정.)
#define CAM_MAX_DIST	60
#define CAM_MAX_DIST_FREE	75
#define CAM_MIN_TH		-35

extern	int	g_nOpenedDlgBoxCount;	// 열려있는 dialog box 갯수

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

Vector3f g_vecTemp;		// 임시코드
BOOL
CGP_MainGame::InputProcess( void )
{
	if( !m_bIsCompleteDataDownload )
		return FALSE;

	//--------------------------------------------------------------------------
	// 네트웍 상태 체크

	// 로그 아웃
	if( m_bIsRequestLogout )
	{
		const	static  DWORD	CLIENT_WAIT_LIMIT_TIME = 20000;
				static	BOOL	l_bShowMessage = FALSE;

		// 인벤토리 업로드를 요청한지 일정 시간이 지났는데도 응답이 오지 않았다면
		// 네트웍 클로즈 요청. ( 현재는 20초 )
		if( m_dwRequestDataUploadTime != 0 )
		{
			if( ( timeGetTime() - m_dwRequestDataUploadTime ) >= CLIENT_WAIT_LIMIT_TIME )
			{
				_GetNetworkMgr()->SetResultOfInventoryUpload( INVEN_UPLOAD_FAILURE );
				m_dwRequestDataUploadTime	= 0;
			}
		}

		// 로그아웃을 요청한지 일정한 시간이 지나도 응답이 없다면 네트웍이 끊긴것으로 간주
		// => 프로그램을 종료한다. ( 현재는 20초 )
		if( m_dwRequestLogoutTime != 0 )
		{
			if( ( timeGetTime() - m_dwRequestLogoutTime ) >= CLIENT_WAIT_LIMIT_TIME )
			{
				_GetMainApp()->RequestQuitProgram( 2000, "서버로부터 응답이 없어 프로그램을 종료합니다. 잠시후 다시 접속해 주십시오." );
				m_dwRequestLogoutTime	= 0;
			}
		}

		// 로그인 화면으로 이동 요청
		if( m_bRequestExit )
		{
			m_bExit			= TRUE;
			m_bRequestExit	= FALSE;

			return TRUE;
		}

		// 인벤토리 업로드 요청 체크
		if( _GetNetworkMgr()->GetResultOfInventoryUpload() != INVEN_UPLOAD_NOT_RECIVED )
		{
			_GetNetworkMgr()->SystemMsg_MoveoutUserRequest_zone( ZONE_TYPE_GATE );
			_GetNetworkMgr()->SetResultOfInventoryUpload( INVEN_UPLOAD_NOT_RECIVED );

			m_dwRequestDataUploadTime	= 0;
			m_dwRequestLogoutTime		= timeGetTime();

			l_bShowMessage				= TRUE;
		}

		// 메세지 출력
		static int l_nCount = 10;
		if( l_bShowMessage )
		{
			static	DWORD	l_dwRequestCloseTime = timeGetTime();
					DWORD	l_dwCurTime = timeGetTime();

			if( l_dwCurTime - l_dwRequestCloseTime >= 1000 )
			{
				l_dwRequestCloseTime = l_dwCurTime;
				--l_nCount;
				if( l_nCount < 0 )
					l_nCount = 0;

				_SetSystemMsg( "%d초 후에 로그 아웃합니다.", l_nCount + 1 );
			}
		}

		// 서버에서 응답이 올 때까지 대기
		if( _GetNetworkMgr()->GetResultOfMoveOutFromServer() != MOVEOUT_FROM_SERVER_NOT_RECIVED )
		{
			_GetNetworkMgr()->SetResultOfMoveOutFromServer( MOVEOUT_FROM_SERVER_NOT_RECIVED );

			l_nCount				= 10;
			m_bRequestExit			= TRUE;

			l_bShowMessage			= FALSE;

			_GetNetworkMgr()->Close( _GetNetworkMgr()->GetActiveServerID() );
			m_dwRequestLogoutTime	= 0;
		}
	}

	//--------------------------------------------------------------------------

	// 키보드나 마우스 입력이 가능한지를 체크
	if( !IsEnableInput() )
		return FALSE;

	//--------------------------------------------------------------------------
	
	// Keyboard
	m_pInputProcessMgr->InputProcess_Keyboard();


	// User Interface
	_GetUIMouseDev()->Update();
	_GetUIScreen()->InputProc();

	if( g_bChangedLangMode ) 
	{
		gInputMgr.SetToggleLang();
		g_bChangedLangMode = FALSE;
	}

	
	// Mouse
	InputProcess_Mouse();
	

	//--------------------------------------------------------------------------

	// 콘솔 핫키
	::ProgressFunctionKey();

	return TRUE;
}

BOOL
CGP_MainGame::InputProcess_Mouse()
{
	// 현재 Mouse가 Hover상태인 UI의 ID
	m_dwHoverMouseUI = g_dwGUIInputID;

	//--------------------------------------------------------------------------
	// 마우스 인풋 처리
	static	DWORD	l_dwMouseButton			= -1;
	static	BOOL	l_bIsFirstMouseInput	= TRUE;
	//static	BOOL	m_bIsMove				= TRUE;

	if( l_dwMouseButton != ( gMouse.button & 0x7 ) )
	{
		l_dwMouseButton			= gMouse.button & 0x7;
		l_bIsFirstMouseInput	= TRUE;
		
		if( !m_bIsAutoMouse )
			m_bIsMoveByDrag		= FALSE;
	}

	//--------------------------------------------------------------------------

	// 오토 마우스일 경우 ( 프리 타겟 모드에서는 오토 마우스가 동작하지 않는다. )
	if( m_bIsAutoMouse && !m_bIsFreeTargetCombatMode )
	{
		if( l_dwMouseButton & _LEFT_BUTTON_DOWN )
		{
			if( l_bIsFirstMouseInput )
				m_bIsMoveByDrag	= !m_bIsMoveByDrag;

			if( _GetAvatar()->GetActionByCategory() != CAI_DEATH )
			{
				if( l_bIsFirstMouseInput )
				{
					if( ProcessMousePicking() )
					{
						m_bIsMoveByDrag		= FALSE;
						l_bIsFirstMouseInput= FALSE;
						return TRUE;
					}
					else
					{
						l_bIsFirstMouseInput= FALSE;
					}
				}
			}
		}

		// 만약 아바타가 타겟을 추적중이라면 작동하지 않는다.
		//if( _GetAvatar()->GetChasePurpose() != CHASE_FOR_NONE )
		if( !_GetTargetMgr()->IsEqualPursuitPurpose( PPT_NONE ) )
			m_bIsMoveByDrag = FALSE;

		// 체이스 모드가 아닐 경우에만 작동
		//if( _GetAvatar()->GetChasePurpose() == CHASE_FOR_NONE )
		if( _GetTargetMgr()->IsEqualPursuitPurpose( PPT_NONE ) )
		{
			if( m_bIsMoveByDrag )
			{
				Vector3f l_vecTargetPos;
				if( _GetLevel()->GetPointFromScreenRay( gMouse.x, gMouse.y, &l_vecTargetPos ) )
				{
					// 옵져버일경우... 
					if( _GetAvatar()->GetObserverInfo()->m_bIsObserver && ( _GetAvatar()->GetObserverInfo()->m_byObserverMode == OMI_NORMAL ) )
					{
						if( _GetAvatar()->IsEnableMove() )
							_GetNetworkMgr()->ObserverMsg_MoveRequest_clzo( l_vecTargetPos );
					}
					// 아바타일 경우
					else if( !_GetAvatar()->GetObserverInfo()->m_bIsObserver )
					{
						if( _GetAvatar()->IsEnableMove() )
						{
							// Free Target Combat 모드일 경우에는 목적지가 현재위치와 너무 가까운지 체크를 하지 않는다.
							if( m_bIsFreeTargetCombatMode )
								_GetNetworkMgr()->PositionMsg_MoveRequest_clzo( l_vecTargetPos, TRUE, FALSE );
							else
								_GetNetworkMgr()->PositionMsg_MoveRequest_clzo( l_vecTargetPos, TRUE, TRUE );
						}
					}
				}
			}
			else
			{
				if( _GetAvatar()->GetAction() == CAI_MOVE_MOVING )
					_GetNetworkMgr()->PositionMsg_PlayerStop_clzo();
			}
		}
	}
	// 오토 마우스가 아닐 경우
	else
	{
		// 프리타겟모드
		if( m_bIsFreeTargetCombatMode )
		{
			if( l_dwMouseButton & _LEFT_BUTTON_DOWN )
				ProcessMouseClick_FreeTarget( TRUE );
			else if( l_dwMouseButton & _RIGHT_BUTTON_DOWN )
				ProcessMouseClick_FreeTarget( FALSE );
		}
		// 타겟모드
		else
		{
			// 아바타의 행동
			if( l_dwMouseButton & _LEFT_BUTTON_DOWN )
			{
				static BOOL l_bIsPickedObject = FALSE;

				if( _GetAvatar()->GetActionByCategory() != CAI_DEATH )
				{
					if( l_bIsFirstMouseInput && !m_bIsMoveByDrag )
					{
						if( ProcessMousePicking() )
						{
							l_bIsFirstMouseInput = FALSE;
							l_bIsPickedObject = TRUE;
							return TRUE;
						}
						else
						{
							l_bIsPickedObject = FALSE;
						}
					}

					if( !l_bIsPickedObject )
					{
						m_bIsMoveByDrag = TRUE;
						l_bIsFirstMouseInput = FALSE;

						// 현재 마우스로 클릭한 좌표를 3D 좌표로 변환
						Vector3f l_vecTargetPos;
						if( _GetLevel()->GetPointFromScreenRay( gMouse.x, gMouse.y, &l_vecTargetPos ) )
						{
							// 옵져버일경우...
							if( _GetAvatar()->GetObserverInfo()->m_bIsObserver && ( _GetAvatar()->GetObserverInfo()->m_byObserverMode == OMI_NORMAL ) )
							{
								//_GetNetworkMgr()->ObserverMsg_MoveRequest_clzo( l_vecTargetPos );
								_GetNetworkMgr()->PositionMsg_MoveRequest_clzo( l_vecTargetPos, TRUE, FALSE );
							}
							// 아바타일 경우
							else if( !_GetAvatar()->GetObserverInfo()->m_bIsObserver )
							{
								if( _GetAvatar()->IsEnableMove() )
								{
									// Free Target Combat 모드일 경우에는 목적지가 현재위치와 너무 가까운지 체크를 하지 않는다.
									if( m_bIsFreeTargetCombatMode )
										_GetNetworkMgr()->PositionMsg_MoveRequest_clzo( l_vecTargetPos, TRUE, FALSE );
									else
										_GetNetworkMgr()->PositionMsg_MoveRequest_clzo( l_vecTargetPos, TRUE, TRUE );
								}
							}

							// 임시 코드
							g_vecTemp[0] = l_vecTargetPos[0];
							g_vecTemp[1] = l_vecTargetPos[1];
							g_vecTemp[2] = l_vecTargetPos[2];
						}
					}
				}
			}
			else
			{
				l_bIsFirstMouseInput= FALSE;
				m_bIsMoveByDrag		= FALSE;
			}
		}
	}

	//--------------------------------------------------------------------------

	// 카메라의 회전과 관련된 변수들
	static	float	l_fViewAngleX, l_fViewAngleY, l_fPrevViewAngleX, l_fPrevViewAngleY, l_fPointDist = 45.0f;
	static	int		l_nPrevX, l_nPrevY;

	// 카메라의 회전
	if( ( l_dwMouseButton & _RIGHT_BUTTON_DOWN ) && !m_bIsFreeTargetCombatMode && m_dwHoverMouseUI == ID_INVALID )
	{
		_GetCamera()->SetRotatedByMouse( TRUE );

		// 만약 카메라가 키보드에 의해 회전된적이 있다면 현재의 값으로 세팅시켜야만 한다.
		if( _GetCamera()->IsRotatedByKeyboard() )
		{
			l_nPrevX			= gMouse.x + 1;
			l_nPrevY			= gMouse.y;
			l_fPrevViewAngleX	= _GetCamera()->GetYaw();
			l_fPrevViewAngleY	= _GetCamera()->GetPitch();

			_GetCamera()->SetRotatedByKeyboard( FALSE );
		}

		// 현재 마우스의 좌표와 이전 좌표가 틀리다면..
		if( ( gMouse.x != l_nPrevX ) || ( gMouse.y != l_nPrevY ) )
		{
			float l_fXrate = (GetR3State()->mScreenXsize/1024.0f)*0.4f;
			float l_fYrate = (GetR3State()->mScreenYsize/768.0f)*0.6f;
//			float l_fXrate=1,l_fYrate=1;

			l_fViewAngleX = l_fPrevViewAngleX - (( l_nPrevY - gMouse.y ) / 2.0f)*l_fXrate;
			if( ( gMouse.x > 40 ) && ( gMouse.x < ( GetR3State()->mScreenXsize - 40 ) ) )
			{
				l_fViewAngleY = ( l_fPrevViewAngleY + (( l_nPrevX - gMouse.x ) / 2.0f)*l_fYrate );
			}
			else if( gMouse.x <= 40 )
			{
				l_fPrevViewAngleY = l_fViewAngleY;
				l_nPrevX = gMouse.x;
				l_fViewAngleY += 3;
			}
			else
			{
				l_fPrevViewAngleY = l_fViewAngleY;
				l_nPrevX = gMouse.x;
				l_fViewAngleY -= 3;
			}

			// 위아래 회전은 약간의 제한을 둔다.
			if( _GetCamera()->GetCameraMode() != 2 )
			{
				if( l_fViewAngleX < CAM_MIN_TH )
					l_fViewAngleX = CAM_MIN_TH;
			}
			else
			{
				if( l_fViewAngleX < -89 )
					l_fViewAngleX = -89;
			}
			if( l_fViewAngleX > 89 )
			{
				l_fViewAngleX = 89;
			}
		}

		_GetCamera()->SetYaw( l_fViewAngleX );
		_GetCamera()->SetPitch( l_fViewAngleY );

		if( !m_bIsAutoMouse )
		{
			l_bIsFirstMouseInput	= FALSE;
			m_bIsMoveByDrag			= FALSE;
		}
	}

	//미들 버튼 누를경우 거꾸로 본다.
	if( !m_bIsFreeTargetCombatMode && (gMouse.button & _MIDDLE_BUTTON_UP) )
	{
		l_fViewAngleY += 180;
		_GetCamera()->SetPitch( l_fViewAngleY );
	}

	//--------------------------------------------------------------------------

	// 프리 타겟 모드에서는 마우스 클릭없이 회전
	// shift키를 눌렀다면 마우스 커서가 나타나면서 카메라 회전은 되지 않는다.
	else if( m_bIsFreeTargetCombatMode && !( GetKeyState( VK_SHIFT ) & 0x8000 ) )
	{
		// 현재 마우스의 좌표와 이전 좌표가 틀리다면..
		if( ( gMouse.x != l_nPrevX ) || ( gMouse.y != l_nPrevY ) )
		{
			l_fViewAngleX = l_fPrevViewAngleX - ( l_nPrevY - gMouse.y ) / 2.0f;
			//if( ( gMouse.x > 40 ) && ( gMouse.x < ( _GetClientRect()->right - 40 ) ) )
			if( ( gMouse.x > 40 ) && ( gMouse.x < ( GetR3State()->mScreenXsize - 40 ) ) )
			{
				l_fViewAngleY = ( l_fPrevViewAngleY + ( l_nPrevX - gMouse.x ) / 2.0f );
			}
			else
			{
				l_nPrevX			= 400;
				l_nPrevY			= 300;
				l_fPrevViewAngleX	= _GetCamera()->GetYaw();
				l_fPrevViewAngleY	= _GetCamera()->GetPitch();

				SetCursorPos( 400, 300 );
			}

			// 위아래 회전은 약간의 제한을 둔다.
			if( _GetCamera()->GetCameraMode() != 2 )
			{
				if( l_fViewAngleX < CAM_MIN_TH )
					l_fViewAngleX = CAM_MIN_TH;
			}
			else
			{
				if( l_fViewAngleX < -89 )
					l_fViewAngleX = -89;
			}
			if( l_fViewAngleX > 89 )
			{
				l_fViewAngleX = 89;
			}
		}

		_GetCamera()->SetYaw( l_fViewAngleX );
		_GetCamera()->SetPitch( l_fViewAngleY );
	}

	//--------------------------------------------------------------------------

	// 이전 회전값 저장
	else
	{
		l_fPrevViewAngleX	= l_fViewAngleX;
		l_fPrevViewAngleY	= l_fViewAngleY;
		l_nPrevX			= gMouse.x;
		l_nPrevY			= gMouse.y;
	}

	//--------------------------------------------------------------------------

	// hover mouse 처리
	ProcessMouseMove();

	//--------------------------------------------------------------------------
	// 장언일 추가 코드

	//메시지에있는 휠처리를 옮김.
	if( gMouse.z )	//휠체크 
	{
		if( m_dwHoverMouseUI == ID_INVALID )
		{	
			l_fPointDist-=gMouse.z * 0.07f;
			if( l_fPointDist < CAM_MIN_DIST )
				l_fPointDist = CAM_MIN_DIST;
			if( _GetCamera()->GetCameraMode()==1 )
			{
				if( l_fPointDist > CAM_MAX_DIST_FREE )
					l_fPointDist = CAM_MAX_DIST_FREE; 
			}
			else
			{
				if( l_fPointDist > CAM_MAX_DIST )
					l_fPointDist = CAM_MAX_DIST; 
			}
			gMouse.z=0;
			_GetCamera()->SetDistance( l_fPointDist );
		}
		else
		{
			// UI에서 휠을 사용해서 skip..
			gMouse.z=0;
		}
	}

	// 카메라 모드 세팅
	if( m_byOldCameraMode != _GetCamera()->GetCameraMode() )
	{
		if( _GetCamera()->GetCameraMode() == 0 )
		{
			l_fPointDist = CAM_MAX_DIST*0.9f;
			_GetCamera()->SetDistance( l_fPointDist );
		}
		else
		if( _GetCamera()->GetCameraMode() == 1 )
		{
			/*
			l_fPointDist = CAM_MAX_DIST*0.9f;
			_GetCamera()->SetDistance( l_fPointDist );
			l_fViewAngleX=-35;
			l_fViewAngleY= 0;
			_GetCamera()->SetYaw( l_fViewAngleX );
			_GetCamera()->SetPitch( l_fViewAngleY );
			*/
			// => 레이다 & 미니맵과 맞추기 위해서 변경
			_GetCamera()->SetDistance( CAM_MAX_DIST );
			l_fViewAngleX= 40;
			l_fViewAngleY= 180;
			_GetCamera()->SetYaw( l_fViewAngleX );
			_GetCamera()->SetPitch( l_fViewAngleY );
		}
		else
		if( _GetCamera()->GetCameraMode() == 4 )
		{
			l_fPointDist = CAM_MAX_DIST*0.9f;
			_GetCamera()->SetDistance( l_fPointDist );
		}
	}
	//카메라 모드 위치 셋팅 작업...(임시 테스트중.)
	if( _GetCamera()->GetCameraMode() == 0 )
	{
		l_fViewAngleX=14;
		_GetCamera()->SetYaw( l_fViewAngleX );
		_GetCamera()->SetPitch( l_fViewAngleY );
	}
	else if( _GetCamera()->GetCameraMode() == 2 )
	{
		l_fPointDist = 0.001f;
		_GetCamera()->SetDistance( l_fPointDist );
	}
	else if( _GetCamera()->GetCameraMode() == 3 )
	{
		l_fViewAngleX=18;
		_GetCamera()->SetYaw( l_fViewAngleX );
		_GetCamera()->SetPitch( l_fViewAngleY );
		l_fPointDist = CAM_MIN_DIST;
		_GetCamera()->SetDistance( l_fPointDist );
	}
	else if( _GetCamera()->GetCameraMode() == 4 )
	{
//		l_fViewAngleX=46;
//		l_fViewAngleY=315;
		l_fViewAngleX=24;
		_GetCamera()->SetYaw( l_fViewAngleX );
//		_GetCamera()->SetPitch( l_fViewAngleY );
	}

	m_byOldCameraMode = _GetCamera()->GetCameraMode();

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 일정한 시간마다 마우스의 움직임을 체크해서 마우스가 위치한 곳의 오브젝트를 표시한다.
void
CGP_MainGame::ProcessMouseMove( void )
{
	//--------------------------------------------------------------------------
	// 현재는 0.2초 마다 처리
	static	DWORD l_dwPrevTime	= timeGetTime();
			DWORD l_dwCurTime	= timeGetTime();

	if( l_dwCurTime - l_dwPrevTime < 200 )
		return;

	l_dwPrevTime = l_dwCurTime;

	//--------------------------------------------------------------------------

	// 오브젝트의 충돌 처리
	CObject * l_pObject = CollisionCheckWithBoundBox( FALSE );

	// l_pObject가 NULL 이라면 이전의 캐릭터의 색상을 원래대로 복원시킨다.
	if( !l_pObject )
	{
		ProcessMouseMove_Character( NULL );
		ProcessMouseMove_Item( NULL );

		if( !_GetGBM()->GetSelectSF() )
			_GetUIScreen()->SetMouseState( CGUIMouse::NORMAL );
		return;
	}

	// 오브젝트의 타입을 알아낸다.
	switch( l_pObject->GetObjectTypeID() )
	{
		// 캐릭터
		case OTI_CHAR :
			ProcessMouseMove_Character( ( CCharacter * )l_pObject );
			break;

		// 아이템
		case OTI_ITEM :
			ProcessMouseMove_Item( static_cast< CGroundItem * >( l_pObject ) );
			break;
	}
}

void
CGP_MainGame::ProcessMouseMove_Character( CCharacter * pi_pChar )
{
	static const DWORD HOVER_MOUSE_COLOR = D3DCOLOR_XRGB( 200, 200, 200 );

	//----------------------------------------------------------------------

	//static DWORD l_dwHoverCharID[2] = { ID_INVALID, ID_INVALID };
	//static DWORD l_dwHoverCharColor = ID_INVALID;

	//CCharacter * l_pPrevHoverChar = _GetCharMgr()->GetCharacter( l_dwHoverCharID[0], l_dwHoverCharID[1] );
	CCharacter * l_pPrevHoverChar = _GetCharMgr()->GetMouseHoverChar();

	//----------------------------------------------------------------------
	// l_pChar 가 NULL이라면 이전의 캐릭터를 원래대로 복구 시킨다.
	if( !pi_pChar )
	{
		if( l_pPrevHoverChar )
		{
			//l_pPrevHoverChar->SetLightColor( l_dwHoverCharColor );
			l_pPrevHoverChar->Disable( ROSF_RENDER_NAME );
		}

		//l_dwHoverCharID[0] = ID_INVALID;
		//l_dwHoverCharID[1] = ID_INVALID;
		_GetCharMgr()->SetMouseHoverChar( NULL );

		return;
	}

	//----------------------------------------------------------------------
	// 동일한 캐릭터라면 return
	//if( ( l_dwHoverCharID[0] == l_pChar->GetCharTypeID() ) &&
	//	( l_dwHoverCharID[1] == l_pChar->GetIndex() ) )
	if( l_pPrevHoverChar )
	{
		if( l_pPrevHoverChar->IsEqual( pi_pChar ) )
		{
			// 오토 타겟팅 모드일 경우
			if( m_bIsAutoTargeting )
			{
				// 아바타가 움직일때는 적용되지 않는다.
				if( _GetAvatar()->GetAction() != CAI_MOVE_MOVING )
				{
					static	DWORD l_dwPrevTargetTime	= timeGetTime();
							DWORD l_dwCurTime			= timeGetTime();

					if( l_dwCurTime - l_dwPrevTargetTime >= 1000 )
					{
						//_GetAvatar()->SetTarget( pi_pChar );
						_GetTargetMgr()->SetTarget( pi_pChar );
						l_dwPrevTargetTime = l_dwCurTime;
					}
				}
			}

			return;
		}
	}

	//----------------------------------------------------------------------
	// 다른 캐릭터일 경우
	if( l_pPrevHoverChar )
	{
		//l_pPrevHoverChar->SetLightColor( l_dwHoverCharColor );

		//if( !( ( l_dwHoverCharID[0] == CTI_AVATAR ) || ( l_dwHoverCharID[0] == CTI_PLAYER ) ) )
			l_pPrevHoverChar->Disable( ROSF_RENDER_NAME );
	}

	//----------------------------------------------------------------------
	// DEATH
	if( pi_pChar->GetActionByCategory() == CAI_DEATH )
	{
		pi_pChar->SetLightColor( ID_INVALID );
		//return;
	}

	//----------------------------------------------------------------------
	//

	//l_dwHoverCharID[0] = l_pChar->GetCharTypeID();
	//l_dwHoverCharID[1] = l_pChar->GetIndex();
	//l_dwHoverCharColor = l_pChar->GetLightColor();
	_GetCharMgr()->SetMouseHoverChar( pi_pChar );

	pi_pChar->SetLightColor( HOVER_MOUSE_COLOR );
	pi_pChar->Enable( ROSF_RENDER_NAME );

	if( !_GetGBM()->GetSelectSF() )
	{
		if( pi_pChar->IsEqualCharType( CTI_MONSTER ) )
		{
			_GetUIScreen()->SetMouseState( CGUIMouse::ATTACK );
		}
		else if( pi_pChar->IsEqualCharType( CTI_PLAYER ) )
		{
			if( _GetAvatar()->IsMyCompanion( pi_pChar ) )
				_GetUIScreen()->SetMouseState( CGUIMouse::SAME_RACE );
			else
				_GetUIScreen()->SetMouseState( CGUIMouse::ATTACK );
		}
		else if( pi_pChar->IsEqualCharType( CTI_NPC ) )
		{
			_GetUIScreen()->SetMouseState( CGUIMouse::NPC );
		}
	}
}

void
CGP_MainGame::ProcessMouseMove_Item( CGroundItem * pi_pGroundItem )
{
	static const DWORD HOVER_MOUSE_COLOR1 = D3DCOLOR_XRGB( 100, 255, 100 );
	static const DWORD HOVER_MOUSE_COLOR2 = D3DCOLOR_XRGB( 255, 100, 100 );

	//----------------------------------------------------------------------

	//static DWORD l_dwPrevItemIndex = ID_INVALID;
	//static DWORD l_dwPrevItemColor = ID_INVALID;

	CGroundItem * l_pPrevItem = _GetGroundItemMgr()->GetMouseHoverItem();

	//----------------------------------------------------------------------
	// pi_pGroundItem 이 NULL이라면 이전의 아이템을 원래대로 복구 시킨다.
	if( !pi_pGroundItem )
	{
		if( l_pPrevItem )
		{
			//l_pPrevItem->SetLightColor( l_dwPrevItemColor );
			l_pPrevItem->Disable( ROSF_RENDER_NAME );
		}

		//l_dwPrevItemIndex = ID_INVALID;
		_GetGroundItemMgr()->SetMouseHoverItem( NULL );

		return;
	}

	//----------------------------------------------------------------------
	// 동일한 아이템이라면 return
	if( l_pPrevItem )
	{
		if( l_pPrevItem->IsEqual( OTI_ITEM, pi_pGroundItem->GetIndex() ) )
			return;
	}

	//----------------------------------------------------------------------
	// 다른 아이템일 경우
	if( l_pPrevItem )
	{
		//l_pPrevItem->SetLightColor( l_dwPrevItemColor );
		l_pPrevItem->Disable( ROSF_RENDER_NAME );
	}

	//----------------------------------------------------------------------
	//

	//l_dwPrevItemIndex = pi_pGroundItem->GetIndex();
	//l_dwPrevItemColor = pi_pGroundItem->GetLightColor();

	_GetGroundItemMgr()->SetMouseHoverItem( pi_pGroundItem );

	//----------------------------------------------------------------------

	// 아바타가 획득할 수 있는 아이템인지 체크한다.
	if( ( pi_pGroundItem->GetOwnerIndex() == _GetAvatar()->GetIndex() ) ||
		( pi_pGroundItem->GetOwnerIndex() == ID_INVALID ) )
	{
		pi_pGroundItem->SetLightColor( HOVER_MOUSE_COLOR1 );
	}
	else
	{
		pi_pGroundItem->SetLightColor( HOVER_MOUSE_COLOR2 );
	}

	pi_pGroundItem->Enable( ROSF_RENDER_NAME );

	if( !_GetGBM()->GetSelectSF() )
		_GetUIScreen()->SetMouseState( CGUIMouse::PICKUP_ITEM );
}


////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

DWORD	g_pPickedObjectID[2];		// 임시코드 - 마우스로 pick up한 object
char	g_pPickedObjectName[32];	// 임시코드

BOOL
CGP_MainGame::ProcessMousePicking( void )
{
	if( _GetAvatar()->GetObserverInfo()->m_bIsObserver )
		return FALSE;

	CObject	*	l_pObject = NULL;
	BOOL		l_bResult = FALSE;

	g_pPickedObjectID[0] = ID_INVALID;	// 임시코드
	g_pPickedObjectID[1] = ID_INVALID;	// 임시코드

	// Skip GUI event
	if( m_dwHoverMouseUI != ID_INVALID )
		return TRUE;

	/*
	if( _GetFuncKeyWindow()->IsOpenedCharInfoSubkey() )
	{
		_GetFuncKeyWindow()->CloseCharInfoSubkey();
		return TRUE;
	}
	*/

	if( _GetAvatar()->IsDeath() )
		return TRUE;

	// 초기화
	_GetTargetMgr()->CloseWindow( UII_PLAYER_POPUP_MENU );

	//--------------------------------------------------------------------------

	// 오브젝트의 충돌 처리
	l_pObject = CollisionCheckWithBoundBox( TRUE );

	//--------------------------------------------------------------------------

	// 아바타가 스킬/포스를 선택후 클릭이라면...
	if( !l_pObject || ( l_pObject->GetObjectTypeID() == OTI_CHAR ) )
	{
		if( _GetGBM()->GetSelectSF() )
			return ProcessMousePicking_ForCast( ( CCharacter * )l_pObject );
	}

	//--------------------------------------------------------------------------

	// 아바타의 타겟 관련 정보 초기화
	BOOL l_bDoNotClearPursuitInfo = TRUE;
	if( l_pObject )
	{
		if( l_pObject->GetObjectTypeID() == OTI_CHAR )
		{
			if( !_GetTargetMgr()->IsEqualTarget( ( CCharacter * )l_pObject ) )
			{
				if( _GetGBM()->GetSelectSF() )
					l_bDoNotClearPursuitInfo = FALSE;
			}
		}
	}

	if( l_bDoNotClearPursuitInfo )
		_GetTargetMgr()->SetPursuitInfo( PTT_NONE, PPT_NONE );

	//--------------------------------------------------------------------------

	// 아무런 오브젝트도 Pick되지 않았음
	if( !l_pObject )
	{
		return ProcessMousePicking_Map();
	}
	// 오브젝트가 Pick됐을 경우
	else
	{
		// 아바타가 채굴중이라면 그냥 나온다.
		if( _GetAvatar()->IsEqualAction( CAI_ITEM_USE_MINING ) )
			return TRUE;
	}

	//--------------------------------------------------------------------------

	// 오브젝트의 타입을 알아낸다.
	switch( l_pObject->GetObjectTypeID() )
	{
		// 캐릭터
		case OTI_CHAR :
			l_bResult = ProcessMousePicking_Character( ( CCharacter * )l_pObject, FALSE );
			break;

		// 아이템
		case OTI_ITEM :
			l_bResult = ProcessMousePicking_Item( ( CGroundItem * )l_pObject );
			break;
	}

	return l_bResult;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CGP_MainGame::ProcessMousePicking_ForCast( CCharacter * pi_pTarget )
{
	CSFItem * pSelectSF = _GetGBM()->GetSelectSF();
	if( !pSelectSF )
		return FALSE;

	if( !pi_pTarget && !pSelectSF->IsEqualTargetType( SFT_MAP ) )
	{
		_GetTargetMgr()->SetPursuitInfo( PTT_NONE, PPT_NONE );
		ProcessMousePicking_Map();

		return FALSE;
	}
	else
	{
		Vector3f vecSFTargetPos;
		if( pSelectSF->IsEqualTargetType( SFT_MAP ) )
			_GetLevel()->GetPointFromScreenRay( gMouse.x, gMouse.y, &vecSFTargetPos );
		else
			ZeroMemory( vecSFTargetPos, sizeof( Vector3f ) );
		_GetAvatar()->SetSFTargetPos( vecSFTargetPos );

		if( !_GetTargetMgr()->IsEmptyTarget() )
			_GetAvatar()->InstantAction( TRUE );

		_GetLinkBoard()->UI_UseSkillForceRequest( pSelectSF, pi_pTarget );
		return TRUE;
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CGP_MainGame::ProcessMousePicking_Map( void )
{
	Vector3f l_vecTargetPos;

	// 채굴
	if( _GetMiningBoard()->CheckConditionForMining() )
	{
		_GetLevel()->GetPointFromScreenRay( gMouse.x, gMouse.y, &l_vecTargetPos );
		return _GetMiningBoard()->Mining_Request_By_TargetPos( l_vecTargetPos );
	}

	// 아바타가 움직일 수 있는 상태일 경우
	if( _GetAvatar()->IsEnableMove() )
	{
		_GetNextActionMgr()->SetEmpty( TRUE );
		return FALSE;
	}
	// 아바타가 움직일 수 없는 상태일 경우라면 상태를 저장해 둔다.
	else
	{
		_GetLevel()->GetPointFromScreenRay( gMouse.x, gMouse.y, &l_vecTargetPos );
		_GetNextActionMgr()->SetNextAction( CAI_MOVE_MOVING );
		_GetNextActionMgr()->SetNextTargetPosition( l_vecTargetPos );

		return TRUE;
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CGP_MainGame::ProcessMousePicking_Character( CCharacter * pi_pChar, BOOL pi_bIsByKeyboard )
{
	if( !pi_pChar )
		return FALSE;

	BOOL l_bResult = FALSE;

	g_pPickedObjectID[0] = pi_pChar->GetCharTypeID();		// 임시코드
	g_pPickedObjectID[1] = pi_pChar->GetIndex();			// 임시코드
	memcpy( g_pPickedObjectName, pi_pChar->GetName(), 32 );	// 임시코드

	// 아바타가 스킬/포스를 선택후 클릭이라면...
	CSFItem * pSelectSF = _GetGBM()->GetSelectSF();
	if( pSelectSF )
	{
		return ProcessMousePicking_ForCast( pi_pChar );
	}

	// 아바타가 움직일 수 없는 상태라면 Next Action 으로 저장
	if( !_GetAvatar()->IsEnableMove() )
	{
		// 현재 타겟과 틀릴 경우에만 저장
		if( !pi_pChar->IsEqual( _GetTargetMgr()->GetTarget() ) )
		{
			_GetNextActionMgr()->SetNextAction( CAI_NEXTACTION_SELECT_CHAR );
			_GetNextActionMgr()->SetNextTarget( pi_pChar );
			return TRUE;
		}
	}

	switch( pi_pChar->GetCharTypeID() )
	{
		case CTI_AVATAR :	// 아바타가 피킹되는 경우는 스킬이나 포스를 자신에게 사용했을 경우이다.
			if( pSelectSF )
				_GetAvatar()->Attack( pSelectSF->GetSFType(), _GetAvatar() );
			return TRUE;

		case CTI_PLAYER :
			l_bResult = ProcessMousePicking_Character_Player( ( CPlayer * )pi_pChar, pi_bIsByKeyboard );
			break;

		case CTI_MONSTER :
			l_bResult = ProcessMousePicking_Character_Monster( ( CMonster * )pi_pChar, pi_bIsByKeyboard );
			break;

		case CTI_ANIMUS :
			l_bResult = ProcessMousePicking_Character_Animus( ( CAnimus * )pi_pChar, pi_bIsByKeyboard );
			break;

		case CTI_GUARD_TOWER :
			l_bResult = ProcessMousePicking_Character_GuardTower( ( CGuardTower * )pi_pChar, pi_bIsByKeyboard );
			break;

		case CTI_NPC :
			l_bResult = ProcessMousePicking_Character_Npc( ( CNpc * )pi_pChar, pi_bIsByKeyboard );
			break;

		case CTI_UNIT :
			l_bResult = ProcessMousePicking_Character_Unit( ( CPlayer * )pi_pChar, pi_bIsByKeyboard );
			break;
	}

	return l_bResult;
}

BOOL
CGP_MainGame::ProcessMousePicking_Character_Player( CPlayer * pi_pPlayer, BOOL pi_bIsByKeyboard )
{
	// 픽업된 캐릭터가 죽어가는 상태라면 그냥 넘어감.
	// TRUE 로 반환해야 InputProcess()에서 MoveRequest 를 하지 않는다.
	if( pi_pPlayer->GetAction() == CAI_DEATH_READY )
		return TRUE;

	// 픽업된 캐릭터가 죽었다면 그냥 넘어감.
	// FALSE 로 반환해야 InputProcess()에서 MoveRequest 를 한다.
	// 나중에는 루팅 부분으로 바꿔야 되겠쥐?
	if( pi_pPlayer->GetAction() == CAI_DEATH_DIE )
		return FALSE;

	// 픽업된 캐릭터가 내부적으로 DEATH 상태라면 그냥 넘어간다.
	if( pi_pPlayer->GetAction() == CAI_INTERNALSTATE_DEATH )
		return TRUE;

	//--------------------------------------------------------------------------

	// Control 키와 같이 눌렀을 경우에는 메뉴 오픈
	if( GetKeyState( VK_CONTROL ) & 0x8000 )
	{
		//_GetAvatar()->UI_OpenSelectedCharMenu( pi_pPlayer );
		_GetTargetMgr()->SetTarget( pi_pPlayer );
		_GetTargetMgr()->OpenWindow( UII_PLAYER_POPUP_MENU );
		return TRUE;
	}

	//--------------------------------------------------------------------------

	// Free Target Combat Mode라면 타겟을 설정하지 않는다.
	if( !m_bIsFreeTargetCombatMode )
	{
		// 타겟 설정 - GetTarget() 와 pi_pPlayer 가 같다면 다음으로 진행,
		//             틀리다면 GetTarget() 만 설정
		BOOL l_bIsSameTarget = FALSE;
		CCharacter * l_pTarget = _GetTargetMgr()->GetTarget();
		if( l_pTarget )
		{
			if( l_pTarget->IsEqual( pi_pPlayer ) )
			{
				// 자동 공격 모드에서 계속 같은 타겟을 클릭하고 있으면 처리하지 않고, 그냥 나옴
				l_bIsSameTarget = TRUE;
				if( _GetAvatar()->IsEnableAutoAttack() )
				{
					if( ( _GetAvatar()->GetCombatMode() == CSI_PEACE ) && !_GetAvatar()->IsMyCompanion( pi_pPlayer ) )
					{
						_GetAvatar()->SetRequestChangeToCombatMode( TRUE );
					}
					else
					{
						if( _GetGBM()->GetSelectSF() && !pi_bIsByKeyboard )
							_GetTargetMgr()->SetPursuitInfo( PTT_ENEMY, PPT_SKILL_FORCE );
						else
							_GetTargetMgr()->SetPursuitInfo( PTT_ENEMY, PPT_NORMAL_ATTACK );
					}

					if( !_GetAvatar()->IsEnableMove() )
						return TRUE;
				}
			}
		}

		if( !l_bIsSameTarget )
		{
			_GetTargetMgr()->SetTarget( pi_pPlayer );
			return TRUE;
		}
	}

	//--------------------------------------------------------------------------

	// 비전투 모드일 경우
	if( _GetAvatar()->GetCombatMode() == CSI_PEACE )
	{
		// 스킬/포스 사용
		if( _GetGBM()->GetSelectSF() && !pi_bIsByKeyboard )
		{
			// 움직일 수 있는 상태면 곧장 발동
			if( _GetAvatar()->IsEnableMove() )
				_GetAvatar()->Attack( _GetGBM()->GetSelectSF()->GetSFType(), pi_pPlayer );
		}
		// 공격 => 평화 모드라면 서버에 공격 모드로 변경을 요청한 후 공격
		else if( !_GetAvatar()->IsMyCompanion( pi_pPlayer ) )
		{
			_GetAvatar()->SetRequestChangeToCombatMode( TRUE );
		}
	}
	// 전투 모드일 경우
	else
	{
		// Free Target Combat Mode라면 return
		if( !m_bIsFreeTargetCombatMode )
		{
			if( _GetGBM()->GetSelectSF() && !pi_bIsByKeyboard )
			{
				_GetAvatar()->Attack( _GetGBM()->GetSelectSF()->GetSFType(), pi_pPlayer );
			}
			else
			{
				_GetAvatar()->Attack( CAT_NORMAL, pi_pPlayer );
			}
		}
		else
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL
CGP_MainGame::ProcessMousePicking_Character_Monster( CMonster * pi_pMonster, BOOL pi_bIsByKeyboard )
{
	// Free Target Combat Mode라면 return
	if( m_bIsFreeTargetCombatMode )
		return FALSE;

	return ProcessMousePicking_Character_Attack( pi_pMonster, pi_bIsByKeyboard );
}

BOOL
CGP_MainGame::ProcessMousePicking_Character_Animus( CAnimus * pi_pAnimus, BOOL pi_bIsByKeyboard )
{
	if( !pi_pAnimus )
		return FALSE;

	if( pi_pAnimus->IsDeath() )
		return TRUE;

	if( _GetAvatar()->IsMyCompanion( pi_pAnimus ) )
		return TRUE;

	return ProcessMousePicking_Character_Attack( pi_pAnimus, pi_bIsByKeyboard );
}

BOOL
CGP_MainGame::ProcessMousePicking_Character_GuardTower( CGuardTower * pi_pGuardTower, BOOL pi_bIsByKeyboard )
{
	if( !pi_pGuardTower )
		return FALSE;

	if( pi_pGuardTower->IsEqualOwner( _GetAvatar() ) )
	{
		// Control 키와 같이 눌렀을 경우에는 메뉴 오픈
		if( GetKeyState( VK_CONTROL ) & 0x8000 )
		{
			_GetTargetMgr()->SetTarget( pi_pGuardTower );
			_GetTargetMgr()->OpenWindow( UII_PLAYER_POPUP_MENU );
		}
	}

	if( pi_pGuardTower->IsDeath() )
		return TRUE;

	if( _GetAvatar()->IsMyCompanion( pi_pGuardTower ) )
		return TRUE;

	return ProcessMousePicking_Character_Attack( pi_pGuardTower, pi_bIsByKeyboard );
}

BOOL
CGP_MainGame::ProcessMousePicking_Character_Attack( CCharacter * pi_pEnemy, BOOL pi_bIsByKeyboard )
{
	// 타겟 설정 - GetTarget() 와 pi_pEnemy 가 같다면 다음으로 진행,
	//             틀리다면 GetTarget() 만 설정
	BOOL		bIsSameTarget = FALSE;
	CCharacter*	pTarget = _GetTargetMgr()->GetTarget();
	if( pTarget )
	{
		if( pTarget->IsEqual( pi_pEnemy ) )
		{
			// 자동 공격 모드에서 계속 같은 타겟을 클릭하고 있으면 처리하지 않고, 그냥 나옴
			bIsSameTarget = TRUE;
			if( _GetAvatar()->IsEnableAutoAttack() )
			{
				if( _GetAvatar()->GetCombatMode() == CSI_PEACE )
				{
					_GetAvatar()->SetRequestChangeToCombatMode( TRUE );
				}
				else
				{
					if( _GetGBM()->GetSelectSF() && !pi_bIsByKeyboard )
						_GetTargetMgr()->SetPursuitInfo( PTT_ENEMY, PPT_SKILL_FORCE );
					else
						_GetTargetMgr()->SetPursuitInfo( PTT_ENEMY, PPT_NORMAL_ATTACK );
				}

				if( !_GetAvatar()->IsEnableMove() )
					return TRUE;
			}
		}
	}

	if( !bIsSameTarget )
	{
		_GetTargetMgr()->SetTarget( pi_pEnemy );
		return TRUE;
	}

	//--------------------------------------------------------------------------

	// 픽업된 캐릭터가 DEATH 상태라면 그냥 넘어간다.
	// TRUE 로 반환해야 InputProcess()에서 MoveRequest 를 하지 않는다.
	if( pi_pEnemy->IsDeath() )
		return TRUE;

	// 전투 모드가 아니라면 => 서버에 공격 모드로 변경을 요청한 후 공격
	if( _GetAvatar()->GetCombatMode() == CSI_PEACE )
	{
		_GetAvatar()->SetRequestChangeToCombatMode( TRUE );
	}
	// 공격
	else
	{
		if( _GetGBM()->GetSelectSF() && !pi_bIsByKeyboard )
			_GetAvatar()->Attack( _GetGBM()->GetSelectSF()->GetSFType(), pi_pEnemy );
		else
			_GetAvatar()->Attack( CAT_NORMAL, pi_pEnemy );
	}

	return TRUE;
}

BOOL
CGP_MainGame::ProcessMousePicking_Character_Npc( CNpc * pi_pNpc, BOOL pi_bIsByKeyboard )
{
	// 이미 상점 윈도우가 열려져 있다면 리턴
	if( _GetShopTradeBoard()->IsOpen() || _GetPlayerTradeBoard()->IsOpen() ||
		_GetItemWorkBoard()->IsOpen() || _GetItemUpgradeBoard()->IsOpen() ||
		_GetOreProcBoard()->IsOpen() ||
		_GetUnitStorehouseBoard()->IsOpen() || _GetUnitTuningBoard()->IsOpen() ||
		_GetExchangeWindow()->IsShow() || _GetNpcIntroWindow()->IsShow() )
		return TRUE;

	// 물건을 구입할 만한 위치까지 이동한다.
	float l_fDist = GetDist( _GetAvatar()->GetPosition(), pi_pNpc->GetPosition() );
	if( l_fDist	> DISTANCE_OPEN_STORE )
	{
		_GetAvatar()->MoveToTarget( pi_pNpc, l_fDist - DISTANCE_OPEN_STORE + 2.0f );
		_GetNextActionMgr()->SetNextAction( CAI_NEXTACTION_SELECT_CHAR );
		_GetNextActionMgr()->SetNextTarget( pi_pNpc );
		return TRUE;
	}
	else
	{
		// 상점의 아이템 목록과 수량을 요청한다.
		pi_pNpc->UI_OpenIntroduceWindow();
	}

	return TRUE;
}

BOOL
CGP_MainGame::ProcessMousePicking_Character_Unit( CPlayer * pi_pUnit, BOOL pi_bIsByKeyboard )
{
	if( !pi_pUnit )
		return FALSE;

	// Control 키와 같이 눌렀을 경우에는 메뉴 오픈
	if( GetKeyState( VK_CONTROL ) & 0x8000 )
	{
		_GetTargetMgr()->SetTarget( pi_pUnit );
		_GetTargetMgr()->OpenWindow( UII_PLAYER_POPUP_MENU );
	}

	return TRUE;
}

// 아바타가 단축키를 이용해서 타겟없이 공격할 경우 사용
BOOL
CGP_MainGame::ProcessMousePicking_Character_None( BYTE pi_byAttackType )
{
	// m_bIsFreeTargetCombatMode 가 아니라면 return
	if( !m_bIsFreeTargetCombatMode )
		return FALSE;

	// 아바타의 상태 체크
	if( !_GetAvatar()->IsEnableMove() )
		return FALSE;

	if( !_GetAvatar()->CheckAttackDelay( CAT_NORMAL, 0 ) )
		return FALSE;

	// 전투모드가 아니라면 변경 요청
	if( _GetAvatar()->GetCombatMode() == CSI_PEACE )
	{
		_GetAvatar()->SetRequestChangeToCombatMode( TRUE );
		return TRUE;
	}

	//--------------------------------------------------------------------------

	// 타겟이 될 캐릭터를 찾는다.
	CCharacter * l_pTarget = _GetCharMgr()->FindNearestCharacter( _GetAvatar(),
																  _GetAvatar()->GetAttackRange( CAT_NORMAL ) );

	// 타겟이 있다면 공격
	if( l_pTarget )
	{
		_GetTargetMgr()->SetTarget( l_pTarget );
		_GetAvatar()->Attack( pi_byAttackType, l_pTarget );
	}
	// 타겟이 없다면 더미를 만든다.
	else
	{
		CNetMsgProcessor_Combat * pNMP_Combat = static_cast< CNetMsgProcessor_Combat * >( _GetNetworkMgr()->GetNetMsgProcessor( NMP_COMBAT ) );
		pNMP_Combat->AttackTestRequest_clzo( CAT_NORMAL, _GetAvatar()->CreateDummy() );
	}

	return TRUE;
}

BOOL
CGP_MainGame::ProcessMousePicking_Item( CGroundItem * pi_pGroundItem )
{
	return _GetInventoryBoard()->UI_PickUpItemRequest( pi_pGroundItem );
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CGP_MainGame::ProcessMouseClick_FreeTarget( BOOL pi_bLeft )
{
	// Skip GUI event
	if( m_dwHoverMouseUI != ID_INVALID )
		return TRUE;

	// 왼쪽 버튼
	if( pi_bLeft )
	{
		ProcessMousePicking_Character_None( CAT_NORMAL );
	}
	// 오른쪽 버튼
	else
	{
		CSFItem * pBeltSF = static_cast< CSFItem * >( _GetLinkBoard()->GetObject( 0, 0 ) );
		if( !pBeltSF )
			return FALSE;

		// 타겟이 될 캐릭터를 찾는다.
		CCharacter * pTarget = _GetCharMgr()->FindNearestCharacter( _GetAvatar(),
												_GetAvatar()->GetAttackRange( pBeltSF->GetSFType() ) );

		// 타겟이 있다면 공격
		if( pTarget )
		{
			_GetTargetMgr()->SetTarget( pTarget );
			_GetLinkBoard()->UI_UseSkillForceRequest( 0, NULL, TRUE );
		}
		// 타겟이 없다면 더미를 만든다.
		else
		{
			if( !_GetAvatar()->IsEnableMove() )
				return FALSE;

			// 딜레이 체크
			if( _GetAvatar()->CheckAttackDelay( pBeltSF->GetSFType(), pBeltSF ) < TRUE )
				return FALSE;

			// 선택한 포스 설정
			_GetAvatar()->SetSFItem( pBeltSF );

			// 타겟 타입이 SFT_ANYWHERE 라면 무조건 아바타에게만 적용되므로 곧장 발동시킨다.
			if( pBeltSF->IsEqualTargetType( SFT_ANYWHERE ) )
			{
				CNetMsgProcessor_Combat * pNMP_Combat = static_cast< CNetMsgProcessor_Combat * >( _GetNetworkMgr()->GetNetMsgProcessor( NMP_COMBAT ) );
				pNMP_Combat->AttackTestRequest_clzo( pBeltSF->GetSFType(), _GetAvatar() );
			}
			else
			{
				CDummy * pDummy = _GetAvatar()->CreateDummy();
				if( pDummy )
				{
					_GetTargetMgr()->SetTarget( pDummy );

					CNetMsgProcessor_Combat * pNMP_Combat = static_cast< CNetMsgProcessor_Combat * >( _GetNetworkMgr()->GetNetMsgProcessor( NMP_COMBAT ) );
					pNMP_Combat->AttackTestRequest_clzo( pBeltSF->GetSFType(), pDummy );
				}
			}
		}
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

// pi_bIsPicking => TRUE  : 스킬 포스가 선택되었을 경우에만 아바타 체크를 함
//               => FALSE : 아바타 체크 하지 않음
CObject *
CGP_MainGame::CollisionCheckWithBoundBox( BOOL pi_bIsPicking )
{
	Vector3f	l_vecCamPos, l_vecPickedPos;
	CObject *	l_pObject1 = NULL, * l_pObject2 = NULL;

	// 카메라 위치를 얻어온다.
	_GetCamera()->GetR3Camera()->GetPos( l_vecCamPos );

	// 충돌 체크
	l_pObject1= _GetCharMgr()->CollisionCheckWithBoundBox( gMouse.x, gMouse.y,
														   l_vecCamPos,
														   l_vecPickedPos,
														   pi_bIsPicking );

	l_pObject2 = _GetGroundItemMgr()->CollisionCheckWithBoundBox( gMouse.x, gMouse.y,
														   l_vecCamPos,
														   l_vecPickedPos );

	if( l_pObject1 && l_pObject2 )
	{
		if( GetDist( _GetAvatar()->GetPosition(), ((CResObject *)l_pObject1)->GetPosition() ) >
			GetDist( _GetAvatar()->GetPosition(), ((CResObject *)l_pObject2)->GetPosition() ) )
		{
			return ( l_pObject1 = l_pObject2 );
		}
	}
	else if( l_pObject1 )
		return l_pObject1;
	else if( l_pObject2 )
		return l_pObject2;

	return NULL;
}

void
CGP_MainGame::MoveCursor( int pi_nX, int pi_nY, DWORD pi_dwButton )
{
	m_bIsMove_Cursor		= TRUE;

	m_vecCursorCurPos[0]	= gMouse.x;
	m_vecCursorCurPos[1]	= 0;
	m_vecCursorCurPos[2]	= gMouse.y;

	m_vecCursorOrgPos[0]	= gMouse.x;
	m_vecCursorOrgPos[1]	= 0;
	m_vecCursorOrgPos[2]	= gMouse.y;

	m_vecCursorTarPos[0]	= pi_nX;
	m_vecCursorTarPos[1]	= 0;
	m_vecCursorTarPos[2]	= pi_nY;

	m_fCursorAngle_PI		= AngleToPi( GetYAngle( m_vecCursorCurPos, m_vecCursorTarPos ) );

	m_dwMouseButton			= pi_dwButton;

	// button down
	if( m_dwMouseButton != 0xFF )
	{
		DWORD l_lParam = ( ( WORD )m_vecCursorCurPos[2] << 16 ) | ( WORD )m_vecCursorCurPos[0];
		SendMessage( _GetHWND(), MOUSE_BD_MSG[m_dwMouseButton], MOUSE_BUTTON[m_dwMouseButton], l_lParam );	
	}
}