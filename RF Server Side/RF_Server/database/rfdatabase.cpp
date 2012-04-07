// RFDatabase.cpp: implementation of the CRFDatabase class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "RFDatabase.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
 
CRFDatabase::CRFDatabase()
{
	m_pConnection = NULL;
	m_pCommand	  = NULL;
	m_pRecordSet  = NULL;
	m_pParameter  = NULL;

	HRESULT  hr = S_OK;
	TESTHR( m_pConnection.CreateInstance( __uuidof( Connection ) ) );
	TESTHR( m_pCommand.CreateInstance( __uuidof( Command ) ) );
	TESTHR( m_pParameter.CreateInstance( __uuidof( Parameter ) ) );
	
	m_szConn   [ 0 ] = 0;
	m_szUserID [ 0 ] = 0;
	m_szPasswd [ 0 ] = 0;
	m_szMachine[ 0 ] = 0;
	m_szDBName [ 0 ] = 0;

	m_szErrMsgCom     [ 0 ] = 0;
	m_szErrMsgProvider[ 0 ] = 0;
}

CRFDatabase::~CRFDatabase()
{
	this->Close();
}

//--------------------------------------------------------------------------------------
BOOL CRFDatabase::SetConnectionStr( const char* szConn )
{
//	assert( szConn != NULL );	

	if ( ::lstrlen( szConn ) >= CRFDatabase::CONNECTION_STR_SIZE )
		return FALSE;

	::lstrcpy( this->m_szConn, szConn );

	return TRUE;
}

//--------------------------------------------------------------------------------------
BOOL CRFDatabase::SetConnectionStr( const char* UserID, const char* Passwd, const char* Machine, const char* DBName )
{
//	assert( UserID != NULL && Passwd != NULL && Machine != NULL && DBName != NULL );

	if ( ::lstrlen( UserID )  >= CRFDatabase::USERID_SIZE   || 
		 ::lstrlen( Passwd )  >= CRFDatabase::PASSWD_SIZE   ||
		 ::lstrlen( Machine ) >= CRFDatabase::MACHINE_SIZE  || 
		 ::lstrlen( DBName )  >= CRFDatabase::DBNAME_SIZE )
		 return FALSE;
	
	::lstrcpy( this->m_szUserID,  UserID );
	::lstrcpy( this->m_szPasswd,  Passwd );
	::lstrcpy( this->m_szMachine, Machine );
	::lstrcpy( this->m_szDBName,  DBName );

	::lstrcpy( this->m_szConn, szConnectionHeader );
	
	::lstrcat( this->m_szConn, "User ID = " );
	::lstrcat( this->m_szConn, m_szUserID );
	
	::lstrcat( this->m_szConn, ";Password = " );
	::lstrcat( this->m_szConn, m_szPasswd );
	
	::lstrcat( this->m_szConn, ";Initial Catalog = " );
	::lstrcat( this->m_szConn, m_szDBName );
	
	::lstrcat( this->m_szConn, ";Data Source = " );
	::lstrcat( this->m_szConn, m_szMachine );

	return TRUE;
}

//--------------------------------------------------------------------------------------
const char* CRFDatabase::GetUserID( )
{
	return ( const char* )m_szUserID;
}

//--------------------------------------------------------------------------------------
const char* CRFDatabase::GetPasswd( )
{
	return ( const char* )m_szPasswd;
}

//--------------------------------------------------------------------------------------
const char* CRFDatabase::GetMachine( )
{
	return ( const char* )m_szMachine;
}

//--------------------------------------------------------------------------------------
const char* CRFDatabase::GetDBName( )
{
	return ( const char* )m_szDBName;
}

//--------------------------------------------------------------------------------------
BOOL CRFDatabase::IsOpen()
{
	if ( this->m_pConnection == NULL )
		return FALSE;

	return ( BOOL )( this->m_pConnection->State == adStateOpen );
}

//--------------------------------------------------------------------------------------
BOOL CRFDatabase::Open( int nConnectionTimeOut )
{
	if ( ::lstrlen( this->m_szConn ) == 0 )
		return FALSE;

	try 
	{
		if ( m_pConnection->State == adStateOpen )
			m_pConnection->Close();

		_bstr_t bstrConn( this->m_szConn );

		m_pConnection->ConnectionTimeout = nConnectionTimeOut;
		m_pConnection->Open( bstrConn, "", "", NULL );
		m_pCommand->ActiveConnection = m_pConnection;
		
		return TRUE;
	}
	catch( _com_error &e )
	{
		ErrorCom( e );
		return FALSE;
	}
}

