#include "MainThread.h"
#include "MyUtil.h"

static char g_szSql[1280];

BYTE CMainThread::db_join_dev(_qry_sheet_join* pData)
{
	BYTE byRetCode = RET_CODE_SUCCESS;

	//등록된 계정이 있는지 찾는다
	DWORD dwAccountSerial = 0;
	
	if ( m_pAccountDB->Select_DevSerial( pData->in_szAccountID, &dwAccountSerial ) )
	{
		return RET_CODE_REGED_ACCOUNT;
	}

	//등록된 계정이 아니라면 새로 insert시킨다.
	char szClientIP[64];
	strcpy(szClientIP, inet_ntoa(*(in_addr*)&pData->in_dwClientIP));

	if ( m_pAccountDB->Insert_Dev( pData->in_szAccountID, pData->in_szPassword, szClientIP ) )
		byRetCode = RET_CODE_SUCCESS;
	else
		byRetCode = RET_CODE_DB_QUERY_ERR;
	
	return byRetCode;
}

BYTE CMainThread::db_login_account(_qry_sheet_login* pData)
{
	BYTE byRetCode = RET_CODE_SUCCESS;
	bool bReged = false;

	char szClientIP[64];
	strcpy(szClientIP, inet_ntoa(*(in_addr*)&pData->in_dwClientIP));

	if(pData->in_byUserCode == LOGIN_CODE_USER)
	{
		//RF에 등록된 계정인지 확인..
		DWORD pTotalLogMinTemp;
		BYTE byDBRetCode = m_pAccountDB->Select_UserInfo( pData->in_szAccountID, &pData->out_dwAccountSerial, &pTotalLogMinTemp, &pData->out_byUserDgr, &pData->out_bySubDgr, pData->out_szLogDate );
		if ( byDBRetCode == db_result_sql_success )
			bReged = true;
		else if ( byDBRetCode == db_result_sql_error )
			return RET_CODE_DB_QUERY_ERR;

		//등록된 계정이 아니면 새로 삽입한다.
		if(!bReged)
		{
			if ( !m_pAccountDB->Insert_User( pData->in_szAccountID, szClientIP ) )
			{
				return RET_CODE_DB_QUERY_ERR;
			}

			if ( m_pAccountDB->Select_UserSerial( pData->in_szAccountID, &pData->out_dwAccountSerial, pData->out_szLogDate ) )
			{
				pData->out_byUserDgr = USER_DEGREE_STD;
				pData->out_bySubDgr = 0;
			}
			else
				byRetCode = RET_CODE_DB_QUERY_ERR;

			if(byRetCode != RET_CODE_SUCCESS)
				return byRetCode;
		}
		else
		{
			// 블럭 리스트에 있는지 검사
			BYTE byBlockRet = m_pAccountDB->Exist_UserBan( pData->out_dwAccountSerial );

			if ( byBlockRet == block_list_account )
			{
				return RET_CODE_BLOCK_AVATOR;
			}
			else if ( byBlockRet == block_list_chat )
			{
				byRetCode = RET_CODE_BLOCK_CHAT;
			}

		}
	}
	else
	{
		//RF에 등록된 개발계정인지 확인..
		char szPW[16], szLoginDataTemp[32];
		DWORD dwTotalLogMin;
		memset( szPW, 0, sizeof(szPW) );

		if ( m_pAccountDB->Select_DevInfo( pData->in_szAccountID, szPW, &pData->out_dwAccountSerial, &dwTotalLogMin, szLoginDataTemp ) )
		{
			if(!strcmp(szPW, pData->in_szPassword))
			{
				pData->out_byUserDgr = USER_DEGREE_DEV;
				pData->out_bySubDgr = 0;
			}
			else
				byRetCode = RET_CODE_WRONG_PASSWD;	

		}
		else
			byRetCode = RET_CODE_DB_QUERY_ERR;

		if(byRetCode != RET_CODE_SUCCESS)
			return byRetCode;

	}

	// tbl_UserAccount 유저 레코드에 접속시간 및 IP기록(Update)
	if ( pData->out_byUserDgr == LOGIN_CODE_USER )
		m_pAccountDB->Update_UserLoginDate( pData->in_szAccountID, szClientIP );

	return byRetCode;
}

BYTE CMainThread::db_log_usernum(_log_sheet_usernum* pData)
{
	/*
	::sprintf(g_szSql, "INSERT INTO tbl_usernum_RF_World_CloseBeta ( year, month, day, hour, AverageUser, MaxUser ) VALUES (%d, %d, %d, %d, %d, %d )", 
				pData->in_nLastYear, pData->in_nLastMonth, pData->in_nLastDay, pData->in_nLastHour, pData->in_nAveragePerHour, pData->in_nMaxPerHour);

	if(!m_pDB_Account->ExecuteSQL(g_szSql, 1000))
		return RET_CODE_DB_QUERY_ERR;
	return RET_CODE_SUCCESS;
	*/

	if ( m_pAccountDB->Insert_UserCountLog( pData->in_nAveragePerHour, pData->in_nMaxPerHour ) )
		return RET_CODE_SUCCESS;
	else
		return RET_CODE_DB_QUERY_ERR;
}

BYTE CMainThread::db_log_pushclose(_log_sheet_push* pData)
{

	char szPushIP[64], szCloseIP[64];
	strcpy(szPushIP, inet_ntoa(*(in_addr*)&pData->in_dwPushIP));
	strcpy(szCloseIP, inet_ntoa(*(in_addr*)&pData->in_dwCloseIP));

	if ( m_pAccountDB->Insert_UserPushLog( pData->in_dwAvatorSerial, szPushIP, szCloseIP ) )
		return RET_CODE_SUCCESS;
	else
		return RET_CODE_DB_QUERY_ERR;

}

BYTE CMainThread::db_log_logtime(_log_sheet_logtime* pData)
{
	//tbl_UserAccount 유저 레코드에 로그아웃시간 기록
	if ( pData->in_byUserGrade == LOGIN_CODE_USER )
		m_pAccountDB->Update_UserLogoffDate( pData->in_dwAccountSerial );
	
	char szClientIP[64];
	strcpy(szClientIP, inet_ntoa(*(in_addr*)&pData->in_dwClientIP));
	
	if ( m_pAccountDB->Insert_UserConnLog( pData->in_dwAccountSerial, pData->in_szLoginTime, szClientIP ) )
		return RET_CODE_SUCCESS;
	else
		return RET_CODE_DB_QUERY_ERR;
}

BYTE CMainThread::db_upt_worldServer(_upt_case_worldserver* pData)
{
	if ( m_pAccountDB->Update_WorldServer( pData->bySerial, pData->byState ) )
		return RET_CODE_SUCCESS;
	else
		return RET_CODE_DB_QUERY_ERR;
}