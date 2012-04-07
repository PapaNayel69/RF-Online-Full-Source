////////////////////////////////////////////////////////////////////////////
//
// Tooltip Implementation
//
////////////////////////////////////////////////////////////////////////////

#include "GUIScreen.h"
#include "../UIGlobal.h"

#define	TITLE_RGB			D3DCOLOR_XRGB(200,217,255)	// 제목
#define	SUBTITLE_RGB		D3DCOLOR_XRGB(141,172,218)	// 부제
#define	UNAVAILABLE_RGB		D3DCOLOR_XRGB(255,0,0)		// 사용할수 없는 항목
#define UPGRADE_ARGB		D3DCOLOR_XRGB(0,255,0)		// 업그레이드된 정보


///////////////////////////////////////////////////////////////////////////////
// tooltip string buffer
static char	g_ppTooltipBuffer[TOOLTIP_MAX_LINE][TOOLTIP_MAX_STR];

static TOOLTIP_COLOR_STR_INFO	g_stTooltipColorStrInfo[TOOLTIP_MAX_COLOR_STR_NUM];
static BYTE		g_byLineCnt;
static BYTE		g_byColorStrCnt;

static BYTE		g_bySubTitleStrMaxLen;

static UPGRADE_ITEM_INFO	g_stTooltipUpgradeInfo;

///////////////////////////////////////////////////////////////////////////////

static const int max_item_type_armor = 10;
static const char ITEM_TYPE_NAME_ARMOR[10][8] =
{
	"투구", "투구", "상의", "하의", "장갑",
	"신발", "상의", "방패", "아뮬렛", "링" 
};

static const int max_item_type_weapon = 29;
static const char ITEM_TYPE_NAME_WEAPON[29][16] =
{
	"소드류", "소드류", "소드류",
	"나이프류", "나이프류", "나이프류",
	"엑스류", "엑스류", "엑스류", "엑스류",
	"메이스류", "메이스류",
	"스태프류", "스태프류",
	"스피어류", 
	"보우류", "보우류",
	"파이어암류", "파이어암류", "파이어암류",
	"런처류",
	"파이어암류", "파이어암류",
	"투척류",
	"파이어암류", "파이어암류", "파이어암류", "파이어암류",
	"채굴기"
};

static const int max_item_type_consumption = 6;
static const char ITEM_TYPE_NAME_CONSUMPTION[6][16] =
{
	"생명력", "포스력", "스테미너", "탄창", "화살", "아드레날린"
};

static const int max_item_type_etc = 15;
static const char ITEM_TYPE_NAME_ETC[15][32] =
{
	"원석", "재료", "전리품", "보석", "제작 도구", "가방", "채굴 배터리",
	"벨라토필드유닛", "벨라토공성유닛", "코라공성유닛", "아크레시아공성유닛",
	"맵", "귀환", "게이트", "가드타워"
};

static const int max_item_type_force = 7;
static const char ITEM_TYPE_NAME_FORCE[7][16] =
{
	"다크", "홀리", "파이어", "아쿠아", "테라", "윈드", "애니머스리버"
};


static const char ITEM_GRAPS_TYPE[4][8] = { "오른손", "왼손", "양손", "듀얼" };

static const char RACE_TYPE_NAME[11][32] =
{ 
	"벨라토 연방",
	"벨라토 연방",
	"신성동맹 코라",
	"신성동맹 코라",
	"아크레시아 제국",
	"전체 사용 가능",
	"전체 사용 가능",
	"벨라토 연방",
	"신성동맹 코라",
	"벨라토 연방, 신성동맹 코라",
	"전체 사용 가능"
 };

static const char SEX_TYPE_NAME[3][5] = { "남", "여", "무관" };

static const char ITEM_MONEY_TYPE[2][8] = { "달란트", "골드" };

char *
GetMoneyName( BYTE pi_byMoneyType )
{
	if( pi_byMoneyType == 0 )
		return _GetAvatarMoneyName();
	else
		return (char * )ITEM_MONEY_TYPE[1];
}

static const char ITEM_ATTRIBUTE_NAME[5][8] =
{
	"불", "물", "흙", "바람", "없음"
};

static const char ITEM_TOLERANCE_NAME[5][16] =
{
	"없음", "불내성", "물내성", "흙내성", "바람내성"
};

// 스킬 분류 명칭
static const char SKILL_CLASS_NAME[6][16] =
{
	"근접", "원거리", "런처", "포스", "방패", "방어"	
};

// 전투 스킬 단계 명칭
static const char ATTACK_SKILL_LV_NAME[4][10] = 
{
	"스컬러", "익스퍼트", "마스터", "그랜져"
};


// 포스 단계 명칭
static const char FORCE_LV_NAME[24][32] = 
{
	"다크",	"데쓰",	"익스팅션", "헬",

	"홀리", "핼로우", "디바인", "리바이벌",
	
	"파이어&라이트", "플레임&스웜", "블레이즈&브라이트", "헬파이어&솔라",
	
	"아쿠아&아이스", "폭포&프로스트", "웨이브&블리자드", "플러드&노바",
	
	"어스&포이즌", "스톤&베넘", "쉐이크&턱신", "싱킹&바이러스",

	"에어&일렉트릭", "블래스트&라이티닝", "토네이도&썬더", "그래버티&썬더버드"
};

// 유닛 공격타입
static const char UNIT_ATTACK_TYPE[2][16] = 
{
	"근접 공격", "원거리 공격"
};

char *
GetAttackType( BYTE pi_byAttackType )
{
	if( pi_byAttackType >=2 )
		return ( char * )UNIT_ATTACK_TYPE[0];

	return ( char * )UNIT_ATTACK_TYPE[pi_byAttackType];
}

static const char UNIT_MOVE_SPEED[5][16] =
{
	"매우느림", "느림",	"보통", "빠름", "매우빠름"
};

char *
GetUnitMoveSpeed( float pi_fMoveSpeed )
{
	if( pi_fMoveSpeed <= 1.f )
		return ( char * )UNIT_MOVE_SPEED[0];
	else if( pi_fMoveSpeed <= 2.4f )
		return ( char * )UNIT_MOVE_SPEED[1];
	else if( pi_fMoveSpeed <= 3.0f )
		return ( char * )UNIT_MOVE_SPEED[2];
	else if( pi_fMoveSpeed <= 4.4f )
		return ( char * )UNIT_MOVE_SPEED[3];
	else
		return ( char * )UNIT_MOVE_SPEED[4];
}


char *
GetItemTypeName( DWORD pi_dwMeshID )
{
	static const BYTE ITEM_TYPE_NUM[5] = { max_item_type_armor, max_item_type_weapon, max_item_type_consumption,
										   max_item_type_etc, max_item_type_force };	

	BYTE l_byItemType[2];
	l_byItemType[0] = ( pi_dwMeshID & 0x000F0000 ) >> 16;
	l_byItemType[1] = ( pi_dwMeshID & 0x0000FF00 ) >> 8;

	if( ITEM_TYPE_NUM[l_byItemType[0]] <= 0 )
		return NULL;

	if( l_byItemType[1] >= ITEM_TYPE_NUM[l_byItemType[0]] )
		return NULL;

	switch( l_byItemType[0] )
	{
		case 0 : return ( char * )ITEM_TYPE_NAME_ARMOR[l_byItemType[1]];
		case 1 : return ( char * )ITEM_TYPE_NAME_WEAPON[l_byItemType[1]];
		case 2 : return ( char * )ITEM_TYPE_NAME_CONSUMPTION[l_byItemType[1]];
		case 3 : return ( char * )ITEM_TYPE_NAME_ETC[l_byItemType[1]];
		case 4 : return ( char * )ITEM_TYPE_NAME_FORCE[l_byItemType[1]];
	}

	return NULL;
}

char *
GetGrapsTypeName( DWORD pi_dwMeshID )
{
	BYTE l_byItemType = _GetItemType( pi_dwMeshID );

	// 방패
	if( l_byItemType == IID_ARMOR )
	{
		if( _GetItemExtType( pi_dwMeshID ) == IID_LSHIELD )
			return ( char * )ITEM_GRAPS_TYPE[1];
		
		return NULL;
	}
	// 무기
	else if( l_byItemType == IID_WEAPON )
	{
		BYTE l_byHandType = _GetWeapnHandType( pi_dwMeshID );
		switch( l_byHandType )
		{
		case RIGHT_HAND_WEAPON:
			return ( char * )ITEM_GRAPS_TYPE[0];

		case DUAL_HAND_WEAPON:
			return ( char * )ITEM_GRAPS_TYPE[3];

		default:
			return ( char * )ITEM_GRAPS_TYPE[2];		
		}
	}

	return NULL;
}

char *
GetRaceTypeName( DWORD pi_dwMeshID )
{	
	BYTE l_byRace = ( pi_dwMeshID & 0x0FF00000 ) >> 20;

	return ( char * )RACE_TYPE_NAME[l_byRace];
}

BYTE
GetRaceType( DWORD pi_dwMeshID )
{
	return ( pi_dwMeshID & 0x0FF00000 ) >> 20;
}

