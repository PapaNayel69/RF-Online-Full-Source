////////////////////////////////////////////////////////////////////////////////
//
// CCharacterMgr Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __CHARACTER_MGR_H__
#define __CHARACTER_MGR_H__

#include "../../Character/Avatar/CAvatar.h"
#include "../../Character/Npc/CMonster.h"
#include "../../Character/Npc/CAnimus.h"
#include "../../Character/Npc/CGuardTower.h"
#include "../../Character/Npc/CNpc.h"

#include < list >

#pragma warning ( disable : 4786 )

using namespace std;

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define MAX_DUMMY							( 10 )
#define INDEX_DUMMY							( 0xFFFF0000 )

#define MAX_VISUALFIELD_ADJUSTMENT_LEVEL	( 4 )

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

struct	FPS_INFO
{
	DWORD	m_dwTime;
	float	m_fFPS;
};

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

class CCharacterMgr
{
public :

	typedef list< CCharacter * >			CHAR_LIST;
	typedef list< CCharacter * >::iterator	CHAR_LIST_ITER;

	typedef deque< FPS_INFO >				FPS_INFO_LIST;
	typedef deque< FPS_INFO >::iterator		FPS_INFO_LIST_ITER;

private :

	CAvatar *		m_pAvatar;
	CDummy *		m_pDummy;
	CHAR_LIST		m_listActive[MAX_CHAR_TYPE];
	CHAR_LIST		m_listCache[MAX_CHAR_TYPE];

	//--------------------------------------------------------------------------

	int				m_nCollisionObjectNum;					// 충돌한 오브젝트이 갯수
	CObject *		m_pCollisionObject[100];				// 충돌한 오브젝트의 포인터
	Vector3f		m_vecCollisoinObjectPos[100];			// 충돌한 오브젝트의 위치
	Vector3f		m_vecCollidedPoint[100];				// 스크린 좌표와 오브젝트와 충돌한 좌표값
	long			m_nPosX;								// 체크할 스크린의 X 좌표값
	long			m_nPosY;								// 체크할 스크린의 Y 좌표값

	//--------------------------------------------------------------------------

	float			m_fVisualField;							// 아바타의 시야
	BOOL			m_bIsEnableAutoVisualFieldAdjustment;	// 자동 시야 조정
	BYTE			m_byVisualFieldAdjustmentLevel;			// 시야 조정 레벨

	BOOL			m_bIsDrawBBox;							// 캐릭터의 바운딩 박스를 그릴것인가?
	BOOL			m_bIsRenderShadow;						// 캐릭터의 그림자를 렌더링할 것인가?
	BOOL			m_bIsRenderInfo;						// 캐릭터의 정보를 출력할 것인가? - 테스트 코드
	BOOL			m_bIsRenderPlayerName;					// 플레이어의 이름을 출력할 것인가?

	//--------------------------------------------------------------------------

	DWORD			m_dwMouseHoverCharID[2];				// 마우스가 Hover된 캐릭터
	DWORD			m_dwSelectedCharID[2];					// 아바타가 선택한 캐릭터

	//--------------------------------------------------------------------------

	FPS_INFO_LIST	m_dequeFPSInfo;
	float			m_fAverageFPS;

public :

	CCharacterMgr();
	virtual ~CCharacterMgr();

	//--------------------------------------------------------------------------

			void			Clear( void );

			void			InitCharacter_EveryFrame( void );

			CCharacter	*	AddCharacter( DWORD pi_dwCharTypeID, DWORD pi_dwIndex );
			void			AddCharacter( DWORD pi_dwCharTypeID, CCharacter * pi_pCharacter );
			CPlayer		*	AddPlayer( DWORD pi_dwIndex );
			CMonster	*	AddMonster( DWORD pi_dwIndex );
			CNpc		*	AddNpc( DWORD pi_dwIndex );
			CAnimus		*	AddAnimus( DWORD pi_dwIndex );
			CGuardTower *	AddGuardTower( DWORD pi_dwIndex );
			CPlayer		*	AddUnit( DWORD pi_dwIndex );

