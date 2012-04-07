
#include "MainThread.h"
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <crtdbg.h>

extern CMainThread g_Main;

#define		ID_LISTBOX 100
#define		ID_EDIT 101

#define		MAX_OBITEM_COUNT	22
#define		ITEM_TYPE_INT		0
#define		ITEM_TYPE_CHAR		1
#define		ITEM_TYPE_BOOL		2


WNDPROC		g_OldEditProc;
HWND		g_hList, g_hEdit, g_hMain;
HINSTANCE	g_hInst;
char*		g_pszClass = g_Main.GetProcessName();
int			g_nListBoxCount = 0;

static DWORD s_dwFrames = 0;
static DWORD s_dwFrameCount = 0;
static DWORD s_dwFrameTime = timeGetTime();
static bool	 s_bInitSucc = false;

WORD	g_wItemCount = 0;

struct _observe_item_data
{
	BYTE	byItemType;
	char	szItemName[32];
	int*	itemValueInt;
	char	itemValueChar[32];
	bool*	itemValueBool;
};
_observe_item_data itemData[MAX_OBITEM_COUNT];


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK EditSubProc(HWND, UINT, WPARAM, LPARAM);

void StringOutput(const char *fmt, ...);
void ShowOBData();

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
//	_CrtSetBreakAlloc(49);

	MSG			Message;
	WNDCLASS	WndClass;

	g_hInst = hInstance;
	
	WndClass.cbClsExtra=0;
	WndClass.cbWndExtra=0;
//	WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	WndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.lpszClassName = g_pszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	g_hMain = CreateWindow(g_pszClass, g_pszClass, WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, 500, 700,	NULL, (HMENU)NULL, hInstance, NULL);

	ShowWindow(g_hMain, nCmdShow);
	
	while(TRUE)
    {
		if(PeekMessage(&Message, NULL, 0, 0, PM_NOREMOVE))
        {
			if(0 == GetMessage(&Message, NULL, 0, 0))             // WM_QUIT was posted, so exit
            {
				return (int)Message.wParam;
            }
			TranslateMessage(&Message); 
            DispatchMessage(&Message);
        }
        else
        {
			g_Main.MainLoop();
		}

		//calc fps
		DWORD time = timeGetTime();		
		s_dwFrameCount++;
		int time2 = time - s_dwFrameTime;
		if(time2 > 1000)
		{
			s_dwFrames = (s_dwFrameCount*1000)/time2;
			s_dwFrameCount = 0;			
			s_dwFrameTime = time;
		}
		else if(time2 < 0)
		{
			s_dwFrameCount = 0;			
			s_dwFrameTime = time;
		}
	}
    
	return Message.wParam;	
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	char szWndTxt[32];

	switch(iMessage) 
	{
	case WM_CREATE:
		g_hList = CreateWindow("listbox", NULL, WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL|WS_HSCROLL|LBS_SORT|LBS_NOTIFY,
			10, 210, 473, 440, hWnd, (HMENU)ID_LISTBOX, g_hInst, NULL);

		g_hEdit = CreateWindow("edit", NULL, WS_CHILD|WS_VISIBLE|WS_BORDER,
			10, 650, 473, 22, hWnd, (HMENU)ID_EDIT, g_hInst, NULL);

		g_OldEditProc = (WNDPROC)SetWindowLong(g_hEdit, GWL_WNDPROC, (LONG)EditSubProc);
		SetFocus(g_hEdit);

		if(!g_Main.InitMainThread())
		{
			SendMessage(hWnd, WM_DESTROY, 0, 0);
			return 0;
		}

		SetTimer(hWnd, 0, 1000, NULL);

		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		KillTimer(hWnd, 0);
		return 0;

	case WM_TIMER:
		sprintf(szWndTxt, "%s, fps:%d", g_pszClass, s_dwFrames);
		SetWindowText(hWnd, szWndTxt);
		ShowOBData();
		return 0;

	case WM_CLOSE:
		if(MessageBox(hWnd, "Close OK?", g_pszClass, MB_OKCANCEL) == IDOK)
		{
			PostQuitMessage(0);
		//	_CrtMemDumpAllObjectsSince(0);
		}

		return 0;
	}
	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}

