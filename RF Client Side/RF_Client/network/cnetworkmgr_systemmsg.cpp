////////////////////////////////////////////////////////////////////////////
//
// CNetworkMgr Class Implementation
//
////////////////////////////////////////////////////////////////////////////

#include "../GameObject/Manager/CCharacterMgr.h"
#include "../GameObject/Board/Common/CBoardManager.h"

#include "../CLand.h"
#include "../CCamera.h"

#include "../UserInterface/GameWindow/EtcWin/GUIMapLoadingWindow.h"
#include "../UserInterface/GUIController/GUIMessagebox.h"

extern BOOL  g_bWriteTestLog;		// 테스트 코드
extern DWORD g_bWriteTestLogCount;	// 테스트 코드

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 아바타의 소지품 목록 등의 요청
BOOL
CNetworkMgr::InitMsg_BaseDownloadRequest_clzo( void )
{
	// 메세지를 보냄
	BYTE l_nType[] = { init_msg, base_download_request_clzo };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 NULL,
									 0 );

	// 결과 처리
	if( !l_nResult )
		NetLogWrite( "CNetworkMgr::UIMsg_BaseDownloadRequest_clzo(%d) Failure : %d", NST_GAME_SERVER, l_nResult );

	/*
	//_movein_user_result_zone후에 아바타데이터를 다운받는다.
	//장착, 착용, 벨트, 링크..
	#define base_download_request_clzo 1
	struct _base_download_request_clzo {
	};*/

//	if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) UIMsg_BaseDownloadRequest_clzo", ++g_bWriteTestLogCount ); // 테스트 코드

	return l_nResult;
}

