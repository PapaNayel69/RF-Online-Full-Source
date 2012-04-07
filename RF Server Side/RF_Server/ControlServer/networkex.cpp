#include "StdAfx.h"
#include "NetworkEX.h"
#include "rf_Protocol.h"
#include "pt_account_control.h"
#include "pt_control_client.h"
#include "pt_logspy_control.h"
#include "MainThread.h"

extern CMainThread g_Main;
extern void StringOutput(const char *fmt, ...);
static bool sIsCheckLvInfo(int nTable, int nItemIndex, DWORD dwLvInfo, char* OUT pszErrMsg);

void CNetworkEX::AcceptClientCheck(DWORD dwProID, DWORD dwIndex, DWORD dwSerial)
{
	if(dwProID == LINE_ACCOUNT)
	{	
		g_Main.LoginAccountServer();

		::StringOutput("AccountServer Connecting");
	}
	if(dwProID == LINE_LOGSPY)
	{
		::StringOutput("LogSpy Connecting");
	}

	if(dwProID == LINE_CLIENT)
	{
		::StringOutput("Client Connecting");
	}

}

void CNetworkEX::CloseClientCheck(DWORD dwProID, DWORD dwIndex, DWORD dwSerial)
{
	if(dwProID == LINE_ACCOUNT)
	{
		g_Main.m_bLoginAccount = false;

		::StringOutput("AccountServer Disconnecting");
	}
	else if(dwProID == LINE_CLIENT)
	{
		g_Main.m_AdminData[dwIndex].ReleaseData();
	}
}

bool CNetworkEX::DataAnalysis(DWORD	dwProID, DWORD dwClientIndex, _MSG_HEADER* pMsgHeader, char* pMsg)
{
	if(dwProID == LINE_ACCOUNT)
	{
		return AccountLineAnalysis(dwClientIndex, pMsgHeader, pMsg);
	}
	if(dwProID == LINE_CLIENT)
	{
		return ClientLineAnalysis(dwClientIndex, pMsgHeader, pMsg);
	}
	if(dwProID == LINE_LOGSPY)
	{
		return LogSpyLineAnalysis(dwClientIndex, pMsgHeader, pMsg);
	}
	return false;
}

bool CNetworkEX::AccountLineAnalysis(DWORD n, _MSG_HEADER* pMsgHeader, char* pMsg)
{
	if(control_msg == pMsgHeader->m_byType[0])
	{
		if (login_control_result_acco == pMsgHeader->m_byType[1])
			return LoginControlResult(n, pMsg);
		if (world_inf_inform_acco == pMsgHeader->m_byType[1])
			return WorldInfInform(n, pMsg);
		if (user_position_result_acco == pMsgHeader->m_byType[1])
			return UserPositionResult(n, pMsg);
		if (error_msg_inform_acco == pMsgHeader->m_byType[1])
			return ErrorMsgInform(n, pMsg);
		return false;
	}

	return false;
}

bool CNetworkEX::ClientLineAnalysis(DWORD n, _MSG_HEADER* pMsgHeader, char* pMsg)
{
	if(control_msg == pMsgHeader->m_byType[0])
	{
		if (login_admin_request_clco == pMsgHeader->m_byType[1])
			return LoginAdminRequest(n, pMsg);
		if (load_avator_request_clco == pMsgHeader->m_byType[1])
			return LoadAvatorRequest(n, pMsg);
		if (world_service_request_clco == pMsgHeader->m_byType[1])
			return WorldServiceRequest(n, pMsg);
		if (world_exit_request_clco == pMsgHeader->m_byType[1])
			return WorldExitRequest(n, pMsg);
		if (world_msg_request_clco == pMsgHeader->m_byType[1])
			return WorldMsgRequest(n, pMsg);
		if (world_inf_request_clco == pMsgHeader->m_byType[1])
			return WorldInfRequest(n, pMsg);
		if (user_inf_request_clco == pMsgHeader->m_byType[1])
			return UserInfRequest(n, pMsg);
		if (push_item_request_clco == pMsgHeader->m_byType[1])
			return PushItemRequest(n, pMsg);
		if (user_count_request_clco == pMsgHeader->m_byType[1])
			return UserCountRequest(n, pMsg);
		if (history_filecount_request_clco == pMsgHeader->m_byType[1])
			return HistoryFilecountRequest(n, pMsg);
		if (history_fileget_request_clco == pMsgHeader->m_byType[1])
			return HistoryFileGetRequest(n, pMsg);
		if (history_filemore_request_clco == pMsgHeader->m_byType[1])
			return HistoryFileMoreRequest(n, pMsg);
		if (character_rebirth_request_clco == pMsgHeader->m_byType[1])
			return CharacterRebirthRequest(n, pMsg);
		if (user_ban_request_clco == pMsgHeader->m_byType[1])
			return UserBanRequest(n, pMsg);
		if (character_base_info_request_clco == pMsgHeader->m_byType[1])
			return CharacterBaseInfoRequest(n, pMsg);
		if (avator_give_item_request_clco == pMsgHeader->m_byType[1])
			return AvatorGiveItemRequest(n, pMsg);
		if (user_position_request_clco == pMsgHeader->m_byType[1])
			return UserPositionRequest(n, pMsg);
		
		return false;
	}
	return false;
}

