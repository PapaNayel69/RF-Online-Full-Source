// MsgData.cpp: implementation of the CMsgData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MsgData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMsgData::CMsgData(int nObjNum)
{
	Init(nObjNum);
}

CMsgData::CMsgData()
{
	m_gmBuf = NULL;
}

void CMsgData::Init(int nObjNum)
{
	int i;
	m_nObjNum = nObjNum;
	m_nMaxBufNum = nObjNum*100;

	m_gmBuf = new _message [m_nMaxBufNum];

	m_gmListEmptyHead.pNext = &m_gmListEmptyTail;
	m_gmListEmptyTail.pPrev = &m_gmListEmptyHead;
	m_gmListEmptyHead.pPrev = &m_gmListEmptyHead;
	m_gmListEmptyTail.pNext = &m_gmListEmptyTail;
	
	for(i = 0; i < m_nMaxBufNum; i++)
	{
		m_gmBuf[i].pNext = &m_gmListEmptyTail;
		m_gmBuf[i].pPrev = m_gmListEmptyTail.pPrev;

		m_gmListEmptyTail.pPrev->pNext = &m_gmBuf[i];
		m_gmListEmptyTail.pPrev = &m_gmBuf[i];
	}

	m_gmListHead.pNext = &m_gmListTail;
	m_gmListTail.pPrev = &m_gmListHead;
	m_gmListHead.pPrev = &m_gmListHead;
	m_gmListTail.pNext = &m_gmListTail;		
}

CMsgData::~CMsgData()
{
	delete [] m_gmBuf;
	m_gmBuf = NULL;
}

void CMsgData::PumpMsgList()
{
	if(!m_gmBuf)
		return;

	m_csList.Lock();

	while(1)
	{
		_message* pMsg = PopMsg();

		if(!pMsg)
			break;

		ProcessMessage(pMsg);
	
		PushEmptyBuf(pMsg);
	}

	m_csList.Unlock();
}

bool CMsgData::PackingMsg(DWORD dwMessage, DWORD dwKey1, DWORD dwKey2, DWORD dwKey3)
{
	_message* pMsg = PopEmptyBuf();
	if(!pMsg)
		return false;

	pMsg->SetMsg(dwMessage, dwKey1, dwKey2, dwKey3);
	PushMsg(pMsg);

	return true;
}

void CMsgData::PushMsg(_message* pMsg)
{
	m_csList.Lock();

	pMsg->pNext = &m_gmListTail;
	pMsg->pPrev = m_gmListTail.pPrev;

	m_gmListTail.pPrev->pNext = pMsg;
	m_gmListTail.pPrev = pMsg;

	m_csList.Unlock();
}

_message* CMsgData::PopMsg()
{
	m_csList.Lock();

	_message* pMsg = m_gmListHead.pNext;
	if(pMsg == &m_gmListTail)
	{
		m_csList.Unlock();
		return NULL;
	}

	m_gmListHead.pNext = pMsg->pNext;
	pMsg->pNext->pPrev = &m_gmListHead;

	m_csList.Unlock();
	return pMsg;
}

void CMsgData::PushEmptyBuf(_message* pMsg)
{
	m_csEmpty.Lock();

	pMsg->pPrev = &m_gmListEmptyHead;
	pMsg->pNext = m_gmListEmptyHead.pNext;

	m_gmListEmptyHead.pNext->pPrev = pMsg;
	m_gmListEmptyHead.pNext = pMsg;

	m_csEmpty.Unlock();
}

_message* CMsgData::PopEmptyBuf()
{
	m_csEmpty.Lock();

	_message* pBuf = m_gmListEmptyHead.pNext;
	if(pBuf == &m_gmListEmptyTail)
	{
		m_csEmpty.Unlock();
		return NULL;
	}

	m_gmListEmptyHead.pNext = pBuf->pNext;
	pBuf->pNext->pPrev = &m_gmListEmptyHead;

	m_csEmpty.Unlock();
	return pBuf;
}