// 아바타의 소지품 목록 등의 요청에 대한 응답
void
CNetworkMgr::InitMsg_BaseDownloadResult_zocl( char * pi_pMsg )
{
	_base_download_result_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _base_download_result_zocl ) );

	if( l_sRecv.byRetCode != 0 )
		return;

	_GetLand()->SetMapIndex( l_sRecv.wMapIndex );			// 맵의 인덱스 번호
	_GetAvatar()->SetServerIndex( l_sRecv.wZoneIndex );		// 아바타의 Server 인덱스

	//--------------------------------------------------------------------------
	// 카메라 & 플레이어 위치 설정
	ShortPosToFloatPos( m_vecAvatarStartPos, l_sRecv.zPos );
	if( _GetAvatar()->GetObserverInfo()->m_bIsObserver && ( _GetAvatar()->GetObserverInfo()->m_byObserverMode == OMI_FLY ) )
		_GetCamera()->SetPosition( m_vecAvatarStartPos );
	else
		_GetCamera()->GetR3Camera()->SetPos( m_vecAvatarStartPos[0], m_vecAvatarStartPos[1], m_vecAvatarStartPos[2] );

	//--------------------------------------------------------------------------
	// 아바타의 갑옷
	int		i;
	CBoardItem		clsNewItem;
	m_dwItemIndexForFirstItemSetting = 0;
	for( i = 0; i < equip_fix_num; ++i )
	{
		if( l_sRecv.EquipList[i].sTableCode != -1 )
		{
			if( !clsNewItem.Create( IEPT_SERVER_TO_CLIENT[ l_sRecv.EquipList[i].sTableCode ], l_sRecv.EquipList[i].wItemIndex ) )
				continue;

			clsNewItem.SetIndex( m_dwItemIndexForFirstItemSetting );
			clsNewItem.SetServerIndex( m_dwItemIndexForFirstItemSetting );
			
			clsNewItem.SetQuantity( 1 );

			clsNewItem.InsertTalikFromServerFlag( l_sRecv.EquipList[i].dwUptInfo ); // item upgrade

			_GetEquipBoard()->Add( 0, CEPT_CLIENT_TO_UI[IEPT_SERVER_TO_CLIENT[i]], &clsNewItem );

			++m_dwItemIndexForFirstItemSetting;
		}
		else
		{
			_GetEquipBoard()->Remove( 0, CEPT_CLIENT_TO_UI[IEPT_SERVER_TO_CLIENT[i]], 1 );
		}
	}

	//--------------------------------------------------------------------------
	// 장신구, 탄환 설정
	static const BYTE ACCESSORY_SOCKET_INDEX[6] = { UIPT_AMULET_LEFT, UIPT_AMULET_RIGHT, UIPT_RING_LEFT,
													UIPT_RING_RIGHT, UIPT_BULLET_LEFT, UIPT_BULLET_RIGHT };
	BYTE l_bySocketNo;
	for( i = 0; i < embellish_fix_num; ++i )
	{
		if( l_sRecv.EmbellishList[i].sTableCode != -1 )
		{
			if( !clsNewItem.Create( IEPT_SERVER_TO_CLIENT[ l_sRecv.EmbellishList[i].sTableCode ], l_sRecv.EmbellishList[i].wItemIndex ) )
				continue;

			clsNewItem.SetIndex( m_dwItemIndexForFirstItemSetting );
			clsNewItem.SetServerIndex( m_dwItemIndexForFirstItemSetting );
			clsNewItem.SetDurability( l_sRecv.EmbellishList[i].wNum );
			clsNewItem.SetQuantity( 1 );

			l_bySocketNo = l_sRecv.EmbellishList[i].sClientIndex % 2;
			if( l_bySocketNo > 1 ) l_bySocketNo = 1;

			if( clsNewItem.IsEqualItemKind( IEPT_AMULET ) )
			{
				if( !_GetEquipBoard()->GetObject( 0, ACCESSORY_SOCKET_INDEX[l_bySocketNo] ) )
					_GetEquipBoard()->Add( 0, ACCESSORY_SOCKET_INDEX[l_bySocketNo], &clsNewItem );
				else
					_GetEquipBoard()->Add( 0, ACCESSORY_SOCKET_INDEX[!l_bySocketNo], &clsNewItem );
			}
			else if( clsNewItem.IsEqualItemKind( IEPT_RING ) )
			{
				if( !_GetEquipBoard()->GetObject( 0, ACCESSORY_SOCKET_INDEX[l_bySocketNo + 2] ) )
					_GetEquipBoard()->Add( 0, ACCESSORY_SOCKET_INDEX[l_bySocketNo + 2], &clsNewItem );
				else
					_GetEquipBoard()->Add( 0, ACCESSORY_SOCKET_INDEX[!l_bySocketNo + 2], &clsNewItem );
			}
			else if( clsNewItem.IsEqualItemKind( IEPT_BULLET ) )
			{
				if( !_GetEquipBoard()->GetObject( 0, ACCESSORY_SOCKET_INDEX[l_bySocketNo + 4] ) )
					_GetEquipBoard()->Add( 0, ACCESSORY_SOCKET_INDEX[l_bySocketNo + 4], &clsNewItem );
				else
					_GetEquipBoard()->Add( 0, ACCESSORY_SOCKET_INDEX[!l_bySocketNo + 4], &clsNewItem );
			}
			
			++m_dwItemIndexForFirstItemSetting;			
		}
	}


	//--------------------------------------------------------------------------
	// 포션 벨트 아이템 설정
	BOOL	l_bMustSearchAEmptySocket;

	for( i = 0; i < potion_belt_num; ++i )
	{
		if( l_sRecv.BeltList[i].zItemIndex != -1 )
		{
			if( !clsNewItem.Create( IEPT_POTION, l_sRecv.BeltList[i].zItemIndex ) )
				continue;

			clsNewItem.SetIndex( m_dwItemIndexForFirstItemSetting );
			clsNewItem.SetServerIndex( m_dwItemIndexForFirstItemSetting );			
			clsNewItem.SetQuantity( l_sRecv.BeltList[i].byNum );

			l_bMustSearchAEmptySocket = FALSE;
			if( ( l_sRecv.BeltList[i].sClientIndex < 0 ) ||
				( l_sRecv.BeltList[i].sClientIndex >= MAX_ITEM_BELT_SOCKET ) )
			{
				l_bMustSearchAEmptySocket = TRUE;
			}
			else
			{
				if( _GetBeltBoard()->GetObject( 0, l_sRecv.BeltList[i].sClientIndex ) )
					l_bMustSearchAEmptySocket = TRUE;
			}

			if( l_bMustSearchAEmptySocket )
				l_sRecv.BeltList[i].sClientIndex = _GetBeltBoard()->FindEmptySocket();

			_GetBeltBoard()->Add( 0, l_sRecv.BeltList[i].sClientIndex, &clsNewItem );
			
			++m_dwItemIndexForFirstItemSetting;
		}
	}

	//--------------------------------------------------------------------------
	// 스킬/포스 벨트 설정
	CSFItem *		pSFItem;
	CAnimusItem *	pAnimusItem;
	for( i = 0; i < sf_linker_num; ++i )
	{
		pSFItem		= NULL;
		pAnimusItem	= NULL;

		if( l_sRecv.LinkerList[i].byEffectCode == CAT_SKILL || l_sRecv.LinkerList[i].byEffectCode == CAT_CLASS_SKILL )
			pSFItem = _GetSkillBoard()->GetSFByDTIndex( l_sRecv.LinkerList[i].wItemIndex );
		else if( l_sRecv.LinkerList[i].byEffectCode == CAT_FORCE )
			pSFItem = _GetForceBoard()->GetSFByDTIndex( l_sRecv.LinkerList[i].wItemIndex );
		else if( l_sRecv.LinkerList[i].byEffectCode == link_code_animus )
			pAnimusItem	= _GetAnimusBoard()->GetAnimusByDTIndex( l_sRecv.LinkerList[i].wItemIndex );
		else
			continue;

		if( pSFItem || pAnimusItem )
		{
			l_bMustSearchAEmptySocket = FALSE;
			if( ( l_sRecv.LinkerList[i].sClientIndex < 0 ) ||
				( l_sRecv.LinkerList[i].sClientIndex >= MAX_SF_BELT_SOCKET ) )
				l_bMustSearchAEmptySocket = TRUE;
			else if( _GetLinkBoard()->GetObject( 0, l_sRecv.LinkerList[i].sClientIndex ) )
				l_bMustSearchAEmptySocket = TRUE;

			if( l_bMustSearchAEmptySocket )
			{
				l_sRecv.LinkerList[i].sClientIndex = _GetLinkBoard()->FindEmptySocket();
				if( l_sRecv.LinkerList[i].sClientIndex == 0xFF )
					continue;
			}

			if( pSFItem )
				_GetLinkBoard()->Add( 0, l_sRecv.LinkerList[i].sClientIndex, pSFItem );
			else
				_GetLinkBoard()->Add( 0, l_sRecv.LinkerList[i].sClientIndex, pAnimusItem );
		}
	}

	//--------------------------------------------------------------------------
	// 기타 수치 설정
	_GetAvatar()->SetExp( ( float )l_sRecv.wExpRate / 100.0f );
	_GetAvatar()->SetMaxHP( l_sRecv.wMaxHP );
	_GetAvatar()->SetHP( l_sRecv.wHP );
	_GetAvatar()->SetMaxFP( l_sRecv.wMaxFP );
	_GetAvatar()->SetFP( l_sRecv.wFP );
	_GetAvatar()->SetMaxSP( l_sRecv.wMaxSP );
	_GetAvatar()->SetSP( l_sRecv.wSP );
	for( i = 0; i < class_history_num; ++i )
	{
		if( l_sRecv.wClassHistory[i] != 0xFFFF )
			_GetAvatar()->SetClassDTIndex( i + 1, l_sRecv.wClassHistory[i] );
	}

	// 속도 설정
	DT_CPInfo * l_pData = ( DT_CPInfo * )_GetCharDataMgr()->GetCharacter( CTI_PLAYER,
																		  _GetAvatar()->GetDTIndex() );
	if( l_pData )
	{
		_GetAvatar()->SetData( l_pData );

		//_GetAvatar()->SetSpeed( CMM_MOVE_WALK, CHAR_SPEED_DEFAULT * ( l_pData->m_fMoveWalkRate * 0.95f ) );
		//_GetAvatar()->SetSpeed( CMM_MOVE_RUN, CHAR_SPEED_DEFAULT * ( l_pData->m_fMoveRunRate * 0.95f ) );
		_GetAvatar()->SetSpeed( CMM_MOVE_WALK, CHAR_SPEED_DEFAULT * l_pData->m_fMoveWalkRate );
		_GetAvatar()->SetSpeed( CMM_MOVE_RUN, CHAR_SPEED_DEFAULT * l_pData->m_fMoveRunRate );
	}

	//--------------------------------------------------------------------------
	// Inventory 내용 요청
	InitMsg_InvenDownloadRequest_clzo();

	/*
	//저장 아이템 시리얼 부여순서..
	//1. Equip
	//2. Embellish
	//3. Belt
	//4. Inven
	//5. Force
	//6. Animus
	#define base_download_result_zocl 6
	struct _base_download_result_zocl {

		BYTE byRetCode;	//0이면 성공..

		WORD wZoneIndex;	//현재 존상에서의 인덱스
		WORD wExpRate;	
		WORD wClassHistory[class_history_num];	//과거 class 기록
		WORD wHP;		
		WORD wSP;		
		WORD wFP;		
		WORD wMaxHP;			
		WORD wMaxSP;			
		WORD wMaxFP;			
		WORD wMapIndex;		//맵인덱스	
		short zPos[3];		//위치

		struct _list_equip{
			char sTableCode;	//table code	.. 빈 슬롯이면 -1
			WORD wItemIndex;	//item index
			DWORD dwUptInfo;//업그레이드정보..오른쪽부터 4비트씩 업그레이드정보.. F가 나올때까지업글레벨을 ++한다.
		}; 

		_list_equip EquipList[equip_fix_num];

		struct _list_embellish{
			char sTableCode;	//table code	.. 빈 슬롯이면 -1
			WORD wItemIndex;	//item index
			WORD wNum;		//화살일경우 남은갯수
			char sClientIndex;	
		};
		
		_list_embellish EmbellishList[embellish_fix_num];	

		struct _list_belt{
			short zItemIndex;	//item index 빈거면 -1 
			BYTE byNum;		//갯수
			char sClientIndex;	
		};
		
		_list_belt BeltList[potion_belt_num];

		struct _list_linker{
			BYTE byEffectCode;	
			WORD wItemIndex;	//item index
			char sClientIndex;	
		};

		_list_linker LinkerList[sf_linker_num];

		int size(){
			if(byRetCode != 0)
				return sizeof(byRetCode);
			return sizeof(*this);
		}
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Avatar의 인벤토리 내용을 서버에 요청한다.
// 게임 접속시에 한번만 사용하면 된다.
BOOL
CNetworkMgr::InitMsg_InvenDownloadRequest_clzo( void )
{
	// 메세지를 보냄
	BYTE l_nType[] = { init_msg, inven_download_request_clzo };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 NULL,
									 0 );

	// 결과 처리
	if( !l_nResult )
		NetLogWrite( "CNetworkMgr::SystemMsg_StorageListRequest_clzo(%d) Failure : %d", NST_GAME_SERVER, l_nResult );

	/*
	//_movein_user_result_zone후에 아바타데이터를 다운받는다.
	//인벤토리..
	#define inven_download_request_clzo 7//storage_list_request_clzo 7
	struct _inven_download_request_clzo {//_storage_list_request_clzo{
	};*/

	if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) SystemMsg_StorageListRequest_clzo", ++g_bWriteTestLogCount ); // 테스트 코드

	return l_nResult;
}