bool CNetworkEX::LogSpyLineAnalysis(DWORD n, _MSG_HEADER* pMsgHeader, char* pMsg)
{
	if(control_msg == pMsgHeader->m_byType[0])
	{
		if( history_filecount_result_lsco == pMsgHeader->m_byType[1])
			return HistoryFileCountResult(n, pMsg);

		if( history_fileget_result_lsco == pMsgHeader->m_byType[1])
			return HistoryFileGetResult(n, pMsg);

		return false;
	}
	return false;
}

bool CNetworkEX::LoginControlResult(DWORD n, char* pMsg)
{
	_login_control_result_acco* pRecv = (_login_control_result_acco*)pMsg;

	if(pRecv->bRet)
	{
		g_Main.m_bLoginAccount = true;
		g_Main.m_wRegedWorldNum = pRecv->wRegedWorldNum;
		strcpy(g_Main.m_szAccountDBName, pRecv->szAccountDBName);
		strcpy(g_Main.m_szAccountDBMachine, pRecv->szAccountDBMachine);

		g_Main._DatabaseInit( g_Main.m_szAccountDBName );

		_WORLD_LIST wl[MAX_WORLD_PER_GLOBAL];

		for(int i = 0; i < pRecv->wRegedWorldNum; i++)
		{
			g_Main.m_WorldSetData[i].SetWorld(
				false, false, pRecv->WorldList[i].szWorldName, pRecv->WorldList[i].szDBName);

			::StringOutput("%d) %s world ... service", i, pRecv->WorldList[i].szWorldName);

			strcpy(wl[i].m_szWorldName, pRecv->WorldList[i].szWorldName);
		}

		::StringOutput("Succeed Login To AccountServer");
	}
	else
	{
		::StringOutput("Fail Login To AccountServer");
	}

	return true;
}

bool CNetworkEX::LoginAdminRequest(DWORD n, char* pMsg)
{
	_login_admin_request_clco* pRecv = (_login_admin_request_clco*)pMsg;

	if(strlen(pRecv->szID) > max_id_len)
		return false;

	if(strlen(pRecv->szPW) > max_pw_len)
		return false;
	
	_ADMIN_DATA* pa = &g_Main.m_AdminData[n];
	if(pa->m_bLogin)
		return false;

	//중복접속자 있는지 확인..
	for(int i = 0; i < MAX_ADMIN_PER_GLOBAL; i++)
	{
		if(!pa->m_bLogin)
			continue;

		if(!strcmp(pa->m_szAccountID, pRecv->szID))
		{
			g_Main.SendErrMsgToClient(n, "중복접속자가 있음");
			return true;
		}
	}

	BYTE byGrade = 0;
	char szBufPW[16];
	memset( szBufPW, 0, sizeof(szBufPW) );
	if ( g_Main.m_pAccountDB->Select_HostAccountInfo( pRecv->szID, szBufPW, &byGrade ) == false )
	{
//		g_Main.SendErrMsgToClient(n, "ID가 다릅니다");
		_login_admin_result_cocl Ret;
		Ret.byRetCode = RET_CODE_WRONG_USERID;
		BYTE byType[msg_header_num] = {control_msg, login_admin_result_cocl};
		m_pProcess[LINE_CLIENT]->LoadSendMsg(n, byType, (char*)&Ret, Ret.size());
		return false;
	}

	if ( strcmp( szBufPW, pRecv->szPW ) != 0 )
	{
//		g_Main.SendErrMsgToClient(n, "암호가 맞지 않습니다");
		_login_admin_result_cocl Ret;
		Ret.byRetCode = RET_CODE_WRONG_PASSWD;
		BYTE byType[msg_header_num] = {control_msg, login_admin_result_cocl};
		m_pProcess[LINE_CLIENT]->LoadSendMsg(n, byType, (char*)&Ret, Ret.size());
		return false;
	}

	pa->SetAdminData( byGrade );

	//결과전송
	_login_admin_result_cocl Send;

	Send.byRetCode = 0;
//	Send.byGrade = byGrade;
	Send.wRegedWorldNum =  g_Main.m_wRegedWorldNum;

	for(i = 0; i < g_Main.m_wRegedWorldNum; i++)
	{
		_WORLD_DATA* pw = &g_Main.m_WorldSetData[i];

		Send.WorldList[i].bOpen = pw->m_bOpen;
		strcpy(Send.WorldList[i].szWorldName, pw->m_szName);
	}

	BYTE byType[msg_header_num] = {control_msg, login_admin_result_cocl};
	m_pProcess[LINE_CLIENT]->LoadSendMsg(n, byType, (char*)&Send, Send.size());
	
	return true;
}

