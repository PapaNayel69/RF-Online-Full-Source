////////////////////////////////////////////////////////////////////////////////
//
// CAvatar Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../../GameObject/Manager/CCharacterMgr.h"
#include "../../GameObject/Manager/CTargetMgr.h"
#include "../../GameObject/Board/Common/CBoardManager.h"

#include "../../CLand.h"
#include "../../CCamera.h"

#include "../../UserInterface/GameWindow/Npc/GUINpcSelectWindow.h"
#include "../../UserInterface/GameWindow/GUIChatWindow.h"
#include "../../UserInterface/GUIController/GUIMessageBox.h"
#include "../../UserInterface/GameWindow/GUIHotKeyBoard.h"
#include "../../UserInterface/GameWindow/GUISelectedCharWin.h"

#include "../../GameProgress/CGP_MainGame.h"

#include "../../Network/NetMsgProcessor/Combat/CNetMsgProcessor_Combat.h"
#include "../../Network/NetMsgProcessor/Unit/CNetMsgProcessor_Unit.h"
#include "../../Network/NetMsgProcessor/Resource/CNetMsgProcessor_Resource.h"

#include "CAvatar_Quest.h"

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CAvatar::CAvatar()
{
	m_dwCharTypeID				= CTI_AVATAR;

	m_dwLevel					= 1;
	m_fExp						= 0.0f;

	m_dwPrevMaxHP				= 0;
	m_dwPrevHP					= 0;
	m_dwPrevMaxFP				= 0;
	m_dwPrevFP					= 0;
	m_dwMaxSP					= 0;
	m_dwPrevMaxSP				= 0;
	m_dwPrevSP					= 0;

	m_dwGold					= 0;
	m_dwDalant					= 0;

	m_fTexRate					= 1.0f;
	m_dwExchangeRate			= DEFAULT_EXCHANGE_RATE;
	m_dwExchangeManageValue		= 0;

	for( int i = 0; i < MAX_AVATAR_CLASS_NUM; ++i )
		m_dwClassDTIndex[i]		= ID_INVALID;
	ZeroMemory( m_pClassName, MAX_NAME_LENGTH );

	ZeroMemory( m_dwExpertness, sizeof( DWORD ) * MAX_EXPERTNESS_VALUE_TYPE );

	//--------------------------------------------------------------------------

	m_byAttackMode				= CAT_NORMAL;
	m_bEnableAutoAttack			= FALSE;
	m_bEnableAutoChaseTarget	= FALSE;
	m_bEnableAutoCounterAttack	= FALSE;
	m_dwMoveStartTime			= 0;
	m_dwPrevAttackTime			= 0;
	m_dwPrevAttackDelay			= 0;
	m_byPrevAttackType[0]		= 0xFF;
	m_byPrevAttackType[1]		= 0xFF;
	m_bIsInstantAction			= FALSE;

	//--------------------------------------------------------------------------

	m_fProfitOfTrade			= 0.0f;
	m_bEnable_SendMsgToRace		= FALSE;
	m_nItemEquipLevel			= 0;

	m_dwPotionEffectEndTime		= 0;
	m_pUsedPotionData			= NULL;

	//----------------------------------------------------------------------

	m_byUsingSFCount			= 0;
	m_bIsRequestChangeToCombatMode	= FALSE;

	m_bMoveByKeyboard			= FALSE;

	m_bShowSFDelayMsg			= FALSE;

	//--------------------------------------------------------------------------

	m_pNpcTradeInfo				= NULL;
	m_pMoveMapInfo				= NULL;			
	m_pTutorialInfo				= NULL;
	m_pAfterImageInfo			= NULL;
	m_pChatInfo					= NULL;
	m_pObserverInfo				= NULL;

	m_pNMP_Combat				= NULL;

	//--------------------------------------------------------------------------

	ZeroMemory( m_pSystemMsg, 256 );
	m_dwReciveSystemMsgTime		= 0;

	ZeroMemory( m_pSFUsingMsg, 256 );
	m_dwReciveSFUsingMsgTime	= 0;

	//--------------------------------------------------------------------------

	m_bIsEnableTeleport			= FALSE;
	m_bIsArrivedTargetPosition	= FALSE;
	m_bShowRebirthMessageBox	= FALSE;
	m_stAppliedSF.m_bSetUI		= TRUE;

	//--------------------------------------------------------------------------

	m_dwCharTypeID				= CTI_AVATAR;
	//m_dwTextColor				= CHAR_AVATAR_NAME_COLOR;
	Enable( ROSF_RENDER_MESH );
}

