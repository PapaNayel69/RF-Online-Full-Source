// rfcontroldatabase.h: interface for the CRFControlDatabase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RFCONTROLDATABASE_H__608792C3_9F51_461E_9122_ECDA99F89671__INCLUDED_)
#define AFX_RFCONTROLDATABASE_H__608792C3_9F51_461E_9122_ECDA99F89671__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "rfnewdatabase.h"

struct _controldb_user_info_data
{
	DWORD	dwSerial;
	WORD	wGrade;
	DWORD	dwTotallogmin;
	char	szCreateTime[32];
	char	szCreateip[16];
	char	szLastloginTime[32];
	char	szLastlogofftime[32];
	char	szLastconnectip[16];
};

#define	max_character_count	30

struct _controldb_charater_info_data
{
	WORD	wCharacterCount;

	struct __controldb_charater_info
	{
		WORD	wWorldCode;
		char	szAvatorName[16];
		BYTE	bySlotIndex;
		BYTE	byRaceCode;
		BYTE	byLevel;
	};

	__controldb_charater_info	CharacterInfo[max_character_count];

	int size() { return sizeof(*this)-sizeof(__controldb_charater_info)*(max_character_count-wCharacterCount);}

};

/// ControlServer DB Class
class CRFControlDatabase   : public CRFNewDatabase
{
public:
	CRFControlDatabase();
	virtual ~CRFControlDatabase();

	/**
	Todo : 운영자의 계정정보를 가져옴
	Returns : 성공시 true, 실패시 false
	*/
	bool	Select_HostAccountInfo( char* id, char* password, bool* readable, bool* writable );

	/**
	Todo : 사용자의 계정정보를 가져옴
	Returns : 성공시 true, 실패시 false
	*/
	bool	Select_UserInfoEx( char* id, _controldb_user_info_data* pUserData );
	
	/**
	Todo : 동접자 정보를 가져옴
	Returns : 성공시 true, 실패시 false
	*/
	bool	Select_UserCountInfo( char* szStartDate, char* szEndDate, void* pUserCountData);

	/**
	Todo : 캐릭터 정보를 가져옴
	*/
	bool	Select_CharacterInfo( DWORD dwAccountSerial, _controldb_charater_info_data* pCharacterData ); 

	/**
	Todo : 캐릭터 Serial을 가져옴
	*/
	bool	Select_CharacterSerial( char* szCharacterName, DWORD* pSerial );

};

#endif // !defined(AFX_RFCONTROLDATABASE_H__608792C3_9F51_461E_9122_ECDA99F89671__INCLUDED_)