bool CNetworkEX::WorldInfInform(DWORD n, char* pMsg)
{
	_world_inf_inform_acco* pRecv = (_world_inf_inform_acco*)pMsg;

	if(pRecv->byWorldNum > MAX_WORLD_PER_GLOBAL)
		return false;

	char szTemp[32] = {0,};
//	sprintf( szTemp, "pRecv->byWorldNum = %d \n", pRecv->byWorldNum );
//	OutputDebugString( szTemp );

	for(int i = 0; i < pRecv->byWorldNum; i++)
	{
		g_Main.m_WorldSetData[i].UpdateWorldState(
			pRecv->WorldInf[i].bOpen, pRecv->WorldInf[i].bService, pRecv->WorldInf[i].nUserNum);
	}

//	sprintf( szTemp, "g_Main.m_wRegedWorldNum = %d \n", g_Main.m_wRegedWorldNum );
//	OutputDebugString( szTemp );

	return true;
}

bool CNetworkEX::UserPositionResult(DWORD n, char* pMsg)
{
	_user_position_result_acco* pRecv = (_user_position_result_acco*)pMsg;

	_user_position_result_cocl Send;
	strcpy( Send.szAccountID, pRecv->szAccountID );
	strcpy( Send.szWorldName, pRecv->szWorldName );
	strcpy( Send.szAvatorName, pRecv->szAvatorName );

	BYTE byType[msg_header_num] = {control_msg, user_position_result_cocl};
	m_pProcess[LINE_CLIENT]->LoadSendMsg( pRecv->wClientIndex, byType, (char*)&Send, Send.size());

	return true;
}

bool CNetworkEX::ErrorMsgInform(DWORD n, char* pMsg)
{
	_error_msg_inform_acco* pRecv = (_error_msg_inform_acco*)pMsg;

	_ADMIN_DATA* pa = &g_Main.m_AdminData[pRecv->idClient.wIndex];

	if(!pa->m_bLogin)
		return true;

	if(pa->m_idLocal.dwSerial != pRecv->idClient.dwSerial)
		return false;

	_error_msg_inform_cocl Send;

	strcpy(Send.szMessage, pRecv->szMessage);

	BYTE byType[msg_header_num] = {control_msg, error_msg_inform_cocl};
	m_pProcess[LINE_CLIENT]->LoadSendMsg(pa->m_idLocal.wIndex, byType, (char*)&Send, Send.size());

	return true;
}

bool CNetworkEX::LoadAvatorRequest(DWORD n, char* pMsg)
{
	_load_avator_request_clco* pRecv = (_load_avator_request_clco*)pMsg;

	if(pRecv->wWorldCode >= g_Main.m_wRegedWorldNum)
		return false;

	if(strlen(pRecv->szAvatorName) > max_name_len)
		return false;

	_WORLD_DATA* pw = &g_Main.m_WorldSetData[pRecv->wWorldCode];
	_ADMIN_DATA* pa =  &g_Main.m_AdminData[n];

	if(!pa->m_bLogin)
	{
		g_Main.SendErrMsgToClient(n, "로그인한 계정이 아님");
		return true;
	}

	_AVATOR_DATA AvatorData;
	char szAccount[32] = {0, };
	char szErrMsg[32] = {0, };

	if(!g_Main.db_Load_Avator(pRecv->wWorldCode, pRecv->szAvatorName, &AvatorData, szAccount, szErrMsg))
	{
		g_Main.SendErrMsgToClient(n, szErrMsg);
		return true;
	}

	_load_avator_result_cocl Ret;

	Ret.wWorldCode = pRecv->wWorldCode;
	strcpy(Ret.szID, szAccount);
	memcpy(&Ret.Data, &AvatorData, sizeof(_AVATOR_DATA));
	Ret.bConnecting = false;

	BYTE byType[msg_header_num] = {control_msg, load_avator_result_cocl};
	m_pProcess[LINE_CLIENT]->LoadSendMsg(n, byType, (char*)&Ret, Ret.size());

	return true;
}

bool CNetworkEX::WorldServiceRequest(DWORD n, char* pMsg)
{
	_world_service_request_clco* pRecv = (_world_service_request_clco*)pMsg;

	_ADMIN_DATA* pa =  &g_Main.m_AdminData[n];
	if(!pa->m_bLogin)
		return false;

	if(pRecv->byWorldCode >= g_Main.m_wRegedWorldNum)
		return false;

	_world_service_request_coac Send;

	memcpy(&Send.idClient, &pa->m_idLocal, sizeof(_CLID));
	Send.byWorldCode = pRecv->byWorldCode;
	Send.bService = pRecv->bService;

	BYTE byType[msg_header_num] = {control_msg, world_service_request_coac};
	m_pProcess[LINE_ACCOUNT]->LoadSendMsg(0, byType, (char*)&Send, Send.size());

	return true;
}

