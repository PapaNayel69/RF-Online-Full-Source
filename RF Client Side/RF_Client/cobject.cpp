////////////////////////////////////////////////////////////////////////////////
//
// CObject Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "CObject.h"

CObject::CObject()
{
	m_byObjectTypeID	= ID_INVALID;
	m_dwIndex			= ID_INVALID;
	m_dwServerIndex		= ID_INVALID;

	ZeroMemory( m_pName, MAX_NAME_LENGTH );
}

CObject::~CObject()
{
}

BOOL
CObject::IsEqual( BYTE pi_byObjectTypeID, DWORD pi_dwIndex )
{
	return ( ( pi_byObjectTypeID == m_byObjectTypeID ) && ( pi_dwIndex == m_dwIndex ) );
}