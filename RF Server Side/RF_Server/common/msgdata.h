// MsgData.h: interface for the CMsgData class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _MsgData_h_
#define _MsgData_h_

#include <afxmt.h>
#include "MyClassUtil.h"

struct _message
{
	DWORD dwMessage;
	DWORD dwKey1;
	DWORD dwKey2;
	DWORD dwKey3;

	_message* pNext;
	_message* pPrev;

	_message()
	{
		pNext = NULL;
		pPrev = NULL;
	};

	~_message()
	{
		ZeroMemory(this, sizeof(*this));
	}

	void SetMsg(DWORD message, DWORD key1 = 0, DWORD key2 = 0, DWORD key3 = 0)
	{
		dwMessage = message;
		dwKey1 = key1;
		dwKey2 = key2;
		dwKey3 = key3;
	};

	DWORD GetMessage()
	{
		return dwMessage;
	}

	DWORD GetKey1()
	{
		return dwKey1;
	}

	DWORD GetKey2()
	{
		return dwKey2;
	}

	DWORD GetKey3()
	{
		return dwKey3;
	}
};

class CMsgData  
{
private:
	int m_nObjNum;
	int m_nMaxBufNum;

	_message m_gmListHead;	
	_message m_gmListTail;	

	_message* m_gmBuf;
	_message m_gmListEmptyHead;
	_message m_gmListEmptyTail;

	CMyCriticalSection m_csList;
	CMyCriticalSection m_csEmpty;

public:
	CMsgData(int nObjNum);
	CMsgData();
	virtual ~CMsgData();

	void Init(int nObjNum);
	void PumpMsgList();
	bool PackingMsg(DWORD dwMessage, DWORD dwKey1 = 0, DWORD dwKey2 = 0, DWORD dwKey3 = 0);

private:
	void PushMsg(_message* pMsg);
	_message* PopMsg();

	void PushEmptyBuf(_message* pMsg);
	_message* PopEmptyBuf();

	virtual void ProcessMessage(_message* pMsg){}
};

#endif 