bool CNetworkEX::WorldExitRequest(DWORD n, char* pMsg)
{
	_world_exit_request_clco* pRecv = (_world_exit_request_clco*)pMsg;

	_ADMIN_DATA* pa =  &g_Main.m_AdminData[n];
	if(!pa->m_bLogin)
		return false;

	if(pRecv->byWorldCode >= g_Main.m_wRegedWorldNum)
		return false;

	_world_exit_request_coac Send;

	memcpy(&Send.idClient, &pa->m_idLocal, sizeof(_CLID));
	Send.byWorldCode = pRecv->byWorldCode;

	BYTE byType[msg_header_num] = {control_msg, world_exit_request_coac};
	m_pProcess[LINE_ACCOUNT]->LoadSendMsg(0, byType, (char*)&Send, Send.size());

	return true;
}

bool CNetworkEX::WorldMsgRequest(DWORD n, char* pMsg)
{
	_world_msg_request_clco* pRecv = (_world_msg_request_clco*)pMsg;

	_ADMIN_DATA* pa =  &g_Main.m_AdminData[n];
	if(!pa->m_bLogin)
		return false;

	if(pRecv->byWorldCode >= g_Main.m_wRegedWorldNum)
		return false;

	if(strlen(pRecv->szMsg) > 255)
		return false;

	if(pRecv->byMsgSize > 255-1)
		return false;

	_world_msg_request_coac Send;

	memcpy(&Send.idClient, &pa->m_idLocal, sizeof(_CLID));
	Send.byWorldCode = pRecv->byWorldCode;
	Send.byMsgSize = pRecv->byMsgSize;
	strcpy(Send.szMsg, pRecv->szMsg);

	BYTE byType[msg_header_num] = {control_msg, world_msg_request_coac};
	m_pProcess[LINE_ACCOUNT]->LoadSendMsg(0, byType, (char*)&Send, Send.size());

	return true;
}

bool CNetworkEX::WorldInfRequest(DWORD n, char* pMsg)
{
	_world_inf_request_clco* pRecv = (_world_inf_request_clco*)pMsg;

	_world_inf_result_cocl Send;

	Send.byWorldNum = (BYTE)g_Main.m_wRegedWorldNum;
	for(int i = 0; i < g_Main.m_wRegedWorldNum; i++)
	{
		_WORLD_DATA* pwd = &g_Main.m_WorldSetData[i];

		Send.WorldInf[i].bOpen = pwd->m_bOpen;
		Send.WorldInf[i].bService = pwd->m_bService;
		Send.WorldInf[i].byWorldCode = i;
		Send.WorldInf[i].nUserNum = pwd->m_nUserNum;	
	}

	BYTE byType[msg_header_num] = {control_msg, world_inf_result_cocl};
	m_pProcess[LINE_CLIENT]->LoadSendMsg(n, byType, (char*)&Send, Send.size());

	return true;
}

bool CNetworkEX::UserInfRequest(DWORD n, char* pMsg)
{
	_user_inf_request_clco* pRecv = (_user_inf_request_clco*)pMsg;

	if(strlen(pRecv->szUserAccount) > max_id_len)
		return false;

	_ADMIN_DATA* pa =  &g_Main.m_AdminData[n];
	if(!pa->m_bLogin)
		return false;

	bool bSucc = false;
	DWORD dwAccountSerial = 0xFFFFFFFF;

	_user_inf_result_cocl Send;

	strcpy(Send.szUserAccount, pRecv->szUserAccount);

	if ( g_Main.m_pAccountDB->Select_UserInfoEx( pRecv->szUserAccount, &Send, &dwAccountSerial ) == false )
	{
		g_Main.SendErrMsgToClient(n, "%s는 없는 ID입니다", pRecv->szUserAccount);
		return true;
	}

	//각 월드에서 해당유저의 캐릭터를 가져온다.
	WORD	wCharCnt = 0;
	for(DWORD i = 0; i < g_Main.m_wRegedWorldNum; i++)
	{
		g_Main.m_pWorldDB = new CRFWorldDatabase();
		if ( g_Main.m_pWorldDB->StartDataBase(g_Main.m_WorldSetData[i].m_szDBName, "odinWorld", "tkfkd2du" ) )
		{
			_worlddb_character_array_info characterData;
			memset( &characterData, 0, sizeof(_worlddb_character_array_info) );
			if ( g_Main.m_pWorldDB->Select_CharactersInfo( dwAccountSerial, &characterData ) )
			{
				for ( int j=0; j<characterData.wCharacterCount; j++)
				{
					Send.AvatorData[wCharCnt].wWorldCode = (WORD)i;
					memcpy(Send.AvatorData[wCharCnt].szAvatorName, characterData.CharacterInfo[j].szAvatorName, sizeof(Send.AvatorData[wCharCnt].szAvatorName) );
					Send.AvatorData[wCharCnt].bySlotIndex = characterData.CharacterInfo[j].bySlotIndex;
					Send.AvatorData[wCharCnt].byRaceCode = characterData.CharacterInfo[j].byRaceCode;
					Send.AvatorData[wCharCnt].byLv = characterData.CharacterInfo[j].byLevel;
					Send.AvatorData[wCharCnt].dwSerial = characterData.CharacterInfo[j].dwSerial;
					Send.AvatorData[wCharCnt].byDck = characterData.CharacterInfo[j].byDck;

					wCharCnt++;
				}
			}
			else
			{
				return false;
			}

		}
		else
		{
			return false;
		}
		delete g_Main.m_pWorldDB;
		g_Main.m_pWorldDB = NULL;
	}
	Send.wAvatorNum = wCharCnt;

	BYTE byType[msg_header_num] = {control_msg, user_inf_result_cocl};
	m_pProcess[LINE_CLIENT]->LoadSendMsg(n, byType, (char*)&Send, Send.size());

//	UserPositionRequest( n, pRecv->szUserAccount);

	return true;
}

