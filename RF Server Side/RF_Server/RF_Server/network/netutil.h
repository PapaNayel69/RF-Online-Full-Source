#ifndef _NETUTIL_H_
#define _NETUTIL_H_

#include <winsock2.h>
#include <wsipx.h>
#include <stdio.h>
#include <time.h>
#include <mmsystem.h>

BOOL GetIPAddress(char* szAddr);
BOOL GetIPAddress(sockaddr_in* pAddr);
DWORD GetIPAddress();
void AtoH(char * src, char * dest, int destlen);
unsigned char BtoH(char ch);
void HtoA(char * src, char * dest, int srclen);
char HtoB(UCHAR ch);
bool GetSocketName(SOCKET socket, char* pOut);
void GetAddrString(sockaddr_ipx* pSAddr, char* dest);
bool IPConvertingStringToByte(char* szIP, BYTE* pbyIP);
void NetTrace(const char *fmt, ...);
int FindAllFile(char* pszDirectory, char** OUT ppszFileName, int nMax);

class CNetFrameRate  
{
public:
	DWORD m_dwFrames;

private:
	DWORD m_dwFrameTime;
	DWORD m_dwFrameCount;

public:
	CNetFrameRate()
	{
		m_dwFrameCount = 0;
	}

	void CalcFrameRate()
	{
		DWORD time, time2;

		time = timeGetTime();
		time2 = time - m_dwFrameTime;
		m_dwFrameCount++;

		if(time2 > 1000)
		{
			m_dwFrames = (m_dwFrameCount*1000)/time2;
			m_dwFrameTime = timeGetTime();
			m_dwFrameCount = 0;
		}			
	}
};

class CNetTimer  
{
private:
	int		m_nTickTerm;
	DWORD	m_dwTickOld;

public:
	bool m_bOper;

public:	
	CNetTimer()
	{
		m_bOper = false;
		m_dwTickOld = 0;
	}

	void BeginTimer(DWORD dwTerm)
	{
		m_bOper = true;
		m_nTickTerm = (int)dwTerm;	
		m_dwTickOld = timeGetTime();
	}

	bool CountingTimer()
	{
		if(!m_bOper)
			return false;

		DWORD dwNew = timeGetTime();
		int nGap = dwNew-m_dwTickOld;
		if(nGap < m_nTickTerm)
			return false;

		m_dwTickOld = dwNew;

		return true;
	}

	void AbortTimer()
	{
		m_bOper = false;
	}
};

class CNetCriticalSection
{
private:
	CRITICAL_SECTION m_cs;

public:
	CNetCriticalSection()
	{
		InitializeCriticalSection(&m_cs);		               
	}

	~CNetCriticalSection()
	{
		DeleteCriticalSection(&m_cs);
	}

	void Lock()
	{
		EnterCriticalSection(&m_cs);
	}

	void Unlock()
	{
		LeaveCriticalSection(&m_cs);
	}
};

class CNetIndexList
{
public:

	struct _index_node
	{
		DWORD m_dwIndex;

		_index_node* m_pPrev;
		_index_node* m_pNext;

		_index_node()
		{
			m_pPrev = NULL;
			m_pNext = NULL;
		}
	};

	_index_node m_Head;
	_index_node m_Tail;
	_index_node m_BufHead;
	_index_node m_BufTail;

	_index_node* m_pBufNode;

	CNetCriticalSection m_csList;

//public:
	DWORD m_dwCount;
	DWORD m_dwBufCount;
	DWORD m_dwMaxBufNum;

public:
	CNetIndexList()
	{
		m_Head.m_pPrev = &m_Head;
		m_Head.m_pNext = &m_Tail;
		m_Tail.m_pPrev = &m_Head;
		m_Tail.m_pNext = &m_Tail;

		m_BufHead.m_pPrev = &m_BufHead;
		m_BufHead.m_pNext = &m_BufTail;
		m_BufTail.m_pPrev = &m_BufHead;
		m_BufTail.m_pNext = &m_BufTail;

		m_dwCount = 0;
		m_dwBufCount = 0;
		m_dwMaxBufNum = 0;
		m_pBufNode = NULL;
	}

	~CNetIndexList()
	{
		if(m_pBufNode)
			delete [] m_pBufNode;
	}

