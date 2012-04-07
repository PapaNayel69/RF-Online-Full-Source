////////////////////////////////////////////////////////////////////////////////
//
// CHelpBoard Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __HELP_BOARD_H__
#define __HELP_BOARD_H__

#include "../../../DefineMessage.h"
#include "../../../UserInterface/GameWindow/EtcWin/GUIHelpWindow.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*
struct HSD_FONT
{
	BYTE			m_byStyle;
	BYTE			m_bySize;
	DWORD			m_dwColor;
	DWORD			m_dwLinkID;

	HSD_FONT();
};

struct HSD_STRING
{
	HSD_FONT		m_stFont;
	WORD			m_wLength;
	char *			m_pString;

	HSD_STRING();
	~HSD_STRING();
};

struct HSD_BASE_DATA
{
	DWORD			m_dwID;

	HSD_STRING *	m_listString;
	BYTE			m_byTotalStringNum;

	HSD_BASE_DATA();
	~HSD_BASE_DATA();

	DWORD	GetStringLength( void );
};

struct HSD_HINT : public HSD_BASE_DATA
{
	BYTE			m_byLevelMin;
	BYTE			m_byLevelMax;
	BYTE			m_byMapNo;
	DWORD			m_dwActionID;
	DWORD			m_dwTime;
	BOOL			m_bIsDispaly;

	HSD_HINT();
};

struct HSD_HELP : public HSD_BASE_DATA
{
	DWORD			m_dwWindowID;
	DWORD			m_dwSectionID;

	HSD_HELP();
};*/

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CGUIScreen;
typedef vector< HSD_HELP * >			CHelpList;
typedef vector< HSD_HELP * >::iterator	CHelpListIter;

#define MAX_HELP_TYPE_NUM	( MAX_UI + 1 )		// 제일 마지막에는 툴팁이 저장됨
#define UI_HELP_TOOLTIP		( MAX_UI )

class CHelpBoard
{
	CHelpList		m_listHelp[MAX_HELP_TYPE_NUM];
	CGUIHelpWindow	m_uiHelpWindow;

	DWORD			m_dwPrevOpendWindowID;

public :

	CHelpBoard();
	~CHelpBoard();

	void		InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY, CGUIScreen * pi_pUiScreen );

	void		FrameMove( void );
	void		Render( void );

	BOOL		LoadData( void );

	LRESULT		MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

private :

	BOOL		LoadData_Help( void );
	BOOL		LoadData_Hint( void );
	BOOL		LoadData_HSDString( HSD_BASE_DATA * pio_pHSDData, CDataString * pi_pSourceData );

public :

	void		OpenHelpWindow( DWORD pi_dwWindowID );
	void		CloseHelpWindow( void );

	void		OpenHintWindow( void );
	void		CloseHintWindow( void );

private :

	void		OpenHelpTooltip( DWORD pi_dwLinkID );
};

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

extern CHelpBoard * g_pHelpBoard;
#define _GetHelpBoard() g_pHelpBoard

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __HELP_BOARD_H__