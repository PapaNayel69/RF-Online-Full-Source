// MyClassUtil.h
//
//////////////////////////////////////////////////////////////////////
#ifndef _MYCLASSUTIL_H_
#define _MYCLASSUTIL_H_

#include <windows.h>

//////////////////////////
// CMyTimer  
//////////////////////////
class CMyTimer  
{
private:

	DWORD	m_dwTickTerm;
	DWORD	m_dwTickOld;

public:

	bool m_bOper;

public:	
	CMyTimer();
	virtual ~CMyTimer(){}

	void BeginTimer(DWORD dwTerm);
	void StopTimer();
	bool CountingTimer();
	void NextTimeRun();
	void TermTimeRun();
	DWORD GetTerm();
};

//////////////////////////
// CMyCriticalSection
//////////////////////////
class CMyCriticalSection
{
private:
	CRITICAL_SECTION m_cs;

public:
	CMyCriticalSection();
	virtual ~CMyCriticalSection();
	void Lock();
	void Unlock();
};

//////////////////////////
// CFrameRate
//////////////////////////
class CFrameRate  
{
public:
	enum { ESPECT_FPS = 15, 
		MAX_MOVE_UNIT = 1 };

	float m_fSpeedPerFrame;
	DWORD m_dwFrames;

	DWORD m_dwFrameTime;
	DWORD m_dwFrameCount;

	DWORD m_dwDisplayTick;

public:
	CFrameRate();
	virtual ~CFrameRate(){}

	void CalcSpeedPerFrame();
	float GetSpeedPerFrame();
	DWORD GetFPS();
};

//////////////////////////
// CFPS
//////////////////////////
class CFPS
{
public:

	DWORD m_dwFrames;

	DWORD m_dwFrameTime;
	DWORD m_dwFrameCount;

public:
	CFPS();
	virtual ~CFPS(){}

	void CalcFPS();
	DWORD GetFPS();
};

//////////////////////////
// CTerm
//////////////////////////
class CTerm
{
private:

	DWORD m_dwStartTime;

public:

	CTerm() 
	{
		m_dwStartTime = timeGetTime();
	}

	DWORD GetTerm()
	{
		return timeGetTime() - m_dwStartTime;
	}
};

//////////////////////////
// __timer
//////////////////////////
class __timer
{
private:

	DWORD m_dwCurTime;

public:

	__timer() 
	{
		SetTime();
	}

	void SetTime()
	{
		m_dwCurTime = timeGetTime();
	}

	DWORD GetTime()
	{
		return m_dwCurTime;
	}
};
#endif