	bool SetList(DWORD dwMaxBufNum = 10000)
	{
		if(m_pBufNode || m_dwMaxBufNum > 0)
			return false;
		
		m_dwMaxBufNum = dwMaxBufNum;
		m_pBufNode = new _index_node [m_dwMaxBufNum];

		for(DWORD n = 0; n < m_dwMaxBufNum; n++)
		{
			m_pBufNode[n].m_pNext = &m_BufTail;
			m_pBufNode[n].m_pPrev = m_BufTail.m_pPrev;
			m_BufTail.m_pPrev->m_pNext = &m_pBufNode[n];
			m_BufTail.m_pPrev = &m_pBufNode[n];

			m_dwBufCount++;
		}
		return true;
	}

	bool IsSetting()
	{
		if(m_pBufNode)
			return true;
		return false;
	}

	void ResetList()
	{
		while(1)
		{
			DWORD dwIndex;
			if(!PopNode_Front(&dwIndex))
				return;
		}
	}

	bool PopNode_Front(DWORD* pdwOutIndex)
	{
		m_csList.Lock();

		if(m_Head.m_pNext == &m_Tail)
		{
			m_csList.Unlock();
			return false;
		}

		_index_node* pNode = m_Head.m_pNext;
		*pdwOutIndex = pNode->m_dwIndex;

		m_Head.m_pNext = pNode->m_pNext;
		pNode->m_pNext->m_pPrev = &m_Head;
		m_dwCount--;

		pNode->m_pNext = &m_BufTail;
		pNode->m_pPrev = m_BufTail.m_pPrev;
		m_BufTail.m_pPrev->m_pNext = pNode;
		m_BufTail.m_pPrev = pNode;
		m_dwBufCount++;

		m_csList.Unlock();
		return true;
	}

	bool CopyFront(DWORD* pdwOutIndex)
	{
		m_csList.Lock();

		if(m_Head.m_pNext == &m_Tail)
		{
			m_csList.Unlock();
			return false;
		}

		_index_node* pNode = m_Head.m_pNext;
		*pdwOutIndex = pNode->m_dwIndex;

		m_csList.Unlock();
		return true;
	}
	
	bool PushNode_Back(DWORD dwIndex)
	{
		m_csList.Lock();

		if(m_BufHead.m_pNext == &m_BufTail)
		{
			m_csList.Unlock();
			return false;
		}

		_index_node* pNode = m_BufHead.m_pNext;

		m_BufHead.m_pNext = pNode->m_pNext;
		pNode->m_pNext->m_pPrev = &m_BufHead;
		m_dwBufCount--;
	
		pNode->m_dwIndex = dwIndex;		

		pNode->m_pNext = &m_Tail;
		pNode->m_pPrev = m_Tail.m_pPrev;
		m_Tail.m_pPrev->m_pNext = pNode;
		m_Tail.m_pPrev = pNode;
		m_dwCount++;

		m_csList.Unlock();
		return true;
	}

	_index_node* FindNode(DWORD dwIndex)
	{
		m_csList.Lock();

		_index_node* pNode = m_Head.m_pNext;

		while(pNode != &m_Tail)
		{
			if(pNode->m_dwIndex == dwIndex)
			{
				pNode->m_pPrev->m_pNext = pNode->m_pNext;
				pNode->m_pNext->m_pPrev = pNode->m_pPrev;
				m_dwCount--;

				pNode->m_pNext = &m_BufTail;
				pNode->m_pPrev = m_BufTail.m_pPrev;
				m_BufTail.m_pPrev->m_pNext = pNode;
				m_BufTail.m_pPrev = pNode;
				m_dwBufCount++;

				m_csList.Unlock();
				return pNode;
			}
			pNode = pNode->m_pNext;
		}

		m_csList.Unlock();
		return NULL;
	}

	bool IsInList(DWORD dwIndex)
	{
		m_csList.Lock();

		_index_node* pNode = m_Head.m_pNext;

		while(pNode != &m_Tail)
		{
			if(pNode->m_dwIndex == dwIndex)
			{
				m_csList.Unlock();
				return true;
			}
			pNode = pNode->m_pNext;
		}

		m_csList.Unlock();
		return false;
	}

	int CopyIndexList(DWORD* pdwList, int nMax)
	{
		m_csList.Lock();
		_index_node* pNode = m_Head.m_pNext;
		int nCnt = 0;
		while(pNode != &m_Tail)
		{
			pdwList[nCnt++] = pNode->m_dwIndex;
			if(nCnt >= nMax)
				break;
			pNode = pNode->m_pNext;
		}

		m_csList.Unlock();
		return nCnt;
	}

