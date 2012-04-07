
#include "NetUtil.h"
#include "RFControlDatabase.h"
#include "pt_control_client.h"

CLogFile	controlProcessLog;
CLogFile	controlErrorLog;

CRFControlDatabase::CRFControlDatabase()
{
	controlProcessLog.SetWriteLogFile(".\\DBLog\\ControlDBProcess.log", true, true);
	controlErrorLog.SetWriteLogFile(".\\DBLog\\ControlDBError.log", true, true);
}

CRFControlDatabase::~CRFControlDatabase()
{
	EndDataBase();
}

bool	CRFControlDatabase::Select_HostAccountInfo( char* id, char* password, bool* readable, bool* writable )
{
	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256];
	int			nReadable, nWritable;

	sqlRet = SQLAllocHandle( SQL_HANDLE_STMT, m_hDbc, &m_hStmtSelect );
	if (sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO)
	{
		controlErrorLog.Write( "SQLAllocHandle Fail. id : %s", id );
		return false;
	}

	wsprintf( strQuery, "{ CALL pSelect_HostAccountInfo('%s') }", id);

	if ( m_bSaveDBLog )
		controlProcessLog.Write( strQuery );
	
	sqlRet = SQLExecDirect( m_hStmtSelect, ( SQLCHAR *)strQuery, SQL_NTS);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		ErrorAction( sqlRet, strQuery, "SQLExecDirect" );
		SQLFreeHandle( SQL_HANDLE_STMT, m_hStmtSelect );
		return false;
	}

	sqlRet = SQLFetch(m_hStmtSelect);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		ErrorAction( sqlRet, strQuery, "SQLFetch" );
		SQLFreeHandle(SQL_HANDLE_STMT, m_hStmtSelect );
		return false;
	}

	sqlRet = SQLGetData( m_hStmtSelect, 1, SQL_C_CHAR, password, 16, &size);
	sqlRet = SQLGetData( m_hStmtSelect, 2, SQL_C_ULONG, &nReadable, 0, &size);
	sqlRet = SQLGetData( m_hStmtSelect, 3, SQL_C_ULONG, &nWritable, 0, &size);

	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		ErrorAction( sqlRet, strQuery, "SQLGetData" );
		SQLFreeHandle(SQL_HANDLE_STMT, m_hStmtSelect );
		return false;
	}

	SQLFreeHandle(SQL_HANDLE_STMT, m_hStmtSelect );

	if ( nReadable == 1 )
		*readable = true;
	else
		*readable = false;

	if ( nWritable == 1 )
		*writable = true;
	else
		*writable = false;

	if ( m_bSaveDBLog )
		controlProcessLog.Write( "%s Success", strQuery );


	return true;

}

bool	CRFControlDatabase::Select_UserInfoEx( char* id, _controldb_user_info_data* pUserData )
{
	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256];

	sqlRet = SQLAllocHandle( SQL_HANDLE_STMT, m_hDbc, &m_hStmtSelect );
	if (sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO)
	{
		controlErrorLog.Write( "SQLAllocHandle Fail. id : %s", id );
		return false;
	}

	wsprintf( strQuery, "{ CALL pSelect_UserInfoEx('%s') }", id);

	if ( m_bSaveDBLog )
		controlProcessLog.Write( strQuery );

	
	sqlRet = SQLExecDirect( m_hStmtSelect, ( SQLCHAR *)strQuery, SQL_NTS);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		ErrorAction( sqlRet, strQuery, "SQLExecDirect" );
		SQLFreeHandle( SQL_HANDLE_STMT, m_hStmtSelect );
		return false;
	}

	sqlRet = SQLFetch(m_hStmtSelect);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		ErrorAction( sqlRet, strQuery, "SQLFetch" );
		SQLFreeHandle(SQL_HANDLE_STMT, m_hStmtSelect );
		return false;
	}

	sqlRet = SQLGetData( m_hStmtSelect, 1, SQL_C_ULONG,	&pUserData->dwSerial, 0, &size);
	sqlRet = SQLGetData( m_hStmtSelect, 2, SQL_C_CHAR,	pUserData->szCreateTime, 32, &size);
	sqlRet = SQLGetData( m_hStmtSelect, 3, SQL_C_ULONG,	&pUserData->wGrade, 0, &size);
	sqlRet = SQLGetData( m_hStmtSelect, 4, SQL_C_CHAR,	pUserData->szCreateip, 16, &size);
	sqlRet = SQLGetData( m_hStmtSelect, 5, SQL_C_CHAR,	pUserData->szLastloginTime, 32, &size);
	sqlRet = SQLGetData( m_hStmtSelect, 6, SQL_C_CHAR,	pUserData->szLastlogofftime, 32, &size);
	sqlRet = SQLGetData( m_hStmtSelect, 7, SQL_C_ULONG,	&pUserData->dwTotallogmin, 0, &size);
	sqlRet = SQLGetData( m_hStmtSelect, 8, SQL_C_CHAR,	pUserData->szLastconnectip, 16, &size);

	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		ErrorAction( sqlRet, strQuery, "SQLGetData" );
		SQLFreeHandle(SQL_HANDLE_STMT, m_hStmtSelect );
		return false;
	}

	SQLFreeHandle(SQL_HANDLE_STMT, m_hStmtSelect );

	if ( m_bSaveDBLog )
		controlProcessLog.Write( "%s Success", strQuery );

	return true;
}

