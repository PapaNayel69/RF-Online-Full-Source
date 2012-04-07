#include "stdafx.h"
#include "NetUtil.h"
#include "RFWorldDatabase.h"

CLogFile	worldProcessLog;
CLogFile	worldErrorLog;

CRFWorldDatabase::CRFWorldDatabase()
{
	if(!worldProcessLog.m_bInit)
		worldProcessLog.SetWriteLogFile(".\\DBLog\\WorldDBProcess.log", true, true);
	if(!worldErrorLog.m_bInit)
		worldErrorLog.SetWriteLogFile(".\\DBLog\\WorldDBError.log", true, true);
}

CRFWorldDatabase::~CRFWorldDatabase()
{
	EndDataBase();
}

bool	CRFWorldDatabase::Select_CharactersInfo( DWORD dwAccountSerial, _worlddb_character_array_info* pCharacterData )
{
	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256], szDeleteName[16];
	int			nRowCount = 0;

	wsprintf( strQuery, "{ CALL pSelect_CharactersInfo( %d ) }", dwAccountSerial);

	if ( m_bSaveDBLog )
		worldProcessLog.WriteString( strQuery );

	if ( m_hStmtSelect == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			worldErrorLog.Write( "ReConnectDataBase Fail. Query : %s", strQuery );
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
	/*
	SELECT Name, Slot, Race, Lv, Serial, DCK FROM tbl_base 
	WHERE 
	AccountSerial = @accountserial
	*/
	while(TRUE) 
	{
		sqlRet = SQLFetch(m_hStmtSelect);

		if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) break;

		memset( szDeleteName, 0, sizeof(szDeleteName) );

		sqlRet  = SQLGetData( m_hStmtSelect, 1, SQL_C_CHAR, pCharacterData->CharacterInfo[nRowCount].szAvatorName, 16, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 2, SQL_C_USHORT, &pCharacterData->CharacterInfo[nRowCount].bySlotIndex, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 3, SQL_C_USHORT, &pCharacterData->CharacterInfo[nRowCount].byRaceCode, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 4, SQL_C_USHORT, &pCharacterData->CharacterInfo[nRowCount].byLevel, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 5, SQL_C_ULONG, &pCharacterData->CharacterInfo[nRowCount].dwSerial, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 6, SQL_C_USHORT, &pCharacterData->CharacterInfo[nRowCount].byDck, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 7, SQL_C_CHAR, szDeleteName, 16, &size);

		if ( pCharacterData->CharacterInfo[nRowCount].byDck == 1 )
		{
			memcpy( pCharacterData->CharacterInfo[nRowCount].szAvatorName, szDeleteName, sizeof(szDeleteName) );
		}

		nRowCount++;

		if ( nRowCount >= 100 ) break;

		if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) break;

	}

	pCharacterData->wCharacterCount = nRowCount;

	SQLCloseCursor( m_hStmtSelect );

	if ( m_bSaveDBLog )
		worldProcessLog.Write( "%s Success", strQuery );

	return true;
}


bool	CRFWorldDatabase::Select_CharacterSerial( char* szCharacterName, DWORD* pSerial )
{
	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256];
	int			nRowCount = 0;
	
	wsprintf( strQuery, "{ CALL pSelect_CharacterSerial('%s') }", szCharacterName );

	if ( m_bSaveDBLog )
		worldProcessLog.WriteString( strQuery );

	if ( m_hStmtSelect == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			worldErrorLog.Write( "ReConnectDataBase Fail. Query : %s", strQuery );
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

	sqlRet  = SQLGetData( m_hStmtSelect, 1, SQL_C_ULONG, pSerial, 0, &size);

	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		ErrorAction( sqlRet, strQuery, "SQLGetData", m_hStmtSelect );
		return false;
	}

	SQLCloseCursor( m_hStmtSelect );

	if ( m_bSaveDBLog )
		worldProcessLog.Write( "%s Success", strQuery );

	return true;
}

BYTE	CRFWorldDatabase::Select_CharacterBaseInfoByName( char* szCharacterName, _worlddb_character_base_info* pCharacterData )
{
	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256];
	int			i = 0;

	wsprintf( strQuery, "{ CALL pSelect_CharacterBaseInfoByName('%s') }", szCharacterName );

	if ( m_bSaveDBLog )
		worldProcessLog.WriteString( strQuery );

	if ( m_hStmtSelect == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			worldErrorLog.Write( "ReConnectDataBase Fail. Query : %s", strQuery );
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

	memcpy( pCharacterData->szName, szCharacterName, strlen(szCharacterName) );
	/*
	SELECT 
	Serial, Race, Class, Slot, Lv, Dalant, 
	Gold, BaseShape, LastConnTime, Account, 
	EK0, EK1, EK2, EK3, EK4, EK5, EK6, EK7, 
	EU0, EU1, EU2, EU3, EU4, EU5, EU6, EU7
	from tbl_base where Name=@charactername and DCK = 0
	*/
	sqlRet  = SQLGetData( m_hStmtSelect, 1, SQL_C_ULONG, &pCharacterData->dwSerial, 0, &size);
	sqlRet  = SQLGetData( m_hStmtSelect, 2, SQL_C_USHORT, &pCharacterData->byRace, 0, &size);
	sqlRet  = SQLGetData( m_hStmtSelect, 3, SQL_C_CHAR, pCharacterData->szClassCode, class_code_len+1, &size);
	sqlRet  = SQLGetData( m_hStmtSelect, 4, SQL_C_USHORT, &pCharacterData->bySlotIndex, 0, &size);
	sqlRet  = SQLGetData( m_hStmtSelect, 5, SQL_C_USHORT, &pCharacterData->byLevel, 0, &size);
	sqlRet  = SQLGetData( m_hStmtSelect, 6, SQL_C_ULONG, &pCharacterData->dwDalant, 0, &size);
	sqlRet  = SQLGetData( m_hStmtSelect, 7, SQL_C_ULONG, &pCharacterData->dwGold, 0, &size);
	sqlRet  = SQLGetData( m_hStmtSelect, 8, SQL_C_ULONG, &pCharacterData->dwBaseShape, 0, &size);
	sqlRet  = SQLGetData( m_hStmtSelect, 9, SQL_C_ULONG, &pCharacterData->dwLastConnTime, 0, &size);
	sqlRet  = SQLGetData( m_hStmtSelect, 10, SQL_C_CHAR, pCharacterData->szAccount, max_name_len+1, &size);

	for ( i = 0; i<equip_fix_num; i++)
	{
		sqlRet  = SQLGetData( m_hStmtSelect, i+11, SQL_C_SSHORT, &pCharacterData->shEKArray[i], 0, &size);
	}
	for ( i = 0; i<equip_fix_num; i++)
	{
		sqlRet  = SQLGetData( m_hStmtSelect, i+19, SQL_C_ULONG, &pCharacterData->dwEUArray[i], 0, &size);
	}

	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		ErrorAction( sqlRet, strQuery, "SQLGetData", m_hStmtSelect );
		return db_result_sql_error;
	}

	SQLCloseCursor( m_hStmtSelect );

	if ( m_bSaveDBLog )
		worldProcessLog.Write( "%s Success", strQuery );

	return db_result_sql_success;
}

