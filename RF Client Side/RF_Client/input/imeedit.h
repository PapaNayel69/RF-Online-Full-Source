/*------------------------------------------------------------
	File name   : IMEEdit.h
	Description : IME for TestClient Project
	Author      : Jingyu,Kim
	Update Date : 2002.1.11

	Copyright (c) 2001-2002 CCR Entertainment Inc.
	All rights reserved.
-------------------------------------------------------------*/

#ifndef	__IMEEDIT_H__
#define	__IMEEDIT_H__

#include "d3d8types.h"
#include "2dsprite.h"

#define	MAX_SIZE_IMEBUFFER	5
#define	MAX_SIZE_TEXTBUFFER	512

#define ONEBYTE_FONTSIZE 6
#define TWOBYTE_FONTSIZE 12

class CIMEEdit
{
protected:
	char	szCompStr[MAX_SIZE_IMEBUFFER];
	char	szResultStr[MAX_SIZE_TEXTBUFFER];

	DWORD	m_dwTextColor;
	DWORD	m_dwCaretColor;

	BOOL	bKeyDown;
	BOOL	m_bEnableInput;			// input이 가능한 상태인가?
	BOOL	m_bEnableBlinkCaret;	// 캐럿을 깜빡이게 한다.
	BOOL	m_bBlinkCaretState;		// TRUE = show, FALSE = hide
	DWORD	m_dwBlinkGap;
	DWORD	m_dwPrevBlinkTime;

public:
	CIMEEdit();
	virtual ~CIMEEdit();

	int GetCompLength() { return strlen( szCompStr ); } 
	int GetResultLength() { return strlen( szResultStr ); }

	char* GetResultString() { return szResultStr; }
	char SetResultString(char *StrTemp) { strcpy(szResultStr, StrTemp); }
	char ResetResultString() { szResultStr[0] = '\0'; }

	char* GetCompString() { return szCompStr; }

	POINT nCaretPos;

	POINT nEditBoxPos;
	int nEditBoxLength;
	void IMEEditInit(POINT, int, DWORD pi_dwTextColor = 0xFFFFFFFF );

	virtual void OnChar(HWND, WPARAM, LPARAM);
	virtual void OnKeyDown(HWND, WPARAM, LPARAM);
	virtual void OnIMEComposition(HWND, WPARAM, LPARAM);

	void TextOutput(HWND);
	void ShowCaret(HWND);

	BOOL IMEToggleLanguage(HWND);
	BOOL IMEIsKoreanMode( HWND hWnd );
	BOOL IsHangulMiddleByte(LPCSTR, BYTE);
	BOOL IMESetKorean(HWND);
	BOOL IMESetEnglish(HWND);

	void	SetIMEPosition( int x, int y );
	void	SetIMEPosition( POINT &pi_ptIME );
	void	SetIMEPosition( int x, int y, int caretX );
	void	GetIMEPosition( int &x, int &y );
	void	GetIMEPosition( POINT &po_ptIME );

	void	SetCaretPosition( int x, int y );
	void	GetCaretPosition( int &x, int &y );	
	int		GetCaretPositionX( void ) { return nCaretPos.x - nEditBoxPos.x; }

	inline	void  SetTextColor( DWORD pi_dwTextColor )	{ m_dwTextColor = pi_dwTextColor; }
	inline	DWORD GetTextColor( void )					{ return m_dwTextColor; }

	inline	void  SetCaretColor( DWORD pi_dwCaretColor ){ m_dwCaretColor = pi_dwCaretColor; }
	inline	DWORD GetCaretColor( void )					{ return m_dwCaretColor; }

	inline	char *GetText( void )						{ return szResultStr; }
			void  SetText( char * pi_pText );

	inline	void  SetEnableInput( BOOL pi_bFlag )		{ m_bEnableInput = pi_bFlag; m_bBlinkCaretState	= TRUE; }
	inline	BOOL  IsEnableInput( void )					{ return m_bEnableInput; }
	inline	void  SetEnableBlinkCaret( BOOL pi_bFlag )	{ m_bEnableBlinkCaret = pi_bFlag; m_bBlinkCaretState = TRUE; }
	inline	BOOL  IsEnableBlinkCaret( void )			{ return m_bEnableBlinkCaret; }

	inline	void  SetBlinkCaretState( BOOL pi_bBlink )	{ m_bBlinkCaretState = pi_bBlink; }
};

#endif // __IMEEDIT_H__