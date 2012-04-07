#include "StdAfx.h"
#include "NetworkEX.h"
#include "rf_Protocol.h"
#include "pt_account_login.h"
#include "pt_login_client.h"
#include "MainThread.h"
#include "myutil.h"

extern CMainThread g_Main;
extern void StringOutput(const char *fmt, ...);

void CNetworkEX::AcceptClientCheck(DWORD dwProID, DWORD dwIndex, DWORD dwSerial)
{
	if(dwProID == LINE_ACCOUNT)
	{
		g_Main.LoginAccount();
	}
	else if(dwProID == LINE_CLIENT)
	{
		_socket* pSocket = GetSocket(LINE_CLIENT, dwIndex);

	/*	if(!g_Main.m_bExternalOpen)
		{
			if(!g_Main.IsPassIPFilter(*(DWORD*)&pSocket->m_Addr.sin_addr))
			{
				Close(dwProID, dwIndex);
				return;
			}
		}*/
	
		g_Main.m_ClientData[dwIndex].Accept(dwSerial);
		g_Main.m_nAliveClient++;
		pSocket->m_dwBoundResponTerm = 1000000;	//대기시간을 늘려준다..(런쳐에 블럭을 대비)
		
		m_pProcess[dwProID]->StartSpeedHackCheck(dwIndex, dwSerial);
	}
}

void CNetworkEX::CloseClientCheck(DWORD dwProID, DWORD dwIndex, DWORD dwSerial)
{
	if(dwProID == LINE_ACCOUNT)
	{
		g_Main.m_bAccountLogin = false;
		g_Main.m_dwCheckAccountOldTick = timeGetTime()+10000;
		::StringOutput("Account Server Log Off");
	}
	else if(dwProID == LINE_CLIENT)
	{
		_CLIENT_DATA* pObj = &g_Main.m_ClientData[dwIndex];

		//if client select not world, query log_off to account server 
		//
		if(!pObj->m_bSelectWorld && pObj->m_gidGlobal.dwSerial != 0xFFFFFFFF)
		{
			_logout_account_request_loac Send;

			memcpy(&Send.gidGlobal, &pObj->m_gidGlobal, sizeof(_GLBID));

			BYTE byType[msg_header_num] = {system_msg, logout_account_request_loac};
			m_pProcess[LINE_ACCOUNT]->LoadSendMsg(0, byType, (char*)&Send, Send.size());
		}

		//log off 
		//
		pObj->Logoff();
		g_Main.m_nAliveClient--;
	}
}

bool CNetworkEX::DataAnalysis(DWORD	dwProID, DWORD dwClientIndex, _MSG_HEADER* pMsgHeader, char* pMsg)
{
	if(dwProID == LINE_ACCOUNT)
		return AccountLineAnalysis(dwClientIndex, pMsgHeader, pMsg);
	if(dwProID == LINE_CLIENT)
		return ClinetLineAnalysis(dwClientIndex, pMsgHeader, pMsg);
	return false;
}

bool CNetworkEX::AccountLineAnalysis(DWORD n, _MSG_HEADER* pMsgHeader, char* pMsg)
{
	if(system_msg == pMsgHeader->m_byType[0])
	{
		if(world_list_result_aclo == pMsgHeader->m_byType[1])
			return WorldListResult(n, pMsg);
		if(inform_open_world_aclo == pMsgHeader->m_byType[1])
			return InformOpenWorld(n, pMsg);
		if(inform_close_world_aclo == pMsgHeader->m_byType[1])
			return InformCloseWorld(n, pMsg);
		if(inform_usernum_world_aclo == pMsgHeader->m_byType[1])
			return InformUserNumWorld(n, pMsg);
		if(join_account_result_aclo == pMsgHeader->m_byType[1])
			return JoinAccountResult(n, pMsg);
		if(login_account_result_aclo == pMsgHeader->m_byType[1])
			return LoginAccountResult(n, pMsg);
		if(select_world_result_aclo == pMsgHeader->m_byType[1])
			return SelectWorldResult(n, pMsg);
		if(push_close_result_aclo == pMsgHeader->m_byType[1])
			return PushCloseResult(n, pMsg);
		if(force_close_command_aclo == pMsgHeader->m_byType[1])
			return ForceCloseCommand(n, pMsg);
		if(login_server_stat_request_aclo == pMsgHeader->m_byType[1])
			return LoginStatRequest(n, pMsg);
		return false;
	}

	return false;
}

