
#include "NetUtil.h"
#include "RFAccountDatabase.h"

CLogFile	accountProcessLog;
CLogFile	accountErrorLog;

CRFAccountDatabase::CRFAccountDatabase()
{
	if(!accountProcessLog.m_bInit)
		accountProcessLog.SetWriteLogFile(".\\DBLog\\AccountDBProcess.log", true, true);
	if(!accountErrorLog.m_bInit)
		accountErrorLog.SetWriteLogFile(".\\DBLog\\AccountDBError.log", true, true);
}

CRFAccountDatabase::~CRFAccountDatabase()
{
	EndDataBase();
}

BYTE	CRFAccountDatabase::Select_UserInfo( char* id , DWORD* pSerial, DWORD* pTotalLogMin, BYTE* pGrade, BYTE* pSubGrade, char* loginDate )
{
	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256];

	wsprintf( strQuery, "{ CALL pSelect_UserInfo('%s') }", id);

	if ( m_bSaveDBLog )
		accountProcessLog.WriteString( strQuery );

	if ( m_hStmtSelect == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			accountErrorLog.Write( "ReConnectDataBase() Fail. Query : %s", strQuery );
			return false;
		}
	}
	
	sqlRet = SQLExecDirect( m_hStmtSelect, ( SQLCHAR *)strQuery, SQL_NTS);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		if ( sqlRet == SQL_NO_DATA )
			return db_result_no_data;
		else
		{
			ErrorAction( sqlRet, strQuery, "SQLExecDirect", m_hStmtSelect );
			return db_result_sql_error;
		}
	}

	sqlRet = SQLFetch(m_hStmtSelect);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		if ( sqlRet == SQL_NO_DATA )
			return db_result_no_data;
		else
		{
			ErrorAction( sqlRet, strQuery, "SQLFetch", m_hStmtSelect );
			return db_result_sql_error;
		}
	}

	sqlRet = SQLGetData( m_hStmtSelect, 1, SQL_C_ULONG, pSerial, 0, &size);
	sqlRet = SQLGetData( m_hStmtSelect, 2, SQL_C_ULONG, pTotalLogMin, 0, &size);
	sqlRet = SQLGetData( m_hStmtSelect, 3, SQL_C_ULONG, pGrade, 0, &size);
	sqlRet = SQLGetData( m_hStmtSelect, 4, SQL_C_ULONG, pSubGrade, 0, &size);
	sqlRet = SQLGetData( m_hStmtSelect, 5, SQL_C_CHAR, loginDate, 32, &size);

	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		ErrorAction( sqlRet, strQuery, "SQLGetData", m_hStmtSelect);
		return db_result_sql_error;
	}

	SQLCloseCursor( m_hStmtSelect );

	if ( m_bSaveDBLog )
		accountProcessLog.Write( "%s Success", strQuery );

	return db_result_sql_success;

}

bool	CRFAccountDatabase::Insert_User( char* id, char* ip )
{
	char		strQuery[256];

	wsprintf( strQuery, "{ CALL pInsert_User('%s', '%s') }", id, ip );
	return ExecUpdateQuery( strQuery );
}

bool	CRFAccountDatabase::Select_UserSerial( char* id, DWORD* pSerial, char* pDate )
{
	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256];
	
	wsprintf( strQuery, "{ CALL pSelect_UserSerial('%s') }", id);

	if ( m_bSaveDBLog )
		accountProcessLog.WriteString( strQuery );

	if ( m_hStmtSelect == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			accountErrorLog.Write( "ReConnectDataBase() Fail. Query : %s", strQuery );
			return false;
		}
	}
	
	sqlRet = SQLExecDirect( m_hStmtSelect, ( SQLCHAR *)strQuery, SQL_NTS);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		if ( sqlRet == SQL_NO_DATA )
			return false;
		else
		{
			ErrorAction( sqlRet, strQuery, "SQLExecDirect", m_hStmtSelect );
			return false;
		}
	}

	sqlRet = SQLFetch(m_hStmtSelect);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		if ( sqlRet == SQL_NO_DATA )
			return false;
		else
		{
			ErrorAction( sqlRet, strQuery, "SQLFetch", m_hStmtSelect );
			return false;
		}
	}

	sqlRet = SQLGetData( m_hStmtSelect, 1, SQL_C_ULONG, pSerial, 0, &size);
	sqlRet = SQLGetData( m_hStmtSelect, 2, SQL_C_CHAR, pDate, 32, &size);

	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		ErrorAction( sqlRet, strQuery, "SQLGetData", m_hStmtSelect );
		return false;
	}

	SQLCloseCursor( m_hStmtSelect );

	if ( m_bSaveDBLog )
		accountProcessLog.Write( "%s Success", strQuery );

	return true;
}