BYTE	CRFWorldDatabase::Select_CharacterGeneralInfo( DWORD dwCharacterSerial, _worlddb_character_general_info* pCharacterData )
{
	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256];
	int			i = 0, nGetDataCounter;
	
	wsprintf( strQuery, "{ CALL pSelect_CharacterGeneralInfo( %d ) }", dwCharacterSerial );

	if ( m_bSaveDBLog )
		worldProcessLog.WriteString( strQuery );

	if ( m_hStmtSelect == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			worldErrorLog.Write( "ReConnectDataBase Fail. Query : %s", strQuery );
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
	/*
	SELECT 
	HP, FP, SP, Exp, BagNum, Map, X, Y, Z, TotalPlayMin, LeftResList, 
	PK0, PK1, PK2, PK3, 
	LK0 ~ LK7,
	EK0, EK1, EK2, EK3, EK4, EK5,
	ED0, ED1, ED2, ED3, ED4, ED5,
	F0 ~ F87, 
	WM0, WM1,
	FM0 ~ FM23,
	SM0 ~ SM47,
	MI0, MI1, MI2,
	SR, DM, PM,
	AK0, AK1, AK2, AK3,
	AD0, AD1, AD2, AD3,
	AP0, AP1, AP2, AP3,
	Class0, Class1, Class2
	from tbl_general where serial=@characterSerial and DCK = 0
	*/

	nGetDataCounter = 0;

	nGetDataCounter++;
	sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_ULONG, &pCharacterData->dwHP, 0, &size);
	nGetDataCounter++;
	sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_ULONG, &pCharacterData->dwFP, 0, &size);
	nGetDataCounter++;
	sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_ULONG, &pCharacterData->dwSP, 0, &size);
	nGetDataCounter++;
	sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_DOUBLE, &pCharacterData->dExp, 0, &size);
	nGetDataCounter++;
	sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_SHORT, &pCharacterData->byBagNum, 0, &size);
	nGetDataCounter++;
	sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_SHORT, &pCharacterData->byMapCode, 0, &size);
	nGetDataCounter++;
	sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_FLOAT, &pCharacterData->fStartPos[0], 0, &size);
	nGetDataCounter++;
	sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_FLOAT, &pCharacterData->fStartPos[1], 0, &size);
	nGetDataCounter++;
	sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_FLOAT, &pCharacterData->fStartPos[2], 0, &size);
	nGetDataCounter++;
	sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_ULONG, &pCharacterData->dwTotalPlayMin, 0, &size);
	nGetDataCounter++;
	sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_CHAR, pCharacterData->szLeftResList, 160, &size);

	for ( i = 0; i<potion_belt_num; i++)
	{
		nGetDataCounter++;
		sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_LONG, &pCharacterData->lPK[i], 0, &size);
	}

	for ( i = 0; i<sf_linker_num; i++)
	{
		nGetDataCounter++;
		sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_SHORT, &pCharacterData->shLK[i], 0, &size);
	}

	for ( i = 0; i<embellish_fix_num; i++)
	{
		nGetDataCounter++;
		sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_LONG, &pCharacterData->lEK[i], 0, &size);
	}

	for ( i = 0; i<embellish_fix_num; i++)
	{
		nGetDataCounter++;
		sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_SHORT, &pCharacterData->shED[i], 0, &size);
	}

	for ( i = 0; i<force_storage_num; i++)
	{
		nGetDataCounter++;
		sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_LONG, &pCharacterData->lF[i], 0, &size);
	}

	for ( i = 0; i<WP_CLASS_NUM; i++)
	{
		nGetDataCounter++;
		sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_ULONG, &pCharacterData->dwWM[i], 0, &size);
	}

	for ( i = 0; i<force_mastery_num; i++)
	{
		nGetDataCounter++;
		sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_ULONG, &pCharacterData->dwFM[i], 0, &size);
	}

	for ( i = 0; i<max_skill_num; i++)
	{
		nGetDataCounter++;
		sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_ULONG, &pCharacterData->dwSM[i], 0, &size);
	}

	for ( i = 0; i<MI_CLASS_NUM; i++)
	{
		nGetDataCounter++;
		sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_ULONG, &pCharacterData->dwMI[i], 0, &size);
	}

	nGetDataCounter++;
	sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_ULONG, &pCharacterData->dwSR, 0, &size);
	nGetDataCounter++;
	sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_ULONG, &pCharacterData->dwDM, 0, &size);
	nGetDataCounter++;
	sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_ULONG, &pCharacterData->dwPM, 0, &size);

	for ( i = 0; i<animus_storage_num; i++)
	{
		nGetDataCounter++;
		sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_ULONG, &pCharacterData->dwAK[i], 0, &size);
	}

	for ( i = 0; i<animus_storage_num; i++)
	{
		nGetDataCounter++;
		sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_ULONG, &pCharacterData->dwAD[i], 0, &size);
	}

	for ( i = 0; i<animus_storage_num; i++)
	{
		nGetDataCounter++;
		sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_ULONG, &pCharacterData->dwAP[i], 0, &size);
	}

	for ( i = 0; i<class_history_num; i++)
	{
		nGetDataCounter++;
		sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_SHORT, &pCharacterData->zClassHistory[i], 0, &size);
	}


	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		ErrorAction( sqlRet, strQuery, "SQLGetData", m_hStmtSelect );
		return db_result_sql_error;
	}

	SQLCloseCursor( m_hStmtSelect );

	if ( m_bSaveDBLog )
		worldProcessLog.Write( "%s Success", strQuery );

	return db_result_sql_success;
}


