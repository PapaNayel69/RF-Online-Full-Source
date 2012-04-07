////////////////////////////////////////////////////////////////////////////
//
// CInputMgr Class Header
//
////////////////////////////////////////////////////////////////////////////
#ifndef __GUIINPUTMGR_H__
#define __GUIINPUTMGR_H__

#include <windows.h>
#include "Singleton.h"
#include "../input/imeedit.h"

class CGUIInput;

/*//////////////////////////////////////////////////////////////////////////

[ CInputMgr ]

//////////////////////////////////////////////////////////////////////////*/
class CInputMgr : public CSingleton<CInputMgr>
{
// < Data Member > ---------------------------------------------------------
public:
	CIMEEdit	m_cIME;
	HWND		m_hWnd;

	POINT		m_ptInputPos;

	CGUIInput	*	m_pInput;

// < Member Function > -----------------------------------------------------
public:
	CInputMgr();
	~CInputMgr();

					void	SetInputObject( CGUIInput * pi_pInput );
	inline	CGUIInput	*	GetInputObject( void ) { return m_pInput; }

					void	Init( HWND pi_hWnd );
					void	ReadyInput( POINT & pi_ptInputPos, char * pi_pInitStr );
					void	SetText( char * pi_pStr );
					void	SetInputPosition( POINT & pi_ptInputPos );

					WORD	GetTextLen( void ) { return m_cIME.GetResultLength(); }
					
					void	DrawText( void );
					void	DrawCaret( void );

					void	SetCaretColor( DWORD pi_dwColor );
					void	SetTextColor( DWORD pi_dwColor ); 

					BOOL	IsKoreanMode( void );
					
					void	SetKorean( void );
					void	SetEnglish( void );
					void	SetToggleLang( void );
};

#define	gInputMgr	CInputMgr::GetSingleton()

#endif // __GUIINPUTMGR_H__