void
CNetworkMgr::InitMsg_InvenDownloadResult_zocl( char * pi_pMsg )
{
	_inven_download_result_zocl * pRecvMsg = reinterpret_cast< _inven_download_result_zocl * >( pi_pMsg );

	if( pRecvMsg->byRetCode != 0 )
		return;

	int		i, j;
	BYTE	byPackIndex, bySocketIndex;

	// Pack의 활성화
	for( i = 0; i < pRecvMsg->byBagNum; ++i )
		_GetInventoryBoard()->AddPack( i );

	// Item 설정 => 첫번째 루프에서는 sClientIndex값이 제대로 되어있는 아이템을 세팅한다.
	//           => 두번째 루프에서는 sClientIndex값이 이상한 아이템을 빈칸에 세팅한다.
	bool	IS_SETED[bag_storage_num];	// 아이템이 제대로 세팅됐는지 체크
	DWORD	dwItemIndex;				// 루프를 2번 돌기때문에 따로 변수가 필요하다.
	ZeroMemory( IS_SETED, sizeof( bool ) * bag_storage_num );
	for( i = 0; i < 2; ++i )
	{
		dwItemIndex = m_dwItemIndexForFirstItemSetting;

		for( j = 0; j < pRecvMsg->bySlotNum; ++j, ++dwItemIndex )
		{
			// 이미 세팅해 놓은 아이템이라면...
			if( IS_SETED[j] )
				continue;

			// create item
			CBoardItem		clsNewItem;
			ITEM_DATA *		pItemData = NULL;
			BYTE			byItemKind = IEPT_SERVER_TO_CLIENT[ pRecvMsg->ItemSlotInfo[j].byTableCode ];
			if( !clsNewItem.Create( byItemKind, pRecvMsg->ItemSlotInfo[j].wItemIndex ) )
				continue;

			// set property
			clsNewItem.SetIndex( dwItemIndex );
			clsNewItem.SetServerIndex( dwItemIndex );

			// animus item 일 경우 dwDurPoint 에는 경험치가 들어있음 => CBoardItem 에서는 SetCurMaxDurability()으로 저장을 했다가 CAnimusItem 으로 이동시 경험치로 변경시킨다.
			if( byItemKind == IEPT_ANIMUS )
				clsNewItem.SetCurMaxDurability( pRecvMsg->ItemSlotInfo[j].dwDurPoint );
			else if( clsNewItem.IsSplitable() )
				clsNewItem.SetQuantity( pRecvMsg->ItemSlotInfo[j].dwDurPoint );
			else
				clsNewItem.SetDurability( pRecvMsg->ItemSlotInfo[j].dwDurPoint );

			// upgrade info
			// animus item 일 경우 hp/fp 가 있음 => CBoardItem 에서는 내구성으로 저장을 했다가 CAnimusItem 으로 이동시 HP/FP로 변경시킨다.
			// unit 일 경우에는 키와 연결된 슬롯 번호임.
			if( byItemKind == IEPT_UNIT )
				_GetUnitStorehouseBoard()->AddUnitByIndex( pRecvMsg->ItemSlotInfo[j].dwUptInfo, dwItemIndex );
			else if( byItemKind == IEPT_ANIMUS )
				clsNewItem.SetDurability( pRecvMsg->ItemSlotInfo[j].dwUptInfo );
			else
				clsNewItem.InsertTalikFromServerFlag( pRecvMsg->ItemSlotInfo[j].dwUptInfo );

			// set inventory
			if( i == 0 )
			{
				byPackIndex		= pRecvMsg->ItemSlotInfo[j].sClientIndex / MAX_INVENTORY_SOCKET;
				bySocketIndex	= pRecvMsg->ItemSlotInfo[j].sClientIndex % MAX_INVENTORY_SOCKET;
				if( ( byPackIndex < MAX_INVENTORY_PACK ) && ( bySocketIndex < MAX_INVENTORY_SOCKET ) )
				{
					if( !_GetInventoryBoard()->GetObject( byPackIndex, bySocketIndex ) )
					{
						if( _GetInventoryBoard()->Add( byPackIndex, bySocketIndex, &clsNewItem ) )
							IS_SETED[j] = true;
					}
				}
			}
			else
			{
				if( !_GetInventoryBoard()->FindEmptySocket( byPackIndex, bySocketIndex ) )
					continue;
				_GetInventoryBoard()->Add( byPackIndex, bySocketIndex, &clsNewItem );
			}
		}
	}

	m_dwItemIndexForFirstItemSetting = dwItemIndex;

	//--------------------------------------------------------------------------
	// 포스창의 아이템 요청
	InitMsg_ForceDownloadRequest_clzo();

	/*	
	#define inven_download_result_zocl 6
	struct _inven_download_result_zocl {

		BYTE byRetCode;	//0이면 성공..

		BYTE byBagNum;	
		BYTE bySlotNum;

		struct _list{
			//BYTE bySlotIndex;//0~99 del
			BYTE byTableCode; 
			WORD wItemIndex;
			WORD wSerial; 
			DWORD dwDurPoint;
			char sClientIndex;
			DWORD dwUptInfo;//add(4/8)..업그레이드정보
		}; 	
		
		_list ItemSlotInfo[bag_storage_num];

		int size(){
			if(byRetCode != 0)
				return sizeof(byRetCode);
			return sizeof(*this)-sizeof(_list)*(bag_storage_num-bySlotNum);
		}
	};*/
}