//--------------------------------------------------------------------------------------
BOOL CRFDatabase::Open( const char* szConnectionString, int nConnectionTimeOut )
{
//	assert( szConnectionString != NULL );
	
	if ( szConnectionString == NULL )
		return FALSE;
	
	if ( ::lstrlen( szConnectionString ) >= CRFDatabase::CONNECTION_STR_SIZE )
		return FALSE;

	::lstrcpy( this->m_szConn, szConnectionString );

	try 
	{
		if ( m_pConnection->State == adStateOpen )
			m_pConnection->Close();

		_bstr_t bstrConn( this->m_szConn );

		m_pConnection->ConnectionTimeout = nConnectionTimeOut;
		m_pConnection->Open( bstrConn, "", "", NULL );
		m_pCommand->ActiveConnection = m_pConnection;
		
		return TRUE;
	}
	catch( _com_error &e )
	{
		ErrorCom( e );
		return FALSE;
	}
}

//--------------------------------------------------------------------------------------
void CRFDatabase::Close()
{
	try
	{
		if ( this->m_pConnection->State == adStateOpen )
			m_pConnection->Close();
	}
	catch( _com_error &e )
	{
		ErrorCom ( e );
	}
	
	return;
}

//--------------------------------------------------------------------------------------
BOOL CRFDatabase::QuerySql( char* szSql, CADORecordBinding* pUserData )
{	
	if(m_pConnection == NULL)
		return FALSE;
	
	HRESULT  hr = S_OK;
	if ( m_pConnection->State == adStateClosed ) return FALSE;

	IADORecordBinding *picRs      = NULL;

	try
	{
		if ( m_pRecordSet == NULL )
		{
			TESTHR( m_pRecordSet.CreateInstance( __uuidof( Recordset ) ) );
		}

		m_pRecordSet->CursorLocation = adUseClient;

		m_pRecordSet->Open ( szSql, 
							 m_pConnection.GetInterfacePtr(),
							 adOpenKeyset, 
							 adLockOptimistic, 
							 adCmdText );

		TESTHR( m_pRecordSet->QueryInterface( __uuidof(IADORecordBinding), ( LPVOID* ) &picRs ) );
        TESTHR( picRs->BindToRecordset( pUserData ) );

		if ( picRs ) 
		{ 
			picRs->Release(); 
			picRs = NULL; 
		}

		return TRUE;
	}
	catch( _com_error &e )
    {
        //QueryErrorHandler( e );
		return FALSE;
    }
}

//TODO..
//쿼리의 타임아웃을 설정하기위해 command객체를 이용한 작업을 시도..
BOOL CRFDatabase::QuerySql( char* szSql, int nTimeOut )
{
	if(m_pConnection == NULL)
		return FALSE;

	if(m_pCommand == NULL)
		return FALSE;

	HRESULT  hr = S_OK;
	if ( m_pConnection == NULL || m_pConnection->State == adStateClosed ) return FALSE;
	if ( m_pCommand == NULL ) return FALSE;

	try
	{
		// command를 통한 퀴리에서는 recordset pointer가 초기화된 객체가 아닌 NULL 이어야 한다
		if ( m_pRecordSet )	
		{
			if ( m_pRecordSet->State == adStateOpen ) 
			{
				m_pRecordSet->Close();
			}

			m_pRecordSet = NULL;
		}

		m_pCommand->CommandText    = szSql;
		m_pCommand->CommandType    = adCmdText;
		m_pCommand->CommandTimeout = nTimeOut;

		m_pRecordSet = m_pCommand->Execute( NULL, NULL, NULL );

		return TRUE;
	}
	catch( _com_error &e )
    {
        QueryErrorHandler( e );
		return FALSE;
    }
}

BOOL CRFDatabase::QuerySqlRsCount( char* szSql, int nTimeOut )
{
	if(m_pConnection == NULL)
		return FALSE;

	if(m_pCommand == NULL)
		return FALSE;

	HRESULT  hr = S_OK;
	if ( m_pConnection == NULL || m_pConnection->State == adStateClosed ) return FALSE;
	if ( m_pCommand == NULL ) return FALSE;

	try
	{
		// command를 통한 퀴리에서는 recordset pointer가 초기화된 객체가 아닌 NULL 이어야 한다
		if ( m_pRecordSet )	
		{
			if ( m_pRecordSet->State == adStateOpen ) 
			{
				m_pRecordSet->Close();
			}
		}
		else
		{
			TESTHR( m_pRecordSet.CreateInstance( __uuidof( Recordset ) ) );
		}

		m_pRecordSet->CursorLocation = adUseClient;
		m_pRecordSet->Open ( szSql, 
							 m_pConnection.GetInterfacePtr(),
							 adOpenKeyset, 
							 adLockOptimistic, 
							 adCmdText );

		return TRUE;
	}
	catch( _com_error &e )
    {
        //QueryErrorHandler( e );
		return FALSE;
    }
}

