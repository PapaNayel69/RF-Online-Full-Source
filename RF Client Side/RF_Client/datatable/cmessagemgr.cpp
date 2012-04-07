#include "../BaseHeader.h"
#include "../Global.h"
#include "../DataFile.h"

#include "CMessageMgr.h"

#include "../UserInterface/GameWindow/GUIChatWindow.h"

/*
error code :

0xFFFFFFFF
8   : Client, UI, Server, Engine
7   : Login, Game
5-6 : Game Management, Move, Combat, Shop trade, Player trade
4   : Condition, Process, Error, Result, Notify
1-3 : message
*/

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CGameMessage::CGameMessage()
{
	Init();
}

CGameMessage::~CGameMessage()
{
	if( m_listString )
	{
		delete [] m_listString;
		m_listString = NULL;
	}
}

void
CGameMessage::Init( void )
{
	m_dwID				= ID_INVALID;

	m_listString		= NULL;
	m_dwTotalStringNum	= 0;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

int
CGameMessage::GetString( char * po_pBuf )
{
	for( int i = 0, cnt = 0; i < m_dwTotalStringNum; ++i )
	{
		memcpy( &po_pBuf[cnt], m_listString[i].m_pString, m_listString[i].m_wLength );
		cnt += m_listString[i].m_wLength;
	}

	po_pBuf[cnt] = 0;

	return cnt;
}

int
CGameMessage::GetStringLength( void )
{
	int l_nStringLength = 0;

	for( int i = 0; i < m_dwTotalStringNum; ++i )
		l_nStringLength += m_listString[i].m_wLength;

	return l_nStringLength;
}


HSD_STRING *
CGameMessage::GetString_HSD( BYTE pi_byOrderNo )
{
	if( pi_byOrderNo >= m_dwTotalStringNum )
		return NULL;

	return &m_listString[pi_byOrderNo];
}

char *
CGameMessage::GetString( BYTE pi_byOrderNo )
{
	if( pi_byOrderNo >= m_dwTotalStringNum )
		return NULL;

	return m_listString[pi_byOrderNo].m_pString;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CMessageMgr::CMessageMgr()
{
	Init();
}

CMessageMgr::~CMessageMgr()
{
	if( m_listMessage )
	{
		delete [] m_listMessage;
		m_listMessage = NULL;
	}
}

void
CMessageMgr::Init( void )
{
	m_listMessage		= NULL;
	m_dwTotalMessageNum	= 0;

	m_byLanguageType	= GLT_KOREAN;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CMessageMgr::LoadData( void )
{
	const char FILE_NAME[MAX_LANGUAGE_TYPE][32] =
	{
		"./DataTable/Message_k.edf", "./DataTable/Message_e.edf", "./DataTable/Message_j.edf"
	};

	CDataFile l_fileMessageData( ( char * )FILE_NAME[m_byLanguageType] );

	CDataString * l_pSourceData = l_fileMessageData.GetSourceData();
	if( !l_pSourceData )
		return FALSE;
	if( !l_pSourceData->GetString() )
		return FALSE;

	/*
	// data read
	l_pSourceData->Read( &m_dwTotalNum, sizeof( DWORD ), 1 );
	l_pSourceData->Read( &m_dwRecordSize, sizeof( DWORD ), 1 );

	m_pData = new char[ m_dwRecordSize * m_dwTotalNum ];

	l_pSourceData->Read( m_pData, m_dwRecordSize, m_dwTotalNum );
	*/

	return TRUE;
}

BOOL
CMessageMgr::DisplayMessage( DWORD pi_dwID )
{
	static DWORD	l_dwSystemCount = 0;
	static char		l_pHeader[32];
	static char		l_pDisplayString[256];
	static va_list	l_pArgumentList;

	if( !_GetChatWindow() )
		return FALSE;

	if( GetString( l_pDisplayString, pi_dwID ) <= 0 )
		return FALSE;

	sprintf( l_pHeader, "[%d]", ++l_dwSystemCount );
	_GetChatWindow()->InsertText( l_pHeader, l_pDisplayString, CGUIChatWindow::FS_SYSTEM );

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CGameMessage *
CMessageMgr::GetMessage( DWORD pi_dwID )
{
	for( int i = 0; i < m_dwTotalMessageNum; ++i )
	{
		if( m_listMessage[i].GetID() == pi_dwID )
			return &m_listMessage[i];
	}

	return NULL;
}

HSD_STRING *
CMessageMgr::GetString_HSD( DWORD pi_dwID, BYTE pi_byOrderNo )
{
	for( int i = 0; i < m_dwTotalMessageNum; ++i )
	{
		if( m_listMessage[i].GetID() == pi_dwID )
			return m_listMessage[i].GetString_HSD( pi_byOrderNo );
	}

	return NULL;
}

char *
CMessageMgr::GetString( DWORD pi_dwID, BYTE pi_byOrderNo )
{
	for( int i = 0; i < m_dwTotalMessageNum; ++i )
	{
		if( m_listMessage[i].GetID() == pi_dwID )
			return m_listMessage[i].GetString( pi_byOrderNo );
	}

	return NULL;
}

int
CMessageMgr::GetString( char * po_pBuf, DWORD pi_dwID )
{
	for( int i = 0; i < m_dwTotalMessageNum; ++i )
	{
		if( m_listMessage[i].GetID() == pi_dwID )
			return m_listMessage[i].GetString( po_pBuf );
	}

	return NULL;
}

