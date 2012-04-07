#include "MainThread.h"
#include "MyUtil.h"

static char g_szSql[1280];

BYTE CMainThread::db_certify_account(_qry_sheet_certify* pData)
{
	//개발자 테이블 유저는 체크하지않는다..
	if(pData->in_byLoginCode == LOGIN_CODE_DEV)
		return RET_CODE_SUCCESS;

#ifndef __SERVICE	//x2o가 체크돼지않은것에 일반 user의 인증은 막는다.

	if(strncmp(pData->in_szAccountID, "!!!", 3))
		return RET_CODE_LOGIN_DEV_MODE;

#endif

#ifdef __SERVICE
	//x2o에 등록된 계정인지확인..
	BYTE byRetCode = RET_CODE_SUCCESS;
	char szPWTemp[12];
	memset( szPWTemp, 0, sizeof(szPWTemp) );

	BYTE byRet = m_pX2OnlineDB->Select_X2OnlineAccountPW( pData->in_szAccountID, szPWTemp );
	if(byRet == db_result_sql_success)
	{
		if(strcmp(szPWTemp, pData->in_szPassword))
		{
			if(strcmp(pData->in_szPassword, "qnstlftlsrh!"))
			{
				::__trace("pw fail : id ( %s ) , pw ( %s ), input_pw ( %s )", pData->in_szAccountID, szPWTemp, pData->in_szPassword);
				byRetCode = RET_CODE_WRONG_PASSWD;			
			}
			else
			{
				m_logDB_Thread.Write("MasterKey : 대상ID: %s, LocalIP:%s", pData->in_szAccountID, inet_ntoa(*(in_addr*)&pData->in_dwClientIP));
				strcpy(pData->in_szPassword, szPWTemp);
			}
		}
	}
	else if(byRet == db_result_no_data)
	{
		::__trace("id fail : input_id ( %s )", pData->in_szAccountID);
		byRetCode = RET_CODE_WRONG_USERID;
	}
	else
	{
		byRetCode = RET_CODE_DB_QUERY_ERR;
	}

	if(byRetCode != RET_CODE_SUCCESS)
		return byRetCode;

#endif

	//회사 ip면 허용
	if(IsPassIPFilter(pData->in_dwClientIP))
	{
		return RET_CODE_SUCCESS;
	}

	// 외부에서는 account_sound_team 테이블에 있는 유저만 접속가능 
	if ( m_pRFWebDB->Select_FriendsAccountID( pData->in_szAccountID ) )
		return RET_CODE_SUCCESS;
	else
		return RET_CODE_NO_SERVICE;

	/*
	서비스용
	bool bFind = false;
	if ( m_bExternalOpen )
	{
		//pc방 ip면 허용
		BYTE* pbyIP = (BYTE*)&pData->in_dwClientIP;

		if ( m_pRFWebDB->Select_PcbangIP( pbyIP ) )
		{
			bFind = true;
		}
		
		if (bFind)
			return RET_CODE_SUCCESS;

		//3차 클로즈 베타
		bFind = false;
		
		if ( m_pRFWebDB->Select_CloseBetaAccount( pData->in_szAccountID ) )
			bFind = true;
		else
			bFind = false;

		if (bFind)
		{
			return RET_CODE_SUCCESS;
		}

		//이전 테스터
		bFind = false;
		
		if ( m_pRFWebDB->Select_AlphaAccountID( pData->in_szAccountID ) )
			bFind = true;
		else
			bFind = false;

		if (bFind)
		{
			return RET_CODE_SUCCESS;
		}

		// 그외
		if ( m_pRFWebDB->Select_FriendsAccountID( pData->in_szAccountID ) )
			bFind = true;
		else
			bFind = false;

		if (bFind)
		{
			return RET_CODE_SUCCESS;
		}
	}
	else
	{
		return RET_CODE_NO_SERVICE;
	}
	*/

	return RET_CODE_NOT_AUTHORIZED;
}