bool CNetworkEX::PushItemRequest(DWORD n, char* pMsg)
{
	_push_item_request_clco* pRecv = (_push_item_request_clco*)pMsg;

	//맞는 아이템 코드인지..
	int nTable = ::GetItemTableCode(pRecv->szItemCode);
	if(nTable == -1)
	{
		g_Main.SendErrMsgToClient(n, "아이템 테이블코드 에러(%s)", pRecv->szItemCode);
		return true;
	}

	_base_fld* pItemFld = g_Main.m_tblItemData[nTable].GetRecord(pRecv->szItemCode);
	if(!pItemFld)
	{
		g_Main.SendErrMsgToClient(n, "아이템 테이블에 없는 아이템(%s)", pRecv->szItemCode);
		return true;
	}

	//수량검사.. 중첩일경우 max_overlap_num이하인지..
	if(::IsOverLapItem(nTable))
	{
		if(pRecv->dwDur > max_overlap_num)
		{
			g_Main.SendErrMsgToClient(n, "중첩아이템 갯수초과(%s)", pRecv->dwDur);
			return true;
		}
	}
	else	//내구성..
	{
//		::GetItemDurPoint();..
	}

	//


	


	//존재하는 케릭터인지..
	/*
	CRFDatabase DB;	
	DB.SetConnectionStr("sa", "tkfkd2du", g_Main.m_szAccountDBMachine, g_Main.m_szAccountDBName);
	if(!DB.Open())
	{
		g_Main.SendErrMsgToClient(n, "DB 초기화 실패");
		return true;
	}

	DWORD dwAvatorSerial = 0xFFFFFFFF;

	char szSql[128];
	::sprintf(szSql, "SELECT Serial FROM tbl_base WHERE Name = %s AND DCK = 0", pRecv->szAvatorName);
	if(DB.QuerySql(szSql, 1000))
	{
		if(!DB.GetRecordSet()->EndOfFile)
		{
			FieldsPtr pField = DB.GetRecordSet()->Fields;

			dwAvatorSerial = (DWORD)(long)pField->Item["Serial"]->Value;	
		}

		DB.GetRecordSet()->Close();
	}
	else
	{
		g_Main.SendErrMsgToClient(n, "DB Query Failure");
		return true;
	}	

	if(dwAvatorSerial == 0xFFFFFFFF)
	{
		g_Main.SendErrMsgToClient(n, "없는 케릭터 이름임");
		return true;
	}	

	DB.Close();
	*/
	//DB에추가
		

	//결과전송

	return true;

}

bool CNetworkEX::UserCountRequest(DWORD n, char* pMsg)
{
	_user_count_request_clco* userCountRequest = (_user_count_request_clco*)pMsg;

	_user_count_result_cocl userCountData;
	memset( &userCountData, 0, sizeof(_user_count_result_cocl) );

	g_Main.m_pAccountDB->Select_UserCountInfo( userCountRequest->szStartDate, userCountRequest->szEndDate, (char*)&userCountData.wRowCount );
	
	BYTE byType[msg_header_num] = {control_msg, user_count_result_cocl};
	m_pProcess[LINE_CLIENT]->LoadSendMsg(n, byType, (char*)&userCountData, userCountData.size());
	return true;
}

bool CNetworkEX::HistoryFilecountRequest(DWORD n, char* pMsg)
{
	_history_filecount_request_clco* historyFilecount = (_history_filecount_request_clco*)pMsg;

	DWORD dwSerial = 0;

	g_Main.m_pWorldDB = new CRFWorldDatabase();
	
	if ( g_Main.m_pWorldDB->StartDataBase(g_Main.m_WorldSetData[historyFilecount->wWorldCode].m_szDBName, "odinWorld", "tkfkd2du" ) )
	{
		if ( g_Main.m_pWorldDB->Select_CharacterSerial( historyFilecount->szCharacterName, &dwSerial) == false )
		{
			_history_filecount_result_cocl Send;
			Send.dwSerial = 0;
			Send.wFileCount = 0;
			BYTE byType[msg_header_num] = {control_msg, history_filecount_result_cocl};
			m_pProcess[LINE_CLIENT]->LoadSendMsg(n, byType, (char*)&Send, Send.size());
			return false;
		}
	}
	else
		return false;

	delete g_Main.m_pWorldDB;
	g_Main.m_pWorldDB = NULL;

	_history_filecount_request_cols Send;
	Send.dwConCenterSerial = n;
	Send.dwSerial = dwSerial;
	Send.wYear = historyFilecount->wYear;
	Send.byMonth = historyFilecount->byMonth;
	Send.byDay = historyFilecount->byDay;

	BYTE byType[msg_header_num] = {control_msg, history_filecount_request_cols};
	m_pProcess[LINE_LOGSPY]->LoadSendMsg(historyFilecount->wWorldCode, byType, (char*)&Send, Send.size());

	return true;
}