BOOL
CNetworkMgr::InitMsg_ExitSaveDataRequest_clzo( void )
{
	_exit_save_data_request_clzo stSendMsg;

	CBoardItem *	pBoardItem = NULL;
	int				i = 0, j = 0, nCnt = 0;

	//--------------------------------------------------------------------------

	struct UPLOAD_DATA
	{
		BYTE	m_byInvenNum;
		WORD	m_wInvenIndex[100];
		BYTE	m_byInvenSocketNo[100];

		BYTE	m_byItemBeltNum;
		WORD	m_wItemBeltIndex[4];
		BYTE	m_byItemBeltSocketNo[4];

		BYTE	m_bySFBeltNum;
		BYTE	m_wSFBeltType[8];
		BYTE	m_wSFBeltDTIndex[8];
		BYTE	m_bySFBeltSocketNo[8];

		BYTE	m_byAccessoryNum;
		WORD	m_wAccessoryIndex[6];
		BYTE	m_byAccessorySocketNo[6];

		UPLOAD_DATA() { ZeroMemory( this, sizeof( UPLOAD_DATA ) ); }
	};

	UPLOAD_DATA stUploadData;

	for( i = 0; i < MAX_INVENTORY_PACK; ++i )
	{
		for( j = 0; j < MAX_INVENTORY_SOCKET; ++j )
		{
			pBoardItem = _GetInventoryBoard()->GetItem( i, j );
			if( !pBoardItem )
				continue;

			stUploadData.m_wInvenIndex[stUploadData.m_byInvenNum]		= pBoardItem->GetIndex();
			stUploadData.m_byInvenSocketNo[stUploadData.m_byInvenNum]	= ( i * MAX_INVENTORY_SOCKET ) + j;
			++stUploadData.m_byInvenNum;
		}
	}

	for( i = 0; i < MAX_ITEM_BELT_SOCKET; ++i )
	{
		pBoardItem = _GetBeltBoard()->GetItem( i );
		if( !pBoardItem )
			continue;

		stUploadData.m_wItemBeltIndex[stUploadData.m_byItemBeltNum]		= pBoardItem->GetIndex();
		stUploadData.m_byItemBeltSocketNo[stUploadData.m_byItemBeltNum]	= i;
		++stUploadData.m_byItemBeltNum;
	}

	CBoardObject *	pBoardObject = NULL;
	CSFItem *		pSFItem = NULL;
	CAnimusItem *	pAnimusItem = NULL;
	for( i = 0; i < MAX_SF_BELT_SOCKET; ++i )
	{
		pBoardObject = _GetLinkBoard()->GetObject( 0, i );
		if( !pBoardObject )
			continue;

		if( pBoardObject->IsEqualItemType( BOT_SF ) )
		{
			pSFItem = static_cast< CSFItem * >( pBoardObject );

			stUploadData.m_wSFBeltType[stUploadData.m_bySFBeltNum]		= pSFItem->GetSFType();
			stUploadData.m_wSFBeltDTIndex[stUploadData.m_bySFBeltNum]	= pSFItem->GetDTIndex();
		}
		else if( pBoardObject->IsEqualItemType( BOT_ANIMUS ) )
		{
			pAnimusItem = static_cast< CAnimusItem * >( pBoardObject );

			stUploadData.m_wSFBeltType[stUploadData.m_bySFBeltNum]		= link_code_animus;
			stUploadData.m_wSFBeltDTIndex[stUploadData.m_bySFBeltNum]	= pAnimusItem->GetDTIndex();
		}

		stUploadData.m_bySFBeltSocketNo[stUploadData.m_bySFBeltNum]	= i;
		++stUploadData.m_bySFBeltNum;
	}

	static const BYTE ACCESSORY_SOCKET_INDEX[6] = { UIPT_AMULET_LEFT, UIPT_AMULET_RIGHT, UIPT_RING_LEFT,
													UIPT_RING_RIGHT, UIPT_BULLET_LEFT, UIPT_BULLET_RIGHT };
	for( i = 0; i < 6; ++i )
	{
		pBoardItem = _GetEquipBoard()->GetItem( ACCESSORY_SOCKET_INDEX[i] ); 
		if( !pBoardItem )
			continue;

		stUploadData.m_wAccessoryIndex[stUploadData.m_byAccessoryNum]		= pBoardItem->GetIndex();
		stUploadData.m_byAccessorySocketNo[stUploadData.m_byAccessoryNum]	= i;
		++stUploadData.m_byAccessoryNum;
	}

	//--------------------------------------------------------------------------

	stSendMsg.sData[nCnt++] = stUploadData.m_byInvenNum;
	for( i = 0; i < stUploadData.m_byInvenNum; ++i )
	{
		memcpy( &stSendMsg.sData[nCnt], &stUploadData.m_wInvenIndex[i], 2 );
		stSendMsg.sData[nCnt+2] = stUploadData.m_byInvenSocketNo[i];
		nCnt += 3;
	}

	stSendMsg.sData[nCnt++] = stUploadData.m_byItemBeltNum;
	for( i = 0; i < stUploadData.m_byItemBeltNum; ++i )
	{
		memcpy( &stSendMsg.sData[nCnt], &stUploadData.m_wItemBeltIndex[i], 2 );
		stSendMsg.sData[nCnt+2] = stUploadData.m_byItemBeltSocketNo[i];
		nCnt += 3;
	}

	stSendMsg.sData[nCnt] = stUploadData.m_bySFBeltNum;
	for( i = 0; i < stUploadData.m_bySFBeltNum; ++i )
	{
		stSendMsg.sData[++nCnt] = stUploadData.m_wSFBeltType[i];
		stSendMsg.sData[++nCnt] = stUploadData.m_wSFBeltDTIndex[i];
		stSendMsg.sData[++nCnt] = stUploadData.m_bySFBeltSocketNo[i];
	}

	stSendMsg.sData[(++nCnt)++] = stUploadData.m_byAccessoryNum;
	for( i = 0; i < stUploadData.m_byAccessoryNum; ++i )
	{
		memcpy( &stSendMsg.sData[nCnt], &stUploadData.m_wAccessoryIndex[i], 2 );
		stSendMsg.sData[nCnt+2] = stUploadData.m_byAccessorySocketNo[i];
		nCnt += 3;
	}

	stSendMsg.wDataSize = nCnt + 1;

	//--------------------------------------------------------------------------

	/*
	stSendMsg.sData[nCnt++] = _GetInventoryBoard()->GetItemNum();
	//stSendMsg.sData[nCnt] = _GetInventoryBoard()->GetItemNum();
	for( i = 0; i < MAX_INVENTORY_PACK; ++i )
	{
		for( j = 0; j < MAX_INVENTORY_SOCKET; ++j )
		{
			pBoardItem = _GetInventoryBoard()->GetItem( i, j );
			if( !pBoardItem )
				continue;
			if( pBoardItem->GetIndex() == ID_INVALID )
				continue;

			l_wIndex = pBoardItem->GetIndex();
			memcpy( &stSendMsg.sData[nCnt], &l_wIndex, 2 );
			stSendMsg.sData[nCnt+2] = ( i * MAX_INVENTORY_SOCKET ) + j;
			nCnt += 3;
			//stSendMsg.sData[++nCnt] = ( pBoardItem->GetIndex() & 0x0000FF00 ) >> 8;
			//stSendMsg.sData[++l_nCnt] = ( pBoardItem->GetIndex() & 0x000000FF );
			//stSendMsg.sData[++l_nCnt] = ( i * MAX_INVENTORY_SOCKET ) + j;
		}
	}

	//--------------------------------------------------------------------------

	stSendMsg.sData[l_nCnt++] = _GetItemBeltBoard()->GetItemNum();
	for( i = 0; i < MAX_ITEM_BELT_SOCKET; ++i )
	{
		pBoardItem = _GetItemBeltBoard()->GetItem( i );
		if( !pBoardItem )
			continue;
		if( pBoardItem->GetIndex() == ID_INVALID )
			continue;

		l_wIndex = pBoardItem->GetIndex();
		memcpy( &stSendMsg.sData[l_nCnt], &l_wIndex, 2 );
		stSendMsg.sData[l_nCnt + 2] = i;
		l_nCnt += 3;
		//stSendMsg.sData[++l_nCnt] = ( pBoardItem->GetIndex() & 0x0000FF00 ) >> 8;
		//stSendMsg.sData[++l_nCnt] = ( pBoardItem->GetIndex() & 0x000000FF );
		//stSendMsg.sData[++l_nCnt] = i;
	}

	//--------------------------------------------------------------------------

	stSendMsg.sData[l_nCnt] = _GetSFBeltBoard()->GetTotalSFNum();
	for( i = 0; i < MAX_SF_BELT_SOCKET; ++i )
	{
		l_pSFInfo = _GetSFBeltBoard()->GetSFByOrder( i );
		if( !l_pSFInfo )
			continue;
		if( l_pSFInfo->GetIndex() == ID_INVALID )
			continue;

		stSendMsg.sData[++l_nCnt] = l_pSFInfo->GetType();
		stSendMsg.sData[++l_nCnt] = l_pSFInfo->GetDTIndex();
		stSendMsg.sData[++l_nCnt] = i;
	}

	//--------------------------------------------------------------------------

	stSendMsg.wDataSize = l_nCnt + 1;
	*/

	// 메세지를 보냄
	BYTE l_nType[] = { init_msg, exit_save_data_request_clzo };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 ( char * )&stSendMsg,
									 stSendMsg.size() );

	// 결과 처리
	if( !l_nResult )
		NetLogWrite( "CNetworkMgr::InitMsg_ExitSaveDataRequest_clzo(%d) Failure : %d", NST_GAME_SERVER, l_nResult );

	/*
	#define exit_save_data_request_clzo 32
	struct _exit_save_data_request_clzo {	//upt(7/25)

		WORD wDataSize;

	//inven 저장수[1], { serial[2], client_index[1] }
	///belt 저장수[1], { serial[2], client_index[1] }
	///link 저장수[1], { effectcode[1], effectindex[1], client_index[1] }
	///embell 저장수[1], { serial[2], client_index[1] }

		char sData[0x0FFF];

		int size(){
			return sizeof(*this)-sizeof(char)*(0x0FFF-wDataSize);
		}			
	};*/

#ifdef _DEVELOPER_
	m_ClientLogFile.Write( "( %d ) InitMsg_ExitSaveDataRequest_clzo", ++g_bWriteTestLogCount ); // 테스트 코드
#endif

	return l_nResult;
}

void
CNetworkMgr::InitMsg_ExitSaveDataResult_zocl( char * pi_pMsg )
{
	_exit_save_data_result_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _exit_save_data_result_zocl ) );

	if( l_sRecv.byRetCode == 0 )
		m_byResultOfInventoryUpload = INVEN_UPLOAD_SUCCESS;
	else
		m_byResultOfInventoryUpload = INVEN_UPLOAD_FAILURE;

	/*	
	#define exit_save_data_result_zocl 31	//add(4/21)
	struct _exit_save_data_result_zocl {
		BYTE byRetCode;	//0;//성공, 1;//시리얼틀림 2;//이미인덱싱했음
	};*/

	//if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) InitMsg_ExitSaveDataResult_zocl", ++g_bWriteTestLogCount ); // 테스트 코드
}