DWORD	CRFAccountDatabase::Select_UserSerial( char* id )
{
	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256], szTemp[32];
	DWORD		dwSerial = 0;

	wsprintf( strQuery, "{ CALL pSelect_UserSerial('%s') }", id);

	if ( m_bSaveDBLog )
		accountProcessLog.WriteString( strQuery );

	if ( m_hStmtSelect == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			accountErrorLog.Write( "ReConnectDataBase() Fail. Query : %s", strQuery );
			return 0;
		}
	}
	
	sqlRet = SQLExecDirect( m_hStmtSelect, ( SQLCHAR *)strQuery, SQL_NTS);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		if ( sqlRet == SQL_NO_DATA )
			return 0;
		else
		{
			ErrorAction( sqlRet, strQuery, "SQLExecDirect", m_hStmtSelect );
			return 0;
		}
	}

	sqlRet = SQLFetch(m_hStmtSelect);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		if ( sqlRet == SQL_NO_DATA )
			return 0;
		else
		{
			ErrorAction( sqlRet, strQuery, "SQLFetch", m_hStmtSelect );
			return 0;
		}
	}

	sqlRet = SQLGetData( m_hStmtSelect, 1, SQL_C_ULONG, &dwSerial, 0, &size);
	sqlRet = SQLGetData( m_hStmtSelect, 2, SQL_C_CHAR, szTemp, 32, &size);

	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		ErrorAction( sqlRet, strQuery, "SQLGetData", m_hStmtSelect );
		return 0;
	}

	SQLCloseCursor( m_hStmtSelect );

	if ( m_bSaveDBLog )
		accountProcessLog.Write( "%s Success", strQuery );

	return dwSerial;
}


bool	CRFAccountDatabase::Update_UserLoginDate( char* id, char* ip )
{
	char strQuery[256];

	wsprintf( strQuery, "{ CALL pUpdate_UserLoginDate('%s', '%s') }", id, ip );
	return ExecUpdateQuery( strQuery );
}

bool	CRFAccountDatabase::Update_UserPushDate( char* id, char* ip )
{
	char strQuery[256];

	wsprintf( strQuery, "{ CALL pUpdate_UserPushDate('%s', '%s') }", id, ip );
	return ExecUpdateQuery( strQuery );
}

bool	CRFAccountDatabase::Update_UserLogoffDate( DWORD dwAccountSerial )
{
	char strQuery[256];

	wsprintf( strQuery, "{ CALL pUpdate_UserLogoffDate( %d ) }", dwAccountSerial );
	return ExecUpdateQuery( strQuery );
}