CAvatar::~CAvatar()
{
	_DELETE( m_pNpcTradeInfo );
	_DELETE( m_pMoveMapInfo );			
	_DELETE( m_pTutorialInfo );
	_DELETE( m_pAfterImageInfo );
	_DELETE( m_pChatInfo );
	_DELETE( m_pObserverInfo );
}

void
CAvatar::CreateBoard( void )
{
	if( !m_pNpcTradeInfo )
	{
		m_pNpcTradeInfo			= new NPC_TRADE_INFO;
		m_pMoveMapInfo			= new MOVEMAP_INFO;						
		m_pTutorialInfo			= new QUEST_INFO;
		m_pAfterImageInfo		= new AFTER_IMAGE_INFO;
		m_pChatInfo				= new CHAT_INFO;
		m_pObserverInfo			= new OBSERVER_INFO;
	}

	m_pNMP_Combat	= static_cast< CNetMsgProcessor_Combat * >( _GetNetworkMgr()->GetNetMsgProcessor( NMP_COMBAT ) );
	m_pNMP_Unit		= static_cast< CNetMsgProcessor_Unit * >( _GetNetworkMgr()->GetNetMsgProcessor( NMP_UNIT ) );
}

CCharacter *
CAvatar::New( void )
{
	return ( dynamic_cast< CCharacter * >( new CAvatar ) );
}

