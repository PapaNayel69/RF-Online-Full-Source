// RFAccountDatabase.h: interface for the CRFAccountDatabase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RFACCOUNTDATABASE_H__FB304E98_1904_4CD6_973E_6AD3E4EDD389__INCLUDED_)
#define AFX_RFACCOUNTDATABASE_H__FB304E98_1904_4CD6_973E_6AD3E4EDD389__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "rfnewdatabase.h"

#define	block_list_nothing	1
#define	block_list_account	2
#define	block_list_chat		3

struct	_accountdb_user_base_info
{
	char	szCreatTime[32];
	char	szCreateIP[16];
	char	szLastLoginTime[32];
	char	szLastLogoffTime[32];
	DWORD	dwTotalPlayMTime;
	char	szLastLoginIP[16];

	WORD	wBanPeriod;
	char	szBanStartDate[32];
	BYTE	byBanKind;
};

struct _accountdb_user_ban_history_info
{
	struct	__ban_history_info
	{
		BYTE	byKind;
		WORD	wPeriod;
		char	szDate[32];
		char	szReason[32];
	};

	BYTE	byBanHistoryCount;

	__ban_history_info	banHistoryData[32];
};

struct _accoutndb_user_count_info
{
	WORD		wRowCount;

	struct __user_count_info 
	{
		WORD	wYear;
		BYTE	byMonth;
		BYTE	byDay;
		BYTE	byHour;
		DWORD	dwAvgCount;
		DWORD	dwMaxCount;
	};

	__user_count_info	UserCount[256];
};

/// AccountServer DB Class
class CRFAccountDatabase  : public CRFNewDatabase
{
public:
	CRFAccountDatabase();
	virtual ~CRFAccountDatabase();

	/**	
	Todo : User정보를 가져온다( Serial, TotalLogMin, Grade, SubGrade) <br>
	Returns : 성공시 true, 실패시 false
	*/
	BYTE	Select_UserInfo( char* id , DWORD* pSerial, DWORD* pTotalLogMin, BYTE* pGrade, BYTE* pSubGrade, char* loginDate );

	/**	
	Todo : User Serial을 가져온다 <br>
	Returns : 성공시 true, 실패시 false
	*/
	bool	Select_UserSerial( char* id, DWORD* pSerial, char* pDate );
	DWORD	Select_UserSerial( char* id );

	/**
	Todo : User를 추가하고 추가된 유저의 Serial을 가져온다 <br>
	Returns : 성공시 true, 실패시 false
	*/
	bool	Insert_User( char* id, char* ip );

	/**
	Todo : Login시간을 변경한다 <br>
	Returns : 성공시 true, 실패시 false
	*/
	bool	Update_UserLoginDate( char* id, char* ip );

	/**
	Todo : 이중접속으로 밀려난 시간, IP를 변경한다 <br>
	Returns : 성공시 true, 실패시 false
	*/
	bool	Update_UserPushDate( char* id, char* ip );

	/**
	Todo : LogOff 시간 및 총사용시간을 변경한다 <br>
	Returns : 성공시 true, 실패시 false
	*/
	bool	Update_UserLogoffDate( DWORD dwAccountSerial );

	/**	
	Todo : Dev Serial을 가져온다 <br>
	Returns : 성공시 true, 실패시 false
	*/
	bool	Select_DevSerial( char* id, DWORD* pSerial );

	/**
	Todo : Dev를 추가하고 추가된 Dev의 Serial을 가져온다 <br>
	Returns : 성공시 true, 실패시 false
	*/
	bool	Insert_Dev( char* id, char* pw, char* ip );

	/**	
	Todo : Dev 정보를 가져온다 <br>
	Returns : 성공시 true, 실패시 false
	*/
	bool	Select_DevInfo( char* id , char* pw, DWORD* pSerial, DWORD* pTotalLogMin, char* loginDate );

	/**
	Todo : Dev Login시간을 변경한다 <br>
	Returns : 성공시 true, 실패시 false
	*/
	bool	Update_DevLoginDate( char* id, char* ip );

	/**
	Todo : Dev LogOff 시간 및 총사용시간을 변경한다 <br>
	Returns : 성공시 true, 실패시 false
	*/
	bool	Update_DevLogoffDate( char* id, DWORD dwTotalLogMin );

	/**
	Todo : 접속자수 기록 <br>
	Returns : 성공시 true, 실패시 false
	*/
	bool	Insert_UserCountLog( DWORD averageUser, DWORD maxUser );

	/**
	Todo : 접속로그 기록 <br>
	Returns : 성공시 true, 실패시 false
	*/
	bool	Insert_UserConnLog( int serial, char* loginDate, char* ip );
	
	/**
	Todo : 이중접속으로 밀려난 로그를 기록
	Returns : 성공시 true, 실패시 false
	*/
	bool	Insert_UserPushLog( int serial, char* pushIP, char* closeIP );

	/**
	Todo : 운영자의 계정정보를 가져옴
	Returns : 성공시 true, 실패시 false
	*/
	bool	Select_HostAccountInfo( char* id, char* password, BYTE* byGrade );

	/**
	Todo : 사용자의 계정정보를 가져옴
	Returns : 성공시 true, 실패시 false
	*/
	bool	Select_UserInfoEx( char* id, _accountdb_user_base_info* OUT pUserData, DWORD* OUT dwAccountSerial );
	
	/**
	Todo : 동접자 정보를 가져옴
	Returns : 성공시 true, 실패시 false
	*/
	bool	Select_UserCountInfo( char* szStartDate, char* szEndDate, _accoutndb_user_count_info* OUT pUserCountData );

	/**
	Todo : 징계를 먹임(기존데이터 없을때)
	*/
	bool	Insert_UserBan( DWORD dwAccountSerial, BYTE byKind, DWORD dwPeriod, char* szBanReason );

	/**
	Todo : 징계정보를 가져옴
	*/
	BYTE	Select_UserBan( DWORD dwAccountSerial, BYTE* byKind, WORD* wPeriod, char* szStartdate );

	/**
	Todo : 징계정보를 삭제
	*/
	bool	Delete_UserBan( DWORD dwAccountSerial );

	/**
	Todo : 징계를 먹임(기존데이터 있을때)
	*/
	bool	Update_UserBan( DWORD dwAccountSerial, BYTE byKind, DWORD dwPeriod, char* szReason );

	/**
	Todo : 징계정보가 존재하는지 (만료기간이 지난 레코드 삭제 기능도 있음)
	Returns : 존재하면 true
	*/
	BYTE	Exist_UserBan( DWORD dwAccountSerial );

	/**
	Todo : 징계로그를 기록
	*/
	bool	Insert_UserBan_Log( DWORD dwAccountSerial, BYTE byKind, DWORD dwPeriod, char* szReason );

	/**
	Todo : 징계히스토리를 가져옴
	*/
	bool	Select_UserBan_History( DWORD dwAccountSerial, _accountdb_user_ban_history_info* banHistory );

	/**
	Todo : 운영툴 작업을 기록
	*/
	bool	Insert_GMJob( char* szGMID, BYTE nKind, char* szAvatorName, DWORD dwAvatorSerial, DWORD dwAccountSerial, char* szDetail );

	/**
	Todo : 월드서버 상태를 기록
	*/
	bool	Update_WorldServer( int nSerial, int nState );
	
};

#endif // !defined(AFX_RFACCOUNTDATABASE_H__FB304E98_1904_4CD6_973E_6AD3E4EDD389__INCLUDED_)