BYTE	CRFWorldDatabase::Select_Inven( DWORD dwCharacterSerial, WORD wBagCount, _worlddb_inven_info* pInvenData )
{

	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256];
	int			i = 0, nGetDataCounter = 0;
	
	wsprintf( strQuery, "{ CALL pSelect_inven_%d( %d ) }", wBagCount, dwCharacterSerial );

	if ( m_bSaveDBLog )
		worldProcessLog.WriteString( strQuery );

	if ( m_hStmtSelect == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			worldErrorLog.Write( "ReConnectDataBase Fail. Query : %s", strQuery );
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

	for ( i=0; i<max_inven_item_count; i++ )
	{
		nGetDataCounter++;
		sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_LONG, &pInvenData->lK[i], 0, &size);
		if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) break;

		nGetDataCounter++;
		sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_ULONG, &pInvenData->dwD[i], 0, &size);
		nGetDataCounter++;
		sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_ULONG, &pInvenData->dwU[i], 0, &size);
	}

	SQLCloseCursor( m_hStmtSelect );

	if ( m_bSaveDBLog )
		worldProcessLog.Write( "%s Success", strQuery );

	return db_result_sql_success;
}


BYTE	CRFWorldDatabase::Select_CharacterBaseInfoBySerial( DWORD dwAccountSerial, _worlddb_character_base_info_array* pCharacterDataArray )
{
	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256];
	int			nRowCount = 0, i = 0;

	wsprintf( strQuery, "{ CALL pSelect_CharacterBaseInfoBySerial( %d ) }", dwAccountSerial );

	if ( m_bSaveDBLog )
		worldProcessLog.WriteString( strQuery );

	if ( m_hStmtSelect == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			worldErrorLog.Write( "ReConnectDataBase Fail. Query : %s", strQuery );
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

	/*
	SELECT 
	Serial, Name, Slot, Race, Class, Lv, 
	Dalant, Gold, BaseShape, LastConnTime
	EK0, EK1, EK2, EK3, EK4, EK5, EK6, EK7, 
	EU0, EU1, EU2, EU3, EU4, EU5, EU6, EU7
	from tbl_base where AccountSerial=@accountserial and DCK = 0
	*/

	while(TRUE) 
	{
		sqlRet = SQLFetch(m_hStmtSelect);

		if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) break;
	
		sqlRet  = SQLGetData( m_hStmtSelect, 1, SQL_C_ULONG, &pCharacterDataArray->CharacterInfo[nRowCount].dwSerial, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 2, SQL_C_CHAR, pCharacterDataArray->CharacterInfo[nRowCount].szName, max_name_len+1, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 3, SQL_C_USHORT, &pCharacterDataArray->CharacterInfo[nRowCount].bySlotIndex, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 4, SQL_C_USHORT, &pCharacterDataArray->CharacterInfo[nRowCount].byRace, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 5, SQL_C_CHAR, pCharacterDataArray->CharacterInfo[nRowCount].szClassCode, class_code_len+1, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 6, SQL_C_USHORT, &pCharacterDataArray->CharacterInfo[nRowCount].byLevel, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 7, SQL_C_ULONG, &pCharacterDataArray->CharacterInfo[nRowCount].dwDalant, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 8, SQL_C_ULONG, &pCharacterDataArray->CharacterInfo[nRowCount].dwGold, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 9, SQL_C_ULONG, &pCharacterDataArray->CharacterInfo[nRowCount].dwBaseShape, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 10, SQL_C_ULONG, &pCharacterDataArray->CharacterInfo[nRowCount].dwLastConnTime, 0, &size);
		
		for ( i = 0; i<equip_fix_num; i++)
		{
			sqlRet  = SQLGetData( m_hStmtSelect, i+11, SQL_C_SSHORT, &pCharacterDataArray->CharacterInfo[nRowCount].shEKArray[i], 0, &size);
		}
		for ( i = 0; i<equip_fix_num; i++)
		{
			sqlRet  = SQLGetData( m_hStmtSelect, i+19, SQL_C_ULONG, &pCharacterDataArray->CharacterInfo[nRowCount].dwEUArray[i], 0, &size);
		}

		nRowCount++;

		if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) break;

	}

	pCharacterDataArray->wCharacterCount = nRowCount;

	SQLCloseCursor( m_hStmtSelect );

	if ( m_bSaveDBLog )
		worldProcessLog.Write( "%s Success", strQuery );

	return db_result_sql_success;

}

