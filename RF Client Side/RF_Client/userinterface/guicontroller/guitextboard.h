////////////////////////////////////////////////////////////////////////////
//
// CGUITextBoard Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUITEXTBOARD_H__
#define __GUITEXTBOARD_H__

#include "GUIContainer.h"
#include "GUIScrollbar.h"

#include "GUIText.h"
#include <deque>

typedef deque<CGUITextInfo *>	GUITextList;

/*//////////////////////////////////////////////////////////////////////////

[ CGUITextBoard ]

  - text만 보여준다.
  - 

//////////////////////////////////////////////////////////////////////////*/
class CGUITextBoard : public CGUIContainer, IScrollObserver
{
// < Data Member > ---------------------------------------------------------
private:
	GUITextList			m_listText;
	
	WORD				m_wLimitLineNum;			// 제한된 줄수. 제한줄수를 넘으면 오래된것부터제거한다.
	WORD				m_wColumnGap;				// 세로로 text간 간격
	POINT				m_ptMargin;					// 가로 여백	

	DWORD				m_dwViewedFirstTextIndex;	// 처음 보여지는 Text List index
	WORD				m_wViewedFirstCharIndex;	// 처음 보여지는 char index

	DWORD				m_dwTotalLineNum;			// board에 출력할수있는 총 라인수
	WORD				m_wLastCharNum;				// 마지막 줄에 문자 갯수

	char	*			m_pOneLineBuffer;			// 한줄을 출력하기 위한 버퍼

	CGUIScrollbar	*	m_pScrollbar;
	BOOL				m_bAutoScroll;				// text가 삽입될 때마다
													// 맨 마지막 줄이 보이도록 자동 스크롤 해줄껀가	

// < Member Function > -----------------------------------------------------
public:
	CGUITextBoard();
	~CGUITextBoard();

	void	SetLimitLineNum( DWORD pi_dwLimitLineNum ) { m_wLimitLineNum = pi_dwLimitLineNum; }
	void	SetMargin( int pi_nX, int pi_nY ) { m_ptMargin.x = pi_nX; m_ptMargin.y = pi_nY; }
	void	SetColumnGap( int pi_nGap );

	void	SetAutoScroll( BOOL pi_bAutoScroll ) { m_bAutoScroll = pi_bAutoScroll; }

	int		GetMarginX( void ) { return m_ptMargin.x; }
	int		GetMarginY( void ) { return m_ptMargin.y; }

	int		GetColumnGap( void ) { return m_wColumnGap; }

	void	AddScrollbar( CGUIScrollbar	* pi_pScrollbar );	
	void	SetBound( RECT & pi_rtBound );	
	void	SetSize( LONG pi_nSizeX, LONG pi_nSizeY );

	//----------------------------------------------------------------------
	void	Clear( void );
	void	InsertText( char * pi_pText );
	void	InsertText( char * pi_pText, FONT_INFO * pi_pFontInfo );
	void	InsertText( CGUITextInfo * pi_pTextInfo, BOOL pi_bResize = FALSE );

	int		GetTotalLineNum( void ) { return m_listText.size(); }

	//----------------------------------------------------------------------
	BOOL	Draw( void );
	BOOL	Draw( RECT & pi_rtClipping );

	void	ScrollEvent( _scroll_event & event );

private:	
	void	SetViewedText( void );	// m_dwViewedFirstText(+char)Index Setting

	void	ResizeByText( char * pi_pStr );
};

#endif // __GUITEXTBOARD_H__
