#include "NetworkEX.h"
#include "rf_Protocol.h"
#include "pt_account_login.h"
#include "pt_account_world.h"
#include "pt_account_control.h"
#include "MainThread.h"

extern CMainThread g_Main;
extern void StringOutput(const char *fmt, ...);

void CNetworkEX::AcceptClientCheck(DWORD dwProID, DWORD dwIndex, DWORD dwSerial)
{
	if(dwProID == LINE_CONTROL)
	{
		if(g_Main.m_nControlSocketIndex != -1)
		{
			Close(dwProID, dwIndex, false);
		}
		else
		{
			::StringOutput("!!Control Server Connecting");
			g_Main.m_nControlSocketIndex = dwIndex;
		}
	}
}

void CNetworkEX::CloseClientCheck(DWORD dwProID, DWORD dwIndex, DWORD dwSerial)
{
	if(dwProID == LINE_LOGIN)
	{
		g_Main.CloseLoginServer(dwIndex);
	}
	else if(dwProID == LINE_WORLD)
	{
		g_Main.CloseWorldServer(dwIndex);
	}
	else if(dwProID == LINE_CONTROL)
	{
		if(m_pProcess[dwProID]->m_NetSocket.m_TotalCount.m_dwAcceptNum == 0)
		{
			g_Main.CloseControlServer();
			g_Main.m_nControlSocketIndex = -1;
		}
	}	
}

bool CNetworkEX::DataAnalysis(DWORD	dwProID, DWORD dwClientIndex, _MSG_HEADER* pMsgHeader, char* pMsg)
{
	if(dwProID == LINE_LOGIN)
	{
		return LoginLineAnalysis(dwClientIndex, pMsgHeader, pMsg);
	}
	if(dwProID == LINE_WORLD)
	{
		return WorldLineAnalysis(dwClientIndex, pMsgHeader, pMsg);
	}
	if(dwProID == LINE_CONTROL)
	{
		return ControlLineAnalysis(dwClientIndex, pMsgHeader, pMsg);
	}
	return false;
}

bool CNetworkEX::LoginLineAnalysis(DWORD n, _MSG_HEADER* pMsgHeader, char* pMsg)
{
	if(system_msg == pMsgHeader->m_byType[0])
	{
		if(join_account_request_loac == pMsgHeader->m_byType[1])
			return JoinAccountRequest(n, pMsg);
		if(login_account_request_loac == pMsgHeader->m_byType[1])
			return LoinAccountRequest(n, pMsg);
		if(select_world_request_loac == pMsgHeader->m_byType[1])
			return SelectWorldRequest(n, pMsg);
		if(push_close_request_loac == pMsgHeader->m_byType[1])
			return PushCloseRequest(n, pMsg);
		if(logout_account_request_loac == pMsgHeader->m_byType[1])
			return CloseAccountFromLoginRequest(n, pMsg);
		if(world_list_request_loac == pMsgHeader->m_byType[1])
			return WorldListRequest(n, pMsg);
		if(login_server_stat_result_loac == pMsgHeader->m_byType[1])
			return LoginServerStatResult(n, pMsg);
		return false;
	}
	if(test_msg == pMsgHeader->m_byType[0])
	{
		if(stress_send_msg == pMsgHeader->m_byType[1])
			return true;
		return false;
	}

	return false;
}

