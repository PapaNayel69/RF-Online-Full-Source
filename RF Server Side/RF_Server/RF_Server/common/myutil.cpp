#include "StdAfx.h"
#include "MainThread.h"
#include <math.h>
#include "MyUtil.h"

bool _100_per_random_table::s_bRecordSet = false;
WORD _100_per_random_table::s_wRecord[max_table_num][max_record_num];

float GetYAngle(float* Pos, float* Tar)
{
	double Sqrt = sqrt( pow((Tar[0]-Pos[0]), 2) + pow((Tar[2]-Pos[2]), 2) );
	double Acos = acos((Tar[2]-Pos[2])/ Sqrt);

	if(Tar[0]-Pos[0] > 0)
		return((float)(PiToAngle( Acos )+32768));
	else
		return((float)(PiToAngle( -Acos )+32768));
}

float GetSqrt(float* fPos, float* fTar)
{
	return (float)sqrt( pow((fTar[0]-fPos[0]), 2) + pow((fTar[2]-fPos[2]), 2) );
}

int CalcSquare(int nLoot, int nMulti)
{	
	int nRet = 1;
	for(int i = 0; i < nMulti; i++)
	{
		nRet *= nLoot;
	}
	return nRet;
}

void MyMessageBox(char* szTitle, char* szMessage, ...)
{
	va_list args;
	char szMsg[128];
	
	va_start(args, szMessage);
	vsprintf(szMsg, szMessage, args);
	va_end(args);

	char szTrace[128];

	sprintf(szTrace, "%s - %s", szTitle, szMsg);
	MessageBox(NULL, szMsg, szTitle, MB_OK);
}

void ShortToFloat(short* pShort, float* pFloat, int size)
{
	for(int i = 0; i < size; i++)
	{
		pFloat[i] = (float)pShort[i];
	}
}

void FloatToShort(float* pFloat, short* pShort, int size)
{
	for(int i = 0; i < size; i++)
	{
		pShort[i] = (short)pFloat[i];
	}
}

void __trace(char* fmt, ...)
{
	va_list arg_ptr;
	char str[256] = {0,};

	va_start(arg_ptr, fmt);
	vsprintf(str, fmt, arg_ptr);
	va_end(arg_ptr);

	strcat(str, "\n");
	
    OutputDebugString( str );
}

DWORD GetKorLocalTime()
{
	char szDate[32], szTime[32];

	_strdate(szDate);
	_strtime(szTime);

	char szLocalTime[32];

	//년도..(6, 7)
	szLocalTime[0] = szDate[7];
	//월..(0, 1)
	szLocalTime[1] = szDate[0];
	szLocalTime[2] = szDate[1];
	//일..(3, 4)
	szLocalTime[3] = szDate[3];
	szLocalTime[4] = szDate[4];
	//시..(0, 1)
	szLocalTime[5] = szTime[0];
	szLocalTime[6] = szTime[1];
	//분..(3, 4)
	szLocalTime[7] = szTime[3];
	szLocalTime[8] = szTime[4];

	szLocalTime[9] = NULL;

	return (DWORD)atoi(szLocalTime);
}

DWORD GetLocalDate()
{
	return (::GetCurrentYear() * 10000) + (::GetCurrentMonth() * 100) + ::GetCurrentDay();	
}

int CalcFileSize(char* pszFileName)		//파일 사이즈를 알아낸다.
{
	HANDLE hFile = ::CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return 0;

	DWORD dwStart = ::SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	DWORD dwEnd = ::SetFilePointer(hFile, 0, NULL, FILE_END);

	::CloseHandle(hFile);

	return dwEnd-dwStart;
}

int GetCurrentYear()
{
    struct tm *newtime;
    time_t long_time;

    ::time( &long_time );  
    newtime = ::localtime( &long_time );

    return newtime->tm_year+1900;
}

int	GetCurrentMonth()
{
    struct tm *newtime;
    time_t long_time;

    ::time( &long_time );  
    newtime = ::localtime( &long_time );

    return newtime->tm_mon+1;
}

int	GetCurrentDay()
{
    struct tm *newtime;
    time_t long_time;

    ::time( &long_time );  
    newtime = ::localtime( &long_time );

    return newtime->tm_mday;
}

int GetCurrentHour()
{
    struct tm *newtime;
    time_t long_time;

    ::time( &long_time );  
    newtime = ::localtime( &long_time );

    return newtime->tm_hour;
}

int GetCurrentMin()
{
    struct tm *newtime;
    time_t long_time;

    ::time( &long_time );  
    newtime = ::localtime( &long_time );

    return newtime->tm_min;
}

int GetCurrentSec()
{
    struct tm *newtime;
    time_t long_time;

    ::time( &long_time );  
    newtime = ::localtime( &long_time );

    return newtime->tm_sec;
}

bool WriteLogFileLong(char* pszFileName, char* log, ...)
{
	char szBuf[1280], str[1280];

	va_list arg_ptr;		
	va_start(arg_ptr, log);
	vsprintf(str, log, arg_ptr);
	va_end(arg_ptr);

	char szDate[128], szTime[128];

	_strdate(szDate);
	_strtime(szTime);

	::sprintf(szBuf, "%s %s %s\n", szDate, szTime, str);

    ::OutputDebugString( szBuf );

	FILE *fp;
	fp = fopen(pszFileName, "a");
	if(fp)
	{
		fprintf(fp, szBuf);
		fclose(fp);
	}
	else
		return false;

	return true;
}

int ParsingCommand(char* pszSrc, int nMaxWordNum, char** OUT ppszDst, int nMaxWordSize)
{
	char* pSrc = pszSrc;

	for(int i = 0; i < nMaxWordNum; i++)
	{
		char* pDst = ppszDst[i];
		int nCnt = 0;
		while(*pSrc != NULL && *pSrc != ' ')
		{
			*pDst++ = *pSrc++;
			if(nCnt++ >= nMaxWordSize-1)
				return 0;
		}		
		*pDst = NULL;

		if(*pSrc == NULL)
		{
			if(strlen(ppszDst[i]) > 0)
				return i+1;
			else
				return i;
		}
		else
		{
			if(pDst == ppszDst[i])
				return i;
		}

		pSrc++;

	}

	return 0;
}


void EnCryptString( char* pStr, int nSize, BYTE byPlus, WORD wCryptKey )
{
	for ( int i=0; i<nSize; i++)
	{
		*pStr += byPlus;
		*pStr ^= wCryptKey;
		pStr++;
	}
}

void DeCryptString( char* pStr, int nSize, BYTE byPlus, WORD wCryptKey )
{
	for ( int i=0; i<nSize; i++)
	{
		*pStr ^= wCryptKey;
		*pStr -= byPlus;
		pStr++;
	}
}