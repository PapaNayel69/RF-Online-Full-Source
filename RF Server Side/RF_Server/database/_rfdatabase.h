// RFDatabase.h: interface for the CRFDatabase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RFDATABASE_H__E558BC82_A7E9_4D67_B6BC_FF7E567F6F3E__INCLUDED_)
#define AFX_RFDATABASE_H__E558BC82_A7E9_4D67_B6BC_FF7E567F6F3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4146)	// ado 상수 경고 무시..

#import "C:\Program Files\Common Files\System\ADO\msado15.dll" no_namespace rename("EOF", "EndOfFile")

#define TESTHR(x) if FAILED(x) _com_issue_error(hr)
#include "icrsint.h"
#include <winnt.h>
#include <ole2.h>

char* __CVC(char* pStr);

enum { DB_USERID_COL = 0, DB_PASSWD_COL, DB_CATALOG_COL, DB_DBNAME_COL };

// connection string header..
static const char* szConnectionHeader = "Provider = SQLOLEDB.1;Persist Security Info = False;";

class CRFDatabase  
{
protected:
	// ADO..
	_ConnectionPtr    m_pConnection;			// ado connection interface pointer
	_CommandPtr		  m_pCommand;				// ado command interface pointer
	_RecordsetPtr	  m_pRecordSet;				// ado recordset interface pointer
	_ParameterPtr	  m_pParameter;				// ado parameter interface pointer

public:
	enum	{ CONNECTION_STR_SIZE = 200,		// connection string length
		      USERID_SIZE	      = 20,			// user id length
			  PASSWD_SIZE	      = 20,			// password length
			  MACHINE_SIZE		  = 20,			// machine length
			  DBNAME_SIZE	      = 64 };		// database name length

	char	m_szConn[ CONNECTION_STR_SIZE ];	// db connection string

	char	m_szUserID [ USERID_SIZE ];			// db 사용자 ID
	char	m_szPasswd [ PASSWD_SIZE ];			// db 패스워드
	char	m_szMachine[ MACHINE_SIZE ];		// db machine name
	char	m_szDBName [ DBNAME_SIZE ];			// db name

	enum	{ ERRMSG_SIZE = 1024 };				// error msg length
	char    m_szErrMsgCom     [ ERRMSG_SIZE ];	// com error msg
	char    m_szErrMsgProvider[ ERRMSG_SIZE ];	// provider error msg

	const char* GetUserID	( );
	const char* GetPasswd	( );
	const char* GetMachine	( );
	const char* GetDBName   ( );

	const _RecordsetPtr GetRecordSet()
	{
		return ( const _RecordsetPtr ) m_pRecordSet;
	}

	const _CommandPtr GetCommand()
	{
		return ( const _CommandPtr ) m_pCommand;
	}

	const _ParameterPtr GetParameter()
	{
		return ( const _ParameterPtr ) m_pParameter;
	}

	//--------------------------------------------------------------------------------------
	// setting Connection String..
	BOOL SetConnectionStr( const char* UserID, const char* Passwd, const char* Machine, const char* DBName );
	BOOL SetConnectionStr( const char* Conn );

	//--------------------------------------------------------------------------------------
	// open Database..
	BOOL Open            ( int nConnectionTimeOut = 30 );
	BOOL Open            ( const char* szConnectionString, int nConnectionTimeOut = 30 );

	//--------------------------------------------------------------------------------------
	// close Database..
	void Close			 ();
	
	//--------------------------------------------------------------------------------------
	// for select..
	BOOL QuerySql		 ( char* szSql, CADORecordBinding* pUserData );
	BOOL QuerySql		 ( char* szSql, int nTimeOut = 1 );
	BOOL QuerySqlRsCount ( char* szSql, int nTimeOut = 1 );
	BOOL QueryTable		 ( char* szTableName, CADORecordBinding* pUserData );
	 
	//--------------------------------------------------------------------------------------
	// update, insert, delete..
	BOOL ExecuteSQL		 ( char* szSql, int nCommandTimeOut = 1 );
	BOOL ExecuteProc	 ( char* szProcName, int nCommandTimeOut = 1 );

	//--------------------------------------------------------------------------------------
	// adding Parameter Object..
	void AddParameter    ( char* szVarName, DataTypeEnum nParamType, long nParamSize, ParameterDirectionEnum nDirection, VARIANT vtValue );

	//--------------------------------------------------------------------------------------
	// deleting Parameter Object..
	void DeleteParameter ( char* szVarName );

	//--------------------------------------------------------------------------------------
	// transaction..
	enum { BEGIN = 0, COMMIT, ROLLBACK };
	BOOL Trans( int nCmd );

	//--------------------------------------------------------------------------------------
	// confirm db opened..
	BOOL IsOpen          ();
	
	//--------------------------------------------------------------------------------------
	// Anal field value of each type and return value;
	int   AnalFieldInt   ( ULONG ulStatus, int iValue );
	UINT  AnalFieldUINT  ( ULONG ulStatus, UINT iValue );
	LONG  AnalFieldLong  ( ULONG ulStatus, LONG iValue );
	char  AnalFieldChar  ( ULONG ulStatus, char szValue );
	char* AnalFieldString( ULONG ulStatus, char* szString );
	BYTE  AnalFieldBYTE  ( ULONG ulStatus, BYTE szValue );

	//--------------------------------------------------------------------------------------
	// Fill Validate value each data type..
	void  GetFieldInt	 ( int&  nDest,  ULONG ulStatus, int iValue );
	void  GetFieldUINT   ( UINT& nDest,  ULONG ulStatus, UINT iValue );
	void  GetFieldLong   ( LONG& nDest,  ULONG ulStatus, LONG iValue );
	void  GetFieldChar   ( char& nDest,  ULONG ulStatus, char cChar );
	BOOL  GetFieldString ( char* szDest, ULONG ulStatus, char* szValue, int iDestSize );
	void  GetFieldBYTE   ( BYTE& cDest,  ULONG ulStatus, BYTE cValue );

	//--------------------------------------------------------------------------------------
	// error handler..
	void ErrorCom		     ( _com_error &e );
	void ErrorProvider		 ( _ConnectionPtr pConnection );
	const char* GetLastErrMsg();
	void QueryErrorHandler   ( _com_error &e );
	
	//--------------------------------------------------------------------------------------
	// constructor, destructor
	CRFDatabase();
	virtual ~CRFDatabase();
};

#endif // !defined(AFX_RFDATABASE_H__E558BC82_A7E9_4D67_B6BC_FF7E567F6F3E__INCLUDED_)
