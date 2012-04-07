
#include "NetUtil.h"
#include "RFZoneDatabase.h"

CLogFile	zoneProcessLog;
CLogFile	zoneErrorLog;

CRFZoneDatabase::CRFZoneDatabase()
{
	zoneProcessLog.SetWriteLogFile(".\\DBLog\\ZoneDBProcess.log", true, true);
	zoneErrorLog.SetWriteLogFile(".\\DBLog\\ZoneDBError.log", true, true);
}

CRFZoneDatabase::~CRFZoneDatabase()
{
	EndDataBase();
}

bool	CRFZoneDatabase::Select_RegedCharacterInfo( DWORD dwAccountSerial, _REGED* OUT pRegedList )
{
//	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		szQuery[256];
	int			nRowCount = 0;

	sqlRet = SQLAllocHandle( SQL_HANDLE_STMT, m_hDbc, &m_hStmtSelect );
	if (sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO)
	{
		zoneErrorLog.Write( "SQLAllocHandle Fail" );
		return false;
	}

	wsprintf( szQuery, "{ CALL pSelect_RegedCharacterInfo( %d ) }", dwAccountSerial );

	if ( m_bSaveDBLog )
		zoneProcessLog.Write( szQuery );

	
	sqlRet = SQLExecDirect( m_hStmtSelect, ( SQLCHAR *)szQuery, SQL_NTS);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		ErrorAction( sqlRet, szQuery, "SQLExecDirect" );
		SQLFreeHandle( SQL_HANDLE_STMT, m_hStmtSelect );
		return false;
	}



	return true;
}