bool CNetworkEX::HistoryFileCountResult(DWORD n, char* pMsg)
{
	_history_filecount_result_lsco* fileCountResult = (_history_filecount_result_lsco*)pMsg;

	_history_filecount_result_cocl Send;

	Send.dwSerial = fileCountResult->dwSerial;
	Send.wFileCount = fileCountResult->wFileCount;

	for ( int i=0; i<Send.wFileCount; i++)
	{
		memcpy( &Send.HistoryFile[i], &fileCountResult->HistoryFile[i], sizeof(Send.HistoryFile[i]) );
	}

	BYTE byType[msg_header_num] = {control_msg, history_filecount_result_cocl};
	m_pProcess[LINE_CLIENT]->LoadSendMsg(fileCountResult->dwConCenterSerial, byType, (char*)&Send, Send.size());

	return true;
}

bool CNetworkEX::HistoryFileGetRequest(DWORD n, char* pMsg)
{
	_history_fileget_request_clco* fileGetRequest = (_history_fileget_request_clco*)pMsg;

	_history_fileget_request_cols Send;
	Send.dwConCenterSerial = n;
	Send.wYear = fileGetRequest->wYear;
	Send.byMonth = fileGetRequest->byMonth;
	Send.byDay = fileGetRequest->byDay;
	Send.byHour = fileGetRequest->byHour;
	memcpy( Send.szFileName, fileGetRequest->szFileName, sizeof(Send.szFileName) );

	BYTE byType[msg_header_num] = {control_msg, history_fileget_request_cols};
	m_pProcess[LINE_LOGSPY]->LoadSendMsg(fileGetRequest->wWorldCode, byType, (char*)&Send, Send.size());

	return true;
}

bool CNetworkEX::HistoryFileMoreRequest(DWORD n, char* pMsg)
{
	_history_filemore_request_clco* fileMoreRequest = (_history_filemore_request_clco*)pMsg;

	_history_filemore_request_cols Send;
	Send.dwConCenterSerial = n;

	BYTE byType[msg_header_num] = {control_msg, history_filemore_request_cols};
	m_pProcess[LINE_LOGSPY]->LoadSendMsg(fileMoreRequest->wWorldCode, byType, (char*)&Send, Send.size());

	return true;
}

bool CNetworkEX::HistoryFileGetResult(DWORD n, char* pMsg)
{
	_history_fileget_result_lsco* fileGetResult = (_history_fileget_result_lsco*)pMsg;

	_history_fileget_result_cocl Send;
	Send.dwFileBodyLength = fileGetResult->dwFileBodyLength;
	memcpy( Send.szFileBody, fileGetResult->szFileBody, fileGetResult->dwFileBodyLength );

	BYTE byType[msg_header_num] = {control_msg, history_fileget_result_cocl};
	m_pProcess[LINE_CLIENT]->LoadSendMsg(fileGetResult->dwConCenterSerial, byType, (char*)&Send, Send.size());

	return true;
}

bool CNetworkEX::CharacterRebirthRequest(DWORD n, char* pMsg)
{
	_character_rebirth_request_clco* rebirthRequest = (_character_rebirth_request_clco*)pMsg;

	g_Main.m_pWorldDB = new CRFWorldDatabase();

	_character_rebirth_result_cocl Send;
	Send.byRet = RET_CODE_DB_QUERY_ERR;

	if ( g_Main.m_pWorldDB->StartDataBase(g_Main.m_WorldSetData[rebirthRequest->wWorldCode].m_szDBName, "odinWorld", "tkfkd2du" ) )
	{
		if ( g_Main.m_pWorldDB->Select_Equal_Name( rebirthRequest->szCharacterName ) )
		{
			if ( g_Main.m_pWorldDB->Rebirth_Base( rebirthRequest->dwCharacterSerial, rebirthRequest->szCharacterName ) )
			{
				Send.byRet = RET_CODE_SUCCESS;
			}
		}
		else
		{
			// 동일이름 존재
		}
	}

	BYTE byType[msg_header_num] = {control_msg, character_rebirth_result_cocl};
	m_pProcess[LINE_CLIENT]->LoadSendMsg(n, byType, (char*)&Send, Send.size());

	delete g_Main.m_pWorldDB;
	g_Main.m_pWorldDB = NULL;

	return true;
}