BYTE	CRFWorldDatabase::Select_CharacterBaseInfo( DWORD dwCharacterSerial, _worlddb_character_base_info* pCharacterData )
{

	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256];
	int			i = 0;
	
	wsprintf( strQuery, "{ CALL pSelect_CharacterBaseInfo( %d ) }", dwCharacterSerial );

	if ( m_bSaveDBLog )
		worldProcessLog.WriteString( strQuery );

	if ( m_hStmtSelect == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			worldErrorLog.Write( "ReConnectDataBase Fail. Query : %s", strQuery );
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

	/*
	SELECT 
	Name, Slot, Race, Class, Lv, 
	Dalant, Gold, BaseShape, LastConnTime,
	EK0, EK1, EK2, EK3, EK4, EK5, EK6, EK7, 
	EU0, EU1, EU2, EU3, EU4, EU5, EU6, EU7
	from tbl_base where Serial=@serial and DCK = 0
	*/

	sqlRet  = SQLGetData( m_hStmtSelect, 1, SQL_C_CHAR, pCharacterData->szName, max_name_len+1, &size);
	sqlRet  = SQLGetData( m_hStmtSelect, 2, SQL_C_USHORT, &pCharacterData->bySlotIndex, 0, &size);
	sqlRet  = SQLGetData( m_hStmtSelect, 3, SQL_C_USHORT, &pCharacterData->byRace, 0, &size);
	sqlRet  = SQLGetData( m_hStmtSelect, 4, SQL_C_CHAR, pCharacterData->szClassCode, class_code_len+1, &size);
	sqlRet  = SQLGetData( m_hStmtSelect, 5, SQL_C_USHORT, &pCharacterData->byLevel, 0, &size);
	sqlRet  = SQLGetData( m_hStmtSelect, 6, SQL_C_ULONG, &pCharacterData->dwDalant, 0, &size);
	sqlRet  = SQLGetData( m_hStmtSelect, 7, SQL_C_ULONG, &pCharacterData->dwGold, 0, &size);
	sqlRet  = SQLGetData( m_hStmtSelect, 8, SQL_C_ULONG, &pCharacterData->dwBaseShape, 0, &size);
	sqlRet  = SQLGetData( m_hStmtSelect, 9, SQL_C_ULONG, &pCharacterData->dwLastConnTime, 0, &size);
	
	for ( i = 0; i<equip_fix_num; i++)
	{
		sqlRet  = SQLGetData( m_hStmtSelect, i+10, SQL_C_SSHORT, &pCharacterData->shEKArray[i], 0, &size);
	}
	for ( i = 0; i<equip_fix_num; i++)
	{
		sqlRet  = SQLGetData( m_hStmtSelect, i+18, SQL_C_ULONG, &pCharacterData->dwEUArray[i], 0, &size);
	}

	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		ErrorAction( sqlRet, strQuery, "SQLGetData", m_hStmtSelect );
		return db_result_sql_error;
	}

	SQLCloseCursor( m_hStmtSelect );

	if ( m_bSaveDBLog )
		worldProcessLog.Write( "%s Success", strQuery );

	return db_result_sql_success;

}


BYTE	CRFWorldDatabase::Select_Unit( DWORD dwCharacterSerial, _worlddb_unit_info_array* pUnitInfo )
{
	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256];
	int			i = 0, nGetDataCounter = 0;

	wsprintf( strQuery, "{ CALL pSelect_Unit( %d ) }", dwCharacterSerial );

	if ( m_bSaveDBLog )
		worldProcessLog.WriteString( strQuery );

	if ( m_hStmtSelect == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			worldErrorLog.Write( "ReConnectDataBase Fail. Query : %s", strQuery );
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
/*
	SELECT 
	F_0, F_1, F_2, F_3,
	H_0, H_1, H_2, H_3,
	U_0, U_1, U_2, U_3,
	L_0, L_1, L_2, L_3,
	A_0, A_1, A_2, A_3,
	S_0, S_1, S_2, S_3,
	B_0, B_1, B_2, B_3,
	Gg_0, Gg_1, Gg_2, Gg_3,
	AB_0, AB_1, AB_2, AB_3,
	SB_0, SB_1, SB_2, SB_3,
	KF_0, KF_1, KF_2, KF_3,
	PF_0, PF_1, PF_2, PF_3,
	Cut_0, Cut_1, Cut_2, Cut_3,
	SP0_0, SP0_1, SP0_2, SP0_3,
	SP1_0, SP1_1, SP1_2, SP1_3,
	SP2_0, SP2_1, SP2_2, SP2_3,
	SP3_0, SP3_1, SP3_2, SP3_3,
	SP4_0, SP4_1, SP4_2, SP4_3,
	SP5_0, SP5_1, SP5_2, SP5_3,
	SP5_0, SP6_1, SP6_2, SP6_3,
	SP7_0, SP7_1, SP7_2, SP7_3
	FROM tbl_unit WHERE Serial=@serial
*/

	for ( i = 0; i<unit_storage_num; i++)
	{
		nGetDataCounter++;
		sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_SHORT, &pUnitInfo->UnitInfo[i].byFrame, 0, &size);
	}

	for ( i = 0; i<unit_storage_num; i++)
	{
		nGetDataCounter++;
		sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_SHORT, &pUnitInfo->UnitInfo[i].byPart[unit_part_head], 0, &size);
	}
	for ( i = 0; i<unit_storage_num; i++)
	{
		nGetDataCounter++;
		sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_SHORT, &pUnitInfo->UnitInfo[i].byPart[unit_part_upper], 0, &size);
	}
	for ( i = 0; i<unit_storage_num; i++)
	{
		nGetDataCounter++;
		sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_SHORT, &pUnitInfo->UnitInfo[i].byPart[unit_part_lower], 0, &size);
	}
	for ( i = 0; i<unit_storage_num; i++)
	{
		nGetDataCounter++;
		sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_SHORT, &pUnitInfo->UnitInfo[i].byPart[unit_part_arms], 0, &size);
	}
	for ( i = 0; i<unit_storage_num; i++)
	{
		nGetDataCounter++;
		sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_SHORT, &pUnitInfo->UnitInfo[i].byPart[unit_part_shoulder], 0, &size);
	}
	for ( i = 0; i<unit_storage_num; i++)
	{
		nGetDataCounter++;
		sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_SHORT, &pUnitInfo->UnitInfo[i].byPart[unit_part_back], 0, &size);
	}

	for ( i = 0; i<unit_storage_num; i++)
	{
		nGetDataCounter++;
		sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_ULONG, &pUnitInfo->UnitInfo[i].dwGauge, 0, &size);
	}

	for ( i = 0; i<unit_storage_num; i++)
	{
		nGetDataCounter++;
		sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_ULONG, &pUnitInfo->UnitInfo[i].dwBullet[unit_bullet_arms], 0, &size);
	}
	for ( i = 0; i<unit_storage_num; i++)
	{
		nGetDataCounter++;
		sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_ULONG, &pUnitInfo->UnitInfo[i].dwBullet[unit_bullet_shoulder], 0, &size);
	}

	for ( i = 0; i<unit_storage_num; i++)
	{
		nGetDataCounter++;
		sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_LONG, &pUnitInfo->UnitInfo[i].nKeepingFee, 0, &size);
	}
	for ( i = 0; i<unit_storage_num; i++)
	{
		nGetDataCounter++;
		sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_LONG, &pUnitInfo->UnitInfo[i].nPullingFee, 0, &size);
	}
	for ( i = 0; i<unit_storage_num; i++)
	{
		nGetDataCounter++;
		sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_ULONG, &pUnitInfo->UnitInfo[i].dwCutTime, 0, &size);
	}

	for ( int j = 0; j<max_unit_spare; j++)
	{
		for ( i = 0; i<unit_storage_num; i++)
		{
			nGetDataCounter++;
			sqlRet  = SQLGetData( m_hStmtSelect, nGetDataCounter, SQL_C_LONG, &pUnitInfo->UnitInfo[i].dwSpare[j], 0, &size);
		}
	}

	SQLCloseCursor( m_hStmtSelect );

	if ( m_bSaveDBLog )
		worldProcessLog.Write( "%s Success", strQuery );
	

	return db_result_sql_success;
}