BYTE
GetRace( BYTE pi_byRaceType )
{
	if( pi_byRaceType < IID_ACCRETIA )
		return pi_byRaceType / 2;

	else if( pi_byRaceType == IID_BELLATOR )
		return RT_BELLATO;

	else if( pi_byRaceType == IID_CORA )
		return RT_CORA;

	else if( pi_byRaceType == IID_ACCRETIA )
		return RT_ACCRETIA;

	return RT_NONE;
}

char *
GetSexTypeName( DWORD pi_dwMeshID )
{
	BYTE l_bySex = ( pi_dwMeshID & 0x0FF00000 ) >> 20;
	if( l_bySex < IID_ACCRETIA )
		return ( char * )SEX_TYPE_NAME[l_bySex%2];

	else if( l_bySex == IID_ACCRETIA )
		return NULL;

	else if( l_bySex < IID_BELLATOR )
		return ( char * )SEX_TYPE_NAME[(l_bySex+1)%2];

	return ( char * )SEX_TYPE_NAME[2];
}

BYTE
GetSexType( BYTE pi_byRaceType )
{
	if( pi_byRaceType < IID_ACCRETIA )
		return pi_byRaceType % 2;

	else if( pi_byRaceType < IID_BELLATOR )
		return (pi_byRaceType+1) % 2;

	else 
		return ST_NONE;
}

char *
GetItemAttributeName( BYTE pi_byAttribute )
{
	if( pi_byAttribute > 3 )
		return ( char * )ITEM_ATTRIBUTE_NAME[4];

	return ( char * )ITEM_ATTRIBUTE_NAME[pi_byAttribute];
}

char *
GetToleranceName( BYTE pi_byTolerance )
{
	if( ( pi_byTolerance < 0 ) || ( pi_byTolerance > 4 ) )
		return NULL;
	
	return ( char * )ITEM_TOLERANCE_NAME[pi_byTolerance];	
}

