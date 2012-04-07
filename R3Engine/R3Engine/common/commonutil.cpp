#include "CommonUtil.h"
#include "jpegfile.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#define _MIN_FPS	6.0f

typedef int (__stdcall *FNQUERYPERFORMANCEFREQUENCY)(    LARGE_INTEGER *lpFrequency    );

typedef DWORD (__stdcall *PFNTIMEGETTIME)(void);

FNQUERYPERFORMANCEFREQUENCY pQueryPerformanceFrequency  = QueryPerformanceFrequency;
PFNTIMEGETTIME	ptimeGetTime = timeGetTime;

void StripEXT(char *name)
{
	for(int i=strlen(name)-1; i>=0; i--)
	{
		if(name[i]=='.')
		{
			name[i]=NULL;
			return;
		}
	}
}
void StripPath(char *name)
{
	char r_name[256];

	for(int i=strlen(name)-1; i>=0; i--)
	{
		if(name[i]=='\\')
		{
			break;
		}
	}
	int st=i+1;
	for(i=st; i<(int)strlen(name); i++)
	{
		r_name[i-st]=name[i];
	}
	r_name[i-st]=NULL;
	strcpy(name,r_name);
}
void StripName(char *name)
{
	for(int i=strlen(name)-1; i>=0; i--)
	{
		if(name[i]=='\\')
		{
			break;
		}
	}
	if( i==0 )
		return;
	name[i+1]=NULL;
}
BOOL GetNowPath(char *get_path)//현재 실행하고 있는 패스를 얻어온다.
{
	char szBuffer[256];
    PSTR pszFilename;
    
    // Get the complete path to this EXE - The spy dll should be in the
    // same directory.
    GetModuleFileName(0, szBuffer, sizeof(szBuffer));
	pszFilename = strrchr(szBuffer, '\\');
    if ( !pszFilename )
        return FALSE;
	strcpy(get_path,szBuffer);
	StripName(get_path);
	return TRUE;
}

void GetFinalPath(char *get,char *path)
{
	DWORD i,j;

	for( i=strlen(path)-2; i>=0; i--)
	{
		if(path[i]=='\\')
		{
			break;
		}
	}
	if( i==0 )
	{
		strcpy(get,".\\");
		return;
	}
	for( j=i+1; j<strlen(path); j++)
	{
		get[j-(i+1)] = path[j];
	}
	get[j-(i+1)] =NULL;
}

void GetEXT(char *get,char *name)
{	
	int i,j;

	for( i=strlen(name)-1; i>=0; i--)
	{
		if(name[i]=='.')
		{
			break;
		}
	}
	if( i==0 || (strlen(name)-1 -i)> 3)
	{
		get[0]=NULL;
		return;
	}
	for( j=i+1; j<(int)strlen(name); j++)
	{
		get[j-(i+1)] = name[j];
	}
	get[j-(i+1)] =NULL;
}

void StripFinalPath(char *name)
{
	int i;

	for( i=strlen(name)-2; i>=0; i--)
	{
		if(name[i]=='\\')
		{
			break;
		}
	}
	if( i==0 )
	{
		return;
	}
	name[i+1]=NULL;
}

int GetFileSize(char *name)		//파일 사이즈를 알아낸다.
{
	int end,start;

	FILE *fp = fopen(name,"rb");
	if(fp==NULL)
		return 0;
	
	start=ftell(fp);	//파일 사이즈를 알아낸다.
	fseek(fp,0,SEEK_END);
	end=ftell(fp);
	fclose(fp);
	
	return (end-start);

}



int MaxFixFloatToInt(float su);
int MinFixFloatToInt(float su);

int MaxFixFloatToInt(float su)
{
	su+=0.999f;
	return (int )su;
}

int MinFixFloatToInt(float su)
{
	su-=0.999f;
	return (int )su;
}

BOOL IsExistFile(char *name)
{
	if(name[0]==NULL)
		return FALSE;
	FILE *fp;
	fp=fopen(name,"rb");
	if( fp==NULL )
		return FALSE;
	fclose(fp);
	return TRUE;
}