bool CNetworkEX::WorldLineAnalysis(DWORD n, _MSG_HEADER* pMsgHeader, char* pMsg)
{
	if(system_msg == pMsgHeader->m_byType[0])
	{
		if(logout_account_request_wrac == pMsgHeader->m_byType[1])
			return CloseAccountFromWorldRequest(n, pMsg);
		if(open_world_request_wrac == pMsgHeader->m_byType[1])
			return OpenWorldRequest(n, pMsg);
		if(start_world_request_wrac == pMsgHeader->m_byType[1])
			return StartWorldRequest(n, pMsg);
		if(stop_world_request_wrac == pMsgHeader->m_byType[1])
			return StopWorldRequest(n, pMsg);
		if(enter_world_request_wrac == pMsgHeader->m_byType[1])
			return EnterWorldRequest(n, pMsg);
		if(trans_account_report_wrac == pMsgHeader->m_byType[1])
			return TransAccountReport(n, pMsg);
		if(select_avator_report_wrac == pMsgHeader->m_byType[1])
			return SelectAvatorReport(n, pMsg);
		if(enter_lobby_report_wrac == pMsgHeader->m_byType[1])
			return EnterLobbyReport(n, pMsg);
		return false;
	}
	if(control_msg == pMsgHeader->m_byType[0])
	{
		if(world_service_report_wrac == pMsgHeader->m_byType[1])
			return WorldServiceReport(n, pMsg);
		return false;
	}
	if(sync_msg == pMsgHeader->m_byType[0])
	{
		if(world_account_ping_wrac == pMsgHeader->m_byType[1])
			return WorldAccountPing(n, pMsg);
		return false;
	}
	return false;
}

bool CNetworkEX::ControlLineAnalysis(DWORD n, _MSG_HEADER* pMsgHeader, char* pMsg)
{
	if(control_msg == pMsgHeader->m_byType[0])
	{
		if (login_control_request_coac == pMsgHeader->m_byType[1])
			return LoginControlRequest(n, pMsg);
//		if(login_admin_request_coac == pMsgHeader->m_byType[1])
//			return LoginAdminRequest(n, pMsg);
		if (world_service_request_coac == pMsgHeader->m_byType[1])
			return WorldServiceRequest(n, pMsg);
		if (world_exit_request_coac == pMsgHeader->m_byType[1])
			return WorldUserExitRequest(n, pMsg);
		if (world_msg_request_coac == pMsgHeader->m_byType[1])
			return WorldMsgRequest(n, pMsg);
		if (user_position_request_coac == pMsgHeader->m_byType[1])
			return UserPositionReqeust(n, pMsg);
		if (user_block_command_request_coac == pMsgHeader->m_byType[1])
			return UserBlockCommandRequest(n, pMsg);
		if (login_server_stat_request_coac == pMsgHeader->m_byType[1])
			return LoginServerStatRequest(n, pMsg);

		return false;
	}
	return false;
}

//LOGIN LINE..
bool CNetworkEX::JoinAccountRequest(DWORD n, char* pMsg)
{
	_join_account_request_loac* pRecv = (_join_account_request_loac*)pMsg;

	if(strlen(pRecv->szAccountID) > max_id_len)
	{
		g_Main.m_logMain.Write("CNetworkEX::JoinAccountRequest() : ID Size가 크다");
		return false;
	}

	if(strlen(pRecv->szPassword) > max_pw_len)
	{
		g_Main.m_logMain.Write("CNetworkEX::JoinAccountRequest() : PW Size가 크다");
		return false;	
	}

	g_Main.pc_JoinAccount((BYTE)n, &pRecv->idLocal, pRecv->szAccountID, pRecv->szPassword, pRecv->dwClientIP);

	return true;
}

bool CNetworkEX::LoinAccountRequest(DWORD n, char* pMsg)
{
	_login_account_request_loac* pRecv = (_login_account_request_loac*)pMsg;

	if(strlen(pRecv->szAccountID) > max_id_len)
	{
		g_Main.m_logMain.Write("CNetworkEX::LoinAccountRequest() : ID Size가 크다");
		return false;
	}

	if(strlen(pRecv->szPassword) > max_pw_len)
	{
		g_Main.m_logMain.Write("CNetworkEX::LoinAccountRequest() : PW Size가 크다");
		return false;	
	}

	if(pRecv->byUserCode != LOGIN_CODE_USER && pRecv->byUserCode != LOGIN_CODE_DEV)
		return false;

	g_Main.pc_LoginAccount((BYTE)n, pRecv->byUserCode, &pRecv->idLocal, pRecv->szAccountID, pRecv->szPassword, pRecv->dwClientIP);

	return true;
}