LRESULT CALLBACK EditSubProc(HWND hWnd, UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	if(iMessage == WM_KEYDOWN && wParam == VK_RETURN)
	{
		char szCmd[128];
		GetWindowText(hWnd, szCmd, 120);

		StringOutput(szCmd);

		if(!g_Main.CommandProcess(szCmd))
		{
			StringOutput("잘못된 명령어입니다.");
		}

		SetWindowText(hWnd, "");	
	}

	return CallWindowProc(g_OldEditProc, hWnd, iMessage, wParam, lParam);
}

void StringOutput(const char *fmt, ...)
{
	va_list arg_ptr;
	char str[256] = {0,};

	va_start(arg_ptr, fmt);
	vsprintf(str, fmt, arg_ptr);
	va_end(arg_ptr);

    ListBox_InsertString(g_hList, g_nListBoxCount, str);
	PostMessage( g_hList ,WM_VSCROLL, SB_BOTTOM, NULL);
	g_nListBoxCount++;
}

void ShowOBData()
{
	HDC hDC = GetDC( g_hMain );
	if ( hDC == NULL)	return;
	SetBkColor( hDC, RGB( 214, 211, 206));

	char szMessage[128] = {0,};
	WORD wPosX, wPosY;

	for ( int i=0; i<MAX_OBITEM_COUNT; i++ )
	{
		if ( i >= g_wItemCount )
			break;
		
		if ( i <= 10 )
		{
			wPosX = 10;
			wPosY = 10 + i*17;
		}
		else
		{
			wPosX = 260;
			wPosY = 10 + (i-11)*17;
		}

		if ( itemData[i].byItemType == ITEM_TYPE_INT )
		{
			sprintf( szMessage, "%s : %d", itemData[i].szItemName, *itemData[i].itemValueInt );
		}
		else if ( itemData[i].byItemType == ITEM_TYPE_CHAR )
		{
			sprintf( szMessage, "%s : %s", itemData[i].szItemName, itemData[i].itemValueChar );
		}
		else if ( itemData[i].byItemType == ITEM_TYPE_BOOL )
		{
			if ( *itemData[i].itemValueBool )
				sprintf( szMessage, "%s : true", itemData[i].szItemName );
			else
				sprintf( szMessage, "%s : false", itemData[i].szItemName );
		}

		TextOut( hDC, wPosX, wPosY, szMessage, strlen( szMessage));
	}

	ReleaseDC( g_hMain, hDC);
}

void AddOBItemTypeInt( char* szItemName, int* pItemValue )
{
	if ( g_wItemCount >= MAX_OBITEM_COUNT)
	{
		StringOutput("AddOBItem : MAX_OBITEM_COUNT 초과");
		return ;
	}

	strcpy( itemData[g_wItemCount].szItemName, szItemName );
	itemData[g_wItemCount].byItemType = ITEM_TYPE_INT;
	itemData[g_wItemCount].itemValueInt = pItemValue;
	g_wItemCount++;
}

void AddOBItemTypeChar( char* szItemName, char* pItemValue )
{
	if ( g_wItemCount >= MAX_OBITEM_COUNT)
	{
		StringOutput("AddOBItem : MAX_OBITEM_COUNT 초과");
		return ;
	}

	strcpy( itemData[g_wItemCount].szItemName, szItemName );
	itemData[g_wItemCount].byItemType = ITEM_TYPE_CHAR;
	strcpy( itemData[g_wItemCount].itemValueChar, pItemValue );
	g_wItemCount++;
}

void AddOBItemTypeBool( char* szItemName, bool* pItemValue )
{
	if ( g_wItemCount >= MAX_OBITEM_COUNT)
	{
		StringOutput("AddOBItem : MAX_OBITEM_COUNT 초과");
		return ;
	}

	strcpy( itemData[g_wItemCount].szItemName, szItemName );
	itemData[g_wItemCount].byItemType = ITEM_TYPE_BOOL;
	itemData[g_wItemCount].itemValueBool = pItemValue;
	g_wItemCount++;
}