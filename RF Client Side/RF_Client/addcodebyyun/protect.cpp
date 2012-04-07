#include "Protect.h"
//#include "Winreg.h"
#include <Winable.h>
#include <stdio.h>
#include "../DefineMessage.h"
#include "../AddCodeByJang/EffectUtil.h"

#ifndef _DEVELOPER_
#define __PROTECT_MODE__ 
#endif


#ifdef  __PROTECT_MODE__
#pragma comment ( lib, "PDLL.lib" )
#endif

FARPROC pRecv = (FARPROC)recv;
FARPROC pSend = (FARPROC)send;
FARPROC ptimeGetTime = (FARPROC)timeGetTime;
BOOL	bInstall = false;

char	stitle[256] = "";
char	sclass[256] = "";
DWORD	g_PID  = 0;
extern	HWND			g_hWnd;
DWORD	g_min=0;
DWORD	g_max=0;	
HHOOK	PlayHook = NULL;
HHOOK	RecordHook = NULL;
DWORD	state[12] = {0,};//f1~f12;
DWORD	astate[12] = {0,};//f1~f12;

//FARPROC pWriteProcessMemory = (FARPROC)WriteProcessMemory;
//FARPROC pReadProcessMemory = (FARPROC)ReadProcessMemory;
//FARPROC pVirtualProtectEx = (FARPROC)VirtualProtectEx;
//FARPROC pOpenProcess = (FARPROC)OpenProcess;
//FARPROC pkeybd_event = (FARPROC)keybd_event;

typedef BOOL (WINAPI* RPM)(
    HANDLE hProcess,LPCVOID lpBaseAddress,LPVOID lpBuffer,DWORD nSize,LPDWORD lpNumberOfBytesRead
    );


/*
DWORD CheckEOF(DWORD addr2)
{
	BYTE *start =NULL ;
	
	start = (BYTE *)(addr2);
	DWORD  c =0;
	while ( c<99999 )
	{
		for (int i = 0; i <3; i++)
		{
			if ( start[c] != EndofFunction[i] )
			{
				c++;
				break;
			}
			c++;
		}
		if (i==3 ) 
			return c;
	}
	return NULL;
}
*/
//char *ReadOrg = NULL;


FARPROC orgr = (FARPROC)ReadProcessMemory;
//FARPROC orgr = (FARPROC)SetWindowsHookEx;

// Returns the HMODULE that contains the specified memory address
static HMODULE ModuleFromAddress(PVOID pv) {

   MEMORY_BASIC_INFORMATION mbi;
   return((VirtualQuery(pv, &mbi, sizeof(mbi)) != 0) 
      ? (HMODULE) mbi.AllocationBase : NULL);
}

LRESULT CALLBACK JournalRecordProc(
  int code,       // hook code
  WPARAM wParam,  // not used
  LPARAM lParam   // message being processed
)
{
	return(CallNextHookEx(RecordHook, code, wParam, lParam));
}



LRESULT CALLBACK JournalPlaybackProc(
  int code,       // hook code
  WPARAM wParam,  // not used
  LPARAM lParam   // message being processed
)
{
	//::SendMessage(NULL,WM_CANCELJOURNAL,NULL,NULL);
	//BlockInput(false);
	//return NULL;
	//UnhookWindowsHookEx(PlayHook);
	return(CallNextHookEx(PlayHook, code, wParam, lParam));
}

BOOL Check98()
{
	OSVERSIONINFO vi = { sizeof(vi) };
	GetVersionEx(&vi);
	if (vi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
		return true;
 	}
	return false;
}
BOOL __declspec(dllimport) WINAPI HookAllApps(BOOL fInstall, DWORD dwThreadId);
//BOOL __declspec(dllimport) WINAPI HookAllApps(BOOL fInstall, DWORD dwThreadId,char *title, char *cname);

