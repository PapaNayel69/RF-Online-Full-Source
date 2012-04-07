// rfworlddatabase.h: interface for the CRFWorldDatabase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RFWORLDDATABASE_H__608792C3_9F51_461E_9122_ECDA99F89671__INCLUDED_)
#define AFX_RFWORLDDATABASE_H__608792C3_9F51_461E_9122_ECDA99F89671__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "rfnewdatabase.h"
#include "characterdb.h"

#define	max_character_count	100

struct _worlddb_character_array_info
{
	WORD	wCharacterCount;

	struct __worlddb_charater_info
	{
		WORD	wWorldCode;
		char	szAvatorName[16];
		BYTE	bySlotIndex;
		BYTE	byRaceCode;
		BYTE	byLevel;
		DWORD	dwSerial;
		BYTE	byDck;
	};

	__worlddb_charater_info	CharacterInfo[max_character_count];

	int size() { return sizeof(*this)-sizeof(__worlddb_charater_info)*(max_character_count-wCharacterCount);}
};

struct _worlddb_character_base_info
{
	char	szName[max_name_len+1];
	DWORD	dwSerial;
	BYTE	byRace;
	char	szClassCode[class_code_len+1];
	BYTE	bySlotIndex;
	BYTE	byLevel;
	DWORD	dwDalant;
	DWORD	dwGold;
	DWORD	dwBaseShape;
	DWORD	dwLastConnTime;
	char	szAccount[max_name_len+1];
	short	shEKArray[equip_fix_num];
	DWORD	dwEUArray[equip_fix_num];
};

struct _worlddb_character_general_info
{
	DWORD	dwHP;						// [HP]	:	HP					
	DWORD	dwFP;						// [FP]	:	FP					
	DWORD	dwSP;						// [SP]	:	SP					
	double	dExp;						// [Exp]	:	경험치				
	BYTE	byBagNum;					// [BagNum]	:	사용중인 가방갯수		
	BYTE	byMapCode;					// [Map]	:	시작할 맵코드			
	float	fStartPos[3];				// [X], [Y], [Z]	:	시작할 좌표			
	DWORD	dwTotalPlayMin;				// [TotalPlayMin]	:	총 플레이한 시간		
	char	szLeftResList[160];			// 처리못한 보석
	long	lPK[potion_belt_num];		// 포션창
	short	shLK[sf_linker_num];		// 스킬/포스창
	long	lEK[embellish_fix_num];		// 장신구창
	short	shED[embellish_fix_num];	// 장신구창
	long	lF[force_storage_num];		// 포스
	DWORD	dwWM[WP_CLASS_NUM];			// 일반공격계열
	DWORD	dwFM[force_mastery_num];	// 포스마스터리
	DWORD	dwSM[max_skill_num];		// 스킬마스터리
	DWORD	dwMI[MI_CLASS_NUM];			// 제작계열
	DWORD	dwSR;						//
	DWORD	dwDM;						//
	DWORD	dwPM;						//

	DWORD	dwAK[animus_storage_num];	// 애니머스 관련
	DWORD	dwAD[animus_storage_num];
	DWORD	dwAP[animus_storage_num];

	short	zClassHistory[class_history_num];
};

#define max_inven_item_count	100

struct _worlddb_inven_info
{
	long	lK[max_inven_item_count];
	DWORD	dwD[max_inven_item_count];
	DWORD	dwU[max_inven_item_count];
};

struct _worlddb_character_base_info_array
{
	WORD	wCharacterCount;

	_worlddb_character_base_info	CharacterInfo[max_character_count];
};


struct _worlddb_unit_info_array
{
	struct __worlddb_unit_info
	{
		BYTE	byFrame;					//Unit [F_%d]	:	프레임인덱스, 0xFF면 빈 슬롯
		DWORD	dwGauge;						//Unit [Gg_%d]	:	hp
		BYTE	byPart[UNIT_PART_NUM];		//Unit [H_%d], [U_%d], [L_%d], [A_%d], [S_%d], [B_%d]	:	파트정보
		DWORD	dwBullet[UNIT_BULLET_NUM];	//Unit [AB_%d], [SB_%d]:	탄창정보 (_unit_bullet_param로 변환) 0xFFFFFFFF면 빈탄창
		DWORD	dwSpare[max_unit_spare];	//Unit [Sp%d_%d]:	여분탄창정보 (_unit_bullet_param로 변환) 0xFFFFFFFF면 빈탄창
		int		nKeepingFee;				//Unit [KF_%d]	:	채무 보관료, 견인료
		int		nPullingFee;				//Unit [PF_%d]	:	채무 견인료 
		DWORD	dwCutTime;					//Unit [Cut_%d]	:	유닛띄운상태에서 비정상 종료시 그 시간
	};

	__worlddb_unit_info	UnitInfo[unit_storage_num];

};

struct _worlddb_economy_history_info_array
{
	WORD	wRowCount;
	
	struct _worlddb_economy_history_info
	{
		double	dTradeGold[RACE_NUM];			//총거래골드
		double	dTradeDalant[RACE_NUM];			//총거래달란트
		DWORD	dwManageValue;
	};

	_worlddb_economy_history_info EconomyData[MAX_ECONOMY_HISTORY];
};

struct _worlddb_item_list
{
	struct __item_charge_info
	{
		DWORD	dwItemCode_K;
		DWORD	dwItemCode_D;
		DWORD	dwItemCode_U;
		char	szDate[32];
	};

	BYTE	byItemCount;

	__item_charge_info itemList[32];
};

/// WorldDB Class
class CRFWorldDatabase   : public CRFNewDatabase
{
public:
	CRFWorldDatabase();
	virtual ~CRFWorldDatabase();

