////////////////////////////////////////////////////////////////////////////
//
// CGP_CharacterCreate Class Implementation
//
////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------

#include "../GameObject/Manager/CCharacterMgr.h"
#include "../CLand.h"
#include "../CCamera.h"

#include "../Input/IMEEdit.h"

#include "../BaseHeader.h"
#include "../DataFile.h"

//--------------------------------------------------------------------------

#include <windows.h>

#include "CGP_CharacterCreate.h"

////////////////////////////////////////////////////////////////////////////
//

CGP_CharacterCreate::CGP_CharacterCreate()
{
	Init();
}

CGP_CharacterCreate::~CGP_CharacterCreate()
{
	Destroy();
}

void
CGP_CharacterCreate::Init( void )
{
	m_dwInputFocus	= 0;
}

BOOL
CGP_CharacterCreate::Create( void )
{
	if( !CGameProgress::Create() ) return FALSE;

	// 아바타와 카메라가 이미 생성되어 있어야만 한다.
	if( !_GetCamera() || !_GetAvatar() )
		return FALSE;

	// 카메라 세팅
	_GetCamera()->SetPosition( 0.0f, 0.0f, 10.0f );

	// 아바타 세팅
	_GetAvatar()->SetCombatMode( CSI_COMBAT );
	_GetAvatar()->SetAction( CAI_MOVE_STOP );

	m_bIsCreated = TRUE;

	return TRUE;
}

BOOL
CGP_CharacterCreate::Destroy( void )
{
	if( !CGameProgress::Destroy() ) return FALSE;

	m_bIsCreated = FALSE;

	return TRUE;
}

HRESULT
CGP_CharacterCreate::FrameMove( void )
{
	return S_OK;
}

