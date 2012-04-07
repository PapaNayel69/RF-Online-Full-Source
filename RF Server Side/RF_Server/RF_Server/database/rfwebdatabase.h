// RFWebDatabase.h: interface for the CRFWebDatabase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RFWEBDATABASE_H__DA1FA8F9_4D52_4EAB_BAA4_BEFA46A30247__INCLUDED_)
#define AFX_RFWEBDATABASE_H__DA1FA8F9_4D52_4EAB_BAA4_BEFA46A30247__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "rfnewdatabase.h"

class CRFWebDatabase  : public CRFNewDatabase
{
public:
	CRFWebDatabase();
	virtual ~CRFWebDatabase();

	/**
	Todo : RFCloseWEb에서 User ID를 가져옴 <br>
	Returns : 성공시 true, 실패시 false
	*/
	bool	Select_AlphaAccountID( char* id );

	/**
	Todo : RFCloseWEb에서 Friends User ID를 가져옴 <br>
	Returns : 성공시 true, 실패시 false
	*/
	bool	Select_FriendsAccountID( char* id );

	/**
	Todo : X2Online DB에서 User Password를 가져옴 <br>
	Returns : 성공시 true, 실패시 false
	*/
	BYTE	Select_X2OnlineAccountPW( char* id, char* pw );

	/**
	Todo : PC방 IP목록에 있는지 확인해 줌
	Returns : 있으면 true, 없으면 false
	*/
	bool	Select_PcbangIP( BYTE* ip );

	/**
	Todo : 클로즈베타 계정검사 
	*/
	bool	Select_CloseBetaAccount( char* id );

};

#endif // !defined(AFX_RFWEBDATABASE_H__DA1FA8F9_4D52_4EAB_BAA4_BEFA46A30247__INCLUDED_)