// 각종 누적치의 변경 메세지
void
CNetworkMgr::InitMsg_StatInform_zocl( char * pi_pMsg )
{
	const _stat_inform_zocl * const pRecvMsg = reinterpret_cast< _stat_inform_zocl * >( pi_pMsg );

	// 무기
	if( pRecvMsg->byStatIndex >= _STAT_DB_BASE::DATAPOS_WPCNT && pRecvMsg->byStatIndex < _STAT_DB_BASE::DATAPOS_DEF )
		_GetAvatar()->SetExpertness( pRecvMsg->byStatIndex - _STAT_DB_BASE::DATAPOS_WPCNT, pRecvMsg->dwNewStat );
	// 방어
	else if( pRecvMsg->byStatIndex == _STAT_DB_BASE::DATAPOS_DEF )
		_GetAvatar()->SetExpertness( EVT_DEFENCE, pRecvMsg->dwNewStat );
	// 방패
	else if( pRecvMsg->byStatIndex == _STAT_DB_BASE::DATAPOS_SHLD )
		_GetAvatar()->SetExpertness( EVT_SHIELD, pRecvMsg->dwNewStat );
	// 제작
	else if( pRecvMsg->byStatIndex >= _STAT_DB_BASE::DATAPOS_MAKE && pRecvMsg->byStatIndex < _STAT_DB_BASE::DATAPOS_SPECIAL )
		_GetAvatar()->SetExpertness( pRecvMsg->byStatIndex - _STAT_DB_BASE::DATAPOS_MAKE + EVT_MAKE_WEAPON, pRecvMsg->dwNewStat );
	// 종족별 특수 기술
	else if( pRecvMsg->byStatIndex == _STAT_DB_BASE::DATAPOS_SPECIAL )
		_GetAvatar()->SetExpertness( EVT_RACE_SKILL, pRecvMsg->dwNewStat );
	// 스킬
	else if( ( pRecvMsg->byStatIndex >= _STAT_DB_BASE::DATAPOS_SKILL ) && ( pRecvMsg->byStatIndex < _STAT_DB_BASE::DATAPOS_SKILL + max_skill_num ) )
		_GetAvatar()->SetSFSuccessCount( CAT_SKILL, pRecvMsg->byStatIndex - _STAT_DB_BASE::DATAPOS_SKILL, pRecvMsg->dwNewStat );
	// 포스
	else if( ( pRecvMsg->byStatIndex >= _STAT_DB_BASE::DATAPOS_FORCE ) && ( pRecvMsg->byStatIndex < _STAT_DB_BASE::DATAPOS_FORCE + force_mastery_num ) )
		_GetForceBoard()->SetSuccessCountOfMastery( ( pRecvMsg->byStatIndex - _STAT_DB_BASE::DATAPOS_FORCE ),
													( pRecvMsg->byStatIndex - _STAT_DB_BASE::DATAPOS_FORCE ) % mastery_level_num,
													pRecvMsg->dwNewStat, FALSE );

	//_WriteLog( "3) stat inform => %d : %d", pRecvMsg->byStatIndex, l_sRecv.dwNewStat );

	/*
	struct _STAT_DB_BASE
	{
		enum {  DATAPOS_MONMEN = 0, 
				DATAPOS_WPCNT = DATAPOS_MONMEN+mon_mem_unit,
				DATAPOS_SUFF = DATAPOS_WPCNT+WP_CLASS_NUM,
				DATAPOS_SHLD = DATAPOS_SUFF+1,
				DATAPOS_SKILL = DATAPOS_SHLD+1,
				DATAPOS_FORCE = DATAPOS_SKILL+max_skill_num,
				DATAPOS_MAKE = DATAPOS_FORCE+force_mastery_num,
				DATAPOS_SPECIAL = DATAPOS_MAKE+MI_CLASS_NUM	};	


		DWORD	m_dbMonMemory[mon_mem_unit];//exit_update 시작포인터.. 변경시 포인터변경 작업해야함..
		DWORD	m_dwDamWpCnt[WP_CLASS_NUM];	
		DWORD	m_dwSufferDamCnt;				
		DWORD	m_dwShieldDamCnt;				
		DWORD	m_dwSkillCum[max_skill_num];
		DWORD	m_dwForceCum[force_mastery_num];
		DWORD	m_dwMakeCum[MI_CLASS_NUM];	//add
		DWORD	m_dwSpecialCum;				//add

		_STAT_DB_BASE(){ Init(); }

		void Init(){
			memset(this, 0, sizeof(*this));
		};
	};*/

	/*	
	#define stat_inform_zocl	34
	struct _stat_inform_zocl {
		BYTE byStatIndex;
		DWORD dwNewStat;

	//#define reward_case_std			0	//일반적인경우
	//#define reward_case_class			1	//전직
	//#define reward_case_quest			2	//퀘스트

		BYTE byReason;		//add(5/6)
	};*/
}

// 포스 아이템 누적치의 변경 메세지
void
CNetworkMgr::InitMsg_FcitemInform_zocl( char * pi_pMsg )
{
	_fcitem_inform_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _fcitem_inform_zocl ) );

	_GetAvatar()->SetSFSuccessCount( CAT_FORCE, l_sRecv.wItemSerial, l_sRecv.dwNewStat );

	/*
	#define fcitem_inform_zocl	35
	struct _fcitem_inform_zocl {	//add(12/12)
		WORD wItemSerial;
		DWORD dwNewStat;*/
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 스킬/포스의 계열별 마스터리를 서버에 요청한다.
// 게임 접속시에 한번만 사용하면 된다.
BOOL
CNetworkMgr::InitMsg_CumDownloadRequest_clzo( void )
{
	// 메세지를 보냄
	BYTE l_nType[] = { init_msg, cum_download_request_clzo };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 NULL,
									 0 );

	// 결과 처리
	if( !l_nResult )
		NetLogWrite( "CNetworkMgr::SystemMsg_SFCumRequest_clzo(%d) Failure : %d", NST_GAME_SERVER, l_nResult );

	/*	
	//각종 누적치..
	#define cum_download_request_clzo 9 //sf_cum_request_clzo 9
	struct _cum_download_request_clzo { //_sf_cum_request_clzo{
	};*/

	if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) SystemMsg_SFCumRequest_clzo", ++g_bWriteTestLogCount ); // 테스트 코드

	return l_nResult;
}

