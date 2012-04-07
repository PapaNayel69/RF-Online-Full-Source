////////////////////////////////////////////////////////////////////////////////
//
// CCamera Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "CPath.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define CAM_INVALID			-1

#define CAM_STOP			0x01000000
#define CAM_MOVE			0x01000001
#define CAM_ROTATE			0x01000002

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

#define CAM_DISTANCE_DEFAULT	( 45.0f )

#define CAM_SPEED_DEFAULT		( 15.0f )
#define CAM_SPEED_NORMAL		( CAM_SPEED_DEFAULT * 5.0f )
#define CAM_SPEED_FLY			( CAM_SPEED_NORMAL	* 10.0f )

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CCamera
{
// < Data Member > -------------------------------------------------------------
private :
	R3Camera	m_cCamera;
	CAniCamera	m_cAniCamera;

	Vector3f	m_vecPos;						// 카메라의 위치
	Vector3f	m_vecRot;						// 카메라의 회전값 ( [0] = Yaw, [1] = Pitch, [2] = Dist )
	float		m_fAddY;						// 종족별로 올려보는 위치가 틀리므로 이변수를 쓴다.(장언일추가)

	DWORD		m_dwState;						// 카메라의 현재 상태
	BOOL		m_bIsChaseMove;					// 카메라의 움직임이 아바타에 고정되어 있는가?	( default = TRUE )
	BOOL		m_bIsChaseView;					// 카메라의 뷰가 아바타에 고정되어 있는가?		( default = TRUE )

	float		m_fSpeed;						// 카메라의 움직임 속도 ( default = 15.0f )

	//--------------------------------------------------------------------------

	CPath		m_cPathStack;					// Camera가 이동할 Path를 저장

	BOOL		m_bIsFirstMoving;
	Vector3f	m_vecNextPath;

	//--------------------------------------------------------------------------

	Vector3f	m_vecTargetRot;					// Camera가 Rotate할 최종값

	//--------------------------------------------------------------------------

	BYTE		m_bySpecialCameraMode;			// by J.U.I
	BYTE		m_byCameraMode;					// by J.U.I

	BOOL		m_bIsCameraInterpolation;		// 카메라 위치와 외전 보정
	BOOL		m_bIsRotatedByMouse;			// 마우스에 의해서 카메라가 회전된적이 있는가?
	BOOL		m_bIsRotatedByKeyboard;			// 키보드에 의해서 카메라가 회전된적이 있는가?

// < Member Function > ---------------------------------------------------------
public :
	CCamera();
	~CCamera();

	void	Init( void );
	BOOL	Create( void );
	BOOL	Destroy( void );

	BOOL	Render( void );
	void	Animation( void );

	//--------------------------------------------------------------------------
	// 카메라의 이동, 회전 관련
	BOOL	Move( Vector3f pi_vecTargetPos );
	void	Rotate( void );

	//--------------------------------------------------------------------------
	// 이 함수들은 단순히 수치만 설정하고 실제 처리는 Render 함수에서 한다.
	inline R3Camera *	GetR3Camera( void )					{ return &m_cCamera; }
	inline CAniCamera *	GetAniCamera( void )				{ return &m_cAniCamera; }

	inline void		SetYaw( float pi_fAngle )				{ m_vecRot[0] = pi_fAngle; }
	inline void		SetPitch( float pi_fAngle )				{ m_vecRot[1] = pi_fAngle; }
	inline void		SetDistance( float pi_fDist )			{ m_vecRot[2] = pi_fDist; }
	inline void		SetState( DWORD pi_dwState )			{ m_dwState = pi_dwState; }
	inline void		SetChaseMove( BOOL pi_bFlag )			{ m_bIsChaseMove = pi_bFlag; }
	inline void		SetChaseView( BOOL pi_bFlag )			{ m_bIsChaseView = pi_bFlag; }
	inline void		SetSpeed( float pi_fSpeed )				{ m_fSpeed = pi_fSpeed; }

	inline float	GetYaw( void )							{ return m_vecRot[0]; }
	inline float	GetPitch( void )						{ return m_vecRot[1]; }
	inline float	GetDistance( void )						{ return m_vecRot[2]; }
	inline int		GetState( void )						{ return m_dwState; }
	inline BOOL		IsChaseMove( void )						{ return m_bIsChaseMove; }
	inline BOOL		IsChaseView( void )						{ return m_bIsChaseView; }
	inline float	GetSpeed( void )						{ return m_fSpeed; }
	inline void		SetAddY( float pi_fAddy )				{ m_fAddY = pi_fAddy; }
	inline float	GetAddY()								{ return m_fAddY; }

		   void		SetRotate( Vector3f pi_vecTargetRot );

	//--------------------------------------------------------------------------
	// 포지션 설정 관련
	inline void		SetPosX( float pi_fPosX )				{ m_vecPos[0] = pi_fPosX; }
	inline void		SetPosY( float pi_fPosY )				{ m_vecPos[1] = pi_fPosY; }
	inline void		SetPosZ( float pi_fPosZ )				{ m_vecPos[2] = pi_fPosZ; }
	inline void		SetPosition( float pi_fPosX, float pi_fPosY, float pi_fPosZ )
					{ m_vecPos[0] = pi_fPosX; m_vecPos[1] = pi_fPosY; m_vecPos[2] = pi_fPosZ; }
	inline void		SetPosition( Vector3f pi_vecPos )
					{ m_vecPos[0] = pi_vecPos[0]; m_vecPos[1] = pi_vecPos[1]; m_vecPos[2] = pi_vecPos[2]; }
	inline float	GetPosX( void )							{ return m_vecPos[0]; }
	inline float	GetPosY( void )							{ return m_vecPos[1]; }
	inline float	GetPosZ( void )							{ return m_vecPos[2]; }
	inline void		GetPosition( Vector3f po_vecPos )
					{ po_vecPos[0] = m_vecPos[0]; po_vecPos[1] = m_vecPos[1]; po_vecPos[2] = m_vecPos[2]; }
	inline float *	GetPosition( void )						{ return m_vecPos;}

	//--------------------------------------------------------------------------
	// 카메라의 패쓰 스택 관련 - CPath 클래스에서 필요한 함수만 래핑
	inline CPath *	GetPathStack( void )					{ return & m_cPathStack; }
	inline void		CreatePath( Vector3f pi_vecTargetPos );
	inline BOOL		GetNextPath( Vector3f po_vecPos );
	inline void		GetCurPath( Vector3f po_vecPos );

	//--------------------------------------------------------------------------

			void	SetCameraMode( BYTE pi_byMode );
	inline	BYTE	GetCameraMode( void )					{ return m_byCameraMode; }
	inline	void	SetSpecialCameraMode( BYTE pi_byMode )	{  m_bySpecialCameraMode=pi_byMode; }
	inline	BYTE	GetSpecialCameraMode( void )			{ return m_bySpecialCameraMode; }

	//--------------------------------------------------------------------------

	inline	void	SetRotatedByMouse( BOOL pi_bFlag )		{ m_bIsRotatedByMouse = pi_bFlag; }
	inline	BOOL	IsRotatedByMouse( void )				{ return m_bIsRotatedByMouse; }
	inline	void	SetRotatedByKeyboard( BOOL pi_bFlag )	{ m_bIsRotatedByKeyboard = pi_bFlag; }
	inline	BOOL	IsRotatedByKeyboard( void )				{ return m_bIsRotatedByKeyboard; }
	inline	void	SetCameraInterpolation( BOOL pi_bFlag )	{ m_bIsCameraInterpolation = pi_bFlag; }
	inline	BOOL	IsCameraInterpolation( void )			{ return m_bIsCameraInterpolation; }
};

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

inline void
CCamera::CreatePath( Vector3f pi_vecTargetPos )
{
	m_cPathStack.Create( m_vecPos, pi_vecTargetPos );
}

inline int
CCamera::GetNextPath( Vector3f po_vecPos )
{
	if( m_cPathStack.GetNextPath( po_vecPos ) )
	{
		m_cPathStack.IncPath();
		return TRUE;
	}

	return FALSE;
}

inline void
CCamera::GetCurPath( Vector3f po_vecPos )
{
	m_cPathStack.GetCurPath( po_vecPos );
}

#endif // __CAMERA_H__