bool CNetworkEX::ClinetLineAnalysis(DWORD n, _MSG_HEADER* pMsgHeader, char* pMsg)
{
	if(account_msg == pMsgHeader->m_byType[0])
	{
		if(join_account_request_cllo == pMsgHeader->m_byType[1])
			return JoinAccountRequest(n, pMsg);
		if(login_account_request_cllo == pMsgHeader->m_byType[1])
			return LoginAccountRequest(n, pMsg);
		if(world_list_request_cllo == pMsgHeader->m_byType[1])
			return WorldListRequest(n, pMsg);
		if(select_world_request_cllo == pMsgHeader->m_byType[1])
			return SelectWorldRequest(n, pMsg);
		if(push_close_request_cllo == pMsgHeader->m_byType[1])
			return PushCloseRequest(n, pMsg);
		if(crypty_key_request_cllo == pMsgHeader->m_byType[1])
			return CryptKeyRequest(n, pMsg);

		return false;
	}
	return false;
}

bool CNetworkEX::WorldListResult(int n, char* pMsg)
{
	_world_list_result_aclo* pRecv = (_world_list_result_aclo*)pMsg;

	if(pRecv->byWorldNum > MAX_WORLD_PER_GLOBAL)
		return false;

	for(int i = 0; i < pRecv->byWorldNum; i++)
	{
		if(strlen(pRecv->WorldList[i].szWorldName) > max_world_name_len)
		{
			::StringOutput("%d번월드이름이 길다", i);
			return false;
		}
	}

	g_Main.pc_WorldListResult(pRecv->byWorldNum, pRecv->WorldList, pRecv->byServiceWorldNum);

	return true;
}

bool CNetworkEX::InformOpenWorld(int n, char* pMsg)
{
	_inform_open_world_aclo* pRecv = (_inform_open_world_aclo*)pMsg;

	if(pRecv->dwWorldCode >= g_Main.m_dwWorldNum)
		return false;

	g_Main.pc_InformOpenWorld(pRecv->dwWorldCode, pRecv->dwGateIP, pRecv->wGatePort);

	return true;
}

bool CNetworkEX::InformCloseWorld(int n, char* pMsg)
{
	_inform_close_world_aclo* pRecv = (_inform_close_world_aclo*)pMsg;

	if(pRecv->dwWorldCode >= g_Main.m_dwWorldNum)
		return false;

	g_Main.pc_InformCloseWorld(pRecv->dwWorldCode);

	return true;
}

bool CNetworkEX::InformUserNumWorld(int n, char* pMsg)
{
	_inform_usernum_world_aclo* pRecv = (_inform_usernum_world_aclo*)pMsg;

	g_Main.pc_InformUserNumWorld(pRecv->byServiceWorldNum, pRecv->wUserNum);

	return true;
}

bool CNetworkEX::JoinAccountResult(int n, char* pMsg)
{
	_join_account_result_aclo* pRecv = (_join_account_result_aclo*)pMsg;

	if(pRecv->idLocal.wIndex >= MAX_LOGIN_CLINET_NUM)
		return false;

	g_Main.pc_JoinAccountResult(pRecv->byRetCode, &pRecv->idLocal);

	return true;	
}

bool CNetworkEX::LoginAccountResult(int n, char* pMsg)
{
	_login_account_result_aclo* pRecv = (_login_account_result_aclo*)pMsg;

	if(pRecv->idLocal.wIndex >= MAX_LOGIN_CLINET_NUM)
		return false;

	g_Main.pc_LoginAccountResult(pRecv->byRetCode, pRecv->dwAccountSerial, pRecv->byUserGrade, &pRecv->idLocal, &pRecv->gidNewGlobal);

	return true;
}

bool CNetworkEX::SelectWorldResult(int n, char* pMsg)
{
	_select_world_result_aclo* pRecv = (_select_world_result_aclo*)pMsg;

	if(pRecv->idLocal.wIndex >= MAX_LOGIN_CLINET_NUM)
		return false;
	
	g_Main.pc_SelectWorldResult(pRecv->byRetCode, &pRecv->idLocal, pRecv->dwWorldMasterKey);

	return true;
}

bool CNetworkEX::PushCloseResult(int n, char* pMsg)
{
	_push_close_result_aclo* pRecv = (_push_close_result_aclo*)pMsg;

	if(pRecv->idLocal.wIndex >= MAX_LOGIN_CLINET_NUM)
		return false;

	g_Main.pc_PushCloseResult(pRecv->byRetCode, &pRecv->idLocal);

	return true;		
}