bool	CRFWorldDatabase::Insert_Unit( DWORD dwCharacterSerial )
{
	char strQuery[256];

	wsprintf( strQuery, "{ CALL pInsert_Unit( %d ) }", dwCharacterSerial );
	return ExecUpdateQuery( strQuery );
}

bool	CRFWorldDatabase::Select_Equal_Name( char* szCharacterName )
{
	SQLRETURN	sqlRet;
	char		strQuery[256];
		
	wsprintf( strQuery, "{ CALL pSelect_Equal_Name('%s') }", szCharacterName );

	if ( m_bSaveDBLog )
		worldProcessLog.WriteString( strQuery );

	if ( m_hStmtSelect == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			worldErrorLog.Write( "ReConnectDataBase Fail. Query : %s", strQuery );
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

	sqlRet = SQLFetch(m_hStmtSelect);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		if ( sqlRet == SQL_NO_DATA )
			return true;
		else
		{
			ErrorAction( sqlRet, strQuery, "SQLFetch", m_hStmtSelect );
			return false;
		}
	}

	SQLCloseCursor( m_hStmtSelect );

	if ( m_bSaveDBLog )
		worldProcessLog.Write( "%s Success", strQuery );

	return false;
}

bool	CRFWorldDatabase::Insert_CharacterData( char* szCharacterName, char* szClassCode, DWORD dwAccountSerial, char* szAccount, BYTE bySlotIndex, BYTE byRaceSexCode, DWORD dwBaseShape, int nMapIndex, DWORD* pDwSerial )
{
	SQLRETURN	sqlRet;
	char strQuery[256];
	wsprintf( strQuery, "{ CALL pInsert_Base( '%s', '%s', %d, '%s', %d, %d, %d ) }", 
		szCharacterName, szClassCode, dwAccountSerial, szAccount, bySlotIndex, byRaceSexCode, dwBaseShape);

	if ( m_bSaveDBLog )
		worldProcessLog.WriteString( strQuery );

	if ( m_hStmtUpdate == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			worldErrorLog.WriteString( "ReConnectDataBase Fail. Query : Insert_CharacterData" );
			return false;
		}
	}

	SetAutoCommitMode( false );

	sqlRet = SQLExecDirect(m_hStmtUpdate, (SQLCHAR *)strQuery, SQL_NTS);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO ) 
	{
		RollbackTransaction();
		SetAutoCommitMode( true );
		ErrorAction( sqlRet, strQuery, "SQLExecDirect", m_hStmtUpdate );
		return false;
	}

	if ( m_bSaveDBLog )
		worldProcessLog.Write( "%s Success", strQuery );

	if ( Select_CharacterSerial( szCharacterName, pDwSerial ) == false )
	{
		RollbackTransaction();
		SetAutoCommitMode( true );
		return false;
	}

	wsprintf( strQuery, "{ CALL pInsert_General( %d, %d ) }", *pDwSerial, nMapIndex );

	if ( m_bSaveDBLog )
		worldProcessLog.WriteString( strQuery );

	sqlRet = SQLExecDirect(m_hStmtUpdate, (SQLCHAR *)strQuery, SQL_NTS);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO ) 
	{
		RollbackTransaction();
		SetAutoCommitMode( true );
		ErrorAction( sqlRet, strQuery, "SQLExecDirect", m_hStmtUpdate );
		return false;
	}

	if ( m_bSaveDBLog )
		worldProcessLog.Write( "%s Success", strQuery );

	wsprintf( strQuery, "{ CALL pInsert_inven( %d ) }", *pDwSerial );

	if ( m_bSaveDBLog )
		worldProcessLog.WriteString( strQuery );

	sqlRet = SQLExecDirect(m_hStmtUpdate, (SQLCHAR *)strQuery, SQL_NTS);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO ) 
	{
		RollbackTransaction();
		SetAutoCommitMode( true );
		ErrorAction( sqlRet, strQuery, "SQLExecDirect", m_hStmtUpdate );
		return false;
	}

	if ( m_bSaveDBLog )
		worldProcessLog.Write( "%s Success", strQuery );

	if ( byRaceSexCode/2 == race_code_bellato )
	{
		wsprintf( strQuery, "{ CALL pInsert_Unit( %d ) }", *pDwSerial );

		if ( m_bSaveDBLog )
			worldProcessLog.WriteString( strQuery );

		sqlRet = SQLExecDirect(m_hStmtUpdate, (SQLCHAR *)strQuery, SQL_NTS);
		if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO ) 
		{
			RollbackTransaction();
			SetAutoCommitMode( true );
			ErrorAction( sqlRet, strQuery, "SQLExecDirect", m_hStmtUpdate );
			return false;
		}

		if ( m_bSaveDBLog )
			worldProcessLog.Write( "%s Success", strQuery );
	}

	CommitTransaction();
	SetAutoCommitMode( true );
	return true;
}

