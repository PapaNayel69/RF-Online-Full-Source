#ifndef _ConnNumPHMgr_h_
#define _ConnNumPHMgr_h_

struct _USER_NUM_SHEET
{
	int nAveragePerHour;
	int nMaxPerHour;
	int	nCount;
};

class CConnNumPHMgr
{
public:

	struct __cnt_per_h		//동접수를 계산하기위한 구조체
	{
		int		nUserCumPerMin;		//한시간동안의 매분당 누적 유저수
		DWORD	dwCounting;			//한시간동안 누적카운팅.. 
		int		nMaxUserNum;		//한시간동안 최대유저수	

		__cnt_per_h() {	Init();	}

		void Init()		
		{	
			memset(this, 0, sizeof(*this));	
		}

		int GetAverageUserNumPerHour()	
		{
			if(dwCounting == 0)
				return 0;

			return nUserCumPerMin/dwCounting;	//시간당 평균유저수 = 한시간동안의 매분당 누적 유저수/한시간동안 누적카운팅
		}
	};

	__cnt_per_h			m_cph;
	_USER_NUM_SHEET		m_LastResult;

	int					m_nLastHour;

	DWORD				m_dwCheckLastTime;

public:

	CConnNumPHMgr() {	Init();	}

	void Init()
	{
		m_nLastHour = GetCurHour();
		m_dwCheckLastTime = timeGetTime();

		m_cph.Init();
	}

	_USER_NUM_SHEET* Check(int nLoginNum)
	{
		bool bUpdate = false;
		DWORD dwCurTime = timeGetTime();

		if(dwCurTime - m_dwCheckLastTime > 60000)
		{
			m_dwCheckLastTime = dwCurTime;

			m_cph.nUserCumPerMin += nLoginNum;
			m_cph.dwCounting++;
			m_cph.nMaxUserNum = max(m_cph.nMaxUserNum, nLoginNum);//최대 동접수

			int nCurHour = GetCurHour();

			if(nCurHour != m_nLastHour)
			{
				if(m_cph.nUserCumPerMin > 0)
				{
					m_LastResult.nAveragePerHour = m_cph.GetAverageUserNumPerHour();
					m_LastResult.nMaxPerHour = m_cph.nMaxUserNum;
					m_LastResult.nCount = m_cph.dwCounting;

					bUpdate = true;
				}

				m_cph.Init();
				m_nLastHour = nCurHour;
			}
		}
		
		if(bUpdate)
			return &m_LastResult;
		return NULL;
	}

	int GetCurHour()
	{
		struct tm *newtime;
		time_t long_time;

		::time( &long_time );  
		newtime = ::localtime( &long_time );

		return newtime->tm_hour;
	}
};

#endif