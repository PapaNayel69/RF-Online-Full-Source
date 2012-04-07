////////////////////////////////////////////////////////////////////////////////
//
// CBoard Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __BOARD_H__
#define __BOARD_H__

#include "../../../DefineMessage.h"

class CGUIWindow;
class CBoardObject;

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CBaseBoard
{
protected :

	BYTE			m_byBoardType;
	CGUIWindow *	m_uiWindow;

public :

	CBaseBoard();
	virtual	~CBaseBoard();

	virtual	void		Create( BYTE pi_byBoardType )		{ m_byBoardType = pi_byBoardType; }
	inline	BOOL		IsCreate( void )					{ return ( m_byBoardType != 0xFF ); }
	inline	BOOL		IsEqual( BYTE pi_byBoardType )		{ return ( m_byBoardType == pi_byBoardType ); }

	virtual	void		InitWindow( DWORD pi_dwWindowID, int pi_nPosX, int pi_nPosY );
			BOOL		IsEqualWindow( DWORD pi_dwWindowID );

	inline	void		SetWindow( CGUIWindow * pi_pWindow ) { m_uiWindow = pi_pWindow; }
	inline	CGUIWindow* GetWindow( void ) { return m_uiWindow; }

			BOOL		IsOpen( void );
	virtual	void		OpenWindow( void );
	virtual	void		CloseWindow( void );
			void		ToggleWindow( void );
			void		SetWindowPos( int pi_nPosX, int pi_nPosY );
			void		GetWindowPos( POINT & po_ptWindowPos );
			void		GetWindowRect( RECT & po_ptWindowRect );	

	virtual	void		FrameMove( void );
	virtual	LRESULT		MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CObjectPack
{
	friend class CObjectBoard;
	friend class CInvenBoard;
	friend class CAnimusBoard;
	friend class CSFBoard;

private :

	CBoardObject **	m_listSocket;
	BYTE			m_byMaxSocketNum;			// 소켓의 최대 갯수
	BYTE			m_byUsingSocketNum;			// 현재 사용중인 소켓의 총 갯수

	BOOL			m_bIsExist;

public :

	CObjectPack();
	virtual ~CObjectPack();

			void			Create( BYTE pi_byMaxSocketNum );
			void			Clear( void );

			BOOL			Add( BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject );
			BOOL			Remove( BYTE pi_byOrgSocketNo );
			BOOL			Empty( BYTE pi_byOrgSocketNo );
			void			EmptyAll( void );

			CBoardObject *	GetObject( BYTE pi_byOrgSocketNo );
			CBoardObject *	GetObjectByIndex( DWORD pi_dwIndex );
			BYTE			GetObjectPosition( DWORD pi_dwIndex );

			BYTE			FindEmptySocket( void );
			BYTE			GetEmptySocketNum( void );

	inline	BYTE			GetMaxSocketNum( void )		{ return m_byMaxSocketNum; }

	inline	BOOL			IsExist( void )				{ return m_bIsExist; }
};

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

class CObjectBoard : public CBaseBoard
{
protected :

	CObjectPack *	m_listItemPack;

	BYTE			m_byMaxPackNum;
	BYTE			m_byActivePackNo;

public :

	CObjectBoard();
	virtual ~CObjectBoard();

			void	Create( BYTE pi_byBoardType, BYTE pi_byMaxPackNum, BYTE pi_byMaxSocketNum );
			void	Clear( void );

	virtual	BOOL	AddPack( BYTE pi_byPackNo );

			BOOL	Add( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject );
			BOOL	Remove( BYTE pi_byPackNo, BYTE pi_bySocketNo, BYTE pi_byQuantity = 0xFF );
			BOOL	RemoveByIndex( DWORD pi_dwOrgIndex, BYTE pi_byQuantity = 0xFF );

			BOOL	UI_AddRequest( BYTE pi_byPackNo, BYTE pi_bySocketNo );
			void	UI_AddResult( BOOL pi_bIsSuccessed );
			BOOL	UI_RemoveRequest( BYTE pi_byPackNo, BYTE pi_bySocketNo, BYTE pi_byQuantity );
			void	UI_RemoveResult( void );
			BOOL	UI_SwapRequest( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo );
			void	UI_SwapResult( BOOL pi_bIsSuccessed );
			BOOL	UI_UseRequest( BYTE pi_byPackNo, BYTE pi_bySocketNo );
			void	UI_UseResult( BOOL pi_bIsSuccessed );

protected :

	virtual	BOOL	Add_ProcessExistObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject ) = 0;
	virtual	BOOL	Add_ProcessNewObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject ) = 0;

			BOOL	Remove_Common( BYTE pi_byPackNo, BYTE pi_bySocketNo, BYTE pi_byQuantity );
	virtual	BOOL	Remove_ProcessRemoveQuantity( CBoardObject * pi_pOrgObject, BYTE pi_byQuantity ) = 0;
	virtual	BOOL	Remove_ProcessRemoveObject( BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardObject * pi_pOrgObject ) = 0;

			BOOL	UI_CheckBeforeAdd( void );
			BOOL	UI_CheckBeforeAdd_CheckAvatar( void );
	virtual	BOOL	UI_CheckBeforeAdd_CheckEmptySocket( void );
	virtual	BOOL	UI_CheckBeforeAdd_CheckHoverObject( void );
	virtual	BOOL	UI_CheckBeforeAdd_CheckTargetObject( void );
	virtual	BOOL	UI_AddRequest_ProcessObject( void ) = 0;
	virtual	void	UI_AddResult_ProcessObject( void ) = 0;

			BOOL	UI_CheckBeforeRemove( BYTE pi_byQuantity );
	virtual	BOOL	UI_CheckBeforeRemove_CheckHoverObject( void );
	virtual	BOOL	UI_CheckBeforeRemove_CheckOriginalObject( BYTE pi_byQuantity );
	virtual	BOOL	UI_RemoveRequest_ProcessObject( BYTE pi_byQuantity ) = 0;

	virtual	BOOL	UI_SwapRequest_ProcessObject( void ) = 0;
	virtual	void	UI_SwapResult_ProcessObject( void ) = 0;

			BOOL	UI_CheckBeforeUse( void );
	virtual	BOOL	UI_CheckBeforeUse_CheckHoverObject( void );
	virtual	BOOL	UI_CheckBeforeUse_CheckOriginalObject( void );
	virtual	BOOL	UI_UseRequest_ProcessObject( void ) = 0;
	virtual	void	UI_UseResult_ProcessObject( void ) = 0;

public :

	virtual	void	FrameMove( void );
	virtual	LRESULT	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	CBoardObject *	GetObject( BYTE pi_byPackNo, BYTE pi_bySocketNo );
	CBoardObject *	GetObjectByIndex( DWORD pi_dwIndex );
			BOOL	GetObjectPosition( BYTE & po_byPackNo, BYTE & po_bySocketNo, DWORD pi_dwIndex );
			BOOL	IsExistObject( CBoardObject * pi_pObject );

			BOOL	FindEmptySocket( BYTE & po_byPackNo, BYTE & po_bySocketNo );
			BYTE	FindEmptySocket( BYTE pi_byPackNo );
			BYTE	GetEmptySocketNum( void );

	inline	BYTE	GetMaxPackNum( void )			{ return m_byMaxPackNum; }
	inline	BYTE	GetActivePackNo( void )			{ return m_byActivePackNo; }
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __BOARD_H__
