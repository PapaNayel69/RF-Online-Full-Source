////////////////////////////////////////////////////////////////////////////////
//
// CGameEventBoard Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../../../BaseHeader.h"
#include "../../../Define.h"
#include "../../../Global.h"
#include "../../../DataFile.h"
#include <vector>

#include "../../../UserInterface/GUIController/GUIScreen.h"

#include "CHelpBoard.h"

CHelpBoard * g_pHelpBoard = NULL;

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*
HSD_FONT::HSD_FONT()
{
	m_byStyle	= 0;
	m_bySize	= 7;
	m_dwColor	= 0xFFA0A0A0;
	m_dwLinkID	= 0xFFFFFFFF;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

HSD_STRING::HSD_STRING()
{
	m_wLength	= 0;
	m_pString	= NULL;
}

HSD_STRING::~HSD_STRING()
{
	if( m_pString )
	{
		delete [] m_pString;
		m_pString = NULL;
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

HSD_BASE_DATA::HSD_BASE_DATA()
{
	m_dwID				= ID_INVALID;

	m_listString		= NULL;
	m_byTotalStringNum	= 0;
}

HSD_BASE_DATA::~HSD_BASE_DATA()
{
	if( m_listString )
	{
		delete [] m_listString;
		m_listString = NULL;
	}
}

DWORD
HSD_BASE_DATA::GetStringLength( void )
{
	DWORD l_dwStringLength = 0;
	for( int i = 0; i < m_byTotalStringNum; ++i )
		l_dwStringLength += m_listString[i].m_wLength;

	return l_dwStringLength;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

HSD_HINT::HSD_HINT( void )
{
	m_byLevelMin	= 0;
	m_byLevelMax	= 50;
	m_byMapNo		= 0xFF;		// 모든맵
	m_dwActionID	= 0;
	m_dwTime		= 5000;
	m_bIsDispaly	= TRUE;		// 화면상에 디스플레이할 것인가?
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

HSD_HELP::HSD_HELP()
{
	m_dwWindowID	= ID_INVALID;
	m_dwSectionID	= ID_INVALID;
}
*/
////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CHelpBoard::CHelpBoard()
{
	m_dwPrevOpendWindowID	= ID_INVALID;
}

CHelpBoard::~CHelpBoard()
{
	for( int i = 0; i < MAX_HELP_TYPE_NUM; ++i )
	{
		for( CHelpListIter iter = m_listHelp[i].begin(); iter != m_listHelp[i].end(); ++iter )
			delete ( *iter );

		m_listHelp[i].clear();
	}
}

void
CHelpBoard::InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY, CGUIScreen * pi_pUiScreen )
{
	m_uiHelpWindow.Init();
	m_uiHelpWindow.SetID( pi_dwID );
	m_uiHelpWindow.SetPos( pi_nPosX, pi_nPosY );

	if( pi_pUiScreen ) pi_pUiScreen->Add( &m_uiHelpWindow );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CHelpBoard::FrameMove( void )
{
}

void
CHelpBoard::Render( void )
{
}

BOOL
CHelpBoard::LoadData( void )
{
	return LoadData_Help();
}

BOOL
CHelpBoard::LoadData_Help( void )
{
	CDataFile l_fileHelp( "./DataTable/UIHelp.edf" );

	CDataString * l_pSourceData = l_fileHelp.GetSourceData();
	if( !l_pSourceData )
		return FALSE;
	if( !l_pSourceData->GetString() )
		return FALSE;

	DWORD		l_dwTotalHelpNum;
	HSD_HELP *	l_pHelp;
	for( int i = 0; i < MAX_HELP_TYPE_NUM; ++i )
	{
		l_pSourceData->Read( &l_dwTotalHelpNum, sizeof( DWORD ), 1 );
		if( l_dwTotalHelpNum <= 0 )
			continue;

		m_listHelp[i].reserve( l_dwTotalHelpNum );

		for( int j = 0; j < l_dwTotalHelpNum; ++j )
		{
			l_pHelp = new HSD_HELP;

			l_pSourceData->Read( &l_pHelp->m_dwID, sizeof( DWORD ), 1 );
			l_pSourceData->Read( &l_pHelp->m_dwWindowID, sizeof( DWORD ), 1 );
			l_pSourceData->Read( &l_pHelp->m_dwSectionID, sizeof( DWORD ), 1 );

			LoadData_HSDString( l_pHelp, l_pSourceData );

			m_listHelp[i].push_back( l_pHelp );
		}
	}

	return TRUE;
}

BOOL
CHelpBoard::LoadData_HSDString( HSD_BASE_DATA * pio_pHSDData, CDataString * pi_pSourceData )
{
	if( !pio_pHSDData || !pi_pSourceData )
		return FALSE;

	pi_pSourceData->Read( &pio_pHSDData->m_byTotalStringNum, sizeof( BYTE ), 1 );
	pio_pHSDData->m_listString = new HSD_STRING[pio_pHSDData->m_byTotalStringNum];
	for( int j = 0; j < pio_pHSDData->m_byTotalStringNum; ++j )
	{
		pi_pSourceData->Read( &pio_pHSDData->m_listString[j].m_stFont, sizeof( HSD_FONT ), 1 );
		pi_pSourceData->Read( &pio_pHSDData->m_listString[j].m_wLength, sizeof( WORD ), 1 );
		pio_pHSDData->m_listString[j].m_pString = new char[pio_pHSDData->m_listString[j].m_wLength+1];
		ZeroMemory( pio_pHSDData->m_listString[j].m_pString, pio_pHSDData->m_listString[j].m_wLength+1 );
		pi_pSourceData->Read( pio_pHSDData->m_listString[j].m_pString, pio_pHSDData->m_listString[j].m_wLength, 1 );
	}

	return TRUE;
}

BOOL
CHelpBoard::LoadData_Hint( void )
{
	/*
	CDataFile l_fileHint( "./DataTable/Hint.edf" );

	CDataString * l_pSourceData = l_fileHint.GetSourceData();
	if( !l_pSourceData )
		return FALSE;
	if( !l_pSourceData->GetString() )
		return FALSE;

	l_pSourceData->Read( &m_dwTotalHintNum, sizeof( DWORD ), 1 );

	m_listHint = new HSD_HINT[ m_dwTotalHintNum ];

	for( int i = 0; i < m_dwTotalHintNum; ++i )
	{
		l_pSourceData->Read( &m_listHint[i].m_dwID, sizeof( DWORD ), 1 );
		l_pSourceData->Read( &m_listHint[i].m_byLevelMin, sizeof( BYTE ), 1 );
		l_pSourceData->Read( &m_listHint[i].m_byLevelMax, sizeof( BYTE ), 1 );
		l_pSourceData->Read( &m_listHint[i].m_byMapNo, sizeof( BYTE ), 1 );
		l_pSourceData->Read( &m_listHint[i].m_dwActionID, sizeof( DWORD ), 1 );
		l_pSourceData->Read( &m_listHint[i].m_dwTime, sizeof( DWORD ), 1 );
		l_pSourceData->Read( &m_listHint[i].m_bIsDispaly, sizeof( BOOL ), 1 );

		l_pSourceData->Read( &m_listHint[i].m_byTotalStringNum, sizeof( BYTE ), 1 );
		m_listHint[i].m_listString = new HSD_STRING[m_listHint[i].m_byTotalStringNum];
		for( int j = 0; j < m_listHint[i].m_byTotalStringNum; ++j )
		{
			l_pSourceData->Read( &m_listHint[i].m_listString[j].m_stFont, sizeof( HSD_FONT ), 1 );
			l_pSourceData->Read( &m_listHint[i].m_listString[j].m_wLength, sizeof( WORD ), 1 );
			m_listHint[i].m_listString[j].m_pString = new char[m_listHint[i].m_listString[j].m_wLength+1];
			ZeroMemory( m_listHint[i].m_listString[j].m_pString, m_listHint[i].m_listString[j].m_wLength+1 );
			l_pSourceData->Read( m_listHint[i].m_listString[j].m_pString, m_listHint[i].m_listString[j].m_wLength, 1 );
		}
	}*/

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

LRESULT
CHelpBoard::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
		case WM_OPEN_WINDOW_HELP :
			OpenHelpWindow( wParam );
			break;

		case WM_OPEN_WINDOW_HELP_TOOLTIP :
			OpenHelpTooltip( wParam );
			break;
	}

	return 0;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CHelpBoard::OpenHelpWindow( DWORD pi_dwWindowID )
{
	if( m_dwPrevOpendWindowID == pi_dwWindowID )
	{
		m_uiHelpWindow.Open();
		return;
	}

	if( m_listHelp[pi_dwWindowID].empty() )
		return;

	int			i = 0;
	HELP_INFO * l_listHelp	= new HELP_INFO[ m_listHelp[pi_dwWindowID].size() ];

	for( CHelpListIter iter = m_listHelp[pi_dwWindowID].begin(); iter != m_listHelp[pi_dwWindowID].end(); ++iter, ++i )
	{
		l_listHelp[i].m_dwWindowID		= ( *iter )->m_dwWindowID;
		l_listHelp[i].m_byTotalStrNum	= ( *iter )->m_byTotalStringNum;
		l_listHelp[i].m_pStringInfoList	= reinterpret_cast< HELP_STRING * >( ( *iter )->m_listString );
	}

	m_uiHelpWindow.SetHelpInfo( l_listHelp, m_listHelp[pi_dwWindowID].size() );
	m_uiHelpWindow.Open();

	m_dwPrevOpendWindowID = pi_dwWindowID;

	delete [] l_listHelp;
}

void
CHelpBoard::CloseHelpWindow( void )
{
	m_uiHelpWindow.Close();
}

void
CHelpBoard::OpenHelpTooltip( DWORD pi_dwLinkID )
{
	HELP_INFO l_HelpInfo;

	for( CHelpListIter iter = m_listHelp[UI_HELP_TOOLTIP].begin(); iter != m_listHelp[UI_HELP_TOOLTIP].end(); ++iter )
	{
		if( ( *iter )->IsEqual( pi_dwLinkID ) )
		{
			l_HelpInfo.m_dwWindowID			= ( *iter )->m_dwWindowID;
			l_HelpInfo.m_byTotalStrNum		= ( *iter )->m_byTotalStringNum;
			l_HelpInfo.m_pStringInfoList	= reinterpret_cast< HELP_STRING * >( ( *iter )->m_listString );
			m_uiHelpWindow.SetHelpTooltip( &l_HelpInfo );
			break;
		}
	}
}

void
CHelpBoard::OpenHintWindow( void )
{
}

void
CHelpBoard::CloseHintWindow( void )
{
}