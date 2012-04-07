////////////////////////////////////////////////////////////////////////////
//
// CCamera Class Implementation
//
////////////////////////////////////////////////////////////////////////////

#include "BaseHeader.h"

#include "Global.h"

#include "CLand.h"
#include "CCamera.h"

#include "./UserInterface/GameWindow/EtcWin/GUIOptionWindow.h"

////////////////////////////////////////////////////////////////////////////

BOOL
EP_SetCameraMode( BYTE pi_byCameraMode )
{
	_GetCamera()->SetCameraMode( pi_byCameraMode );
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////

CCamera::CCamera()
{
	Init();
}

CCamera::~CCamera()
{
	Destroy();
}

////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
// < Function >
//   o 카메라 클래스의 인스턴스를  초기화 시킨다.
// < Parameter >
//   o None
// < Return Value >
//   o None
//--------------------------------------------------------------------------
void
CCamera::Init( void )
{
	ZeroMemory( m_vecPos, sizeof( Vector3f ) );
	ZeroMemory( m_vecRot, sizeof( Vector3f ) );
	m_vecRot[2]				= CAM_DISTANCE_DEFAULT;

	m_dwState				= CAM_STOP;
	m_bIsChaseMove			= TRUE;
	m_bIsChaseView			= TRUE;

	m_fSpeed				= CAM_SPEED_NORMAL;

	m_bIsFirstMoving		= TRUE;

	m_byCameraMode			= 1;
	m_bySpecialCameraMode	= 0;

	m_bIsCameraInterpolation= TRUE;
	m_bIsRotatedByMouse		= FALSE;
	m_bIsRotatedByKeyboard	= FALSE;
}

//--------------------------------------------------------------------------
// < Function >
//   o 카메라 생성.
//	   ( Create() 함수는 클래스에서 사용할 실제 데이터를 생성해 내는 일을 한다. )
// < Parameter >
//   o None
// < Return Value >
//   o BOOL - TRUE / FALSE
//--------------------------------------------------------------------------
BOOL
CCamera::Create( void )
{
	m_cCamera.SetBspPtr( _GetLevel()->mBsp );

	g_pCamera = this;

	return TRUE;
}

BOOL
CCamera::Destroy( void )
{
	return TRUE;
}

//--------------------------------------------------------------------------
// < Function >
//   o 매 루프마다 실행될 카메라에 관련된 세팅 및 명령을 수행한다.
//     다른 클래스의 메쏘드들과 이름을 통일시키기 위해 Render 라고 지음.
// < Parameter >
//   o None
// < Return Value >
//   o BOOL - 현재 반환값은 의미를 가지지 못한다.
//--------------------------------------------------------------------------

BOOL
CCamera::Render( void )
{	
	Vector3f l_vecPos;
	//	const float ADD_Y = 17.0f;	//각 종족별 카메라 위치 바꾸기위함...
	//m_cCamera.SetSmoothPointLoop( m_vecPos[0], m_vecPos[1] + ADD_Y, m_vecPos[2] );

	if( m_bIsCameraInterpolation )
	{
		m_cCamera.SetSmoothPointLoop( m_vecPos[0], m_vecPos[1] + m_fAddY, m_vecPos[2] );
		m_cCamera.SetSmoothThPhDistLoop( m_vecRot[0], m_vecRot[1], m_vecRot[2] );
	}
	else
	{
		m_cCamera.SetPoint( m_vecPos[0], m_vecPos[1] + m_fAddY, m_vecPos[2] );
		m_cCamera.SetThPhDist( m_vecRot[0], m_vecRot[1], m_vecRot[2] );
	}

	m_cCamera.MakeCameraAndViewMatrix();

	m_cCamera.GetPos(l_vecPos);		//맵 프래임무브 전에 해주어야한다. JUI
	_GetLevel()->SetCameraPos( l_vecPos );
	_GetLevel()->SetViewMatrix( R3MoveGetViewMatrix() );

	return TRUE;
}

void
CCamera::Animation( void )
{
	//--------------------------------------------------------------------------
	// 현재 카메라의 움직임
	// 카메라가 움직임 상태라면...
	if( m_dwState == CAM_MOVE )
	{
		// 제일 처음 움직임
		if( m_bIsFirstMoving )
		{
			GetCurPath( m_vecNextPath );

			m_bIsFirstMoving = FALSE;
		}

		// 캐릭터 움직임 처리
		if( !Move( m_vecNextPath ) )
		{
			// 다음 Path 가 없음
			if( !GetNextPath( m_vecNextPath ) )
			{
				SetState( CAM_STOP );

				m_bIsFirstMoving = TRUE;
			}
		}
	}

	//--------------------------------------------------------------------------

	Rotate();
}

// 계속 움직일 수 있으면 TRUE, 아니면 FALSE;
BOOL
CCamera::Move( Vector3f pi_vecTargetPos )
{
	float	l_fSpeed = R3API::R3GetLoopTime() * m_fSpeed;
	float	l_fDist, l_fAngleY;//, l_fNextYPos;

	// 타겟 위치에 도착 했는지를 검사
	l_fDist = GetDist( m_vecPos, pi_vecTargetPos );
	//if( fabs( l_fDist ) <= 0.0f )
	//	return FALSE;
	if( fabs( l_fDist ) <= ( R3API::R3GetLoopTime() * m_fSpeed ) )
		return FALSE;

	// 다음 갈곳의 방향으로 꺽어준다.
	l_fAngleY = R3API::GetYAngle( m_vecPos, pi_vecTargetPos );

	//----------------------------------------------------------------------
	m_vecPos[0] += ( (float)sin( AngleToPi( l_fAngleY ) ) * l_fSpeed );
	m_vecPos[1]	 = m_vecPos[1];
	m_vecPos[2] += ( (float)cos( AngleToPi( l_fAngleY ) ) * l_fSpeed );

	/* 현재는 FLY 모드에서만 동작하므로 주석 처리
	//----------------------------------------------------------------------
	// 앞으로 진행할 수 있는지 없는지를 검사
	// 갈 수 있음
	if( pi_cLand.GetLevel()->GetNextYpos( m_vecPos, &l_fNextYPos ) )
	{
		m_vecPos[1]  = l_fNextYPos;
	}
	// 갈 수 없음
	else
	{
		m_dwState = CAM_STOP;
		return FALSE;
	}

	//----------------------------------------------------------------------
	// 이후 Fly 모드를 위해서..
	R3API::R3FlyMoveSetPos( m_vecPos );
	*/
	
	return TRUE;
}

void
CCamera::Rotate( void )
{
	if( m_dwState == CAM_ROTATE )
	{
		BOOL	l_bIsMove	= 3;
		float	l_fSpeed	= R3API::R3GetLoopTime() * ( m_fSpeed / 2 );
		for( int i = 0; i < 3; ++i )
		{
			if( m_vecRot[i] != m_vecTargetRot[i] )
			{
				if( m_vecRot[i] >= m_vecTargetRot[i] )
				{
					m_vecRot[i] -= l_fSpeed;
					if( m_vecRot[i] < m_vecTargetRot[i] )
						--l_bIsMove;
				}
				else
				{
					m_vecRot[i] += l_fSpeed;
					if( m_vecRot[i] >= m_vecTargetRot[i] )
						--l_bIsMove;
				}
			}
			else
			{
				--l_bIsMove;
			}
		}

		if( !l_bIsMove )
		{
			m_dwState = CAM_STOP;
			m_bIsCameraInterpolation = TRUE;
		}
	}
}

void
CCamera::SetRotate( Vector3f pi_vecTargetRot )
{
	m_dwState = CAM_ROTATE;
	m_bIsRotatedByKeyboard = TRUE;
	m_bIsCameraInterpolation = FALSE;

	m_vecTargetRot[0]	= m_vecRot[0] + pi_vecTargetRot[0];
	m_vecTargetRot[1]	= m_vecRot[1] + pi_vecTargetRot[1];
	m_vecTargetRot[2]	= m_vecRot[2] + pi_vecTargetRot[2];
}

void
CCamera::SetCameraMode( BYTE pi_byMode )
{
	m_byCameraMode = pi_byMode;

	_GetOptionWindow()->SelectCamera( pi_byMode );
}