	int size(){
		return m_dwCount;
	}

};

struct _DELAY_PROCESS
{
	DWORD			m_dwObjectNum;
	DWORD*			m_pdwPushTime;
	DWORD*			m_pdwPushSerial;
	CNetIndexList	m_list;
	DWORD			m_dwTerm;
	
	_DELAY_PROCESS()
	{
		m_dwObjectNum = 0;
		m_pdwPushTime = NULL;
		m_pdwPushSerial = NULL;
	}

	_DELAY_PROCESS(DWORD dwObjectNum, DWORD dwTerm)
	{
		m_dwObjectNum = 0;
		m_pdwPushTime = NULL;
		m_pdwPushSerial = NULL;

		Init(dwObjectNum, dwTerm);
	}

	~_DELAY_PROCESS()
	{
		if(m_pdwPushTime)
		{
			delete [] m_pdwPushTime;
			m_pdwPushTime = NULL;
		}
		if(m_pdwPushSerial)
		{
			delete [] m_pdwPushSerial;
			m_pdwPushSerial = NULL;
		}
	}

	bool Init(DWORD dwObjectNum, DWORD dwTerm)
	{
		if(m_pdwPushTime)
			return false;

		if(dwObjectNum == 0)
			return false;

		m_dwObjectNum = dwObjectNum;
		m_dwTerm = dwTerm;

		m_pdwPushTime = new DWORD [dwObjectNum];
		m_pdwPushSerial = new DWORD [dwObjectNum];
		m_list.SetList(dwObjectNum);

		memset(m_pdwPushSerial, 0xFFFFFFFF, sizeof(DWORD)*dwObjectNum);

		return true;
	}

	void CheckOnLoop()
	{
		if(!m_pdwPushTime)
			return;

		DWORD dwCurTime = timeGetTime();

		DWORD	dwNodeIndex;
		while(m_list.CopyFront(&dwNodeIndex))
		{			
			if(dwCurTime - m_pdwPushTime[dwNodeIndex] < m_dwTerm)
				break;

			Process(dwNodeIndex, m_pdwPushSerial[dwNodeIndex]);

			m_pdwPushSerial[dwNodeIndex] = 0xFFFFFFFF;
			m_list.PopNode_Front(&dwNodeIndex);
		}
	}

	bool Push(DWORD dwIndex, DWORD dwSerial)
	{
		if(m_pdwPushTime)
		{
			if(m_pdwPushSerial[dwIndex] != 0xFFFFFFFF)
				return false;

			m_pdwPushTime[dwIndex] = timeGetTime();
			m_pdwPushSerial[dwIndex] = dwSerial;

			return m_list.PushNode_Back(dwIndex);
		}
		return false;
	}

	void Delete(DWORD dwIndex, DWORD dwSerial)
	{
		if(m_pdwPushSerial[dwIndex] == dwSerial && m_pdwPushSerial[dwIndex] != 0xFFFFFFFF)
		{			
			if(m_list.FindNode(dwIndex))
				m_pdwPushSerial[dwIndex] = 0xFFFFFFFF;
		}
	}

	virtual void Process(DWORD dwIndex, DWORD dwSerial) {} 
};


class CLogFile
{
private:
	char m_szFileName[128];
	DWORD m_dwLogCount;
	BOOL m_bWriteAble;
	CNetCriticalSection m_cs;
	bool m_bAddCount;
	bool m_bDate;
	bool m_bTrace;

public:

	bool m_bInit;

public:

	CLogFile()
	{
		m_szFileName[0] = NULL;
		m_dwLogCount = 0;
		m_bInit = false;
		m_bWriteAble = false;
		m_bAddCount = true;
		m_bDate = false;
		m_bTrace = false;
	}

	void SetWriteLogFile(char* szFileName, BOOL bWriteAble = false, bool bTrace = false, bool bDate = true, bool bAddCount = true)
	{
		int nOffset = 0;
		while(szFileName[nOffset] != NULL)
		{
			if(szFileName[nOffset] == '/' || szFileName[nOffset] == ':')
				szFileName[nOffset] = '_';
			nOffset++;
		}

		m_szFileName[0] = NULL;
		wsprintf(m_szFileName, "%s", szFileName);
		DeleteFile(m_szFileName);

		m_dwLogCount = 0;
		m_bInit = true;
		m_bWriteAble = bWriteAble;
		m_bAddCount = bAddCount;
		m_bDate = bDate;
		m_bTrace = bTrace;
	}

