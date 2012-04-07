////////////////////////////////////////////////////////////////////////////////
//
// CResObject Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "CResObject.h"
#include "./AddCodeByJang/EffectUtil.h"
#include "./AddCodeByJang/EffectCore.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////


CAnimation::CAnimation()
{
	ZeroMemory( this, sizeof( CAnimation ) );

	m_dwAnimationID			= ID_INVALID;
	m_fAnimationSpeed		= 1.0f;

	for( int i = 0; i < MAX_EFFECT_FRAME_NUM; ++i )
		m_dwEffectFrame[i]	= ID_INVALID;
}

void
CAnimation::LoadData( void )
{
	if( m_pAniData )
		if( !m_pAniData->m_Load )
			m_pAniData->LoadRealAnimation( TRUE );
}

void
CAnimation::ResetEffectFrame( DWORD pi_dwBoneFrame )
{
	if( pi_dwBoneFrame <= 160 )
	{
		m_nCurrentFrameNo	= 0;
		for( int i = 0; i < MAX_EFFECT_FRAME_NUM; ++i )
			m_bIsInvokedEffect[i] = FALSE;
	}
}

BOOL
CAnimation::IsPassedCurrentEffectFrame( DWORD pi_dwBoneFrame )
{
	if( m_dwEffectFrame[m_nCurrentFrameNo] == ID_INVALID )
		return TRUE;

	if( pi_dwBoneFrame >= m_dwEffectFrame[m_nCurrentFrameNo] )
		return TRUE;

	return FALSE;
}

BOOL
CAnimation::IsPassedLastEffectFrame( DWORD pi_dwBoneFrame )
{
	if( m_dwEffectFrame[m_nMaxEffectFrameNum-1] == ID_INVALID )
		return TRUE;

	if( pi_dwBoneFrame >= m_dwEffectFrame[m_nMaxEffectFrameNum-1] )
		return TRUE;

	return FALSE;
}

BOOL
CAnimation::IsEffectFrame( DWORD pi_dwBoneFrame )
{
	if( !m_bIsAnimating || !m_pAniData )
		return FALSE;

	if( ( m_nMaxEffectFrameNum <= 0 ) || ( m_nMaxEffectFrameNum >= MAX_EFFECT_FRAME_NUM ) || ( m_nCurrentFrameNo >= m_nMaxEffectFrameNum ) )
		return FALSE;

	if( m_dwEffectFrame[m_nCurrentFrameNo] == ID_INVALID )
		return FALSE;

	if( pi_dwBoneFrame >= m_dwEffectFrame[m_nCurrentFrameNo] )
		return TRUE;

	return FALSE;
}

void
CAnimation::SetAniData( CAniData * pi_pAniData )
{
	m_pAniData = pi_pAniData;

	if( !pi_pAniData )
		return;

	ANI_EXT_INFO * l_pAniExtInfo = ( ANI_EXT_INFO * )pi_pAniData->m_pExtra;
	if( !l_pAniExtInfo )
		return;

	m_nMaxEffectFrameNum = l_pAniExtInfo->nMaxFrameNum;
	for( int i = 0; i < m_nMaxEffectFrameNum; ++i )
		m_dwEffectFrame[i] = l_pAniExtInfo->dwEffectFrame[i];
}

