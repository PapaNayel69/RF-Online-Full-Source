////////////////////////////////////////////////////////////////////////////////
//
// CGP_MainGame Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../GameObject/Manager/CCharacterMgr.h"
#include "../GameObject/Manager/CTargetMgr.h"

#include "../CLand.h"
#include "../CCamera.h"
#include "../Item/CItemMgr.h"
#include "../DataTable/CResourceDataMgr.h"

#include "CGP_MainGame.h"
#include "../MainApp.h"

//------------------------------------------------------------------------------

extern BOOL	g_bWriteTestLog;	// 테스트 코드

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CGP_MainGame::DisplayInformation( int pi_nX, int pi_nY )
{
	switch( m_dwDisplayInfomationMode )
	{
		case DISPLAY_INFO_AVATAR :
			DisplayInformation_Avatar( pi_nX, pi_nY );
			break;

		case DISPLAY_INFO_PLAYER :
			DisplayInformation_Player( pi_nX, pi_nY );
			break;

		case DISPLAY_INFO_MONSTER :
			DisplayInformation_Monster( pi_nX, pi_nY );
			break;
	}
}

void
CGP_MainGame::DrawAvatarPath( void )
{
	Vector3f l_vecCurPath, l_vecNextPath;
	float	 l_fPosY;
	DWORD	 l_dwColor = D3DCOLOR_XRGB( 255, 255, 255 );

	_GetAvatar()->GetPosition( l_vecCurPath );
	_GetLevel()->GetNextYpos( l_vecCurPath, &l_fPosY );
	l_vecCurPath[1] = l_fPosY;

	for( int i = _GetAvatar()->GetCurPathNo();
		 i < _GetAvatar()->GetTotalPathNo(); ++i )
	{
		_GetAvatar()->GetPathStack()->GetPath( l_vecNextPath, i );
		_GetLevel()->GetNextYpos( l_vecNextPath, &l_fPosY );
		l_vecNextPath[1] = l_fPosY;

		_DrawLine( l_vecCurPath[0], l_vecCurPath[1] + 3.0f, l_vecCurPath[2],
				   l_vecNextPath[0], l_vecNextPath[1] + 3.0f, l_vecNextPath[2],
				   l_dwColor );
		
		l_vecCurPath[0] = l_vecNextPath[0];
		l_vecCurPath[1] = l_vecNextPath[1];
		l_vecCurPath[2] = l_vecNextPath[2];
	}
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

extern float	g_fSpeed;					// 임시코드

extern Vector3f g_vecTemp;					// 임시코드
extern DWORD	g_pPickedObjectID[2];		// 임시코드 - 마우스로 pick up한 object
extern char		g_pPickedObjectName[32];	// 임시코드
void
CGP_MainGame::DisplayInformation_Avatar( int pi_nX, int pi_nY )
{
	const int	LINE_GAP		= 20;
	const DWORD TITLE_COLOR		= D3DCOLOR_XRGB( 234, 140,   0 );
	const DWORD STRING_COLOR1	= D3DCOLOR_XRGB(  74, 100, 147 );

	char		l_pBuf[256];
	int			i = 0;

	// ---------------------------------------------------------------------

	sprintf( l_pBuf, "< Avatar Position Information >" );
	DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, TITLE_COLOR );

	sprintf( l_pBuf, "Avatar Name : %s, Server Index : %d", _GetAvatar()->GetName(), _GetAvatar()->GetIndex() );
	DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );

	sprintf( l_pBuf, "Avatar Position            : %f, %f, %f", _GetAvatar()->GetPosX(), _GetAvatar()->GetPosY(), _GetAvatar()->GetPosZ() );
	DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );

	sprintf( l_pBuf, "Target Position ( Client ) : %f, %f, %f", g_vecTemp[0], g_vecTemp[1], g_vecTemp[2] );
	DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );

	Vector3f l_vecTarget;
	_GetAvatar()->GetCurPath( l_vecTarget );
	sprintf( l_pBuf, "Target Position ( Server ) : %f, %f, %f", l_vecTarget[0], l_vecTarget[1], l_vecTarget[2] );
	DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );

	sprintf( l_pBuf, "Target Distance            : %f", GetDist( _GetAvatar()->GetPosition(), l_vecTarget ) );
	DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );

	sprintf( l_pBuf, "Avatar Speed               : %f ( %f )", g_fSpeed, _GetAvatar()->GetMagnificationOfSpeed() );
	DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );

	// ---------------------------------------------------------------------

	sprintf( l_pBuf, "< Avatar Animation Information >" );
	DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, TITLE_COLOR );

	sprintf( l_pBuf, "Animation ID : %X", _GetAvatar()->GetAnimationID() );
	DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );

	sprintf( l_pBuf, "Animation Frame : %d", _GetAvatar()->GetBoneFrame() );
	DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );

	// ---------------------------------------------------------------------

	sprintf( l_pBuf, "< Avatar Etc Information >" );
	DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, TITLE_COLOR );

	sprintf( l_pBuf, "Avatar HP : %d", _GetAvatar()->GetHP() );
	DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );

	sprintf( l_pBuf, "Frame Rate : %f", GetFPS() );
	DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );

	sprintf( l_pBuf, "Picked Character Index : %d - %d, Name : %s",
			 g_pPickedObjectID[0], g_pPickedObjectID[1], g_pPickedObjectName );
	DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );

	sprintf( l_pBuf, "Size of Visual Field : %f", _GetCharMgr()->GetVisualField() );
	DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );

	sprintf( l_pBuf, "Attack Range : %f, Attack Delay : %d", _GetAvatar()->GetAttackRange( CAT_NORMAL ), _GetAvatar()->CalculateAttackDelayWithAllEffect( CAT_NORMAL, _GetAvatar()->GetAttackDelay( NULL ) ) );
	DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );

	sprintf( l_pBuf, "Write Network Test Code : %d", g_bWriteTestLog );				// 테스트 코드
	DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );		// 테스트 코드

	// ---------------------------------------------------------------------
	// Path 정보
	sprintf( l_pBuf, "< Avatar Path Information >" );
	DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, TITLE_COLOR );

	CPath * l_pPath = _GetAvatar()->GetPathStack();
	if( l_pPath->GetTotalPathNo() > 0 )
	{
		sprintf( l_pBuf, "Total Path Num : %d", l_pPath->GetTotalPathNo() );
		DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );

		Vector3f l_vecPath;
		for( int j = 0; j < l_pPath->GetTotalPathNo(); ++j )
		{
			l_pPath->GetPath( l_vecPath, j );
			sprintf( l_pBuf, "Path %d Position : %f, %f, %f", j, l_vecPath[0], l_vecPath[1], l_vecPath[2] );
			DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

static int g_nCurPlayer;	// 임시코드
static int g_nCurMonster;	// 임시코드

void
CGP_MainGame::DisplayInformation_Player( int pi_nX, int pi_nY )
{
	const int	LINE_GAP = 20;
	const DWORD TITLE_COLOR		= D3DCOLOR_XRGB( 234, 140,   0 );
	const DWORD STRING_COLOR1	= D3DCOLOR_XRGB(  74, 100, 147 );

	char		l_pBuf[256];
	int			i = 0;

	sprintf( l_pBuf, "[ Total Player : %d ]", _GetCharMgr()->GetTotalPlayerNum() );
	DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );

	CPlayer * l_pChar = ( CPlayer * )_GetCharMgr()->GetCharacterByOrder( CTI_PLAYER, g_nCurPlayer );
	if( l_pChar )
	{
		sprintf( l_pBuf, "< Current Player > " );
		DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, TITLE_COLOR );

		char l_pPlayerName[32];
		memcpy( l_pPlayerName, l_pChar->GetName(), 32 );
		sprintf( l_pBuf, "Player Order No : %d, Player Index : %d, Player Name : %s", g_nCurPlayer, l_pChar->GetIndex(), l_pPlayerName );
		DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );

		sprintf( l_pBuf, "< Player Position Information >" );
		DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, TITLE_COLOR );

		sprintf( l_pBuf, "Player Position : %f, %f, %f", l_pChar->GetPosX(), l_pChar->GetPosY(), l_pChar->GetPosZ() );
		DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );

		sprintf( l_pBuf, "< Player Mesh & Bone Information > " );
		DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, TITLE_COLOR );

		sprintf( l_pBuf, "Race : %X", l_pChar->GetRace() );
		DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );

		sprintf( l_pBuf, "Bone ID : %X", l_pChar->GetBoneID() );
		DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );

		sprintf( l_pBuf, "Helmet ID : %X", l_pChar->GetEquipItem( CEPT_HELMET )->GetMeshID() );
		DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );

		sprintf( l_pBuf, "Upper Part ID : %X", l_pChar->GetEquipItem( CEPT_UPPER_PART )->GetMeshID() );
		DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );

		sprintf( l_pBuf, "Lower Part ID : %X", l_pChar->GetEquipItem( CEPT_LOWER_PART )->GetMeshID() );
		DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );

		sprintf( l_pBuf, "Gloves ID : %X", l_pChar->GetEquipItem( CEPT_GLOVES )->GetMeshID() );
		DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );

		sprintf( l_pBuf, "Shoes ID : %X", l_pChar->GetEquipItem( CEPT_SHOES )->GetMeshID() );
		DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );

		sprintf( l_pBuf, "Face ID : %X", l_pChar->GetEquipItem( CEPT_FACE )->GetMeshID() );
		DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );

		if( l_pChar->GetEquipItem( CEPT_WEAPON ) )
		{
			sprintf( l_pBuf, "Weapon ID : %X", l_pChar->GetEquipItem( CEPT_WEAPON )->GetMeshID() );
			DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );
		}

		sprintf( l_pBuf, "Animation ID : %X ( Avatar Animation ID : %X )", l_pChar->GetAnimationID(), _GetAvatar()->GetAnimationID() );
		DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );

		sprintf( l_pBuf, "Animation Frame : %d", l_pChar->GetBoneFrame() );
		DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );
	}
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CGP_MainGame::DisplayInformation_Monster( int pi_nX, int pi_nY )
{
	const int	LINE_GAP = 20;
	const DWORD TITLE_COLOR		= D3DCOLOR_XRGB( 234, 140,   0 );
	const DWORD STRING_COLOR1	= D3DCOLOR_XRGB(  74, 100, 147 );

	char		l_pBuf[256];
	int			i = 0;

	sprintf( l_pBuf, "[ Total Monster : %d ]", _GetCharMgr()->GetTotalMonsterNum() );
	DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );

	CMonster * l_pChar = ( CMonster * )_GetCharMgr()->GetCharacterByOrder( CTI_MONSTER, g_nCurMonster );
	if( l_pChar )
	{
		sprintf( l_pBuf, "< Current Monster > " );
		DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, TITLE_COLOR );

		char l_pMonsterName[32];
		memcpy( l_pMonsterName, l_pChar->GetName(), 32 );
		sprintf( l_pBuf, "Monster Order No : %d, Monster Index : %d, Monster Name : %s", g_nCurPlayer, l_pChar->GetIndex(), l_pMonsterName );
		DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );

		sprintf( l_pBuf, "< Monster Position Information >" );
		DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, TITLE_COLOR );

		sprintf( l_pBuf, "Monster Position : %f, %f, %f", l_pChar->GetPosX(), l_pChar->GetPosY(), l_pChar->GetPosZ() );
		DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );

		sprintf( l_pBuf, "< Monster Mesh & Bone Information > " );
		DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, TITLE_COLOR );

		sprintf( l_pBuf, "Bone ID : %X", l_pChar->GetBoneID() );
		DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );

		sprintf( l_pBuf, "Mesh ID : %X", l_pChar->GetMeshID() );
		DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );

		sprintf( l_pBuf, "Animation ID : %X", l_pChar->GetAnimationID() );
		DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );
	}

	if( g_pPickedObjectID[0] == CTI_MONSTER )
	{
		l_pChar = ( CMonster * )_GetCharMgr()->GetCharacter( CTI_MONSTER, g_pPickedObjectID[1] );

		if( l_pChar )
		{
			sprintf( l_pBuf, "< Picked Monster > " );
				DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, TITLE_COLOR );

			sprintf( l_pBuf, "Monster Index - Name : [%d] - [%s]", l_pChar->GetIndex(), l_pChar->GetName() );
				DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );

			sprintf( l_pBuf, "Monster HP : [%d] - [%d]", l_pChar->GetMaxHP(), l_pChar->GetHP() );
				DrawR3Hangul( pi_nX, pi_nY + ( LINE_GAP * ++i ), l_pBuf, STRING_COLOR1 );
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CGP_MainGame::DisplayNetworkMessage( int pi_nX, int pi_nY )
{
	/*
	const int	LINE_GAP = 20;
	const DWORD TITLE_COLOR		= D3DCOLOR_XRGB( 234, 140,   0 );
	const DWORD STRING_COLOR1	= D3DCOLOR_XRGB(  74, 100, 147 );

	char		l_pBuf[256];
	int			i = 0;*/
}