bool	CRFWorldDatabase::Delete_CharacterData( DWORD dwCharacterSerial )
{
	char strQuery[256];
	SQLRETURN	sqlRet;

	wsprintf( strQuery, "{ CALL pDelete_Base( %d ) }", dwCharacterSerial );

	if ( m_bSaveDBLog )
		worldProcessLog.Write( "Query : %s", strQuery );

	if ( m_hStmtUpdate == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			worldErrorLog.Write( "ReConnectDataBase Fail. Query : %s", strQuery );
			return false;
		}
	}

	SetAutoCommitMode( false );

	sqlRet = SQLExecDirect(m_hStmtUpdate, (SQLCHAR *)strQuery, SQL_NTS);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO ) 
	{
		RollbackTransaction();
		SetAutoCommitMode( true );
		ErrorAction( sqlRet, strQuery, "SQLExecDirect", m_hStmtUpdate );
		return false;
	}

	wsprintf( strQuery, "{ CALL pUpdate_DeleteName_Step1( %d ) }", dwCharacterSerial );

	sqlRet = SQLExecDirect(m_hStmtUpdate, (SQLCHAR *)strQuery, SQL_NTS);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO ) 
	{
		RollbackTransaction();
		SetAutoCommitMode( true );
		ErrorAction( sqlRet, strQuery, "SQLExecDirect", m_hStmtUpdate );
		return false;
	}

	wsprintf( strQuery, "{ CALL pUpdate_DeleteName_Step2( %d ) }", dwCharacterSerial );

	sqlRet = SQLExecDirect(m_hStmtUpdate, (SQLCHAR *)strQuery, SQL_NTS);
	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO ) 
	{
		RollbackTransaction();
		SetAutoCommitMode( true );
		ErrorAction( sqlRet, strQuery, "SQLExecDirect", m_hStmtUpdate );
		return false;
	}

	if ( m_bSaveDBLog )
		worldProcessLog.WriteString("Query Success");

	CommitTransaction();
	SetAutoCommitMode( true );

	return true;
}

bool	CRFWorldDatabase::Update_CharacterData( char* szBaseQuery, char* szGeneralQuery, char* szInvenQuery, char* szUnitQuery )
{
	SQLRETURN	sqlRet;

	SetAutoCommitMode( false );

	if ( m_bSaveDBLog )
		worldProcessLog.WriteString( "Update_CharacterData");

	if ( m_hStmtUpdate == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			worldErrorLog.WriteString( "ReConnectDataBase Fail. Query : Update_CharacterData" );
			SetAutoCommitMode( true );
			return false;
		}
	}

	if ( szBaseQuery[0] != NULL )
	{
		sqlRet = SQLExecDirect(m_hStmtUpdate, (SQLCHAR *)szBaseQuery, SQL_NTS);
		if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO ) 
		{
			RollbackTransaction();
			SetAutoCommitMode( true );
			ErrorAction( sqlRet, szBaseQuery, "SQLExecDirect", m_hStmtUpdate );
			return false;
		}
	}

	if ( szGeneralQuery[0] != NULL )
	{
		sqlRet = SQLExecDirect(m_hStmtUpdate, (SQLCHAR *)szGeneralQuery, SQL_NTS);
		if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO ) 
		{
			RollbackTransaction();
			SetAutoCommitMode( true );
			ErrorAction( sqlRet, szGeneralQuery, "SQLExecDirect", m_hStmtUpdate );
			return false;
		}
	}

	if ( szInvenQuery[0] != NULL )
	{
		sqlRet = SQLExecDirect(m_hStmtUpdate, (SQLCHAR *)szInvenQuery, SQL_NTS);
		if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO ) 
		{
			RollbackTransaction();
			SetAutoCommitMode( true );
			ErrorAction( sqlRet, szInvenQuery, "SQLExecDirect", m_hStmtUpdate );
			return false;
		}
	}

	if ( szUnitQuery[0] != NULL )
	{
		sqlRet = SQLExecDirect(m_hStmtUpdate, (SQLCHAR *)szUnitQuery, SQL_NTS);
		if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO ) 
		{
			RollbackTransaction();
			SetAutoCommitMode( true );
			ErrorAction( sqlRet, szUnitQuery, "SQLExecDirect", m_hStmtUpdate );
			return false;
		}
	}

	CommitTransaction();
	SetAutoCommitMode( true );

	if ( m_bSaveDBLog )
		worldProcessLog.WriteString( "Update_CharacterData Success");

	return true;
}

BYTE	CRFWorldDatabase::Select_Economy_History( _worlddb_economy_history_info_array* pEconomyData, DWORD dwDate )
{
	SQLINTEGER	size;
	SQLRETURN	sqlRet;
	char strQuery[256];
	int nRowCount = 0;

	wsprintf( strQuery, "{ CALL pSelect_Last_Economy_History ( %d )}", dwDate );

	if ( m_bSaveDBLog )
		worldProcessLog.WriteString( strQuery );

	if ( m_hStmtSelect == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			worldErrorLog.Write( "ReConnectDataBase Fail. Query : %s", strQuery );
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
/*
	SELECT 
	top 12 
	Serial,
	B_Dalant, C_Dalant, A_Dalant, 
	B_Gold, C_Gold, A_Gold,
	ManageValue
	FROM tbl_economy_history order by Serial desc
*/
	while(TRUE) 
	{
		sqlRet = SQLFetch(m_hStmtSelect);

		if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) break;
	
		sqlRet  = SQLGetData( m_hStmtSelect, 1, SQL_C_DOUBLE, &pEconomyData->EconomyData[nRowCount].dTradeDalant[race_code_bellato], 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 2, SQL_C_DOUBLE, &pEconomyData->EconomyData[nRowCount].dTradeDalant[race_code_cora], 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 3, SQL_C_DOUBLE, &pEconomyData->EconomyData[nRowCount].dTradeDalant[race_code_accretia], 0, &size);

		sqlRet  = SQLGetData( m_hStmtSelect, 4, SQL_C_DOUBLE, &pEconomyData->EconomyData[nRowCount].dTradeGold[race_code_bellato], 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 5, SQL_C_DOUBLE, &pEconomyData->EconomyData[nRowCount].dTradeGold[race_code_cora], 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 6, SQL_C_DOUBLE, &pEconomyData->EconomyData[nRowCount].dTradeGold[race_code_accretia], 0, &size);

		sqlRet  = SQLGetData( m_hStmtSelect, 7, SQL_C_ULONG, &pEconomyData->EconomyData[nRowCount].dwManageValue, 0, &size);

		nRowCount++;

		if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) break;
	}

	pEconomyData->wRowCount = nRowCount;

	SQLCloseCursor( m_hStmtSelect );

	if ( m_bSaveDBLog )
		worldProcessLog.Write( "%s Success", strQuery );

	return db_result_sql_success;
}