bool	CRFAccountDatabase::Select_DevSerial( char* id, DWORD* pSerial )
{
	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256];

	wsprintf( strQuery, "{ CALL pSelect_DevSerial('%s') }", id);

	if ( m_bSaveDBLog )
		accountProcessLog.WriteString( strQuery );

	if ( m_hStmtSelect == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			accountErrorLog.Write( "ReConnectDataBase() Fail. Query : %s", strQuery );
			return false;
		}
	}
	
	sqlRet = SQLExecDirect( m_hStmtSelect, ( SQLCHAR *)strQuery, SQL_NTS);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		if ( sqlRet == SQL_NO_DATA )
			return false;
		else
		{
			ErrorAction( sqlRet, strQuery, "SQLExecDirect", m_hStmtSelect );
			return false;
		}
	}

	sqlRet = SQLFetch(m_hStmtSelect);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		if ( sqlRet == SQL_NO_DATA )
			return false;
		else
		{
			ErrorAction( sqlRet, strQuery, "SQLFetch", m_hStmtSelect );
			return false;
		}
	}

	sqlRet = SQLGetData( m_hStmtSelect, 1, SQL_C_ULONG, pSerial, 0, &size);

	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		ErrorAction( sqlRet, strQuery, "SQLGetData", m_hStmtSelect );
		return false;
	}

	SQLCloseCursor( m_hStmtSelect );

	if ( m_bSaveDBLog )
		accountProcessLog.Write( "%s Success", strQuery );

	return true;
}

bool	CRFAccountDatabase::Insert_Dev( char* id, char* pw, char* ip )
{
	char		strQuery[256];

	wsprintf( strQuery, "{ CALL pInsert_Dev('%s', '%s' , '%s') }", id, pw, ip );
	return ExecUpdateQuery( strQuery );
}

bool	CRFAccountDatabase::Select_DevInfo( char* id , char* pw, DWORD* pSerial, DWORD* pTotalLogMin, char* loginDate )
{
	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256];

	wsprintf( strQuery, "{ CALL pSelect_DevInfo('%s') }", id);

	if ( m_bSaveDBLog )
		accountProcessLog.WriteString( strQuery );

	if ( m_hStmtSelect == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			accountErrorLog.Write( "SQLAllocHandle Fail. Query : %s", strQuery );
			return false;
		}
	}
	
	sqlRet = SQLExecDirect( m_hStmtSelect, ( SQLCHAR *)strQuery, SQL_NTS);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		if ( sqlRet == SQL_NO_DATA )
			return false;
		else
		{
			ErrorAction( sqlRet, strQuery, "SQLExecDirect", m_hStmtSelect );
			return false;
		}
	}

	sqlRet = SQLFetch(m_hStmtSelect);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		if ( sqlRet == SQL_NO_DATA )
			return false;
		else
		{
			ErrorAction( sqlRet, strQuery, "SQLFetch", m_hStmtSelect );
			return false;
		}
	}

	sqlRet = SQLGetData( m_hStmtSelect, 1, SQL_C_CHAR, pw, 12, &size);
	sqlRet = SQLGetData( m_hStmtSelect, 2, SQL_C_ULONG, pSerial, 0, &size);
	sqlRet = SQLGetData( m_hStmtSelect, 3, SQL_C_ULONG, pTotalLogMin, 0, &size);
	sqlRet = SQLGetData( m_hStmtSelect, 4, SQL_C_CHAR, loginDate, 32, &size);

	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		ErrorAction( sqlRet, strQuery, "SQLGetData", m_hStmtSelect );
		return false;
	}

	SQLCloseCursor( m_hStmtSelect );

	if ( m_bSaveDBLog )
		accountProcessLog.Write( "%s Success", strQuery );

	return true;

}

bool	CRFAccountDatabase::Update_DevLoginDate( char* id, char* ip )
{
	char strQuery[256];

	wsprintf( strQuery, "{ CALL pUpdate_DevLoginDate('%s', '%s') }", id, ip );
	return ExecUpdateQuery( strQuery );
}

bool	CRFAccountDatabase::Update_DevLogoffDate( char* id, DWORD dwTotalLogMin )
{
	char strQuery[256];

	wsprintf( strQuery, "{ CALL pUpdate_devLogoffDate('%s', %d) }", id, dwTotalLogMin );
	return ExecUpdateQuery( strQuery );
}

bool	CRFAccountDatabase::Insert_UserCountLog( DWORD averageUser, DWORD maxUser )
{
	char strQuery[256];

	wsprintf( strQuery, "{ CALL pInsert_UserCountLog(%d, %d) }", averageUser, maxUser );
	return ExecUpdateQuery( strQuery );
}

