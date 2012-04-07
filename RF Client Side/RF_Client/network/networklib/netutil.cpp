#include "NetUtil.h"
#include <winsock2.h>
#include <stdlib.h>

BOOL GetIPAddress(char* szAddr)
{
	char	lpszHostName[255];
	char*	pIP; 
	PHOSTENT pHostEntry;
	IN_ADDR inAddr;
	
	gethostname( lpszHostName, 255 );
	pHostEntry = gethostbyname( lpszHostName ) ;
	memcpy( &inAddr, pHostEntry->h_addr, 4 );

	pIP = inet_ntoa(inAddr);

	memcpy(szAddr, pIP, 16);

	return TRUE;
}

BOOL GetIPAddress(sockaddr_in* pAddr)
{
	char	lpszHostName[255];
	PHOSTENT pHostEntry;
	
	gethostname( lpszHostName, 255 );
	pHostEntry = gethostbyname( lpszHostName ) ;
	memcpy( pAddr, pHostEntry->h_addr, 4 );

	return TRUE;
}

DWORD GetIPAddress()
{
	char szAddr[64];

	if(GetIPAddress(szAddr))
		return inet_addr(szAddr);

	return 0xFFFFFFFF;
}

void AtoH(char * src, char * dest, int destlen)
{
    char * srcptr;

    srcptr = src;

    while(destlen--)
    {
		*dest = BtoH(*srcptr++) << 4;    // Put 1st ascii byte in upper nibble.
		*dest++ += BtoH(*srcptr++);      // Add 2nd ascii byte to above.
    }
}


unsigned char BtoH(char ch)
{
    if (ch >= '0' && ch <= '9') return (ch - '0');        // Handle numerals
    if (ch >= 'A' && ch <= 'F') return (ch - 'A' + 0xA);  // Handle capitol hex digits
    if (ch >= 'a' && ch <= 'f') return (ch - 'a' + 0xA);  // Handle small hex digits
    return(255);
}

void HtoA(char * src, char * dest, int srclen)
{
    char * destptr; // temp pointers
    UCHAR * srcptr;
        
    srcptr = (UCHAR *)src;
    destptr = dest;

    while(srclen--)
    {
    *destptr++ = HtoB((UCHAR)(*srcptr >> 4));      // Convert high order nibble
    *destptr++ = HtoB((UCHAR)(*srcptr++ & 0x0F));  // Convert low order nibble
    }
    *destptr = 0;  // Null terminator
}

char HtoB(UCHAR ch)
{
    if (ch <= 9) return ('0' + ch);             // handle decimal values
    if (ch <= 0xf) return ('A' + ch - 10);      // handle hexidecimal specific values
    return('X');                                // Someone screwed up
}

bool GetSocketName(SOCKET socket, char* pOut)
{
	sockaddr Addr;
	int	nlen = sizeof(Addr);
	
	// Get full network.number.socket address

    if(getsockname(socket, &Addr, &nlen) == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		return false;
	}

    else
        GetAddrString((sockaddr_ipx*)&Addr, pOut);

	return true;	
}

void GetAddrString(sockaddr_ipx* pSAddr, char * dest)
{
    char abuf[15];                                // temp buffer
    char * currptr;                               // temp destination pointer
    int saddrlen = sizeof(struct sockaddr_ipx);   // sizeof address struc

    currptr = dest; // initialize destination pointer

    HtoA((char *)&pSAddr->sa_netnum, abuf, 4);    // convert network number
    lstrcpy(currptr, abuf);
    currptr += 8;
    lstrcat(currptr, ".");                        // don't forget seperator
    currptr++;
    
    HtoA((char *)&pSAddr->sa_nodenum, abuf, 6);   // convert node number
    lstrcat(currptr, abuf);
    currptr += 12;
    lstrcat(currptr, ".");                        // seperator
    currptr++;

    HtoA((char *)&pSAddr->sa_socket, abuf, 2);    // convert socket number
    lstrcat(currptr, abuf);   
}

bool IPConvertingStringToByte(char* szIP, BYTE* pbyIP)
{
	char szBuf[4];

	int nCnt = -1;
	int nIPCnt = 0;
	while(szIP[nCnt] != NULL)
	{
		for(int i = 0; i < 4; i++)
		{
			nCnt++;
			if(i == 4)
				return false;

			if(szIP[nCnt] != '.' && szIP[nCnt] != NULL)
			{
				szBuf[i] = szIP[nCnt];
			}
			else
			{
				szBuf[i] = NULL;
				break;
			}			
		}

		pbyIP[nIPCnt] = (BYTE)atoi(szBuf);
		nIPCnt++;
	}

	return true;
}

void NetTrace(const char *fmt, ...)
{
	va_list arg_ptr;
	char str[256] = {0,};

	va_start(arg_ptr, fmt);
	vsprintf(str, fmt, arg_ptr);
	va_end(arg_ptr);

	strcat(str, "\n");
	
    OutputDebugString( str );
}

int FindAllFile(char* pszDirectory, char** OUT ppszFileName, int nMax)
{
	WIN32_FIND_DATA wfd;

	HANDLE hSrch = FindFirstFile(pszDirectory, &wfd);
	BOOL bResult = TRUE;
	int nCnt = 0;

	while(bResult) 
	{
		if(hSrch != INVALID_HANDLE_VALUE)
		{
			ppszFileName[nCnt] = new char [128];
			strcpy(ppszFileName[nCnt], wfd.cFileName);
			nCnt++;
			if(nCnt >= nMax)
				break;
		}
		bResult = FindNextFile(hSrch, &wfd);
	}
	FindClose(hSrch);

	return nCnt;
}