BYTE
GetWeaponType( DWORD pi_dwMeshID )
{
	BYTE l_byItemType = _GetItemType( pi_dwMeshID );
	
	if( l_byItemType != IID_WEAPON )
		return MELEE_MASTERY;		
	
	BYTE l_byExtType = _GetItemExtType( pi_dwMeshID );

	if( l_byExtType <= IID_TMACE )
		return MELEE_MASTERY; // melee

	switch( l_byExtType )
	{
	case IID_TBOW:
	case IID_TCROSSBOW:		// bow		

	case IID_RGUN:
	case IID_DGUN:
	case IID_TRIFLE:
	case IID_TFAUST:
	case IID_TMACHINEGUN:
	case IID_RBEAMGUN:
	case IID_DBEAMGUN:
	case IID_TBEAMRIFLE:
	case IID_TPLASMAGUN:	// firearm		
	
	case IID_TFLAMETHROWER:	// throw
		return MISSILE_MASTERY;

	case IID_TLAUNCHER:	
		return RACE_MASTERY; // launcher

	case IID_RSTAFF:
	case IID_TSTAFF:
		return FORCE_MASTERY; // force 

	default:
		return MELEE_MASTERY; // melee
	}	
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "../GameWindow/GUICharInfoWindow.h"
#include "../GameWindow/GUIInventoryWindow.h"

extern CGUICharInfoWindow *		g_pUiCharInfoWindow;
extern CGUIInventoryWindow *	g_pUiInventWindow;

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
SetStringtoLine( WORD pi_wFrontBlankNum, WORD pi_wCharNumInLine, char * pi_pStr )
{
	WORD l_wCharIndex;
	int ch;
	
	l_wCharIndex = 0;

	for( int i=0; pi_pStr[i] != '\0'; )
	{
		// 줄 앞부분에 빈칸 채우기
		if( l_wCharIndex == 0 &&
			pi_wFrontBlankNum > 0 )
		{				
			memset( g_ppTooltipBuffer[g_byLineCnt], ' ', pi_wFrontBlankNum );
			g_ppTooltipBuffer[g_byLineCnt][pi_wFrontBlankNum] = '\0';

			l_wCharIndex = pi_wFrontBlankNum;
		}
		else
		{
			g_ppTooltipBuffer[g_byLineCnt][l_wCharIndex] = pi_pStr[i];

			if( g_ppTooltipBuffer[g_byLineCnt][l_wCharIndex] == '\n' )
			{
				g_ppTooltipBuffer[g_byLineCnt][l_wCharIndex] = '\0';

				// next line
				l_wCharIndex = 0;
				++g_byLineCnt;
			}
			else if( l_wCharIndex == pi_wCharNumInLine ) // 한줄에 36자만 출력한다.
			{
				if( g_ppTooltipBuffer[g_byLineCnt][l_wCharIndex] & 0x80 )
				{
					for( ch=l_wCharIndex-1; ch>=0; --ch )
					{
						if( !(g_ppTooltipBuffer[g_byLineCnt][ch] & 0x80) )
							break;						
					}

					// 한글 짤린다.
					if( (l_wCharIndex - ch)%2 != 0 )
					{
						g_ppTooltipBuffer[g_byLineCnt][l_wCharIndex] = '\0';
						--i; // 다음줄로 내린다.
					}
					else
					{
						g_ppTooltipBuffer[g_byLineCnt][l_wCharIndex+1] = '\0';
					}
				}
				else
				{
					g_ppTooltipBuffer[g_byLineCnt][l_wCharIndex+1] = '\0';
				}

				// next line
				l_wCharIndex = 0;
				++g_byLineCnt;
			}
			else
			{
				++l_wCharIndex;
			}		

			// next char
			++i;
		}
	}

//	if( i == 0 )
//		g_ppTooltipBuffer[g_byLineCnt++][l_wCharIndex] = '\n';

	//else if( l_wCharIndex > 0 )
	if( l_wCharIndex > 0 )
		g_ppTooltipBuffer[g_byLineCnt++][l_wCharIndex] = '\0';
}

// ================================================================================================

void
ClearTooltipBuffer( void )
{
	for( int i = 0; i < TOOLTIP_MAX_LINE; ++i )
		g_ppTooltipBuffer[i][0] = '\0';

	for( i = 0; i < TOOLTIP_MAX_COLOR_STR_NUM; ++i )
	{
		g_stTooltipColorStrInfo[i].pString[0] = '\0';

		g_stTooltipColorStrInfo[i].byCharIndex = 0;
		g_stTooltipColorStrInfo[i].dwColor = SUBTITLE_RGB;
	}

	g_stTooltipColorStrInfo[0].dwColor = TITLE_RGB;

	g_bySubTitleStrMaxLen = 0;

	g_byLineCnt		= 0;
	g_byColorStrCnt	= 0;
}

void
SetTooltipStrBuffer( BYTE pi_byFrontSpaceNum, const char * pi_pFormatString, ... )
{
	static va_list	pArgumentList;

	if( pi_byFrontSpaceNum )
	{
		memset( g_ppTooltipBuffer[g_byLineCnt], ' ', pi_byFrontSpaceNum );
		g_ppTooltipBuffer[g_byLineCnt][pi_byFrontSpaceNum] = '\0';
	}

	va_start( pArgumentList, pi_pFormatString );
	vsprintf( &g_ppTooltipBuffer[g_byLineCnt][pi_byFrontSpaceNum], pi_pFormatString, pArgumentList );
	va_end( pArgumentList );

	++g_byLineCnt;
}

void
SetTooltipColorStrBuffer( BYTE pi_byCharIndex, DWORD pi_dwStrColor, const char * pi_pContentsFormatStr, ... )
{
	static va_list	pArgumentList;

	va_start( pArgumentList, pi_pContentsFormatStr );
	vsprintf( g_stTooltipColorStrInfo[g_byColorStrCnt].pString, pi_pContentsFormatStr, pArgumentList );
	va_end( pArgumentList );

	g_stTooltipColorStrInfo[g_byColorStrCnt].byLineIndex	= g_byLineCnt;
	g_stTooltipColorStrInfo[g_byColorStrCnt].byCharIndex	= pi_byCharIndex;
	g_stTooltipColorStrInfo[g_byColorStrCnt].dwColor		= pi_dwStrColor;
	
	++g_byColorStrCnt;
}

void
SetTooltipNextLine( char * pi_pSubTitleStr, const char * pi_pContentsFormatStr, ... )
{
	static va_list	pArgumentList;
	static int		nSubTitleStrLen;

	// title
	if( g_byLineCnt == 0 ) 
	{
		sprintf( g_stTooltipColorStrInfo[g_byColorStrCnt].pString, "[%s]", pi_pSubTitleStr );
	}
	// sub title
	else 
	{
		nSubTitleStrLen = strlen( pi_pSubTitleStr );
		if( g_bySubTitleStrMaxLen < nSubTitleStrLen )
			g_bySubTitleStrMaxLen = nSubTitleStrLen;

		if( ( pi_pSubTitleStr[0] == '[' ) || ( g_bySubTitleStrMaxLen == nSubTitleStrLen ) )
		{
			g_stTooltipColorStrInfo[g_byColorStrCnt].pString[0] = '\0';
		}
		else
		{
			memset( g_stTooltipColorStrInfo[g_byColorStrCnt].pString, ' ', g_bySubTitleStrMaxLen - nSubTitleStrLen );
			g_stTooltipColorStrInfo[g_byColorStrCnt].pString[g_bySubTitleStrMaxLen - nSubTitleStrLen] = '\0';
		}
		strcat( g_stTooltipColorStrInfo[g_byColorStrCnt].pString, pi_pSubTitleStr );
	}
	g_stTooltipColorStrInfo[g_byColorStrCnt].byLineIndex = g_byLineCnt;

	// set contents string
	if( pi_pContentsFormatStr )
	{
		// set space
		memset( g_ppTooltipBuffer[g_byLineCnt], ' ', g_bySubTitleStrMaxLen + 2 );
		g_ppTooltipBuffer[g_byLineCnt][g_bySubTitleStrMaxLen + 2] = '\0';

		va_start( pArgumentList, pi_pContentsFormatStr );
		vsprintf( &g_ppTooltipBuffer[g_byLineCnt][g_bySubTitleStrMaxLen + 2], pi_pContentsFormatStr, pArgumentList );
		va_end( pArgumentList );
	}

	++g_byLineCnt;
	++g_byColorStrCnt;
}

// ================================================================================================

void
SetTooltipNeededMastery( BYTE * pi_pMasteryList, BYTE pi_byAvatarRaceType )
{
	if( g_bySubTitleStrMaxLen < 9 )
		g_bySubTitleStrMaxLen = 9;
	
	SetTooltipColorStrBuffer( g_bySubTitleStrMaxLen - 9, SUBTITLE_RGB, "요구 기술" );
	BYTE byPrevLineCnt = g_byLineCnt;

	for( int i = 0; i < DEFFENCE_MASTERY+1; ++i )
	{
		if( pi_pMasteryList[i] == 0xFF || pi_pMasteryList[i] == 0 )
			continue;
		
		if( ( i == FORCE_MASTERY && pi_byAvatarRaceType == RT_ACCRETIA ) || // 포스
			( i == RACE_MASTERY && pi_byAvatarRaceType != RT_ACCRETIA ) ||  // 런처
			g_pUiCharInfoWindow->GetBattleMastery( i ) < pi_pMasteryList[i] )
		{
			if( pi_pMasteryList[i] >= 99 )
				SetTooltipColorStrBuffer( g_bySubTitleStrMaxLen + 2, UNAVAILABLE_RGB, "%s기술 GM", SKILL_CLASS_NAME[i] );
			else
				SetTooltipColorStrBuffer( g_bySubTitleStrMaxLen + 2, UNAVAILABLE_RGB, "%s기술 %d", SKILL_CLASS_NAME[i], pi_pMasteryList[i] );

			++g_byLineCnt;
		}
		else
		{
			if( pi_pMasteryList[i] >= 99 )
				SetTooltipStrBuffer( g_bySubTitleStrMaxLen + 2, "%s기술 GM", SKILL_CLASS_NAME[i] );
			else
				SetTooltipStrBuffer( g_bySubTitleStrMaxLen + 2, "%s기술 %d", SKILL_CLASS_NAME[i], pi_pMasteryList[i] );
		}
	}
	if( g_byLineCnt == byPrevLineCnt )
	{
		SetTooltipStrBuffer( g_bySubTitleStrMaxLen + 2, "없음" );
	}
}

void
SetTooltipPriceInfo( BYTE pi_byShopMode, BYTE pi_byPriceType, DWORD pi_dwPrice, BYTE pi_byNum )
{
	if( pi_byShopMode > SHOP_MODE_REPAIR )
		return;

	static char TRADE_STR[3][8] = { "구입", "판매", "수리" };

	SetTooltipNextLine( "[가격 정보]", NULL );

	SetTooltipColorStrBuffer( 2, SUBTITLE_RGB, "%s 가격", TRADE_STR[pi_byShopMode] );
	if( pi_byNum > 1 )
		SetTooltipStrBuffer( 13, "%d ( 총계 %d ) %s", pi_dwPrice, pi_dwPrice * pi_byNum, GetMoneyName( pi_byPriceType ) );
	else
		SetTooltipStrBuffer( 13, "%d %s", pi_dwPrice, GetMoneyName( pi_byPriceType ) );

	if( pi_byShopMode != SHOP_MODE_SELL )
	{
		if( ( pi_byPriceType == UI_IMT_DALANT && pi_dwPrice > g_pUiInventWindow->GetOwnDalant() ) ||
			( pi_byPriceType == UI_IMT_GOLD && pi_dwPrice > g_pUiInventWindow->GetOwnGold() ) )
		{
			SetTooltipColorStrBuffer( 13, UNAVAILABLE_RGB, "%d", pi_dwPrice );
		}
	}
}

void
SetTooltipUsableWeapon( BYTE pi_WeaponNum, char pi_pWeaponList[][32] )
{
	if( g_bySubTitleStrMaxLen < 12 )
		g_bySubTitleStrMaxLen = 12;
	SetTooltipColorStrBuffer( g_bySubTitleStrMaxLen - 12, SUBTITLE_RGB, "사용가능무기" );

	BYTE	byMaxStrLen[2] = { 0, 0 };
	BYTE	byStrLen = 0;
	BYTE	byWeaponNum = 0;
	
	// 첫번째 줄에 제일 긴 문자열을 찾는다.
	for( int i = 0; i < pi_WeaponNum; ++i )
	{
		if( pi_pWeaponList[i][0] == '\0' )
			break;

		byStrLen = strlen( pi_pWeaponList[i] );

		if( byStrLen > byMaxStrLen[i%2] )
			byMaxStrLen[i%2] = byStrLen;
	}
	byWeaponNum = i;

	if( !byWeaponNum )
	{
		++g_byLineCnt;
		return;
	}


	// 5개 이하이면 한줄로 보여준다
	if( byWeaponNum < 6 )
	{
		for( i = 0; i < byWeaponNum; ++i )
			SetTooltipStrBuffer( g_bySubTitleStrMaxLen + 2, "%s", pi_pWeaponList[i] );
	}
	else
	{
		++g_byLineCnt;

		for( i = 0; i < byWeaponNum; ++i )
		{
			if( i%2 == 0 )
			{
				byStrLen = 2 + byMaxStrLen[0] - strlen( pi_pWeaponList[i] );
				memset( g_ppTooltipBuffer[g_byLineCnt], ' ', byStrLen );
				g_ppTooltipBuffer[g_byLineCnt][byStrLen] = '\0';

				strcat( g_ppTooltipBuffer[g_byLineCnt], pi_pWeaponList[i] );
			}
			else
			{
				strcat( g_ppTooltipBuffer[g_byLineCnt], ", " );

				byStrLen = byMaxStrLen[1] - strlen( pi_pWeaponList[i] );
				memset( &g_ppTooltipBuffer[g_byLineCnt][byMaxStrLen[0]+2+2], ' ', byStrLen );
				g_ppTooltipBuffer[g_byLineCnt][byMaxStrLen[0]+2+2+byStrLen] = '\0';

				strcat( g_ppTooltipBuffer[g_byLineCnt], pi_pWeaponList[i] );
				++g_byLineCnt;
			}
		}
		if( i%2 == 1 ) // 마지막 한줄에 무기 하나만 있을 때 다음줄로
			++g_byLineCnt;
	}
}

void
SetTooltipTolerance( BYTE * pi_pToleranceType, DWORD * pi_dwTolerance )
{
	if( g_bySubTitleStrMaxLen < 4 )
		g_bySubTitleStrMaxLen = 4;

	SetTooltipColorStrBuffer( g_bySubTitleStrMaxLen - 4, SUBTITLE_RGB, "내성" );

	if( pi_pToleranceType[0] == 0 )
	{
		SetTooltipStrBuffer( g_bySubTitleStrMaxLen + 2, "없음" );
	}
	else
	{
		for( int i = 0; i < 4; ++i )
		{
			if( pi_pToleranceType[i] == 0 )
				break;
			
			SetTooltipStrBuffer( g_bySubTitleStrMaxLen + 2, "%s %d", GetToleranceName( pi_pToleranceType[i] ),
																	 pi_dwTolerance[i] );
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
SetTooltip_Weapon( CGUIItem * pi_pItem, TOOLTIP_WEAPON * pi_pItemInfo, BYTE pi_byShopMode )
{
	DWORD pi_dwItemID = pi_pItem->GetItemID();
	if( _IsItemExtType( pi_dwItemID, IID_MININGTOOL ) )
	{
		SetTooltip_Mining( pi_pItem, pi_pItemInfo, pi_byShopMode );
		return;
	}

	ClearTooltipBuffer();
	g_bySubTitleStrMaxLen = 11;

	// 이름
	SetTooltipNextLine( pi_pItemInfo->pNameStr, NULL );
	// 종류
	SetTooltipNextLine( "종류", "%s", GetItemTypeName( pi_pItem->GetItemID() ) );

	// 요구레벨
	if( pi_pItemInfo->wLevel > g_pUiCharInfoWindow->GetLevel() )
	{
		SetTooltipColorStrBuffer( 13, UNAVAILABLE_RGB, "%d", pi_pItemInfo->wLevel );
	}
	SetTooltipNextLine( "요구 레벨", "%d", pi_pItemInfo->wLevel );

	// 잡는 유형
	SetTooltipNextLine( "잡는 유형", "%s", GetGrapsTypeName( pi_pItem->GetItemID() ) );

	// 사용 종족
	BYTE byItemRaceType = GetRaceType( pi_pItem->GetItemID() );
	BYTE byAvatarRaceType = GetRace( g_pUiCharInfoWindow->GetRaceType() );
	if( GetRace( byItemRaceType ) != RT_NONE && GetRace( byItemRaceType ) != byAvatarRaceType )
	{
		SetTooltipColorStrBuffer( g_bySubTitleStrMaxLen + 2, UNAVAILABLE_RGB, "%s", GetRaceTypeName( pi_pItem->GetItemID() ) );
	}
	SetTooltipNextLine( "사용 종족", "%s", GetRaceTypeName( pi_pItem->GetItemID() ) );
	
	// 요구 숙련도
	SetTooltipNeededMastery( pi_pItemInfo->m_byNeedMastery, byAvatarRaceType );

	// 내구도
	if( pi_pItemInfo->bRepairable )
	{
		SetTooltipNextLine( "내구도", "%d / %d / %d", pi_pItemInfo->dwCurDurability, pi_pItemInfo->dwRepairDurability, pi_pItemInfo->dwMaxDurability );

		if( pi_pItemInfo->dwCurDurability < pi_pItemInfo->dwMaxDurability * 0.1 )
		{
			--g_byLineCnt;
			SetTooltipColorStrBuffer( g_bySubTitleStrMaxLen + 2, UNAVAILABLE_RGB, "%d", pi_pItemInfo->dwCurDurability );
			++g_byLineCnt;
			SetTooltipNextLine( "수리여부", "가능" );
		}
		else
			SetTooltipNextLine( "수리여부", "불가" );
	}
	else
	{
		SetTooltipNextLine( "내구도", "제한없음" );
	}

	// 일반 공격력
	if( pi_pItemInfo->dwMinGenAttPower[0] < pi_pItemInfo->dwMinGenAttPower[1] )
	{
		SetTooltipColorStrBuffer( g_bySubTitleStrMaxLen + 2, UNAVAILABLE_RGB, "%d", pi_pItemInfo->dwMinGenAttPower[1] );
	}
	if( pi_pItemInfo->dwMaxGenAttPower[0] < pi_pItemInfo->dwMaxGenAttPower[1] )
	{
		char buf[8];
		itoa( pi_pItemInfo->dwMinGenAttPower[1], buf, 10 );
		SetTooltipColorStrBuffer( g_bySubTitleStrMaxLen + 2 + strlen(buf) + 3, UNAVAILABLE_RGB, "%d", pi_pItemInfo->dwMaxGenAttPower[1] );
	}
	SetTooltipNextLine( "일반 공격력", "%d ~ %d", pi_pItemInfo->dwMinGenAttPower[1], pi_pItemInfo->dwMaxGenAttPower[1] );

	// 포스 공격력
	if( pi_pItemInfo->dwMinForceAttPower[0] < pi_pItemInfo->dwMinForceAttPower[1] )
	{
		SetTooltipColorStrBuffer( g_bySubTitleStrMaxLen + 2, UNAVAILABLE_RGB, "%d", pi_pItemInfo->dwMinForceAttPower[1] );
	}
	if( pi_pItemInfo->dwMaxForceAttPower[0] < pi_pItemInfo->dwMaxForceAttPower[1] )
	{
		char buf[8];
		itoa( pi_pItemInfo->dwMinForceAttPower[1], buf, 10 );
		SetTooltipColorStrBuffer( g_bySubTitleStrMaxLen + 2 + strlen(buf) + 3, UNAVAILABLE_RGB, "%d", pi_pItemInfo->dwMaxForceAttPower[1] );
	}
	SetTooltipNextLine( "포스 공격력", "%d ~ %d", pi_pItemInfo->dwMinForceAttPower[1], pi_pItemInfo->dwMaxForceAttPower[1] );

	// 속성
	SetTooltipNextLine( "속성", "%s", GetItemAttributeName( pi_pItemInfo->byAttribute ) );

	// 업그레이드
	g_stTooltipUpgradeInfo.byTotalSocketNum = 0;
	if( pi_pItemInfo->byUpgradeSocketNo > 0 )
	{
		SetTooltipNextLine( "업그레이드", NULL );
		
		g_stTooltipUpgradeInfo.byTotalSocketNum	= pi_pItemInfo->byUpgradeSocketNo;
		g_stTooltipUpgradeInfo.byTotalItemNum	= pi_pItemInfo->wUpgradeLevel;

		memcpy( g_stTooltipUpgradeInfo.dwSpriteIDList, pi_pItemInfo->dwUpgradeSpriteID, g_stTooltipUpgradeInfo.byTotalItemNum * sizeof( DWORD ) );

		g_stTooltipUpgradeInfo.byUpgradeSocketLineIdx = g_byLineCnt-1;
		g_stTooltipUpgradeInfo.byUpgradeSocketCharIdx = g_bySubTitleStrMaxLen + 2;
	}

	SetTooltipNextLine( "[설명]", NULL );
	SetStringtoLine( 2, 36, pi_pItemInfo->pDescription );

	// 가격 정보
	SetTooltipPriceInfo( pi_byShopMode, pi_pItemInfo->byPriceType, pi_pItemInfo->dwPrice, 1 );
	
	gScreen.SetTooltip_Item( pi_pItem, g_byLineCnt, g_ppTooltipBuffer, g_byColorStrCnt, g_stTooltipColorStrInfo, &g_stTooltipUpgradeInfo );
}

void
SetTooltip_Armor( CGUIItem * pi_pItem, TOOLTIP_ARMOR * pi_pItemInfo, BYTE pi_byShopMode )
{
	DWORD pi_dwItemID = pi_pItem->GetItemID();
	if( _IsItemExtType( pi_dwItemID, IID_LSHIELD ) )
	{
		SetTooltip_Shield( pi_pItem, pi_pItemInfo, pi_byShopMode );
		return;
	}

	ClearTooltipBuffer();
	g_bySubTitleStrMaxLen = 11;

	// 이름
	SetTooltipNextLine( pi_pItemInfo->pNameStr, NULL );
	// 종류
	SetTooltipNextLine( "종류", "%s", GetItemTypeName( pi_pItem->GetItemID() ) );

	// 요구레벨
	if( pi_pItemInfo->wLevel > g_pUiCharInfoWindow->GetLevel() )
	{
		SetTooltipColorStrBuffer( g_bySubTitleStrMaxLen + 2, UNAVAILABLE_RGB, "%d", pi_pItemInfo->wLevel );
	}
	SetTooltipNextLine( "요구 레벨", "%d", pi_pItemInfo->wLevel );

	// 사용 종족
	BYTE byItemRaceType = GetRaceType( pi_pItem->GetItemID() );
	BYTE byAvatarRaceType = GetRace( g_pUiCharInfoWindow->GetRaceType() );
	if( GetRace( byItemRaceType ) != RT_NONE && GetRace( byItemRaceType ) != byAvatarRaceType )
	{
		SetTooltipColorStrBuffer( g_bySubTitleStrMaxLen + 2, UNAVAILABLE_RGB, "%s", GetRaceTypeName( pi_pItem->GetItemID() ) );
	}
	SetTooltipNextLine( "사용 종족", "%s", GetRaceTypeName( pi_pItem->GetItemID() ) );

	
	// 성별
	// 아크레시아 아이템이면 성별은 출력하지 않는다.
	char * pSexStr = GetSexTypeName( pi_pItem->GetItemID() );
	if( pSexStr )
	{
		if( GetSexType( byItemRaceType ) != ST_NONE &&	
			GetSexType( byItemRaceType ) != GetSexType( g_pUiCharInfoWindow->GetRaceType() ) )
		{
			SetTooltipColorStrBuffer( g_bySubTitleStrMaxLen + 2, UNAVAILABLE_RGB, "%s", pSexStr );
		}
	}
	SetTooltipNextLine( "성별", "%s", pSexStr );

	// 요구 숙련도
	SetTooltipNeededMastery( pi_pItemInfo->m_byNeedMastery, byAvatarRaceType );

	// 내구도
	if( pi_pItemInfo->bRepairable )
	{
		SetTooltipNextLine( "내구도", "%d / %d / %d", pi_pItemInfo->dwCurDurability, pi_pItemInfo->dwRepairDurability, pi_pItemInfo->dwMaxDurability );

		if( pi_pItemInfo->dwCurDurability < pi_pItemInfo->dwMaxDurability * 0.1 )
		{
			--g_byLineCnt;
			SetTooltipColorStrBuffer( g_bySubTitleStrMaxLen + 2, UNAVAILABLE_RGB, "%d", pi_pItemInfo->dwCurDurability );
			++g_byLineCnt;
			SetTooltipNextLine( "수리여부", "가능" );
		}
		else
			SetTooltipNextLine( "수리여부", "불가" );
	}
	else
	{
		SetTooltipNextLine( "내구도", "제한없음" );
	}

	// 방어력
	if( pi_pItemInfo->dwDeffensePower[0] < pi_pItemInfo->dwDeffensePower[1] )
	{
		SetTooltipColorStrBuffer( g_bySubTitleStrMaxLen + 2, UNAVAILABLE_RGB, "%d", pi_pItemInfo->dwDeffensePower[1] );
	}
	SetTooltipNextLine( "방어력", "%d", pi_pItemInfo->dwDeffensePower[1] );

	// 내성
	SetTooltipTolerance( pi_pItemInfo->byToleranceType, pi_pItemInfo->dwTolerance );

	// 업그레이드
	g_stTooltipUpgradeInfo.byTotalSocketNum = 0;
	if( pi_pItemInfo->byUpgradeSocketNo > 0 )
	{
		SetTooltipNextLine( "업그레이드", NULL );
		
		g_stTooltipUpgradeInfo.byTotalSocketNum	= pi_pItemInfo->byUpgradeSocketNo;
		g_stTooltipUpgradeInfo.byTotalItemNum	= pi_pItemInfo->wUpgradeLevel;

		memcpy( g_stTooltipUpgradeInfo.dwSpriteIDList, pi_pItemInfo->dwUpgradeSpriteID, g_stTooltipUpgradeInfo.byTotalItemNum * sizeof( DWORD ) );

		g_stTooltipUpgradeInfo.byUpgradeSocketLineIdx = g_byLineCnt-1;
		g_stTooltipUpgradeInfo.byUpgradeSocketCharIdx = g_bySubTitleStrMaxLen + 2;
	}

	// 설명
	SetTooltipNextLine( "[설명]", NULL );
	SetStringtoLine( 2, 36, pi_pItemInfo->pDescription );

	// 가격 정보
	SetTooltipPriceInfo( pi_byShopMode, pi_pItemInfo->byPriceType, pi_pItemInfo->dwPrice, 1 );
	
	// set tooltip
	gScreen.SetTooltip_Item( pi_pItem, g_byLineCnt, g_ppTooltipBuffer, g_byColorStrCnt, g_stTooltipColorStrInfo, &g_stTooltipUpgradeInfo );
}


void
SetTooltip_Shield( CGUIItem * pi_pItem, TOOLTIP_ARMOR * pi_pItemInfo, BYTE pi_byShopMode )
{
	ClearTooltipBuffer();
	g_bySubTitleStrMaxLen = 11;

	// 이름
	SetTooltipNextLine( pi_pItemInfo->pNameStr, NULL );
	// 종류
	SetTooltipNextLine( "종류", "%s", GetItemTypeName( pi_pItem->GetItemID() ) );

	// 요구레벨
	if( pi_pItemInfo->wLevel > g_pUiCharInfoWindow->GetLevel() )
	{
		SetTooltipColorStrBuffer( g_bySubTitleStrMaxLen + 2, UNAVAILABLE_RGB, "%d", pi_pItemInfo->wLevel );
	}
	SetTooltipNextLine( "요구 레벨", "%d", pi_pItemInfo->wLevel );

	// 잡는 유형
	SetTooltipNextLine( "잡는 유형", "%s", GetGrapsTypeName( pi_pItem->GetItemID() ) );

	// 사용 종족
	BYTE byItemRaceType = GetRaceType( pi_pItem->GetItemID() );
	BYTE byAvatarRaceType = GetRace( g_pUiCharInfoWindow->GetRaceType() );
	if( GetRace( byItemRaceType ) != RT_NONE && GetRace( byItemRaceType ) != byAvatarRaceType )
	{
		SetTooltipColorStrBuffer( g_bySubTitleStrMaxLen + 2, UNAVAILABLE_RGB, "%s", GetRaceTypeName( pi_pItem->GetItemID() ) );
	}
	SetTooltipNextLine( "사용 종족", "%s", GetRaceTypeName( pi_pItem->GetItemID() ) );

	// 성별
	char * pSexStr = GetSexTypeName( pi_pItem->GetItemID() );
	if( pSexStr )
	{
		if( GetSexType( byItemRaceType ) != ST_NONE &&	
			GetSexType( byItemRaceType ) != GetSexType( g_pUiCharInfoWindow->GetRaceType() ) )
		{
			SetTooltipColorStrBuffer( g_bySubTitleStrMaxLen + 2, UNAVAILABLE_RGB, "%s", pSexStr );
		}
	}
	SetTooltipNextLine( "성별", "%s", pSexStr );

	// 요구 숙련도
	SetTooltipNeededMastery( pi_pItemInfo->m_byNeedMastery, byAvatarRaceType );
	
	// 내구도
	if( pi_pItemInfo->bRepairable )
	{
		SetTooltipNextLine( "내구도", "%d / %d / %d", pi_pItemInfo->dwCurDurability, pi_pItemInfo->dwRepairDurability, pi_pItemInfo->dwMaxDurability );

		if( pi_pItemInfo->dwCurDurability < pi_pItemInfo->dwMaxDurability * 0.1 )
		{
			--g_byLineCnt;
			SetTooltipColorStrBuffer( g_bySubTitleStrMaxLen + 2, UNAVAILABLE_RGB, "%d", pi_pItemInfo->dwCurDurability );
			++g_byLineCnt;
			SetTooltipNextLine( "수리여부", "가능" );
		}
		else
			SetTooltipNextLine( "수리여부", "불가" );
	}
	else
	{
		SetTooltipNextLine( "내구도", "제한없음" );
	}

	// 방어력
	if( pi_pItemInfo->dwDeffensePower[0] < pi_pItemInfo->dwDeffensePower[1] )
	{
		SetTooltipColorStrBuffer( g_bySubTitleStrMaxLen + 2, UNAVAILABLE_RGB, "%d", pi_pItemInfo->dwDeffensePower[1] );
	}
	SetTooltipNextLine( "방어력", "%d", pi_pItemInfo->dwDeffensePower[1] );

	// 업그레이드
	g_stTooltipUpgradeInfo.byTotalSocketNum = 0;
	if( pi_pItemInfo->byUpgradeSocketNo > 0 )
	{
		SetTooltipNextLine( "업그레이드", NULL );
		
		g_stTooltipUpgradeInfo.byTotalSocketNum	= pi_pItemInfo->byUpgradeSocketNo;
		g_stTooltipUpgradeInfo.byTotalItemNum	= pi_pItemInfo->wUpgradeLevel;

		memcpy( g_stTooltipUpgradeInfo.dwSpriteIDList, pi_pItemInfo->dwUpgradeSpriteID, g_stTooltipUpgradeInfo.byTotalItemNum * sizeof( DWORD ) );

		g_stTooltipUpgradeInfo.byUpgradeSocketLineIdx = g_byLineCnt-1;
		g_stTooltipUpgradeInfo.byUpgradeSocketCharIdx = g_bySubTitleStrMaxLen + 2;
	}

	// 설명
	SetTooltipNextLine( "[설명]", NULL );
	SetStringtoLine( 2, 36, pi_pItemInfo->pDescription );
	
	// 가격 정보
	SetTooltipPriceInfo( pi_byShopMode, pi_pItemInfo->byPriceType, pi_pItemInfo->dwPrice, 1 );
	
	// set tooltip
	gScreen.SetTooltip_Item( pi_pItem, g_byLineCnt, g_ppTooltipBuffer, g_byColorStrCnt, g_stTooltipColorStrInfo, &g_stTooltipUpgradeInfo );
}

void	
SetTooltip_Bullet( CGUIItem * pi_pItem, TOOLTIP_BULLET * pi_pItemInfo, BYTE pi_byShopMode )
{
	ClearTooltipBuffer();
	g_bySubTitleStrMaxLen = 12;

	// 이름
	SetTooltipNextLine( pi_pItemInfo->pNameStr, NULL );
	// 종류
	SetTooltipNextLine( "종류", "%s", GetItemTypeName( pi_pItem->GetItemID() ) );
	// 잔량
	SetTooltipNextLine( "잔량", "%d", pi_pItemInfo->dwRestQuantity );
	// 공격력
	SetTooltipNextLine( "공격력", "%d ~ %d", pi_pItemInfo->dwMinAttackPower, pi_pItemInfo->dwMaxAttackPower );

	// 사용가능 무기
	SetTooltipUsableWeapon( 18, pi_pItemInfo->pUsableWeapon );

	//속성
	SetTooltipNextLine( "속성", "%s", GetItemAttributeName( pi_pItemInfo->byAttribute ) );
	// 부가 속성
//	SetTooltipNextLine( "부가 속성", NULL );
	
	// 설명
	SetTooltipNextLine( "[설명]", NULL );
	SetStringtoLine( 2, 36, pi_pItemInfo->pDescription );

	// 가격 정보
	SetTooltipPriceInfo( pi_byShopMode, pi_pItemInfo->byPriceType, pi_pItemInfo->dwPrice, 1 );
	
	// set tooltip
	gScreen.SetTooltip_Item( pi_pItem, g_byLineCnt, g_ppTooltipBuffer, g_byColorStrCnt, g_stTooltipColorStrInfo );
}

void
SetTooltip_Potion( CGUIItem * pi_pItem, TOOLTIP_POTION * pi_pItemInfo, BYTE pi_byShopMode )
{
	ClearTooltipBuffer();
	g_bySubTitleStrMaxLen = 9;

	// 이름
	SetTooltipNextLine( pi_pItemInfo->pNameStr, NULL );
	// 종류
	SetTooltipNextLine( "종류", "%s", GetItemTypeName( pi_pItem->GetItemID() ) );

	// 사용 종족
	BYTE byItemRaceType = GetRaceType( pi_pItem->GetItemID() );
	BYTE byAvatarRaceType = GetRace( g_pUiCharInfoWindow->GetRaceType() );
	if( GetRace( byItemRaceType ) != RT_NONE && GetRace( byItemRaceType ) != byAvatarRaceType )
	{
		SetTooltipColorStrBuffer( g_bySubTitleStrMaxLen + 2, UNAVAILABLE_RGB, "%s", GetRaceTypeName( pi_pItem->GetItemID() ) );
	}
	SetTooltipNextLine( "사용 종족", "%s", GetRaceTypeName( pi_pItem->GetItemID() ) );

	// 회복량
	SetTooltipNextLine( "회복량", "%d", pi_pItemInfo->dwReoveryPoint );
	// 수량
	if( pi_byShopMode != SHOP_MODE_BUY )
		SetTooltipNextLine( "수량", "%d", pi_pItemInfo->wQuantity );

	// 설명
	SetTooltipNextLine( "[설명]", NULL );
	SetStringtoLine( 2, 36, pi_pItemInfo->pDescription );

	// 가격 정보
	SetTooltipPriceInfo( pi_byShopMode, pi_pItemInfo->byPriceType, pi_pItemInfo->dwPrice, pi_pItemInfo->wQuantity );

	// set tooltip
	gScreen.SetTooltip_Item( pi_pItem, g_byLineCnt, g_ppTooltipBuffer, g_byColorStrCnt, g_stTooltipColorStrInfo );
}

void
SetTooltip_Battery( CGUIItem * pi_pItem, TOOLTIP_BATTERY * pi_pItemInfo, BYTE pi_byShopMode )
{
	ClearTooltipBuffer();
	g_bySubTitleStrMaxLen = 5;

	// 이름
	SetTooltipNextLine( pi_pItemInfo->pNameStr, NULL );
	// 종류
	SetTooltipNextLine( "종류", "%s", GetItemTypeName( pi_pItem->GetItemID() ) );
	// 잔량
	SetTooltipNextLine( "잔량", "%d", pi_pItemInfo->dwQuantity );

	// 설명
	SetTooltipNextLine( "[설명]", NULL );
	SetStringtoLine( 2, 36, pi_pItemInfo->pDescription );

	// 가격 정보
	SetTooltipPriceInfo( pi_byShopMode, pi_pItemInfo->byPriceType, pi_pItemInfo->dwPrice, 1 );

	// set tooltip
	gScreen.SetTooltip_Item( pi_pItem, g_byLineCnt, g_ppTooltipBuffer, g_byColorStrCnt, g_stTooltipColorStrInfo );
}

void
SetTooltip_Accessory( CGUIItem * pi_pItem, TOOLTIP_ACCESSORY * pi_pItemInfo, BYTE pi_byShopMode )
{
	ClearTooltipBuffer();
	g_bySubTitleStrMaxLen = 9;

	// 이름
	SetTooltipNextLine( pi_pItemInfo->pNameStr, NULL );
	// 종류
	SetTooltipNextLine( "종류", "%s", GetItemTypeName( pi_pItem->GetItemID() ) );

	// 요구레벨
	if( pi_pItemInfo->wLevel > g_pUiCharInfoWindow->GetLevel() )
	{
		SetTooltipColorStrBuffer( g_bySubTitleStrMaxLen + 2, UNAVAILABLE_RGB, "%d", pi_pItemInfo->wLevel );
	}
	SetTooltipNextLine( "요구 레벨", "%d", pi_pItemInfo->wLevel );

	// 종족 제한
	BYTE byItemRaceType = GetRaceType( pi_pItem->GetItemID() );
	BYTE byAvatarRaceType = GetRace( g_pUiCharInfoWindow->GetRaceType() );
	if( GetRace( byItemRaceType ) != RT_NONE && GetRace( byItemRaceType ) != byAvatarRaceType )
	{
		SetTooltipColorStrBuffer( g_bySubTitleStrMaxLen + 2, UNAVAILABLE_RGB, "%s", GetRaceTypeName( pi_pItem->GetItemID() ) );
	}
	SetTooltipNextLine( "사용 종족", "%s", GetRaceTypeName( pi_pItem->GetItemID() ) );
	

	// 성별
	// 아크레시아 아이템이면 성별은 출력하지 않는다.
	char * pSexStr = GetSexTypeName( pi_pItem->GetItemID() );
	if( pSexStr )
	{
		if( GetSexType( byItemRaceType ) != ST_NONE &&	
			GetSexType( byItemRaceType ) != GetSexType( g_pUiCharInfoWindow->GetRaceType() ) )
		{
			SetTooltipColorStrBuffer( g_bySubTitleStrMaxLen + 2, UNAVAILABLE_RGB, "%s", pSexStr );
		}
	}
	SetTooltipNextLine( "성별", "%s", pSexStr );

	// 내성
	SetTooltipTolerance( pi_pItemInfo->byToleranceType, pi_pItemInfo->dwTolerance );

	// 설명
	SetTooltipNextLine( "[설명]", NULL );
	SetStringtoLine( 2, 36, pi_pItemInfo->pDescription );

	// 가격 정보
	SetTooltipPriceInfo( pi_byShopMode, pi_pItemInfo->byPriceType, pi_pItemInfo->dwPrice, 1 );

	// set tooltip
	gScreen.SetTooltip_Item( pi_pItem, g_byLineCnt, g_ppTooltipBuffer, g_byColorStrCnt, g_stTooltipColorStrInfo );
}

void
SetTooltip_ForceItem( CGUIItem * pi_pItem, TOOLTIP_FORCE_ITEM * pi_pItemInfo, BYTE pi_byShopMode )
{
	ClearTooltipBuffer();
	g_bySubTitleStrMaxLen = 9;

	// 이름
	SetTooltipNextLine( pi_pItemInfo->pNameStr, NULL );
	// 계열
	SetTooltipNextLine( "계열", "%s", GetItemTypeName( pi_pItem->GetItemID() ) );

	// 요구레벨
	if( pi_pItemInfo->wLevel > g_pUiCharInfoWindow->GetLevel() )
	{
		SetTooltipColorStrBuffer( g_bySubTitleStrMaxLen + 2, UNAVAILABLE_RGB, "%d", pi_pItemInfo->wLevel );
	}
	SetTooltipNextLine( "요구 레벨", "%d", pi_pItemInfo->wLevel );

	++g_byLineCnt;

	// 설명
	SetTooltipNextLine( "[설명]", NULL );
	SetStringtoLine( 2, 36, pi_pItemInfo->pDescription );

	// 가격 정보
	SetTooltipPriceInfo( pi_byShopMode, pi_pItemInfo->byPriceType, pi_pItemInfo->dwPrice, 1 );

	// set tooltip
	gScreen.SetTooltip_Item( pi_pItem, g_byLineCnt, g_ppTooltipBuffer, g_byColorStrCnt, g_stTooltipColorStrInfo );
}

void	
SetTooltip_Mining( CGUIItem * pi_pItem, TOOLTIP_WEAPON * pi_pItemInfo, BYTE pi_byShopMode )
{
	ClearTooltipBuffer();
	g_bySubTitleStrMaxLen = 9;

	// 이름
	SetTooltipNextLine( pi_pItemInfo->pNameStr, NULL );

	// 종족 제한
	BYTE byItemRaceType = GetRaceType( pi_pItem->GetItemID() );
	BYTE byAvatarRaceType = GetRace( g_pUiCharInfoWindow->GetRaceType() );
	if( GetRace( byItemRaceType ) != RT_NONE && GetRace( byItemRaceType ) != byAvatarRaceType )
	{
		SetTooltipColorStrBuffer( g_bySubTitleStrMaxLen + 2, UNAVAILABLE_RGB, "%s", GetRaceTypeName( pi_pItem->GetItemID() ) );
	}
	SetTooltipNextLine( "사용 종족", "%s", GetRaceTypeName( pi_pItem->GetItemID() ) );
	
	// 내구도
	if( pi_pItemInfo->bRepairable )
	{
		SetTooltipNextLine( "내구도", "%d / %d / %d", pi_pItemInfo->dwCurDurability, pi_pItemInfo->dwRepairDurability, pi_pItemInfo->dwMaxDurability );

		if( pi_pItemInfo->dwCurDurability < pi_pItemInfo->dwMaxDurability * 0.1 )
		{
			--g_byLineCnt;
			SetTooltipColorStrBuffer( g_bySubTitleStrMaxLen + 2, UNAVAILABLE_RGB, "%d", pi_pItemInfo->dwCurDurability );
			++g_byLineCnt;
			SetTooltipNextLine( "수리여부", "가능" );
		}
		else
			SetTooltipNextLine( "수리여부", "불가" );
	}
	else
	{
		SetTooltipNextLine( "내구도", "제한없음" );
	}

	++g_byLineCnt;

	// 설명
	SetTooltipNextLine( "[설명]", NULL );
	SetStringtoLine( 2, 36, pi_pItemInfo->pDescription );

	// 가격 정보
	SetTooltipPriceInfo( pi_byShopMode, pi_pItemInfo->byPriceType, pi_pItemInfo->dwPrice, 1 );

	// set tooltip
	gScreen.SetTooltip_Item( pi_pItem, g_byLineCnt, g_ppTooltipBuffer, g_byColorStrCnt, g_stTooltipColorStrInfo );
}

void	
SetTooltip_UnitItem( CGUIItem * pi_pItem, TOOLTIP_UNIT_ITEM * pi_pItemInfo, BYTE pi_byShopMode )
{
	ClearTooltipBuffer();
	g_bySubTitleStrMaxLen = 9;

	// 이름
	SetTooltipNextLine( pi_pItemInfo->pNameStr, NULL );

	// 종족 제한
	BYTE byItemRaceType = GetRaceType( pi_pItem->GetItemID() );
	BYTE byAvatarRaceType = GetRace( g_pUiCharInfoWindow->GetRaceType() );
	if( GetRace( byItemRaceType ) != RT_NONE && GetRace( byItemRaceType ) != byAvatarRaceType )
	{
		SetTooltipColorStrBuffer( g_bySubTitleStrMaxLen + 2, UNAVAILABLE_RGB, "%s", GetRaceTypeName( pi_pItem->GetItemID() ) );
	}
	SetTooltipNextLine( "사용 종족", "%s", GetRaceTypeName( pi_pItem->GetItemID() ) );

	// 공격타입
	SetTooltipNextLine( "공격타입", "%s", GetAttackType( pi_pItemInfo->byAttackType ) );
	
	// 이동속도
	SetTooltipNextLine( "이동속도", "%s", pi_pItemInfo->pMoveSpeed );

	// 공격력
	SetTooltipNextLine( "공격력", "%d ~ %d", pi_pItemInfo->dwMinAttackPower, pi_pItemInfo->dwMaxAttackPower );
	
	// 방어력
	SetTooltipNextLine( "방어력", "%d", pi_pItemInfo->dwDeffensePower );

	// 내성
	SetTooltipTolerance( pi_pItemInfo->byToleranceType, pi_pItemInfo->dwTolerance );

	// 내구도
	SetTooltipNextLine( "내구도", "%d / %d", pi_pItemInfo->dwCurDurability, pi_pItemInfo->dwMaxDurability );

	// 설명
	SetTooltipNextLine( "[설명]", NULL );
	SetStringtoLine( 2, 36, pi_pItemInfo->pDescription );

	// 가격 정보
	SetTooltipPriceInfo( pi_byShopMode, pi_pItemInfo->byPriceType, pi_pItemInfo->dwPrice, 1 );

	// set tooltip
	gScreen.SetTooltip_Item( pi_pItem, g_byLineCnt, g_ppTooltipBuffer, g_byColorStrCnt, g_stTooltipColorStrInfo );
}

void
SetTooltip_Etc( CGUIItem * pi_pItem, TOOLTIP_ETC_ITEM * pi_pItemInfo, BYTE pi_byShopMode, BOOL pi_bExistQuantity )
{
	ClearTooltipBuffer();
	g_bySubTitleStrMaxLen = 5;

	// 이름
	SetTooltipNextLine( pi_pItemInfo->pNameStr, NULL );
	// 종류
	SetTooltipNextLine( "종류", "%s", GetItemTypeName( pi_pItem->GetItemID() ) );

	// 수량
	if( pi_byShopMode != SHOP_MODE_NONE || !pi_bExistQuantity )
		++g_byLineCnt;
	else if( pi_bExistQuantity )
		SetTooltipNextLine( "수량", "%d", pi_pItemInfo->dwQuantity );

	++g_byLineCnt;

	// 설명
	SetTooltipNextLine( "[설명]", NULL );
	SetStringtoLine( 2, 36, pi_pItemInfo->pDescription );

	// 가격 정보
	if( pi_bExistQuantity )
		SetTooltipPriceInfo( pi_byShopMode, pi_pItemInfo->byPriceType, pi_pItemInfo->dwPrice, pi_pItemInfo->dwQuantity );
	else
		SetTooltipPriceInfo( pi_byShopMode, pi_pItemInfo->byPriceType, pi_pItemInfo->dwPrice, 1 );

	// set tooltip
	gScreen.SetTooltip_Item( pi_pItem, g_byLineCnt, g_ppTooltipBuffer, g_byColorStrCnt, g_stTooltipColorStrInfo );
}

void	
SetTooltip_AnimusLiver( CGUIItem * pi_pItem, TOOLTIP_ANIMUS_LIVER_ITEM * pi_pItemInfo, BYTE pi_byShopMode )
{
	ClearTooltipBuffer();
	g_bySubTitleStrMaxLen = 5;

	// 이름
	SetTooltipNextLine( pi_pItemInfo->pNameStr, NULL );
	// 종류
	SetTooltipNextLine( "종류", "%s", GetItemTypeName( pi_pItem->GetItemID() ) );

	++g_byLineCnt;

	// 설명
	SetTooltipNextLine( "[설명]", NULL );
	SetStringtoLine( 2, 36, pi_pItemInfo->pDescription );

	// 가격 정보
	SetTooltipPriceInfo( pi_byShopMode, pi_pItemInfo->byPriceType, pi_pItemInfo->dwPrice, 1 );

	// set tooltip
	gScreen.SetTooltip_Item( pi_pItem, g_byLineCnt, g_ppTooltipBuffer, g_byColorStrCnt, g_stTooltipColorStrInfo );
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

enum { PARTS_HEAD, PARTS_UPPER, PARTS_LOWER, PARTS_ARMS, PARTS_SHOULDER, PARTS_BACK, BULLET };

void
SetTooltip_UnitParts( CGUIItem * pi_pItem, TOOLTIP_UNIT_PARTS * pi_pItemInfo, BYTE pi_byShopMode )
{
	ClearTooltipBuffer();
	g_bySubTitleStrMaxLen = 10;

	// 이름
	SetTooltipNextLine( pi_pItemInfo->pNameStr, NULL );
	
	// 요구레벨
	if( pi_pItemInfo->byEquableLevel > g_pUiCharInfoWindow->GetLevel() )
	{
		SetTooltipColorStrBuffer( g_bySubTitleStrMaxLen + 2, UNAVAILABLE_RGB, "%d", pi_pItemInfo->byEquableLevel );
	}
	SetTooltipNextLine( "요구 레벨", "%d", pi_pItemInfo->byEquableLevel );
	
	if( pi_pItemInfo->byPartsType == PARTS_ARMS || pi_pItemInfo->byPartsType == PARTS_SHOULDER )
	{
		// 공격타입
		SetTooltipNextLine( "공격타입", "%s", GetAttackType( pi_pItemInfo->byAttackType ) );
		
		// 공격력
		SetTooltipNextLine( "공격력", "%d ~ %d", pi_pItemInfo->dwMinAttackPower, pi_pItemInfo->dwMaxAttackPower );

		// 공격 딜레이
		SetTooltipNextLine( "공격딜레이", "%d", pi_pItemInfo->dwAttackDelay );

		// 속성
		SetTooltipNextLine( "속성", "%d", pi_pItemInfo->byAttribute );
	}
	else if( pi_pItemInfo->byPartsType == PARTS_HEAD ||
			 pi_pItemInfo->byPartsType == PARTS_UPPER ||
			 pi_pItemInfo->byPartsType == PARTS_LOWER )
	{
		// 방어력
		SetTooltipNextLine( "방어력", "%d", pi_pItemInfo->dwDeffensePower );
	}

	// 내성
	SetTooltipTolerance( pi_pItemInfo->byToleranceType, pi_pItemInfo->dwTolerance );

	if( pi_pItemInfo->byPartsType == PARTS_BACK )
	{
		SetTooltipNextLine( "차지량", "%d", pi_pItemInfo->dwDurability );
		SetTooltipNextLine( "부스터 속도", "%d", pi_pItemInfo->byBoosterSpeed );
	}
	else if( pi_pItemInfo->byPartsType == BULLET )
	{
		SetTooltipNextLine( "잔량", "%d", pi_pItemInfo->dwDurability );
	}

	// 가격 정보
	SetTooltipPriceInfo( pi_byShopMode, pi_pItemInfo->byPriceType, pi_pItemInfo->dwPrice, 1 );

	// set tooltip
	gScreen.SetTooltip( g_byLineCnt, g_ppTooltipBuffer, g_byColorStrCnt, g_stTooltipColorStrInfo );
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
SetTooltip_Skill( CGUIItem * pi_pItem, TOOLTIP_SF * pi_pSFInfo )
{
	ClearTooltipBuffer();
	g_bySubTitleStrMaxLen = 12;
	
	// 이름
	SetTooltipNextLine( pi_pSFInfo->pNameStr, NULL );
	
	// 분류
	if( pi_pSFInfo->byClassID > DEFFENCE_MASTERY )
		SetTooltipNextLine( "스킬분류", "%s스킬", SKILL_CLASS_NAME[0] );
	else
		SetTooltipNextLine( "스킬분류", "%s스킬", SKILL_CLASS_NAME[pi_pSFInfo->byClassID] );

	// 전투 스킬일 때만 단계 표시
	if( pi_pSFInfo->byClassID < 5 )
		SetTooltipNextLine( "스킬단계", "%s", ATTACK_SKILL_LV_NAME[pi_pSFInfo->byLvID] );

	// 레벨
	SetTooltipNextLine( "레벨", "%d", pi_pSFInfo->byLevel );

	// 숙련도
	SetTooltipNextLine( "숙련도", "%d", pi_pSFInfo->wMastery );

	// 사용 가능 무기
	SetTooltipUsableWeapon( 10, pi_pSFInfo->pUsableWeapon );

	// 필요포스량
	SetTooltipNextLine( "숙련도", "%d", pi_pSFInfo->byNeedFP );

	// 딜레이
	SetTooltipNextLine( "딜레이", "%.1f", pi_pSFInfo->fDelay );

	// 설명
	SetTooltipNextLine( "[설명]", NULL );
	SetStringtoLine( 2, 36, pi_pSFInfo->pDescription );

	gScreen.SetTooltip( g_byLineCnt, g_ppTooltipBuffer, g_byColorStrCnt, g_stTooltipColorStrInfo );
}

void
SetTooltip_Force( CGUIItem * pi_pItem, TOOLTIP_SF * pi_pSFInfo )
{
	ClearTooltipBuffer();
	g_bySubTitleStrMaxLen = 12;

	// 이름
	SetTooltipNextLine( pi_pSFInfo->pNameStr, NULL );

	// 분류
	SetTooltipNextLine( "포스분류", "%s", ITEM_TYPE_NAME_FORCE[pi_pSFInfo->byClassID] );

	// 단계
	SetTooltipNextLine( "단계", "%s", FORCE_LV_NAME[pi_pSFInfo->byClassID*4 + pi_pSFInfo->byLvID] );

	// 레벨
	SetTooltipNextLine( "레벨", "%d", pi_pSFInfo->byLevel );

	// 숙련도
	SetTooltipNextLine( "숙련도", "%d", pi_pSFInfo->wMastery );

	// 사용 가능 무기
	SetTooltipUsableWeapon( 10, pi_pSFInfo->pUsableWeapon );

	// 필요포스량
	SetTooltipNextLine( "필요포스량", "%d", pi_pSFInfo->byNeedFP );

	// 딜레이
	SetTooltipNextLine( "딜레이", "%.1f", pi_pSFInfo->fDelay );

	// 속성
	SetTooltipNextLine( "속성", "%s", GetItemAttributeName( pi_pSFInfo->byAttribute ) );
	
	// 설명
	SetTooltipNextLine( "[설명]", NULL );
	SetStringtoLine( 2, 36, pi_pSFInfo->pDescription );
/*
	g_stTooltipColorStrInfo[l_wColorLineIndex].byLineIndex = l_wLineIndex;		
	sprintf( g_stTooltipColorStrInfo[l_wColorLineIndex++].pString, "        설명  " );
	if( pi_pSFInfo->pDescription[0] == '\0' )
		sprintf( g_ppTooltipBuffer[l_wLineIndex++], "\n" );
	else
		SetStringtoLine( 14, 36, pi_pSFInfo->pDescription );
*/
	gScreen.SetTooltip( g_byLineCnt, g_ppTooltipBuffer, g_byColorStrCnt, g_stTooltipColorStrInfo );
}

void
SetTooltip_SF_Belt( CGUIItem * pi_pItem, TOOLTIP_SF * pi_pSFInfo, BYTE pi_bySFType )
{
	ClearTooltipBuffer();
	g_bySubTitleStrMaxLen = 12;

	// 이름
	SetTooltipNextLine( pi_pSFInfo->pNameStr, NULL );

	// 스킬 - 계열
	if( pi_bySFType == 0 )
	{
		if( pi_pSFInfo->byClassID > DEFFENCE_MASTERY )
			SetTooltipNextLine( "계열", "%s스킬", SKILL_CLASS_NAME[0] );
		else
			SetTooltipNextLine( "계열", "%s스킬", SKILL_CLASS_NAME[pi_pSFInfo->byClassID] );
	}
	// 포스 - 속성
	else
	{
		SetTooltipNextLine( "속성", "%s", GetItemAttributeName( pi_pSFInfo->byAttribute ) );		
	}

	// 레벨
	SetTooltipNextLine( "레벨", "%d", pi_pSFInfo->byLevel );

	// 사용 가능 무기
	SetTooltipUsableWeapon( 10, pi_pSFInfo->pUsableWeapon );

	// 필요포스량
	SetTooltipNextLine( "필요포스량", "%d", pi_pSFInfo->byNeedFP );

	gScreen.SetTooltip( g_byLineCnt, g_ppTooltipBuffer, g_byColorStrCnt, g_stTooltipColorStrInfo );
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

static const char ANIMUS_TYPE[4][16] = { "근접 전투형", "원거리 전투형", "힐러형", "보조형" };
char *
GetAnimusTypeName( BYTE pi_byType )
{
	if( pi_byType >= 4 )
		return (char *)ANIMUS_TYPE[0];
	else 
		return (char *)ANIMUS_TYPE[pi_byType];
}

static const char ANIMUS_DEGREE[4][16] = { "Recruit", "Veteran", "Master", "Ami" };
char *
GetAnimusGradeName( BYTE pi_byDegree )
{
	if( pi_byDegree >= 4 )
		return (char *)ANIMUS_DEGREE[0];
	else 
		return (char *)ANIMUS_DEGREE[pi_byDegree];
}

void
SetTooltip_Animus( CGUIObject * pi_pObject, TOOLTIP_ANIMUS * pi_pAnimusInfo )
{
	ClearTooltipBuffer();
	g_bySubTitleStrMaxLen = 10;

	// 이름
	SetTooltipNextLine( pi_pAnimusInfo->pNameStr, NULL );
	
	// 레벨
	SetTooltipNextLine( "레벨", "%d", pi_pAnimusInfo->byLevel );

	// 애니머스 상태 등급
	SetTooltipNextLine( "등급", "%s", GetAnimusGradeName( pi_pAnimusInfo->byGrade ) );

	// 요구 숙련도
	SetTooltipNextLine( "요구숙련도", "%d", pi_pAnimusInfo->wUsableMastery );

	// 공격력
	SetTooltipNextLine( "공격력", "%d ~ %d", pi_pAnimusInfo->dwMinAttPower, pi_pAnimusInfo->dwMaxAttPower );

	// 방어력
	SetTooltipNextLine( "방어력", "%d", pi_pAnimusInfo->dwDeffensePower );

	// 내성
	SetTooltipTolerance( pi_pAnimusInfo->byToleranceType, pi_pAnimusInfo->dwTolerance );

	// 필요포스량
	SetTooltipNextLine( "필요포스량", "%d", pi_pAnimusInfo->byNeedFP );

	// 설명
	SetTooltipNextLine( "[설명]", NULL );
	SetStringtoLine( 2, 36, pi_pAnimusInfo->pDescription );
	

	gScreen.SetTooltip( g_byLineCnt, g_ppTooltipBuffer, g_byColorStrCnt, g_stTooltipColorStrInfo );
}

void
SetTooltip_Animus_Belt( CGUIObject * pi_pObject, TOOLTIP_ANIMUS * pi_pAnimusInfo )
{
	ClearTooltipBuffer();
	g_bySubTitleStrMaxLen = 10;

	// 이름
	SetTooltipNextLine( pi_pAnimusInfo->pNameStr, NULL );

	// 레벨
	SetTooltipNextLine( "레벨", "%d", pi_pAnimusInfo->byLevel );

	// 애니머스 상태 등급
	SetTooltipNextLine( "등급", "%s", GetAnimusGradeName( pi_pAnimusInfo->byGrade ) );

	// 필요포스량
	SetTooltipNextLine( "필요포스량", "%d", pi_pAnimusInfo->byNeedFP );

	gScreen.SetTooltip( g_byLineCnt, g_ppTooltipBuffer, g_byColorStrCnt, g_stTooltipColorStrInfo );
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
SetTooltip( CGUIObject * pi_pObject, char * pi_pStr )
{
	ClearTooltipBuffer();

	int x, y;
	x = y = 0;
	for( int i = 0; pi_pStr[i] != '\0'; ++i )
	{
		if( pi_pStr[i] == '\n' )
		{
			g_ppTooltipBuffer[y][x] = '\0';
			++y;
			x = 0;
		}
		else
		{
			g_ppTooltipBuffer[y][x++] = pi_pStr[i];
		}
	}

	g_ppTooltipBuffer[y][x] = '\0';
	if( x > 0 )
		++y;

	gScreen.SetTooltip( y, g_ppTooltipBuffer );
}