void
CNetworkMgr::InitMsg_CumDownloadResult_zocl( char * pi_pMsg )
{
	_cum_download_result_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, ( ( _cum_download_result_zocl * )pi_pMsg )->size() );

	if( l_sRecv.byRetCode != 0 )
		return;

	int i;

	//--------------------------------------------------------------------------
	// 숙련도

	//무기
	for( i = EVT_NEAR; i <= EVT_FAR; ++i )
		_GetAvatar()->SetExpertness( i, l_sRecv.Stat.m_dwDamWpCnt[i] );
	// 종족별 특수 기술 ( 유닛, 소환, 런쳐 )
	_GetAvatar()->SetExpertness( EVT_RACE_SKILL, l_sRecv.Stat.m_dwSpecialCum );
	// 방패
	_GetAvatar()->SetExpertness( EVT_SHIELD, l_sRecv.Stat.m_dwShieldCnt ); // i에 1을 더 더해서 EVT_FORCE는 그냥 넘어간다.
	// 방어
	_GetAvatar()->SetExpertness( EVT_DEFENCE, l_sRecv.Stat.m_dwDefenceCnt );
	// 제작
	for( i = EVT_MAKE_WEAPON; i <= EVT_MAKE_BULLET; ++i )
		_GetAvatar()->SetExpertness( i, l_sRecv.Stat.m_dwMakeCum[i-EVT_MAKE_WEAPON] );

	//--------------------------------------------------------------------------
	// 스킬 설정

	CSFItem * pSFItem;	
	for( i = 0; i < max_skill_num; ++i )
	{
		pSFItem = _GetSkillBoard()->GetSFByDTIndex( i );
		if( !pSFItem )
			continue;

		pSFItem->SetHave( TRUE );

		if( _GetAvatar()->IsUsableItem( pSFItem ) )
		{
			if( _GetLinkBoard()->IsExistObject( pSFItem ) ) 
				pSFItem->SetState( CSFItem::SFS_LINK );
			else
				pSFItem->SetState( CSFItem::SFS_ACTIVE );
		}
		else
		{
			pSFItem->SetState( CSFItem::SFS_DEACTIVE );
		}
	}

	// skill을 다 넣은 다음에 mastery를 설정해야된다.
	for( i = 0; i < max_skill_num; ++i )
		_GetAvatar()->SetSFSuccessCount( CAT_SKILL, i, l_sRecv.Stat.m_dwSkillCum[i] );

	//--------------------------------------------------------------------------
	// 포스 마스터리

	for( i = 0; i < MAX_FORCE_CLASS_TYPE*MAX_SF_STEP; ++i )
	{
		_GetForceBoard()->SetSuccessCountOfMastery( i, i % mastery_level_num,
													l_sRecv.Stat.m_dwForceCum[i], FALSE );
	}

	//--------------------------------------------------------------------------
	// 원석 가공창의 내용 세팅

	CBoardItem clsIngot;
	for( i = 0; i < l_sRecv.byLeftCuttingResNum; ++i )
	{
		if( !clsIngot.Create( IEPT_INGOT, l_sRecv.wleftResList[i] & 0x00FF ) )
			continue;

		clsIngot.SetIndex( i );
		clsIngot.SetServerIndex( i );
		clsIngot.SetDurability( 1 );
		clsIngot.SetQuantity( l_sRecv.wleftResList[i] >> 8 );

		_GetOreProcBoard()->Add( 0, i, &clsIngot );
	}

	//--------------------------------------------------------------------------
	// 종족별 특수 기술 리스트 요청

	if( !_GetAvatar()->IsAccretia() )
		InitMsg_SpecialDownloadRequest_clzo();
	else
		_GetShopTradeBoard()->StoreListRequest();

	/*
	#define cum_download_result_zocl 10
	struct _cum_download_result_zocl {
		BYTE byRetCode;	//0이면 성공..

		_STAT_DB_BASE Stat;	//upt(6/24)

		BYTE byLeftCuttingResNum;				
		WORD wleftResList[left_cutting_num];	//..하위바이트: res인덱스, 상위바이트: res갯수

		int size(){
			if(byRetCode != 0)
				return sizeof(byRetCode);
			return sizeof(*this)-sizeof(WORD)*(left_cutting_num-byLeftCuttingResNum);
		}
	};*/

	if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) SystemMsg_SFCumResult_zocl", ++g_bWriteTestLogCount ); // 테스트 코드
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 포스창의 내용을 서버에 요청한다.
BOOL
CNetworkMgr::InitMsg_ForceDownloadRequest_clzo( void )
{
	// 메세지를 보냄
	BYTE l_nType[] = { init_msg, force_download_request_clzo };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 NULL,
									 0 );

	// 결과 처리
	if( !l_nResult )
		NetLogWrite( "CNetworkMgr::SystemMsg_ForceDownloadRequest_clzo(%d) Failure : %d", NST_GAME_SERVER, l_nResult );

	/*
	//포스인벤내용
	#define force_download_request_clzo 11 //force_list_request_clzo 11
	struct _force_download_request_clzo {//_force_list_request_clzo{	
	};*/

	if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) SystemMsg_ForceDownloadRequest_clzo", ++g_bWriteTestLogCount ); // 테스트 코드

	return l_nResult;
}

void
CNetworkMgr::InitMsg_ForceDownloadResult_zocl( char * pi_pMsg )
{
	_force_download_result_zocl * pRecvMsg = reinterpret_cast< _force_download_result_zocl * >( pi_pMsg );

	if( pRecvMsg->byRetCode != 0 )
		return;

	CSFItem *			pForceItem;
	FORCE_ITEM_DATA *	pForceData;
	for( int i = 0; i < pRecvMsg->bySlotNum; ++i, ++m_dwItemIndexForFirstItemSetting )
	{
		pForceData = ( FORCE_ITEM_DATA * )_GetItemDataMgr()->GetData( IEPT_FORCE, pRecvMsg->ItemSlotInfo[i].wItemIndex );
		if( !pForceData )
			continue;

		pForceItem = static_cast< CSFItem * >( _GetForceBoard()->GetSFByDTCode( pForceData->dwForceDTCode ) );
		if( !pForceItem )
			continue;

		//_WriteLog( "8) force download => %s, count : %d", pForceItem->GetData()->pName_Kor, pRecvMsg->ItemSlotInfo[i].dwCum );

		pForceItem->SetItemDTIndex( pRecvMsg->ItemSlotInfo[i].wItemIndex );
		pForceItem->SetIndex( m_dwItemIndexForFirstItemSetting );
		if( pRecvMsg->ItemSlotInfo[i].dwCum > 0 )
		{
			pForceItem->SetSuccessCount( pRecvMsg->ItemSlotInfo[i].dwCum );
			pForceItem->CalculateSuccessCount();
		}

		pForceItem->SetHave( TRUE );		

		if( _GetAvatar()->IsUsableItem( pForceItem ) )
		{
			if( _GetLinkBoard()->IsExistObject( pForceItem ) ) 
				pForceItem->SetState( CSFItem::SFS_LINK );
			else
				pForceItem->SetState( CSFItem::SFS_ACTIVE );
		}
		else
		{
			pForceItem->SetState( CSFItem::SFS_DEACTIVE );
		}		
	}

	// Skill / Force Mastery 요청
	InitMsg_CumDownloadRequest_clzo();

	/*
	#define force_download_result_zocl 12 //force_list_result_zocl 12	
	struct _force_download_result_zocl {//_force_list_result_zocl{	
		BYTE byRetCode;	//0이면 성공..//add(3/4)

		BYTE bySlotNum;

		struct _list{
			WORD wItemIndex;
			WORD wSerial;
			DWORD dwCum;
		};

		_list ItemSlotInfo[force_storage_num];

		int size(){
			if(byRetCode != 0)
				return sizeof(byRetCode);
			return sizeof(*this)-sizeof(_list)*(force_storage_num-bySlotNum);
		}
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 종족에 따라서 _unit_download_result_zocl 나 _animus_download_result_zocl 가 전송된다.
BOOL
CNetworkMgr::InitMsg_SpecialDownloadRequest_clzo( void )
{
	// 메세지를 보냄
	BYTE l_nType[] = { init_msg, special_download_request_clzo };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 NULL,
									 0 );

	// 결과 처리
	if( !l_nResult )
		NetLogWrite( "CNetworkMgr::SystemMsg_ForceDownloadRequest_clzo(%d) Failure : %d", NST_GAME_SERVER, l_nResult );

	/*
	#define special_download_request_clzo 122
	struct _special_download_request_clzo {	//add(2/2)
	};*/

	return l_nResult;
}

