////////////////////////////////////////////////////////////////////////////
//
// Load from Data File
//
////////////////////////////////////////////////////////////////////////////

#include "./GameObject/Manager/CCharacterMgr.h"
#include "./DataTable/CCharacterDataMgr.h"
#include "./DataTable/CItemDataMgr.h"

#include "CLand.h"

#include "Global.h"
#include "DataFile.h"

#include "./UserInterface/GameWindow/EtcWin/GUIOptionWindow.h"

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CDataString::CDataString()
{
	ZeroMemory( this, sizeof( CDataString ) );
}

CDataString::~CDataString()
{
	if( m_pString )
	{
		delete [] m_pString;
		m_pString = NULL;
	}
}

DWORD
CDataString::Read( void * pi_pBuffer, int pi_nSize, int pi_nReadCount )
{
	if( !pi_pBuffer )
		return 0;

	if( !pi_nSize || !pi_nReadCount )
		return 0;

	DWORD l_dwHowMuchRead = pi_nSize * pi_nReadCount;
	if( l_dwHowMuchRead > ( m_dwStringLength - m_dwCurPos ) )
		return 0;

	memcpy( pi_pBuffer, &m_pString[m_dwCurPos], l_dwHowMuchRead );
	m_dwCurPos += l_dwHowMuchRead;

	return l_dwHowMuchRead;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CDataFile::CDataFile()
{
	ZeroMemory( this, sizeof( CDataFile ) );
	m_hSourceFile = INVALID_HANDLE_VALUE;
}

CDataFile::~CDataFile()
{
	if( m_hSourceFile != INVALID_HANDLE_VALUE )
	{
		CloseHandle( m_hSourceFile );
		m_hSourceFile = INVALID_HANDLE_VALUE;
	}
}

CDataFile::CDataFile( char * pi_pFileName )
{
	ZeroMemory( this, sizeof( CDataFile ) );
	m_hSourceFile = INVALID_HANDLE_VALUE;

	LoadFile( pi_pFileName );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CDataFile::LoadFile( char * pi_pFileName )
{
	if( !pi_pFileName )
		return FALSE;

	// open file
	m_hSourceFile = CreateFile( pi_pFileName, GENERIC_READ, FILE_SHARE_READ,
								NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
	if( m_hSourceFile == INVALID_HANDLE_VALUE )
		return FALSE;

	// read file
	LoadFile_ReadFile();

	// close file
	CloseHandle( m_hSourceFile );
	m_hSourceFile = INVALID_HANDLE_VALUE;

	// decode encryption key
	DecodeEncryptionKey();

	// decode source
	DecodeSourceString();

	// delete key
	ZeroMemory( m_pKey, MAX_KEY_LENGTH );

	return TRUE;
}

BOOL
CDataFile::LoadFile_ReadFile( void )
{
	LoadFile_ReadSection( m_pHeader, HEADER_LENGTH );

	DWORD l_dwSourceStringLength;
	LoadFile_ReadSection( ( char * )&l_dwSourceStringLength, sizeof( DWORD ) );
	m_cSourceData.SetStringLength( l_dwSourceStringLength );

	m_cSourceData.SetString( new char[l_dwSourceStringLength+1] );
	char * l_pSourceString = m_cSourceData.GetString();
	LoadFile_ReadSection( l_pSourceString, l_dwSourceStringLength );
	l_pSourceString[l_dwSourceStringLength] = NULL;

	LoadFile_ReadSection( m_pKey, MAX_KEY_LENGTH );

	return TRUE;
}

BOOL
CDataFile::LoadFile_ReadSection( char * po_pBuffer, int pi_nSize )
{
	if( m_hSourceFile == INVALID_HANDLE_VALUE || !po_pBuffer )
		return FALSE;

	DWORD l_dwRealReadSize;
	if( !ReadFile( m_hSourceFile, po_pBuffer, pi_nSize, &l_dwRealReadSize, NULL ) )
	{
		CloseHandle( m_hSourceFile );
		m_hSourceFile = INVALID_HANDLE_VALUE;
		return FALSE;
	}

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CDataFile::DecodeSourceString( void )
{
	char * l_pSourceString = m_cSourceData.GetString();
	if( !l_pSourceString )
		return FALSE;

	BOOL l_bEven = TRUE;

	for( int i = 0; i < m_cSourceData.GetStringLength(); ++i )
	{
		if( l_bEven )	{ l_pSourceString[i] -= m_pKey[ ( i + 1 ) % MAX_KEY_LENGTH ]; l_bEven = FALSE; }
		else			{ l_pSourceString[i] += m_pKey[ ( i + 1 ) % MAX_KEY_LENGTH ]; l_bEven = TRUE; }
	}

	return TRUE;
}

BOOL
CDataFile::DecodeEncryptionKey( void )
{
	int		i;
	char	l_nTemp;

	const char DIGIT[8] = { 1, 2, 4, 8, 16, 32, 64, (char)128 };
	BOOL l_bEven = TRUE;

	for( i = 0; i < MAX_KEY_LENGTH; ++i )
	{
		if( l_bEven )	{ m_pKey[i] -= DIGIT[ ( i + 1 ) % 8 ]; l_bEven = FALSE; }
		else			{ m_pKey[i] += DIGIT[ ( i + 1 ) % 8 ]; l_bEven = TRUE; }
	}

	for( i = 0; i < MAX_KEY_LENGTH / 2; ++i )
	{
		l_nTemp = m_pKey[MAX_KEY_LENGTH-1-i]; m_pKey[MAX_KEY_LENGTH-1-i] = m_pKey[i]; m_pKey[i] = l_nTemp;
	}

	for( i = 0; i < MAX_KEY_LENGTH; i += 2 )
	{
		l_nTemp = m_pKey[i+1]; m_pKey[i+1] = m_pKey[i]; m_pKey[i] = l_nTemp;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CDataRecord::LoadData( char * pi_pFileName )
{
	CDataFile l_fileData( pi_pFileName );

	CDataString * l_pSourceData = l_fileData.GetSourceData();
	if( !l_pSourceData )
		return FALSE;
	if( !l_pSourceData->GetString() )
		return FALSE;

	// read header
	l_pSourceData->Read( &m_Header.m_nRecordNum, sizeof( int ), 1 );
	l_pSourceData->Read( &m_Header.m_nFieldNum, sizeof( int ), 1 );
	l_pSourceData->Read( &m_Header.m_nRecordSize, sizeof( int ), 1 );

	m_psRecord = new char * [m_Header.m_nRecordNum];
	for( int i = 0; i < m_Header.m_nRecordNum; i++ )
		m_psRecord[i] = NULL;

	// read record
	if( m_Header.m_nRecordNum && m_Header.m_nFieldNum && m_Header.m_nRecordSize )
	{
		for( int i = 0; i < m_Header.m_nRecordNum; i++ )
		{
			if( m_psRecord[i] )
				return FALSE;

			m_psRecord[i] = new char [m_Header.m_nRecordSize];
			l_pSourceData->Read( m_psRecord[i], m_Header.m_nRecordSize, 1 );
		}
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

HSD_FONT::HSD_FONT()
{
	m_byStyle	= 0;
	m_bySize	= 7;
	m_dwColor	= 0xFFA0A0A0;
	m_dwLinkID	= 0xFFFFFFFF;
}

//==============================================================================

HSD_STRING::HSD_STRING()
{
	m_wLength	= 0;
	m_pString	= NULL;
}

HSD_STRING::~HSD_STRING()
{
	if( m_pString )
	{
		delete [] m_pString;
		m_pString = NULL;
	}
}

//==============================================================================

HSD_BASE_DATA::HSD_BASE_DATA()
{
	m_dwID				= ID_INVALID;

	m_listString		= NULL;
	m_byTotalStringNum	= 0;
}

HSD_BASE_DATA::~HSD_BASE_DATA()
{
	if( m_listString )
	{
		delete [] m_listString;
		m_listString = NULL;
	}
}

DWORD
HSD_BASE_DATA::GetStringLength( void )
{
	DWORD l_dwStringLength = 0;
	for( int i = 0; i < m_byTotalStringNum; ++i )
		l_dwStringLength += m_listString[i].m_wLength;

	return l_dwStringLength;
}

//==============================================================================

HSD_HINT::HSD_HINT( void )
{
	m_byLevelMin	= 0;
	m_byLevelMax	= 50;
	m_byMapNo		= 0xFF;		// 모든맵
	m_dwActionID	= 0;
	m_dwTime		= 5000;
	m_bIsDispaly	= TRUE;		// 화면상에 디스플레이할 것인가?
}

//==============================================================================

HSD_HELP::HSD_HELP()
{
	m_dwWindowID	= ID_INVALID;
	m_dwSectionID	= ID_INVALID;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CHintMgr::CHintMgr()
{
	ZeroMemory( this, sizeof( CHintMgr ) );

	m_bFindNextHint	= TRUE;

	MAX_HINT_DISPLAY_LENGTH = ( GetR3State()->mScreenXsize / ( 2 * FONT_WIDTH ) );

	m_pCopyBuffer	= new char[MAX_HINT_DISPLAY_LENGTH + 1];

	m_bIsRenderHint	= TRUE;
}

CHintMgr::~CHintMgr()
{
	if( m_listHint )
	{
		delete [] m_listHint;
		m_listHint = NULL;
	}

	if( m_pCopyBuffer )
	{
		delete [] m_pCopyBuffer;
		m_pCopyBuffer = NULL;
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CHintMgr::LoadData( void )
{
	CDataFile l_fileHint( "./DataTable/Hint.edf" );

	CDataString * l_pSourceData = l_fileHint.GetSourceData();
	if( !l_pSourceData )
		return FALSE;
	if( !l_pSourceData->GetString() )
		return FALSE;

	// data read
	l_pSourceData->Read( &m_dwTotalHintNum, sizeof( DWORD ), 1 );

	m_listHint = new HSD_HINT[ m_dwTotalHintNum ];

	for( int i = 0; i < m_dwTotalHintNum; ++i )
	{
		l_pSourceData->Read( &m_listHint[i].m_dwID, sizeof( DWORD ), 1 );
		l_pSourceData->Read( &m_listHint[i].m_byLevelMin, sizeof( BYTE ), 1 );
		l_pSourceData->Read( &m_listHint[i].m_byLevelMax, sizeof( BYTE ), 1 );
		l_pSourceData->Read( &m_listHint[i].m_byMapNo, sizeof( BYTE ), 1 );
		l_pSourceData->Read( &m_listHint[i].m_dwActionID, sizeof( DWORD ), 1 );
		l_pSourceData->Read( &m_listHint[i].m_dwTime, sizeof( DWORD ), 1 );
		l_pSourceData->Read( &m_listHint[i].m_bIsDispaly, sizeof( BOOL ), 1 );

		l_pSourceData->Read( &m_listHint[i].m_byTotalStringNum, sizeof( BYTE ), 1 );
		m_listHint[i].m_listString = new HSD_STRING[m_listHint[i].m_byTotalStringNum];
		for( int j = 0; j < m_listHint[i].m_byTotalStringNum; ++j )
		{
			l_pSourceData->Read( &m_listHint[i].m_listString[j].m_stFont, sizeof( HSD_FONT ), 1 );
			l_pSourceData->Read( &m_listHint[i].m_listString[j].m_wLength, sizeof( WORD ), 1 );
			m_listHint[i].m_listString[j].m_pString = new char[m_listHint[i].m_listString[j].m_wLength+1];
			ZeroMemory( m_listHint[i].m_listString[j].m_pString, m_listHint[i].m_listString[j].m_wLength+1 );
			l_pSourceData->Read( m_listHint[i].m_listString[j].m_pString, m_listHint[i].m_listString[j].m_wLength, 1 );
		}
	}

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CHintMgr::SetDisplayHint( void )
{
	if( m_bFindNextHint )
	{
		for( int i = 0; i < m_dwTotalHintNum; ++i )
		{
			m_dwCurHint = ( ++m_dwCurHint ) % m_dwTotalHintNum;

			if( !m_listHint[m_dwCurHint].m_bIsDispaly )
				continue;

			// 레벨 체크
			if( !( ( _GetAvatar()->GetLevel() >= m_listHint[m_dwCurHint].m_byLevelMin ) &&
				   ( _GetAvatar()->GetLevel() <= m_listHint[m_dwCurHint].m_byLevelMax ) ) )
				continue;

			// 맵 체크
			if( m_listHint[m_dwCurHint].m_byMapNo != 0xFF )
			{
				if( _GetLand()->GetMapIndex() != m_listHint[m_dwCurHint].m_byMapNo )
					continue;
			}

			// 액션 체크
			m_listHint[m_dwCurHint].m_dwActionID;

			// 세팅
			m_bFindNextHint = FALSE;
			m_dwDisplayStartTime = timeGetTime();
			return;
		}
	}
	else
	{
		if( timeGetTime() - m_dwDisplayStartTime > m_listHint[m_dwCurHint].m_dwTime )
			m_bFindNextHint = TRUE;
	}
}

BOOL
CHintMgr::RenderHint( void )
{
	if( !m_bIsRenderHint || ( m_dwTotalHintNum <= 0 ) )
		return FALSE;

	SetDisplayHint();

	// 스트링 출력
	int l_nX = -1, l_nY = -1;
	int l_nStringLength = RenderHint_Single( &m_listHint[m_dwCurHint], l_nX, l_nY );

	// 링크 출력
	int l_nCurLength;
	for( int i = 0; i < m_listHint[m_dwCurHint].m_byTotalStringNum; ++i )
	{
		l_nCurLength = m_listHint[m_dwCurHint].m_listString[i].m_wLength * FONT_WIDTH;

		if( m_listHint[m_dwCurHint].m_listString[i].m_stFont.m_dwLinkID != ID_INVALID )
		{
			if( gMouse.x >= l_nX && gMouse.x <= l_nX + l_nCurLength &&
				gMouse.y >= l_nY - 5 && gMouse.y <= l_nY + 17 )
			{
				l_nX = gMouse.x + 10;
				l_nY = gMouse.y + 20;
				RenderHint_Single( &m_listHint[m_listHint[m_dwCurHint].m_listString[i].m_stFont.m_dwLinkID], l_nX, l_nY );
				break;
			}
		}

		l_nX += l_nCurLength;
	}

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

DWORD
CHintMgr::RenderHint_Single( HSD_HINT * pi_pHint, int & pio_nX, int & pio_nY )
{
	RenderHint_Single_DrawBox( pi_pHint, pio_nX, pio_nY );

	DWORD l_dwDisplayedStringLength = RenderHint_Single_DrawString( pi_pHint, pio_nX, pio_nY );

	return l_dwDisplayedStringLength;
}

BOOL
CHintMgr::RenderHint_Single_DrawBox( HSD_HINT * pi_pHint, int & pio_nX, int & pio_nY )
{
	if( !pi_pHint )
		return FALSE;

	int l_nDisplayLength = pi_pHint->GetStringLength();
	if( l_nDisplayLength > MAX_HINT_DISPLAY_LENGTH )
		l_nDisplayLength = MAX_HINT_DISPLAY_LENGTH;

	int l_nLine = ( pi_pHint->GetStringLength() / MAX_HINT_DISPLAY_LENGTH ) + 1;

	if( pio_nX == -1 || pio_nY == -1 )
	{
		pio_nX = ( GetR3State()->mScreenXsize / 2 ) - ( ( l_nDisplayLength * FONT_WIDTH ) / 2 );
		pio_nY = 20;
	}

	Draw2DRectangle( pio_nX - 10,
					 pio_nY - 5,
					 pio_nX + ( l_nDisplayLength * FONT_WIDTH ) + 10,
					 pio_nY + ( l_nLine * FONT_HEIGHT ),
					 0x70A0A070 );

	return TRUE;
}

DWORD
CHintMgr::RenderHint_Single_DrawString( HSD_HINT * pi_pHint, int & pio_nX, int & pio_nY )
{
	if( !pi_pHint )
		return FALSE;

	int	l_nStrX = 0;
	int l_nStrY = 0;

	int l_nStartPosInCurString = 0;

	for( int i = 0; i < pi_pHint->m_byTotalStringNum; )
	{
		if( ( l_nStrX + ( pi_pHint->m_listString[i].m_wLength - l_nStartPosInCurString ) ) <= MAX_HINT_DISPLAY_LENGTH )
		{
			DrawR3Hangul( pio_nX + ( l_nStrX * FONT_WIDTH ),
						  pio_nY + ( l_nStrY * FONT_HEIGHT ),
						  &pi_pHint->m_listString[i].m_pString[l_nStartPosInCurString],
						  pi_pHint->m_listString[i].m_stFont.m_dwColor,
						  pi_pHint->m_listString[i].m_stFont.m_byStyle );

			l_nStrX += ( pi_pHint->m_listString[i].m_wLength - l_nStartPosInCurString );
			l_nStartPosInCurString = 0;
			++i;
		}
		else
		{
			int l_nCopyLength = MAX_HINT_DISPLAY_LENGTH - l_nStrX;

			if( pi_pHint->m_listString[i].m_pString[l_nStartPosInCurString+l_nCopyLength-1] & 0x80 )
			{
				for( int j = l_nStartPosInCurString+l_nCopyLength-1; j >= l_nStartPosInCurString; --j )
				{
					if( !( pi_pHint->m_listString[i].m_pString[j] & 0x80 ) )
						break;
				}

				if( ( ( l_nStartPosInCurString + l_nCopyLength - 1 - j ) % 2 ) != 0 )
					--l_nCopyLength;
			}

			if( l_nCopyLength > 0 )
			{
				ZeroMemory( m_pCopyBuffer, MAX_HINT_DISPLAY_LENGTH );
				memcpy( m_pCopyBuffer, &pi_pHint->m_listString[i].m_pString[l_nStartPosInCurString], l_nCopyLength );
				m_pCopyBuffer[l_nCopyLength] = 0;
				DrawR3Hangul( pio_nX + ( l_nStrX * FONT_WIDTH ),
							  pio_nY + ( l_nStrY * FONT_HEIGHT ),
							  m_pCopyBuffer,
							  pi_pHint->m_listString[i].m_stFont.m_dwColor,
							  pi_pHint->m_listString[i].m_stFont.m_byStyle );

				l_nStartPosInCurString += l_nCopyLength;
			}
			else
			{
				l_nStartPosInCurString = 0;
			}

			l_nStrX = 0;
			++l_nStrY;
		}
	}

	return l_nStrX;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CHintMgr::EnableRenderHint( BOOL pi_bFlag )
{
	m_bIsRenderHint = pi_bFlag;

	if( _GetOptionWindow() )
		_GetOptionWindow()->SwitchOn( CGUIOptionWindow::Hint, pi_bFlag );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CUIHelpMgr::CUIHelpMgr()
{
	m_listUIHelp		= NULL;
	m_dwTotalHelpNum	= 0;
}

CUIHelpMgr::~CUIHelpMgr()
{
	if( m_listUIHelp )
	{
		delete [] m_listUIHelp;
		m_listUIHelp = NULL;
	}
}

BOOL
CUIHelpMgr::LoadData( void )
{
	CDataFile l_fileUIHelp( "./DataTable/UIHelp.edf" );

	CDataString * l_pSourceData = l_fileUIHelp.GetSourceData();
	if( !l_pSourceData )
		return FALSE;
	if( !l_pSourceData->GetString() )
		return FALSE;

	// data read
	l_pSourceData->Read( &m_dwTotalHelpNum, sizeof( DWORD ), 1 );

	m_listUIHelp = new HSD_HELP[ m_dwTotalHelpNum ];

	for( int i = 0; i < m_dwTotalHelpNum; ++i )
	{
		l_pSourceData->Read( &m_listUIHelp[i].m_dwID, sizeof( DWORD ), 1 );
		l_pSourceData->Read( &m_listUIHelp[i].m_dwWindowID, sizeof( DWORD ), 1 );
		l_pSourceData->Read( &m_listUIHelp[i].m_dwSectionID, sizeof( DWORD ), 1 );

		l_pSourceData->Read( &m_listUIHelp[i].m_byTotalStringNum, sizeof( BYTE ), 1 );
		m_listUIHelp[i].m_listString = new HSD_STRING[m_listUIHelp[i].m_byTotalStringNum];
		for( int j = 0; j < m_listUIHelp[i].m_byTotalStringNum; ++j )
		{
			l_pSourceData->Read( &m_listUIHelp[i].m_listString[j].m_stFont, sizeof( HSD_FONT ), 1 );
			l_pSourceData->Read( &m_listUIHelp[i].m_listString[j].m_wLength, sizeof( WORD ), 1 );
			m_listUIHelp[i].m_listString[j].m_pString = new char[m_listUIHelp[i].m_listString[j].m_wLength+1];
			ZeroMemory( m_listUIHelp[i].m_listString[j].m_pString, m_listUIHelp[i].m_listString[j].m_wLength+1 );
			l_pSourceData->Read( m_listUIHelp[i].m_listString[j].m_pString, m_listUIHelp[i].m_listString[j].m_wLength, 1 );
		}
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CDataMgr::CDataMgr()
{
	m_dwTotalNum	= 0;
	m_dwRecordSize	= 0;
	m_pData			= NULL;
}

CDataMgr::~CDataMgr()
{
	if( m_pData )
	{
		delete [] m_pData;
		m_pData = NULL;
	}
}

BOOL
CDataMgr::LoadData( char * pi_pFileName )
{
	CDataFile fileSourceData( pi_pFileName );

	return LoadData( fileSourceData.GetSourceData() );
}

BOOL
CDataMgr::LoadData( CDataString * pi_pSourceData )
{
	if( !pi_pSourceData )
		return FALSE;
	if( !pi_pSourceData->GetString() )
		return FALSE;

	// data read
	pi_pSourceData->Read( &m_dwTotalNum, sizeof( DWORD ), 1 );
	pi_pSourceData->Read( &m_dwRecordSize, sizeof( DWORD ), 1 );

	m_pData = new char[ m_dwRecordSize * m_dwTotalNum ];

	pi_pSourceData->Read( m_pData, m_dwRecordSize, m_dwTotalNum );

	return TRUE;
}

void
CDataMgr::UnloadData( void )
{
	if( m_pData )
	{
		delete [] m_pData;
		m_pData			= NULL;
		m_dwTotalNum	= 0;
		m_dwRecordSize	= 0;
	}
}

BASE_DATA *
CDataMgr::GetData( DWORD pi_dwIndex )
{
	if( ( pi_dwIndex < 0 ) || ( pi_dwIndex >= m_dwTotalNum ) )
		return NULL;

	return ( ( BASE_DATA * )&m_pData[ pi_dwIndex * m_dwRecordSize ] );
}

void *
CDataMgr::GetData_Void( DWORD pi_dwIndex )
{
	if( ( pi_dwIndex < 0 ) || ( pi_dwIndex >= m_dwTotalNum ) )
		return NULL;

	return ( ( void * )&m_pData[ pi_dwIndex * m_dwRecordSize ] );
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 게임 설정 관련
BOOL
LoadGameSetting( char * pi_pFileName )
{
	assert( pi_pFileName != NULL );
	assert( _GetNetworkMgr() != NULL );

	int		l_nTotalServer = 0;
	BYTE	l_nServerIP[4];
	char	l_pServerName[32];
	
	// Gate Server
	l_nServerIP[0] = GetPrivateProfileInt( "MASTER_SERVER", "SERVER_IP1", -1, pi_pFileName );
	l_nServerIP[1] = GetPrivateProfileInt( "MASTER_SERVER", "SERVER_IP2", -1, pi_pFileName );
	l_nServerIP[2] = GetPrivateProfileInt( "MASTER_SERVER", "SERVER_IP3", -1, pi_pFileName );
	l_nServerIP[3] = GetPrivateProfileInt( "MASTER_SERVER", "SERVER_IP4", -1, pi_pFileName );
	_GetNetworkMgr()->SetServerIP( GST_GATE, *( DWORD * )l_nServerIP );
	_GetNetworkMgr()->SetServerPort( GST_GATE, GetPrivateProfileInt( "MASTER_SERVER", "SERVER_PORT", -1, pi_pFileName ) );
	GetPrivateProfileString( "MASTER_SERVER", "SERVER_NAME", "NULL", l_pServerName, 32, pi_pFileName );
	_GetNetworkMgr()->SetServerName( GST_GATE, l_pServerName );

	return TRUE;
}