char
CAnimation::GetRemainEffectFrameNum( void )
{
	if( m_nMaxEffectFrameNum <= 0 )
		return 0;

	return ( m_nMaxEffectFrameNum - ( m_nCurrentFrameNo - 1 ) );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
Y_INTERPOLATION_INFO::Init( void )
{
	ZeroMemory( this, sizeof( Y_INTERPOLATION_INFO ) );
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CResObject::CResObject()
{
	ZeroMemory( m_vecPos, sizeof( Vector3f ) );
	ZeroMemory( m_vecPrevPos, sizeof( Vector3f ) );
	ZeroMemory( m_vecRot, sizeof( Vector3f ) );

	ResetStatusFlag();

	m_pBone							= NULL;
	m_pBBox							= NULL;
	//m_dwBoneID					= ID_INVALID;
	m_dwBoneFrame					= 160;
	m_dwOldBoneFrame				= 0;
	m_dwFrameNumForAStopTime		= 0;

	m_fScale						= 1.0f;

	m_dwTextColor					= D3DCOLOR_XRGB( 255, 255, 255 );

	m_bIsAniInterpolation			= FALSE;
	m_pAniIPInfo					= NULL;

	ZeroMemory( &m_d3dMaterial, sizeof( D3DMATERIAL8 ) );
	ZeroMemory( &m_d3dLight, sizeof( D3DLIGHT8 ) );
	m_dwLightColor					= ID_INVALID;
	m_dwPrevLightColor				= ID_INVALID;
	m_fAlphaDensity					= 1.0f;

	m_bIsBlink						= FALSE;
	m_bIsHide						= FALSE;
	m_dwPrevBlinkTime				= 0;
	m_dwCurBlinkTime				= 0;

	m_bIsReceiveFixPositionMsg		= FALSE;
	m_dwReceiveFixPositionMsgTime	= 0;

	m_dwEffectID[0]					= 0;
	m_dwEffectID[1]					= 0;

	//--------------------------------------------------------------------------

	Enable( ROSF_ANIMATION | ROSF_ANIMATION_LOOP | ROSF_RENDER_EFFECT );
	Disable( ROSF_RENDER_BONE | ROSF_CLIPPED | ROSF_RENDER_BBOX |
			 ROSF_OUT_OF_VISUAL_FIELD_SERVER | ROSF_OUT_OF_VISUAL_FIELD_CLIENT |
			 ROSF_ANIMATION_INTERPOLATION );
}

CResObject::~CResObject()
{
	DeleteEffectID();	//만의하나JUI
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 매 프레임마다 초기에 설정해 주어야할 데이터들
void
CResObject::Init_EveryFrame( void )
{
}

//--------------------------------------------------------------------------
// < Function >
//   o 메시 혹은 본 데이터를 화면에 렌더링한다.
// < Parameter >
//   o None
// < Return Value >
//   o BOOL - 현재 반환값은 의미를 가지지 못한다.
//--------------------------------------------------------------------------
BOOL
CResObject::Render( void )
{
	// 렌더링이 가능하고 클리핑 되지 않았어야 렌더링 함.
	if( ( IsEnable( ROSF_RENDER_BONE ) ) && ( IsDisable( ROSF_CLIPPED ) ) )
	{
		if( m_pBone )
			_GetCharIF()->DrawCharacter( m_pBone, m_vecPos, m_vecRot[1] );
	}

	/*
	if( m_bIsBlink )
	{
		m_dwCurBlinkTime = timeGetTime();
		if ( m_dwCurBlinkTime - m_dwPrevBlinkTime > 100 )
		{
			if( m_bIsHide )
			{
				Disable( ROSF_RENDER_MESH );
			}
			else
			{
				Enable( ROSF_RENDER_MESH );
			}

			m_bIsHide = !m_bIsHide;
			m_dwPrevBlinkTime = m_dwCurBlinkTime;
		}
	}*/

	// 바운드 박스 렌더링
	if( IsEnable( ROSF_RENDER_BBOX ) )
	{
		BBOX l_sBBox;

		if( GetBoundBox( l_sBBox ) )
		{
			_GetCharIF()->UnSetState();
			if( _GetLevel() )
				_GetLevel()->DrawBBox( l_sBBox.pMin, l_sBBox.pMax, D3DCOLOR_XRGB( 100, 100, 100 ) );
			_GetCharIF()->SetState();
		}
	}

	return TRUE;
}

// 이름 출력
BOOL
CResObject::RenderName( void )
{
	if( IsEnable( ROSF_RENDER_NAME ) && IsEnable( ROSF_RENDER_MESH ) )
	{
		char l_pBuf[32];
		sprintf( l_pBuf, "%s", m_pName );

		Vector4f l_vecPos;
		_GetRenderNamePos( this, l_vecPos, strlen( l_pBuf ) );

		DrawR3Hangul3D( l_vecPos, l_pBuf, m_dwTextColor, R3_HAN_OUTLINE );
	}

	return TRUE;
}

// 이름을 출력할 좌표를 계산해서 가져온다.
void
CResObject::GetRenderNamePos( Vector4f pi_vecPos, BYTE pi_bySizeOfName )
{
	Vector3f	l_vecNamePos;
	BBOX		l_sBBox;

	memcpy( l_vecNamePos, m_vecPos, sizeof( float ) * 3 );

	if( GetBoundBox( l_sBBox ) )
		l_vecNamePos[1] = l_sBBox.pMax[1];
	else
		l_vecNamePos[1] += 19.0f;

	GetTransformVertex( ( Vector4f * )pi_vecPos, l_vecNamePos );
	pi_vecPos[0] = (float)( (LONG)( pi_vecPos[0] + 0.5f ) ) - ( ( (float)pi_bySizeOfName / 2.0f ) * 7.0f );
	pi_vecPos[1] = (float)( (LONG)( pi_vecPos[1] ) ) - 20.0f;
}

//------------------------------------------------------------------------------
// < Function >
//   o 오브젝트의 상태에 따라서 메시 혹은 본 데이터를 애니메이션 시킨다.
// < Parameter >
//   o None
// < Return Value >
//   o None
//------------------------------------------------------------------------------
BOOL
CResObject::Animation( DWORD pi_dwAniFrame )
{
	// 애니메이션 가능 상태라면 처리
	if( IsEnable( ROSF_ANIMATION ) )
	{
		if( !m_pBone )
			return FALSE;

		//----------------------------------------------------------------------
		// 같은 본을 사용하기 때문에 매 루프마다 캐릭터들의 본 프레임, 애니메이션
		// 을 각각 설정해 주어야 한다.
		if( !m_Animation.IsEmpty() )
		{
			m_Animation.LoadData();

			ResetAnimation( m_Animation.GetAniData() );
		}
		else
		{
			_GetCharIF()->FrameMove( m_pBone );
			return FALSE;
		}

		//----------------------------------------------------------------------

		// 프레임 계산전에 이펙트 프레임을 세팅해 줘야 한다.
		// 캐릭터의 경우 SetAction() 에서 m_dwBoneFrame = 160 으로 설정하므로,
		// CalculateCurrentAnimationFrame() 다음에 호출되면 이펙트 프레임이
		// 리셋되지 않을 경우가 있다.
		m_Animation.ResetEffectFrame( m_dwBoneFrame );

		// calculate bone frame
		CalculateCurrentAnimationFrame( pi_dwAniFrame );

		// frame move
		if( IsDisable( ROSF_ANIMATION_STOP_ENGINE ) )
			_GetCharIF()->FrameMove( m_pBone );

		return TRUE;
	}

	return FALSE;
}

void
CResObject::CalculateCurrentAnimationFrame( DWORD pi_dwAniFrame )
{
	if( !m_pBone )
		return;

	m_Animation.Animating( TRUE );

	m_dwOldBoneFrame = m_dwBoneFrame;

	//----------------------------------------------------------------------
	// bone의 animation frame 설정

	// stop animation 상태가 아니라면 bone frame 세팅
	if( IsDisable( ROSF_ANIMATION_STOP ) )
	{
		// 애니메이션 보간이 아닐경우
		if( !m_bIsAniInterpolation )
		{
			if( pi_dwAniFrame == ID_INVALID )
				m_dwBoneFrame += ( ( GetCharLoopTime() * 5 ) + m_dwFrameNumForAStopTime ) * m_Animation.GetAnimationSpeed(); // Max 에서 160 tick 이 1 frame, 160 / 5는 1ms에 해당됨
			else
				m_dwBoneFrame = pi_dwAniFrame;
		}
		// 애니메이션 보간일 경우
		else if( m_pAniIPInfo && IsEnable( ROSF_ANIMATION_INTERPOLATION ) )
		{
			if( pi_dwAniFrame == ID_INVALID )
				m_pAniIPInfo->dwAniInterpolationFrame += ( ( GetCharLoopTime() * 5 ) + m_dwFrameNumForAStopTime ) * m_Animation.GetAnimationSpeed();
			else
				m_dwBoneFrame = pi_dwAniFrame;
			
			if( m_pAniIPInfo->dwAniInterpolationFrame >= m_pAniIPInfo->dwAniInterpolationTime )
			{
				m_pAniIPInfo->dwAniInterpolationFrame	= 0;
				m_pAniIPInfo->dwAniInterpolationTime	= 0;
				m_bIsAniInterpolation					= FALSE; 
			}
		}

		// 마지막 프레임 설정
		if( m_dwBoneFrame >= m_pBone->m_OMaxFrame )
		{
			if( IsEnable( ROSF_ANIMATION_LOOP ) )
			{
				m_dwBoneFrame		= 160;
				m_dwOldBoneFrame	= 0;
			}
			else
				m_dwBoneFrame = m_pBone->m_OMaxFrame;

			m_Animation.Animating( FALSE );		// 애니메이션의 한 루프 종료
		}

		// m_dwFrameNumForAStopTime 리셋
		m_dwFrameNumForAStopTime = 0;
	}
	// stop animation 상태라면
	else
	{
		m_dwFrameNumForAStopTime += 0;//( GetCharLoopTime() * 5 );
	}

	// 본 프레임을 본에 저장
	m_pBone->m_Frame = m_dwBoneFrame;
}

void
CResObject::FrameMove( void )
{
	if( IsDisable( ROSF_MOVABLE ) )
		return;

	//----------------------------------------------------------------------
	// Position Y 값
	if( m_stPosYIPInfo.m_fYPerFrame != 0 )
	{
		m_stPosYIPInfo.m_fCurIncDecY += m_stPosYIPInfo.m_fYPerFrame;
		if( fabs( m_stPosYIPInfo.m_fCurIncDecY ) >= fabs( m_stPosYIPInfo.m_fAngle ) )
		{
			m_stPosYIPInfo.m_fCurIncDecY	= 0;
			m_stPosYIPInfo.m_fYPerFrame		= 0;
			m_vecPos[1]						= m_stPosYIPInfo.m_fTarY;
		}
		m_vecPos[1] += m_stPosYIPInfo.m_fYPerFrame;
	}

	//----------------------------------------------------------------------
	// Rotation Y 값
	if( m_stRotYIPInfo.m_fYPerFrame != 0 )
	{
		m_stRotYIPInfo.m_fCurIncDecY += m_stRotYIPInfo.m_fYPerFrame;
		if( fabs( m_stRotYIPInfo.m_fCurIncDecY ) >= fabs( m_stRotYIPInfo.m_fAngle ) )
		{
			m_stRotYIPInfo.m_fCurIncDecY	= 0;
			m_stRotYIPInfo.m_fYPerFrame		= 0;
			m_vecRot[1]						= m_stRotYIPInfo.m_fTarY;
		}
		m_vecRot[1] += m_stRotYIPInfo.m_fYPerFrame;

		if( m_stRotYIPInfo.m_fYPerFrame >= 0 )
		{
			if( m_vecRot[1] >= 360 )
				m_vecRot[1] = m_vecRot[1] - 360;
		}
		else
		{
			if( m_vecRot[1] <= 0 )
				m_vecRot[1] = 360 + m_vecRot[1];
		}
	}
}

//------------------------------------------------------------------------------
// < Function >
//   o 오브젝트의 애니메이션 ID 를 설정하고, 애니메이션을 리셋한다.
// < Parameter >
//   o None
// < Return Value >
//   o BOOL - 현재 반환값은 의미를 가지지 못한다.
//------------------------------------------------------------------------------
void
CResObject::ResetAnimation( CAniData * pi_pAni )
{
	if( !m_pBone )
		return;

	_GetCharIF()->AnimationReset( m_pBone );

	if( pi_pAni )
	{
		if( !_GetCharIF()->IsLoadAnimation( pi_pAni ) )
			return;

		_GetCharIF()->MatchAnimationToMesh( pi_pAni, m_pBone );
	}

	// MAYUN
	m_pBone->m_Transition		= m_bIsAniInterpolation;
	if( m_pAniIPInfo && IsEnable( ROSF_ANIMATION_INTERPOLATION ) )
	{
		//m_pBone->m_Transition	= m_bIsAniInterpolation;
		m_pBone->m_TransFrame	= m_pAniIPInfo->dwAniInterpolationFrame;
		m_pBone->m_TransTime	= m_pAniIPInfo->dwAniInterpolationTime;

		if ( m_pBone->m_Transition )
		{
			for ( int i = 0; i < m_pBone->m_MaxObj; i++ )
			{
				memcpy( m_pBone->m_pMesh[i].m_BefMatResult, &m_pAniIPInfo->matInterpolation[i], sizeof( D3DMATRIX ) );
				memcpy( m_pBone->m_pMesh[i].m_BefMatLocal , &m_pAniIPInfo->matLInterpolation[i],sizeof( D3DMATRIX ) );
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// < Function >
//   o 오브젝트의 클리핑 처리
// < Parameter >
//   o None
// < Return Value >
//   o BOOL - TRUE : 클리핑 됐음, FALSE - 클리핑 되지 않았음
//------------------------------------------------------------------------------
BOOL
CResObject::ProcessClipping( void )
{
	static BBOX l_BBox;

	if( !GetBoundBox( l_BBox ) )
		return FALSE;

	// 바운드 박스를 검사해서 클리핑 할지 안 할지를 결정한다.
	if( IsBBoxInFrustum( l_BBox.pMin, l_BBox.pMax, g_pFrustumPlane ) )
	{
		Disable( ROSF_CLIPPED );
		return FALSE;
	}
	else
	{
		Enable( ROSF_CLIPPED );
		return TRUE;
	}
}

//------------------------------------------------------------------------------
// < Function >
//   o 주어진 스크린 좌표와 오브젝트의 바운드 박스가 충돌했는지를 체크한다.
// < Parameter >
//   o pi_nPosX - 스크린의 X 좌표
//   o pi_nPosY - 스크린의 Y 좌표
//   o po_vecCollidedPoint - 충돌한 좌표를 저장
// < Return Value >
//   o BOOL - TRUE : 충돌 했음, FALSE - 충돌하지 않았음
//------------------------------------------------------------------------------
BOOL
CResObject::CollisionCheckWithBoundBox( long pi_nPosX, long pi_nPosY, Vector3f & po_vecCollidedPoint )
{
	static BBOX l_BBox;
	if( !GetBoundBox( l_BBox ) )
		return FALSE;

	// 바운드 박스와의 충돌 체크
	return ( _GetLevel()->IsCollisionRayAABB( pi_nPosX, pi_nPosY,
											  l_BBox.pMin, l_BBox.pMax,
											  &po_vecCollidedPoint ) );
}

//------------------------------------------------------------------------------
// < Function >
//   o 오브젝트의 바운드 박스를 가져온다.
// < Parameter >
//   o po_sBBox - 바운드 박스를 저장할 구조체
//   o pi_bGetOriginalBBox - TRUE  : 원래의 바운드 박스값을 가져온다.
//						   - FALSE : 오브젝트의 현재 위치와 더한 바운드
//									 박스값을 가져온다. ( default )
// < Return Value >
//   o None
//------------------------------------------------------------------------------
/*
BOOL
CResObject::GetBoundBox( BBOX & po_sBBox, BOOL pi_bGetOriginalBBox )
{
	BBOX *	l_pOriginalBBox	= NULL;

	l_pOriginalBBox = _GetBBoxMgr()->GetBoundBox( m_dwBoneID );

	if( !l_pOriginalBBox )
		return FALSE;

	if( pi_bGetOriginalBBox )
	{
		memcpy( &po_sBBox, l_pOriginalBBox, sizeof( BBOX ) );
	}
	else
	{
		po_sBBox.pMin[0] = m_vecPos[0] + ( l_pOriginalBBox->pMin[0] * m_fScale );
		po_sBBox.pMin[1] = m_vecPos[1] + ( l_pOriginalBBox->pMin[1] * m_fScale );
		po_sBBox.pMin[2] = m_vecPos[2] + ( l_pOriginalBBox->pMin[2] * m_fScale );
		po_sBBox.pMax[0] = m_vecPos[0] + ( l_pOriginalBBox->pMax[0] * m_fScale );
		po_sBBox.pMax[1] = m_vecPos[1] + ( l_pOriginalBBox->pMax[1] * m_fScale );
		po_sBBox.pMax[2] = m_vecPos[2] + ( l_pOriginalBBox->pMax[2] * m_fScale );
	}

	return TRUE;
}*/

BOOL
CResObject::GetBoundBox( BBOX & po_sBBox, BOOL pi_bGetOriginalBBox )
{
	if( !m_pBBox )
		return FALSE;

	if( pi_bGetOriginalBBox )
	{
		memcpy( &po_sBBox, m_pBBox, sizeof( BBOX ) );
	}
	else
	{
		/*
		D3DXVECTOR3 l_vecMin( m_pBBox->pMin[0], m_pBBox->pMin[1], m_pBBox->pMin[2] );
		D3DXVECTOR3 l_vecMax( m_pBBox->pMax[0], m_pBBox->pMax[1], m_pBBox->pMax[2] );
		D3DXMATRIX	l_matBBox;

		D3DXMatrixIdentity( &l_matBBox );
		D3DXMatrixRotationY( &l_matBBox, 0.0f ); //D3DXToRadian( m_vecRot[1] ) );
		D3DXVec3TransformCoord( &l_vecMin, &l_vecMin, &l_matBBox );
		D3DXVec3TransformCoord( &l_vecMax, &l_vecMax, &l_matBBox );

		po_sBBox.pMin[0] = m_vecPos[0] + ( l_vecMin.x * m_fScale );
		po_sBBox.pMin[1] = m_vecPos[1] + ( l_vecMin.y * m_fScale );
		po_sBBox.pMin[2] = m_vecPos[2] + ( l_vecMin.z * m_fScale );
		po_sBBox.pMax[0] = m_vecPos[0] + ( l_vecMax.x * m_fScale );
		po_sBBox.pMax[1] = m_vecPos[1] + ( l_vecMax.y * m_fScale );
		po_sBBox.pMax[2] = m_vecPos[2] + ( l_vecMax.z * m_fScale );
		*/

		po_sBBox.pMin[0]	= m_vecPos[0] + ( m_pBBox->pMin[0] * m_fScale );
		po_sBBox.pMin[1]	= m_vecPos[1] + ( m_pBBox->pMin[1] * m_fScale );
		po_sBBox.pMin[2]	= m_vecPos[2] + ( m_pBBox->pMin[2] * m_fScale );
		po_sBBox.pMax[0]	= m_vecPos[0] + ( m_pBBox->pMax[0] * m_fScale );
		po_sBBox.pMax[1]	= m_vecPos[1] + ( m_pBBox->pMax[1] * m_fScale );
		po_sBBox.pMax[2]	= m_vecPos[2] + ( m_pBBox->pMax[2] * m_fScale );
		po_sBBox.dwBoneID	= m_pBBox->dwBoneID;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

DWORD
CResObject::GetBoneID( void )
{
	if( m_pBone )
		return m_pBone->m_ID;

	return ID_INVALID;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CResObject::SetPosY( float pi_fPosY, BOOL pi_bEnablePosYInterpolation )
{
	m_vecPos[1]	= pi_fPosY;
	return;
/*
	if( m_vecPos[1] == pi_fPosY )
		return;

	if( pi_bEnablePosYInterpolation )
	{
		m_stPosYIPInfo.m_fTarY			= pi_fPosY;
		m_stPosYIPInfo.m_fAngle			= pi_fPosY - m_vecPos[1];

		if( ( fabs( m_stPosYIPInfo.m_fAngle ) > 1.0f ) &&
			( _GetFPS() >= 20 ) )
		{
			m_stPosYIPInfo.m_fYPerFrame		= m_stPosYIPInfo.m_fAngle / _GetFPS() * 30;
			m_stPosYIPInfo.m_fCurIncDecY	= 0.0f;

			if( fabs( m_stPosYIPInfo.m_fYPerFrame ) > fabs( m_stPosYIPInfo.m_fAngle ) )
				m_stPosYIPInfo.m_fYPerFrame = m_stPosYIPInfo.m_fAngle;

			return;
		}
	}

	m_vecPos[1]					= pi_fPosY;
	m_stPosYIPInfo.m_fTarY		= m_vecPos[1];

	m_stPosYIPInfo.m_fYPerFrame	= 0;
*/
}

void
CResObject::SetPosition( float pi_fPosX, float pi_fPosY, float pi_fPosZ, BOOL pi_bSetPrevPosition, BOOL pi_bEnablePosYInterpolation )
{
	m_vecPos[0] = pi_fPosX;
	SetPosY( pi_fPosY, pi_bEnablePosYInterpolation );
	m_vecPos[2] = pi_fPosZ;

	if( pi_bSetPrevPosition )
	{
		m_vecPrevPos[0] = m_vecPos[0] + 0.5f;
		m_vecPrevPos[1] = m_vecPos[1];
		m_vecPrevPos[2] = m_vecPos[2];
	}
}

void
CResObject::SetPosition( Vector3f pi_vecPos, BOOL pi_bSetPrevPosition, BOOL pi_bEnablePosYInterpolation )
{
	m_vecPos[0] = pi_vecPos[0];
	SetPosY( pi_vecPos[1], pi_bEnablePosYInterpolation );
	m_vecPos[2] = pi_vecPos[2];

	if( pi_bSetPrevPosition )
	{
		m_vecPrevPos[0] = m_vecPos[0] + 0.5f;
		m_vecPrevPos[1] = m_vecPos[1];
		m_vecPrevPos[2] = m_vecPos[2];
	}
}

/*
void
CResObject::SetPrevPosition( Vector3f pi_vecPos )
{
	memcpy( m_vecPrevPos, pi_vecPos, sizeof( float ) * 3 );
}

void
CResObject::SetPrevPosition( float pi_fPosX, float pi_fPosY, float pi_fPosZ )
{
	m_vecPrevPos[0] = pi_fPosX;
	m_vecPrevPos[1] = pi_fPosY;
	m_vecPrevPos[2] = pi_fPosZ;
}*/

void
CResObject::SetRotY( float pi_fRotY, BOOL pi_bEnableRotYInterpolation )
{
	if( pi_bEnableRotYInterpolation )
	{
		m_vecRot[1]				= (float)( (int)m_vecRot[1] % 360 );

		m_stRotYIPInfo.m_fTarY	= (float)( (int)pi_fRotY % 360 );
		m_stRotYIPInfo.m_fAngle	= fabs( m_vecRot[1] - m_stRotYIPInfo.m_fTarY );

		if( fabs( m_stRotYIPInfo.m_fAngle ) > 180 )
		{
			if( m_vecRot[1] > m_stRotYIPInfo.m_fTarY )
				m_stRotYIPInfo.m_fAngle = ( 360 - m_vecRot[1] + m_stRotYIPInfo.m_fTarY );
			else
				m_stRotYIPInfo.m_fAngle = -( 360 - m_stRotYIPInfo.m_fTarY + m_vecRot[1] );
		}
		else
		{
			if( m_vecRot[1] > m_stRotYIPInfo.m_fTarY )
				m_stRotYIPInfo.m_fAngle = -m_stRotYIPInfo.m_fAngle;
			else
				m_stRotYIPInfo.m_fAngle = m_stRotYIPInfo.m_fAngle;
		}

		m_stRotYIPInfo.m_fYPerFrame		= m_stRotYIPInfo.m_fAngle / _GetFPS() * 12;
		m_stRotYIPInfo.m_fCurIncDecY	= 0.0f;

		if( fabs( m_stRotYIPInfo.m_fYPerFrame ) > fabs( m_stRotYIPInfo.m_fAngle ) )
			m_stRotYIPInfo.m_fYPerFrame = m_stRotYIPInfo.m_fAngle;
	}
	else
	{
		m_vecRot[1]					= (float)( (int)pi_fRotY % 360 );
		m_stRotYIPInfo.m_fTarY		= m_vecRot[1];

		m_stRotYIPInfo.m_fYPerFrame	= 0;
	}
}

void
CResObject::SetRotY( Vector3f pi_vecOrgPos, Vector3f pi_vecTarPos, BOOL pi_bEnableRotYInterpolation )
{
	float l_fYAngle = m_vecRot[1];
	if( GetYAngle( &l_fYAngle, pi_vecOrgPos, pi_vecTarPos ) )
		SetRotY( l_fYAngle, pi_bEnableRotYInterpolation );
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// < Function >
//   o 오브젝트의 상태 프래그를 Enable (TRUE) 상태로 만든다.
// < Parameter >
//   o pi_dwFlag - enable로 설정할 플래그 혹은 플래그의 조합
//				 - enable과 disable이 섞여서는 안 된다.
// < Return Value >
//   o None
//------------------------------------------------------------------------------
void
CResObject::Enable( DWORD pi_dwFlag )
{
	assert( pi_dwFlag != ROSF_INVALID );

	m_dwStatusFlag |= pi_dwFlag;

	if( (m_dwStatusFlag & (ROSF_OUT_OF_VISUAL_FIELD_CLIENT|ROSF_CLIPPED) )
		||	m_dwStatusFlag & (~ROSF_RENDER_MESH) )
		DeleteEffectID();
}

//------------------------------------------------------------------------------
// < Function >
//   o 오브젝트의 상태 프래그를 Disable (FALSE) 상태로 만든다.
// < Parameter >
//   o pi_dwFlag - disable로 설정할 플래그 혹은 플래그의 조합
//				 - disable과 enable이 섞여서는 안 된다.
// < Return Value >
//   o None
//------------------------------------------------------------------------------
void
CResObject::Disable( DWORD pi_dwFlag )
{
	assert( pi_dwFlag != ROSF_INVALID );

	m_dwStatusFlag &= ( ~pi_dwFlag );

	if( (m_dwStatusFlag & (ROSF_OUT_OF_VISUAL_FIELD_CLIENT|ROSF_CLIPPED) )
		||	m_dwStatusFlag & (~ROSF_RENDER_MESH) )
		DeleteEffectID();
}

//------------------------------------------------------------------------------
// < Function >
//   o 오브젝트의 상태 프래그가 어떤 상태인지를 가져온다.
// < Parameter >
//   o pi_dwFlag - enable / disable 상태를 가져올 플래그
//				 - 당연히 조합을 사용해서는 안 된다.
// < Return Value >
//   o BOOL - TRUE / FALSE
//------------------------------------------------------------------------------
BOOL
CResObject::IsEnable( DWORD pi_dwFlag )
{
	assert( pi_dwFlag != ROSF_INVALID );

	return ( m_dwStatusFlag & pi_dwFlag ) ? TRUE : FALSE;
}

BOOL
CResObject::IsEnable( DWORD pi_dwStatusFlag, DWORD pi_dwFlag )
{
	assert( pi_dwFlag != ROSF_INVALID );

	return ( pi_dwStatusFlag & pi_dwFlag ) ? TRUE : FALSE;
}

//------------------------------------------------------------------------------
// < Function >
//   o 오브젝트의 상태 프래그가 어떤 상태인지를 가져온다.
// < Parameter >
//   o pi_dwFlag - enable / disable 상태를 가져올 플래그
//				 - 당연히 조합을 사용해서는 안 된다.
// < Return Value >
//   o BOOL - TRUE / FALSE
//------------------------------------------------------------------------------
BOOL
CResObject::IsDisable( DWORD pi_dwFlag )
{
	assert( pi_dwFlag != ROSF_INVALID );

	return ( m_dwStatusFlag & pi_dwFlag ) ? FALSE : TRUE;
}

BOOL
CResObject::IsDisable( DWORD pi_dwStatusFlag, DWORD pi_dwFlag )
{
	assert( pi_dwFlag != ROSF_INVALID );

	return ( pi_dwStatusFlag & pi_dwFlag ) ? FALSE : TRUE;
}

/*
DWORD
CResObject::GetEffectFrame( void )
{
	return m_Animation.GetEffectFrame();
}*/

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CResObject::SetLightColor( DWORD pi_dwColor, BYTE pi_bFlag )
{
	if( pi_bFlag == LIGHT_RESTORE )
	{
		m_dwLightColor		= m_dwPrevLightColor;
		m_dwPrevLightColor	= ID_INVALID;
		return;
	}

	if( pi_bFlag == LIGHT_BACKUP )
		m_dwPrevLightColor = m_dwLightColor;

	m_dwLightColor = pi_dwColor;
}

void
CResObject::SetLightColorFromMap( void )
{
	ZeroMemory( &m_d3dLight, sizeof( D3DLIGHT8 ) );
	ZeroMemory( &m_d3dMaterial,	sizeof( D3DMATERIAL8 ) );

	// 맵에서 라이트 색상을 가져온다.
	DWORD		l_dwColor;
	//Vector3f	l_vecPos = { m_vecPos[0], m_stPosYIPInfo.m_fTarY, m_vecPos[2] };
	Vector3f	l_vecPos = { m_vecPos[0], m_vecPos[1], m_vecPos[2] };
	if( _GetLevel()->IsLoadedBsp() )
		l_dwColor = _GetLevel()->mBsp->GetLightFromPoint( l_vecPos );

	if( IsEnable( ROSF_AFTER_IMAGE_OF_OBJECT ) )
		l_dwColor = 0xFFA0A0FF;

	// 컬러로부터 라이트 머터리얼을 얻어온다.
	::GetMatLightFromColor( &m_d3dLight, &m_d3dMaterial, l_dwColor );
	_GetCharIF()->SetLight( m_d3dLight );
}

void
CResObject::SetBlink( BOOL pi_bIsBlink )
{
	m_bIsBlink = pi_bIsBlink;

	if( m_bIsBlink )
		m_bIsHide = TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// J.U.I < begin > -------------------------------------------------------------
#include "cland.h"		//잠시 로그파일때문에...
// TRUE  - 계속 움직일 수 있음
// FALSE - 계속 움직일 수 없음
BOOL
CResObject::CheckYPos( void )
{
	float	l_fNextYPos;		
	BOOL	l_bIsGetYPos = FALSE;

	if( m_vecPrevPos[0] == m_vecPos[0] && m_vecPrevPos[2] == m_vecPos[2] )	//같을때 들어올때가 있다.ㅡㅡ JUI
	{
		return TRUE;
	}

	// 낮은 프래임시 캐릭터가 뜨게되거나 박히게되는걸 보호
	if( R3API::R3GetLoopTime() > 0.4f )		//2.5프래임 미만으로 나오면..
	{
		if( _GetLevel()->GetNextYposFarProgress( m_vecPrevPos, m_vecPos, &l_fNextYPos ) )
		{
			if( fabs(l_fNextYPos-m_vecPrevPos[1]) > 50)	//5미터 이상을 차이나면 로그를 남긴다.
			{
				char s_string[1256];
				DWORD id = GetObjectTypeID(),char_id=-1;
				if( id == 1 )
				{
					CCharacter *pl = (CCharacter *)this;
					char_id = pl->GetCharTypeID();
				}
				_GetLevel()->GetNextYposFarProgress( m_vecPrevPos, m_vecPos, &l_fNextYPos );
				sprintf(s_string,"이상이동1 맵=%d,obj=%d old_pos %.3f %.3f %.3f,new pos %.3f %.3f %.3f diff=%.3f"
					,_GetLand()->GetMapIndex(),char_id,m_vecPrevPos[0],m_vecPrevPos[1],m_vecPrevPos[2]
					,m_vecPos[0],l_fNextYPos,m_vecPos[2],l_fNextYPos-m_vecPrevPos[1]);
				EFUtil_EffectLog(s_string);
			}
			SetPosY( l_fNextYPos );
			l_bIsGetYPos	= TRUE;
		}
	}
	else
	{
		if( _GetLevel()->GetNextYpos( m_vecPos, &l_fNextYPos ) )
		{
			SetPosY( l_fNextYPos );
			l_bIsGetYPos	= TRUE;
		}
		else
		if( _GetLevel()->GetNextYposFarProgress( m_vecPrevPos, m_vecPos, &l_fNextYPos ) )
		{
			if( fabs(l_fNextYPos-m_vecPrevPos[1]) > 50)	//5미터 이상을 차이나면 로그를 남긴다.
			{
				char s_string[1256];
				DWORD id = GetObjectTypeID(),char_id=-1;
				if( id == 1 )
				{
					CCharacter *pl = (CCharacter *)this;
					char_id = pl->GetCharTypeID();
				}
				_GetLevel()->GetNextYposFarProgress( m_vecPrevPos, m_vecPos, &l_fNextYPos );
				sprintf(s_string,"이상이동2 맵=%d,obj=%d old_pos %.3f %.3f %.3f,new pos %.3f %.3f %.3f diff=%.3f"
					,_GetLand()->GetMapIndex(),char_id,m_vecPrevPos[0],m_vecPrevPos[1],m_vecPrevPos[2]
					,m_vecPos[0],l_fNextYPos,m_vecPos[2],l_fNextYPos-m_vecPrevPos[1]);
				EFUtil_EffectLog(s_string);
			}
			SetPosY( l_fNextYPos );
			l_bIsGetYPos	= TRUE;
		}
	}

	if( !l_bIsGetYPos )		//JUI.만의하나수정. 
	{
		return FALSE;
	}

	return TRUE;
}

void
CResObject::DeleteEffectID( void )
{
	//이펙트가 붙어있다면지운다.
	for(DWORD i=0; i<2; i++)
	{
		if( m_dwEffectID[i] )
			SetDisapearTimeMagicEffect(m_dwEffectID[i],0);
		m_dwEffectID[i]=0;
	}
}

// J.U.I < end > ---------------------------------------------------------------

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CResObject::SetReceiveFixPositionMsg( BOOL pi_bFlag )
{
/*
#ifdef _DEVELOPER_
	if( m_dwIndex == 19264 && _GetCharMgr()->IsRenderInfo() )
	{
		CPlayer * pPlayer = (CPlayer *)this;
		_SetSystemMsg( "-_- : %s", pPlayer->GetEtcInfo()->m_pInfoString[3] );
	}
#endif*/

	m_bIsReceiveFixPositionMsg = pi_bFlag;

	if( m_bIsReceiveFixPositionMsg )
		m_dwReceiveFixPositionMsgTime = timeGetTime();
}

BOOL
CResObject::IsReceiveFixPositionMsg( void )
{
	return m_bIsReceiveFixPositionMsg;
}

// 일정 시간 동안 fix position 메세지를 받지 못했다면 삭제
BOOL
CResObject::ProcessTimeOverFromReceiveFixPositionMsg( void )
{
	static const DWORD LIMIT_TIME = 10000;

	if( !IsRemovableWhenWasNotReciveNetworkMsg() )
		return FALSE;

	if( ( _GetTime() - m_dwReceiveFixPositionMsgTime ) >= LIMIT_TIME )
	{
		m_bIsReceiveFixPositionMsg = FALSE;
		return TRUE;
	}

	return FALSE;
}

// ProcessTimeOverFromReceiveFixPositionMsg() 등의 함수에 의해 삭제될 경우,
// 예외 상황을 처리한다. ( 움직이지 않거나 죽었을 경우에만 삭제한다. )
// 캐릭터가 먼 장소를 클릭해서 계속 이동할 경우 이동 메세지가 10초이상
// 오지 않을 수도 있을므로, 이런 경우들을 처리한다.
BOOL
CResObject::IsRemovableWhenWasNotReciveNetworkMsg( void )
{
	return TRUE;
}