////////////////////////////////////////////////////////////////////////////
//
// CGUIScrollbar Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUISCROLLBAR_H__
#define __GUISCROLLBAR_H__

#include "GUIContainer.h"
#include "GUIButton.h"

/*//////////////////////////////////////////////////////////////////////////

[ CGUIScrollbar ] 

//////////////////////////////////////////////////////////////////////////*/
class CGUIScrollbar : public CGUIContainer,
					  public IMouseObserver,
					  public IMouseMotionObserver
{
public:
	enum Style
	{
		Vertical,
		Horizontal
	};

// < Data Member > ---------------------------------------------------------
private:
	Style			m_eStyle;

	CGUIButton		m_uiTopArrow;
	CGUIButton		m_uiBottomArrow;
	CGUIButton		m_uiBar;	

	LONG			m_nTotalAmount;
	LONG			m_nCurrentAmount;

	WORD			m_wAdjustmentUnit;	
	WORD			m_wArrowAdjustmentUnit;

	DWORD			m_dwScrolledTime;
	DWORD			m_dwStrokedTime;

	WORD			m_wScrollSpeed;	

	int				m_nBarPressedPos;
	int				m_nTotalScrollLength;
	

	IScrollObserver	*	m_pScrollObserver;

// < Member Function > -----------------------------------------------------
public:
	CGUIScrollbar();
	~CGUIScrollbar();

			void	SetStyle( Style pi_eStyle );
	
			void	SetBarSize( POINT & pi_ptSize ) { m_uiBar.SetSize( pi_ptSize ); }
			void	SetTopArrowSize( POINT & pi_ptSize ) { m_uiTopArrow.SetSize( pi_ptSize ); }
			void	SetBottomArrowSize( POINT & pi_ptSize ) { m_uiBottomArrow.SetSize( pi_ptSize ); }

			int		GetMinSize( void );

	// ------------------------------------------------------------------------------			
			void	SetSize( POINT & pi_ptSize );
			void	SetSize( LONG pi_nWidth, LONG pi_nHeight );
			void	SetBound( RECT & pi_rtBound );
	
			void	AddScrollHandler( IScrollObserver * pi_pObj );
	
			void	SetTopArrowSpriteInfo( SPRITE_INFO * pi_spriteInfos );
			void	SetBottomArrowSpriteInfo( SPRITE_INFO * pi_spriteInfos );
			void	SetBarSpriteInfo( SPRITE_INFO * pi_pSpriteInfo );

			void	SetSprite( CSprite * pi_pSprite );

	inline	void	SetScrollSpeed( WORD pi_wSpeed ) { if( pi_wSpeed > 0 ) m_wScrollSpeed = pi_wSpeed; }

	// ------------------------------------------------------------------------------
	inline	void	SetAdjustmentUnit( WORD pi_wAdjustUnit ) { m_wAdjustmentUnit = pi_wAdjustUnit; }
	inline	WORD	GetAdjustmentUnit( void ) { return m_wAdjustmentUnit; }
	inline	void	SetArrowAdjustmentUnit( WORD pi_wAdjustUnit ) { m_wArrowAdjustmentUnit = pi_wAdjustUnit; }

			void	SetTotalAmount( LONG pi_nTotalAmount );
			void	SetCurrentAmount( LONG	pi_nCurrentAmount );
			void	SetAmount( LONG pi_nTotalAmount, LONG pi_nCurrentAmount );
			
	inline	LONG	GetTotalAmount( void ) { return m_nTotalAmount; }
	inline	LONG	GetCurrentAmount( void ) { return m_nCurrentAmount; }

	// ------------------------------------------------------------------------------
			void	UpdateBar( void );			

			void	MouseEvent( _mouse_event & event );
			void	MouseMotionEvent( _mouse_motion_event & event );			


			void	SendScrollEvent( WORD pi_wID );		
private:
			LONG	CalcCurrentAmount( int pi_nCurrentScrollLength );
};

#endif // __GUISCROLLBAR_H__