bool CNetworkEX::SelectWorldRequest(DWORD n, char* pMsg)
{
	_select_world_request_loac* pRecv = (_select_world_request_loac*)pMsg;

	if(pRecv->wWorldIndex >= g_Main.m_dwWorldSetNum)
	{
		g_Main.m_logMain.Write("CNetworkEX::SelectWorldRequest() : 선택 월드인덱스 오류[%d]", pRecv->wWorldIndex);
		return false;
	}

	if(pRecv->gidGlobal.dwIndex >= MAX_USER_PER_GLOBAL)
	{
		g_Main.m_logMain.Write("CNetworkEX::SelectWorldRequest() : G_id 오류[%d]", pRecv->gidGlobal.dwIndex);
		return false;
	}

	g_Main.pc_SelectWorld((BYTE)n, &pRecv->gidGlobal, pRecv->wWorldIndex, pRecv->dwClientIP);

	return true;
}

bool CNetworkEX::PushCloseRequest(DWORD n, char* pMsg)
{
	_push_close_request_loac* pRecv = (_push_close_request_loac*)pMsg;

//	if(strlen(pRecv->szAccountID) > max_id_len)
//	{
//		g_Main.m_logMain.Write("CNetworkEX::PushCloseRequest() : ID Size가 크다");
//		return false;
//	}

//	if(strlen(pRecv->szPassword) > max_pw_len)
//	{
//		g_Main.m_logMain.Write("CNetworkEX::PushCloseRequest() : PW Size가 크다");
//		return false;	
//	}

	if(pRecv->byUserCode != LOGIN_CODE_USER && pRecv->byUserCode != LOGIN_CODE_DEV)
		return false;

	g_Main.pc_PushClose((BYTE)n, pRecv->byUserCode, &pRecv->idLocal, pRecv->dwAccountSerial, pRecv->dwClientIP); //pRecv->szAccountID, pRecv->szPassword, pRecv->dwClientIP);

	return true;
}

bool CNetworkEX::CloseAccountFromLoginRequest(DWORD n, char* pMsg)
{
	_logout_account_request_loac* pRecv = (_logout_account_request_loac*)pMsg;

	if(pRecv->gidGlobal.dwIndex >= MAX_USER_PER_GLOBAL)
	{
		g_Main.m_logMain.Write("CNetworkEX::CloseAccountFromLoginRequest() : G_id 오류[%d]", pRecv->gidGlobal.dwIndex);
		return false;
	}

	g_Main.pc_CloseAccount(&pRecv->gidGlobal);

	return true;
}

bool CNetworkEX::WorldListRequest(DWORD n, char* pMsg)
{
	_world_list_request_loac* pRecv = (_world_list_request_loac*)pMsg;

	g_Main.pc_WorldListRequest((BYTE)n);

	return true;
}

bool CNetworkEX::LoginServerStatResult(DWORD n, char* pMsg)
{
	_login_server_stat_result_loac* pRecv = (_login_server_stat_result_loac*)pMsg;

	if ( pRecv->byRet == 1 )
		g_Main.m_bExternalLoginOpen = true;
	else
		g_Main.m_bExternalLoginOpen = false;
	
	_login_server_stat_result_acco Send;
	Send.byRet = pRecv->byRet;
	Send.wClientIndex = pRecv->wClientIndex;
	BYTE byType[msg_header_num] = {control_msg, login_server_stat_result_acco};
	m_pProcess[LINE_CONTROL]->LoadSendMsg(0, byType, (char*)&Send, Send.size());
	return true;
}

bool CNetworkEX::OpenWorldRequest(DWORD n, char* pMsg)
{
	_open_world_request_wrac* pRecv = (_open_world_request_wrac*)pMsg;

	g_Main.pc_OpenWorld(n, pRecv->szWorldName, pRecv->dwWorldServerIP, pRecv->byRunMode);

	return true;
}

