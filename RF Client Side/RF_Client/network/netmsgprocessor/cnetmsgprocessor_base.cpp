////////////////////////////////////////////////////////////////////////////////
//
// CNetMsgProcessor_Base Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../../GameObject/Manager/CCharacterMgr.h"

#include "CNetMsgProcessor_Base.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifdef _DEVELOPER_

#define	MAX_LOG_LENGTH				( 256 )
#define MAX_NET_LOG_WRITE_LENGTH	( 2048 )
#define MAX_LOG_HEADER_LENGTH		( 16 )

#endif // _DEVELOPER_

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CNetMsgProcessor_Base::CNetMsgProcessor_Base()
{
	m_pNetworkMgr = _GetNetworkMgr();

#ifdef _DEVELOPER_

	m_pLogBuffer			= new char[MAX_LOG_LENGTH];
	m_dwLogCount			= 0;
	m_pNetLogWirteBuffer	= new char[MAX_NET_LOG_WRITE_LENGTH];
	m_pLogHeader			= new char[MAX_LOG_HEADER_LENGTH];

	ZeroMemory( m_pLogBuffer, MAX_LOG_LENGTH );
	ZeroMemory( m_pNetLogWirteBuffer, MAX_NET_LOG_WRITE_LENGTH );
	ZeroMemory( m_pLogHeader, MAX_LOG_HEADER_LENGTH );

#endif // _DEVELOPER_
}

CNetMsgProcessor_Base::~CNetMsgProcessor_Base()
{
#ifdef _DEVELOPER_

	_DELETE_ARRAY( m_pLogBuffer );
	_DELETE_ARRAY( m_pNetLogWirteBuffer );
	_DELETE_ARRAY( m_pLogHeader );

#endif // _DEVELOPER_
}

BOOL
CNetMsgProcessor_Base::SendNetMessage( BYTE pi_byMsgType, BYTE pi_byMsgIndex, void * pi_pMsg, int pi_nSize )
{
	BYTE l_nType[] = { pi_byMsgType, pi_byMsgIndex };
	return m_pNetworkMgr->SendNetMessage( NST_GAME_SERVER, l_nType, pi_pMsg, pi_nSize );
}

#ifdef _DEVELOPER_

void
CNetMsgProcessor_Base::NetLogWrite( const char * pi_pFormatString, ... )
{
	va_list l_pArgumentList;

	va_start( l_pArgumentList, pi_pFormatString );
	vsprintf( m_pNetLogWirteBuffer, pi_pFormatString, l_pArgumentList );
	va_end( l_pArgumentList );

	m_pNetworkMgr->GetClientLog()->Write( "( %s : %d ) %s", m_pLogHeader, ++m_dwLogCount, m_pNetLogWirteBuffer );
}

#endif // _DEVELOPER_