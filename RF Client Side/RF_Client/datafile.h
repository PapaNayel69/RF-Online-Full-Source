////////////////////////////////////////////////////////////////////////////////
//
// Load from Data File
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __DATAFILE_H__
#define __DATAFILE_H__

#include "./DataTable/ServerTable/basefld.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define MAX_KEY_LENGTH	( 256 )
#define HEADER_LENGTH	( 29 )

class CDataString
{
	DWORD	m_dwStringLength;
	DWORD	m_dwCurPos;
	char *	m_pString;

public :

	CDataString();
	~CDataString();

	DWORD	Read( void * pi_pBuffer, int pi_nSize, int pi_nReadCount );

	inline	void	SetStringLength( DWORD pi_dwLength ){ m_dwStringLength = pi_dwLength; }
	inline	DWORD	GetStringLength( void )				{ return m_dwStringLength; }
	inline	void	SetString( char * pi_pString )		{ m_pString = pi_pString; }
	inline	char *	GetString( void )					{ return m_pString; }
};

class CDataFile
{
protected :

	char		m_pHeader[HEADER_LENGTH];
	char		m_pKey[MAX_KEY_LENGTH];
	CDataString	m_cSourceData;

	HANDLE		m_hSourceFile;

public :

	CDataFile();
	CDataFile( char * pi_pFileName );
	~CDataFile();

			BOOL			LoadFile( char * pi_pFileName );

	inline	CDataString *	GetSourceData( void )	{ return &m_cSourceData; }

protected :

			BOOL	LoadFile_ReadFile( void );
			BOOL	LoadFile_ReadSection( char * po_pBuffer, int pi_nSize );

			BOOL	DecodeSourceString( void );
			BOOL	DecodeEncryptionKey( void );
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CDataRecord : public CRecordData
{
public :
	BOOL	LoadData( char * pi_pFileName );
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct HSD_FONT
{
	BYTE			m_byStyle;
	BYTE			m_bySize;
	DWORD			m_dwColor;
	DWORD			m_dwLinkID;

	HSD_FONT();
};

struct HSD_STRING
{
	HSD_FONT		m_stFont;
	WORD			m_wLength;
	char *			m_pString;

	HSD_STRING();
	~HSD_STRING();
};

struct HSD_BASE_DATA
{
	DWORD			m_dwID;

	HSD_STRING *	m_listString;
	BYTE			m_byTotalStringNum;

	HSD_BASE_DATA();
	~HSD_BASE_DATA();

			DWORD	GetStringLength( void );

	inline	BOOL	IsEqual( DWORD pi_dwID )	{ return ( m_dwID == pi_dwID ); }
};

struct HSD_HINT : public HSD_BASE_DATA
{
	BYTE			m_byLevelMin;
	BYTE			m_byLevelMax;
	BYTE			m_byMapNo;
	DWORD			m_dwActionID;
	DWORD			m_dwTime;
	BOOL			m_bIsDispaly;

	HSD_HINT();
};

struct HSD_HELP : public HSD_BASE_DATA
{
	DWORD			m_dwWindowID;
	DWORD			m_dwSectionID;

	HSD_HELP();
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CHintMgr
{
	HSD_HINT *	m_listHint;
	DWORD		m_dwTotalHintNum;
	DWORD		m_dwCurHint;

	BOOL		m_bFindNextHint;
	DWORD		m_dwDisplayStartTime;

	DWORD		MAX_HINT_DISPLAY_LENGTH;
	char *		m_pCopyBuffer;

	BOOL		m_bIsRenderHint;

public :

	CHintMgr();
	~CHintMgr();

	BOOL	LoadData( void );

	BOOL	RenderHint( void );

protected :

	void	SetDisplayHint( void );

	DWORD	RenderHint_Single( HSD_HINT * pi_pHint, int & pio_nX, int & pio_nY );
	BOOL	RenderHint_Single_DrawBox( HSD_HINT * pi_pHint, int & pio_nX, int & pio_nY );
	DWORD	RenderHint_Single_DrawString( HSD_HINT * pi_pHint, int & pio_nX, int & pio_nY );

public :

			void	EnableRenderHint( BOOL pi_bFlag );
	inline	BOOL	IsRenderHint( void )			{ return m_bIsRenderHint; }
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CUIHelpMgr
{
	HSD_HELP *	m_listUIHelp;
	DWORD		m_dwTotalHelpNum;

public :

	CUIHelpMgr();
	~CUIHelpMgr();

	BOOL	LoadData( void );
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct BASE_DATA;

class CDataMgr
{
protected :

	DWORD		m_dwTotalNum;
	DWORD		m_dwRecordSize;

	char *		m_pData;

public :

	CDataMgr();
	~CDataMgr();

			BOOL		LoadData( char * pi_pFileName );
			BOOL		LoadData( CDataString * pi_pSourceData );
			void		UnloadData( void );

			BASE_DATA *	GetData( DWORD pi_dwIndex );
			void *		GetData_Void( DWORD pi_dwIndex );

	inline	char *		GetDataList( void )					{ return m_pData; }
	inline	DWORD		GetTotalNum( void )					{ return m_dwTotalNum; }
	inline	DWORD		GetRecordSize( void )				{ return m_dwRecordSize; }
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 게임 설정 관련 데이터 로드
BOOL	LoadGameSetting( char * pi_pFileName );

#endif // __DATAFILE_H__