void
CNetworkMgr::InitMsg_UnitDownloadResult_zocl( char * pi_pMsg )
{
	_unit_download_result_zocl * pRecvMsg = reinterpret_cast< _unit_download_result_zocl * >( pi_pMsg );

	CUnitItem *				pOrgUnit = NULL;
	CUnitItem 				clsNewUnit;
	CUnitPartsItem *		pParts = NULL;
	_unit_bullet_param *	pServerBulletData = NULL;

	int i, j;
	for( i = 0; i < pRecvMsg->byUnitNum; ++i )
	{
		if( pRecvMsg->UnitList[i].UnitData.byFrame == 0xFF )
			continue;

		// _GetUnitTuningBoard()->AddUnit()은 인벤토리에서 유닛의 키를 받을때 호출된다.
		pOrgUnit = _GetUnitStorehouseBoard()->GetUnit( pRecvMsg->UnitList[i].bySlotIndex );
		if( !pOrgUnit )
			continue;

		if( !clsNewUnit.AddFrame( pRecvMsg->UnitList[i].UnitData.byFrame ) )
			continue;

		clsNewUnit.SetIndex( pOrgUnit->GetIndex() );
		clsNewUnit.SetDurability( pRecvMsg->UnitList[i].UnitData.dwGauge );
		clsNewUnit.SetHave( TRUE );

		// parts
		for( j = 0; j < MAX_UNIT_PART_TYPE; ++j )
		{
			pParts = clsNewUnit.AddParts( j + IEPT_UNIT_HELMET, pRecvMsg->UnitList[i].UnitData.byPart[j] );
			if( pParts )
				pParts->SetHave( TRUE );
		}

		// bullet
		for( j = 0; j < MAX_UNIT_BULLET_TYPE; ++j )
		{
			pServerBulletData = reinterpret_cast< _unit_bullet_param * >( &pRecvMsg->UnitList[i].UnitData.dwBullet[j] );
			if( pServerBulletData->wBulletIndex == 0xFFFF )
				continue;

			pParts = clsNewUnit.AddBullet( j + IEPT_UNIT_ARMS, pServerBulletData->wBulletIndex );
			if( pParts )
			{
				pParts->SetDurability( pServerBulletData->wLeftNum );
				pParts->SetHave( TRUE );
			}
		}

		// backpack bullet
		for( j = 0; j < MAX_BACKPACK_BULLET_NUM; ++j )
		{
			pServerBulletData = reinterpret_cast< _unit_bullet_param * >( &pRecvMsg->UnitList[i].UnitData.dwSpare[j] );
			if( pServerBulletData->wBulletIndex == 0xFFFF )
				continue;

			pParts = clsNewUnit.AddBackpackBullet( j, pServerBulletData->wBulletIndex );
			if( pParts )
			{
				pParts->SetDurability( pServerBulletData->wLeftNum );
				pParts->SetHave( TRUE );
			}
		}

		clsNewUnit.SetTractionDebt( pRecvMsg->UnitList[i].UnitData.nPullingFee );

		// add new unit item
		_GetUnitStorehouseBoard()->ReplaceUnit( pRecvMsg->UnitList[i].bySlotIndex, &clsNewUnit );
	}

	//ItemTradeMsg_StoreListRequest_clzo();	// 상점 리스트 요청
	_GetShopTradeBoard()->StoreListRequest();

	/*
	#define unit_download_result_zocl 123
	struct _unit_download_result_zocl {	//add(2/2)
		struct _list{
			BYTE					bySlotIndex;
			_UNIT_DB_BASE::_LIST	UnitData;
		};

		BYTE	byUnitNum;
		_list	UnitList[unit_storage_num];		
		
		_unit_download_result_zocl()	{
			byUnitNum = 0;
		}

		int size(){
			return sizeof(*this)-sizeof(_list)*(unit_storage_num-byUnitNum);
		}
	};*/
}

void
CNetworkMgr::InitMsg_AnimusDownloadResult_zocl( char * pi_pMsg )
{
	_animus_download_result_zocl * pRecvMsg = reinterpret_cast< _animus_download_result_zocl * >( pi_pMsg );

	CAnimusItem *	pAnimusItem;
	ANIMUS_DATA *	pAnimusData;
	for( int i = 0; i < pRecvMsg->byAnimusNum; ++i )
	{
		pAnimusItem = _GetAnimusBoard()->GetAnimusByDTIndex( pRecvMsg->AnimusList[i].sItemIndex );
		if( !pAnimusItem )
			continue;

		// set item data
		pAnimusData = ( ANIMUS_DATA * )_GetItemDataMgr()->GetData( IEPT_ANIMUS, pRecvMsg->AnimusList[i].sItemIndex );
		if( !pAnimusData )
			continue;
		pAnimusItem->SetData( pAnimusData );

		// set character data
		pAnimusItem->SetCharData( _GetCharDataMgr()->GetAnimusByExp(
										pRecvMsg->AnimusList[i].sItemIndex,
										pRecvMsg->AnimusList[i].dwExp ) );

		// set property from Server
		pAnimusItem->SetIndex( m_dwItemIndexForFirstItemSetting++ );
		pAnimusItem->SetExp( pRecvMsg->AnimusList[i].dwExp );

		_animus_param stAnimusHpFp;
		memcpy( &stAnimusHpFp, &pRecvMsg->AnimusList[i].dwParam, sizeof( DWORD ) );
		pAnimusItem->SetHP( stAnimusHpFp.wHP );
		pAnimusItem->SetFP( stAnimusHpFp.wFP );

		// set link state
		if( _GetLinkBoard()->IsExistObject( pAnimusItem ) ) 
			pAnimusItem->SetState( CAnimusItem::AIS_LINK );
		else
			pAnimusItem->SetState( CAnimusItem::AIS_ACTIVE );
	}

	_GetShopTradeBoard()->StoreListRequest();

	/*
	#define animus_download_result_zocl 124
	struct _animus_download_result_zocl {	//add(2/2)
		struct _list{
			char sItemIndex;	//item index 빈거면 -1 
			DWORD dwExp;		//아이템 수량 맴버변수사용..
			DWORD dwParam;		//	_animus_param 으로 형변환할것..(CharacterDB.h)		//아이템 업그레이드 맴버변수사용..
								//	struct _animus_param{
								//		WORD wHP;
								//		WORD wFP;		};
		};

		BYTE byAnimusNum;
		_list AnimusList[animus_storage_num];
		
		_animus_download_result_zocl()	{
			byAnimusNum = 0;
		}

		int size(){
			return sizeof(*this)-sizeof(_list)*(animus_storage_num-byAnimusNum);
		}
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CNetworkMgr::InitMsg_NewPosStartRequest_clzo( BYTE pi_byStartMode )
{
	_new_pos_start_request_clzo l_sSend;

	l_sSend.byMapInMode = pi_byStartMode;

	// 메세지를 보냄
	BYTE l_nType[] = { init_msg, new_pos_start_request_clzo };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 ( char * )&l_sSend,
									 sizeof( _new_pos_start_request_clzo ) );
	// 결과 처리
	if( l_nResult == FALSE )
		NetLogWrite( "CNetworkMgr::InitMsg_NewPosStartRequest_clzo(%d) Failure : %d", NST_GAME_SERVER, l_nResult );

	/*
	#define new_pos_start_request_clzo 3//client_load_complete_clzo 3
	struct _new_pos_start_request_clzo { //_client_load_complete_clzo{	//upt(3/4)
		//#define mapin_type_start	0	//게임시작
		//#define mapin_type_move		1	//포탈이동..
		//#define mapin_type_revival	2	//부활
		BYTE byMapInMode;
	};*/

	if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) InitMsg_NewPosStartRequest_clzo", ++g_bWriteTestLogCount ); // 테스트 코드

	return l_nResult;
}

void
CNetworkMgr::InitMsg_NewPosStartResult_zocl( char * pi_pMsg )
{
	_new_pos_start_result_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _new_pos_start_result_zocl ) );

	// 성공
	if( l_sRecv.byMapInMode != 0xFF )
	{
		// 던젼이동
		if( l_sRecv.byMapInMode == GST_DUNGEON )
			_GetDungeonInfo()->SetInDungeon( !_GetDungeonInfo()->IsInDungeon() );
		else
			_GetDungeonInfo()->SetInDungeon( FALSE );
	}
	// 실패
	else
	{
		_GUIMessageBox( NULL, "Failure : SystemMsg_NewPosStartResult_zocl", 0xAAFFFFFF );
	}

	/*
	#define new_pos_start_result_zocl 4//game_start_zocl 4
	struct _new_pos_start_result_zocl {//_game_start_zocl{	
		BYTE byMapInMode;	//위 byMapInMode의 리바운드값.. 실패시 0xFF..
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CNetworkMgr::SecureMsg_ResponFreeRequest( void )
{
#ifdef _DEVELOPER_

	_respon_free_request l_sSend;

	l_sSend.dwDeveloperKey[0] = 0;
	l_sSend.dwDeveloperKey[1] = 1;
	l_sSend.dwDeveloperKey[2] = 2;
	l_sSend.dwDeveloperKey[3] = 3;

	// 메세지를 보냄
	BYTE l_nType[] = { secure_msg, respon_free_request };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 ( char * )&l_sSend,
									 sizeof( _respon_free_request ) );
	// 결과 처리
	if( !l_nResult )
		NetLogWrite( "CNetworkMgr::SecureMsg_ResponFreeRequest(%d) Failure : %d", NST_GAME_SERVER, l_nResult );

	/*
	//secure_msg
	#define respon_free_request 1
	struct _respon_free_request {
		DWORD dwDeveloperKey[4];//임시로 {0, 1, 2, 3} 로..

		int size(){	return sizeof(*this); }
	};*/

	if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) SecureMsg_ResponFreeRequest", ++g_bWriteTestLogCount ); // 테스트 코드

	return l_nResult;