static BYTE Bmpbyte[54];
void SaveBMP(char *name,int xl,int yl,int pitch,BYTE *argb)
{
	FILE *fp;
    int i,j,xleng,yleng,size,sparexl,diff;
    BYTE *trbuf;

    memset(Bmpbyte,54,0);
    Bmpbyte[0x0]='B';
    Bmpbyte[0x1]='M';
    Bmpbyte[0x1c]=24;
    Bmpbyte[0x0a]=54;

	Bmpbyte[0x12]=xl&0xff;
	Bmpbyte[0x13]=(xl>>8)&0xff;
	Bmpbyte[0x16]=yl&0xff;
	Bmpbyte[0x17]=(yl>>8)&0xff;
    Bmpbyte[38]=0x12;
    Bmpbyte[39]=0xb;
    Bmpbyte[42]=0x12;
    Bmpbyte[43]=0xb;
    Bmpbyte[26]=1;
    Bmpbyte[14]=40;


    diff=((xl+3)&3);
    sparexl=((xl*3+3)&~3);
    size=sparexl*yl;
	Bmpbyte[0x34]=size&0xff;
	Bmpbyte[0x35]=(size>>8)&0xff;
	Bmpbyte[0x36]=(size>>16)&0xff;
	Bmpbyte[0x37]=(size>>24)&0xff;
    size=sparexl*yl+54;
	Bmpbyte[0x2]=size&0xff;
	Bmpbyte[0x3]=(size>>8)&0xff;
	Bmpbyte[0x4]=(size>>16)&0xff;
	Bmpbyte[0x5]=(size>>24)&0xff;
    xleng=xl;
    yleng=yl;
    trbuf=(BYTE *)malloc((xleng)*(yleng)*4);
    for(i=0;i<yleng;i++)
	{
		for(j=0; j<xleng; j++)
		{
			trbuf[(-i+yleng-1)*sparexl+j*3] = argb[i*pitch+j*4];
			trbuf[(-i+yleng-1)*sparexl+j*3+1] = argb[i*pitch+j*4+1];
			trbuf[(-i+yleng-1)*sparexl+j*3+2] = argb[i*pitch+j*4+2];
		}
	}
//		memcpy(&trbuf[(yleng-1-i)*sparexl],&buf[i*xleng*3],xl*3);

    fp=fopen(name,"wb");
	if( fp == NULL )
	{
	    free(trbuf);
		return;
	}
    fwrite(Bmpbyte,54,1,fp);
    for(i=0;i<yleng;i++)
	    fwrite(&trbuf[(i)*sparexl],sparexl,1,fp);
	fclose(fp);
    free(trbuf);
}


#define JPG_QUALITY		95		//95와 100은 두배차이..
void SaveJPG(char *name,int xl,int yl,int pitch,BYTE *argb)
{
	ARGBToJpegFile( name,argb,pitch,						// 길이다 
							xl,yl,TRUE,JPG_QUALITY);	//지금 은 
}

float GetFPS()
{
    static FLOAT fFPS      = 0.0f;
    static FLOAT fLastTime = 0.0f;
    static DWORD dwFrames  = 0L;

    // Keep track of the time lapse and frame count
    FLOAT fTime = (*ptimeGetTime)() * 0.001f; // Get current time in seconds
    ++dwFrames;

    // Update the frame rate once per second
    if( fTime - fLastTime > 1.0f )
    {
        fFPS      = dwFrames / (fTime - fLastTime);
        fLastTime = fTime;
        dwFrames  = 0L;
    }
    return fFPS;
}


//--------------------------------



//------ 엔진 내부 타이머 
static float stLoopTime=0,stTime,stRealTime;
static float stMinFPS=1.0f,stfLoopHop=0;
DWORD stOldTime=0;
DWORD stLoopCnt=0;
DWORD stLoopHop=0;

typedef unsigned __int64 UINT64;


float m_fTicksPerSec;
float m_fFramesPerSec;
float m_fAverageFramesPerSec;
float m_fSecsPerFrame;
float m_fLastTime;

UINT64 m_qwTicks;
UINT64 m_qwStartTicks;
UINT64 m_qwTicksPerSec;
UINT64 m_qwTicksPerFrame;

static UINT64 GetTicks()
{
	UINT64 qw;

	if(QueryPerformanceCounter((LARGE_INTEGER *) &qw))
	{
		while(!qw)
			QueryPerformanceCounter((LARGE_INTEGER *) &qw);
	} 
	else
	{
		qw = (UINT64) (*ptimeGetTime)();
	}
	return qw;
}
float GetDuration()
{
	INT64 qwDuration = (INT64)(GetTicks() - m_qwStartTicks);
	float fDuration = (float)qwDuration / (float)(INT64)m_qwTicksPerSec;
	return fDuration;
}
BOOL st_bUsingQPF=FALSE,st_bTimerInitialized=FALSE;

