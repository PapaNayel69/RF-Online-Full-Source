
#include "NetUtil.h"
#include "RFWebDatabase.h"

CLogFile	webProcessLog;
CLogFile	webErrorLog;


CRFWebDatabase::CRFWebDatabase()
{
	if(!webProcessLog.m_bInit)
		webProcessLog.SetWriteLogFile(".\\DBLog\\WebDBProcess.log", true, true);
	if(!webErrorLog.m_bInit)
		webErrorLog.SetWriteLogFile(".\\DBLog\\WebDBError.log", true, true);
}

CRFWebDatabase::~CRFWebDatabase()
{
	EndDataBase();
}


bool	CRFWebDatabase::Select_AlphaAccountID( char* id )
{
	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256], szTempID[16] = {0,};

	wsprintf( strQuery, "{ CALL pSelect_AlphaAccountID('%s') }", id);

	if ( m_bSaveDBLog )
		webProcessLog.WriteString( strQuery );

	if ( m_hStmtSelect == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			webErrorLog.Write( "ReConnectDataBase Fail. Query : %s", strQuery );
			return false;
		}
	}

	sqlRet = SQLExecDirect( m_hStmtSelect, ( SQLCHAR *)strQuery, SQL_NTS);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		ErrorAction( sqlRet, strQuery, "SQLExecDirect", m_hStmtSelect );
		return false;
	}

	sqlRet = SQLFetch(m_hStmtSelect);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO ) 
	{
		SQLCloseCursor( m_hStmtSelect );
		if ( sqlRet == SQL_NO_DATA ) return false;
	
		ErrorAction( sqlRet, strQuery, "SQLFetch", m_hStmtSelect );
		return false;
	}

	sqlRet = SQLGetData( m_hStmtSelect, 1, SQL_C_CHAR, szTempID, 16, &size);

	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO ) 
	{
		SQLCloseCursor( m_hStmtSelect );
		ErrorAction( sqlRet, strQuery, "SQLGetData", m_hStmtSelect );
		return false;
	}

	if ( m_bSaveDBLog )
		webProcessLog.Write( "%s Success", strQuery );

	SQLCloseCursor( m_hStmtSelect ); 

	return true;
}


bool	CRFWebDatabase::Select_FriendsAccountID( char* id )
{
	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256], szTempID[16];

	memset( szTempID, 0, sizeof(szTempID) );

	wsprintf( strQuery, "{ CALL pSelect_FriendsAccountID('%s') }", id);

	if ( m_bSaveDBLog )
		webProcessLog.WriteString( strQuery );

	if ( m_hStmtSelect == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			webErrorLog.Write( "ReConnectDataBase Fail. Query : %s", strQuery );
			return false;
		}
	}
	
	sqlRet = SQLExecDirect( m_hStmtSelect, ( SQLCHAR *)strQuery, SQL_NTS);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		ErrorAction( sqlRet, strQuery, "SQLExecDirect", m_hStmtSelect );
		return false;
	}

	sqlRet = SQLFetch(m_hStmtSelect);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		if ( sqlRet == SQL_NO_DATA ) return false;

		ErrorAction( sqlRet, strQuery, "SQLFetch", m_hStmtSelect );
		return false;
	}

	sqlRet = SQLGetData( m_hStmtSelect, 1, SQL_C_CHAR, szTempID, 16, &size);

	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		ErrorAction( sqlRet, strQuery, "SQLGetData", m_hStmtSelect );
		return false;
	}

	if ( m_bSaveDBLog )
		webProcessLog.Write( "%s Success", strQuery );

	SQLCloseCursor( m_hStmtSelect );

	return true;
}


