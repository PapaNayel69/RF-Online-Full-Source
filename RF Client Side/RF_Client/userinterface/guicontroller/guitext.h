////////////////////////////////////////////////////////////////////////////
//
// CGUIText Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUITEXT_H__
#define __GUITEXT_H__

#include "GUIObject.h"
#include "../../input/imeedit.h"

struct _font_info {
	BYTE	bySize;
	DWORD	dwStyle;
	DWORD	dwColor;

	_font_info()
	{
		dwStyle = 0;
		dwColor = 0xffffffff;
	};
};

typedef _font_info	FONT_INFO;

/*//////////////////////////////////////////////////////////////////////////

[ CGUITextInfo ]  

//////////////////////////////////////////////////////////////////////////*/
class CGUITextInfo  
{
private:
	char	*	m_pText;
	FONT_INFO	m_sFontInfo;

public:
	CGUITextInfo();
	virtual ~CGUITextInfo();

	void		Clear( void );

	void		SetText( char * pi_pText );
	char *		GetText( void ) { return m_pText; }	

	void		SetFont( FONT_INFO * pi_pFont );
	FONT_INFO*	GetFont( void ) { return &m_sFontInfo; }	

	CGUITextInfo& operator=( CGUITextInfo & pi_TextInfo );
};


/*//////////////////////////////////////////////////////////////////////////

[ CGUIText ]  

//////////////////////////////////////////////////////////////////////////*/

class CGUIText : public CGUIObject 
{
// < Data Member > ---------------------------------------------------------
private:
	CGUITextInfo	m_sTextInfo;

// < Member Function > -----------------------------------------------------
public:
	CGUIText();
	~CGUIText();

	void	SetString( char * pi_pStr );
	void	SetFont( FONT_INFO * pi_pFont );
	void	SetTextInfo( CGUITextInfo * pi_pTextInfo );

	char *	GetString( void ) { return m_sTextInfo.GetText(); }
	FONT_INFO * GetFont( void ) { return m_sTextInfo.GetFont(); }
	CGUITextInfo * GetTextInfo( void ) { return &m_sTextInfo; }

	BOOL	Draw( void );
	BOOL	Draw( RECT & pi_rtClipping );
};

#endif // __GUITEXT_H__
