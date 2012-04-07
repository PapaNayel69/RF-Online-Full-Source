
#if !defined(AFX_RFNEWDATABASE_H__97A3E79F_E7A9_42B6_9EC2_A6E47F9409CB__INCLUDED_)
#define AFX_RFNEWDATABASE_H__97A3E79F_E7A9_42B6_9EC2_A6E47F9409CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if			!defined( _WINDOWS_ )
#include	<windows.h>
#endif

#if			!defined( __SQL)
#include	<sql.h>
#endif

#if			!defined( __SQLEXT)
#include	<sqlext.h>
#endif

#define db_result_sql_error		0
#define db_result_sql_success	1
#define db_result_no_data		2


/// DB작업을 담당하는 클래스
class CRFNewDatabase  
{
protected :
	/// SQL 환경변수
	SQLHENV		m_hEnv;
	
	/// Connection 변수
	SQLHDBC		m_hDbc;

	/// Select 변수 
	SQLHSTMT	m_hStmtSelect;

	/// Update 변수 
	SQLHSTMT	m_hStmtUpdate;

	/// 커넥션이 살아있는지 확인하기 위한 변수
	bool		m_bConectionActive;
	
	/// DB Log를 저장할것인지(true : 저장)
	bool		m_bSaveDBLog;
	
	char		m_szOdbcName[32];
	char		m_szAccountName[32];
	char		m_szPassword[32];

	void		ErrorAction( SQLRETURN sqlRet, char* strQuery, char* strKind, SQLHSTMT	SQLStmt );

public :
	CRFNewDatabase();
	virtual ~CRFNewDatabase();

	/**	
	Todo : 관련변수 초기화 및 DB 커넥트 <br>
	Returns : 성공시 true, 실패시 false
	*/
	bool	StartDataBase( char* odbcName, char* accountName, char* passWord );

	/**	
	Todo : 변수 반환 및 DB 디스커넥트	 <br>
	Returns : 없음
	*/
	void	EndDataBase();

	/**	
	Todo : 디스커넥트 & 커넥트  <br>
	Returns : 성공시 true, 실패시 false
	*/
	bool	ReConnectDataBase();

	/**	
	Todo : m_bSaveDBLog변수 Setting	 <br>
	Returns : 없음
	*/
	void	SetDBLog(bool log) { m_bSaveDBLog = log; };

	/**	
	Todo : Update, Insert 쿼리실행	 <br>
	Returns : 성공시 true, 실패시 false
	*/
	bool	ExecUpdateQuery( char* strQuery );

	/**
	Todo : Commit 모드 전환 
	Parameter : bAutoCommit = true(자동Commit모드), false(수동Commit모드)
	*/
	void	SetAutoCommitMode( bool bAutoCommit );

	/**
	Todo : Transaction을 commit한다
	*/
	bool	CommitTransaction();

	/**
	Todo : Tracsaction을 Rollback 시킨다
	*/
	bool	RollbackTransaction();

	/**
	Todo : Select관련 SQL_HANDLE_STMT 할당
	*/
	bool	AllocSelectHandle();

	/**
	Todo : Update관련 SQL_HANDLE_STMT 할당
	*/
	bool	AllocUpdateHandle();

	/**
	Todo : Select Handle 해제
	*/
	bool	FreeSelectHandle();

	/**
	Todo : Update Handle 해제
	*/
	bool	FreeUpdateHandle();

};

#endif // !defined(AFX_RFNEWDATABASE_H__97A3E79F_E7A9_42B6_9EC2_A6E47F9409CB__INCLUDED_)
