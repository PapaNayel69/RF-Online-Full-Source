#include "stdafx.h"
#include "NetUtil.h"
#include "rfnewdatabase.h"

CLogFile	processLog;
CLogFile	errorLog;

CRFNewDatabase::CRFNewDatabase()
{
	memset( m_szOdbcName, 0, sizeof(m_szOdbcName) );
	memset( m_szAccountName, 0, sizeof(m_szAccountName) );
	memset( m_szPassword, 0, sizeof(m_szPassword) );

	if(!processLog.m_bInit)
		processLog.SetWriteLogFile(".\\DBLog\\DBProcess.log", true, true);
	if(!errorLog.m_bInit)
		errorLog.SetWriteLogFile(".\\DBLog\\DBError.log", true, true);

	m_bSaveDBLog = true;
}

CRFNewDatabase::~CRFNewDatabase()
{
	EndDataBase();
}

bool	CRFNewDatabase::StartDataBase(char* odbcName, char* accountName, char* passWord)
{
	SQLRETURN	sqlRet;

	if ( m_bSaveDBLog )
		processLog.WriteString( "StartDatabase");

	m_bConectionActive = false;

	if ( SQLAllocHandle ( SQL_HANDLE_ENV , SQL_NULL_HANDLE, &m_hEnv) != SQL_SUCCESS) 
	{
		SQLFreeHandle( SQL_HANDLE_ENV, m_hEnv );
		return false;
	}

	if ( SQLSetEnvAttr(m_hEnv , SQL_ATTR_ODBC_VERSION , (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER) != SQL_SUCCESS) 
	{
		SQLFreeHandle( SQL_HANDLE_ENV, m_hEnv );
		return false;
	}

	if ( SQLAllocHandle( SQL_HANDLE_DBC, m_hEnv, &m_hDbc) ) 
	{
		SQLFreeHandle( SQL_HANDLE_ENV, m_hEnv );
		SQLFreeHandle( SQL_HANDLE_DBC, m_hDbc );
		return false;
	}

	memcpy( m_szOdbcName, odbcName, strlen(odbcName) );
	memcpy( m_szAccountName, accountName, strlen(accountName) );
	memcpy( m_szPassword, passWord, strlen(passWord) );

	SQLSetConnectAttr( m_hDbc, SQL_LOGIN_TIMEOUT, (void*)3, 0 );
	
	sqlRet = SQLConnect( m_hDbc ,(SQLCHAR *)odbcName, SQL_NTS, (SQLCHAR *)accountName, SQL_NTS, (SQLCHAR *)passWord, SQL_NTS);

	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO )
	{
		errorLog.WriteString( "StartDataBase Fail" );
		SQLFreeHandle( SQL_HANDLE_ENV, m_hEnv );
		SQLFreeHandle( SQL_HANDLE_DBC, m_hDbc );
		return false;
	}

	if ( AllocSelectHandle() == false )
	{
		errorLog.WriteString( "StartDataBase Fail" );
		SQLFreeHandle( SQL_HANDLE_ENV, m_hEnv );
		SQLFreeHandle( SQL_HANDLE_DBC, m_hDbc );
		return false;
	}

	if ( AllocUpdateHandle() == false )
	{
		errorLog.WriteString( "StartDataBase Fail" );
		SQLFreeHandle( SQL_HANDLE_ENV, m_hEnv );
		SQLFreeHandle( SQL_HANDLE_DBC, m_hDbc );
		return false;
	}

	m_bConectionActive = true;
	

	if ( m_bSaveDBLog )
		processLog.WriteString( "StartDataBase Success" );

	return true;
}

void	CRFNewDatabase::EndDataBase()
{
	if ( m_bConectionActive )
	{
		FreeSelectHandle();
		FreeUpdateHandle();
		SQLDisconnect( m_hDbc );
		SQLFreeHandle( SQL_HANDLE_ENV, m_hEnv );
		SQLFreeHandle( SQL_HANDLE_DBC, m_hDbc );
	}
	
	m_bConectionActive = false;

	if ( m_bSaveDBLog )
		processLog.WriteString( "EndDatabase" );
}

bool	CRFNewDatabase::ReConnectDataBase()
{
	EndDataBase();
	if ( StartDataBase(m_szOdbcName, m_szAccountName, m_szPassword) )
	{
		if ( m_bSaveDBLog )
			processLog.WriteString("ReConnectDataBase Success.");

		return true;
	}
	else
	{
		errorLog.WriteString( "ReConnectDataBase Fail." );
		return false;
	}
}