void
CGP_MainGame::DisplayHelpText( void )
{
	//--------------------------------------------------------------------------
	// 맵 정보
	_GetLand()->RenderInfo();

	//--------------------------------------------------------------------------
	// 아바타 자동 공격 정보
	int x = 620;
	Draw2DRectangle( x, 500, 780, 540, 0x70020250 );

	if( _GetAvatar()->IsEnableAutoAttack() )
		DrawR3Hangul( x + 10, 505, "Auto Attack On", D3DCOLOR_XRGB( 255, 100, 100 ) );
	else
		DrawR3Hangul( x + 10, 505, "Auto Attack Off", D3DCOLOR_XRGB( 255, 100, 100 ) );

	//if( _GetAvatar()->GetChaseTarget() )
	if( _GetTargetMgr()->GetPursuitTargetType() )
	{
		char l_pBuf[64];
		//CCharacter * l_pEnemy = _GetAvatar()->GetTarget();
		CCharacter * l_pEnemy = _GetTargetMgr()->GetTarget();
		if( l_pEnemy )
		{
			sprintf( l_pBuf, "Chase On : %s", l_pEnemy->GetName() );
			DrawR3Hangul( x + 10, 525, l_pBuf, D3DCOLOR_XRGB( 255, 100, 100 ) );
		}
	}
	else
	{
		DrawR3Hangul( x + 10, 525, "Chase Off", D3DCOLOR_XRGB( 255, 100, 100 ) );
	}

	//--------------------------------------------------------------------------
	// 키 정보
	static	const int	LINE_GAP = 20;
	static	const DWORD TITLE_COLOR		= D3DCOLOR_XRGB( 234, 140,   0 );
	static	const DWORD STRING_COLOR1	= D3DCOLOR_XRGB(  74, 100, 147 );

	static	const int	l_nX = 10;
	static	const int	l_nY = 100;

	static		  char	l_pBuf[256];

	Draw2DRectangle( 5, 95, 250, 255, 0x70020250 );

	int i = -1;
	sprintf( l_pBuf, "[ Key Info ]" );
	DrawR3Hangul( l_nX, l_nY + ( LINE_GAP * ++i ), l_pBuf, TITLE_COLOR );

	sprintf( l_pBuf, "Ctrl + F1 : 도움말 On / Off" );
	DrawR3Hangul( l_nX, l_nY + ( LINE_GAP * ++i ), l_pBuf, TITLE_COLOR );

	sprintf( l_pBuf, "Ctrl + F2 : 장착 제한 On / Off" );
	DrawR3Hangul( l_nX, l_nY + ( LINE_GAP * ++i ), l_pBuf, TITLE_COLOR );

	sprintf( l_pBuf, "Ctrl + F3 : 맵 이동 On / Off" );
	DrawR3Hangul( l_nX, l_nY + ( LINE_GAP * ++i ), l_pBuf, TITLE_COLOR );

	sprintf( l_pBuf, "Ctrl + F4 : 충돌맵 On / Off" );
	DrawR3Hangul( l_nX, l_nY + ( LINE_GAP * ++i ), l_pBuf, TITLE_COLOR );

	sprintf( l_pBuf, "Ctrl + F5 : 게임 정보 On / Off" );
	DrawR3Hangul( l_nX, l_nY + ( LINE_GAP * ++i ), l_pBuf, TITLE_COLOR );

	sprintf( l_pBuf, "Ctrl + F6 : 캐릭터 정보 On / Off" );
	DrawR3Hangul( l_nX, l_nY + ( LINE_GAP * ++i ), l_pBuf, TITLE_COLOR );

	sprintf( l_pBuf, "Ctrl + F7 : 바운딩 박스 렌더링 On / Off" );
	DrawR3Hangul( l_nX, l_nY + ( LINE_GAP * ++i ), l_pBuf, TITLE_COLOR );

	sprintf( l_pBuf, "Ctrl + F8 : 네트웍 메세지 저장 On / Off" );
	DrawR3Hangul( l_nX, l_nY + ( LINE_GAP * ++i ), l_pBuf, TITLE_COLOR );
}

/*
NetPacketList* m_RecvList;	//받은 list int GetSize()
NetPacketList* m_SendList;	//보낼 list


NetPacketList* m_SendEmptyList;	//static으로 할당한 빈 packet을 담는 list
NetPacketList* m_RecvEmptyList;	//static으로 할당한 빈 packet을 담는 list


int m_nRecvEmptyFindFalse;
int m_nSendEmptyFindFalse;




CNetData* m_pNetData;
CNetProcess** m_pProcess;
*/