bool CNetworkEX::UserBanRequest(DWORD n, char* pMsg)
{
	_user_ban_request_clco* userBanRequest = (_user_ban_request_clco*)pMsg;

	DWORD dwAccountSerial;
	char szDate[32];
	memset( szDate, 0, sizeof(szDate) );

	_user_ban_result_cocl Send;
	Send.byRet = user_ban_result_fail;

	if ( g_Main.m_pAccountDB->Select_UserSerial( userBanRequest->szAccountID, &dwAccountSerial, szDate ) )
	{
		BYTE byBanKind;
		WORD wBanPeriod;
		char szBanStartdate[32];
		memset( szBanStartdate, 0, sizeof(szBanStartdate) );

		if ( g_Main.m_pAccountDB->Select_UserBan( dwAccountSerial, &byBanKind, &wBanPeriod, szBanStartdate ) == db_result_sql_success )
		{
			if ( g_Main.m_pAccountDB->Update_UserBan( dwAccountSerial, userBanRequest->byBanKind, userBanRequest->dwBanPeriod ) )
			{
				Send.byRet = user_ban_result_success;
				g_Main.m_pAccountDB->Insert_UserBan_Log( dwAccountSerial, userBanRequest->byBanKind, userBanRequest->dwBanPeriod );
			}
		}
		else
		{
			if ( g_Main.m_pAccountDB->Insert_UserBan( dwAccountSerial, userBanRequest->byBanKind, userBanRequest->dwBanPeriod ) )
			{
				Send.byRet = user_ban_result_success;
				g_Main.m_pAccountDB->Insert_UserBan_Log( dwAccountSerial, userBanRequest->byBanKind, userBanRequest->dwBanPeriod );
			}
		}

		// Account서버에 블럭 사실을 통보한다(접속중이면 실시간으로 끊음)
		_user_block_command_request_coac SendAc;
		SendAc.byBlockKind = userBanRequest->byBanKind;
		SendAc.wBlockHour = (WORD)userBanRequest->dwBanPeriod;
		memcpy( SendAc.szAccountID, userBanRequest->szAccountID, max_name_len+1 );
		BYTE byType[msg_header_num] = {control_msg, user_block_command_request_coac};
		m_pProcess[LINE_ACCOUNT]->LoadSendMsg(0, byType, (char*)&SendAc, SendAc.size());
	}

	BYTE byType[msg_header_num] = {control_msg, user_ban_result_cocl};
	m_pProcess[LINE_CLIENT]->LoadSendMsg(n, byType, (char*)&Send, Send.size());
	
	return true;
}

bool CNetworkEX::CharacterBaseInfoRequest(DWORD n, char* pMsg)
{
	_character_base_info_request_clco* characterBaseInfo = (_character_base_info_request_clco*)pMsg;

	_worlddb_character_base_info dbCharacterInfo;
	memset( &dbCharacterInfo, 0, sizeof(_worlddb_character_base_info) );
	g_Main.m_pWorldDB = new CRFWorldDatabase();

	if ( g_Main.m_pWorldDB->StartDataBase(g_Main.m_WorldSetData[characterBaseInfo->wWorldCode].m_szDBName, "odinWorld", "tkfkd2du" ) )
	{
		BYTE byDBRet = g_Main.m_pWorldDB->Select_CharacterBaseInfoByName( characterBaseInfo->szCharacterName, &dbCharacterInfo );
		
		if ( byDBRet == db_result_sql_error )
		{
			g_Main.SendErrMsgToClient( n, "DB에러 입니다.");
			return false;
		}
		else if ( byDBRet == db_result_no_data )
		{
			g_Main.SendErrMsgToClient( n, "%s는(은) 없는 캐릭터입니다", characterBaseInfo->szCharacterName );
			return false;
		}

		_character_base_info_result_cocl Send;
		Send.byLevel		=	dbCharacterInfo.byLevel;
		Send.byRace			=	dbCharacterInfo.byRace;
		Send.dwDalant		=	dbCharacterInfo.dwDalant;
		Send.dwGold			=	dbCharacterInfo.dwGold;
		Send.dwAvatorSerial	=	dbCharacterInfo.dwSerial;
		BYTE byType[msg_header_num] = {control_msg, character_base_info_result_cocl};
		m_pProcess[LINE_CLIENT]->LoadSendMsg(n, byType, (char*)&Send, Send.size());
	}

	delete g_Main.m_pWorldDB;
	g_Main.m_pWorldDB = NULL;


	return true;
}

