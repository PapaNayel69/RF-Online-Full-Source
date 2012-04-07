////////////////////////////////////////////////////////////////////////////
//
// CGUIAttackPartWindow Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef ___GUIMAPMOVEWINDOW_H__
#define ___GUIMAPMOVEWINDOW_H__

#include "../../GUIController/GUIWindow.h"
#include "../../GUIController/GUIButton.h"
#include "../../GUIController/GUIText.h"

#define	MAX_MOVABLE_MAP_NUM		10


/*//////////////////////////////////////////////////////////////////////////

[ CGUIMapMoveWindow ]
 
   - 맵 이동창

//////////////////////////////////////////////////////////////////////////*/
class CGUIMapMoveWindow : public CGUIWindow
{
// < Data Member > ---------------------------------------------------------
private:
	CGUIObject		m_uiTitleBoard;	
	CGUIText		m_uiTitle;
	
	CGUIText		m_uiMsgStr;

	BYTE			m_byMapIndex[MAX_MOVABLE_MAP_NUM];	
	
	CGUIButtonEx	m_uiMapButton[MAX_MOVABLE_MAP_NUM];


	WORD			m_wMapNum;

	BOOL			m_bUseForWorldSelect;				// 월드 선택을 위해서 쓰이는가? by J.S.H	

// < Member Function > -----------------------------------------------------
public:
	CGUIMapMoveWindow();
	virtual ~CGUIMapMoveWindow();

	void	Init( void );
	void	SetSprite( void );
	
	void	AddMapName( char *(pi_pNameList[64]), BYTE * pi_byMapIndexList, WORD pi_wMapNum );
	void	RemoveAllMapName( void );

	void	MouseEvent( _mouse_event & event );	

	BOOL	Draw( void );

	inline	void SetUseForWorldSelect( BOOL	pi_bFlag ) { m_bUseForWorldSelect = pi_bFlag; } // by J.S.H
};

#endif // ___GUIMAPMOVEWINDOW_H__
