////////////////////////////////////////////////////////////////////////////////
//
// CItemDataMgr Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../GameObject/Manager/CCharacterMgr.h"
#include "../GameObject/Manager/CGroundItemManager.h"
#include "CCharacterDataMgr.h"
#include "CItemDataMgr.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////       CItemDataMgr         //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CItemDataMgr::CItemDataMgr()
{
	Init();
}

CItemDataMgr::~CItemDataMgr()
{
	Destroy();
}

void
CItemDataMgr::Init( void )
{
	ZeroMemory( m_pItemWorkData_ItemTypeStartDTIndex, sizeof( DWORD ) * MAX_ITEM_TYPE );
	ZeroMemory( m_pItemWorkDataNumByItemType, sizeof( DWORD ) * MAX_ITEM_TYPE );

	ZeroMemory( m_listItemDescription, sizeof( CItemDescription * ) * MAX_ITEM_TYPE );
	ZeroMemory( m_dwTotalItemDescriptionNum, sizeof( DWORD ) * MAX_ITEM_TYPE );

	m_listDungeonDescription		= NULL;
	m_dwTotalDungeonDescriptionNum	= 0;
}

BOOL
CItemDataMgr::Create( void )
{
	return TRUE;
}

BOOL
CItemDataMgr::Destroy( void )
{
	for( int i = 0; i < MAX_ITEM_TYPE; ++i )
		delete [] m_listItemDescription[i];

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CItemDataMgr::LoadData( void )
{
	CDataFile l_fileItemData( "./DataTable/Item.edf" );

	CDataString * l_pSourceData = l_fileItemData.GetSourceData();
	if( !l_pSourceData )
		return FALSE;
	if( !l_pSourceData->GetString() )
		return FALSE;

	// data read
	for( int i = 0; i < MAX_ITEM_TYPE; ++i )
		m_listData[i].LoadData( l_pSourceData );

	l_pSourceData->Read( m_pItemWorkData_ItemTypeStartDTIndex, sizeof( DWORD ), MAX_ITEM_TYPE );
	l_pSourceData->Read( m_pItemWorkDataNumByItemType, sizeof( DWORD ), MAX_ITEM_TYPE );

	LoadData_ItemDescription( l_pSourceData );
	LoadData_DungeonDescription( l_pSourceData );

	UNIT_FRAME_DATA * pData = ( UNIT_FRAME_DATA * )GetData( IEPT_UNIT, 0 );

	return TRUE;
}

BOOL
CItemDataMgr::LoadData_ItemDescription( CDataString * l_pSourceData )
{
	CItemDescription * l_pItemDesc = NULL;

	for( int i = 0; i < MAX_ITEM_TYPE; ++i )
	{
		l_pSourceData->Read( &m_dwTotalItemDescriptionNum[i], sizeof( DWORD ), 1 );
		if( m_dwTotalItemDescriptionNum[i] <= 0 )
			continue;

		m_listItemDescription[i] = new CItemDescription[ m_dwTotalItemDescriptionNum[i] ];

		for( int j = 0; j < m_dwTotalItemDescriptionNum[i]; ++j )
		{
			l_pItemDesc = &m_listItemDescription[i][j];
			if( !l_pItemDesc )
				continue;

			l_pSourceData->Read( &l_pItemDesc->m_dwDTIndex, sizeof( DWORD ), 1 );
			l_pSourceData->Read( &l_pItemDesc->m_dwDTCode, sizeof( DWORD ), 1 );

			for( int k = 0; k < MAX_LANGUAGE_TYPE; ++k )
			{
				l_pSourceData->Read( &l_pItemDesc->m_nDescriptionLength[k], sizeof( int ), 1 );
				l_pItemDesc->m_pDescription[k] = new char[ l_pItemDesc->m_nDescriptionLength[k] ];
				l_pSourceData->Read( l_pItemDesc->m_pDescription[k], l_pItemDesc->m_nDescriptionLength[k], 1 );
			}
		}
	}

	return TRUE;
}

BOOL
CItemDataMgr::LoadData_DungeonDescription( CDataString * l_pSourceData )
{
	CDungeonDescription * l_pDungeonDesc = NULL;

	l_pSourceData->Read( &m_dwTotalDungeonDescriptionNum, sizeof( DWORD ), 1 );
	if( m_dwTotalDungeonDescriptionNum <= 0 )
		return FALSE;

	m_listDungeonDescription = new CDungeonDescription[ m_dwTotalDungeonDescriptionNum ];

	for( int i = 0; i < m_dwTotalDungeonDescriptionNum; ++i )
	{
		l_pDungeonDesc = &m_listDungeonDescription[i];
		if( !l_pDungeonDesc )
			continue;

		l_pSourceData->Read( &l_pDungeonDesc->m_dwDTIndex, sizeof( DWORD ), 1 );
		l_pSourceData->Read( &l_pDungeonDesc->m_dwDTCode, sizeof( DWORD ), 1 );

		for( int j = 0; j < CDungeonDescription::MAX_DDT_TYPE; ++j )
		{
			for( int k = 0; k < MAX_LANGUAGE_TYPE; ++k )
			{
				l_pSourceData->Read( &l_pDungeonDesc->m_nDescriptionLength[j][k], sizeof( int ), 1 );
				l_pDungeonDesc->m_pDescription[j][k] = new char[ l_pDungeonDesc->m_nDescriptionLength[j][k] ];
				l_pSourceData->Read( l_pDungeonDesc->m_pDescription[j][k], l_pDungeonDesc->m_nDescriptionLength[j][k], 1 );
			}
		}
	}

	return TRUE;
}

BOOL
CItemDataMgr::UnloadData( BYTE pi_dwItemTypeID )
{
	if( ( pi_dwItemTypeID < 0 ) || ( pi_dwItemTypeID >= MAX_ITEM_TYPE ) )
		return FALSE;

	m_listData[pi_dwItemTypeID].UnloadData();

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

BASE_DATA *
CItemDataMgr::GetData( DWORD pi_dwItemTypeID, DWORD pi_dwIndex )
{
	if( ( pi_dwItemTypeID < 0 ) || ( pi_dwItemTypeID >= MAX_ITEM_TYPE ) )
		return NULL;

	return m_listData[pi_dwItemTypeID].GetData( pi_dwIndex );
}


BOOL
CItemDataMgr::GetData( DWORD pi_dwItemTypeID, DWORD pi_dwIndex, CBoardItem & pio_cItem,
					   BOOL pi_bIsLoadResource, BYTE pi_byRaceCode )
{
	return GetData( pi_dwItemTypeID, pi_dwIndex, &pio_cItem, pi_bIsLoadResource, pi_byRaceCode );
}

BOOL
CItemDataMgr::GetData( DWORD pi_dwItemTypeID, DWORD pi_dwIndex, CBoardItem * pio_pItem,
					   BOOL pi_bIsLoadResource, BYTE pi_byRaceCode )
{
	if( ( pi_dwItemTypeID < 0 ) || ( pi_dwItemTypeID >= MAX_ITEM_TYPE ) )
		return NULL;

	if( !pio_pItem )
		return NULL;

	ITEM_DATA * l_pData = ( ITEM_DATA * )m_listData[pi_dwItemTypeID].GetData( pi_dwIndex );
	if( !l_pData )
		return FALSE;

	pio_pItem->SetIndex( l_pData->GetDTIndex() );
	pio_pItem->SetName( l_pData->GetName() );

	pio_pItem->SetData( l_pData );

	if( pio_pItem->IsSplitable() )
	{
		pio_pItem->SetCurMaxDurability( 0 );
		pio_pItem->SetDurability( 0 );
	}
	else
	{
		pio_pItem->SetCurMaxDurability( pio_pItem->GetOrgMaxDurability() );
		pio_pItem->SetDurability( pio_pItem->GetCurMaxDurability() );
		pio_pItem->SetQuantity( 1 );
	}

	if( pio_pItem->IsEquipmentItem() )
		pio_pItem->SetMaxSocketNum( ( ( EQUIPMENT_DATA * )l_pData )->GetUpgradeSocketNum() );

/*
	if( pi_bIsLoadResource )
	{
		DWORD l_dwItemDTIndex = l_pData->GetMeshID();
		if( pi_byRaceCode != 0xFF )
		{
			if(	( ( l_dwItemDTIndex & 0x000F0000 ) == 0x00000000 ) &&	// 规绢备老 版快
				( ( l_dwItemDTIndex & 0x0000FF00 ) != 0x00000700 ) )
				l_dwItemDTIndex = ( l_dwItemDTIndex & 0xFF0FFFFF ) | ( pi_byRaceCode << 20 );
		}
		else
		{
			pio_pItem->SetItemTypeID( ITI_GROUND );
		}

		pio_pItem->SetBone( l_dwItemDTIndex );
		pio_pItem->SetMesh( l_dwItemDTIndex );
	}
*/

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

BASE_DATA *
CItemDataMgr::GetDataByDTCode( DWORD pi_dwItemTypeID, DWORD pi_dwDTCode )
{
	if( ( pi_dwItemTypeID < 0 ) || ( pi_dwItemTypeID >= MAX_ITEM_TYPE ) )
		return NULL;

	ITEM_DATA * l_pItemData;
	for( int i = 0; i < m_listData[pi_dwItemTypeID].GetTotalNum(); ++i )
	{
		l_pItemData = ( ITEM_DATA * )m_listData[pi_dwItemTypeID].GetData( i );

		if( l_pItemData->GetDTCode() == pi_dwDTCode )
			return ( BASE_DATA * )l_pItemData;
	}

	return NULL;
}

BOOL
CItemDataMgr::GetDataByDTCode( DWORD pi_dwItemTypeID, DWORD pi_dwDTCode, CBoardItem & pio_cItem, BOOL pi_bIsLoadResource )
{
	return GetDataByDTCode( pi_dwItemTypeID, pi_dwDTCode, &pio_cItem, pi_bIsLoadResource );
}

BOOL
CItemDataMgr::GetDataByDTCode( DWORD pi_dwItemTypeID, DWORD pi_dwDTCode, CBoardItem * pio_pItem, BOOL pi_bIsLoadResource )
{
	if( ( pi_dwItemTypeID < 0 ) || ( pi_dwItemTypeID >= MAX_ITEM_TYPE ) )
		return NULL;

	if( !pio_pItem )
		return NULL;

	ITEM_DATA * l_pItemData;
	for( int i = 0; i < m_listData[pi_dwItemTypeID].GetTotalNum(); ++i )
	{
		l_pItemData = ( ITEM_DATA * )m_listData[pi_dwItemTypeID].GetData( i );

		if( l_pItemData->GetDTCode() == pi_dwDTCode )
		{
			pio_pItem->SetIndex( l_pItemData->GetDTIndex() );
			pio_pItem->SetName( l_pItemData->GetName() );

			pio_pItem->SetData( l_pItemData );

			if( pio_pItem->IsEquipmentItem() )
				pio_pItem->SetMaxSocketNum( ( ( EQUIPMENT_DATA * )l_pItemData )->GetUpgradeSocketNum() );

//			if( pi_bIsLoadResource )
//			{
//				pio_pItem->SetBone( l_pItemData->GetMeshID() );
//				pio_pItem->SetMesh( l_pItemData->GetMeshID() );
//			}

			return TRUE;
		}
	}

	return FALSE;
}

BASE_DATA *
CItemDataMgr::GetDataByMeshID( DWORD pi_dwItemTypeID, DWORD pi_dwMeshID )
{
	if( ( pi_dwItemTypeID < 0 ) || ( pi_dwItemTypeID >= MAX_ITEM_TYPE ) )
		return NULL;

	ITEM_DATA * l_pItemData;
	for( int i = 0; i < m_listData[pi_dwItemTypeID].GetTotalNum(); ++i )
	{
		l_pItemData = ( ITEM_DATA * )m_listData[pi_dwItemTypeID].GetData( i );

		if( l_pItemData->GetMeshID() == pi_dwMeshID )
			return ( BASE_DATA * )l_pItemData;
	}

	return NULL;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

char *
CItemDataMgr::GetItemDescription( DWORD pi_dwItemTypeID, DWORD pi_dwDTIndex )
{
	if( ( pi_dwItemTypeID < 0 ) || ( pi_dwItemTypeID >= MAX_ITEM_TYPE ) )
		return NULL;

	for( int i = 0; i < m_dwTotalItemDescriptionNum[pi_dwItemTypeID]; ++i )
	{
		if( m_listItemDescription[pi_dwItemTypeID][i].m_dwDTIndex == pi_dwDTIndex )
			return m_listItemDescription[pi_dwItemTypeID][i].m_pDescription[0];
	}

	return NULL;
}

char *
CItemDataMgr::GetDungeonDescription( DWORD pi_dwDTIndex, BYTE pi_byDescType )
{
	if( pi_byDescType >= CDungeonDescription::MAX_DDT_TYPE )
		return NULL;

	for( int i = 0; i < m_dwTotalDungeonDescriptionNum; ++i )
	{
		if( m_listDungeonDescription[i].m_dwDTIndex == pi_dwDTIndex )
			return m_listDungeonDescription[i].m_pDescription[pi_byDescType][0];
	}

	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

ITEM_WORK_DATA *
CItemDataMgr::GetItemWorkData( DWORD pi_dwItemTypeID, DWORD pi_dwIndex )
{
	if( ( pi_dwItemTypeID < 0 ) || ( pi_dwItemTypeID >= MAX_ITEM_TYPE ) )
		return NULL;

	return ( ITEM_WORK_DATA * )m_listData[IEPT_ITEM_WORK].GetData( GetItemWorkDataStartDTIndex( pi_dwItemTypeID ) + pi_dwIndex );
}

ITEM_WORK_DATA *
CItemDataMgr::GetItemWorkDataByMeshID( DWORD pi_dwMeshID )
{
	BYTE l_byItemType, l_byItemTypeID;

	l_byItemType = ( pi_dwMeshID & 0x000F0000 ) >> 16;
	
	if( l_byItemType == 0 )			// 规绢备
	{
		switch( ( pi_dwMeshID & 0x0000FF00 ) >> 8 )
		{
			case 0	: l_byItemTypeID = IEPT_HELMET;		break;
			case 2	: l_byItemTypeID = IEPT_UPPER_PART; break;
			case 3	: l_byItemTypeID = IEPT_LOWER_PART; break;
			case 4	: l_byItemTypeID = IEPT_GLOVES;		break;
			case 5	: l_byItemTypeID = IEPT_SHOES;		break;
			case 6	: l_byItemTypeID = IEPT_CLOAK;		break;
			case 7	: l_byItemTypeID = IEPT_SHIELD;		break;
			default : return NULL;
		}
	}
	else if( l_byItemType == 1 )	// 公扁
	{
		l_byItemTypeID = IEPT_WEAPON;
	}
	else if( l_byItemType == 2 )	// 藕券
	{
		switch( ( pi_dwMeshID & 0x0000FF00 ) >> 8 )
		{
			case 3	:
			case 4	: l_byItemTypeID = IEPT_BULLET; break;
			default : return NULL;
		}	
	}
	else
		return NULL;

	//--------------------------------------------------------------------------

	ITEM_WORK_DATA * l_pItemWorkData;
	DWORD i				= GetItemWorkDataStartDTIndex( l_byItemTypeID );
	DWORD l_dwMaxNum	= i + GetItemWorkDataNum( l_byItemTypeID );
	for( ; i < l_dwMaxNum; ++i )
	{
		l_pItemWorkData = ( ITEM_WORK_DATA * )m_listData[IEPT_ITEM_WORK].GetData( i );

		if( l_pItemWorkData->GetItemMeshID() == pi_dwMeshID )
			return l_pItemWorkData;
	}

	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

int
CItemDataMgr::GetTotalDataNum( DWORD pi_dwItemTypeID )
{
	if( ( pi_dwItemTypeID < 0 ) || ( pi_dwItemTypeID >= MAX_ITEM_TYPE ) )
		return -1;

	return m_listData[pi_dwItemTypeID].GetTotalNum();
}