void	CRFNewDatabase::ErrorAction( SQLRETURN sqlRet, char* strQuery, char* strKind, SQLHSTMT	SQLStmt )
{
	errorLog.Write( "Query : %s", strQuery );

	SQLCHAR       SqlState[6], Msg[SQL_MAX_MESSAGE_LENGTH];
	SQLINTEGER    NativeError;
	SQLSMALLINT   MsgLen;

	SQLGetDiagRec( SQL_HANDLE_STMT, SQLStmt, 1, SqlState, &NativeError, Msg, sizeof(Msg), &MsgLen );

	errorLog.Write( "SqlState = %s, Msg = %s ", SqlState, Msg );

	switch( sqlRet )
	{
	case SQL_NEED_DATA :
		errorLog.Write( "%s Returns : SQL_NEED_DATA", strKind );
		break;

	case SQL_STILL_EXECUTING :
		errorLog.Write( "%s Returns : SQL_STILL_EXECUTING", strKind );
		break;

	case SQL_ERROR :
		errorLog.Write( "%s Returns : SQL_ERROR", strKind );
		ReConnectDataBase();
		break;

	case SQL_NO_DATA :
		errorLog.Write( "%s Returns : SQL_NO_DATA", strKind );
		break;

	case SQL_INVALID_HANDLE :
		errorLog.Write( "%s Returns : SQL_INVALID_HANDLE", strKind );
		ReConnectDataBase();
		break;

	default :
		errorLog.Write( "%s Returns : sqlRet = %d", strKind, sqlRet );
		break;
	}
}

bool	CRFNewDatabase::ExecUpdateQuery( char* strQuery )
{
	SQLRETURN	sqlRet;

	if ( m_bSaveDBLog )
		processLog.Write( "Query : %s", strQuery );

	if ( m_hStmtUpdate == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			errorLog.Write( "ReConnectDataBase Fail. Query : %s", strQuery );
			return false;
		}
	}

	sqlRet = SQLExecDirect(m_hStmtUpdate, (SQLCHAR *)strQuery, SQL_NTS);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO ) 
	{
		ErrorAction( sqlRet, strQuery, "SQLExecDirect", m_hStmtUpdate );
		return false;
	}

	if ( m_bSaveDBLog )
		processLog.WriteString("Query Success");

	return true;

}

void	CRFNewDatabase::SetAutoCommitMode( bool bAutoCommit )
{
	if ( bAutoCommit )
		SQLSetConnectAttr( m_hDbc, SQL_ATTR_AUTOCOMMIT, (void*)SQL_AUTOCOMMIT_ON, 0 );
	else
		SQLSetConnectAttr( m_hDbc, SQL_ATTR_AUTOCOMMIT, (void*)SQL_AUTOCOMMIT_OFF, 0 );
}

bool	CRFNewDatabase::CommitTransaction()
{
	SQLRETURN sqlRet = SQLEndTran( SQL_HANDLE_DBC, m_hDbc, SQL_COMMIT );

	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO ) 
		return false;
	else
		return true;
}

bool	CRFNewDatabase::RollbackTransaction()
{
	SQLRETURN sqlRet = SQLEndTran( SQL_HANDLE_DBC, m_hDbc, SQL_ROLLBACK );

	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO ) 
		return false;
	else
		return true;

}

bool	CRFNewDatabase::AllocSelectHandle()
{
	SQLRETURN sqlRet;

	sqlRet = SQLAllocHandle( SQL_HANDLE_STMT, m_hDbc, &m_hStmtSelect );
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		errorLog.WriteString( "SELECT SQLAllocHandle Fail" );
		return false;
	}
	return true;
}

bool	CRFNewDatabase::AllocUpdateHandle()
{
	SQLRETURN sqlRet;

	sqlRet = SQLAllocHandle( SQL_HANDLE_STMT, m_hDbc, &m_hStmtUpdate );
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		errorLog.WriteString( "UPDATE SQLAllocHandle Fail" );
		return false;
	}
	return true;
}

bool	CRFNewDatabase::FreeSelectHandle()
{
	SQLRETURN sqlRet;

	if ( m_hStmtSelect == SQL_NULL_HSTMT )
		return true;

	sqlRet = SQLFreeHandle( SQL_HANDLE_STMT, m_hStmtSelect );
	m_hStmtSelect = SQL_NULL_HSTMT;
	if ( sqlRet != SQL_SUCCESS ) 
	{
		errorLog.WriteString( "SELECT SQLFreeHandle Fail" );
		return false;
	}
	return true;
}

bool	CRFNewDatabase::FreeUpdateHandle()
{
	SQLRETURN sqlRet;

	if ( m_hStmtUpdate == SQL_NULL_HSTMT )
		return true;

	sqlRet = SQLFreeHandle( SQL_HANDLE_STMT, m_hStmtUpdate );
	m_hStmtUpdate = SQL_NULL_HSTMT;
	if ( sqlRet != SQL_SUCCESS ) 
	{
		errorLog.WriteString( "UPDATE SQLFreeHandle Fail" );
		return false;
	}
	return true;
}