//--------------------------------------------------------------------------------------
BOOL CRFDatabase::QueryTable( char* szTableName, CADORecordBinding* pUserData )
{
	if(m_pRecordSet == NULL)
		return FALSE;

	if(m_pConnection == NULL)
		return FALSE;

	if(pUserData == NULL)
		return FALSE;

	HRESULT  hr = S_OK;
	
	if ( m_pConnection == NULL || m_pConnection->State == adStateClosed ) return FALSE;

	IADORecordBinding *picRs      = NULL;

	try
	{
		if ( m_pRecordSet == NULL )
		{
			TESTHR( m_pRecordSet.CreateInstance( __uuidof( Recordset ) ) );
		}

		m_pRecordSet->CursorLocation = adUseClient;
	
		m_pRecordSet->Open( szTableName, 
			                _variant_t( ( IDispatch * ) m_pConnection ), 
					        adOpenKeyset, 
							adLockOptimistic, 
							adCmdTable );

		TESTHR( m_pRecordSet->QueryInterface( __uuidof(IADORecordBinding), ( LPVOID* ) &picRs ) );
        TESTHR( picRs->BindToRecordset( pUserData ) );

		if ( picRs ) 
		{
			picRs->Release();
			picRs = NULL;
		}

		return TRUE;
	}
	catch( _com_error &e )
    {
        //QueryErrorHandler( e );
		return FALSE;
    }
}

//--------------------------------------------------------------------------------------
int CRFDatabase::AnalFieldInt( ULONG ulStatus, int iValue )
{
	return ( int ) ( ulStatus == adFldOK ? iValue : 0 );
}

UINT CRFDatabase::AnalFieldUINT( ULONG ulStatus, UINT iValue )
{
	return ( UINT ) ( ulStatus == adFldOK ? iValue : 0 );
}

LONG CRFDatabase::AnalFieldLong( ULONG ulStatus, LONG iValue )
{
	return ( LONG ) ( ulStatus == adFldOK ? iValue : 0 );
}

char CRFDatabase::AnalFieldChar( ULONG ulStatus, char szValue )
{
	return ( char ) ( ulStatus == adFldOK ? szValue : '0' );
}

BYTE CRFDatabase::AnalFieldBYTE( ULONG ulStatus, BYTE Value )
{
	return ( BYTE ) ( ulStatus == adFldOK ? Value : 0 );
}

char * CRFDatabase::AnalFieldString( ULONG ulStatus, char* szValue )
{
	return ( char * ) ( ulStatus == adFldOK ? szValue : "" );
}

//--------------------------------------------------------------------------------------
void  CRFDatabase::GetFieldInt( int& nDest, ULONG ulStatus, int iValue )
{
	nDest = AnalFieldInt( ulStatus, iValue );
}

void  CRFDatabase::GetFieldUINT( UINT& nDest, ULONG ulStatus, UINT iValue )
{
	nDest = AnalFieldUINT( ulStatus, iValue );
}

void  CRFDatabase::GetFieldLong( LONG& nDest, ULONG ulStatus, LONG iValue )
{
	nDest = AnalFieldLong( ulStatus, iValue );
}

void  CRFDatabase::GetFieldChar( char& cDest, ULONG ulStatus, char cValue )
{
	cDest = AnalFieldChar( ulStatus, cValue );	
}

void  CRFDatabase::GetFieldBYTE( BYTE& cDest, ULONG ulStatus, BYTE cValue )
{
	cDest = AnalFieldBYTE( ulStatus, cValue );	
}

BOOL  CRFDatabase::GetFieldString( char* szDest, ULONG ulStatus, char* szValue, int iDestSize )
{
//	assert( szDest );
	
	if ( iDestSize <= ::lstrlen( szValue ) )
		return FALSE;

	::lstrcpy( szDest, AnalFieldString( ulStatus, szValue ) );
	return TRUE;
}

//--------------------------------------------------------------------------------------
BOOL CRFDatabase::ExecuteSQL( char* szSql, int nCommandTimeOut )
{
//	assert( szSql != NULL );
	
	if ( m_pConnection == NULL || m_pConnection->State == adStateClosed ) 
		return FALSE;
	    
	try
	{
		m_pCommand->ActiveConnection = m_pConnection;
		m_pCommand->CommandTimeout   = nCommandTimeOut;
		m_pCommand->CommandType      = adCmdText;
		m_pCommand->CommandText      = szSql;

		m_pCommand->Execute( NULL, NULL, NULL );

		return TRUE;
	}
	catch( _com_error &e )
	{
		ErrorProvider( m_pConnection );
		ErrorCom( e );
		return FALSE;
	}
}