BYTE	CRFWorldDatabase::Select_Exist_Economy( DWORD dwDate, _worlddb_economy_history_info_array::_worlddb_economy_history_info* OUT pEconomyData )
{
	SQLINTEGER	size;
	SQLRETURN	sqlRet;
	char		strQuery[256];
	int			nRowCount = 0;

	wsprintf( strQuery, "{ CALL pSelect_Exist_Economy( %d ) }", dwDate );

	if ( m_bSaveDBLog )
		worldProcessLog.WriteString( strQuery );

	if ( m_hStmtSelect == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			worldErrorLog.Write( "ReConnectDataBase Fail. Query : %s", strQuery );
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
		}
		return db_result_sql_error;
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
		}
		return db_result_sql_error;
	}

	sqlRet  = SQLGetData( m_hStmtSelect, 1, SQL_C_DOUBLE, &pEconomyData->dTradeDalant[race_code_bellato], 0, &size);
	sqlRet  = SQLGetData( m_hStmtSelect, 2, SQL_C_DOUBLE, &pEconomyData->dTradeDalant[race_code_cora], 0, &size);
	sqlRet  = SQLGetData( m_hStmtSelect, 3, SQL_C_DOUBLE, &pEconomyData->dTradeDalant[race_code_accretia], 0, &size);

	sqlRet  = SQLGetData( m_hStmtSelect, 4, SQL_C_DOUBLE, &pEconomyData->dTradeGold[race_code_bellato], 0, &size);
	sqlRet  = SQLGetData( m_hStmtSelect, 5, SQL_C_DOUBLE, &pEconomyData->dTradeGold[race_code_cora], 0, &size);
	sqlRet  = SQLGetData( m_hStmtSelect, 6, SQL_C_DOUBLE, &pEconomyData->dTradeGold[race_code_accretia], 0, &size);

	sqlRet  = SQLGetData( m_hStmtSelect, 7, SQL_C_ULONG, &pEconomyData->dwManageValue, 0, &size);

	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		ErrorAction( sqlRet, strQuery, "SQLGetData", m_hStmtSelect );
		return db_result_sql_error;
	}

	SQLCloseCursor( m_hStmtSelect );

	if ( m_bSaveDBLog )
		worldProcessLog.Write( "%s Success", strQuery );

	return db_result_sql_success;

}

bool	CRFWorldDatabase::Insert_Economy_History( DWORD dwDate, double* pdTradeDalant, double* pdTradeGold, int nMgrValue )
{
	char strQuery[256] = {0,};
	_worlddb_economy_history_info_array::_worlddb_economy_history_info economyData;
	BYTE byRet = Select_Exist_Economy( dwDate, &economyData );

	if ( byRet == db_result_no_data )
	{
		sprintf( strQuery, "{ CALL pInsert_Economy_History( %d, %f, %f, %f, %f, %f, %f, %d ) }", 
			dwDate, 
			pdTradeDalant[race_code_bellato], pdTradeGold[race_code_bellato], 
			pdTradeDalant[race_code_cora], pdTradeGold[race_code_cora], 
			pdTradeDalant[race_code_accretia], pdTradeGold[race_code_accretia], 
			nMgrValue );

		return ExecUpdateQuery( strQuery );
	}
	else if ( byRet == db_result_sql_success )
	{
		sprintf( strQuery, "{ CALL pUpdate_Economy_History( %d, %f, %f, %f, %f, %f, %f, %d ) }", 
			dwDate, 
			pdTradeDalant[race_code_bellato], pdTradeGold[race_code_bellato], 
			pdTradeDalant[race_code_cora], pdTradeGold[race_code_cora], 
			pdTradeDalant[race_code_accretia], pdTradeGold[race_code_accretia], 
			nMgrValue );

		return ExecUpdateQuery( strQuery );
	}
	else
	{
		return false;
	}

	return true;
}

bool	CRFWorldDatabase::Insert_Level_Log( DWORD dwCharacterSerial, BYTE byLevel, DWORD dwTotalPlayMin )
{
	char strQuery[256];

	sprintf( strQuery, "{ CALL pInsert_Level_Log( %d, %d, %d ) }", dwCharacterSerial, byLevel, dwTotalPlayMin );

	return ExecUpdateQuery( strQuery );
}

bool	CRFWorldDatabase::Insert_UserNum_Log( int nAvgUserNum, int nMaxUserNum )
{
	char strQuery[256];

	sprintf( strQuery, "{ CALL pInsert_UserNum_Log( %d, %d ) }", nAvgUserNum, nMaxUserNum );

	return ExecUpdateQuery( strQuery );
}

bool	CRFWorldDatabase::Rebirth_Base( DWORD dwCharacterSerial, char* szName )
{
	char strQuery[256];

	sprintf( strQuery, "{ CALL pRebirth_Base( %d, '%s' ) }", dwCharacterSerial, szName );

	return ExecUpdateQuery( strQuery );
}

bool	CRFWorldDatabase::Insert_ItemCharge( DWORD dwAvatorSerial, DWORD dwItemCode_K, DWORD dwItemCode_D, DWORD dwItemCode_U )
{
	char strQuery[256];

	sprintf( strQuery, 
		"{ CALL pInsert_ItemCharge( %d, %d, %d, %d ) }", dwAvatorSerial, dwItemCode_K, dwItemCode_D, dwItemCode_U );

	return ExecUpdateQuery( strQuery );
}