class GlobalEnd
{
public:
	GlobalEnd()
	{
		/*
		int b =0;
		HMODULE h = GetModuleHandle("PDLL.DLL");
		RPM rpm = (RPM)GetProcAddress(h,"?newEReadProcessMemory@@YGHPAXPBX0KPAK@Z");
		FARPROC pOpenProcess = (FARPROC)OpenProcess;
		OpenProcess(NULL,NULL,NULL);
		DWORD	n = CheckEOF((DWORD)rpm);
		(*rpm)(NULL,NULL,NULL,0,NULL);
		ReadOrg = new char[n];
		b = WriteProcessMemory(GetCurrentProcess(), ReadOrg ,orgr ,n, NULL);//Backup
		b = WriteProcessMemory(GetCurrentProcess(), orgr, rpm,n, NULL);//Change
		//*/
		/*
		//?newSetWindowsHookExA@@YGPAUHHOOK__@@HP6GJHIJ@ZPAUHINSTANCE__@@K@Z
		HMODULE h = GetModuleHandle("PDLL.DLL");
		
		
		SWHookExA rpm = (SWHookExA)GetProcAddress(h,"?newSetWindowsHookExA@@YGPAUHHOOK__@@HP6GJHIJ@ZPAUHINSTANCE__@@K@Z");
		//SWHookExA rpm = (SWHookExA)newSetWindowsHookExA;
		(*rpm)(NULL,NULL,NULL,NULL);
		//RPM rpm = (RPM)GetProcAddress(h,"?newEReadProcessMemory@@YGHPAXPBX0KPAK@Z");
		DWORD	n = CheckEOF((DWORD)rpm);
		//DWORD	n = CheckEOF((DWORD)newSetWindowsHookExA);

		if (n)
		{
			int b = 0;
			ReadOrg = new char[n];
			b = WriteProcessMemory(GetCurrentProcess(), ReadOrg ,orgr ,n, NULL);//Backup
			b = WriteProcessMemory(GetCurrentProcess(), orgr, rpm,n, NULL);//Change
		}
		PlayHook = SetWindowsHookEx(WH_JOURNALPLAYBACK, JournalPlaybackProc, 
			ModuleFromAddress(Protect),0);
			*/
	}
	~GlobalEnd()
	{
		if (bInstall) 
		{
#ifdef  __PROTECT_MODE__
			if (!Check98())
			{
				HookAllApps( false , 0 );
			}
#endif
			bInstall = false;

			

			//b = WriteProcessMemory(GetCurrentProcess(), orgr,ReadOrg ,n, NULL);//Change
			//if ( ReadOrg ) delete[] ReadOrg;
		}
	}
};
GlobalEnd g_End;
/*
void RecoverSocket()
{
	HANDLE hFile,hFileMap;
	char *pMapFile = NULL;
	char system[256];
	DWORD dwSize = 0;
	DWORD start = 0, end = 0;
	GetSystemDirectory(system,256);
	sprintf( system, "%s\\ws2_32.dll",system );
	hFile	= CreateFile( system, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	hFileMap= CreateFileMapping (hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	pMapFile= (char * ) MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, dwSize );
	dwSize = GetFileSize (hFile, 0);
	HMODULE h = GetModuleHandle( "Ws2_32.dll" );
	
	start  = ((DWORD)recv - (DWORD)h) + (DWORD)pMapFile;
	end = dwSize/2 - ((DWORD)pRecv - (DWORD)h);
	DWORD b = WriteProcessMemory(GetCurrentProcess(), pRecv, (void *)start ,end, NULL);//Change
	//DWORD b = WriteProcessMemory(GetCurrentProcess(), recv, recv ,5, NULL);//Change

	UnmapViewOfFile( pMapFile );
	CloseHandle( hFileMap );
	CloseHandle( hFile );
	

}
*/
BOOL CheckSocket()
{
	//RecoverSocket();
	if (pRecv !=(FARPROC)recv ||
		pSend !=(FARPROC)send )
	{
		MessageBox(NULL,"SOCKET ERROR: 다른 네트워크 보안프로그램이 실행 되었을 경우에도, 나타날 수 있습니다.",0,0);
		PostQuitMessage(0);
	}

	BYTE *check = NULL;
	

	check =(BYTE*) recv;
		if ((*check) !=0x55) { 
			MessageBox(NULL,"SOCKET ERROR: 다른 네트워크 보안프로그램이 실행 되었을 경우에도 나타날 수 있습니다.",0,0);
			PostQuitMessage(0); 
		}
	check =(BYTE*) send;
		if ((*check) !=0x55) { 
			//RecoverSocket();
			MessageBox(NULL,"SOCKET ERROR: 다른 네트워크 보안프로그램이 실행 되었을 경우에도, 나타날 수 있습니다.",0,0);
			PostQuitMessage(0); 
		}
	return true;
}
BOOL CheckKeyState()
{
	static DWORD time = 0;

	static DWORD bef = 0;
	DWORD	cur = timeGetTime();
	DWORD	END_TIME = 90;
	if (!bef) bef = cur;
	DWORD	del = cur - bef;
	
	//
	if ( del > 500 )
	{
		for (int i = 0; i < 12; i++)
		{
			short flag = GetAsyncKeyState(VK_F1+i ) & 0x8000;
			if ( flag )
			{
				astate[i] +=1;
			}
			
			if ( gScanExt[VK_F1+i]==1 )
			{
				state[i] +=1;
				//gScanExt[VK_F1+i] = 0;
			}
		}
		bef = cur;
		time++;
	}
	
	if ( time >= END_TIME * 2 )
	{
		time	= 0;
		BOOL	key = false;
		for ( int i = 0; i< 12; i++)
		{
			if ( state[i] >= ( END_TIME- 2 )*2 )
			{
				if ( astate[i] >= ( END_TIME- 2 )*2 )
				{
					key = true;
					//MessageBox(NULL,"키보드에 문제가 있습니다. 프로그램을 종료합니다.",NULL,NULL );
					//PostQuitMessage(0);
					char buf[256];
					sprintf(buf,"Key Error State: %d, %d\n", i,state[i] );
					EFUtil_EffectLog( buf );
				}
		
			}
		}
		if ( key )
		{
			STARTUPINFO         siStartupInfo;

            PROCESS_INFORMATION piProcessInfo;

            memset(&siStartupInfo, 0, sizeof(siStartupInfo));
            memset(&piProcessInfo, 0, sizeof(piProcessInfo));

            siStartupInfo.cb = sizeof(siStartupInfo);

            CreateProcess(".\\SendLog.exe",     // 실행 경로에 유의
                                                                              " -a",                                // 자동 전송용 파라미터
                                                                              0,
                                                                              0,
                                                                              FALSE,
                                                                              CREATE_DEFAULT_ERROR_MODE,
                                                                              0,
                                                                              ".\\",                  // 작업 디렉토리 
                                                                              &siStartupInfo,
                                                                              &piProcessInfo);
			PostMessage( g_hWnd, WM_EXIT_KEYBOARD_MACRO, 0, 0 );
		}

		memset( state,0, sizeof (state ));
	}
	return true;
}
BOOL CheckTime()
{
	if ( ptimeGetTime != (FARPROC)timeGetTime )
	{
		PostQuitMessage(0);
	}
	BYTE *check =(BYTE*) ptimeGetTime;
	if ((*check) !=0x83) { 
		MessageBox(NULL,"SPEED HACK",0,0);
		PostQuitMessage(0);
	} 
	
	return true;
}
void SetReg(char *title, char *cname );
void CheckReg();