bool	CRFControlDatabase::Select_UserCountInfo( char* szStartDate, char* szEndDate, void* pUserCountData )
{
	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256];
	int			nRowCount = 0;

	_user_count_result_cocl* userCountDate = (_user_count_result_cocl*)pUserCountData;
	
	sqlRet = SQLAllocHandle( SQL_HANDLE_STMT, m_hDbc, &m_hStmtSelect );
	if (sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO)
	{
		controlErrorLog.Write( "SQLAllocHandle Fail" );
		return false;
	}

	wsprintf( strQuery, "{ CALL pSelect_UserCountInfo('%s', '%s') }", szStartDate, szEndDate);

	if ( m_bSaveDBLog )
		controlProcessLog.Write( strQuery );

	
	sqlRet = SQLExecDirect( m_hStmtSelect, ( SQLCHAR *)strQuery, SQL_NTS);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		ErrorAction( sqlRet, strQuery, "SQLExecDirect" );
		SQLFreeHandle( SQL_HANDLE_STMT, m_hStmtSelect );
		return false;
	}

	while(TRUE) 
	{
		sqlRet = SQLFetch(m_hStmtSelect);

		if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) break;

		sqlRet  = SQLGetData( m_hStmtSelect, 1, SQL_C_ULONG, &userCountDate->UserCount[nRowCount].wYear, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 2, SQL_C_ULONG, &userCountDate->UserCount[nRowCount].byMonth, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 3, SQL_C_ULONG, &userCountDate->UserCount[nRowCount].byDay, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 4, SQL_C_ULONG, &userCountDate->UserCount[nRowCount].byHour, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 5, SQL_C_ULONG, &userCountDate->UserCount[nRowCount].dwAvgCount, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 6, SQL_C_ULONG, &userCountDate->UserCount[nRowCount].dwMaxCount, 0, &size);

		nRowCount++;

		if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) break;

	}

	userCountDate->wRowCount = nRowCount;

	SQLFreeHandle(SQL_HANDLE_STMT, m_hStmtSelect );

	if ( m_bSaveDBLog )
		controlProcessLog.Write( "%s Success", strQuery );

	return true;
}


bool	CRFControlDatabase::Select_CharacterInfo( DWORD dwAccountSerial, _controldb_charater_info_data* pCharacterData )
{
	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256];
	int			nRowCount = 0;

	sqlRet = SQLAllocHandle( SQL_HANDLE_STMT, m_hDbc, &m_hStmtSelect );
	if (sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO)
	{
		controlErrorLog.Write( "SQLAllocHandle Fail" );
		return false;
	}

	wsprintf( strQuery, "SELECT Name, Slot, Race, Lv FROM tbl_base WHERE AccountSerial = %d AND DCK = 0", dwAccountSerial);

	if ( m_bSaveDBLog )
		controlProcessLog.Write( strQuery );

	
	sqlRet = SQLExecDirect( m_hStmtSelect, ( SQLCHAR *)strQuery, SQL_NTS);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		ErrorAction( sqlRet, strQuery, "SQLExecDirect" );
		SQLFreeHandle( SQL_HANDLE_STMT, m_hStmtSelect );
		return false;
	}

	while(TRUE) 
	{
		sqlRet = SQLFetch(m_hStmtSelect);

		if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) break;

		sqlRet  = SQLGetData( m_hStmtSelect, 1, SQL_C_CHAR, &pCharacterData->CharacterInfo[nRowCount].szAvatorName, 16, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 2, SQL_C_ULONG, &pCharacterData->CharacterInfo[nRowCount].bySlotIndex, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 3, SQL_C_ULONG, &pCharacterData->CharacterInfo[nRowCount].byRaceCode, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 4, SQL_C_ULONG, &pCharacterData->CharacterInfo[nRowCount].byLevel, 0, &size);

		nRowCount++;

		if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) break;

	}

	pCharacterData->wCharacterCount = nRowCount;

	SQLFreeHandle(SQL_HANDLE_STMT, m_hStmtSelect );

	if ( m_bSaveDBLog )
		controlProcessLog.Write( "%s Success", strQuery );

	return true;
}

bool	CRFControlDatabase::Select_CharacterSerial( char* szCharacterName, DWORD* pSerial )
{

	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256];
	int			nRowCount = 0;

	sqlRet = SQLAllocHandle( SQL_HANDLE_STMT, m_hDbc, &m_hStmtSelect );
	if (sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO)
	{
		controlErrorLog.Write( "SQLAllocHandle Fail" );
		return false;
	}

	wsprintf( strQuery, "SELECT serial FROM tbl_base WHERE Name = '%s'", szCharacterName);

	if ( m_bSaveDBLog )
		controlProcessLog.Write( strQuery );

	
	sqlRet = SQLExecDirect( m_hStmtSelect, ( SQLCHAR *)strQuery, SQL_NTS);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		ErrorAction( sqlRet, strQuery, "SQLExecDirect" );
		SQLFreeHandle( SQL_HANDLE_STMT, m_hStmtSelect );
		return false;
	}

	sqlRet = SQLFetch(m_hStmtSelect);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		ErrorAction( sqlRet, strQuery, "SQLFetch" );
		SQLFreeHandle( SQL_HANDLE_STMT, m_hStmtSelect );
		return false;
	}

	sqlRet  = SQLGetData( m_hStmtSelect, 1, SQL_C_ULONG, pSerial, 0, &size);

	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		ErrorAction( sqlRet, strQuery, "SQLGetData" );
		SQLFreeHandle(SQL_HANDLE_STMT, m_hStmtSelect );
		return false;
	}

	SQLFreeHandle(SQL_HANDLE_STMT, m_hStmtSelect );

	return true;


}