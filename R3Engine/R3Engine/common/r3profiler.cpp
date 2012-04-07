#include <windows.h>
#include "R3Profiler.h"

UINT64 CR3Profiler::GetTicks()
{
	UINT64 qw;

	if(QueryPerformanceCounter((LARGE_INTEGER *) &qw))
	{
		while(!qw)
			QueryPerformanceCounter((LARGE_INTEGER *) &qw);
	} 
	else
		qw = 0;
	return qw;
}


CR3Profiler::CR3Profiler()
{
	memset(this,0,sizeof(CR3Profiler));
}

CR3Profiler::~CR3Profiler()
{
}

void CR3Profiler::BeginR3Profiler()
{
	float fFramesPerSec =1000;
    if( FALSE == m_bTimerInitialized )
    {
        m_bTimerInitialized = TRUE;
		if(!QueryPerformanceFrequency((LARGE_INTEGER *) &m_qwTicksPerSec))
			m_qwTicksPerSec = 1000;
		m_fTicksPerSec  = (float) (__int64) m_qwTicksPerSec;
		m_fSecsPerFrame = 1.0f / fFramesPerSec;
		m_qwTicksPerFrame = m_qwTicksPerSec / (__int64) fFramesPerSec;
		m_qwStartTicks = m_qwTicks = GetTicks();
 	}
	m_NowID=0;
	m_bIsBegin=1;
}

float CR3Profiler::IntervalTime(UINT64 old_qw,UINT64 qw)
{
	if( old_qw == qw )
		return 0;
	m_qwTicksPerFrame = qw - old_qw;
	float m_fFramesPerSec = (m_fTicksPerSec / (float) (__int64) m_qwTicksPerFrame);
	if(m_fFramesPerSec < 1.0f)
		m_fFramesPerSec = 1.0f;
	m_fSecsPerFrame = 1.0f / m_fFramesPerSec;
	return m_fSecsPerFrame;
}

void CR3Profiler::CleanR3Profiler()		//토탈타임을 지운다.
{
	for(DWORD i=0; i<m_TotalIDNum; i++)
		m_ProfileTime[i].m_TotalTime =0;
}

void CR3Profiler::EndR3Profiler()
{
	UINT64 final_qw = GetTicks();
	m_TotalIDNum = m_NowID;
	UINT64 old_qw = m_ProfileTime[0].m_qwStartTicks;
	for(DWORD i=1; i<m_NowID; i++)
	{
		m_ProfileTime[i-1].m_LoopTime = IntervalTime(old_qw,m_ProfileTime[i].m_qwStartTicks);
		m_ProfileTime[i-1].m_TotalTime += m_ProfileTime[i-1].m_LoopTime;
		old_qw = m_ProfileTime[i].m_qwStartTicks;
	}
	m_ProfileTime[i-1].m_LoopTime = IntervalTime(old_qw,final_qw);
	m_ProfileTime[i-1].m_TotalTime += m_ProfileTime[i-1].m_LoopTime;

	m_TotalTime=0;
	for( i=0; i<m_NowID; i++)
		m_TotalTime += m_ProfileTime[i].m_TotalTime;
	m_bIsBegin=0;
}

DWORD CR3Profiler::GetR3ProfilerTotalNum()
{
	return m_TotalIDNum;
}

char *CR3Profiler::GetR3ProfilerIDName(DWORD id)
{
	return m_ProfileTime[id].mName;
}

float CR3Profiler::GetR3ProfilerIDTotalTime(DWORD id)
{
	return m_ProfileTime[id].m_TotalTime;
}
float CR3Profiler::GetR3ProfilerIDLoopTime(DWORD id)
{
	return m_ProfileTime[id].m_LoopTime*1000.0f;
}
float CR3Profiler::GetR3ProfilerIDPercent(DWORD id)
{
	return ((m_ProfileTime[id].m_TotalTime/m_TotalTime)*100.0f);
}

void CR3Profiler::StartR3ProfilerName(char *name)
{
	if( !m_bIsBegin )
		return;
	if( m_NowID >= _TOTAL_ELEMENT )	//넘치는거 막는다.
		return;
	if( m_ProfileTime[m_NowID].mName[0] == NULL )
	{
		strncpy(m_ProfileTime[m_NowID].mName,name,31);
		m_ProfileTime[m_NowID].mName[31]=NULL;
	}
	m_ProfileTime[m_NowID].m_qwStartTicks = GetTicks();
	m_NowID++;
}