//#include "r3input.h"

#define _30FPS_LOOP_TIME	(1/30.f)
#define _15FPS_LOOP_TIME	(1/15.0f)
#define _7FPS_LOOP_TIME	(1/7.0f)

static float Nucnt30fps=0;
static float Nucnt15fps=0;
static float Nucnt7fps=0;
static LONG stToggle30fps=1;
static LONG stToggle15fps=1;
static LONG stToggle7fps=1;

LONG R3GetToggle30fps()	//30fps로 토글을 해준다.
{
	return stToggle30fps;
}

static void R3Toggle30fps()	//30fps로 토글을 해준다.
{
	Nucnt30fps += stLoopTime;
	if( Nucnt30fps >= _30FPS_LOOP_TIME )
	{
		stToggle30fps=-stToggle30fps;
		Nucnt30fps -= _30FPS_LOOP_TIME;
		if( Nucnt30fps >= _30FPS_LOOP_TIME )
			Nucnt30fps=0;
	}
}

LONG R3GetToggle15fps()	//15fps로 토글을 해준다.
{
	return stToggle15fps;
}

static void R3Toggle15fps()	//15fps로 토글을 해준다.
{
	Nucnt15fps += stLoopTime;
	if( Nucnt15fps >= _15FPS_LOOP_TIME )
	{
		stToggle15fps=-stToggle15fps;
		Nucnt15fps -= _15FPS_LOOP_TIME;
		if( Nucnt15fps >= _15FPS_LOOP_TIME )
			Nucnt15fps=0;
	}
}

LONG R3GetToggle7fps()	//7fps로 토글을 해준다.
{
	return stToggle7fps;
}

static void R3Toggle7fps()	//7fps로 토글을 해준다.
{
	Nucnt7fps += stLoopTime;
	if( Nucnt7fps >= _7FPS_LOOP_TIME )
	{
		stToggle7fps=-stToggle7fps;
		Nucnt7fps -= _7FPS_LOOP_TIME;
		if( Nucnt7fps >= _7FPS_LOOP_TIME )
			Nucnt7fps=0;
	}
}