bool	CRFAccountDatabase::Insert_UserConnLog( int serial, char* loginDate, char* ip )
{
	char strQuery[256];

	wsprintf( strQuery, "{ CALL pInsert_UserLogoutLog(%d, '%s', '%s') }", serial, loginDate, ip );
	return ExecUpdateQuery( strQuery );
}


bool	CRFAccountDatabase::Insert_UserPushLog( int serial, char* pushIP, char* closeIP )
{
	char strQuery[256];

	wsprintf( strQuery, "{ CALL pInsert_UserPushLog(%d, '%s', '%s') }", serial, pushIP, closeIP );
	return ExecUpdateQuery( strQuery );
}


bool	CRFAccountDatabase::Select_HostAccountInfo( char* id, char* password, BYTE* byGrade )
{
	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256];

	wsprintf( strQuery, "{ CALL pSelect_HostAccountInfo('%s') }", id);

	if ( m_bSaveDBLog )
		accountProcessLog.WriteString( strQuery );

	if ( m_hStmtSelect == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			accountErrorLog.Write( "ReConnectDataBase() Fail. Query : %s", strQuery );
			return false;
		}
	}
	
	sqlRet = SQLExecDirect( m_hStmtSelect, ( SQLCHAR *)strQuery, SQL_NTS);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		if ( sqlRet == SQL_NO_DATA )
			return false;
		else
		{
			ErrorAction( sqlRet, strQuery, "SQLExecDirect", m_hStmtSelect );
			return false;
		}
	}

	sqlRet = SQLFetch(m_hStmtSelect);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		if ( sqlRet == SQL_NO_DATA )
			return false;
		else
		{
			ErrorAction( sqlRet, strQuery, "SQLFetch", m_hStmtSelect );
			return false;
		}
	}

	sqlRet = SQLGetData( m_hStmtSelect, 1, SQL_C_CHAR, password, 16, &size);
	sqlRet = SQLGetData( m_hStmtSelect, 2, SQL_C_USHORT, byGrade, 0, &size);

	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		ErrorAction( sqlRet, strQuery, "SQLGetData", m_hStmtSelect );
		return false;
	}

	SQLCloseCursor( m_hStmtSelect );

	if ( m_bSaveDBLog )
		accountProcessLog.Write( "%s Success", strQuery );


	return true;

}

bool	CRFAccountDatabase::Select_UserInfoEx( char* id, _accountdb_user_base_info* pUserData, DWORD* dwAccountSerial )
{
	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256];

	wsprintf( strQuery, "{ CALL pSelect_UserInfoEx('%s') }", id);

	if ( m_bSaveDBLog )
		accountProcessLog.WriteString( strQuery );

	if ( m_hStmtSelect == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			accountErrorLog.Write( "SQLAllocHandle Fail. Query : %s", strQuery );
			return false;
		}
	}
	
	sqlRet = SQLExecDirect( m_hStmtSelect, ( SQLCHAR *)strQuery, SQL_NTS);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		if ( sqlRet == SQL_NO_DATA )
			return false;
		else
		{
			ErrorAction( sqlRet, strQuery, "SQLExecDirect", m_hStmtSelect );
			return false;
		}
	}

	sqlRet = SQLFetch(m_hStmtSelect);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		if ( sqlRet == SQL_NO_DATA )
			return false;
		else
		{
			ErrorAction( sqlRet, strQuery, "SQLFetch", m_hStmtSelect );
			return false;
		}
	}
	
	sqlRet = SQLGetData( m_hStmtSelect, 1, SQL_C_ULONG,	dwAccountSerial, 0, &size);
	sqlRet = SQLGetData( m_hStmtSelect, 2, SQL_C_CHAR,	pUserData->szCreatTime, 32, &size);
	sqlRet = SQLGetData( m_hStmtSelect, 3, SQL_C_CHAR,	pUserData->szCreateIP, 16, &size);
	sqlRet = SQLGetData( m_hStmtSelect, 4, SQL_C_CHAR,	pUserData->szLastLoginTime, 32, &size);
	sqlRet = SQLGetData( m_hStmtSelect, 5, SQL_C_CHAR,	pUserData->szLastLogoffTime, 32, &size);
	sqlRet = SQLGetData( m_hStmtSelect, 6, SQL_C_ULONG,	&pUserData->dwTotalPlayMTime, 0, &size);
	sqlRet = SQLGetData( m_hStmtSelect, 7, SQL_C_CHAR,	pUserData->szLastLoginIP, 16, &size);

	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		ErrorAction( sqlRet, strQuery, "SQLGetData", m_hStmtSelect );
		return false;
	}

	SQLCloseCursor( m_hStmtSelect );

	if ( m_bSaveDBLog )
		accountProcessLog.Write( "%s Success", strQuery );

	BYTE byBanKind;
	WORD wBanPeriod;
	char szBanStartdate[32];
	memset( szBanStartdate, 0, sizeof(szBanStartdate) );

	if ( Select_UserBan( *dwAccountSerial, &byBanKind, &wBanPeriod, szBanStartdate ) == db_result_sql_success )
	{
		pUserData->byBanKind = byBanKind;
		pUserData->wBanPeriod = wBanPeriod;
		memcpy( pUserData->szBanStartDate, szBanStartdate, sizeof(pUserData->szBanStartDate) );
	}
	else
	{
		pUserData->wBanPeriod = 0;
	}

	return true;
}

