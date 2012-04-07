////////////////////////////////////////////////////////////////////////////
//
// CGUIInput Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUIINPUT_H__
#define __GUIINPUT_H__

#include "GUIObject.h"
#include "../InputMgr.h"

#define	MAX_INPUT_STRING	256

/*//////////////////////////////////////////////////////////////////////////

[ CGUIInput ]

  - user의 문자 입력을 받아서 보여준다.

//////////////////////////////////////////////////////////////////////////*/
class CGUIInput : public CGUIObject
{
public:
	enum Alignment {
		Left_Alignment,
		Right_Alignment
	};

	enum InputType {
		Free,
		Only_Number,
		Only_Char
	};
			

// < Data Member > ---------------------------------------------------------
private:
	Alignment	m_eAlignment;			// 문자 정렬
	BOOL		m_bIsSecretText;		// password처럼 다른 문자로 보이게 할때
	InputType	m_eInputType;			// 입력타입(숫자, 문자) 제한
	DWORD		m_dwTextColor;

	WORD		m_wShowBeginCharIndex;	// string이 보여 줄수 있는 길이보다 길때
										// 맨처음 보이는 문자 index
	WORD		m_wShowEndCharIndex;	// 마지막에 보이는 문자 index
	
	char		m_pInputString[MAX_INPUT_STRING+1];
	POINT		m_ptMargin;				// 가로 세로 여백
	int			m_nStrPosX;				// 보여지는 문자가 시작되는 위치
	WORD		m_wInputLenLimit;		// string 길이 제한

	int			m_nCaretPosX;			// 캐럿의 x 위치 
										// 입력 객체마다 각각의 캐럿 위치를 가지고 있어야 한다.

	DWORD		m_dwCaretColor;

// < Member Function > -----------------------------------------------------
public:
	CGUIInput();
	~CGUIInput();

			void	SetAlignment( Alignment pi_eAlignment );
			void	SetShowText( BOOL pi_bShowText );	
	inline	void	SetInputType( InputType pi_eInputType ) { m_eInputType = pi_eInputType; }	

			void	SetInputLimit( WORD pi_wLimit );	

			void	SetMargin( LONG pi_nPosX, LONG pi_nPosY );

			// Active()는 기본적으로 mouse입력을 받을 수 있게 하는 것이고
			// CGUIInput에 키입력을 받을려면 Focus되어있어야 한다.
			BOOL	IsFocused( void );
			void	SetFocus( void );
			
			void	SetPos( POINT & pi_newPos );
			void	SetPos( LONG pi_nPosX, LONG pi_nPosY );

			void	Clear( void );
	inline	char *	GetText( void )	{ return m_pInputString; }	

			void	SetText( char * pi_pText );

			void	SetCaretPos( int pi_nLeftStrIndexOfCaret );
	inline	int		GetCaretPos( void ) { return m_nCaretPosX / ONEBYTE_FONTSIZE; }
	
	
	inline	void	SetTextColor( DWORD pi_dwColor ) { m_dwTextColor = pi_dwColor; }

	inline	void	SetCaretColor( DWORD pi_dwColor ) { m_dwCaretColor = pi_dwColor; }
	inline	DWORD	GetCaretColor( void ) { return m_dwCaretColor; }
			

			BOOL	Draw( void );
			BOOL	Draw( RECT & pi_rtClipping );
			LRESULT	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

private:
			void	ResetInputPos( void );
			void	SetShowCharIndex( void );

			void	DrawText( void );
			void	DrawSecretText( void );			
};

#endif // __GUIINPUT_H__
