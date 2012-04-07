////////////////////////////////////////////////////////////////////////////
//
// CGUINpcSelectWindow Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUINPCSELECTWINDOW_H__
#define __GUINPCSELECTWINDOW_H__

#include "../../GUIController/GUIWindow.h"
#include "../../GUIController/GUIButton.h"
#include "../../GUIController/GUITextBoard.h"

#define	MAX_SELECT_BUTTON_NUM	5

class CGUINpcSelectWindow : public CGUIWindow
{	
// < Data Member > ---------------------------------------------------------
private:	

	CGUIObject		m_uiTitleBoard;	
	CGUIText		m_uiTitle;		

	CGUITextBoard	m_uiDescBoard;			// npc 선택 도움말
	CGUIScrollbar	m_uiScrollbar;

	BYTE			m_byButtonNum;
	CGUIButtonEx	m_uiNpcSelectButton[MAX_SELECT_BUTTON_NUM];	// npc 선택 버튼	

	CGUIButton		m_uiCloseButton;	

	BOOL			m_bCreated;

// < Member Function > -----------------------------------------------------
public:
	CGUINpcSelectWindow();
	virtual ~CGUINpcSelectWindow();

	//----------------------------------------------------------------------
	void	Init( void );
	void	SetSprite( void );	

	void	Open( void );	

	
	void	SetButtonType( BYTE pi_byTotalButtonNum, BYTE * pi_pButtonTypeList );

	void	SetTitle( char * pi_pTitle );
	void	SetDescription( char * pi_pDescStr );
	
	
	void	MouseEvent( _mouse_event & event );	

	BOOL	Draw( void );
};

#endif // __GUINPCSELECTWINDOW_H__
