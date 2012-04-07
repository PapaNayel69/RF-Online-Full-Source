////////////////////////////////////////////////////////////////////////////
//
// CGUIChatWindow Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef ___GUICHATWINDOW_H__
#define ___GUICHATWINDOW_H__

#include "../GUIController/GUIWindow.h"
#include "../GUIController/GUIScrollbar.h"
#include "../GUIController/GUIInput.h"
#include "../GUIController/GUIButton.h"
#include "../GUIController/GUITextBoard.h"
#include "../NDQueue.h"

#define	MAX_CHAT_INPUT_LEN	128

#define	TOTAL_CHAT_MODE		4


extern BOOL	g_bChangedLangMode;

class CGUIChatInputWindow;

/*//////////////////////////////////////////////////////////////////////////

[ CGUIChatWindow ]

//////////////////////////////////////////////////////////////////////////*/
class CGUIChatWindow : public CGUIWindow
{
// < Data Member > ---------------------------------------------------------
public:
	enum ChatMode {
		ALL,
		PARTY,
		GUILD,
		NATION,
		WHISPER,
		NONE
	};

	enum FontStyle {
		FS_NORMAL,
		FS_AVATAR,
		FS_SYSTEM,
		FS_PARTY_NORMAL,
		FS_PARTY_AVATAR,
		FS_WHISPER_NORMAL,
		FS_WHISPER_AVATAR,
		FS_RACE,
		FS_NOTIFY,
		FS_TIP
	};

	enum LangMode {
		Korean,
		English,
		NoneMode
	};	

	// Event Procedure ( for external process )
	typedef DWORD ( * pfEventProc_Ex )( void * pi_pObject );

private:	
	// ------------------------------------------------------------------
	// title
	CGUIContainer		m_uiTitleBoard;	
	CGUIObject			m_uiResizeButton;
	CGUIObject			m_uiResizeObj;
	CGUIButton			m_uiCloseButton;
	CGUIButton			m_uiHelpButton;

	// output
	CGUIContainer		m_uiOutputBoard;
	CGUITextBoard		m_uiChatBoard[TOTAL_CHAT_MODE];
	CGUIScrollbar		m_uiScrollbar[TOTAL_CHAT_MODE];	
	
	// input	
	CGUIContainer		m_uiInputBoard;	
	CGUIObject			m_uiChatTabBase;
	CGUIButtonEx		m_uiChatTab[TOTAL_CHAT_MODE];	
	CGUIInput			m_uiInputField;
	CGUIButton			m_uiWhisperButton;			// 귓속말 버튼
	CGUIInput			m_uiWhisperIDInput;			// 귓속말 할 player id
	CGUIButton			m_uiLangButton;

	// ------------------------------------------------------------------	

	// chat font
	FONT_INFO			m_pIDFont[10];			// FontStyle
	FONT_INFO			m_pTextFont[10];		// FontStyle

	ChatMode			m_eCurChatMode;
	ChatMode			m_eWhisperPrevChatMode;	// 현재 채팅모드가 귓속말일때 이전 채팅모드

	// language mode	
	LangMode			m_eLangMode;
	

	pfEventProc_Ex		m_pfEventProc_Ex;


	CGUIChatInputWindow * m_pChatInputWin;

	BOOL				m_bCreated;

// < Member Function > -----------------------------------------------------
public:
	CGUIChatWindow();
	~CGUIChatWindow();

	void	SetChatInputWindow( CGUIChatInputWindow * pi_pWin );

	void	Open( void );
	void	Close( void );


	void	Init( void );
	void	SetSprite( void );
	void	SetBaseARGB( DWORD pi_dwARGB );

	void	SetBound( RECT & pi_rtBound );	
	
	//--------------------------------------------------------------------------
	// chatting window clear
	void	Clear( ChatMode pi_eOutputMode = NONE );

	// chatting msg를 뿌려준다
	void	InsertText( char * pi_pID, char * pi_pChat, FontStyle pi_eFontStyle, ChatMode pi_eOutputMode = ALL );

	// 귓속말 대상 설정
	void	SetWhisperCharName( char * pi_pName );
	char *	GetWhisperCharName( void );

	//--------------------------------------------------------------------------
	// chat mode setting
	void	SetChatMode( ChatMode pi_eChatMode );