bool	CRFAccountDatabase::Select_UserCountInfo( char* szStartDate, char* szEndDate, _accoutndb_user_count_info* pUserCountData )
{
	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256];
	int			nRowCount = 0;

	wsprintf( strQuery, "{ CALL pSelect_UserCountInfo('%s', '%s') }", szStartDate, szEndDate);

	if ( m_bSaveDBLog )
		accountProcessLog.WriteString( strQuery );

	if ( m_hStmtSelect == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			accountErrorLog.Write( "SQLAllocHandle Fail. Query : %s", strQuery );
			return false;
		}
	}
	
	sqlRet = SQLExecDirect( m_hStmtSelect, ( SQLCHAR *)strQuery, SQL_NTS);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		if ( sqlRet == SQL_NO_DATA )
			return false;
		else
		{
			ErrorAction( sqlRet, strQuery, "SQLExecDirect", m_hStmtSelect );
			return false;
		}
	}

	while(TRUE) 
	{
		sqlRet = SQLFetch(m_hStmtSelect);

		if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) break;

		sqlRet  = SQLGetData( m_hStmtSelect, 1, SQL_C_ULONG, &pUserCountData->UserCount[nRowCount].wYear, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 2, SQL_C_ULONG, &pUserCountData->UserCount[nRowCount].byMonth, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 3, SQL_C_ULONG, &pUserCountData->UserCount[nRowCount].byDay, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 4, SQL_C_ULONG, &pUserCountData->UserCount[nRowCount].byHour, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 5, SQL_C_ULONG, &pUserCountData->UserCount[nRowCount].dwAvgCount, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 6, SQL_C_ULONG, &pUserCountData->UserCount[nRowCount].dwMaxCount, 0, &size);

		nRowCount++;

		if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) break;

	}

	pUserCountData->wRowCount = nRowCount;

	SQLCloseCursor( m_hStmtSelect );

	if ( m_bSaveDBLog )
		accountProcessLog.Write( "%s Success", strQuery );

	return true;
}

bool	CRFAccountDatabase::Insert_UserBan( DWORD dwAccountSerial, BYTE byKind, DWORD dwPeriod, char* szBanReason)
{
	char strQuery[256];
	wsprintf( strQuery, "{ CALL pInsert_UserBan(%d, %d, %d, '%s') }", dwAccountSerial, dwPeriod, byKind, szBanReason );
	return ExecUpdateQuery( strQuery );
}