bool CNetworkEX::ForceCloseCommand(int n, char* pMsg)
{
	_force_close_command_aclo* pRecv = (_force_close_command_aclo*)pMsg;

	if(pRecv->idLocal.wIndex >= MAX_LOGIN_CLINET_NUM)
		return false;

	g_Main.pc_ForceCloseCommand(&pRecv->idLocal);

	return true;		
}

bool CNetworkEX::LoginStatRequest(int n, char* pMsg)
{
	_login_server_stat_request_aclo* pRecv = (_login_server_stat_request_aclo*)pMsg;

	_login_server_stat_result_loac Send;
	Send.wClientIndex = pRecv->wClientIndex;

	if ( pRecv->byStat == 0 )
	{
		
	}
	else if ( pRecv->byStat == 1 )
	{
		g_Main.m_bExternalOpen = true;
		::StringOutput("외부접속 가능상태로 변경");
	}
	else if ( pRecv->byStat == 2 )
	{
		g_Main.m_bExternalOpen = false;
		::StringOutput("외부접속 불가상태로 변경");
	}

	if ( g_Main.m_bExternalOpen )
		Send.byRet = 1;
	else
		Send.byRet = 2;

	BYTE byType[msg_header_num] = {system_msg, login_server_stat_result_loac};
	m_pProcess[LINE_ACCOUNT]->LoadSendMsg(0, byType, (char*)&Send, Send.size());

	return true;
}

bool CNetworkEX::JoinAccountRequest(int n, char* pMsg)
{
	_join_account_request_cllo* pRecv = (_join_account_request_cllo*)pMsg;

	if(strlen(pRecv->szAccountID) > max_id_len)
		return false;

	if(strlen(pRecv->szPassword) > max_pw_len)
		return false;

	g_Main.pc_JoinAccountRequest(n, pRecv->szAccountID, pRecv->szPassword);

	return true;
}

bool CNetworkEX::LoginAccountRequest(int n, char* pMsg)
{
	_login_account_request_cllo* pRecv = (_login_account_request_cllo*)pMsg;

	DeCryptString( pRecv->szAccountID, pRecv->size(), g_Main.m_ClientData[n].m_byPlusKey, g_Main.m_ClientData[n].m_wXORKey );

	if(strlen(pRecv->szAccountID) > max_id_len)
		return false;

	if(strlen(pRecv->szPassword) > max_pw_len)
		return false;

	g_Main.pc_LoginAccountRequest(n, pRecv->szAccountID, pRecv->szPassword);

	return true;
}

bool CNetworkEX::WorldListRequest(int n, char* pMsg)
{
	_world_list_request_cllo* pRecv = (_world_list_request_cllo*)pMsg;

	g_Main.pc_WorldListRequest(n, pRecv->dwClientVersion);

	return true;	
}

bool CNetworkEX::SelectWorldRequest(int n, char* pMsg)
{
	_select_world_request_cllo* pRecv = (_select_world_request_cllo*)pMsg;

	if(pRecv->wWorldIndex >= g_Main.m_dwWorldNum)
		return false;

	g_Main.pc_SelectWorldRequest(n, pRecv->wWorldIndex);

	return true;
}

bool CNetworkEX::PushCloseRequest(int n, char* pMsg)
{
	_push_close_request_cllo* pRecv = (_push_close_request_cllo*)pMsg;

	if(strlen(pRecv->szAccountID) > max_id_len)
		return false;

	if(strlen(pRecv->szPassword) > max_pw_len)
		return false;

	g_Main.pc_PushCloseRequest(n, pRecv->szAccountID, pRecv->szPassword);

	return true;
}

bool CNetworkEX::CryptKeyRequest(int n, char* pMsg)
{
	g_Main.m_ClientData[n].m_byPlusKey = ::rand() % 5;
	g_Main.m_ClientData[n].m_wXORKey = ::rand() % 1024;

	_crypty_key_inform_locl Send;
	Send.byPlus = g_Main.m_ClientData[n].m_byPlusKey;
	Send.wKey = g_Main.m_ClientData[n].m_wXORKey;
	BYTE byType[msg_header_num] = {account_msg, crypty_key_inform_locl};
	m_pProcess[LINE_CLIENT]->LoadSendMsg(n, byType, (char*)&Send, Send.size());

	g_Main.m_ClientData[n].m_byPlusKey += 1;
	g_Main.m_ClientData[n].m_wXORKey += 3;

	return true;
}