#ifndef _NetworkEX_h_
#define _NetworkEX_h_

#include "NetWorking.h"

#define LINE_ACCOUNT	0
#define LINE_CLIENT		1
#define LINE_LOGSPY		2
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

	bool AccountLineAnalysis(DWORD n, _MSG_HEADER* pMsgHeader, char* pMsg);
	bool ClientLineAnalysis(DWORD n, _MSG_HEADER* pMsgHeader, char* pMsg);
	bool LogSpyLineAnalysis(DWORD n, _MSG_HEADER* pMsgHeader, char* pMsg);

	//ACCOUNT..
	bool LoginControlResult(DWORD n, char* pMsg);
	bool WorldInfInform(DWORD n, char* pMsg);
	bool UserPositionResult(DWORD n, char* pMsg);
	bool ErrorMsgInform(DWORD n, char* pMsg);

	//CLIENT..
	bool LoginAdminRequest(DWORD n, char* pMsg);
	bool LoadAvatorRequest(DWORD n, char* pMsg);

	bool WorldServiceRequest(DWORD n, char* pMsg);
	bool WorldExitRequest(DWORD n, char* pMsg);
	bool WorldMsgRequest(DWORD n, char* pMsg);
	bool WorldInfRequest(DWORD n, char* pMsg);
	bool UserInfRequest(DWORD n, char* pMsg);
	bool PushItemRequest(DWORD n, char* pMsg);
	bool UserCountRequest(DWORD n, char* pMsg);
	bool CharacterRebirthRequest(DWORD n, char* pMsg);
	bool UserBanRequest(DWORD n, char* pMsg);
	bool CharacterBaseInfoRequest(DWORD n, char* pMsg);
	bool AvatorGiveItemRequest(DWORD n, char* pMsg);
	bool UserPositionRequest(DWORD n, char* pMsg);
	

	// LogSpy
	bool HistoryFilecountRequest(DWORD n, char* pMsg);
	bool HistoryFileCountResult(DWORD n, char* pMsg);

	bool HistoryFileGetRequest(DWORD n, char* pMsg);
	bool HistoryFileGetResult(DWORD n, char* pMsg);
	bool HistoryFileMoreRequest(DWORD n, char* pMsg);

	
};

#endif