bool CNetworkEX::AvatorGiveItemRequest(DWORD n, char* pMsg)
{
	_avator_give_item_request_clco* avatorGiveItem = (_avator_give_item_request_clco*)pMsg;

	_avator_give_item_result_cocl Send;
	memset( &Send, 0, sizeof(_avator_give_item_result_cocl) ); 
	Send.byRet = RET_CODE_DB_QUERY_ERR;
	bool bValidItemCode = true;

	// K값이 Valid 한지 검사
	if ( avatorGiveItem->dwKey == 0xFFFFFFFF )
	{
		strcpy( Send.szErrorMsg, "아이템 코드가 맞지 않습니다" );
		bValidItemCode = false;
	}

	// D값이 Valid 한지 검사
	_INVENKEY invenKey;
	invenKey.SetRelease();
	invenKey.LoadDBKey( avatorGiveItem->dwKey );
	if ( IsOverLapItem( invenKey.byTableCode ) )
	{
		if ( avatorGiveItem->dwDur == 0 || avatorGiveItem->dwDur > max_overlap_num )
		{
			strcpy( Send.szErrorMsg, "수량이 적절치 않습니다" );
			bValidItemCode = false;
		}
	}
	else
	{
		avatorGiveItem->dwDur = GetItemDurPoint( invenKey.byTableCode, invenKey.wItemIndex );
	}

	// U값이 Valid 한지 검사
	if ( sIsCheckLvInfo( invenKey.byTableCode, invenKey.wItemIndex, avatorGiveItem->dwUpt, Send.szErrorMsg ) == false )
		bValidItemCode = false;

	if ( bValidItemCode )
	{
		g_Main.m_pWorldDB = new CRFWorldDatabase();

		if ( g_Main.m_pWorldDB->StartDataBase(g_Main.m_WorldSetData[avatorGiveItem->wWorldCode].m_szDBName, "odinWorld", "tkfkd2du" ) )
		{
			if ( g_Main.m_pWorldDB->Insert_ItemCharge( avatorGiveItem->dwAvatorSerial, avatorGiveItem->dwKey, avatorGiveItem->dwDur, avatorGiveItem->dwUpt ) )
			{
				Send.byRet = RET_CODE_SUCCESS;
			}
			else
			{
				strcpy( Send.szErrorMsg, "DB 에러입니다" );
			}
		}
		else
		{
			strcpy( Send.szErrorMsg, "DB 에러입니다" );
		}

		delete g_Main.m_pWorldDB;
		g_Main.m_pWorldDB = NULL;
	}

	BYTE byType[msg_header_num] = {control_msg, avator_give_item_result_cocl};
	m_pProcess[LINE_CLIENT]->LoadSendMsg(n, byType, (char*)&Send, Send.size());

	return true;
}

bool CNetworkEX::UserPositionRequest(DWORD n, char* pMsg)
{
	_user_position_request_clco* pRecv = (_user_position_request_clco*)pMsg;
	
	_user_position_request_coac Send;
	Send.wClientIndex = (WORD)n;
	strcpy( Send.szAccountID, pRecv->szAccountID );

	BYTE byType[msg_header_num] = {control_msg, user_position_request_coac};
	m_pProcess[LINE_ACCOUNT]->LoadSendMsg(0, byType, (char*)&Send, Send.size());
	return true;
}

bool sIsCheckLvInfo(int nTable, int nItemIndex, DWORD dwLvInfo, char* OUT pszErrMsg)
{
	BYTE byItemKind = ::GetItemKindCode(nTable);
	if(byItemKind == item_kind_std)
	{
		//레벨검사..
		BYTE byMaxSlot = ::GetDefItemUpgSocketNum(nTable, nItemIndex);	//최대소켓수
		BYTE byLimSlot = ::GetItemUpgLimSocket(dwLvInfo);						//설정소켓수

		if(byLimSlot > byMaxSlot)
		{
			sprintf(pszErrMsg, "제한레벨초과(Max:%d)", byMaxSlot);
			return false;
		}

		DWORD dwLvBuffer = ::GetBitAfterSetLimSocket(byLimSlot);

		for(int i = 0; i < byLimSlot; i++)
		{
			BYTE byTalikIndex = ::GetTalikFromSocket(dwLvInfo, i);
			_ItemUpgrade_fld* pUgrFld = g_Main.m_tblItemUpgrade.GetRecord(byTalikIndex);
			if(!pUgrFld)
			{
				sprintf(pszErrMsg, "없는탈릭(Talik:%d)", byTalikIndex);
				return false;
			}

			if(!::IsAddAbleTalikToItem(nTable, nItemIndex, dwLvBuffer, byTalikIndex, &pUgrFld->m_nUpperUp))
			{
				sprintf(pszErrMsg, "탈릭조합에러(Talik:%d)", byTalikIndex);
				return false;
			}
			dwLvBuffer = ::GetBitAfterUpgrade(dwLvBuffer, byTalikIndex, i);
		}

		if(dwLvInfo != dwLvBuffer)	
		{
			sprintf(pszErrMsg, "탈릭조합에러");
			return false;			
		}			
	}
	else if(byItemKind == item_kind_key)
	{
		//unit_storage_num보다 크면 안됀다..
		if(dwLvInfo >= unit_storage_num)
		{
			sprintf(pszErrMsg, "unit_storage_num보다 크다(%d)", dwLvInfo);
			return false;			
		}			
	}
	else if(byItemKind == item_kind_ani)
	{
		if(dwLvInfo != 0)
		{
			sprintf(pszErrMsg, "0 보다 크다(%d)", dwLvInfo);
			return false;			
		}			
	}

	return true;
}