	void SetReWriteLogFile(char* szFileName, BOOL bWriteAble = false, bool bTrace = false, bool bDate = true, bool bAddCount = true)
	{
		int nOffset = 0;
		while(szFileName[nOffset] != NULL)
		{
			if(szFileName[nOffset] == '/' || szFileName[nOffset] == ':')
				szFileName[nOffset] = '_';
			nOffset++;
		}

		m_szFileName[0] = NULL;
		wsprintf(m_szFileName, "%s", szFileName);

		m_dwLogCount = 0;
		m_bInit = true;
		m_bWriteAble = bWriteAble;
		m_bAddCount = bAddCount;
		m_bDate = bDate;
		m_bTrace = bTrace;
	}

	void SetWriteAble(bool bAble)
	{
		m_bWriteAble = bAble;
	}

	void Write(const char *fmt, ...)	
	{
		if(!m_bInit)
			return;

		if(!m_bWriteAble)
			return;

	//	m_cs.Lock();

		char szBuf[1280], str[1280], szTime[128];
		va_list arg_ptr;		

		va_start(arg_ptr, fmt);
		vsprintf(str, fmt, arg_ptr);
		va_end(arg_ptr);

		if(m_bDate)
			_strtime(szTime);
		else
			szTime[0] = NULL;

		if(m_bAddCount)
		{
			if(m_bDate)
				wsprintf(szBuf, "%d %s : %s\n", m_dwLogCount++, szTime, str);
			else
				wsprintf(szBuf, "%s : %s\n", m_dwLogCount++, str);
		}
		else
		{
			if(m_bDate)
				wsprintf(szBuf, "%s %s\n", szTime, str);
			else
				wsprintf(szBuf, "%s\n", str);
		}

		HANDLE hFile = ::CreateFile(m_szFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
			return;

		::SetFilePointer(hFile, 0, NULL, FILE_END);

		DWORD dwWrite;
		::WriteFile(hFile, szBuf, strlen(szBuf), &dwWrite, NULL);

		::CloseHandle(hFile);		

	//	m_cs.Unlock();
	}

	void WriteString(char *fmt)	
	{
		if(!m_bInit)
			return;

		if(!m_bWriteAble)
			return;

	//	m_cs.Lock();

		char szBuf[1280], szTime[128];

		if(m_bDate)
			_strtime(szTime);
		else
			szTime[0] = NULL;

		if(m_bAddCount)
		{
			if(m_bDate)
				wsprintf(szBuf, "%d %s : %s\n", m_dwLogCount++, szTime, fmt);
			else
				wsprintf(szBuf, "%s : %s\n", m_dwLogCount++, fmt);
		}
		else
		{
			if(m_bDate)
				wsprintf(szBuf, "%s %s\n", szTime, fmt);
			else
				wsprintf(szBuf, "%s\n", fmt);
		}

		HANDLE hFile = ::CreateFile(m_szFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
			return;

		::SetFilePointer(hFile, 0, NULL, FILE_END);

		DWORD dwWrite;
		::WriteFile(hFile, szBuf, strlen(szBuf), &dwWrite, NULL);

		::CloseHandle(hFile);		

	//	m_cs.Unlock();
	}

	void WriteData(char *sData)	
	{
		if(!m_bInit)
			return;

		if(!m_bWriteAble)
			return;

	//	m_cs.Lock();

		HANDLE hFile = ::CreateFile(m_szFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
			return;

		::SetFilePointer(hFile, 0, NULL, FILE_END);

		DWORD dwWrite;
		::WriteFile(hFile, sData, strlen(sData), &dwWrite, NULL);

		::CloseHandle(hFile);		

	//	m_cs.Unlock();
	}
};

class CUnDeadLock
{
private:
	DWORD m_dwLimCount;
	DWORD m_dwCurCount;

public:
	CUnDeadLock(DWORD dwLimCount){
		m_dwLimCount = dwLimCount;
		m_dwCurCount = 0;
	}

	void SetZeroCurCount(){
		m_dwCurCount = 0;
	}

	bool Counting(){
		m_dwCurCount++;
		if(m_dwCurCount > m_dwLimCount)
			return false;
		return true;
	}
};

#endif