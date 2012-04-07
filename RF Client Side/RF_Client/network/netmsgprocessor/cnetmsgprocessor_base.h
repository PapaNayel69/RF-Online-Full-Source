////////////////////////////////////////////////////////////////////////////////
//
// CNetMsgProcessor_Base Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __NET_MSG_BASE_PROCESSOR_H__
#define __NET_MSG_BASE_PROCESSOR_H__

#include "../../Network/CNetworkMgr.h"

#ifdef _DEVELOPER_
	#define NMP_VIRTUAL	virtual
#else
	#define NMP_VIRTUAL
#endif

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CNetMsgProcessor_Base
{
protected :

	CNetworkMgr *	m_pNetworkMgr;

#ifdef _DEVELOPER_

	char *			m_pLogBuffer;
	DWORD			m_dwLogCount;
	char *			m_pNetLogWirteBuffer;

	char *			m_pLogHeader;

#endif // _DEVELOPER_

public :

	CNetMsgProcessor_Base();
	virtual ~CNetMsgProcessor_Base();

	virtual	bool	AnalysisServerPacket( _MSG_HEADER * pMsgHeader, char * pi_pMsg ) = 0;
			BOOL	SendNetMessage( BYTE pi_byMsgType, BYTE pi_byMsgIndex, void * pi_pMsg, int pi_nSize );

#ifdef _DEVELOPER_

			void	NetLogWrite( const char * pi_pFormatString, ... );

#endif // _DEVELOPER_
};

#endif // __NET_MSG_BASE_PROCESSOR_H__