			void			DeleteCharacter( DWORD pi_dwCharTypeID, DWORD pi_dwIndex );
			void			ClearCharacter( DWORD pi_dwCharTypeID, BOOL pi_bDeletePartyMember = FALSE );

	//--------------------------------------------------------------------------

			CCharacter	*	GetCharacter( DWORD pi_dwCharTypeID, DWORD pi_dwIndex );
			CPlayer		*	GetPlayer( DWORD pi_dwIndex );
			CMonster	*	GetMonster( DWORD pi_dwIndex );
			CNpc		*	GetNpc( DWORD pi_dwIndex );
			CAnimus		*	GetAnimus( DWORD pi_dwIndex );
			CGuardTower	*	GetGuardTower( DWORD pi_dwIndex );
			CPlayer		*	GetUnit( DWORD pi_dwIndex );

			CCharacter	*	GetCharacterByServerIndex( DWORD pi_dwCharTypeID, DWORD pi_dwServerIndex );
			CCharacter	*	GetCharacterByName( DWORD pi_dwCharTypeID, char * pi_pName );
			CCharacter	*	GetCharacterByDTIndex( DWORD pi_dwCharTypeID, DWORD pi_dwDTIndex );
			CCharacter	*	GetCharacterByOrder( DWORD pi_dwCharTypeID, DWORD pi_dwOrderIndex );
			
			CDummy		*	CreateDummy( void );
			void			DeleteDummy( DWORD pi_dwIndex );
			CDummy		*	GetDummy( DWORD pi_dwIndex );

			void			CreateAvatar( void );
	inline	CAvatar		*	GetAvatar( void )	{ return m_pAvatar; }

private :

			void			DeleteCharacterFromList( CHAR_LIST_ITER & pio_iter );
			void			DeleteCharacterFromList_Data( CHAR_LIST_ITER & pio_iter );
			void			DeleteCharacterFromList_UI( CHAR_LIST_ITER pi_iter );

	//--------------------------------------------------------------------------

public :

			void			SaveToCache( CCharacter * pi_pChar );

private :

			void			SaveToCache( CHAR_LIST_ITER & pio_iter );
			CCharacter *	LoadFromCache( DWORD pi_dwCharTypeID, DWORD pi_dwIndex );
			void			CacheArrangementByTime( void );

	//--------------------------------------------------------------------------

public :

			BOOL			FrameMove( void );
			BOOL			Animation( void );
			BOOL			Render( void );
			BOOL			RenderText( void );
			BOOL			RenderInformation( void );
			LRESULT			MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

private :

			BOOL			FrameMove_Character( BYTE pi_dwCharTypeID, CHAR_LIST_ITER & pio_iter );
			void			FrameMove_ProcessStealthAndDetect( CCharacter * pi_pCharacter );

			BOOL			Animation_Avatar( void );
			void			Animation_Avatar_SetAvatarHead( void );
			void			Animation_Avatar_UnetAvatarHead( void );
			void			Animation_Avatar_MakeShadow( void );
			void			Animation_Avatar_SetUI( void );
			BOOL			Animation_Character( CCharacter * pi_pChar );
			void			Animation_Character_ProcessEffect( CCharacter * pi_pChar );
			void			Animation_Character_ProcessShadow( CCharacter * pi_pChar );

			void			Render_Avatar( void );
			void			Render_Character( CCharacter * pi_pChar );
			void			RenderText_Character( CCharacter * pi_pChar );
			void			RenderInformation_Character( CCharacter * pi_pChar );

	//--------------------------------------------------------------------------

public :

			CObject		*	CollisionCheckWithBoundBox( long pi_nPosX, long pi_nPosY, Vector3f & pi_vecCamPos, Vector3f & po_vecCollidedPoint, BOOL pi_bIsPicking );
			void			CheckOutPositionOfVisualField( void );
			void			CheckOutPositionOfVisualField_Character( CCharacter * pi_pChar, BOOL pi_bCheckRemovableFlag );
			CCharacter	*	FindNearestCharacter( CCharacter * pi_pCenterChar, float pi_fLimitDist, float pi_fLimitYAngleDist = 0.0f );