#else

	return TRUE;

#endif
}

// 서버에서 오는 통지 메세지 처리
void
CNetworkMgr::SystemMsg_ServerNotifyInform_zone( char * pi_pMsg )
{
	_server_notify_inform_zone * pRecvMsg = reinterpret_cast< _server_notify_inform_zone * >( pi_pMsg );

	// 유저 퇴장 메세지
	if( pRecvMsg->wMsgCode == FORCE_CLOSE_TYPE_KICK )
	{
		_SetSystemMsg( "잠시 후 게임 서비스가 중단됩니다. 자세한 사항은 운영자에게 문의하시기 바랍니다." );
	}
	// 서버 종료 메세지 => 다른곳에서 같은 계정의 사용자가 접속해서 접속을 곧 종료하겟음
	else if( pRecvMsg->wMsgCode == FORCE_CLOSE_TYPE_PUSH )
	{
		_SetSystemMsg( "같은 계정의 사용자가 다른곳에서 로그인을 하였습니다. 잠시 후 접속이 끊어집니다." );
	}
	// 서버 종료 메세지 => 내부 사정으로 인한 강제 종료
	else if( pRecvMsg->wMsgCode == FORCE_CLOSE_TYPE_SERVICE || pRecvMsg->wMsgCode == FORCE_CLOSE_TYPE_ERROR )
	{
		_SetSystemMsg( "잠시 후 게임 서비스가 중단됩니다. 자세한 사항은 RF Online 공식 홈페이지를 참조하시기 바랍니다." );
	}

	RequestQuitProgram();

	/*
	//SERVER NOTIFY CODE
	#define server_notify_inform_zone 17
	struct _server_notify_inform_zone {
		WORD wMsgCode;
		int size()	{	return sizeof(*this);	}
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CNetworkMgr::SyncMsg_ObjectServerPosRequest_clzo( CCharacter * pi_pCharacter )
{
#ifdef _DEVELOPER_

	if( !pi_pCharacter )
		return FALSE;

	_object_server_pos_request_clzo l_Send;

	l_Send.byObjKind	= obj_kind_char;
	l_Send.byObjID		= pi_pCharacter->GetCharTypeID();
	l_Send.wObjIndex	= pi_pCharacter->GetServerIndex();
	l_Send.dwObjSerial	= pi_pCharacter->GetIndex();

	// 메세지를 보냄
	BYTE l_nType[] = { sync_msg, object_server_pos_request_clzo };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 ( char * )&l_Send,
									 sizeof( _object_server_pos_request_clzo ) );
	// 결과 처리
	if( !l_nResult )
		NetLogWrite( "CNetworkMgr::SyncMsg_ObjectServerPosRequest_clzo(%d) Failure : %d", NST_GAME_SERVER, l_nResult );

	/*
	#define object_server_pos_request_clzo 1
	struct _object_server_pos_request_clzo {	//add(11/05)

		BYTE byObjKind;
		BYTE byObjID;
		WORD wObjIndex;
		DWORD dwObjSerial;
	};*/

	return l_nResult;

#else

	return TRUE;

#endif // _DEVELOPER_
}

char g_pTargetClientPosMsg[128];	// 임시코드
char g_pTargetServerPosMsg[128];
void
CNetworkMgr::SyncMsg_ObjectServerPosResult_zocl( char * pi_pMsg )
{
#ifdef _DEVELOPER_

	_object_server_pos_result_zocl l_Recv;
	memcpy( &l_Recv, pi_pMsg, sizeof( _object_server_pos_result_zocl ) );

	static Vector3f l_vecPrevServerPos = { 0.0f, 0.0f, 0.0f };
	static Vector3f l_vecPrevClientPos = { 0.0f, 0.0f, 0.0f };

	if( l_Recv.byErrCode == 0 )
	{
		if( l_Recv.byObjKind == obj_kind_char )
		{
			CCharacter * l_pChar = _GetCharMgr()->GetCharacter( l_Recv.byObjID, l_Recv.dwObjSerial );
			if( !l_pChar )
				return;

			sprintf( g_pTargetClientPosMsg, "Client Position : %.f %.f %.f, ( Dist : %.f )", l_pChar->GetPosX(), l_pChar->GetPosY(), l_pChar->GetPosZ(), GetDist( _GetAvatar()->GetPosition(), l_pChar->GetPosition() ) );

			Vector3f l_vecServerPosition = { l_Recv.fServerPos[0], l_Recv.fServerPos[1], l_Recv.fServerPos[2] };
			sprintf( g_pTargetServerPosMsg, "Server Position : %.f %.f %.f, ( Dist : %.f )", l_Recv.fServerPos[0], l_Recv.fServerPos[1], l_Recv.fServerPos[2], GetDist( _GetAvatar()->GetPosition(), l_vecServerPosition ) );

			memcpy( l_vecPrevServerPos, l_vecServerPosition, sizeof( Vector3f ) );
			memcpy( l_vecPrevClientPos, l_pChar->GetPosition(), sizeof( Vector3f ) );

			//_GetNetworkMgr()->NetLogWrite( g_pTargetServerPosMsg );
			//_GetNetworkMgr()->NetLogWrite( g_pTargetClientPosMsg );
		}
	}

	/*
	#define object_server_pos_result_zocl 2
	struct _object_server_pos_result_zocl {		//add(11/05)

		BYTE byErrCode; //0: 성공..1: 배열범위초과, 2: 죽은오브젝트, 3: 시리얼이 틀림

		BYTE byObjKind;
		BYTE byObjID;
		WORD wObjIndex;
		DWORD dwObjSerial;

		float	fServerPos[3];
	};*/

#endif // _DEVELOPER_
}

// 아이템의 수량 정보 ( 클라이언트에서 정보를 잘못보내면 서버에서 전달됨 )
void
CNetworkMgr::SyncMsg_AdjustAmountInform_zocl( char * pi_pMsg )
{
	_adjust_amount_inform_zocl * pRecvMsg = reinterpret_cast< _adjust_amount_inform_zocl * >( pi_pMsg );

	CBoardItem *	pHoverItem = _GetGBM()->GetHoverItem();
	CBoardItem *	pBoardItem = NULL;

	if( pHoverItem && ( pHoverItem->GetIndex() == pRecvMsg->wSerial ) )
	{
		CBoardItem * pOrgItem = static_cast< CBoardItem * >( _GetGBM()->GetOrgObject() );
		if( pOrgItem )
		{
			pOrgItem->SetQuantity( pRecvMsg->dwNewDur - pHoverItem->GetQuantity() );
		}
		else
		{
			pHoverItem->SetQuantity( pRecvMsg->dwNewDur );
		}

		return;
	}

	switch( pRecvMsg->byStorageCode )
	{
		case IBT_INVENTORY :
			pBoardItem = _GetInventoryBoard()->GetItemByIndex( pRecvMsg->wSerial );
			if( pBoardItem )
				pBoardItem->SetQuantity( pRecvMsg->dwNewDur );
			break;

		case IBT_EQUIP :
		case _STORAGE_POS::EMBELLISH :
			break;

		case IBT_ITEM_BELT :
			pBoardItem = _GetBeltBoard()->GetItemByIndex( pRecvMsg->wSerial );
			if( pBoardItem )
				pBoardItem->SetQuantity( pRecvMsg->dwNewDur );
			break;
	}

	/*
	#define adjust_amount_inform_zocl 3
	struct _adjust_amount_inform_zocl {

		BYTE byStorageCode;
		WORD wSerial;
		DWORD dwNewDur;
	};*/
}