BYTE	CRFWebDatabase::Select_X2OnlineAccountPW( char* id, char* pw )
{
	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256], szTempID[16];

	memset( szTempID, 0, sizeof(szTempID) );
	
	wsprintf( strQuery, "SELECT CONVERT(VARCHAR(12), password) FROM userlogin WHERE userid = CONVERT(binary, '%s')", id);

	if ( m_bSaveDBLog )
		webProcessLog.WriteString( strQuery );

	if ( m_hStmtSelect == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			webErrorLog.Write( "ReConnectDataBase Fail. Query : %s", strQuery );
			return false;
		}
	}
	
	sqlRet = SQLExecDirect( m_hStmtSelect, ( SQLCHAR *)strQuery, SQL_NTS);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
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

	sqlRet = SQLGetData( m_hStmtSelect, 1, SQL_C_CHAR, pw, 12, &size);

	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		ErrorAction( sqlRet, strQuery, "SQLGetData", m_hStmtSelect );
		return db_result_sql_error;
	}

	if ( m_bSaveDBLog )
		webProcessLog.Write( "%s Success", strQuery );

	SQLCloseCursor( m_hStmtSelect );

	return db_result_sql_success;
}

bool	CRFWebDatabase::Select_PcbangIP( BYTE* ip )
{
	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256];
	BYTE		byStartIP = 0, byEndIP = 0;
	bool		bFind = false;

	wsprintf( strQuery, "{ CALL pSelect_PcbangIP(%d, %d, %d) }", ip[0], ip[1], ip[2] );

	if ( m_bSaveDBLog )
		webProcessLog.WriteString( strQuery );

	if ( m_hStmtSelect == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			webErrorLog.Write( "ReConnectDataBase Fail. Query : %s", strQuery );
			return false;
		}
	}
	
	sqlRet = SQLExecDirect( m_hStmtSelect, ( SQLCHAR *)strQuery, SQL_NTS);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		ErrorAction( sqlRet, strQuery, "SQLExecDirect", m_hStmtSelect );
		return false;
	}

	while(TRUE) 
	{
		sqlRet = SQLFetch(m_hStmtSelect);

		if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) break;

		sqlRet  = SQLGetData( m_hStmtSelect, 1, SQL_C_ULONG, &byStartIP, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 2, SQL_C_ULONG, &byEndIP, 0, &size);

		if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) break;

		if ( ip[3] >= byStartIP && ip[3] <= byEndIP )
		{
			bFind = true;			// Find it!
			break; 
		}
	}

	if ( m_bSaveDBLog )
		webProcessLog.Write( "%s Success", strQuery );

	SQLCloseCursor( m_hStmtSelect );

	return bFind;

}

bool	CRFWebDatabase::Select_CloseBetaAccount( char* id )
{
	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256], szTempID[16] = {0,};

	wsprintf( strQuery, "{ CALL pSelect_CloseBetaAccount('%s') }", id);

	if ( m_bSaveDBLog )
		webProcessLog.WriteString( strQuery );

	if ( m_hStmtSelect == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			webErrorLog.Write( "ReConnectDataBase Fail. Query : %s", strQuery );
			return false;
		}
	}

	sqlRet = SQLExecDirect( m_hStmtSelect, ( SQLCHAR *)strQuery, SQL_NTS);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		ErrorAction( sqlRet, strQuery, "SQLExecDirect", m_hStmtSelect );
		return false;
	}

	sqlRet = SQLFetch(m_hStmtSelect);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO ) 
	{
		SQLCloseCursor( m_hStmtSelect );
		if ( sqlRet == SQL_NO_DATA ) return false;
	
		ErrorAction( sqlRet, strQuery, "SQLFetch", m_hStmtSelect );
		return false;
	}

	sqlRet = SQLGetData( m_hStmtSelect, 1, SQL_C_CHAR, szTempID, 16, &size);

	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO ) 
	{
		SQLCloseCursor( m_hStmtSelect );
		ErrorAction( sqlRet, strQuery, "SQLGetData", m_hStmtSelect );
		return false;
	}

	if ( m_bSaveDBLog )
		webProcessLog.Write( "%s Success", strQuery );

	SQLCloseCursor( m_hStmtSelect ); 

	return true;

}