BOOL CRFDatabase::ExecuteProc( char* szProcName, int nCommandTimeOut )
{
	if ( m_pConnection == NULL || m_pConnection->State == adStateClosed ) 
		return FALSE;
	    
	try
	{
		m_pCommand->CommandText      = szProcName;
		m_pCommand->ActiveConnection = m_pConnection;
		m_pCommand->CommandTimeout   = nCommandTimeOut;
		m_pCommand->CommandType      = adCmdStoredProc;

		m_pCommand->Execute( NULL, NULL, NULL );

		return TRUE;
	}
	catch( _com_error &e )
	{
		ErrorProvider( m_pConnection );
		ErrorCom( e );
		return FALSE;
		
/*		_bstr_t bstrSource(e.Source());
		_bstr_t bstrDescription(e.Description());
		CString errMsg, sMsg;
		errMsg.Format("%08lx : %s\n", e.Error(), e.ErrorMessage());
		sMsg += errMsg;
		errMsg.Format("Source = %s\n", (LPCSTR) bstrSource);
		sMsg += errMsg;
		errMsg.Format("Description = %s", (LPCSTR) bstrDescription);
		sMsg += errMsg;
		AfxMessageBox(sMsg, MB_ICONSTOP);

		return FALSE;*/
	}
}

//--------------------------------------------------------------------------------------
void CRFDatabase::AddParameter( char* szVarName, DataTypeEnum nParamType, long nParamSize, ParameterDirectionEnum nDirection, VARIANT vtValue )
{
	_ParameterPtr pParam; 
	pParam = m_pCommand->CreateParameter( szVarName, 
									      nParamType, 
								          nDirection, 
								          nParamSize, 
								          vtValue );
	m_pCommand->Parameters->Append( pParam );
}

void CRFDatabase::DeleteParameter( char* szVarName )
{
	m_pCommand->Parameters->Delete( szVarName );
}

//--------------------------------------------------------------------------------------
BOOL CRFDatabase::Trans( int nCmd )
{
	if ( m_pConnection == NULL || m_pConnection->State == adStateClosed ) 
		return FALSE;
	    
	switch ( nCmd )
	{
	case BEGIN:
		m_pConnection->BeginTrans();
		break;
	case COMMIT:
		m_pConnection->CommitTrans();
		break;
	case ROLLBACK:
		m_pConnection->RollbackTrans();
		break;
	}

	return TRUE;
}

//--------------------------------------------------------------------------------------
void CRFDatabase::QueryErrorHandler( _com_error &e )
{
	_variant_t vtConnect = m_pRecordSet->GetActiveConnection();

    switch( vtConnect.vt )
    {
    case VT_BSTR:		ErrorCom( e );
						break;
    case VT_DISPATCH:   ErrorProvider( vtConnect );
						break;
    default:
        break;
    }
}

//--------------------------------------------------------------------------------------
void CRFDatabase::ErrorProvider( _ConnectionPtr pConnection )
{
	if( ( pConnection->Errors->Count ) > 0 )
	{
		ErrorPtr pErr   = NULL;

		long nCount = pConnection->Errors->Count;

		char szErrTemp[ 256 ];
		for( long i = 0; i < nCount; i++ )    
		{
			pErr = pConnection->Errors->GetItem( i );
			
			::wsprintf( szErrTemp, 
				        "Error number: %d\t( %s )\r\n", 
				        pErr->Number, ( LPCSTR ) pErr->Description );
			
			::lstrcat( m_szErrMsgProvider, szErrTemp );
		}
	}
}

//--------------------------------------------------------------------------------------
void CRFDatabase::ErrorCom( _com_error &e )
{
	_bstr_t bstrSource( e.Source() );
	_bstr_t bstrDescription( e.Description() );

	CHAR szErrMsg[ 500 ];
	CHAR szBuff  [ 200 ];

	::lstrcpy( szErrMsg, "Error\r\n" );

	::wsprintf( szBuff, "Code = %ld\r\n", e.Error() );
	::lstrcat( szErrMsg, szBuff );

	::wsprintf( szBuff, "Code meaning = %s\r\n", e.ErrorMessage() );
	::lstrcat( szErrMsg, szBuff );

	::wsprintf( szBuff, "Source = %s\r\n", (LPCSTR) bstrSource );
	::lstrcat( szErrMsg, szBuff );

	::wsprintf( szBuff, "Description = %s\r\n", (LPCSTR) bstrDescription );
	::lstrcat( szErrMsg, szBuff );

	::lstrcpy( m_szErrMsgCom, szErrMsg );
}

const char* CRFDatabase::GetLastErrMsg()
{
	if ( ::lstrlen( m_szErrMsgProvider ) <= 0 ) 
		return NULL;

	return ( const char * ) m_szErrMsgProvider;
}

char* __CVC(char* pStr)
{	
	char* p = pStr;
	while(*p != ' ' && *p != NULL)
		p++;
	*p = NULL;
	return pStr;
}

