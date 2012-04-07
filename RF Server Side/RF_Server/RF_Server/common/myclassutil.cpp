#include "stdafx.h"
#include "MyClassUtil.h"
#include <mmsystem.h>

CMyTimer::CMyTimer()
{
	m_bOper = false;
	m_dwTickOld = 0;
}

void CMyTimer::BeginTimer(DWORD dwTerm)
{
	m_bOper = true;
	m_dwTickTerm = dwTerm;	
	m_dwTickOld = timeGetTime();
}

void CMyTimer::StopTimer()
{
	m_bOper = false;
}

bool CMyTimer::CountingTimer()
{
	if(!m_bOper)
		return false;

	DWORD dwCurTime = timeGetTime();

	if(dwCurTime - m_dwTickOld < m_dwTickTerm)
		return false;

	m_dwTickOld = dwCurTime;

	return true;
}

void CMyTimer::NextTimeRun()
{
	m_dwTickOld = timeGetTime() + m_dwTickTerm + 1;
}

void CMyTimer::TermTimeRun()
{
	m_dwTickOld = timeGetTime();
}

DWORD CMyTimer::GetTerm()
{
	return m_dwTickTerm;
}

CMyCriticalSection::CMyCriticalSection()
{
	InitializeCriticalSection(&m_cs);		               
}

CMyCriticalSection::~CMyCriticalSection()
{
	DeleteCriticalSection(&m_cs);
}

void CMyCriticalSection::Lock()
{
	EnterCriticalSection(&m_cs);
}

void CMyCriticalSection::Unlock()
{
	LeaveCriticalSection(&m_cs);
}


CFrameRate::CFrameRate()
{
	m_fSpeedPerFrame = 1.0f;
	m_dwFrameCount = 0;
}

void CFrameRate::CalcSpeedPerFrame()
{
	DWORD time, time2;

	time = timeGetTime();
	
	m_dwFrameCount++;

	time2 = time - m_dwFrameTime;

	if(time2 > 1000)
	{
		m_dwFrames = (m_dwFrameCount*1000)/time2;
		m_dwFrameCount = 0;
		
		m_dwFrameTime = timeGetTime();
		m_fSpeedPerFrame = (float)m_dwFrames/ESPECT_FPS;
		m_fSpeedPerFrame = MAX_MOVE_UNIT/m_fSpeedPerFrame;

		if(m_fSpeedPerFrame > MAX_MOVE_UNIT)
			m_fSpeedPerFrame = MAX_MOVE_UNIT;
	}
}

float CFrameRate::GetSpeedPerFrame()
{
	return m_fSpeedPerFrame;
}

DWORD CFrameRate::GetFPS()
{
	return m_dwFrames;
}


CFPS::CFPS()
{
	m_dwFrames = 0;
	m_dwFrameCount = 0;
	m_dwFrameTime = timeGetTime();
}

void CFPS::CalcFPS()
{
	DWORD time, time2;

	time = timeGetTime();
	
	m_dwFrameCount++;

	time2 = time - m_dwFrameTime;

	if(time2 > 1000)
	{
		m_dwFrames = (m_dwFrameCount*1000)/time2;
		m_dwFrameCount = 0;
		
		m_dwFrameTime = timeGetTime();
	}
}

DWORD CFPS::GetFPS()
{
	return m_dwFrames;
}


