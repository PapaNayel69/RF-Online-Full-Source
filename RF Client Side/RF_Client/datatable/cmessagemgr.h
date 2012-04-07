#ifndef __MESSAGE_MGR_H__
#define __MESSAGE_MGR_H__

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Game Language Type
#define	MAX_LANGUAGE_TYPE	( 3 )
enum { GLT_KOREAN, GLT_ENGLISH, GLT_JAPANESS };

struct HSD_STRING;

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

class CGameMessage
{
	DWORD			m_dwID;

	HSD_STRING *	m_listString;
	DWORD			m_dwTotalStringNum;

public :

	CGameMessage();
	~CGameMessage();

	void			Init( void );

	int				GetString( char * po_pBuf );
	int				GetStringLength( void );

	HSD_STRING *	GetString_HSD( BYTE pi_byOrderNo );
	char *			GetString( BYTE pi_byOrderNo );

	inline	DWORD	GetID( void )				{ return m_dwID; }
	inline	DWORD	GetTotalStringNum( void )	{ return m_dwTotalStringNum; }
};

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

class CMessageMgr
{
	CGameMessage *	m_listMessage;
	DWORD			m_dwTotalMessageNum;

	BYTE			m_byLanguageType;

public :

	CMessageMgr();
	~CMessageMgr();

	void			Init( void );

	BOOL			LoadData( void );
	BOOL			DisplayMessage( DWORD pi_dwID );

	CGameMessage *	GetMessage( DWORD pi_dwID );
	HSD_STRING *	GetString_HSD( DWORD pi_dwID, BYTE pi_byOrderNo );
	char *			GetString( DWORD pi_dwID, BYTE pi_byOrderNo );
	int				GetString( char * po_pBuf, DWORD pi_dwID );

	inline	void	SetLanguageType( BYTE pi_byType )	{ m_byLanguageType = pi_byType; }
	inline	BYTE	GetLanguageType( void )				{ return m_byLanguageType; }
};

#endif // __MESSAGE_MGR_H__