void Protect()
{
	
	if (!bInstall) 
	{
	
		/*
		HMODULE h = GetModuleHandle( NULL );
		char	modulename[256];
		GetModuleFileName( h,modulename, 256 );

		

		
		GetWindowText( g_hWnd, stitle, 256 );
		GetClassName ( g_hWnd, sclass, 256 );
		*/
		//SetReg(stitle,sclass);

		/*
		DWORD ID = GetCurrentThreadId();
		RecordHook = SetWindowsHookEx(WH_JOURNALRECORD, JournalRecordProc, 
			ModuleFromAddress(Protect),0);

		PlayHook = SetWindowsHookEx(WH_JOURNALPLAYBACK, JournalPlaybackProc, 
			ModuleFromAddress(Protect),0);
		*/
		//GetReg();
		//HookAllApps(false, 0, 0, 0);
#ifdef  __PROTECT_MODE__
		if (!Check98())
		{
			HookAllApps(true , 0);
		}
#endif
		bInstall = true;

		//int b = WriteProcessMemory(GetCurrentProcess(), ppfn, &pfnNew, 
		//		sizeof(pfnNew), NULL);
		/*
		int b = WriteProcessMemory(GetCurrentProcess(), 
			NULL, 
			NULL, 
			0,NULL
			);

		*/
		//RPM		rpm = newEReadProcessMemory;
		
	}
	/*
	HookAllApps(false, 0, 0, 0);
	HookAllApps(true , 0, stitle, sclass);
	*/
	//CheckReg();
	//CheckSocket();
	CheckTime();
	CheckKeyState();
}
/*
void CheckReg()
{
	HKEY	hKey;
	DWORD	lRet;
	CHAR	title[256];
	//CHAR	cname[256];
	DWORD	dwBufLen = 256;
	DWORD	ID = 0;
	DWORD	IDSIZE = 4;

	DWORD	min=0;
	DWORD	max=0;	
	
	

	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
        TEXT("Software\\CCR\\RFonline"),
                    0,
                    KEY_QUERY_VALUE,
                    &hKey) != ERROR_SUCCESS) return ;

    lRet = RegQueryValueEx(hKey,
                    TEXT("Title"),
                    NULL,
                    NULL,
                    (LPBYTE)title,
                    &dwBufLen);

	
	lRet =  RegQueryValueEx(hKey,             // subkey handle 
				"Process",       // value name 
				NULL,                        // must be zero 
				NULL,		          // value type 
				(LPBYTE) &ID,           // pointer to value data 
				&IDSIZE );      // length of value data 

	lRet = RegQueryValueEx(hKey,
                    TEXT("MMIN"),
                    NULL,
                    NULL,
                    (LPBYTE)&min,
		            &IDSIZE);

	lRet = RegQueryValueEx(hKey,
                    TEXT("MMAX"),
                    NULL,
                    NULL,
                    (LPBYTE)&max,
		            &IDSIZE);
	
	
    RegCloseKey(hKey);
	
	if ( max!=g_max || min!=g_min ) 
	{
		PostQuitMessage(0);
	}
	if (strcmp( title, stitle) != 0)
	{
		PostQuitMessage(0);
	}
}
void SetReg(char *title, char *cname )
{
	if ( !title ) return;
	if ( !cname ) return;
    HKEY hk; 
	DWORD  ID = GetCurrentProcessId();
	
	FILETIME T1,T2,T3,T4;

	g_PID = ID;
	

	GetProcessTimes(GetCurrentProcess(),&T1,&T2,&T3,&T4);
	
	g_max = T1.dwHighDateTime;
	g_min = T1.dwLowDateTime;

	//GetProcessWorkingSetSize(GetCurrentProcess(),&g_min,&g_max);

    if (RegCreateKey(HKEY_LOCAL_MACHINE, 
            "Software\\CCR\\RFonline", &hk)) 
	{
		return;
	}

    if (RegSetValueEx(hk,             // subkey handle 
            "Title",       // value name 
            0,                        // must be zero 
            REG_SZ,            // value type 
            (LPBYTE) title,           // pointer to value data 
            strlen(title) + 1))       // length of value data 
	{
		return;
     }
	
	if (RegSetValueEx(hk,             // subkey handle 
            "MMAX",       // value name 
            0,                        // must be zero 
            REG_DWORD,		          // value type 
            (LPBYTE) &g_max,           // pointer to value data 
            REG_DWORD ))      // length of value data 
	{
		return;
	}

	if (RegSetValueEx(hk,             // subkey handle 
            "MMIN",       // value name 
            0,                        // must be zero 
            REG_DWORD,		          // value type 
            (LPBYTE) &g_min,           // pointer to value data 
            REG_DWORD ))      // length of value data 
	{
		return;
	}
	RegCloseKey(hk); 
} 

//PROCESS_VM_WRITE //ReadProcessMemory
HHOOK
WINAPI
newSetWindowsHookExA(
    int idHook,
    HOOKPROC lpfn,
    HINSTANCE hmod,
    DWORD dwThreadId)
{
	return NULL;
}
*/