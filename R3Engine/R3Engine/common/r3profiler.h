#ifndef __R3PROFILER_H__
#define __R3PROFILER_H__

/*프로파일 사용법

	BeginR3Profiler();
	StartR3ProfilerName("캐릭터 프레임무브");
	...
	StartR3ProfilerName("배경 프레임무브");
	...
	StartR3ProfilerName("캐릭터렌더");
	...
	StartR3ProfilerName("배경렌더");
	...
	EndR3Profiler();

	시작부터 끝까지를 100으로 본다. 
*/

typedef struct{
	char mName[32];
	UINT64 m_qwStartTicks;
	float m_TotalTime;
	float m_LoopTime;
}_PROFILE_ELMENT;

#define _TOTAL_ELEMENT	64

class CR3Profiler{
protected :
	_PROFILE_ELMENT m_ProfileTime[_TOTAL_ELEMENT];	//
	DWORD m_NowID,m_TotalIDNum;

	BOOL m_bTimerInitialized;
	UINT64 m_qwTicks;
	UINT64 m_qwStartTicks;
	UINT64 m_qwTicksPerSec;
	UINT64 m_qwTicksPerFrame;
	float m_fTicksPerSec,m_fSecsPerFrame;
	BOOL m_bIsBegin;
	float m_TotalTime;


	UINT64 GetTicks();
	float IntervalTime(UINT64 old_qw,UINT64 qw);

public:
	CR3Profiler();
	~CR3Profiler();

	void BeginR3Profiler();
	void CleanR3Profiler();		//토탈타임을 지운다. 첨부터 다시계산하기위해서..
	void EndR3Profiler();
	void StartR3ProfilerName(char *name);

	DWORD GetR3ProfilerTotalNum();
	char *GetR3ProfilerIDName(DWORD id);
	float GetR3ProfilerIDPercent(DWORD id);
	float GetR3ProfilerIDTotalTime(DWORD id);
	float GetR3ProfilerIDLoopTime(DWORD id);
};

#endif