HRESULT
CGP_CharacterCreate::Render( void )
{
	static RECT l_rcCharView = { 30, 50, 30 + 300, 600 - 50 };

	if( R3BeginScene() )
	{
		Draw2DRectangle( l_rcCharView.left, l_rcCharView.top,
						 l_rcCharView.right, l_rcCharView.bottom, 0xFF202020 );
		Clean2DRectangleZbuffer( l_rcCharView.left, l_rcCharView.top,
								 l_rcCharView.right, l_rcCharView.bottom );
  
		PushViewPortArea();
		SetViewPortArea( l_rcCharView.left, l_rcCharView.top,
						 l_rcCharView.right-l_rcCharView.left, l_rcCharView.bottom-l_rcCharView.top );

		//BBOX * l_pBBox = NULL;
		//l_pBBox = _GetBBoxMgr()->GetBoundBox( _GetAvatar()->GetBoneID() );
		BBOX		l_sBBox;
		Vector3f	l_vecPos;
		_GetAvatar()->GetBoundBox( l_sBBox, TRUE );
		Get2DTo3DTranslation( &l_vecPos, l_sBBox.pMin, l_sBBox.pMax,
							  l_rcCharView.left, l_rcCharView.top, l_rcCharView.right, l_rcCharView.bottom,
							  0.75f );

		//l_vecPos[0] -= 1.0f;
		//l_vecPos[1] -= 7.5f;
		l_vecPos[0] -= 1.0f;
		l_vecPos[1] += 1.0f;

		//----------------------------------------------------------------------
		// 캐릭터 렌더링
		_GetCharIF()->SetState();
		_GetAvatar()->SetPosition( l_vecPos );
		_GetAvatar()->SetRotY( 150 );
		_GetAvatar()->Animation();
		_GetAvatar()->Render();
		_GetCharIF()->UnSetState();

		PopViewPortArea();

		//----------------------------------------------------------------------
		// 나머지 정보들
		char buf[256];
		sprintf( buf, "테스트용 캐릭터 크레이트 화면 -_-^" );
		DrawR3Hangul( 400, 50, buf, D3DCOLOR_XRGB( 50, 20, 255 ) );

		sprintf( buf, "Name : %s", _GetAvatar()->GetName() );
		DrawR3Hangul( 400, 140, buf, D3DCOLOR_XRGB( 255, 0, 255 ) );

		sprintf( buf, "Bone ID : %X", _GetAvatar()->GetBoneID() );
		DrawR3Hangul( 400, 160, buf, D3DCOLOR_XRGB( 255, 0, 255 ) );

		if( _GetAvatar()->GetEquipItem( CEPT_HELMET ) )
		{
			sprintf( buf, "Helmet ID : %X, DT Index : %d",
					 _GetAvatar()->GetEquipItem( CEPT_HELMET )->GetMeshID(),
					 _GetAvatar()->GetEquipItem( CEPT_HELMET )->GetDTIndex() );
			DrawR3Hangul( 400, 180, buf, D3DCOLOR_XRGB( 255, 0, 255 ) );
		}

		if( _GetAvatar()->GetEquipItem( CEPT_UPPER_PART ) )
		{
			sprintf( buf, "Upper Part ID : %X, DT Index : %d",
					 _GetAvatar()->GetEquipItem( CEPT_UPPER_PART )->GetMeshID(),
					 _GetAvatar()->GetEquipItem( CEPT_UPPER_PART )->GetDTIndex() );
			DrawR3Hangul( 400, 200, buf, D3DCOLOR_XRGB( 255, 0, 255 ) );
		}

		if( _GetAvatar()->GetEquipItem( CEPT_LOWER_PART ) )
		{
			sprintf( buf, "Lower Part ID : %X, DT Index : %d",
					 _GetAvatar()->GetEquipItem( CEPT_LOWER_PART )->GetMeshID(),
					 _GetAvatar()->GetEquipItem( CEPT_LOWER_PART )->GetDTIndex() );
			DrawR3Hangul( 400, 220, buf, D3DCOLOR_XRGB( 255, 0, 255 ) );
		}

		if( _GetAvatar()->GetEquipItem( CEPT_GLOVES ) )
		{
			sprintf( buf, "Gloves ID : %X, DT Index : %d",
					 _GetAvatar()->GetEquipItem( CEPT_GLOVES )->GetMeshID(),
					 _GetAvatar()->GetEquipItem( CEPT_GLOVES )->GetDTIndex() );
			DrawR3Hangul( 400, 240, buf, D3DCOLOR_XRGB( 255, 0, 255 ) );
		}

		if( _GetAvatar()->GetEquipItem( CEPT_SHOES ) )
		{
			sprintf( buf, "Shoes ID : %X, DT Index : %d",
					 _GetAvatar()->GetEquipItem( CEPT_SHOES )->GetMeshID(),
					 _GetAvatar()->GetEquipItem( CEPT_SHOES )->GetDTIndex() );
			DrawR3Hangul( 400, 260, buf, D3DCOLOR_XRGB( 255, 0, 255 ) );
		}

		if( _GetAvatar()->GetEquipItem( CEPT_FACE ) )
		{
			sprintf( buf, "Face ID : %X, DT Index : %d",
					 _GetAvatar()->GetEquipItem( CEPT_FACE )->GetMeshID(),
					 _GetAvatar()->GetEquipItem( CEPT_FACE )->GetDTIndex() );
			DrawR3Hangul( 400, 280, buf, D3DCOLOR_XRGB( 255, 0, 255 ) );
		}

		if( _GetAvatar()->GetEquipItem( CEPT_WEAPON ) )
		{
			sprintf( buf, "Weapon ID : %X, DT Index : %d",
					_GetAvatar()->GetEquipItem( CEPT_WEAPON )->GetMeshID(),
					_GetAvatar()->GetEquipItem( CEPT_WEAPON )->GetDTIndex() );
			DrawR3Hangul( 400, 300, buf, D3DCOLOR_XRGB( 255, 0, 255 ) );
		}

		//----------------------------------------------------------------------
		// 종족
		switch( _GetAvatar()->GetRace() )
		{
			case CRT_BELLATOR_MALE		:
			case CRT_BELLATOR_FEMALE	: sprintf( buf, "종족 : BELLATOR" ); break;
			case CRT_CORA_MALE			:
			case CRT_CORA_FEMALE		: sprintf( buf, "종족 : CORA" ); break;
			case CRT_ACCRETIA			: sprintf( buf, "종족 : ACCRETIAN" ); break;
		}

		DrawR3Hangul( 400, 100, buf, D3DCOLOR_XRGB( 100, 100, 100 ) );

		//----------------------------------------------------------------------
		// 성별
		if( ( _GetAvatar()->GetRace() == CRT_BELLATOR_MALE ) || ( _GetAvatar()->GetRace() == CRT_CORA_MALE ) )
			sprintf( buf, "성별 : 남" );
		else if( ( _GetAvatar()->GetRace() == CRT_BELLATOR_FEMALE ) || ( _GetAvatar()->GetRace() == CRT_CORA_FEMALE ) )
			sprintf( buf, "성별 : 여" );

		DrawR3Hangul( 400, 115, buf, D3DCOLOR_XRGB( 100, 100, 100 ) );
	}

	R3EndScene();

	return S_OK;
}