	/**
	Todo : 캐릭터들의 요약정보를 가져옴
	*/
	bool	Select_CharactersInfo( DWORD dwAccountSerial, _worlddb_character_array_info* OUT pCharacterData ); 

	/**
	Todo : 캐릭터 Serial을 가져옴
	*/
	bool	Select_CharacterSerial( char* szCharacterName, DWORD* pSerial );

	/**
	Todo : 캐릭터 Base 정보를 가져옴(캐릭터 이름으로)
	*/
	BYTE	Select_CharacterBaseInfoByName( char* szCharacterName, _worlddb_character_base_info* OUT pCharacterData );

	/**
	Todo : 캐릭터 General 정보를 가져옴
	*/
	BYTE	Select_CharacterGeneralInfo( DWORD dwCharacterSerial, _worlddb_character_general_info* OUT pCharacterData );

	/**
	Todo : 인벤토리 정보를 가져옴
	*/
	BYTE	Select_Inven( DWORD dwCharacterSerial, WORD wBagCount,  _worlddb_inven_info* OUT pInvenData );

	/**
	Todo : 캐릭터 Base 정보를 가져옴(AccountSerial로)
	*/
	BYTE	Select_CharacterBaseInfoBySerial( DWORD dwAccountSerial, _worlddb_character_base_info_array* OUT pCharacterDataArray );

	/**
	Todo : 캐릭터 Base 정보를 가져옴(캐릭터Serial로)
	*/
	BYTE	Select_CharacterBaseInfo( DWORD dwCharacterSerial, _worlddb_character_base_info* OUT pCharacterData );

	/**
	Todo : Unit 정보를 가져옴
	*/
	BYTE	Select_Unit( DWORD dwCharacterSerial, _worlddb_unit_info_array* OUT pUnitInfo );

	/**
	Todo : Unit 레코드를 생성
	*/
	bool	Insert_Unit( DWORD dwCharacterSerial );

	/**
	Todo : 동일한 이름이 존재하는지 
	Returns : 존재시 false
	*/
	bool	Select_Equal_Name( char* szCharacterName );

	/**
	Todo : 캐릭터 레코드를 생성(Base, General, inven, unit)
	*/
	bool	Insert_CharacterData( char* szCharacterName, char* szClassCode, DWORD dwAccountSerial, char* szAccount, BYTE bySlotIndex, BYTE byRaceSexCode, DWORD dwBaseShape, int nMapIndex, DWORD* OUT pDwSerial );

	/**
	Todo : 캐릭터 레코드를 지운다(실제로는 DCK=1로 세팅함)
	*/
	bool	Delete_CharacterData( DWORD dwCharacterSerial );

	/**
	Todo : 캐릭터 데이터들을 Update한다
	*/
	bool	Update_CharacterData( char* szBaseQuery, char* szGeneralQuery, char* szInvenQuery, char* szUnitQuery );

	/**
	Todo : 가장 최근의 MAX_ECONOMY_HISTORY(12)개의 economy history 데이터를 가져온다
	*/
	BYTE	Select_Economy_History( _worlddb_economy_history_info_array* OUT pEconomyData, DWORD dwDate );

	/**
	Todo : Economy History 데이터를 추가한다(INSERT or UPDATE)
	*/
	bool	Insert_Economy_History( DWORD dwDate, double* pdTradeDalant, double* pdTradeGold, int nMgrValue );

	/**
	Todo : dwDate 값의 데이터가 있는지 확인
	*/
	BYTE	Select_Exist_Economy( DWORD dwDate, _worlddb_economy_history_info_array::_worlddb_economy_history_info* OUT pEconomyData);

	/**
	Todo : 레벨업 로그를 추가한다(INSERT)
	*/
	bool	Insert_Level_Log( DWORD dwCharacterSerial, BYTE byLevel, DWORD dwTotalPlayMin );

	/**
	Todo : 동접자 로그를 추가한다(INSERT)
	*/
	bool	Insert_UserNum_Log( int nAvgUserNum, int nMaxUserNum );

	/**
	Todo : 삭제대기상태에 있는 캐릭터를 복구한다
	*/
	bool	Rebirth_Base( DWORD dwCharacterSerial, char* szName );

	/**
	Todo : 아이템 증여 목록에 추가한다
	*/
	bool	Insert_ItemCharge( DWORD dwAvatorSerial, DWORD dwItemCode_K, DWORD dwItemCode_D, DWORD dwItemCode_U );

	/**
	Todo : 아이템 증여 목록에서 하나만 가져온다
	*/
	bool	Select_ItemCharge( DWORD dwAvatorSerial, DWORD* pDwItemCode_K, DWORD* pDwItemCode_D, DWORD* pDwItemCode_U, DWORD* pDwItemChargeIndex );

	/**
	Todo : 아이템 증여 목록에서 하나를 지운다
	*/
	bool	Delete_ItemCharge( DWORD dwItemChargeIndex );

	/**
	Todo : 대기 아이템 목록을 가져온다
	*/
	BYTE	Select_WaitItem( DWORD dwAvatorSerial, _worlddb_item_list* itemList );

	/**
	Todo : 가져간 아이템 목록을 가져온다
	*/
	BYTE	Select_TakeItem( DWORD dwAvatorSerial, _worlddb_item_list* itemList );

	/**
	Todo : 캐릭터 이름으로 계정을 가져온다
	*/
	bool	Select_AccountByAvatorName( char* IN szAvatorName, char* OUT szAccount );






};

#endif // !defined(AFX_RFWORLDDATABASE_H__608792C3_9F51_461E_9122_ECDA99F89671__INCLUDED_)
