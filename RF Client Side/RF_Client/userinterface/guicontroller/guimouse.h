////////////////////////////////////////////////////////////////////////////
//
// CGUIMouse Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUIMOUSE_H__
#define __GUIMOUSE_H__

#include "GUIObject.h"


/*//////////////////////////////////////////////////////////////////////////

[ CGUIMouse ]

  - Mouse Cursor에 대한 정보

//////////////////////////////////////////////////////////////////////////*/
class CGUIMouse : public CGUIObject
{
public:
	enum CursorState {
		NORMAL,				// 기본 커서
		NORMAL_PRESSED,		// 기본커서에서 누르는 상태
		ATTACK,				// 근거리 공격
		RANGE_ATTACK,		// 원거리 공격
		PICKUP_ITEM,		// 아이템 줍기		
		WAITING_FORCE,		// 포스대기
		WAITING_SKILL,		// 스킬대기
		CAMERA_SET,			// 카메라 설정
		NO_USE,				// 사용, 이동금지
		RESIZE,				// 창 크기 조절
		NPC,				// npc
		SAME_RACE,			// 동족

		TOTAL_MOUSE_CURSOR_NUM
	};

// < Data Member > ---------------------------------------------------------
private:
	CSprite		*	m_pSprite;
	int				m_nActionNo[TOTAL_MOUSE_CURSOR_NUM];	// 커서에 해당하는 action no
	int				m_nTotalFrame[TOTAL_MOUSE_CURSOR_NUM];	// 커서당 frame 갯수
	
	CursorState		m_eCursorState;

	BOOL			m_bSystemMouse;		// 시스템 마우스를 사용할 것인가?

	WORD			m_wFPS;
	float			m_fTempFrameNo;
	int				m_nCurFrameNo;

// < Member Function > -----------------------------------------------------
public:
	CGUIMouse();
	~CGUIMouse();

	void	Init( void );
	void	SetSprite( void );

	void	EnableSystemMouse( BOOL pi_bEnable );	
	
	BOOL	IsSystemMouse( void ) { return m_bSystemMouse; }

	void	SetSpriteInfoEx( SPRITE_INFO * pi_pSpriteInfo );
	void	SetSpriteInfoEx( CursorState pi_eCursorState, SPRITE_INFO * pi_pSpriteInfo );

	void	SetCursorState( CursorState pi_eCursorState );
	
	void	SetPos( POINT & pi_newPos );	// 마우스 커서에 따라 위치가 다르다.
	void	SetPos( LONG pi_nPosX, LONG pi_nPosY );	

	void	Update( void );

	BOOL	Draw( void );

	void	Show( BOOL pi_bShow );
};

#endif // __GUIMOUSE_H__