bool CNetworkEX::StartWorldRequest(DWORD n, char* pMsg)
{
	_start_world_request_wrac* pRecv = (_start_world_request_wrac*)pMsg;

	g_Main.pc_StartWorld(n, pRecv->dwGateIP, pRecv->wGatePort);

	return true;
}

bool CNetworkEX::StopWorldRequest(DWORD n, char* pMsg)
{
	_stop_world_request_wrac* pRecv = (_stop_world_request_wrac*)pMsg;

	g_Main.pc_StopWorld(n);

	return true;
}

bool CNetworkEX::EnterWorldRequest(DWORD n, char* pMsg)
{
	_enter_world_request_wrac* pRecv = (_enter_world_request_wrac*)pMsg;

	if(pRecv->gidGlobal.dwIndex >= MAX_USER_PER_GLOBAL)
	{
		g_Main.m_logMain.Write("CNetworkEX::EnterWorldRequest() : G_id 오류[%d]", pRecv->gidGlobal.dwIndex);
		return false;
	}

	g_Main.pc_EnterWorldRequest(n, &pRecv->gidGlobal, &pRecv->idLocal);

	return true;
}


bool CNetworkEX::CloseAccountFromWorldRequest(DWORD n, char* pMsg)
{
	_logout_account_request_wrac* pRecv = (_logout_account_request_wrac*)pMsg;

	if(pRecv->gidGlobal.dwIndex >= MAX_USER_PER_GLOBAL)
	{
		g_Main.m_logMain.Write("CNetworkEX::CloseAccountFromWorldRequest() : G_id 오류[%d]", pRecv->gidGlobal.dwIndex);
		return false;
	}

	g_Main.pc_CloseAccount(&pRecv->gidGlobal);

	return true;
}

bool CNetworkEX::TransAccountReport(DWORD n, char* pMsg)
{
	_trans_account_report_wrac* pRecv = (_trans_account_report_wrac*)pMsg;

	if(pRecv->gidGlobal.dwIndex >= MAX_USER_PER_GLOBAL)
	{
		g_Main.m_logMain.Write("CNetworkEX::TransAccountReport() : G_id 오류[%d]", pRecv->gidGlobal.dwIndex);
		return false;
	}

	g_Main.pc_TransAccountReport(n, &pRecv->gidGlobal, pRecv->byRetCode);

	return true;
}

bool CNetworkEX::WorldServiceReport(DWORD n, char* pMsg)
{
	_world_service_report_wrac* pRecv = (_world_service_report_wrac*)pMsg;

	g_Main.pc_WorldServiceReport(n, pRecv->bBefore, pRecv->bAfter);

	return true;
}

bool CNetworkEX::LoginControlRequest(DWORD n, char* pMsg)
{
	_login_control_request_coac* pRecv = (_login_control_request_coac*)pMsg;

	int asd = pRecv->size();

	g_Main.pc_LoginControlRequest(pRecv->dwMasterKey);

	return true;
}
/*
bool CNetworkEX::LoginAdminRequest(DWORD n, char* pMsg)
{
	_login_admin_request_coac* pRecv = (_login_admin_request_coac*)pMsg;

	if(strlen(pRecv->szID) > max_id_len)
		return false;

	if(strlen(pRecv->szPW) > max_pw_len)
		return false;

	g_Main.pc_LoginAdminRequest(pRecv->szID, pRecv->szPW, &pRecv->idClient);

	return true;
}
*/
bool CNetworkEX::WorldServiceRequest(DWORD n, char* pMsg)
{
	_world_service_request_coac* pRecv = (_world_service_request_coac*)pMsg;

	if(pRecv->idClient.wIndex >= MAX_ADMIN_PER_GLOBAL)
		return false;

	if(pRecv->byWorldCode >= g_Main.m_dwWorldSetNum)
		return false;

	g_Main.pc_WorldServiceRequest(pRecv->byWorldCode, pRecv->bService, &pRecv->idClient);

	return true;
}