bool	CRFWorldDatabase::Select_ItemCharge( DWORD dwAvatorSerial, DWORD* pDwItemCode_K, DWORD* pDwItemCode_D, DWORD* pDwItemCode_U, DWORD* pDwItemChargeIndex )
{
	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256];
	int			nRowCount = 0;

	wsprintf( strQuery, "{ CALL pSelect_ItemCharge( %d ) }", dwAvatorSerial );

	if ( m_bSaveDBLog )
		worldProcessLog.WriteString( strQuery );

	if ( m_hStmtSelect == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			worldErrorLog.Write( "ReConnectDataBase Fail. Query : %s", strQuery );
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
		}
		return false;
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
		}
		return false;
	}

	sqlRet  = SQLGetData( m_hStmtSelect, 1, SQL_C_ULONG, pDwItemChargeIndex, 0, &size);
	sqlRet  = SQLGetData( m_hStmtSelect, 2, SQL_C_ULONG, pDwItemCode_K, 0, &size);
	sqlRet  = SQLGetData( m_hStmtSelect, 3, SQL_C_ULONG, pDwItemCode_D, 0, &size);
	sqlRet  = SQLGetData( m_hStmtSelect, 4, SQL_C_ULONG, pDwItemCode_U, 0, &size);

	if ( sqlRet == SQL_NO_DATA ) 
	{
		SQLCloseCursor( m_hStmtSelect );
		return false;
	}

	SQLCloseCursor( m_hStmtSelect );

	if ( m_bSaveDBLog )
		worldProcessLog.Write( "%s Success", strQuery );

	return true;
}

bool	CRFWorldDatabase::Delete_ItemCharge( DWORD dwItemChargeIndex )
{
	char strQuery[256];

	sprintf( strQuery, "{ CALL pDelete_ItemCharge( %d ) }", dwItemChargeIndex );

	return ExecUpdateQuery( strQuery );
}

BYTE	CRFWorldDatabase::Select_WaitItem( DWORD dwAvatorSerial, _worlddb_item_list* itemList )
{
	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256];
	int			nRowCount = 0;

	wsprintf( strQuery, "{ CALL pSelect_WaitItem( %d ) }", dwAvatorSerial );

	if ( m_bSaveDBLog )
		worldProcessLog.WriteString( strQuery );

	if ( m_hStmtSelect == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			worldErrorLog.Write( "ReConnectDataBase Fail. Query : %s", strQuery );
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
		}
		return db_result_sql_error;
	}

	while(TRUE) 
	{
		sqlRet = SQLFetch(m_hStmtSelect);

		if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) break;
	
		sqlRet  = SQLGetData( m_hStmtSelect, 1, SQL_C_ULONG, &itemList->itemList[nRowCount].dwItemCode_K, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 2, SQL_C_ULONG, &itemList->itemList[nRowCount].dwItemCode_D, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 3, SQL_C_ULONG, &itemList->itemList[nRowCount].dwItemCode_U, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 4, SQL_C_CHAR, itemList->itemList[nRowCount].szDate, 32, &size);

		nRowCount++;

		if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) break;
	}

	itemList->byItemCount = nRowCount;

	SQLCloseCursor( m_hStmtSelect );

	if ( m_bSaveDBLog )
		worldProcessLog.Write( "%s Success", strQuery );


	return db_result_sql_success;
}

BYTE	CRFWorldDatabase::Select_TakeItem( DWORD dwAvatorSerial, _worlddb_item_list* itemList)
{
	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256];
	int			nRowCount = 0;

	wsprintf( strQuery, "{ CALL pSelect_TakeItem( %d ) }", dwAvatorSerial );

	if ( m_bSaveDBLog )
		worldProcessLog.WriteString( strQuery );

	if ( m_hStmtSelect == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			worldErrorLog.Write( "ReConnectDataBase Fail. Query : %s", strQuery );
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
		}
		return db_result_sql_error;
	}

	while(TRUE) 
	{
		sqlRet = SQLFetch(m_hStmtSelect);

		if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) break;
	
		sqlRet  = SQLGetData( m_hStmtSelect, 1, SQL_C_ULONG, &itemList->itemList[nRowCount].dwItemCode_K, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 2, SQL_C_ULONG, &itemList->itemList[nRowCount].dwItemCode_D, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 3, SQL_C_ULONG, &itemList->itemList[nRowCount].dwItemCode_U, 0, &size);
		sqlRet  = SQLGetData( m_hStmtSelect, 4, SQL_C_CHAR, itemList->itemList[nRowCount].szDate, 32, &size);

		nRowCount++;

		if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) break;
	}

	itemList->byItemCount = nRowCount;

	SQLCloseCursor( m_hStmtSelect );

	if ( m_bSaveDBLog )
		worldProcessLog.Write( "%s Success", strQuery );


	return db_result_sql_success;
}

bool	CRFWorldDatabase::Select_AccountByAvatorName( char* szAvatorName, char* szAccount )
{
	SQLINTEGER  size;
	SQLRETURN	sqlRet;
	char		strQuery[256];
	int			nRowCount = 0;
	
	wsprintf( strQuery, "{ CALL pSelect_AccountByAvatorName('%s') }", szAvatorName );

	if ( m_bSaveDBLog )
		worldProcessLog.WriteString( strQuery );

	if ( m_hStmtSelect == SQL_NULL_HSTMT )
	{
		if ( ReConnectDataBase() == false )
		{
			worldErrorLog.Write( "ReConnectDataBase Fail. Query : %s", strQuery );
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

	sqlRet  = SQLGetData( m_hStmtSelect, 1, SQL_C_CHAR, szAccount, 17, &size);

	if ( sqlRet != SQL_SUCCESS && sqlRet != SQL_SUCCESS_WITH_INFO) 
	{
		SQLCloseCursor( m_hStmtSelect );
		ErrorAction( sqlRet, strQuery, "SQLGetData", m_hStmtSelect );
		return false;
	}

	SQLCloseCursor( m_hStmtSelect );

	if ( m_bSaveDBLog )
		worldProcessLog.Write( "%s Success", strQuery );

	return true;
}