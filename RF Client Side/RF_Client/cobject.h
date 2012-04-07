////////////////////////////////////////////////////////////////////////////////
//
// CObject Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "./Network/CNetworkMgr.h"

#include "BaseHeader.h"
#include "Global.h"

/*//////////////////////////////////////////////////////////////////////////////

[ CObject ]

 - CObject는 게임상에서 표현되는 모든 객체의 슈퍼 클래스이다.
   ( 하지만, 단순히 데이터와 관련된 객체는 CObject에서 파생되지 않는다. )

 - CObject는 물리적인 단위가 아닌 논리적인 단위이다. 즉, 하나의 메쉬, 본,
   텍스쳐등을 표현하는 것이 아니라 하나의 아이템, 캐릭터, UI 등을 표현한다.

 - m_dwObjectID도 당연히 논리적인 대상의 ID 이다. 하나의 메쉬나 본, 텍스쳐
   등의 실제적인 데이터의 ID 는 따로 취급한다.

//////////////////////////////////////////////////////////////////////////////*/

class CObject
{
	friend class CPlayerProc;
	friend class CUnitProc;

protected :

	BYTE		m_byObjectTypeID;				// 오브젝트의 타입 ID
	DWORD		m_dwIndex;						// 오브젝트의 실제적인 ID
	DWORD		m_dwServerIndex;				// 오브젝트의 서버 Index
	char		m_pName[MAX_NAME_LENGTH];		// 오브젝트명

public :

	CObject();
	virtual ~CObject();

	inline	BOOL	IsEmpty( void )						{ return ( m_dwIndex == ID_INVALID ); }
	inline	void	SetEmpty( void )					{ m_dwIndex = ID_INVALID; }

	virtual	BOOL	IsEqual( BYTE pi_byObjectTypeID, DWORD pi_dwIndex );
	inline	BOOL	IsEqualIndex( DWORD pi_dwIndex )	{ return ( m_dwIndex == pi_dwIndex ); }

public :

	virtual	void	SetIndex( DWORD pi_dwIndex )		{ m_dwIndex = pi_dwIndex; }
	inline	DWORD	GetIndex( void )					{ return m_dwIndex; }
	inline	void	SetServerIndex( DWORD pi_dwIndex )	{ m_dwServerIndex = pi_dwIndex; }
	inline	DWORD	GetServerIndex( void )				{ return m_dwServerIndex; }

	inline	void	SetObjectTypeID( char pi_byTypeID )	{ m_byObjectTypeID = pi_byTypeID; }
	inline	char	GetObjectTypeID( void )				{ return m_byObjectTypeID; }

	virtual	void	SetName( char * pi_pName )			{ _strcpy( m_pName, pi_pName ); }
	inline	char *	GetName( void )						{ return m_pName; }
};

#endif // __OBJECT_H__