bool CNetworkEX::WorldUserExitRequest(DWORD n, char* pMsg)
{
	_world_exit_request_coac* pRecv = (_world_exit_request_coac*)pMsg;

	if(pRecv->idClient.wIndex >= MAX_ADMIN_PER_GLOBAL)
		return false;

	if(pRecv->byWorldCode >= g_Main.m_dwWorldSetNum)
		return false;

	g_Main.pc_WorldUserExitRequest(pRecv->byWorldCode, &pRecv->idClient);

	return true;
}

bool CNetworkEX::WorldMsgRequest(DWORD n, char* pMsg)
{
	_world_msg_request_coac* pRecv = (_world_msg_request_coac*)pMsg;

	if(pRecv->idClient.wIndex >= MAX_ADMIN_PER_GLOBAL)
		return false;

	if(pRecv->byWorldCode >= g_Main.m_dwWorldSetNum)
		return false;

	if(strlen(pRecv->szMsg) >= 255)
		return false;

	if(pRecv->byMsgSize >= 255)
		return false;

	g_Main.pc_WorldMsgRequest(pRecv->byWorldCode, pRecv->byMsgSize, pRecv->szMsg, &pRecv->idClient);

	return true;
}

bool CNetworkEX::UserPositionReqeust(DWORD n, char* pMsg)
{
	_user_position_request_coac* pRecv = (_user_position_request_coac*)pMsg;

	char szWorldName[max_world_name_len+1] = {0,};
	char szAvatorName[max_name_len+1] = {0,};

	g_Main.SearchUserPosition( pRecv->szAccountID, szWorldName, szAvatorName );
	g_Main.pc_UserPositionResult( pRecv->wClientIndex, pRecv->szAccountID, szWorldName, szAvatorName );

	return true;
}

bool CNetworkEX::UserBlockCommandRequest(DWORD n, char* pMsg)
{
	_user_block_command_request_coac* pRecv = (_user_block_command_request_coac*)pMsg;
	g_Main.pc_UserBlockRequest( pRecv->szAccountID, pRecv->byBlockKind, pRecv->wBlockHour );
	return true;
}

bool CNetworkEX::WorldAccountPing(DWORD n, char* pMsg)
{
	g_Main.pc_PingWorld(n);
	return true;
}


bool CNetworkEX::SelectAvatorReport(DWORD n, char* pMsg)
{
	_select_avator_report_wrac* pRecv = (_select_avator_report_wrac*)pMsg;
	g_Main.pc_UserSelectAvatorReport( pRecv->gidGlobal, pRecv->szCharName, pRecv->dwAvatorSerial );
	return true;
}

bool CNetworkEX::EnterLobbyReport(DWORD n, char* pMsg)
{
	_enter_lobby_report_wrac* pRecv = (_enter_lobby_report_wrac*)pMsg;
	g_Main.pc_UserEnterLobby( pRecv->gidGlobal );
	return true;
}

bool CNetworkEX::LoginServerStatRequest(DWORD n, char* pMsg)
{
	_login_server_stat_request_coac* pRecv = (_login_server_stat_request_coac*)pMsg;

	_login_server_stat_request_aclo Send;
	Send.byStat = pRecv->byStat;
	Send.wClientIndex = pRecv->wClientIndex;
	
	BYTE byType[msg_header_num] = {system_msg, login_server_stat_request_aclo};

	for(int i = 0; i < MAX_LOGINER_PER_GLOBAL; i++)
	{
		if(g_Main.m_LobbyData[i].m_bOpen)
			m_pProcess[LINE_LOGIN]->LoadSendMsg(i, byType, (char*)&Send, Send.size());
	}
	
	return true;
}