BYTE	CRFAccountDatabase::Select_UserBan( DWORD dwAccountSerial, BYTE* byKind, WORD* wPeriod, char* szStartdate )
{
	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256];

	wsprintf( strQuery, "{ CALL pSelect_UserBan('%d') }", dwAccountSerial);

	if ( m_bSaveDBLog )
		accountProcessLog.WriteString( strQuery );

	if ( m_hStmtSelect == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			accountErrorLog.Write( "SQLAllocHandle Fail. Query : %s", strQuery );
			return db_result_sql_error;
		}
	}
	
	sqlRet = SQLExecDirect( m_hStmtSelect, ( SQLCHAR *)strQuery, SQL_NTS);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		if ( sqlRet == SQL_NO_DATA )
			return db_result_no_data;
		else
		{
			ErrorAction( sqlRet, strQuery, "SQLExecDirect", m_hStmtSelect );
			return db_result_sql_error;
		}
	}

	sqlRet = SQLFetch(m_hStmtSelect);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		if ( sqlRet == SQL_NO_DATA )
			return db_result_no_data;
		else
		{
			ErrorAction( sqlRet, strQuery, "SQLFetch", m_hStmtSelect );
			return db_result_sql_error;
		}
	}
	
	sqlRet = SQLGetData( m_hStmtSelect, 1, SQL_C_CHAR, szStartdate, 32, &size);
	sqlRet = SQLGetData( m_hStmtSelect, 2, SQL_C_ULONG, wPeriod, 0, &size);
	sqlRet = SQLGetData( m_hStmtSelect, 3, SQL_C_USHORT, byKind, 0, &size);
	

	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		ErrorAction( sqlRet, strQuery, "SQLGetData", m_hStmtSelect );
		return db_result_sql_error;
	}

	SQLCloseCursor( m_hStmtSelect );

	if ( m_bSaveDBLog )
		accountProcessLog.Write( "%s Success", strQuery );

	return db_result_sql_success;
}

bool	CRFAccountDatabase::Delete_UserBan( DWORD dwAccountSerial )
{
	char strQuery[256];
	wsprintf( strQuery, "{ CALL pDelete_UserBan( %d ) }", dwAccountSerial );
	return ExecUpdateQuery( strQuery );
}

bool	CRFAccountDatabase::Update_UserBan( DWORD dwAccountSerial, BYTE byKind, DWORD dwPeriod, char* szReason )
{
	char strQuery[256];
	wsprintf( strQuery, "{ CALL pUpdate_UserBan(%d, %d, %d, '%s') }", dwAccountSerial, dwPeriod, byKind, szReason );
	return ExecUpdateQuery( strQuery );
}

BYTE	CRFAccountDatabase::Exist_UserBan( DWORD dwAccountSerial )
{
	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256];
	BYTE		byRet;

	wsprintf( strQuery, "{ CALL pProcess_UserBan( %d ) }", dwAccountSerial);

	if ( m_bSaveDBLog )
		accountProcessLog.WriteString( strQuery );

	if ( m_hStmtSelect == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			accountErrorLog.Write( "ReConnectDataBase Fail. Query : %s", strQuery );
			return db_result_sql_error;
		}
	}

	
	sqlRet = SQLExecDirect( m_hStmtSelect, ( SQLCHAR *)strQuery, SQL_NTS);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		if ( sqlRet == SQL_NO_DATA )
			return db_result_no_data;
		else
			ErrorAction( sqlRet, strQuery, "SQLExecDirect", m_hStmtSelect );

		return db_result_sql_error;
	}

	sqlRet = SQLFetch(m_hStmtSelect);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		if ( sqlRet == SQL_NO_DATA )
			return db_result_no_data;
		else
			ErrorAction( sqlRet, strQuery, "SQLFetch", m_hStmtSelect );
		return db_result_sql_error;
	}

	sqlRet = SQLGetData( m_hStmtSelect, 1, SQL_C_USHORT, &byRet, 0, &size);
	
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		ErrorAction( sqlRet, strQuery, "SQLGetData", m_hStmtSelect );
		return db_result_sql_error;
	}

	SQLCloseCursor( m_hStmtSelect );

	if ( byRet == 1 )				// 지울데이터가 있다(기간만료된 블럭정보)
	{
		Delete_UserBan( dwAccountSerial );
		return block_list_nothing;
	}
	else if ( byRet == 2 )		
	{
		return block_list_account;				// 계정블럭정보가 아직 유효하다(기간만료 안됨)
	}
	else if ( byRet == 3 )
	{
		return block_list_chat;					// 채팅금지정보가 아직 유효하다(기간만료 안됨)
	}
	else
	{
		return block_list_nothing;				// 블럭정보가 없다
	}

}