BOOL
CGP_CharacterCreate::InputProcess( void )
{
	// 존 서버에 연결이 되었다면 다음 단계로 넘어간다.
	if( ( _GetNetworkMgr()->IsConnectedToServer( GST_ZONE ) ) && ( m_dwInputFocus == GPCC_INPUT_END ) )
	{
		m_bExit = TRUE;

		_GetAvatar()->SetCombatMode( CSI_PEACE );	// 임시코드

		/*
		_GetAvatar()->SetDefaultItem( CDPT_FACE, _GetAvatar()->GetEquipItem( CEPT_FACE )->GetIndex() );
		_GetAvatar()->SetDefaultItem( CDPT_UPPER_PART, _GetAvatar()->GetEquipItem( CEPT_UPPER_PART )->GetIndex() );
		_GetAvatar()->SetDefaultItem( CDPT_LOWER_PART, _GetAvatar()->GetEquipItem( CEPT_LOWER_PART )->GetIndex() );
		_GetAvatar()->SetDefaultItem( CDPT_GLOVES, _GetAvatar()->GetEquipItem( CEPT_GLOVES )->GetIndex() );
		_GetAvatar()->SetDefaultItem( CDPT_SHOES, _GetAvatar()->GetEquipItem( CEPT_SHOES )->GetIndex() );
		_GetAvatar()->SetDefaultItem( CDPT_HELMET, _GetAvatar()->GetEquipItem( CEPT_HELMET )->GetIndex() );
		*/
	}

	return TRUE;
}

