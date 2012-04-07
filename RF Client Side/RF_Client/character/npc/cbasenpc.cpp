////////////////////////////////////////////////////////////////////////////
//
// CBaseNpc Class Implementation
//
////////////////////////////////////////////////////////////////////////////

#include "../../GameObject/Manager/CCharacterMgr.h"
#include "../../CLand.h"

#include "../../AddCodeByJang/MonsterSpecialEffect.h"
#include "../../AddCodeByJang/EffectUtil.h"

#include "../../AddCodeByYun/ShadowList.h"
#include "../../AddCodeByYun/ParticleList.h"

#include "../../UserInterface/GameWindow/EtcWin/GUIRadarWindow.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CBaseNpc::CBaseNpc()
{
	m_pMesh		= NULL;
	m_pData		= NULL;

	m_byForceNo	= 0;

	Enable( ROSF_RENDER_MESH );

	// by J.U.I
	m_dwEFUniqueID		= 0;				//효과 더미.
	m_bCheckExistEF		= FALSE;
	for( int i = 0; i < 8; ++i )			//효과더미 인덱스들
		m_byDummy[i]	= 0;
}

CBaseNpc::~CBaseNpc()
{
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CBaseNpc::Render( void )
{
	if( !CCharacter::Render() )
		return FALSE;

	if( ( IsEnable( ROSF_RENDER_MESH ) ) && ( IsDisable( ROSF_CLIPPED ) ) )
	{
		if( !m_pMesh )
			return FALSE;

		// 캐릭터가 죽어가는 경우라면 알파값을 점차 줄여준다.
		if( IsEqualAction( CAI_DEATH_DIE ) )
			m_fAlphaDensity -= 0.01f;

		// 알파 적용
		if( IsEnable( ROSF_RENDER_ALPHA ) )
			_GetCharIF()->SetAlpha( m_pMesh, m_fAlphaDensity );

		// 머터리얼 적용
		_GetCharIF()->SetMaterial( m_pMesh, m_d3dMaterial );

		// vertex blending
		if( !SetVertexBlending( 0, m_pMesh, FALSE ) ) 
		{
			if( GetAction() == CAI_DEATH_DIE || IsEnable( ROSF_RENDER_ALPHA ) )
				_GetCharIF()->SetAlpha( m_pMesh, 1.0f );
			return TRUE;
		}

		EFUtil_Set2ndLight( this );		// by J.U.I - 타격시 2번째 라이팅 킴

		Render_DrawMesh();
	
		EFUtil_PushShadow(this);	//일단 쉐도우 뿌릴거 리스트를 만든다.함수 안에서 쉐도옵션에따라 작동.

		EFUtil_UnSet2ndLight();

		// 메시를 그린후 다시 알파값을 복구시킨다.
		// 복구시키지 않으면 같은 메시를 사용하는 다른 오브젝트도 사라져 보인다.
		if( IsEqualAction( CAI_DEATH_DIE ) || IsEnable( ROSF_RENDER_ALPHA ) )
			_GetCharIF()->SetAlpha( m_pMesh, 1.0f );
	}

	Render_ProcessEffectAfterRender();
	return TRUE;
}

// J.U.I < begin > -------------------------------------------------------------

void
CBaseNpc::Render_ProcessEffectAfterRender( void )
{
	CCharacter::SetPrevAction( CCharacter::GetAction() );	// 액션 변하기전에 넣어준다.
	
}

// MAYUN < begin > -------------------------------------------------------------

void
CBaseNpc::Render_DrawMesh( void )
{
	// draw mesh
	static	float scroll = 0.0f;
	static	float enscroll = 0.0f;
	//DWORD	ctime = timeGetTime();
	float	ctime = R3GetTime();
	static	float	btime = 0.0f;
	//static	DWORD	btime = 0.0f;
	if (btime==0.0f)btime = ctime;

	float delta = ((float)(ctime - btime)) * 0.0005f;
	
	btime = ctime;
	scroll += delta ;
						
	if (scroll>1.0)	scroll = 0.0;
	_GetCharIF()->InitEffect(m_pMesh);
	if ( IsEqualCharType( CTI_ANIMUS ) ) 
	{
		g_AniEff.SetEffect(m_pMesh,GetDTIndex(), (((CAnimus*)this)->GetLevel()/5) *5 );
	}
	else
	{
		g_EFFMonList.SetEffect(m_pMesh,GetDTIndex());
	}
	//g_EFFMonList.SetEffect(m_pMesh,84);
	//SetMeshEffect( m_pMesh, CItem::GetDTIndex(), 0 );
	float alphad = _GetCharIF()->GetAlpha( m_pMesh );
	_GetCharIF()->SetAlpha( m_pMesh,alphad );
	_GetCharIF()->DrawCharacter( m_pMesh, m_vecPos, m_vecRot[1], m_fScale,scroll );
	CRender::GetRender().UnSetState();	
	
	ResetVertexBufferCache();
	DrawCharacterctEffect( m_pMesh, m_vecPos, m_vecRot[1],alphad, m_fScale );
	_GetCharIF()->SetState();
}

// 그림자 생성
void
CBaseNpc::CreateShadow( void )
{
/*
	D3DXMATRIX  View,Proj;
	D3DXMATRIX	rot;
	float dg=74.0f;
	LPDIRECT3DDEVICE8 pDevice = GetD3dDevice();
		
	D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 0.0f, dg*12, dg*4 );
	D3DXVECTOR4 out;
	D3DXMatrixRotationY(&rot,D3DXToRadian( 180));	
		
	D3DXVec3Transform(&out,&vEyePt,&rot);
	D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	D3DXMatrixLookAtLH( &View, (D3DXVECTOR3*)&out, &vLookatPt, &vUpVec );
	D3DXMatrixPerspectiveFovLH( &Proj, D3DX_PI/32.0f, GetR3State()->mAsfectRatio, 1.0f, VIEWER_FAR_PLANE );
	pDevice->SetTransform( D3DTS_PROJECTION, &Proj );
	pDevice->SetTransform( D3DTS_VIEW, &View );

	// 렌더링이 가능하고 클리핑 되지 않았어야 렌더링 함.
	//if( ( IsEnable( ROSF_RENDER_MESH ) ) && ( IsDisable( ROSF_CLIPPED ) ) )
	//if( ( IsEnable( ROSF_RENDER_MESH ) ) && ( IsDisable( ROSF_CLIPPED ) ) )
	//if( !ProcessClipping() )
	{
	
		if( !m_pMesh )	return;
		// vertex blending
		if( !SetVertexBlending( 0, m_pMesh, FALSE ) ) 
		{
			return;
		}
		// draw mesh
		_GetCharIF()->DrawCharacterShadow( m_pMesh, m_vecPos, m_vecRot[1],0.3f * GetAlphaDensity(), m_fScale );
	}
	
	pDevice->SetTransform( D3DTS_VIEW, &GetR3State()->mMatView );
	pDevice->SetTransform( D3DTS_PROJECTION, &GetR3State()->mMatProj );
*/
	SetVertexBlending( 0, m_pMesh, FALSE );
	_GetCharIF()->DrawCharacterShadow( m_pMesh, m_vecPos, m_vecRot[1],
									   0.6f * m_fAlphaDensity, m_fScale );
}

// 그림자 렌더링
void
CBaseNpc::RenderShadow( void )
{
	/*
	_GetCharIF()->UnSetState();
	for (int i = 0;i<g_ShadowList.GetShadowNum(); i++)
	{
		if (_GetCharIF()->GetShadow(i+1))
			_GetLand()->GetLevel()->mBsp->DrawShadowRender(g_ShadowList.GetShadowPos(i),_GetCharIF()->GetShadow(i+1));
	}
	_GetCharIF()->SetState();
	g_ShadowList.ResetShadow();
	*/
}

// MAYUN < end > ---------------------------------------------------------------

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CBaseNpc::Animation( DWORD pi_dwAniFrame )
{
	if( !Animation_CheckBoneAndMesh() )
		return FALSE;

	if( !CCharacter::Animation( pi_dwAniFrame ) )
		return FALSE;

	if( !m_pMesh )
		return FALSE;

	Animation_ProcessEffectBeforeAnimation();

	if( IsDisable( ROSF_ANIMATION_STOP_ENGINE ) )
	{
		_GetCharIF()->FrameMove( m_pMesh );
		SetVertexBlending( 0, m_pMesh, TRUE );
	}

	return TRUE;
}

BOOL
CBaseNpc::Animation_CheckBoneAndMesh( void )
{
	if( !m_pBone || !m_pMesh )
	{
		SetBone( GetMeshID() );
		SetMesh( GetMeshID() );
	}

	return TRUE;
}

// J.U.I < begin > -------------------------------------------------------------

void
CBaseNpc::Animation_ProcessEffectBeforeAnimation( void )
{
	// 특별한 몬스터의 효과,효과음 처리등...
	::CMonsterSpecialEffectFrameMove((void*)this);

	// 몬스터에 들러붙는 효과들 처리
	if( !m_bCheckExistEF && m_pMesh->m_Load )
	{
		m_bCheckExistEF=TRUE;
		m_dwEFUniqueID = ::EFUtil_GetUniqueIDFromMonster((void*)this); // 0을 리턴하면 효과가 없는경우다...
		//CBone * l_pBone = _GetBoneMgr()->GetBone( MMI_MONSTER, GetBoneID() );
		//::CMonsterSpecialEffectCreateDummy((void*)this, l_pBone );	// 장언일 추가 코드 몬스터 더미들 처리다.
		::CMonsterSpecialEffectCreateDummy((void*)this, m_pMesh );	//장언일 추가 코드 몬스터 더미들 처리다.
	}
	if( m_dwEFUniqueID )	// 유닉크 아이디가 있다.( 들러붙는 효과가 있다.)
		::CMonsterSpecialEffectFrameMove((void*)this,m_dwEFUniqueID);
}

// J.U.I < end > ---------------------------------------------------------------

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CBaseNpc::FrameMove( void )
{
	if( IsDisable( ROSF_MOVABLE ) )
		return;

	if( IsEqualActionCategory( CAI_FORCE ) )
		FrameMove_Force();

	CCharacter::FrameMove();
}

void
CBaseNpc::FrameMove_Force( void )
{
	if( IsEqualAction( CAI_FORCE_INVOKE ) )
	{
		if( IsEffectFrame() )
			FrameMove_Attack_SetAttacker( CAI_FORCE_INVOKE, FALSE, FALSE );

		// 스킬 / 포스 사용 후에는 stop 모션으로 돌아온다.
		if( !m_Animation.IsAnimating() )
		{
			SetAction( CAI_MOVE_STOP );
			m_Animation.SetAnimationSpeed( 1.0f );
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CBaseNpc::SetEffectInfo( DWORD pi_dwActionID )
{
	if( CCharacter::SetEffectInfo( pi_dwActionID ) )
		return TRUE;

	EffectInfo stEffectInfo;

	switch( pi_dwActionID >> 28 )
	{
		case CAI_CREATE		: SetEffectInfo_Create( stEffectInfo, pi_dwActionID ); break;
		case CAI_DEATH		: SetEffectInfo_Death( stEffectInfo, pi_dwActionID ); break;
		case CAI_COMBAT		: SetEffectInfo_Combat( stEffectInfo, pi_dwActionID ); break;
		case CAI_FORCE		: SetEffectInfo_Force( stEffectInfo, pi_dwActionID ); break;
		case CAI_DAMAGED	: SetEffectInfo_Damaged( stEffectInfo, pi_dwActionID ); break;
		case CAI_GESTURE	: SetEffectInfo_Gesture( stEffectInfo, pi_dwActionID ); break;
	}

	return PushEffectInfoToQueue( stEffectInfo );
}

//==============================================================================

void
CBaseNpc::SetEffectInfo_Create( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID )
{
	po_stEffectInfo.dwType	= GetEffectType();
	po_stEffectInfo.dwID	= GetNpcEffectID( GetEffectTypeID( pi_dwActionID ) );
}

//==============================================================================

void
CBaseNpc::SetEffectInfo_Death( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID )
{
	po_stEffectInfo.dwType	= GetEffectType();
	po_stEffectInfo.dwID	= GetNpcEffectID( GetEffectTypeID( pi_dwActionID ) );
}

//==============================================================================

void
CBaseNpc::SetEffectInfo_Combat( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID )
{
	po_stEffectInfo.dwType	= GetEffectType();
	po_stEffectInfo.dwID	= GetNpcEffectID( GetEffectTypeID( pi_dwActionID ) );
	po_stEffectInfo.SetTarget( m_pCombatState->GetCurEnemy( ET_VICTIM ) );
}

//==============================================================================

void
CBaseNpc::SetEffectInfo_Force( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID )
{
	po_stEffectInfo.dwType	= ETI_FORCE;
	po_stEffectInfo.dwID	= GetNpcForceEffectID( pi_dwActionID, &m_clSFItem );

	po_stEffectInfo.byRangeType_For_Effect = m_clSFItem.GetRangeTypeForEffect();

	// set target
	SetEffectInfo_Force_SetTarget( po_stEffectInfo );

	// set range & angle
	SetEffectInfo_Force_SetRangeAndAngle( po_stEffectInfo );
}

void
CBaseNpc::SetEffectInfo_Force_SetTarget( EffectInfo & po_stEffectInfo )
{
	// 자신에게만 쓸 수 있는 기술일경우..
	if( m_clSFItem.IsEqualTargetType( SFT_ANYWHERE ) )
	{
		po_stEffectInfo.SetTarget( this );
	}
	// 지역기일 경우..
	else if( m_clSFItem.IsEqualTargetType( SFT_MAP ) )
	{
		po_stEffectInfo.SetTarget( NULL );
	}
	// 타겟에게 쓸 수 있는 기술일경우..
	else
	{
		po_stEffectInfo.SetTarget( m_pCombatState->GetCurEnemy( ET_VICTIM ) );
	}
}

void
CBaseNpc::SetEffectInfo_Force_SetRangeAndAngle( EffectInfo & po_stEffectInfo )
{
	// 스킬/포스의 범위 or 각도
	static const float SELFAREA_EXTENT_RANGE[4] = { 42.0f, 56.0f, 70.0f, 84.0f };
	static const float FLASH_ANGLE[4] = { 15.0f, 30.0f, 45.0f, 60.0f };

	// 스킬/포스의 이펙트 거리 or 각도
	po_stEffectInfo.byRangeType = m_clSFItem.GetRangeType();

	if( ( po_stEffectInfo.byRangeType == SFRT_SELF_AREA ) || ( po_stEffectInfo.byRangeType == SFRT_EXTENT ) )
	{
		po_stEffectInfo.fRange = SELFAREA_EXTENT_RANGE[m_clSFItem.GetStep()];
	}
	else if( po_stEffectInfo.byRangeType == SFRT_FLASH_DAMAGE )
	{
		po_stEffectInfo.fRange = FLASH_ANGLE[m_clSFItem.GetStep()];
	}
}

//==============================================================================

void
CBaseNpc::SetEffectInfo_Damaged( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID )
{
	po_stEffectInfo.dwType	= GetEffectType();

	CAttackerInfo * pAttackerInfo = static_cast< CAttackerInfo * >( m_pCombatState->GetCurEnemyInfo( ET_ATTACKER ) );
	if( !pAttackerInfo )
		return;

	po_stEffectInfo.dwID	= GetNpcEffectID( GetEffectTypeID( pi_dwActionID ) );
	po_stEffectInfo.dwValue	= pAttackerInfo->GetCurDamagePoint();

	po_stEffectInfo.SetTarget( pAttackerInfo->GetEnemy() );	// 타겟에 공격자를 넣어서 보낸다.
}

//==============================================================================

void
CBaseNpc::SetEffectInfo_Gesture( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID )
{
	po_stEffectInfo.dwType	= GetEffectType();
	po_stEffectInfo.dwID	= GetNpcEffectID( GetEffectTypeID( pi_dwActionID ) );
}

//==============================================================================

BOOL
CBaseNpc::PushEffectInfoToQueue( EffectInfo & pi_stEffectInfo )
{
	if( pi_stEffectInfo.dwID == ID_INVALID )
		return FALSE;

	pi_stEffectInfo.SetMyself( this );

	_GetEffectInfoList()->push_back( &pi_stEffectInfo );

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

DWORD
CBaseNpc::GetNpcEffectID( DWORD pi_dwEffectType )
{
	DWORD dwEffectID = 0;

	dwEffectID = GetMeshID() << 8;
	dwEffectID |= pi_dwEffectType << 4;
	dwEffectID |= m_Animation.GetAnimationID() & 0x0000000F;	// 애니메이션의 순서과 맞춰야한다.

	return dwEffectID;
}

// 스킬/포스 이펙트는 외부에서도 호출될 수 있으므로 CSFItem 을 파라미터로 받아야만 한다.
DWORD
CBaseNpc::GetNpcForceEffectID( DWORD pi_dwActionID, CSFItem * pi_pSFItem )
{
	if( !pi_pSFItem )
		return ID_INVALID;

	DWORD dwEffectID = pi_pSFItem->GetDTCode() << 8;

	switch( pi_dwActionID )
	{
		case CAI_FORCE_CAST			: return dwEffectID |= pi_pSFItem->GetLevel() - 1;
		case CAI_FORCE_INVOKE		: return dwEffectID |= pi_pSFItem->GetLevel() - 1 + 7;
		case CAI_DAMAGED_BY_FORCE	: return dwEffectID |= pi_pSFItem->GetLevel() - 1 + 14;
	}

	return ID_INVALID;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CBaseNpc::SetAction( DWORD pi_dwActionID, BOOL pi_bCheckActionDuplication )
{
	if( pi_dwActionID == ID_INVALID )
		return;

	// check duplication
	if( SetAction_IsDuplicateAction( pi_dwActionID, pi_bCheckActionDuplication ) )
		return;

	// pi_dwActionID 에 따른 애니메이션 설정 전 선행처리
	SetAction_ProcessActionID( pi_dwActionID );

	// set animation
	if( SetAction_WillSetAnimation( pi_dwActionID ) )
	{
		CAniData * pNewAniData = GetAnimationData( pi_dwActionID, GetAnimationTypeID( pi_dwActionID ) );
		if( pNewAniData )
			SetAction_SetAnimationInterpolation( pi_dwActionID, pNewAniData );
	}

	// set action
	CCharacter::SetAction( pi_dwActionID );

	// set effect
	SetAction_SetEffectAtferSetAnimation( pi_dwActionID );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CBaseNpc::SetAction_IsDuplicateAction( DWORD pi_dwActionID, BOOL pi_bCheckActionDuplication )
{
	if( !pi_bCheckActionDuplication )
		return FALSE;

	if( !IsEqualActionCategory( CAI_COMBAT ) &&
		!IsEqualActionCategory( CAI_SKILL ) &&
		!IsEqualActionCategory( CAI_FORCE ) &&
		( m_dwActionID == pi_dwActionID ) )
		return TRUE;

	return FALSE;
}

BOOL
CBaseNpc::SetAction_WillSetAnimation( DWORD pi_dwActionID )
{
	return TRUE;
}

void
CBaseNpc::SetAction_SetEffectAtferSetAnimation( DWORD pi_dwActionID )
{
	switch( pi_dwActionID )
	{
		case CAI_FORCE_INVOKE :
			SetEffectInfo( CAI_FORCE_CAST );		// 포스는 처음 사용시 애니는 CAI_FORCE_INVOKE, 이펙트는 CAI_FORCE_CAST 로 나간다.
			break;
	}
}

void
CBaseNpc::SetAction_SetAnimationInterpolation( DWORD pi_dwActionID, CAniData * pi_pNewAniData )
{
	if( pi_pNewAniData )
	{
		if( pi_dwActionID == CAI_DEATH_DIE )
			SetAnimationInterpolation( pi_pNewAniData, TRUE );
		else
			SetAnimationInterpolation( pi_pNewAniData, FALSE );

		m_Animation.SetAniData( pi_pNewAniData );
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CAniData *
CBaseNpc::GetAnimationData( DWORD pi_dwActionID, DWORD pi_dwAniTypeID )
{
	CAniData * pNewAnimationData = NULL;

	if( ( pi_dwActionID >> 28 ) == CAI_FORCE )
		pNewAnimationData = GetAnimationData_Force( pi_dwAniTypeID );
	else
		pNewAnimationData = GetAnimationData_Normal( pi_dwAniTypeID );

	if( !pNewAnimationData )
		m_Animation.SetAnimationID( ID_INVALID );

	return pNewAnimationData;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CBaseNpc::SetData( BASE_DATA * pi_pData )
{
	m_pData = pi_pData ;
}

DWORD
CBaseNpc::GetMeshID( void )
{
	if( m_pMesh )
		return m_pMesh->m_ID;

	return ID_INVALID;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

float
CBaseNpc::GetAttackRange( BYTE pi_byAttackType, BOOL pi_bCheckTarget )
{
	return CCharacter::GetAttackRange( pi_byAttackType, pi_bCheckTarget );
}

/*
inline DWORD
StrToInt( char * pi_pStr )
{
	if( !pi_pStr )
		return 0;

	int		l_nStrLen = strlen( pi_pStr );
	DWORD	l_dwDigit = 0;
	for( int i = 0; ( i < 8 ) && ( pi_pStr[i] != NULL ) ; ++i )
	{
		if( pi_pStr[i] >= '0' && pi_pStr[i] <= '9' )
			l_dwDigit |= ( pi_pStr[i] - '0' ) << ( ( l_nStrLen - 1 - i ) * 4 );
		else if( pi_pStr[i] >= 'a' && pi_pStr[i] <= 'z' )
			l_dwDigit |= ( pi_pStr[i] - 'a' + 10 ) << ( ( l_nStrLen - 1 - i ) * 4 );
		else if( pi_pStr[i] >= 'A' && pi_pStr[i] <= 'Z' )
			l_dwDigit |= ( pi_pStr[i] - 'A' + 10 ) << ( ( l_nStrLen - 1 - i ) * 4 );
	}

	return l_dwDigit;
}

#define FindForce( pi_nNo, pi_strID, pi_dwDTIndex ) \
{ \
	FORCE_DATA * l_pData = _GetCharDataMgr()->GetForceByID( StrToInt( m_pData->pi_strID ) ); \
	if( l_pData ) \
	{ \
		if( l_pData->dwDTIndex == pi_dwDTIndex ) \
		{ \
			m_byForceNo = pi_nNo; \
			return TRUE; \
		} \
	} \
}*/

//------------------------------------------------------------------------------

BOOL
CBaseNpc::IsHaveForce( DWORD pi_dwForceDTIndex )
{
	/*
	if( m_pData )
	{
		FindForce( 0, m_strAttTechID1, pi_dwForceDTIndex );
		FindForce( 1, m_strAttTechID2, pi_dwForceDTIndex );
		FindForce( 2, m_strAttTechID3, pi_dwForceDTIndex );
		FindForce( 3, m_strPSecTechID, pi_dwForceDTIndex );
		FindForce( 4, m_strMSecTechID, pi_dwForceDTIndex );
	}*/

	return FALSE;
}

void
CBaseNpc::Enable( DWORD pi_dwFlag )
{
	CCharacter::Enable( pi_dwFlag );

	if( IsEnable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT ) && _GetRadarWindow() )
		_GetRadarWindow()->RemoveCharacter( m_dwCharTypeID, m_dwIndex );
}

void
CBaseNpc::Disable( DWORD pi_dwFlag )
{
	CCharacter::Disable( pi_dwFlag );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CBaseNpc::IsPassedLimitTimeFromReceiveNetworkMsg( void )
{
	const static DWORD MONSTER_AUTO_DESTROY_TIME = 10000;

	if( !IsRemovableWhenWasNotReciveNetworkMsg() )
		return FALSE;

	if( m_stETCInfo.m_dwReceiveMsgTime != ID_INVALID )
	{
		if( _GetTime() - m_stETCInfo.m_dwReceiveMsgTime > MONSTER_AUTO_DESTROY_TIME )
			return TRUE;
	}

	return FALSE;
}

BOOL
CBaseNpc::IsPassedLimitTimeForDelete( void )
{
	const static DWORD MONSTER_DELETE_TIME = 300000;		// default 5분
	
	if( m_stETCInfo.m_dwReceiveMsgTime != ID_INVALID )
	{
		if( _GetTime() - m_stETCInfo.m_dwReceiveMsgTime > MONSTER_DELETE_TIME )
			return TRUE;
	}

	return FALSE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CBaseNpc::UI_SetRadar( void )
{
	if( !_GetRadarWindow() || ( m_dwIndex == ID_INVALID ) )
		return;
	if( IsDeath() )
		return;

	_GetRadarWindow()->AddCharacter( m_dwCharTypeID, m_dwIndex,
									 ( CGUIRadarWindow::CharType )UI_GetRadarCharType(),
									 m_vecPos[0], m_vecPos[1], m_vecPos[2] );
}

BYTE
CBaseNpc::UI_GetRadarCharType( void )
{
	return CGUIRadarWindow::Mob;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 중요한 애니메이션은 미리 로딩을 해 둔다.
void
CBaseNpc::LoadAnimaitonData( void )
{
	CAniData * pAni = NULL;

	pAni = GetAnimationData( m_dwActionID, MAI_PEACE_STOP );
	if( pAni ) pAni->LoadRealAnimation( TRUE );
	pAni = GetAnimationData( m_dwActionID, MAI_PEACE_WALK );
	if( pAni ) pAni->LoadRealAnimation( TRUE );
	pAni = GetAnimationData( m_dwActionID, MAI_COMBAT_ATTACK );
	if( pAni ) pAni->LoadRealAnimation( TRUE );
	pAni = GetAnimationData( m_dwActionID, MAI_DEATH_READY );
	if( pAni ) pAni->LoadRealAnimation( TRUE );
	pAni = GetAnimationData( m_dwActionID, MAI_DEATH_DIE );
	if( pAni ) pAni->LoadRealAnimation( TRUE );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////
