////////////////////////////////////////////////////////////////////////////////
//
// CResObject Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __RES_OBJECT_H__
#define __RES_OBJECT_H__

#include "CObject.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CAnimation
{
	CAniData *	m_pAniData;
	DWORD		m_dwAnimationID;
	float		m_fAnimationSpeed;

	char		m_nMaxEffectFrameNum;						// 이펙트가 발생하는 프레임의 갯수
	char		m_nCurrentFrameNo;							// 현재 이펙트 프레임의 번호
	DWORD		m_dwEffectFrame[MAX_EFFECT_FRAME_NUM];		// 이펙트가 발생하는 프레임
	BOOL		m_bIsInvokedEffect[MAX_EFFECT_FRAME_NUM];	// 이펙트가 발생되었는가?	( 애니메이션 시작 프레임마다 초기화 됨 )

	BOOL		m_bIsAnimating;								// 애니메이션 중인가? - 애니메이션 시작 프레임에 TRUE로 세팅했다가 마지막 프레임에 FALSE로 세팅

public :

	CAnimation();

			void		LoadData( void );
			void		ResetEffectFrame( DWORD pi_dwBoneFrame );

	inline	BOOL		IsEmpty( void )						{ return ( m_pAniData == NULL ); }
	inline	BOOL		IsEqual( CAniData * pi_pAni )		{ return ( m_pAniData == pi_pAni ); }
	inline	BOOL		IsEqualID( DWORD pi_dwAniID )		{ return ( m_dwAnimationID == pi_dwAniID ); }

			void		SetAniData( CAniData * pi_pData );
	inline	CAniData *	GetAniData( void )					{ return m_pAniData; }

	inline	void		SetAnimationID( DWORD pi_dwID )		{ m_dwAnimationID = pi_dwID; }
	inline	DWORD		GetAnimationID( void )				{ return m_dwAnimationID; }

	inline	char		GetCurrentEffectFrameNo( void )		{ return m_nCurrentFrameNo; }
	inline	char		GetMaxEffectFrameNum( void )		{ return m_nMaxEffectFrameNum; }

	inline	DWORD		GetEffectFrame( void )				{ return m_dwEffectFrame[m_nCurrentFrameNo]; }
	inline	void		IncreaseEffectFrameNum( void )		{ ++m_nCurrentFrameNo; }
			BOOL		IsPassedCurrentEffectFrame( DWORD pi_dwBoneFrame );
			BOOL		IsPassedLastEffectFrame( DWORD pi_dwBoneFrame );
			BOOL		IsEffectFrame( DWORD pi_dwBoneFrame );
			char		GetRemainEffectFrameNum( void );

	inline	void		InvokedEffect( BOOL pi_bFlag )		{ m_bIsInvokedEffect[m_nCurrentFrameNo] = pi_bFlag; }
	inline	BOOL		IsInvokedEffect( void )				{ return m_bIsInvokedEffect[m_nCurrentFrameNo]; }

	inline	void		Animating( BOOL pi_bFlag )			{ m_bIsAnimating = pi_bFlag; }
	inline	BOOL		IsAnimating( void )					{ return m_bIsAnimating; }

	inline	void		SetAnimationSpeed( float pi_fSpeed ){ m_fAnimationSpeed = pi_fSpeed; }
	inline	float		GetAnimationSpeed( void )			{ return m_fAnimationSpeed; }
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct ANIMATION_INTERPOLATION_INFO
{
	DWORD		dwAniInterpolationTime;			// 애니메이션이 보간되는 총 시간
	DWORD		dwAniInterpolationFrame;		// 애니메이션 보간 프레임
	D3DXMATRIX	matInterpolation[64];			// 애니메이션 보관 매트릭스
	D3DXMATRIX	matLInterpolation[64];			// 애니메이션 보관 매트릭스 (Local)
};

typedef ANIMATION_INTERPOLATION_INFO ANI_IP_INFO;

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

struct Y_INTERPOLATION_INFO
{
	float	m_fTarY;							// 최종 Y 값 회전/이동값
	float	m_fAngle;							// Original Y와 Target Y의 회전/이동값
	float	m_fCurIncDecY;						// 현재까지 증감한 Y 값

	float	m_fYPerFrame;						// 한 프레임 당 회전/이동할 Y 값

	Y_INTERPOLATION_INFO() { Init(); }
	void	Init( void );
};

typedef Y_INTERPOLATION_INFO Y_IP_INFO;

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Resource Object Status Flags ( ROSF_ )
// - 리소스 오브젝트 상태에 대한 내용을 나타낸다. 이것은 캐릭터등의 상태와는 틀리다.
//   예를들어, CObject 클래스에서 파생된 CCharacter 클래스는 캐릭터에 대한
//   내용을 다룬다고 가정을 하면, OSI는 캐릭터에 대한 내용이 아니라 캐릭터
//   오브젝트에 대한 내용을 다루는 것이다. 캐릭터는 움직이고, 싸우고, 대화
//   하지만 그런것은 개념적인이므로 캐릭터 오브젝트 자체의 상태는 될 수가 없다.
//   캐릭터의 움직이고, 싸우고, 대화하는 등에 대한 내용은 캐릭터 클래스에서
//   따로 정의한다.
// - ID 가 아니고 Flag이다.
// - | 연산자의 조합은 enable 끼리 혹은 diable 끼리만 가능하다.
//	   enable 과 disalbe 이 섞여서 조합돼서는 안 된다.
#define ROSF_INVALID					0xFFFFFFFF

#define ROSF_NONE						0x0
#define ROSF_RESET						ROSF_NONE
#define ROSF_RENDER_BONE				0x1				// 렌더링 가능 (본)
#define ROSF_RENDER_MESH				0x2				// 렌더링 가능 (메시)
#define ROSF_RENDER						0x3				// 렌더링 가능 (본&메시)
#define ROSF_RENDER_ALPHA				0x4				// 알파 렌더링 적용 (메시)
#define ROSF_RENDER_ALPHA_STEALTH		0x8				// 스텔스 상태를 위해 알파 렌더링 적용 (메시)
#define ROSF_RENDER_BBOX				0x10			// 오브젝트의 바운드 박스 출력
#define ROSF_RENDER_NAME				0x20			// 오브젝트의 이름 출력
#define ROSF_RENDER_SHADOW				0x40    		// 그림자 렌더링
#define ROSF_ANIMATION					0x80			// 애니메이션 가능
#define ROSF_ANIMATION_LOOP				0x100			// 애니메이션 반복 가능
#define ROSF_ANIMATION_STOP				0x200			// 애니메이션이 정지된 채로 있음
#define ROSF_ANIMATION_STOP_ENGINE		0x400			// 캐릭터 엔진쪽 애니메이션만 정지
#define ROSF_ANIMATION_INTERPOLATION	0x800			// 애니메이션 보간
#define ROSF_CLIPPED					0x1000			// 클리핑 됐음
#define ROSF_CHECK_Y_POS				0x2000			// Y 포지션을 체크할 것인가?
#define ROSF_RENDER_DEFAULT_MESH		0x4000			// 메시가 없을 경우 디폴트 메시로 렌더링한다.
#define ROSF_MOVABLE					0x8000			// 오브젝트가 움직일 수가 있는가?
#define ROSF_OUT_OF_VISUAL_FIELD_SERVER	0x10000			// 오브젝트가 아바타의 시야에서 벗어났음 ( 서버 )
#define ROSF_OUT_OF_VISUAL_FIELD_CLIENT	0x20000			// 오브젝트가 아바타의 시야에서 벗어났음 ( 클라이언트 )
#define ROSF_HIDE						0x40000			// 오브젝트가 하이딩 상태인가?
#define ROSF_AFTER_IMAGE_OF_OBJECT		0x80000			// 오브젝트가 다른 오브젝트의 잔상임
#define ROSF_DO_NOT_PICK				0x100000		// 오브젝트를 화면상에서 입력장치로 Pick할 수 없슴
#define ROSF_OBSERVER					0x200000		// 옵져버 상태임
#define ROSF_RENDER_EFFECT				0x20000000		// 이펙트 렌더링
#define ROSF_RENDER_INFO				0x40000000		// 오브젝트의 정보를 출력
#define ROSF_DESTROY					0x80000000		// 파괴시킬수 있음

//------------------------------------------------------------------------------
// Object Light
#define	LIGHT_DEFAULT_COLOR				D3DCOLOR_XRGB( 128, 128, 128 )
#define	LIGHT_ENEMY_COLOR1				D3DCOLOR_XRGB( 255, 0,   0 )
#define	LIGHT_ENEMY_COLOR2				D3DCOLOR_XRGB( 11,  210, 131 )
#define	LIGHT_COMPANION_COLOR			D3DCOLOR_XRGB( 205, 82,  16 )
#define	LIGHT_NORMAL_COLOR				D3DCOLOR_XRGB( 200, 200, 200 )

#define	LIGHT_NONE						0
#define	LIGHT_BACKUP					1
#define	LIGHT_RESTORE					2

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*//////////////////////////////////////////////////////////////////////////////

[ CResObject ]

	┏━━━━━━━━━━━┓
	┃                      ┃
	┃       CObject        ┃
	┃                      ┃
	┗━━━━━━━━━━━┛p1
				△
                ┃
                ┃
	┌───────────┐
	│                      │
	│      CResObject      │
	│                      │
	└───────────┘

 - CResObject는 게임상의 모든 캐릭터, 아이템 등의 최상위 클래스이다.

 - CResObject는 물리적인 단위가 아닌 논리적인 단위이다. 즉, 하나의 메쉬, 본 등을
   표현하는 것이 아니라 하나의 아이템, 캐릭터 등을 표현한다.

 - 기본적으로 Mesh 데이터( m_nBoneID ) 를 1개 가지고 있음

 - Mesh 데이터는 캐릭터에서는 본의 역할을, 아이템에서는 아이템 자체를 나타낸다.

 - 기본적인 오퍼레이션으로는 Mesh 데이터의 이동과 회전, 클리핑, 렌더링의 On/Off
   등이 있다.

 - m_nBoneID와 m_nAnimationID는 실제 그래픽 데이터의 ID 이다.

	┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
	┃                                                                  ┃
	┠─────────────────────────────────┨
	┃                                                                  ┃
	┃                                                                  ┃
	┃                                                                  ┃
	┃                                                                  ┃
	┠─────────────────────────────────┨
	┃                                                                  ┃
	┃                                                                  ┃
	┃                                                                  ┃
	┃                                                                  ┃
	┃                                                                  ┃
	┃                                                                  ┃
	┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

  - 온라인 상태일 경우 네트웍 메세지를 처리하는 프로시져가 필요하다.

//////////////////////////////////////////////////////////////////////////////*/

class CResObject : public CObject
{
	friend class CPlayerProc;
	friend class CUnitProc;

// < Data Member > -------------------------------------------------------------
protected :
	Vector3f		m_vecPos;								// 오브젝트의 현재 위치
	Vector3f		m_vecPrevPos;							// 오브젝트의 이전 위치.
	Vector3f		m_vecRot;								// 오브젝트의 현재 회전값. ( 현재는 Y 값만이 유효하다. )

	DWORD			m_dwStatusFlag;							// 오브젝트의 상태에 대한 플래그

	CBone *			m_pBone;								// 오브젝트의 Bone 오브젝트
	BBOX *			m_pBBox;								// 오브젝트의 바운드
	//DWORD			m_dwBoneID;								// 오브젝트의 Bone 오브젝트
	DWORD			m_dwOldBoneFrame;						// 본의 이전 프레임 값
	DWORD			m_dwBoneFrame;							// 본의 현재 프레임 값
	DWORD			m_dwFrameNumForAStopTime;				// 애니메이션이 중지됐을 동안의 프레임 수
	CAnimation		m_Animation;

	float			m_fScale;								// 오브젝트의 스케일 값  ( defalut = 1.0f )

	//--------------------------------------------------------------------------

	BOOL			m_bIsAniInterpolation;					// 애니메이션 보간중인가?
	ANI_IP_INFO	*	m_pAniIPInfo;							// 애니메이션 보간에 대한 정보를 담고 있는 구조체

	Y_IP_INFO		m_stPosYIPInfo;							// Y 좌표 보간
	Y_IP_INFO		m_stRotYIPInfo;							// Y 회전값 보간

	//--------------------------------------------------------------------------

	D3DMATERIAL8	m_d3dMaterial;							// 머터리얼 구조체
	D3DLIGHT8		m_d3dLight;								// 라이트 구조체
	DWORD			m_dwLightColor;							// 오브젝트의 라이트 컬러값
	DWORD			m_dwPrevLightColor;						// 오브젝트의 이전 라이트 컬러값
	float			m_fAlphaDensity;						// 오브젝트의 알파값 ( default = 1.0f )

	//--------------------------------------------------------------------------

	DWORD			m_dwTextColor;							// 이름의 출력 색상

	BOOL			m_bIsBlink;								// 렌더링시 깜빡일 것인가?
	BOOL			m_bIsHide;								// 현재 오브젝트가 사라지는 중인가?
	DWORD			m_dwPrevBlinkTime;
	DWORD			m_dwCurBlinkTime;

	//--------------------------------------------------------------------------

	BOOL			m_bIsReceiveFixPositionMsg;				// 서버로부터 FixPosition 메세지를 받았는가?
	DWORD			m_dwReceiveFixPositionMsgTime;			// 서버로부터 FixPosition 메세지를 받은 시간

	//--------------------------------------------------------------------------
	DWORD			m_dwEffectID[2];						// 리소스에 붙는 이펙트들 by J.U.I

// < Member Function > ---------------------------------------------------------
public :
	CResObject();
	virtual ~CResObject();

	//--------------------------------------------------------------------------

	virtual	void	Init_EveryFrame( void );				// 매 프레임마다 초기에 설정해 주어야할 데이터들

	virtual	BOOL	Render( void );							// 렌더링
	virtual BOOL	Animation( DWORD pi_dwAniFrame = -1 );	// 애니메이션 ( 본 or 메시 )
	virtual	void	FrameMove( void );						// 오브젝트의 움직임
	virtual	BOOL	RenderName( void );						// 오브젝트명 렌더링

			void	CalculateCurrentAnimationFrame( DWORD pi_dwAniFrame );
			void	GetRenderNamePos( Vector4f pi_vecPos, BYTE pi_bySizeOfName );// 이름을 출력할 좌표를 계산해서 가져온다.

public :
			BOOL	ProcessClipping( void );				// 클리핑 처리

	virtual void	ResetAnimation( ChAnimation * pi_pAni );// 애니메이션 세팅

			void	SetBlink( BOOL pi_bIsBlink );			// 오브젝트의 깜빡임 설정

	virtual	BOOL	CheckYPos( void );						// Y 좌표를 체크해서 공중에 떠있으면 땅으로 내려오게한다.

	//--------------------------------------------------------------------------

	// 주어진 스크린 좌표와 오브젝트의 바운드 박스가 충돌했는지를 체크
	virtual	BOOL	CollisionCheckWithBoundBox( long pi_nPosX, long pi_nPosY,
												Vector3f & po_vecCollidedPoint );
	// 캐릭터의 바운드 박스를 얻어낸다.
			BOOL	GetBoundBox( BBOX & po_sBBox, BOOL pi_bGetOriginalBBox = FALSE );

	//--------------------------------------------------------------------------

	virtual	void	Enable( DWORD pi_dwFlag );				// enable 플래그 설정
	virtual	void	Disable( DWORD pi_dwFlag );				// disable 플래그 설정
	virtual	BOOL	IsEnable( DWORD pi_dwFlag );			// 플래그 설정이 enable인가?
			BOOL	IsEnable( DWORD pi_dwStatusFlag, DWORD pi_dwFlag );
	virtual	BOOL	IsDisable( DWORD pi_dwFlag );			// 플래그 설정이 disable인가?
			BOOL	IsDisable( DWORD pi_dwStatusFlag, DWORD pi_dwFlag );
	inline	void	ResetStatusFlag( void ) { m_dwStatusFlag = ROSF_RESET; }

	//--------------------------------------------------------------------------

	virtual	void	SetBone( DWORD pi_dwBoneID )		{ }
	inline	CBone *	GetBone( void )						{ return m_pBone; }
			DWORD	GetBoneID( void );

	inline	void	SetBoneFrame( DWORD pi_dwFrame )	{ m_dwBoneFrame = pi_dwFrame; }
	inline	DWORD	GetBoneFrame( void )				{ return m_dwBoneFrame; }
	inline	DWORD	GetOldBoneFrame( void )				{ return m_dwOldBoneFrame; }	// 본프래임의 이전 프래임값.

	inline	CAnimation	*	GetAnimation( void )		{ return &m_Animation; }
	inline	DWORD	GetAnimationID( void )				{ return m_Animation.GetAnimationID(); }
	inline	char	GetRemainEffectFrameNum( void )		{ return m_Animation.GetRemainEffectFrameNum(); }
	inline	char	GetMaxEffectFrameNum( void )		{ return m_Animation.GetMaxEffectFrameNum(); }

	inline	BOOL	IsEffectFrame( void )				{ return m_Animation.IsEffectFrame( m_dwBoneFrame ); }
	inline	BOOL	IsPassedLastEffectFrame( void )		{ return m_Animation.IsPassedLastEffectFrame( m_dwBoneFrame ); }
	inline	BOOL	IsPassedCurrentEffectFrame( void )	{ return m_Animation.IsPassedCurrentEffectFrame( m_dwBoneFrame ); }

	inline	void	SetAnimationSpeed( float pi_fSpeed ){ m_Animation.SetAnimationSpeed( pi_fSpeed ); }
	inline	float	GetAnimationSpeed( void )			{ return m_Animation.GetAnimationSpeed(); }

	//--------------------------------------------------------------------------

	inline	void	SetPosX( float pi_fPosX )			{ m_vecPos[0] = pi_fPosX; }
	virtual	void	SetPosY( float pi_fPosY, BOOL pi_bEnablePosYInterpolation = FALSE );
	inline	void	SetPosZ( float pi_fPosZ )			{ m_vecPos[2] = pi_fPosZ; }

	virtual	void	SetPosition( float pi_fPosX, float pi_fPosY, float pi_fPosZ, BOOL pi_bSetPrevPosition = TRUE, BOOL pi_bEnablePosYInterpolation = FALSE );
	virtual	void	SetPosition( Vector3f pi_vecPos, BOOL pi_bSetPrevPosition = TRUE, BOOL pi_bEnablePosYInterpolation = FALSE );

	inline	float	GetPosX( void )						{ return m_vecPos[0]; }
	inline	float	GetPosY( void )						{ return m_vecPos[1]; }
	inline	float	GetPosZ( void )						{ return m_vecPos[2]; }

	inline	void	GetPosition( Vector3f po_vecPos )
					{ memcpy( po_vecPos, m_vecPos, sizeof( Vector3f ) ); }
	inline	float *	GetPosition( void )					{ return m_vecPos; }
	inline	void	GetPrevPosition( Vector3f po_vecPos )
					{ memcpy( po_vecPos, m_vecPrevPos, sizeof( Vector3f ) ); }

	//		void	SetPrevPosition( Vector3f pi_vecPos );
	//		void	SetPrevPosition( float pi_fPosX, float pi_fPosY, float pi_fPosZ );

	inline	void	SetRotX( float pi_fRotX )			{ m_vecRot[0] = (float)( (int)pi_fRotX % 360 ); }
	inline	float	GetRotX( void )						{ return m_vecRot[0]; }
	virtual	void	SetRotY( float pi_fRotY, BOOL pi_bEnableRotYInterpolation = FALSE );
	virtual	void	SetRotY( Vector3f pi_vecOrgPos, Vector3f pi_vecTarPos, BOOL pi_bEnableRotYInterpolation = FALSE );
	inline	float	GetRotY( void )						{ return m_vecRot[1]; }
	inline	void	SetRotZ( float pi_fRotZ )			{ m_vecRot[2] = (float)( (int)pi_fRotZ % 360 ); }
	inline	float	GetRotZ( void )						{ return m_vecRot[2]; }

	inline	void	GetRotation( Vector3f po_vecPos )
					{ memcpy( po_vecPos, m_vecRot, sizeof( Vector3f ) ); }
	inline	float *	GetRotation( void )					{ return m_vecRot; }

	inline	void	SetScale( float pi_fScale )			{ m_fScale = pi_fScale; }
	inline	float	GetScale( void )					{ return m_fScale; }

	//--------------------------------------------------------------------------

	inline	void	SetMaterial( D3DMATERIAL8 * pi_d3dMaterial )	{ memcpy( &m_d3dMaterial, pi_d3dMaterial, sizeof( D3DMATERIAL8 ) ); }
	virtual	void	SetLightColor( DWORD pi_dwColor, BYTE pi_bFlag = LIGHT_NONE );
	inline	DWORD	GetLightColor( void )				{ return m_dwLightColor; }
	virtual	void	SetLightColorFromMap( void );

	virtual	void	SetAlphaDensity( float pi_fAlpha )	{ m_fAlphaDensity = pi_fAlpha; }
	inline	float	GetAlphaDensity( void )				{ return m_fAlphaDensity; }

	virtual	void	SetTextColor( DWORD pi_dwColor )	{ m_dwTextColor = pi_dwColor; }
	inline	DWORD	GetTextColor( void )				{ return m_dwTextColor; }

	inline  DWORD	GetEffectID(DWORD pi_dwIndex)				{ return m_dwEffectID[pi_dwIndex]; } //주의 배열 2개밖에없다.by J.U.I
	inline  void 	SetEffectID(DWORD pi_dwIndex,DWORD pi_dwID) { m_dwEffectID[pi_dwIndex]=pi_dwID; } //주의 배열 2개밖에없다.by J.U.I
			void	DeleteEffectID( void );	


	//--------------------------------------------------------------------------

	inline	BOOL	IsAniInterpolation( void )			{ return m_bIsAniInterpolation; }

	//--------------------------------------------------------------------------

			void	SetReceiveFixPositionMsg( BOOL pi_bFlag );
	virtual	BOOL	IsReceiveFixPositionMsg( void );
	inline	DWORD	GetReceiveFixPositionMsgTime( void ){ return m_dwReceiveFixPositionMsgTime; }

	virtual	BOOL	ProcessTimeOverFromReceiveFixPositionMsg( void );
	virtual	BOOL	IsRemovableWhenWasNotReciveNetworkMsg( void );
};

#endif // __RES_OBJECT_H__