CCharacter *
CAvatar::Clone( void )
{
	return ( dynamic_cast< CCharacter * >( new CAvatar( *this ) ) );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// < Function >
//   o 캐릭터를 화면에 렌더링한다.
// < Parameter >
//   o None
// < Return Value >
//   o BOOL - 현재 반환값은 의미를 가지지 못한다.
//------------------------------------------------------------------------------

BOOL
CAvatar::Render( void )
{
	if(	!Render_IsFPSCameraMode() )
	{
		Render_ProcessAlpha();

		if( !CPlayer::Render() )
			return FALSE;
	}

	if( IsDisable( ROSF_AFTER_IMAGE_OF_OBJECT ) )
	{
		if( m_pAfterImageInfo )
			m_pAfterImageInfo->Render();
	}

	return TRUE;
}

BOOL
CAvatar::Render_IsFPSCameraMode( void )
{
	// by J.U.I - 임시 코드.. 카메라가 2번일때 서있거나 움직일땐 캐릭터를 안뿌린다.
	return ( IsEqualActionCategory( CAI_MOVE ) && ( _GetCamera()->GetCameraMode() == 2 ) );
}

void
CAvatar::Render_ProcessAlpha( void )
{
	/* 아바타가 확대 됐을때 일단 보류
	// by J.U.I - 캐릭터 근접될때 반투명처리...
	R3Camera *camera = _GetCamera()->GetR3Camera();
	float char_alpha = camera->GetDist()/20.0f;
	char_alpha=min(char_alpha,1);
	SetAlphaDensity( char_alpha );
	Enable(ROSF_RENDER_ALPHA);
	*/
}

void
CAvatar::RenderText( void )
{
	CCharacter::RenderText();
}

void
CAvatar::RenderInformation( void )
{
#ifdef _DEVELOPER_
	if( IsEnable( ROSF_RENDER_INFO ) )
	{
		CCharacter::RenderInformation();

		RenderModeInfo();
		RenderSFUsingMsg();
		RenderAppliedSFMsg();
	}
#endif
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
// < Function >
//   o 오브젝트의 상태에 따라서 메시 혹은 본 데이터를 애니메이션 시킨다.
// < Parameter >
//   o None
// < Return Value >
//   o None
//--------------------------------------------------------------------------
BOOL
CAvatar::Animation( DWORD pi_dwAniFrame )
{
	if( !CPlayer::Animation( pi_dwAniFrame ) )
		return FALSE;

	if( IsDisable( ROSF_AFTER_IMAGE_OF_OBJECT ) )
	{
		if( m_pAfterImageInfo )
			m_pAfterImageInfo->Animation( static_cast< CPlayer * >( this ) );
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CAvatar::FrameMove( void )
{
	if( !_GetMainGame() )
		return;

	if( IsEqualActionCategory( CAI_MOVE ) )
		FrameMove_Move();
	else if( IsEqualActionCategory( CAI_COMBAT ) )
		FrameMove_Combat();
	else if( IsEqualActionCategory( CAI_SKILL ) || IsEqualActionCategory( CAI_FORCE ) )
		FrameMove_SkillForce();
	else if( IsEqualActionCategory( CAI_DEATH ) )
		FrameMove_Death();
	else if( IsEqualActionCategory( CAI_ITEM ) )
		FrameMove_Item();

	FrameMove_EveryFrame();

	CPlayer::FrameMove();
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CAvatar::FrameMove_Move( void )
{
	if( IsEqualAction( CAI_MOVE_STOP ) )
	{
		FrameMove_Move_Stop();
	}
	else if( IsEqualAction( CAI_MOVE_MOVING ) )
	{
		FrameMove_Move_Moving();
	}
	else if( IsEqualAction( CAI_MOVE_IDLE ) )
	{
		if( !m_Animation.IsAnimating() )
		{
			SetAction( CAI_MOVE_STOP );
			m_dwIdleTime = IDLE_TIME2;
		}
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CAvatar::FrameMove_Move_Stop( void )
{
	if( FrameMove_ProcessNextAction() )
	{
		_GetNextActionMgr()->SetEmpty( FALSE );
		return;
	}

	FrameMove_ProcessPursuit();
}

//==============================================================================

BOOL
CAvatar::FrameMove_ProcessNextAction( void )
{
	if( _GetNextActionMgr()->IsEmpty() )
		return FALSE;

	switch( _GetNextActionMgr()->GetNextAction() )
	{
		case CAI_MOVE_MOVING			: FrameMove_ProcessNextAction_Move(); return TRUE;
		case CAI_COMBAT_ATTACK			: FrameMove_ProcessNextAction_NormalAttack(); return TRUE;
		case CAI_SKILL_INVOKE			:
		case CAI_FORCE_INVOKE			: FrameMove_ProcessNextAction_SkillForce(); return TRUE;
		case CAI_ITEM_USE_MINING		: FrameMove_ProcessNextAction_Mining(); return TRUE;
		case CAI_NEXTACTION_SELECT_CHAR	: FrameMove_ProcessNextAction_SelectCharacter(); return TRUE;
	}

	return FALSE;
}

void
CAvatar::FrameMove_ProcessNextAction_Move( void )
{
	_GetNetworkMgr()->PositionMsg_MoveRequest_clzo( _GetNextActionMgr()->GetNextTargetPosition(), TRUE );

	if( _GetMainGame() )
	{
		if( _GetMainGame()->IsAutoMouse() )
			_GetMainGame()->SetMoveByDrag( TRUE );
	}
}

void
CAvatar::FrameMove_ProcessNextAction_NormalAttack( void )
{
	_GetTargetMgr()->ChangeTargetFromNextTarget();
	AttackToTarget( CAT_NORMAL, _GetTargetMgr()->GetTarget(), TRUE );
}

BOOL
CAvatar::FrameMove_ProcessNextAction_SkillForce( void )
{
	CSFItem * pNextSF = _GetNextActionMgr()->GetNextSF();
	if( !pNextSF )
		return FALSE;

	if( pNextSF->IsEqualSFType( CAT_SKILL ) )
		_GetSkillBoard()->UseSkillForce_Request( pNextSF, _GetNextActionMgr()->GetNextTarget() );
	else
		_GetForceBoard()->UseSkillForce_Request( pNextSF, _GetNextActionMgr()->GetNextTarget() );

	return TRUE;
}

void
CAvatar::FrameMove_ProcessNextAction_Mining( void )
{
	_GetTargetMgr()->SetPursuitInfo( PTT_ORE, PPT_MINING );
	_GetNetworkMgr()->PositionMsg_MoveRequest_clzo( _GetNextActionMgr()->GetNextTargetPosition(), TRUE );
}

void
CAvatar::FrameMove_ProcessNextAction_SelectCharacter( void )
{
	CCharacter * pNextTaret = _GetNextActionMgr()->GetNextTarget();

	_GetTargetMgr()->ChangeTargetFromNextTarget();
	_GetMainGame()->ProcessMousePicking_Character( pNextTaret, FALSE );
}

//==============================================================================

void
CAvatar::FrameMove_ProcessPursuit( void )
{
	if( _GetTargetMgr()->IsEqualPursuitTargetType( PPT_NONE ) )
		return;

	CCharacter * pTarget = _GetTargetMgr()->GetTarget();
	if( !pTarget || pTarget->IsDeath() || pTarget->IsEqualCharType( CTI_AVATAR ) )
	{
		StopTargetPursuit();
		return;
	}

	if( _GetTargetMgr()->IsEqualPursuitPurpose( PPT_PURSUIT ) )
		FrameMove_ProcessPursuit_Pursuit( pTarget );
	else if( _GetTargetMgr()->IsEqualPursuitPurpose( PPT_NORMAL_ATTACK ) )
		FrameMove_ProcessPursuit_AutoAttack( pTarget );
}

BOOL  g_bIsFirstMoving;		// 임시코드
float g_fEnemyDist = 0.0f;	// 임시코드

void
CAvatar::FrameMove_ProcessPursuit_Pursuit( CCharacter * pi_pTarget )
{
	static const float PURSUIT_DISTANCE = 30.0f;

	g_fEnemyDist = GetDist( m_vecPos, pi_pTarget->GetPosition() );
	if( g_fEnemyDist > PURSUIT_DISTANCE )
	{
		if( pi_pTarget->IsEqualAction( CAI_MOVE_MOVING ) )
		{
			MoveToTarget( pi_pTarget, g_fEnemyDist - 10 );
		}
		else if( !IsEqualAction( CAI_MOVE_MOVING ) )
		{
			MoveToTarget( pi_pTarget, g_fEnemyDist - 30 );
		}
	}
}

void
CAvatar::FrameMove_ProcessPursuit_AutoAttack( CCharacter * pi_pTarget )
{
	if( IsEnableAutoAttack() )
		AttackToTarget( CAT_NORMAL, pi_pTarget, TRUE );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CAvatar::FrameMove_Move_Moving( void )
{
	// GetNextAction()이 이동이라면 반드시 ID_INVALID 로 무효화시켜야 한다.
	if( _GetNextActionMgr()->IsEqualNextAction( CAI_MOVE_MOVING ) )
		_GetNextActionMgr()->SetEmpty( TRUE );

	FrameMove_Move_Moving_CloseWindow();

	FrameMove_Move_Moving_ProcessAfterShadow();

	switch( _GetTargetMgr()->GetPursuitTargetType() )
	{
		case PTT_PLAYER			:
		case PTT_ENEMY			: FrameMove_Move_Moving_PursuitEnemy(); break;
		case PTT_MERCHANT		: FrameMove_Move_Moving_PursuitMerchant(); break;
		case PTT_GROUND_ITEM	: FrameMove_Move_Moving_PursuitItem(); break;
		case PTT_ORE			: FrameMove_Move_Moving_PursuitOre(); break;
		default					: if( !_GetTargetMgr()->GetTarget() ) StopTargetPursuit( FALSE );
	}

	g_bIsFirstMoving = m_bIsFirstMoving;
}

void
CAvatar::FrameMove_Move_Moving_CloseWindow( void )
{
	if( !m_bIsPassedFrameMoveLimitTime )
		return;

	if( _GetShopWindow()->IsShow() || _GetOreProcWindow()->IsShow() || _GetExchangeWindow()->IsShow() )
	{
		CCharacter * l_pChar = _GetCharMgr()->GetCharacter( m_pNpcTradeInfo->m_dwTradeCharType,
															m_pNpcTradeInfo->m_dwTradeCharIndex );
		if( l_pChar )
		{
			if( GetDist( m_vecPos, l_pChar->GetPosition() ) >= DISTANCE_OPEN_STORE )
			{				
				_GetShopTradeBoard()->CloseWindow();
				_GetOreProcBoard()->CloseWindow();
				_GetExchangeWindow()->Close();
			}
		}
	}

	if( _GetNpcIntroWindow()->IsShow() )
	{
		CCharacter * l_pChar = _GetCharMgr()->GetCharacter( CTI_NPC, m_pNpcTradeInfo->m_dwCurrentIntroduceNpc );
		if( l_pChar )
		{
			if( GetDist( m_vecPos, l_pChar->GetPosition() ) >= DISTANCE_OPEN_STORE )
				_GetNpcIntroWindow()->Close();
		}
	}
}

void
CAvatar::FrameMove_Move_Moving_ProcessAfterShadow( void )
{
	/*
	if( GetAfterImageInfo()->m_pImage_Remain->GetIndex() != ID_INVALID )
	{
		float	l_fDist = GetDist( GetAfterImageInfo()->m_pImage_Remain->GetPosition(), GetPosition() );
		BOOL	l_bIsMoving = TRUE;
		if( l_fDist <= 10 )
		{
			Enable( ROSF_RENDER_ALPHA );
			SetAlphaDensity( 1.0f - ( 0.1f * l_fDist ) );
			l_bIsMoving = FALSE;
		}
		l_fDist = GetDist( GetPosition(), m_vecNextPath );
		if( l_fDist <= 10 )
		{
			Enable( ROSF_RENDER_ALPHA );
			SetAlphaDensity( 0.1f * ( 10.0f - l_fDist ) );
			l_bIsMoving = FALSE;
		}
		if( l_bIsMoving )
		{
			SetAlphaDensity( 0.0f );
		}
	}*/
}

void
CAvatar::FrameMove_Move_Moving_PursuitEnemy( void )
{
	CCharacter * pTarget = _GetTargetMgr()->GetTarget();
	if( !pTarget || pTarget->IsDeath() )
	{
		StopTargetPursuit();
		return;
	}

	if( _GetTargetMgr()->IsEqualPursuitPurpose( PPT_PURSUIT ) )
	{
		FrameMove_ProcessPursuit_Pursuit( pTarget );
	}
	else if( _GetTargetMgr()->IsEqualPursuitPurpose( PPT_NORMAL_ATTACK ) )
	{
		AttackToTarget( CAT_NORMAL, pTarget, TRUE );
	}
	else if( _GetTargetMgr()->IsEqualPursuitPurpose( PPT_SKILL_FORCE ) )
	{
		if( _GetGBM()->GetSelectSF() )
			AttackToTarget( _GetGBM()->GetSelectSF()->GetSFType(), pTarget, TRUE );
	}
}

/*
void
CAvatar::FrameMove_Move_Moving_PursuitEnemy_Pursue( CCharacter * pi_pTarget )
{
	g_fEnemyDist = GetDist( m_vecPos, pi_pTarget->GetPosition() );
	if( g_fEnemyDist > 30 )
	{
		if( pi_pTarget->IsEqualAction( CAI_MOVE_MOVING ) )
			MoveToTarget( pi_pTarget, g_fEnemyDist - 10 );
		//else
		//	MoveToTarget( pi_pTarget, g_fEnemyDist - 20 );
	}
	else
	{
		//StopTargetPursuit();
		//_GetNetworkMgr()->PositionMsg_PlayerStop_clzo();
	}
}*/

void
CAvatar::FrameMove_Move_Moving_PursuitMerchant( void )
{
	if( !m_bIsPassedFrameMoveLimitTime )
		return;

	if( _GetTargetMgr()->IsEqualPursuitTargetType( PTT_MERCHANT ) )
	{
		CNpc * l_pMerchant = static_cast< CNpc * >( _GetTargetMgr()->GetTarget() );
		if( !l_pMerchant )
		{
			StopTargetPursuit();
			return;
		}

		g_fEnemyDist = GetDist( m_vecPos, l_pMerchant->GetPosition() );
		if( g_fEnemyDist <= DISTANCE_OPEN_STORE )
		{
			StopTargetPursuit();
			l_pMerchant->UI_OpenIntroduceWindow();
		}
	}
}

void
CAvatar::FrameMove_Move_Moving_PursuitItem( void )
{
	CGroundItem * pTargetItem = _GetTargetMgr()->GetTargetItem();
	if( !pTargetItem )
	{
		StopTargetPursuit();
		return;
	}

	// Pickup 메세지를 보낸다. => 먼저 아이템 을 루팅한 후에, 아이템이 있던 지점까지 이동 후 멈춤
	if( _GetInventoryBoard()->UI_PickUpItemRequest( pTargetItem ) )
		StopTargetPursuit();
}

void
CAvatar::FrameMove_Move_Moving_PursuitOre( void )
{
	g_fEnemyDist = ::GetDist( m_vecPos, _GetMiningBoard()->m_vecMiningPosition );
	if( g_fEnemyDist <= DISTANCE_MINING )
	{
		StopTargetPursuit();

		_GetMiningBoard()->Mining_Request();		
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CAvatar::FrameMove_Combat( void )
{
	if( IsEqualAction( CAI_COMBAT_ATTACK ) )
	{
		if( !m_Animation.IsAnimating() )
			m_pAfterImageInfo->DisableRender();
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CAvatar::FrameMove_SkillForce( void )
{
	// 스킬의 마지막 이펙트 프레임이 지나갔다면 잔상을 사라지게 한다.
	if( IsEqualAction( CAI_SKILL_INVOKE ) )
	{
		if( IsPassedLastEffectFrame() )
			m_pAfterImageInfo->DisableRender();
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CAvatar::FrameMove_Death( void )
{
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CAvatar::FrameMove_Item( void )
{
	// 채굴
	if( IsEqualAction( CAI_ITEM_USE_MINING ) )
	{
		// 채굴중이라면 채굴 완료 메세지 전송
		if( _GetMiningBoard()->IsMining() )
		{
			_GetMiningBoard()->Mining_Complete_Request();

		}
		// 채굴중이지 않다면 채굴 시작 요청 메세지 전송
		else
		{
			if( !_GetMiningBoard()->Mining_Request( FALSE ) )
				SetAction( CAI_MOVE_STOP );			
		}
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CAvatar::FrameMove_EveryFrame( void )
{
	FrameMove_Portal();

	FrameMove_Quest();

	FrameMove_FreeTargetMode();

	FrameMove_ItemBoard();

	FrameMove_Target();

	FrameMove_CheckUsedPotion();
}

void
CAvatar::FrameMove_Portal( void )
{
	static	DWORD l_dwPrevSendTime = 0;
			DWORD l_dwCurSendTime = timeGetTime();

	if( !m_pMoveMapInfo )
		return;

	m_pMoveMapInfo->FrameMove();

	if( IsDeath() )
		return;
	if( !IsEnableMove() )
		return;

	if( m_pMoveMapInfo->m_byMoveStep != MOVEMENT_BETWEEN_MAP_INFO::MMS_NONE )
		return;

	// 포탈 이동 전
	if( !m_pMoveMapInfo->m_bIsMoved )
	{
		if( _GetLand()->GetMapIndex() == 0xFF )
			return;

		PORTAL_INFO *	l_pPortalInfo = NULL;
		int				l_nPortalNum = _GetLand()->GetMaxPortalNum( _GetLand()->GetMapIndex() );

		static	BYTE	l_byPrevUsedPortalNo = 0xFF;
				BYTE	l_byCurUsedPortalNo = 0xFF;

		//----------------------------------------------------------------------
		// 사용가능한 포탈 설정
		for( int i = 0; i < l_nPortalNum; ++i )
		{
			l_pPortalInfo = _GetLand()->GetPortalInfo( _GetLand()->GetMapIndex(), i );
			if( l_pPortalInfo )
			{
				l_pPortalInfo->bIsUsable = FALSE;

				if( !IsEnableTeleport() )
				{
					if( !l_pPortalInfo->bIsMovable )
						continue;

					if( !l_pPortalInfo->byUsableRace[m_byRace] )
						continue;

					if( ( GetPosX() >= l_pPortalInfo->vecMin[0] ) &&
						( GetPosZ() >= l_pPortalInfo->vecMin[2] ) &&
						( GetPosX() <= l_pPortalInfo->vecMax[0] ) &&
						( GetPosZ() <= l_pPortalInfo->vecMax[2] ) )
					{
						l_pPortalInfo->bIsUsable	= TRUE;
						l_byCurUsedPortalNo			= l_pPortalInfo->byIndex;
					}
					else
					{
						l_pPortalInfo->bIsUsable = FALSE;
					}
				}
				else
				{
					l_pPortalInfo->bIsUsable	= TRUE;
					l_byCurUsedPortalNo			= l_pPortalInfo->byIndex;
				}
			}
		}

		// 맵 이동 UI
		if( l_byPrevUsedPortalNo != l_byCurUsedPortalNo )
		{
			l_byPrevUsedPortalNo = l_byCurUsedPortalNo;
			_GetLand()->UI_OpenMoveMapWindow();
		}
	}
	// 포탈 이동 후
	else
	{
	}
}

void
CAvatar::FrameMove_Quest( void )
{
	if( !m_pTutorialInfo )
		return;

	m_pTutorialInfo->ProcessQuest();
}

void
CAvatar::FrameMove_FreeTargetMode( void )
{
	// Free Target 전투 모드일 경우 => 아바타는 가만히 서있을 경우에는 카메라의 회전에 따라 Y축 값을 바꾼다.
	if( _GetMainGame() && _GetMainGame()->IsFreeTargetCombatMode() )
	{
		if( IsEqualAction( CAI_MOVE_STOP ) || IsEqualAction( CAI_MOVE_IDLE ) )
		{
			float fCameraRotY = ( int )_GetCamera()->GetPitch() % 360;
			if( fabs( m_vecRot[1] - fCameraRotY ) >= 10 )
				SetRotY( 360 - fCameraRotY );
			else
				SetRotY( 360 - fCameraRotY );
		}
	}
}

void
CAvatar::FrameMove_ItemBoard( void )
{
}

void
CAvatar::FrameMove_Target( void )
{
#ifdef _DEVELOPER_
	static	DWORD dwPrevSendTime = 0;
			DWORD dwCurSendTime = timeGetTime();

	if( dwCurSendTime - dwPrevSendTime > 1000 )
	{
		_GetNetworkMgr()->SyncMsg_ObjectServerPosRequest_clzo( _GetTargetMgr()->GetTarget() );
		dwPrevSendTime = dwCurSendTime;
	}
#endif
}

void
CAvatar::FrameMove_CheckUsedPotion( void )
{
	if( m_dwPotionEffectEndTime )
	{
		if( timeGetTime() >= m_dwPotionEffectEndTime )
		{
			if( m_pUsedPotionData )
				_SetSystemMsg( "%s의 효과가 해제 되었습니다.", m_pUsedPotionData->pName );

			m_stConditionInfo.ApplyPotionEffect( m_pUsedPotionData, FALSE );

			m_dwPotionEffectEndTime = 0;
			m_pUsedPotionData		= NULL;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

float
CAvatar::PositionMove( Vector3f pi_vecTargetPos )
{
	float l_fResult = CCharacter::PositionMove( pi_vecTargetPos );

	// 퀘스트에 필요한 사항
	if( m_pTutorialInfo )
	{
		m_pTutorialInfo->QSV.m_fTargetDistance -= l_fResult;
	}

	// 옵져버일 경우
	if( m_pObserverInfo->m_bIsObserver && ( m_pObserverInfo->m_byObserverMode == OMI_FLY ) )
	{
		if( m_pObserverInfo->m_byKeyDownCode == CMK_UP )
			m_vecPos[1] -= AngleToPi( _GetCamera()->GetYaw() ) * GetRealSpeed();
		else if( m_pObserverInfo->m_byKeyDownCode == CMK_DOWN )
			m_vecPos[1] += AngleToPi( _GetCamera()->GetYaw() ) * GetRealSpeed();
	}

	// 일정 시간마다 위치를 서버로 보낸다.
	if( l_fResult != -1 )
	{
		DWORD l_dwCurTime = timeGetTime();
		if( l_dwCurTime - m_dwMoveStartTime > 500 )
		{
			_GetNetworkMgr()->PositionMsg_RealMovposRequest_clzo();
			m_dwMoveStartTime = l_dwCurTime;
		}
	}

	return l_fResult;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CAvatar::StopTargetPursuit( BOOL pi_bIsSetAction )
{
	_GetTargetMgr()->ResetPursuitInfo();

	if( pi_bIsSetAction )
	{
		if( m_dwActionID == CAI_MOVE_MOVING )
			_GetNetworkMgr()->PositionMsg_PlayerStop_clzo();
	}
}

void
CAvatar::AttackToTarget( BYTE pi_byAttackType, CCharacter * pi_pTarget, BOOL pi_bIsMoveToTarget )
{
	if( !pi_pTarget )
		return;

	g_fEnemyDist = ::GetDist( m_vecPos, pi_pTarget->GetPosition() );
	if( g_fEnemyDist <= GetAttackRange( pi_byAttackType ) )
	{
		Attack( pi_byAttackType, pi_pTarget );
	}
	else
	{
		if( pi_bIsMoveToTarget )
		{
			g_fEnemyDist -= GetAttackRange( pi_byAttackType ) - 5.0f;
			MoveToTarget( pi_pTarget, g_fEnemyDist );
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CAvatar::SetEffectInfo( DWORD pi_dwActionID )
{
	return CPlayer::SetEffectInfo( pi_dwActionID );
}

void
CAvatar::SetAction( DWORD pi_dwActionID, BOOL pi_bCheckActionDuplication )
{
	if( pi_dwActionID == ID_INVALID )
		return;

	switch( pi_dwActionID >> 28 )
	{
		case CAI_MOVE	: SetAction_Move( pi_dwActionID ); break;
		case CAI_COMBAT	: SetAction_Combat( pi_dwActionID ); break;
		case CAI_SKILL	: SetAction_Skill( pi_dwActionID ); break;
		case CAI_FORCE	: SetAction_Force( pi_dwActionID ); break;
		case CAI_DEATH	: SetAction_Death( pi_dwActionID ); break;
	}

	m_bShowSFDelayMsg = FALSE;

	CPlayer::SetAction( pi_dwActionID, pi_bCheckActionDuplication );
}

void
CAvatar::SetAction_Move( DWORD pi_dwActionID )
{
	if( pi_dwActionID == CAI_MOVE_STOP )
	{
		m_bMoveByKeyboard = FALSE;

		if( m_pAfterImageInfo )
			m_pAfterImageInfo->DisableRender();
	}
	else if( pi_dwActionID == CAI_MOVE_MOVING )
	{
		if( m_pAfterImageInfo )
			m_pAfterImageInfo->DisableRender();

		m_dwMoveStartTime = timeGetTime();
	}
}

void
CAvatar::SetAction_Combat( DWORD pi_dwActionID )
{
	if( pi_dwActionID == CAI_COMBAT_ATTACK )
	{
		m_dwPrevAttackTime		= timeGetTime();
		m_dwPrevAttackDelay		= GetAttackDelay( NULL );
		m_byPrevAttackType[0]	= CAT_NORMAL;
		// m_byPrevAttackType[1]	= ID_INVALID; // 여기서 => m_byPrevAttackType[1] 은 설정해서는 안 된다.
	}
}

void
CAvatar::SetAction_Skill( DWORD pi_dwActionID )
{
	if( pi_dwActionID == CAI_SKILL_INVOKE )
	{
		m_dwPrevAttackTime		= timeGetTime();
		m_dwPrevAttackDelay		= GetAttackDelay( &m_clSFItem );
		m_byPrevAttackType[0]	= CAT_SKILL;
		m_byPrevAttackType[1]	= m_clSFItem.GetDTIndex();
		_GetSkillBoard()->SetCastTime( m_byPrevAttackType[1], m_dwPrevAttackTime );

		// 공격형일 경우에만 해당
		if( m_clSFItem.IsAttackable() )
			m_pAfterImageInfo->EnableRender( this );
	}
}

void
CAvatar::SetAction_Force( DWORD pi_dwActionID )
{
	if( pi_dwActionID == CAI_FORCE_INVOKE )
	{
		m_dwPrevAttackTime		= timeGetTime();
		m_dwPrevAttackDelay		= GetAttackDelay( &m_clSFItem );
		m_byPrevAttackType[0]	= CAT_FORCE;
		m_byPrevAttackType[1]	= m_clSFItem.GetDTIndex();
		_GetForceBoard()->SetCastTime( m_byPrevAttackType[1], m_dwPrevAttackTime );
	}
}

void
CAvatar::SetAction_Death( DWORD pi_dwActionID )
{
	if( pi_dwActionID == CAI_DEATH_READY )
	{
		_GetGBM()->CloseUnnecessaryWindow();
		UI_DeselectSF();
		_GetTargetMgr()->SetTarget( NULL );
		_GetTargetMgr()->SetTargetItem( NULL );
		_GetNextActionMgr()->SetEmpty( TRUE );
	}
	else if( pi_dwActionID == CAI_DEATH_DIE )
	{
		if( !_IsOpenedDialogBox( NULL, MSGBOX_ID_REBIRTH ) )
			_GUIMessageBox( NULL, "부활 하시겠습니까?", 0xb0ffffff, MSGBOX_ID_REBIRTH, DLG_STYLE_OK );
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

LRESULT
CAvatar::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
		case WM_AVATAR_TOGGLE_COMBAT_MODE	:
			ToggleCombatMode();
			break;

		case WM_AVATAR_TOGGLE_MOVE_MODE		:
			ToggleMoveMode();
			break;

		case WM_AVATAR_TOGGLE_AUTO_ATTACK	:
			m_bEnableAutoAttack = !m_bEnableAutoAttack;
			if( m_bEnableAutoAttack )
				_GetFuncKeyWindow()->ChangeAttackMode( CGUIHotKeyBoard::Auto );
			else
				_GetFuncKeyWindow()->ChangeAttackMode( CGUIHotKeyBoard::Manual );
			break;

		case WM_AVATAR_UNIT_DELIVERY		:
			_GetUnitStorehouseBoard()->UI_DeliveryUnitRequest( m_clsUsingItem.GetIndex() );
			break;

		case WM_AVATAR_UNIT_KEEPING			:
			_GetUnitStorehouseBoard()->UI_KeepingUnitRequest();
			break;

		case WM_AVATAR_UNIT_MOUNT			:
			MountUnitRequest( m_pLinkPlayer );
			break;

		case WM_AVATAR_UNIT_DISMOUNT		:
			DismountUnitRequest();
			break;

		case WM_AVATAR_UNIT_TRACTION		:
			break;

		case WM_AVATAR_UNIT_SELECT_WEAPON	:
			m_clsUnitProc.SetAttackParts( IEPT_UNIT_ARMS + wParam );
			break;

		case WM_AVATAR_GUARDTOWER_TAKEIN	:
			_GetNetworkMgr()->EffectMsg_BackTowerRequest_clzo();
			break;
	}

	return 0;
}
