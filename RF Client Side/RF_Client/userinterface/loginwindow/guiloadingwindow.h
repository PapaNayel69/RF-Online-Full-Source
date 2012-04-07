// GUILoadingWindow.h: interface for the CGUILoadingWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUILOADINGWINDOW_H__1E06A253_5466_4ED6_A8FA_CBCD797F93E0__INCLUDED_)
#define AFX_GUILOADINGWINDOW_H__1E06A253_5466_4ED6_A8FA_CBCD797F93E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../GUIController/GUIWindow.h"

/*//////////////////////////////////////////////////////////////////////////

[ CGUILoginWindow ]

//////////////////////////////////////////////////////////////////////////*/
class CGUILoadingWindow : public CGUIWindow
{
private:
	CGUIObject		m_cLoading;

	// progress bar
	// ...

public:
	CGUILoadingWindow();
	~CGUILoadingWindow();

	void	Init( void );
	void	SetSprite( void );
};

#endif // !defined(AFX_GUILOADINGWINDOW_H__1E06A253_5466_4ED6_A8FA_CBCD797F93E0__INCLUDED_)