LRESULT
CGP_CharacterCreate::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	DWORD	l_dwTemp;

	switch ( uMsg ) 
	{
		case WM_KEYDOWN:
			if( wParam == VK_RETURN )
			{
				m_dwInputFocus = GPCC_INPUT_END;

				_GetNetworkMgr()->Connect( GST_ZONE );
			}
			break;

		case WM_CHAR:
			if( wParam == '1' )	// 종족 & 성별
			{
				switch( _GetAvatar()->GetRace() )
				{
					case CRT_BELLATOR_MALE :
						_GetAvatar()->SetRace( CRT_BELLATOR_MALE );
						_GetAvatar()->SetBone( ID_DEFAULT_BONE_BF );
						_GetAvatar()->PutOnEquipment( CDPT_HELMET, ID_DEFAULT_ARMOR_BF );
						_GetAvatar()->PutOnEquipment( CDPT_UPPER_PART, ID_DEFAULT_ARMOR_BF );
						_GetAvatar()->PutOnEquipment( CDPT_LOWER_PART, ID_DEFAULT_ARMOR_BF );
						_GetAvatar()->PutOnEquipment( CDPT_GLOVES, ID_DEFAULT_ARMOR_BF );
						_GetAvatar()->PutOnEquipment( CDPT_SHOES, ID_DEFAULT_ARMOR_BF );
						_GetAvatar()->PutOnEquipment( CDPT_FACE, ID_DEFAULT_ARMOR_BF );
						break;
					case CRT_BELLATOR_FEMALE :
						_GetAvatar()->SetRace( CRT_CORA_MALE );
						_GetAvatar()->SetBone( ID_DEFAULT_BONE_CM );
						_GetAvatar()->PutOnEquipment( CDPT_HELMET, ID_DEFAULT_ARMOR_CM );
						_GetAvatar()->PutOnEquipment( CDPT_UPPER_PART, ID_DEFAULT_ARMOR_CM );
						_GetAvatar()->PutOnEquipment( CDPT_LOWER_PART, ID_DEFAULT_ARMOR_CM );
						_GetAvatar()->PutOnEquipment( CDPT_GLOVES, ID_DEFAULT_ARMOR_CM );
						_GetAvatar()->PutOnEquipment( CDPT_SHOES, ID_DEFAULT_ARMOR_CM );
						_GetAvatar()->PutOnEquipment( CDPT_FACE, ID_DEFAULT_ARMOR_CM );
						break;
					case CRT_CORA_MALE :
						_GetAvatar()->SetRace( CRT_CORA_FEMALE );
						_GetAvatar()->SetBone( ID_DEFAULT_BONE_CF );
						_GetAvatar()->PutOnEquipment( CDPT_HELMET, ID_DEFAULT_ARMOR_CF );
						_GetAvatar()->PutOnEquipment( CDPT_UPPER_PART, ID_DEFAULT_ARMOR_CF );
						_GetAvatar()->PutOnEquipment( CDPT_LOWER_PART, ID_DEFAULT_ARMOR_CF );
						_GetAvatar()->PutOnEquipment( CDPT_GLOVES, ID_DEFAULT_ARMOR_CF );
						_GetAvatar()->PutOnEquipment( CDPT_SHOES, ID_DEFAULT_ARMOR_CF );
						_GetAvatar()->PutOnEquipment( CDPT_FACE, ID_DEFAULT_ARMOR_CF );
						break;
					case CRT_CORA_FEMALE :
						_GetAvatar()->SetRace( CRT_ACCRETIA );
						_GetAvatar()->SetBone( ID_DEFAULT_BONE_AC );
						_GetAvatar()->PutOnEquipment( CDPT_HELMET, ID_DEFAULT_ARMOR_AC );
						_GetAvatar()->PutOnEquipment( CDPT_UPPER_PART, ID_DEFAULT_ARMOR_AC );
						_GetAvatar()->PutOnEquipment( CDPT_LOWER_PART, ID_DEFAULT_ARMOR_AC );
						_GetAvatar()->PutOnEquipment( CDPT_GLOVES, ID_DEFAULT_ARMOR_AC );
						_GetAvatar()->PutOnEquipment( CDPT_SHOES, ID_DEFAULT_ARMOR_AC );
						_GetAvatar()->PutOnEquipment( CDPT_FACE, ID_DEFAULT_ARMOR_AC );
						break;
					case CRT_ACCRETIA :
						_GetAvatar()->SetRace( CRT_BELLATOR_MALE );
						_GetAvatar()->SetBone( ID_DEFAULT_BONE_BM );
						_GetAvatar()->PutOnEquipment( CDPT_HELMET, ID_DEFAULT_ARMOR_BM );
						_GetAvatar()->PutOnEquipment( CDPT_UPPER_PART, ID_DEFAULT_ARMOR_BM );
						_GetAvatar()->PutOnEquipment( CDPT_LOWER_PART, ID_DEFAULT_ARMOR_BM );
						_GetAvatar()->PutOnEquipment( CDPT_GLOVES, ID_DEFAULT_ARMOR_BM );
						_GetAvatar()->PutOnEquipment( CDPT_SHOES, ID_DEFAULT_ARMOR_BM );
						_GetAvatar()->PutOnEquipment( CDPT_FACE, ID_DEFAULT_ARMOR_BM );
						break;
				}

				_GetAvatar()->PutOffEquipment( CEPT_WEAPON );
			}

			if( wParam == '2' )	// 헬멧
			{
				l_dwTemp = _GetAvatar()->GetEquipItem( CDPT_HELMET )->GetIndex() + 1;
				if( l_dwTemp >= ( ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ] + MAX_DEFAULT_ITEM_NUM ) )
					l_dwTemp = ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ];

				//_GetAvatar()->SetEquipItem( CDPT_HELMET, l_dwTemp );
			}
			
			if( wParam == '3' )	// 상의
			{
				l_dwTemp = _GetAvatar()->GetEquipItem( CDPT_UPPER_PART )->GetIndex() + 1;
				if( l_dwTemp >= ( ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ] + MAX_DEFAULT_ITEM_NUM ) )
					l_dwTemp = ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ];

				//_GetAvatar()->SetEquipItem( CDPT_UPPER_PART, l_dwTemp );
			}

			if( wParam == '4' )	// 하의
			{
				l_dwTemp = _GetAvatar()->GetEquipItem( CDPT_LOWER_PART )->GetIndex() + 1;
				if( l_dwTemp >= ( ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ] + MAX_DEFAULT_ITEM_NUM ) )
					l_dwTemp = ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ];

				//_GetAvatar()->SetEquipItem( CDPT_LOWER_PART, l_dwTemp );
			}

			if( wParam == '5' )	// 장갑
			{
				l_dwTemp = _GetAvatar()->GetEquipItem( CDPT_GLOVES )->GetIndex() + 1;
				if( l_dwTemp >= ( ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ] + MAX_DEFAULT_ITEM_NUM ) )
					l_dwTemp = ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ];

				//_GetAvatar()->SetEquipItem( CDPT_GLOVES, l_dwTemp );
			}

			if( wParam == '6' )	// 신발
			{
				l_dwTemp = _GetAvatar()->GetEquipItem( CDPT_SHOES )->GetIndex() + 1;
				if( l_dwTemp >= ( ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ] + MAX_DEFAULT_ITEM_NUM ) )
					l_dwTemp = ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ];

				//_GetAvatar()->SetEquipItem( CDPT_SHOES, l_dwTemp );
			}

			/*
			if( wParam == '7' )	// 얼굴
			{
				l_pEquipItem	= _GetAvatar()->GetEquipItem( CDPT_FACE ); if( !l_pEquipItem )break;
				l_dwTemp		= l_pEquipItem->GetMeshID();
				l_dwTemp		= ( ( l_dwTemp & 0x000000FF ) - 1 );									// 메시 ID가 01 부터 시작하기 때문에 마지막에 -1 을 해준다.
				l_dwTemp		= ( ( ++l_dwTemp ) % l_nMaxMeshNum[ _GetAvatar()->GetRace() + _GetAvatar()->GetZender() ] ) + 1;	// 위에서  빼준 1을 마지막에 더해준다.
				l_pItem			= _GetItemMgr()->GetItemByMeshID( ( l_pEquipItem->GetMeshID() & 0xFFFFFF00 ) + l_dwTemp ); if( !l_pItem )break;
				_GetAvatar()->SetEquipItem( CDPT_FACE, l_pItem->GetIndex() );
			}
			*/

			if( wParam == '7' )	// 얼굴
			{
				l_dwTemp = _GetAvatar()->GetEquipItem( CDPT_FACE )->GetIndex() + 1;
				if( l_dwTemp >= ( ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ] + MAX_DEFAULT_ITEM_NUM ) )
					l_dwTemp = ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ];

				//_GetAvatar()->SetEquipItem( CDPT_FACE, l_dwTemp );
			}

			if( wParam == '8' )	// 무기
			{
				const	int		MAX_WEAPON = 33 + 1;
				const	DWORD	WEAPON_DT_INDEX[MAX_WEAPON] =
				{
					1400, 256, 824, 0, 4, 8, 16, 24, 56, 600,
					200, 204, 208, 218, 276, 1200, 1204, 1208, 1216, 1412,
					1418, 1428, 608, 616, 636, 1004, 1056, 800, 808, 836,
					224, 236, 244, -1//, 1F11200, 1F10200, 1F10204, 1F10208, 1F10210, 1F1024C
				};

				static	int l_nCurWeapon = -1;

				//_GetAvatar()->SetEquipItem( CEPT_WEAPON, WEAPON_DT_INDEX[++l_nCurWeapon % MAX_WEAPON] );

				// 무기의 본 설정
				CBone * l_pBone = _GetBoneMgr()->GetBone( RLI_PLAYER, _GetAvatar()->GetBoneID() );
				CMesh * l_pWeapon = _GetAvatar()->GetEquipItem(CEPT_WEAPON)->GetMesh();
				if( l_pBone && l_pWeapon )
				{
					_GetCharIF()->RematchParent( l_pBone, l_pWeapon );
				}
			}

			break;
	}

	return 0;
}

