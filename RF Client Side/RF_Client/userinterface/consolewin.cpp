#include "ConsoleWin.h"

bool g_bConsoleLoop = true;

CConsoleWin::CConsoleWin()
{
	this->m_hStdOut = NULL;
	this->m_hStdIn = NULL;
	this->m_bCreate = false;
}

CConsoleWin::~CConsoleWin()
{
	if(m_bCreate)
		FreeConsole();
}

void CConsoleWin::Create(int width , int height)
{
	AllocConsole();
	SetConsoleTitle("debug");

	// Input Console Handle
	m_hStdIn = GetStdHandle(STD_INPUT_HANDLE);

	// Output Console Handle
	m_hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	COORD co = {width,height};
	SetConsoleScreenBufferSize(m_hStdOut, co);

	// Console Mode Setting;	
	SetConsoleMode(m_hStdIn , ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
	m_bCreate = true;
}

void CConsoleWin::Create(char* title, int width , int height)
{
	AllocConsole();
	SetConsoleTitle(title);

	// Input Console Handle
	m_hStdIn = GetStdHandle(STD_INPUT_HANDLE);

	// Output Console Handle
	m_hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	PCONSOLE_SCREEN_BUFFER_INFO screen_info;
	screen_info = NULL;
	GetConsoleScreenBufferInfo(m_hStdOut, screen_info);

	SMALL_RECT window;
	window.Left = 0;
	window.Top = 0;
	window.Right = width;
	window.Bottom = height;
	SetConsoleWindowInfo(m_hStdOut, true, &window);

	COORD co = {width,height};
//	COORD co = {screen_info->srWindow.Right, screen_info->srWindow.Bottom};
	SetConsoleScreenBufferSize(m_hStdOut, co);

	COORD wCoord;
	wCoord.X = 0;
	wCoord.Y = 0;

	WORD wColor;
	wColor = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY;
	DWORD cWritten;

	FillConsoleOutputAttribute(m_hStdOut, wColor, 
							   80*40,
							   wCoord,
							   &cWritten);
//	FillConsoleOutputAttribute(m_hStdOut, wColor, 
//							   screen_info->srWindow.Right*screen_info->srWindow.Bottom,
//							   wCoord,
//							   &cWritten);
	SetConsoleTextAttribute(m_hStdOut, wColor);

	// Console Mode Setting;
	SetConsoleMode(m_hStdIn , ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
	m_bCreate = true;
}


void CConsoleWin::Out(const char *string)
{	
	if( !m_bCreate )
		return;

	DWORD cCharsWritten;
	if( m_hStdOut )
	  WriteConsole(m_hStdOut,string, strlen(string), &cCharsWritten, NULL); 
}