			void			ProcessDamageFromReturnEffectMgr( CCharacter * pi_pChar );
			void			ClearCharacterEffect( void );

private :

			void			CollisionCheckWithBoundBox_Avatar( BOOL pi_bIsPicking );
			void			CollisionCheckWithBoundBox_Character( CCharacter * pi_pChar );
			CObject		*	CollisionCheckWithBoundBox_GetNearestCharacter( Vector3f & pi_vecCamPos, Vector3f & po_vecCollidedPoint );
			BOOL			IsLocatedInSectorOfAvatar( CCharacter * pi_pChar, float pi_fDistance );
			void			SetOutsideCharOfServerRegion( CCharacter * pi_pChar );
			void			SetOutsideCharOfClientRegion( CCharacter * pi_pChar );

			BOOL			CheckOutPositionOfVisualField_CheckTime( void );

			CCharacter *	FindNearestCharacter_Character( CCharacter * pi_pCenterChar, CCharacter * pi_pChar, float pi_fLimitDist, float pi_fLimitYAngleDist );
			void			FindNearestCharacter_Character_GetLimitYAngleByDistance( float pi_fDist, float & pio_fLimitYAngleDist );
			CCharacter *	FindNearestCharacter_Character_GetCharacterInLimitYAngle( CCharacter * pi_pCenterChar, CCharacter * pi_pChar, float l_fDist, float pi_fLimitYAngleDist );			

	//--------------------------------------------------------------------------

public :

	inline	int				GetTotalPlayerNum( void )				{ return m_listActive[CTI_PLAYER].size(); }
	inline	int				GetTotalMonsterNum( void )				{ return m_listActive[CTI_MONSTER].size(); }
	inline	int				GetTotalNpcNum( void )					{ return m_listActive[CTI_NPC].size(); }
	inline	int				GetTotalAnimusNum( void )				{ return m_listActive[CTI_ANIMUS].size(); }

	inline	void			SetVisualField( float pi_fSize )		{ m_fVisualField = pi_fSize; }
	inline	float			GetVisualField( void )					{ return m_fVisualField; }
	
			void			EnableAutoVisualFieldAdjustment( BOOL pi_bFlag );
	inline	BOOL			IsEnableAutoVisualFieldAdjustment( void ){ return m_bIsEnableAutoVisualFieldAdjustment; }
			void			SetVisualFieldAdjustmentLevel( BYTE pi_byLevel );
	inline	BYTE			GetVisualFieldAdjustmentLevel( void )	{ return m_byVisualFieldAdjustmentLevel; }

	inline	void			SetDrawBBox( BOOL pi_bFlag )			{ m_bIsDrawBBox = pi_bFlag; }
	inline	BOOL			IsDrawBBox( void )						{ return m_bIsDrawBBox; }
	inline	void			Switching_RenderShadow( void )			{ m_bIsRenderShadow = !m_bIsRenderShadow; }
	inline	BOOL			IsRenderShadow( void )					{ return m_bIsRenderShadow; }
	inline	void			SetRenderInfo( BOOL pi_bFlag )			{ m_bIsRenderInfo = pi_bFlag; }
	inline	BOOL			IsRenderInfo( void )					{ return m_bIsRenderInfo; }
	inline	void			SetRenderPlayerName( BOOL pi_bFlag )	{ m_bIsRenderPlayerName = pi_bFlag; }
	inline	BOOL			IsRenderPlayerName( void )				{ return m_bIsRenderPlayerName; }

	//--------------------------------------------------------------------------

			void			SetMouseHoverChar( CCharacter * pi_pChar );
			CCharacter *	GetMouseHoverChar( void );
			void			SetSelectedChar( CCharacter * pi_pChar );
			CCharacter *	GetSelectedChar( void );

private :

			void			VisualFieldAdjustment( void );
};

#endif // __CHARACTER_MGR_H__