bool	CRFAccountDatabase::Insert_UserBan_Log( DWORD dwAccountSerial, BYTE byKind, DWORD dwPeriod, char* szReason )
{
	char strQuery[256];
	wsprintf( strQuery, "{ CALL pInsert_UserBanLog(%d, %d, %d, '%s') }", dwAccountSerial, dwPeriod, byKind, szReason );
	return ExecUpdateQuery( strQuery );
}

bool	CRFAccountDatabase::Select_UserBan_History( DWORD dwAccountSerial, _accountdb_user_ban_history_info* banHistory)
{
	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256];
	int			nRowCount = 0;

	wsprintf( strQuery, "{ CALL pSelect_UserBanHistory( %d ) }", dwAccountSerial);

	if ( m_bSaveDBLog )
		accountProcessLog.WriteString( strQuery );

	if ( m_hStmtSelect == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			accountErrorLog.Write( "ReConnectDataBase Fail. Query : %s", strQuery );
			return false;
		}
	}
	
	sqlRet = SQLExecDirect( m_hStmtSelect, ( SQLCHAR *)strQuery, SQL_NTS);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		if ( sqlRet == SQL_NO_DATA )
			return true;
		else
		{
			ErrorAction( sqlRet, strQuery, "SQLExecDirect", m_hStmtSelect );
			return false;
		}
	}

	/*
	SELECT 
	top 32 nKind, dtStartDate, nPeriod, szReason 
	FROM tbl_UserBan_Log 
	WHERE nAccountSerial=@nSerial order by nSerial desc
	*/

	while(TRUE) 
	{
		sqlRet = SQLFetch(m_hStmtSelect);

		if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) break;

		sqlRet  = SQLGetData( m_hStmtSelect, 1, SQL_C_USHORT, &banHistory->banHistoryData[nRowCount].byKind, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 2, SQL_C_CHAR, &banHistory->banHistoryData[nRowCount].szDate, 32, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 3, SQL_C_USHORT, &banHistory->banHistoryData[nRowCount].wPeriod, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 4, SQL_C_CHAR, &banHistory->banHistoryData[nRowCount].szReason, 32, &size);

		nRowCount++;

		if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) break;
	}

	banHistory->byBanHistoryCount = nRowCount;

	SQLCloseCursor( m_hStmtSelect );

	if ( m_bSaveDBLog )
		accountProcessLog.Write( "%s Success", strQuery );

	return true;
}

bool	CRFAccountDatabase::Insert_GMJob( char* szGMID, BYTE nKind, char* szAvatorName, DWORD dwAvatorSerial, DWORD dwAccountSerial, char* szDetail )
{
	char strQuery[256];
	wsprintf( strQuery, "{ CALL pInsert_GMJob( '%s', %d, '%s', %d, %d, '%s' ) }", 
						szGMID, nKind, szAvatorName, dwAvatorSerial, dwAccountSerial, szDetail );
	return ExecUpdateQuery( strQuery );

}

bool	CRFAccountDatabase::Update_WorldServer( int nSerial, int nState )
{
	char strQuery[256];
	wsprintf( strQuery,"{ CALL pUpdate_WorldServer ( %d, %d ) }", nSerial, nState );
	return ExecUpdateQuery( strQuery );
}