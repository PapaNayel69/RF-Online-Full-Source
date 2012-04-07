#ifndef _CCONSOLEWIN_
#define _CCONSOLEWIN_

#include <windows.h>
#include "Singleton.h"

class CConsoleWin : public CSingleton<CConsoleWin>
{
private:
	HANDLE m_hStdOut;
	HANDLE m_hStdIn;
	bool m_bCreate;

public:
	CConsoleWin();
	~CConsoleWin();

	void Create( int width = 80 , int height = 25 );
	void Create( char* title, int width, int height );
	void Out( const char *string );
};

#define gConsoleWin  CConsoleWin::GetSingleton()
#endif