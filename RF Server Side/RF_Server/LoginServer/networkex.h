#ifndef _NetworkEX_h_
#define _NetworkEX_h_

#include "NetWorking.h"

#define LINE_ACCOUNT	0
#define LINE_CLIENT		1
#define LINE_NUM		2

class CNetworkEX : public CNetWorking
{
public:

	CNetworkEX(){};
	virtual ~CNetworkEX(){};

private:

	virtual bool DataAnalysis(DWORD	dwProID, DWORD dwClientIndex, _MSG_HEADER* pMsgHeader, char* pMsg);
	virtual void AcceptClientCheck(DWORD dwProID, DWORD dwIndex, DWORD dwSerial);
	virtual void CloseClientCheck(DWORD dwProID, DWORD dwIndex, DWORD dwSerial);

	bool AccountLineAnalysis(DWORD n, _MSG_HEADER* pMsgHeader, char* pMsg);
	bool ClinetLineAnalysis(DWORD n, _MSG_HEADER* pMsgHeader, char* pMsg);

	bool WorldListResult(int n, char* pMsg);	
	bool InformOpenWorld(int n, char* pMsg);	
	bool InformCloseWorld(int n, char* pMsg);	
	bool InformUserNumWorld(int n, char* pMsg);	
	bool JoinAccountResult(int n, char* pMsg);	
	bool LoginAccountResult(int n, char* pMsg);	
	bool SelectWorldResult(int n, char* pMsg);	
	bool PushCloseResult(int n, char* pMsg);
	bool ForceCloseCommand(int n, char* pMsg);
	bool LoginStatRequest(int n, char* pMsg);

	bool JoinAccountRequest(int n, char* pMsg);	
	bool LoginAccountRequest(int n, char* pMsg);
	bool WorldListRequest(int n, char* pMsg);	
	bool SelectWorldRequest(int n, char* pMsg);	
	bool PushCloseRequest(int n, char* pMsg);
	bool CryptKeyRequest(int n, char* pMsg);
};

#endif