BOOL
CGP_CharacterCreate::LoadData( void )
{
	//----------------------------------------------------------------------
	// 아이템 바이너리 테이블 로드
	/*
	LoadItemTable_Helmet( "./DataTable/HelmetItem.dat" );
	LoadItemTable_UpperPart( "./DataTable/UpperItem.dat" );
	LoadItemTable_LowerPart( "./DataTable/LowerItem.dat" );
	LoadItemTable_Gloves( "./DataTable/GauntletItem.dat" );
	LoadItemTable_Shoes( "./DataTable/ShoeItem.dat" );
	LoadItemTable_Face( "./DataTable/FaceItem.dat" );
	LoadItemTable_Weapon( "./DataTable/WeaponItem.dat" );
	LoadItemTable_Shield( "./DataTable/ShieldItem.dat" );
	LoadItemTable_Etc( "./DataTable/EtcItem.dat" );
	LoadItemTable_Potion( "./DataTable/PotionItem.dat" );
	*/

	//----------------------------------------------------------------------
	// 본, 메시 등 그래픽 데이터 로드
	//LoadPlayerBoneData( "./Character/Player/Player.ini" );
	//LoadPlayerMeshData( "./Character/Player/Player.ini" );
	//LoadPlayerAnimationData( "./Character/Player/Player.ini" );
	//LoadItemWeaponData( "./Item/Weapon/Weapon.ini" );

	//LoadMonsterBoneData( "./Character/Monster/Monster.ini" );
	//LoadMonsterMeshData( "./Character/Monster/Monster.ini" );
	//LoadMonsterAnimationData( "./Character/Monster/Monster.ini" );

	/*
	LoadPlayerBoneData( "./DataTable/PlayerBone.ini" );
	LoadPlayerMeshData( "./DataTable/PlayerMesh.ini" );
	LoadPlayerAnimationData( "./DataTable/PlayerAni.ini" );
	LoadPlayerAnimationEffectFrameData( "./DataTable/PlayerAni.ini" );

	LoadItemWeaponData( "./DataTable/Weapon.ini" );
	LoadItemWeaponAnimationData( "./DataTable/Weapon.ini" );
	LoadItemShieldData( "./DataTable/Shield.ini" );
	LoadItemModelData( "./DataTable/ModelItem.ini" );
	LoadItemModelAnimationData( "./DataTable/ModelItem.ini" );

	LoadMonsterBoneData( "./DataTable/MonsterBone.ini" );
	LoadMonsterMeshData( "./DataTable/MonsterMesh.ini" );
	LoadMonsterAnimationData( "./DataTable/MonsterAni.ini" );

	LoadNpcBoneData( "./DataTable/NpcBone.ini" );
	LoadNpcMeshData( "./DataTable/NpcMesh.ini" );
	LoadNpcAnimationData( "./DataTable/NpcAni.ini" );
	*/

	// ---------------------------------------------------------------------
	// 플레이어 & 몬스터의 바이너리 테이블 로드
	//LoadPlayerTable( "./DataTable/PlayerCharacter.dat" );
	//LoadMonsterTable( "./DataTable/MonsterCharacter.dat" );
	//LoadNpcTable( "./DataTable/Item-[StoreList].dat" );
	//_GetCharDataMgr()->LoadExpTable( "./DataTable/Exp.dat" );

	//_GetCharDataMgr()->LoadSkillForceTable( "./DataTable/RF-SkillForceInfo.dat" );

	/*
	//----------------------------------------------------------------------
	// 아바타의 갑옷 세팅
	_GetAvatar()->SetDefaultItem( CDPT_FACE, ID_DEFAULT_ARMOR_BM );
	_GetAvatar()->SetDefaultItem( CDPT_UPPER_PART, ID_DEFAULT_ARMOR_BM );
	_GetAvatar()->SetDefaultItem( CDPT_LOWER_PART, ID_DEFAULT_ARMOR_BM );
	_GetAvatar()->SetDefaultItem( CDPT_GLOVES, ID_DEFAULT_ARMOR_BM );
	_GetAvatar()->SetDefaultItem( CDPT_SHOES, ID_DEFAULT_ARMOR_BM );
	_GetAvatar()->SetDefaultItem( CDPT_HELMET, ID_DEFAULT_ARMOR_BM );

	_GetAvatar()->SetEquipItem( CDPT_HELMET, ID_DEFAULT_ARMOR_BM );
	_GetAvatar()->SetEquipItem( CDPT_UPPER_PART, ID_DEFAULT_ARMOR_BM );
	_GetAvatar()->SetEquipItem( CDPT_LOWER_PART, ID_DEFAULT_ARMOR_BM );
	_GetAvatar()->SetEquipItem( CDPT_GLOVES, ID_DEFAULT_ARMOR_BM );
	_GetAvatar()->SetEquipItem( CDPT_SHOES, ID_DEFAULT_ARMOR_BM );
	_GetAvatar()->SetEquipItem( CDPT_FACE, ID_DEFAULT_ARMOR_BM );
	*/

	return TRUE;
}

BOOL
CGP_CharacterCreate::UnloadData( void )
{
	return TRUE;
}

HRESULT
CGP_CharacterCreate::RestoreDeviceObjects( void )
{
	return 0;
}

HRESULT
CGP_CharacterCreate::InvalidateDeviceObjects( void )
{
	return 0;
}