	// 입력 대기상태 초기화
	void	InitInputModeStr( void );

	
	// langauge mode setting
	void	SetLangMode( LangMode pi_eMode );	

	
	//--------------------------------------------------------------------------
	void	KeyEvent( _key_event & event );
	void	MouseEvent( _mouse_event &event );	
	
	void	SetEventProc_Ex( pfEventProc_Ex pi_pfEventProc ) { m_pfEventProc_Ex = pi_pfEventProc; }


	//--------------------------------------------------------------------------
	inline	CGUIInput		*	GetInputField( void )			{ return &m_uiInputField; }
	inline	CGUIInput		*	GetWhisperInputField( void )	{ return &m_uiWhisperIDInput; }
	inline	CGUITextBoard	*	GetChatBoard( void )			{ return &m_uiChatBoard[0]; }	

	BOOL	Draw( void );
};

/*//////////////////////////////////////////////////////////////////////////

[ CGUIChatInputWindow ]

//////////////////////////////////////////////////////////////////////////*/
class CGUIChatInputWindow : public CGUIWindow
{
// < Data Member > ---------------------------------------------------------
public:
	// Event Procedure ( for external process )
	typedef DWORD ( * pfEventProc_Ex )( void * pi_pObject );

	pfEventProc_Ex		m_pfEventProc_Ex;

private:
	CGUIInput	*	m_pInputField;

	CGUIObject		m_uiOpenChatWin;

	CGUIChatWindow	*	m_pChatWindow;

	// -----------------------------------------------------------------	
	// 채팅 출력 부분
	CNDQueue<CGUITextInfo>	m_qChatMsg;

	WORD			m_wMaxLineNum;
	WORD			m_wMaxCharNumInLine;

	WORD			m_wTotalNewLineNum;
	WORD			m_wLastCharNum;
	WORD			m_wViewLineNum;
	
	WORD			m_wColumnGap;	
	WORD			m_wFirstCharIndex;	
	char			m_pOneLineBuf[128];

// < Member Function > -----------------------------------------------------
public:	
	CGUIChatInputWindow();
	~CGUIChatInputWindow();	

	void	Init( CGUIInput * pi_pInput );	
	void	SetSprite( void );

	void	SetChatWindow( CGUIChatWindow * pi_pWin ) { m_pChatWindow = pi_pWin; }

	// ----------------------------------------------------------------------------
	// 채팅 출력 부분
	void	SetMaxLineNum( WORD pi_wMaxLineNum );
	void	SetMaxCharNumInLine( WORD pi_wMaxCharNum ) { m_wMaxCharNumInLine = pi_wMaxCharNum; }

	void	SetColumnGap( WORD pi_wColumnGap ) { m_wColumnGap = pi_wColumnGap; }		
	
	void	InsertChatMsg( char * pi_pMsg, FONT_INFO * pi_pFontInfo );	// 채팅 메시지 추가

	//--------------------------------------------------------------------------
	void	KeyEvent( _key_event & event );
	void	MouseEvent( _mouse_event &event );

	BOOL	Draw( void );
	
	//--------------------------------------------------------------------------
	void	SetEventProc_Ex( pfEventProc_Ex pi_pfEventProc ) { m_pfEventProc_Ex = pi_pfEventProc; }

	inline	CGUIInput		*	GetInputField( void ) { return m_pInputField; }
};


/*//////////////////////////////////////////////////////////////////////////

[ CGUIBuddyWindow ]

//////////////////////////////////////////////////////////////////////////*/
class CGUIBuddyWindow : public CGUIWindow
{
// < Data Member > ---------------------------------------------------------
public:
	CGUIObject		m_uiBoard;

	DWORD			m_dwBuddyID[8];
	CGUIText		m_uiBuddy[8];
	CGUIButton		m_uiRemove[8];

	CGUIButton		m_uiCloseButton;

	SPRITE_INFO		m_spriteRemove;	

	BOOL			m_bCreated;

// < Member Function > -----------------------------------------------------
public:	
	CGUIBuddyWindow();
	~CGUIBuddyWindow();

	void	Init( void );
	void	SetSprite( void );

	void	AddBuddy( DWORD pi_dwBuddyID, char * pi_pBuddyName );
	void	RemoveBuddy( DWORD pi_dwBuddyID );	


	void	EnableRemove( WORD pi_wIndex, BOOL pi_bEnable );

	void	MouseEvent( _mouse_event &event );
};


#endif // ___GUICHATWINDOW_H__