#ifndef _MYUTIL_H_
#define _MYUTIL_H_

#include <windef.h>
#include <time.h>
#include <netutil.h>

#define _PI 3.1415926535
#define PiToAngle(a)  (((double)(a)*65535.0)/(2.0*_PI))
#define AngleToPi(a)  ((2.0*_PI*(double)(a))/65535.0)

float	GetYAngle(float* Pos, float* Tar); 
float	GetSqrt(float* Pos, float* Tar);
int		CalcSquare(int nLoot, int nMulti);
void	MyMessageBox(char* szTitle, char* szMessage, ...);
void	ShortToFloat(short* pShort, float* pFloat, int size);
void	FloatToShort(float* pFloat, short* pShort, int size);
WORD	GetRandom();
void	__trace(char* fmt, ...);
DWORD	GetKorLocalTime();
DWORD	GetLocalDate();
int		CalcFileSize(char* pszFileName);
int		GetCurrentYear();
int		GetCurrentMonth();
int		GetCurrentDay();
int		GetCurrentHour();
int		GetCurrentMin();
int		GetCurrentSec();
bool	WriteLogFileLong(char* pszFileName, char* log, ...);
int		ParsingCommand(char* pszSrc, int nWordNum, char** OUT ppszDst, int nMaxWordSize);

void	EnCryptString( char* pStr, int nSize, BYTE byPlus, WORD wCryptKey );
void	DeCryptString( char* pStr, int nSize, BYTE byPlus, WORD wCryptKey );

struct _100_per_random_table
{
	enum	{	max_record_num = 100	};
	enum	{	max_table_num = 10	};

	static bool s_bRecordSet;
	static WORD s_wRecord[max_table_num][max_record_num];
	
	WORD m_wCurTable;
	WORD m_wCurPoint;

	_100_per_random_table()
	{
		if(!s_bRecordSet)
		{
			s_bRecordSet = true;
			
			srand((unsigned)time(NULL));
			for(int t = 0; t < max_table_num; t++)
			{
				for(int r = 0; r < max_record_num; r++)
					s_wRecord[t][r] = r;
			}

			reset();
		}

		m_wCurTable = rand()%max_table_num;
		m_wCurPoint = 0;
	}

	void reset()
	{
		for(int t = 0; t < max_table_num; t++)
		{
			for(int i = 0; i < max_record_num; i++)
			{
				WORD wFrom = rand()%max_record_num;
				WORD wToIndex  = rand()%max_record_num;

				WORD wBuf = s_wRecord[t][wToIndex];
				s_wRecord[t][wToIndex] = s_wRecord[t][wFrom];
				s_wRecord[t][wFrom] = wBuf;
			}
		}			
	}

	WORD GetRand()
	{
		if(m_wCurPoint >= max_record_num)
		{
			m_wCurTable = rand()%max_table_num;
			m_wCurPoint = 0;
		}

		WORD wR = s_wRecord[m_wCurTable][m_wCurPoint];
		m_wCurPoint++;

		return wR;		
	}
};

struct _SRAND
{
	_SRAND() { ::srand((unsigned)time(NULL)); }

	WORD GetRand() {	return ::rand();	}
};

#endif