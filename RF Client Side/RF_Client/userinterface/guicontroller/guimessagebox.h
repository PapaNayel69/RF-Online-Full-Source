////////////////////////////////////////////////////////////////////////////
//
// CGUIMessagebox Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUIMESSAGEBOX_H__
#define __GUIMESSAGEBOX_H__

#include "GUIDialogBox.h"
#include "GUITextBoard.h"
#include "GUIScrollbar.h"

#define	MSG_BOX_RGBA		0xD0FFFFFF

#define	MSG_WIN_ID			0x00FFFFFF


// 에러 메시지 뿌리고 창 닫을 때 프로그램 종료하는 함수다
void _GUIErrorMessageBox( char * pi_pMsg );


struct _msgbox_create_info
{
	CGUIWindow	*	pOwnerWindow;
	char		*	pMsg;			 
	DWORD			dwRGBA;
	WORD			wUserDefinedID;
	WORD			wDlgStyle;
	DWORD			wDurationTime;
	BOOL			bScrollable;	
	BOOL			bModal;
	BOOL			bLockOtherWin;
	POINT			ptBoardSize;
	POINT			ptPos;	

	_msgbox_create_info()
	{
		pOwnerWindow		= NULL;
		pMsg				= NULL;
		dwRGBA				= MSG_BOX_RGBA;
		wUserDefinedID		= NOT_DEFINE_ID;
		wDlgStyle			= DLG_STYLE_OK;
		wDurationTime		= 0xFFFFFFFF;
		bScrollable			= FALSE;		
		bModal				= TRUE;	
		bLockOtherWin		= TRUE;
		ptPos.x				= -1;
		ptPos.y				= -1;
		ptBoardSize.x		= 0;
		ptBoardSize.y		= 0;		
	}
};

void
_GUIMessageBox( _msgbox_create_info * pi_pCreateInfo );

void
_GUIMessageBox( CGUIWindow	*	pi_pOwnerWindow, 
			    char		*	pi_pMsg,				 
				DWORD			pi_dwRGBA = MSG_BOX_RGBA,
				WORD			pi_wUserDefinedID = NOT_DEFINE_ID,
				WORD			pi_wDlgStyle = DLG_STYLE_OK,
				DWORD			pi_wDurationTime = 0xffffffff,
				BOOL			pi_bScrollable = FALSE );


void
_GUIMessageWin( char		*	pi_pMsg,
				DWORD			pi_dwRGBA = MSG_BOX_RGBA,
				WORD			pi_wUserDefinedID = NOT_DEFINE_ID,				
				int				pi_nMsgBoardSizeX = 0,
				int				pi_nMsgBoardSizeY = 0 );

void
_CloseMessageWin( WORD	pi_wUserDefinedID = NOT_DEFINE_ID );

void
_CloseAllMessageWin( void );

/*//////////////////////////////////////////////////////////////////////////

[ CGUIMessagebox ]  

//////////////////////////////////////////////////////////////////////////*/

class CGUIMessagebox : public CGUIDialogBox
{
// < Data Member > ---------------------------------------------------------
private:
	CGUIObject			m_cBase;
	CGUIObject			m_cUpperOutLine;
	CGUIObject			m_cLowerOutLine;
	CGUIObject			m_cLeftOutLine;
	CGUIObject			m_cRightOutLine;	

	CGUITextBoard		m_uiMsgBoard;
	CGUIScrollbar		m_uiScrollbar;	

	DWORD				m_dwDuration;
	DWORD				m_dwOpenTime;
	

	BOOL				m_bScrollable;

// < Member Function > -----------------------------------------------------
public:
	CGUIMessagebox();
	CGUIMessagebox( CGUIWindow	*	pi_pOwnerWindow,
					char		*	msg,
					WORD			pi_wDlgStyle,
					WORD			pi_wUserDefinedID,
					BOOL			pi_bScrollable = FALSE );	
	virtual ~CGUIMessagebox();
	
	void	Init( WORD pi_wStyle );
	void	SetSprite( void );	

	void	SetStyle( WORD pi_wStyle );
	void	SetScrollable( BOOL pi_bScrollable ) { m_bScrollable = pi_bScrollable; }
	void	SetMsgBoardSize( int pi_nSizeX, int pi_nSizeY ) { m_uiMsgBoard.SetSize( pi_nSizeX, pi_nSizeY ); }	
	void	SetAlpha( BYTE pi_byAlpha );

	void	SetDuration( DWORD pi_dwDur ) { m_dwDuration = pi_dwDur; }

	void	SetMsg( char * pi_pMsg );	

	void	SetBound( RECT & pi_rtBound );

	void	Open( void );	
	void	Update( void );		
};

/*//////////////////////////////////////////////////////////////////////////

[ CGUIMessageWin ]  

//////////////////////////////////////////////////////////////////////////*/
class CGUIMessageWin : public CGUIWindow
{
// < Data Member > ---------------------------------------------------------
private:
	CGUIObject			m_cBase;
	CGUIObject			m_cUpperOutLine;
	CGUIObject			m_cLowerOutLine;
	CGUIObject			m_cLeftOutLine;
	CGUIObject			m_cRightOutLine;	
	CGUIButton			m_uiOKButton;

	CGUITextBoard		m_uiMsgBoard;
	CGUIScrollbar		m_uiScrollbar;		

	WORD				m_wUserDefinedID;

// < Member Function > -----------------------------------------------------
public:
	CGUIMessageWin();
	virtual ~CGUIMessageWin();

	inline	void	SetUserDefinedID( WORD pi_wID ) { m_wUserDefinedID = pi_wID; }
	inline	WORD	GetUserDefinedID( void ) { return m_wUserDefinedID; }

			void	Init( void );
			void	SetSprite( void );

	inline	void	SetMsgBoardSize( int pi_nSizeX, int pi_nSizeY ) { m_uiMsgBoard.SetSize( pi_nSizeX, pi_nSizeY ); }

			void	SetAlpha( BYTE pi_byAlpha );		

			void	SetMsg( char * pi_pMsg );

			void	SetBound( RECT & pi_rtBound );

			void	MouseEvent( _mouse_event & event );
};

#endif // __GUIMESSAGEBOX_H__
