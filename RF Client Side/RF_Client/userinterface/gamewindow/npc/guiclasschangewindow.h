////////////////////////////////////////////////////////////////////////////
//
// CGUIClassChange UI Header
//
////////////////////////////////////////////////////////////////////////////
#ifndef ___GUICLASSCHANGEWINDOW_H__
#define ___GUICLASSCHANGEWINDOW_H__

#include "../../GUIController/GUIWindow.h"
#include "../../GUIController/GUIButton.h"
#include "../../GUIController/GUITextBoard.h"
#include "../../GUIController/GUIItem.h"

#define		MAX_CLASS_BUTTON_NUM		10
#define		MAX_CLASS_SKILL_ICON_NUM	3

/*//////////////////////////////////////////////////////////////////////////

[ CGUIClassChange ]

//////////////////////////////////////////////////////////////////////////*/
class CGUIClassChangeWindow : public CGUIWindow
{
// < Data Member > ---------------------------------------------------------
private:

	CGUIText			m_uiTitle;
	CGUIObjectAni		m_uiAniTitleBar;	
	CGUIButton			m_uiCloseButton;
	CGUIButton			m_uiHelpButton;

	CGUIText			m_uiClassNameStr;
	CGUITextBoard		m_uiClassDescription;
	CGUIScrollbar		m_uiScrollbar;

	CGUIItem			m_uiClassSkillIcon[MAX_CLASS_SKILL_ICON_NUM];

	CGUIText			m_uiClassListStr;
	CGUIObjectEx		m_uiClassSelectButton[MAX_CLASS_BUTTON_NUM];
	BYTE				m_byTotalClassButtonNum;

	CGUIObject			m_uiSelectedButtonMark;

	CGUIButtonEx		m_uiOKButton;
	CGUIButtonEx		m_uiCancelButton;

	BOOL				m_bIsCreated;

// < Member Function > -----------------------------------------------------
public:
	CGUIClassChangeWindow();
	virtual ~CGUIClassChangeWindow();

	void	Init( void );
	void	SetSprite( void );

	void	SetTotalClassNum( BYTE pi_byClassNum, BYTE * pi_pIconIDList );
	
	void	SelectClass( BYTE pi_byIndex );

	void	SetSkillIcon( BYTE pi_byIconNum, DWORD * pi_pIconIDList );

	void	SetClassName( char * pi_pStr );
	void	SetDescription( char * pi_pStr );
	

	void	MouseEvent( _mouse_event &event );
};

#endif // ___GUICLASSCHANGEWINDOW_H__
