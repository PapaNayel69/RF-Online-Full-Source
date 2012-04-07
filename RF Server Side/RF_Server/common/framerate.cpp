// FrameRate.cpp: implementation of the CFrameRate class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "FrameRate.h"
#include <mmsystem.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

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

