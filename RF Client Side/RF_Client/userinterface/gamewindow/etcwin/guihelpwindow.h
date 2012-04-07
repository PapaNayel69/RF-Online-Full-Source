////////////////////////////////////////////////////////////////////////////
//
// CGUIHelpWindow Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUIHELPWINDOW_H__
#define __GUIHELPWINDOW_H__

#include "../../GUIController/GUIWindow.h"
#include "../../GUIController/GUIButton.h"
#include "../../GUIController/GUIText.h"
#include "../../GUIController/GUIScrollbar.h"

struct HELP_FONT
{
	BYTE		m_byStyle;
	BYTE		m_bySize;
	DWORD		m_dwColor;
	DWORD		m_dwLinkID;

	HELP_FONT() 
	{
		m_byStyle	= 0;
		m_bySize	= 12;
		m_dwColor	= -1L;
		m_dwLinkID	= -1L;
	}	
};

struct HELP_STRING
{
	HELP_FONT	m_sFont;

	WORD		m_wStrLen;
	char	*	m_pString;	

	HELP_STRING()
	{
		m_wStrLen	= 0;
		m_pString	= NULL;		
	}
};

struct HELP_INFO
{
	DWORD			m_dwWindowID;
	DWORD			m_dwSectionID;

	// string list
	HELP_STRING	*	m_pStringInfoList;
	BYTE			m_byTotalStrNum;	

	HELP_INFO()
	{
		m_dwWindowID			= -1L;

		m_dwSectionID			= 0;

		m_pStringInfoList		= NULL;
		m_byTotalStrNum			= 0;	
	}
};


class CGUIHelpWindow : public CGUIWindow, IScrollObserver
{
private:
	CGUIObject			m_uiTitleBoard;	
	CGUIText			m_uiTitle;
	CGUIButton			m_uiCloseButton;
	
	CGUIScrollbar		m_uiScrollbar;	

	// --------------------------------------------------------------------------------

	HELP_INFO		*	m_pHelpInfoList;

	BYTE				m_byHelpInfoNum;


	HELP_INFO			m_sLinkInfo;

	DWORD				m_dwPrevLinkID;
	POINT				m_ptLinkStringArea;
	
	// --------------------------------------------------------------------------------	
	WORD				m_wFirstViewListIndex;
	WORD				m_wFirstViewStringIndex;
	WORD				m_wFirstViewCharIndex;
	char				m_pOneLineBuffer[128];

	POINT				m_ptStringArea;

	BOOL				m_bIsCreated;
	
public:
	CGUIHelpWindow();
	virtual ~CGUIHelpWindow();

	void	Init( void );
	void	SetSprite( void );

	void	SetSize( LONG pi_nWidth, LONG pi_nHeight );
	void	SetSize( POINT & pi_ptSize );

	// --------------------------------------------------------------------------------
	// 도움말 정보 설정
	void	SetHelpInfo( HELP_INFO * pi_pHelpInfoList, BYTE pi_byNum );
	void	ClearHelpInfo( void );

	// tooltip
	void	SetHelpTooltip( HELP_INFO * pi_pHelpInfo );
	void	ClearHelpTooltip( void );
	
	// --------------------------------------------------------------------------------

	BOOL	Draw( void );

	void	MouseEvent( _mouse_event & event );
	void	ScrollEvent( _scroll_event & event );

private:
	DWORD	DrawHelpInfo( HELP_INFO * pi_pHelpInfoList, BYTE pi_byNum,
						  POINT pi_ptDrawPos, POINT pi_ptDrawArea,
						  WORD pi_wStartListIndex, WORD pi_wStartStringIndex, WORD pi_wStartCharIndex );
	
	void	ResetScrollbar( void );

	void	SetViewedString( void );	

	void	SetFitSize( void );	// 문자열이 딱맞게 윈도우 크기를 조절한다.

	int		GetTotalLine( int pi_nWidth );
};

#endif // __GUIHELPWINDOW_H__
