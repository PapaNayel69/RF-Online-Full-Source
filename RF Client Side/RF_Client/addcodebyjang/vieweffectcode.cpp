#include "ViewEffectCode.h"

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifdef _DEVELOPER_

static char MONSTER_EFFECT_NAME[8][32] =
{
	"데미지", "죽음", "광분", "생성", "크리티컬", "미스", "타격", "포스 시전"
};

static char PLAYER_EFFECT_NAME[6][32] =
{
	"데미지1", "데미지2", "레벨업", "생성", "크리티컬", "미스"
};

// 밀리, 보조, 제작, 소환 순서임
static char SKILL_EFFECT_NAME1[4][24][32] =
{
	{ "샤이닝컷", "블로우", "프릭", "데스블로우", "클라우트챱", "데스핵", "딥인져리", "프레셔", "슬러그플랫", "페이탈펜스", "라이트닝트릭", "스프릿어택", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL" },
	{ "와일드레이지", "아큐레이트", "익스텐딩", "에이드스트래칭", "컨세션", "디지", "불스아이", "슬램", "페인팅", "엑스슬램", "리지스트", "위닝히트", "퀘렐리로드", "와이드레인지", "프리시젼", "뷸릿리로드", "스나이프모드", "얼랙러티", "패스트리로드", "와이드사이트", "인덕션", "로켓리로드", "오버레인지", "NULL" },
	{ "무기 방패 제작", "방어구 제작", "발사물 제작", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL" },
	{ "소환", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL" },
};

static char SKILL_EFFECT_NAME2[2][32] =
{
	"발동", "데미지"
};

static char FORCE_EFFECT_NAME1[6][24][32] =
{
	{ "브로드아웃룩", "새크리파이스", "어프로칭", "마이티", "이그져션", "인크리즈 스피드", "설리러티", "헬 블레스", "이펙티브 컨슘", "업섭션", "트랜스페런시", "리바이벌", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL" },
	{ "힐링", "레스터레이션", "소울 발라드", "엘러멘틀 리지스트", "홀리 쉴드", "인크리즈 스피드", "어질러티", "블레스", "이펙티브 컨슘", "블러드 캡쳐", "앱솔루트 아머", "오버 힐링", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL" },
	{ "프리즘 빔", "파이어 피스", "파이어 볼", "플래쉬 빔", "플레임 애로우", "서클 플레임", "플레임 익스플루젼", "미러 스웜", "블레이즈 퍼얼", "솔라 블레이드", "블레이즈 랜스", "미티어 스웜", "솔라 오브 파워", "헬 소울", "헬파이어", "베네딕션 오브 솔라", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL" },
	{ "아이스 빔", "스파우트 미스트", "블라인드 사이트", "리듀스 리지스트", "프로스트 애로우", "아쿠아 블레이드", "위크니스", "실링", "레이지 오브 웨이브", "프로스트 노바", "블록", "덜링", "라버 아쿠아", "스노우 드리프트", "카운터랙션", "리미트 크리티컬", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL" },
	{ "베인 폴", "포이즌 스팅킹", "블리딩", "스핀 어 웹", "록 블레이드", "포이즌 브레스", "데설리티", "임모빌러티", "퓨즈 베넘 브레스", "샌드 스톰", "뮤트 소울", "블라인드 소울", "바이러스 디지즈", "그레벌 레인", "디스멘틀", "그로우 렉스", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL" },
	{ "이렉트릭 볼", "에어 스플린터", "쏘잉", "릴리즈", "일렉트릭 볼트", "에어 블래스트", "리프레싱", "디텍트", "거스트 오브 블래스트", "체인 라이트닝", "인디비쥬얼 캔슬레이션", "오펜시브 캔슬레이션", "그래버티 필드", "썬더 버드", "디펜시브 캔슬레이션", "슬래큰 비절런스", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL" },
};

static char FORCE_EFFECT_NAME2[3][32] =
{
	"시전", "발동", "데미지"
};

static char ITEM_EFFECT_NAME[6][32] =
{
	"기본", "타격", "속성", "사용", "스킬 타격", "포스 타격"
};

void
SettingEffectInfo( EffectInfo * pi_pEffectInfo, CCharacter * pi_pChar1, CCharacter * pi_pChar2 )
{
	char	l_pBuf[256], l_pName1[32], l_pName2[32];
	DWORD	l_dwEffectCode1, l_dwEffectCode2, l_dwEffectCode3, l_dwSFClass;

	if( !pi_pEffectInfo )
		return;

#ifdef _DEVELOPER_
	static int count = 0;
	//char l_pBuf[32];

	if( pi_pEffectInfo->dwID == 0x9568781 || pi_pEffectInfo->dwID == 0x9568788 )
	{
		sprintf( l_pBuf, "%d ) JangCode\n", ++count );
		OutputDebugString( l_pBuf );
	}
#endif


	if( pi_pChar1 )
		_strcpy( l_pName1, pi_pChar1->GetName(),20 );
	else
		_strcpy( l_pName1, "None",10 );

	if( pi_pChar2 )
		_strcpy( l_pName2, pi_pChar2->GetName(),20 );
	else
		_strcpy( l_pName2, "None",10 );

	if( pi_pChar1 )
	{
		if( pi_pEffectInfo->dwType == ETI_MONSTER )
		{
			l_dwEffectCode1 = ( pi_pEffectInfo->dwID & 0x00000FF0 ) >> 4;
			//sprintf( l_pBuf, "%X : %s ( MySelf : %s,Target : %s )",
			//		 pi_pEffectInfo->dwID, MONSTER_EFFECT_NAME[l_dwEffectCode1],
			//		 l_pName1, l_pName2 );
			sprintf( l_pBuf, "%X : %s",
					 pi_pEffectInfo->dwID, MONSTER_EFFECT_NAME[l_dwEffectCode1] );

			pi_pChar1->GetEtcInfo()->SetInfo( CIST_EFFECT, l_pBuf );
		}
		else if( pi_pEffectInfo->dwType == ETI_PLAYER )
		{
			l_dwEffectCode1 = ( pi_pEffectInfo->dwID & 0x00000FF0 ) >> 4;
			sprintf( l_pBuf, "%X : %s",
					 pi_pEffectInfo->dwID, PLAYER_EFFECT_NAME[l_dwEffectCode1] );

			pi_pChar1->GetEtcInfo()->SetInfo( CIST_EFFECT, l_pBuf );
		}
		else if( pi_pEffectInfo->dwType == ETI_SKILL )
		{
			l_dwSFClass		= ( pi_pEffectInfo->dwID & 0x0000F000 ) >> 12;
			if( l_dwSFClass == 0x0F )	// 문자열 출력을 위해서 하드코딩
				l_dwSFClass = 1;
			else if( l_dwSFClass == 0x05 )
				l_dwSFClass = 2;
			else if( l_dwSFClass == 0x06 )
				l_dwSFClass = 3;
			l_dwEffectCode1 = ( pi_pEffectInfo->dwID & 0x00000FF0 ) >> 4;
			l_dwEffectCode3 = ( pi_pEffectInfo->dwID & 0x0000000F );
			if( l_dwEffectCode3 < 7 )
				l_dwEffectCode2 = 0;
			else
				l_dwEffectCode2 = 1;
			//sprintf( l_pBuf, "%X : %s - %s - %d ( MySelf : %s,Target : %s )", pi_pEffectInfo->dwID,
			//		 SKILL_EFFECT_NAME1[l_dwSFClass][l_dwEffectCode1], SKILL_EFFECT_NAME2[l_dwEffectCode2], l_dwEffectCode3,
			//		 l_pName1, l_pName2 );
			sprintf( l_pBuf, "%X : %s - %s - %d", pi_pEffectInfo->dwID,
					 SKILL_EFFECT_NAME1[l_dwSFClass][l_dwEffectCode1], SKILL_EFFECT_NAME2[l_dwEffectCode2], l_dwEffectCode3 );

			pi_pChar1->GetEtcInfo()->SetInfo( CIST_EFFECT, l_pBuf );
		}
		else if( pi_pEffectInfo->dwType == ETI_FORCE )
		{
			l_dwSFClass		= ( pi_pEffectInfo->dwID & 0x000F0000 ) >> 16;
			l_dwEffectCode1 = ( pi_pEffectInfo->dwID & 0x0000FF00 ) >> 8;
			l_dwEffectCode3 = ( pi_pEffectInfo->dwID & 0x000000FF );
			if( l_dwEffectCode3 < 7 )
				l_dwEffectCode2 = 0;
			else if( l_dwEffectCode3 < 14 )
				l_dwEffectCode2 = 1;
			else
				l_dwEffectCode2 = 2;
			//sprintf( l_pBuf, "%X : %s - %s - %d ( MySelf : %s,Target : %s )", pi_pEffectInfo->dwID,
			//		 FORCE_EFFECT_NAME1[l_dwSFClass][l_dwEffectCode1], FORCE_EFFECT_NAME2[l_dwEffectCode2], l_dwEffectCode3,
			//		 l_pName1, l_pName2 );
			sprintf( l_pBuf, "%X : %s - %s - %d", pi_pEffectInfo->dwID,
					 FORCE_EFFECT_NAME1[l_dwSFClass][l_dwEffectCode1], FORCE_EFFECT_NAME2[l_dwEffectCode2], l_dwEffectCode3 );

			pi_pChar1->GetEtcInfo()->SetInfo( CIST_EFFECT, l_pBuf );
		}
		else if( pi_pEffectInfo->dwType == ETI_ITEM )
		{
			l_dwEffectCode1 = ( pi_pEffectInfo->dwID & 0x000000F0 ) >> 4;
			//sprintf( l_pBuf, "%X : %s ( MySelf : %s,Target : %s )",
			//		 pi_pEffectInfo->dwID, ITEM_EFFECT_NAME[l_dwEffectCode1],
			//		 l_pName1, l_pName2 );
			sprintf( l_pBuf, "%X : %s",
					 pi_pEffectInfo->dwID, ITEM_EFFECT_NAME[l_dwEffectCode1] );

			pi_pChar1->GetEtcInfo()->SetInfo( CIST_EFFECT, l_pBuf );
		}
	}
}

#endif