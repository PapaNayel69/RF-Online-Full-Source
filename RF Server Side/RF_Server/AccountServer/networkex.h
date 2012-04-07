#ifndef _NetworkEX_h_
#define _NetworkEX_h_

#include "NetWorking.h"

#define LINE_LOGIN		0
#define LINE_WORLD		1
#define LINE_CONTROL	2
#define LINE_NUM		3

class CNetworkEX : public CNetWorking
{
public:

	CNetworkEX(){};
	virtual ~CNetworkEX(){};

private:

	virtual bool DataAnalysis(DWORD	dwProID, DWORD dwClientIndex, _MSG_HEADER* pMsgHeader, char* pMsg);
	virtual void AcceptClientCheck(DWORD dwProID, DWORD dwIndex, DWORD dwSerial);
	virtual void CloseClientCheck(DWORD dwProID, DWORD dwIndex, DWORD dwSerial);

	bool LoginLineAnalysis(DWORD n, _MSG_HEADER* pMsgHeader, char* pMsg);
	bool WorldLineAnalysis(DWORD n, _MSG_HEADER* pMsgHeader, char* pMsg);
	bool ControlLineAnalysis(DWORD n, _MSG_HEADER* pMsgHeader, char* pMsg);

	//LOGIN LINE..
	bool JoinAccountRequest(DWORD n, char* pMsg);
	bool LoinAccountRequest(DWORD n, char* pMsg);
	bool SelectWorldRequest(DWORD n, char* pMsg);
	bool PushCloseRequest(DWORD n, char* pMsg);
	bool CloseAccountFromLoginRequest(DWORD n, char* pMsg);
	bool WorldListRequest(DWORD n, char* pMsg);
	bool LoginServerStatResult(DWORD n, char* pMsg);

	//WORLD_LINE
	bool OpenWorldRequest(DWORD n, char* pMsg);
	bool StartWorldRequest(DWORD n, char* pMsg);
	bool StopWorldRequest(DWORD n, char* pMsg);
	bool EnterWorldRequest(DWORD n, char* pMsg);
	bool CloseAccountFromWorldRequest(DWORD n, char* pMsg);
	bool TransAccountReport(DWORD n, char* pMsg);
	bool WorldServiceReport(DWORD n, char* pMsg);
	bool SelectAvatorReport(DWORD n, char* pMsg);
	bool EnterLobbyReport(DWORD n, char* pMsg);

	//CONTROL_LINE.. form control
	bool LoginControlRequest(DWORD n, char* pMsg);
//	bool LoginAdminRequest(DWORD n, char* pMsg);
	bool WorldServiceRequest(DWORD n, char* pMsg);
	bool WorldUserExitRequest(DWORD n, char* pMsg);
	bool WorldMsgRequest(DWORD n, char* pMsg);	
	bool UserPositionReqeust(DWORD n, char* pMsg);
	bool UserBlockCommandRequest(DWORD n, char* pMsg);
	bool LoginServerStatRequest(DWORD n, char* pMsg);

	//sync_msg
	bool WorldAccountPing(DWORD n, char* pMsg);
};

#endif