void R3CalculateTime()	//루프마다 해줘야된다.
{
	float fFramesPerSec =1000;

//	gMouse.oldx = gMouse.x;
//	gMouse.oldy = gMouse.y;

//	if (gScan[VK_L])
  //      st_bTimerInitialized = FALSE;

    if( FALSE == st_bTimerInitialized )
    {
        st_bTimerInitialized = TRUE;
		if(!(*pQueryPerformanceFrequency)((LARGE_INTEGER *) &m_qwTicksPerSec))
			m_qwTicksPerSec = 1000;
		m_fTicksPerSec  = (float) (__int64) m_qwTicksPerSec;
		m_fSecsPerFrame = 1.0f / fFramesPerSec;
		m_qwTicksPerFrame = m_qwTicksPerSec / (__int64) fFramesPerSec;
		m_qwStartTicks = m_qwTicks = GetTicks();
 	}

	if( 0 )
	{
		UINT64 qw;
		qw = GetTicks();
		if(qw != m_qwTicks)
		{
			m_qwTicksPerFrame = qw - m_qwTicks;
			m_qwTicks = qw;
	//		m_fFramesPerSec = 0.75f * m_fFramesPerSec 
	//			+ 0.25f * (m_fTicksPerSec / (float) (__int64) m_qwTicksPerFrame);
			m_fFramesPerSec = 0.00f * m_fFramesPerSec 
				+ 1.00f * (m_fTicksPerSec / (float) (__int64) m_qwTicksPerFrame);
			if(m_fFramesPerSec < 1.0f)
				m_fFramesPerSec = 1.0f;
			m_fSecsPerFrame = 1.0f / m_fFramesPerSec;
			stLoopTime=m_fSecsPerFrame;
		}
	}
	else
	{
		BYTE *check =(BYTE*) (*ptimeGetTime);
		/*
		if ((*check) !=0x83) { 
			//MessageBox(NULL,"SPEED HACK",0,0);
			exit(0); }//Speed Hack
			*/
		DWORD gop=(*ptimeGetTime)();
		DWORD sub=gop-stOldTime;
		
		stLoopCnt++;
		stLoopHop+=sub;
	//	if( stLoopHop > 200 )
		if( stLoopHop > 10 )
		{
			stLoopTime = (stLoopHop/(float)stLoopCnt)/1000.0f;
			stLoopCnt=0;
			stLoopHop=0;
		}
		stOldTime = gop;

		if( stLoopTime > stMinFPS || stLoopTime < 0 )
		{
			stLoopTime=stMinFPS;
		}

//		SetMultiLayerTime(stRealTime);
	}


	stRealTime+=stLoopTime;
	R3Toggle30fps();
	R3Toggle15fps();
	R3Toggle7fps();
}
/*
BOOL st_bUsingQPF=FALSE,st_bTimerInitialized=FALSE;
static LONGLONG st_llQPFTicksPerSec  = 0,st_llBaseTime        = 0;
static double st_dQPFSecPerTick;
void R3CalculateTime()	//루프마다 해줘야된다.
{
	LARGE_INTEGER qwTime,qwTicksPerSec;
    
    if( FALSE == st_bTimerInitialized )
    {
        st_bTimerInitialized = TRUE;
		st_bUsingQPF = (*pQueryPerformanceFrequency)( &qwTicksPerSec );
       if( st_bUsingQPF )
	   {
            st_llQPFTicksPerSec = qwTicksPerSec.QuadPart;
			st_dQPFSecPerTick = 1.0/(double)st_llQPFTicksPerSec;
			QueryPerformanceCounter( &qwTime );
			st_llBaseTime = qwTime.QuadPart;
	   }
 	}

    if( st_bUsingQPF )
	{

//		QueryPerformanceCounter( &qwTime );
//		stLoopTime = (qwTime.QuadPart - st_llBaseTime) / (float) st_llQPFTicksPerSec;
//		stRealTime += stLoopTime;//qwTime.QuadPart / (double) st_llQPFTicksPerSec;
//		st_llBaseTime = qwTime.QuadPart;

		stLoopCnt++;
		QueryPerformanceCounter( &qwTime );
//		stfLoopHop += (qwTime.QuadPart - st_llBaseTime) / (float) st_llQPFTicksPerSec;
		stfLoopHop += (float)((qwTime.QuadPart - st_llBaseTime) *st_dQPFSecPerTick);
		stRealTime += stLoopTime;

		if( stfLoopHop >  0.1f )//(1/1000.0f)* )
		{
			stLoopTime = stfLoopHop/(float)stLoopCnt;
			if( 0.010032 < stLoopTime )
				int dskfjsd=0;
			if( stLoopTime >  (1000/_MIN_FPS)/1000.0f)
				stLoopTime = (1000/_MIN_FPS)/1000.0f;
			stfLoopHop=0;
			stLoopCnt=0;
		}

		st_llBaseTime = qwTime.QuadPart;

//		char temp[256];
//		sprintf(temp,"%f\n",stLoopTime);
//		OutputDebugString(temp);

	}
	else
	{
		DWORD gop=(*ptimeGetTime)();
		DWORD sub=gop-stOldTime;
		
		stLoopCnt++;
		stLoopHop+=sub;
	//	if( stLoopHop > 200 )
		if( stLoopHop > 100 )
		{
			stLoopTime = (stLoopHop/(float)stLoopCnt)/1000.0f;
			stLoopCnt=0;
			stLoopHop=0;
		}
		stOldTime = gop;

		if( stLoopTime > stMinFPS || stLoopTime < 0 )
		{
			stLoopTime=stMinFPS;
		}

		stRealTime+=stLoopTime;
//		SetMultiLayerTime(stRealTime);
	}
}
*/
float R3GetLoopTime()	//한루프의 도는 시간을 건진다.
{
	return stLoopTime;
}
void R3SetLoopTime(float time)	//한루프의 도는 시간을 건진다.
{
	stLoopTime=time;
}

float R3GetTime()				//현재 시간을 알아낸다.
{
	return stRealTime;
}
void R3SetTime(float time)	//타임을 재 설정...
{
	stRealTime=time;
}
void R3SetMinFPS(float min_fps)	//최소 프래임 레이트를 정의한다.
{
	stMinFPS=1.0f/min_fps;
}

