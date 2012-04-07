#include "R3Input.h"

_MOUSE gMouse;
int gScan[256];
int gScanExt[256];
int gAsci[256];
int gGetKey[256];

int GetCharFromKey()
{
	int i,add=0;
	short state;

	for(i=0; i<256; i++)
	{
		if( gGetKey[i] )
		{
			gGetKey[i]=0;
			if( i >= VK_A && i <= VK_Z )
			{
				state=GetKeyState(VK_CAPITAL);
				state=(GetKeyState(VK_RSHIFT)&0xfffe)|state;
				state=(GetKeyState(VK_LSHIFT)&0xfffe)|state;
				if( state )
					add=32;
				return 0x61 + (i-VK_A)-add;
			}
			if( i >= VK_0 && i <= VK_9 )
			{
				return i;
			}
			if( 190 == i )		//'.'
				return 0x2e;	
			if(  0x8== i )
				return -1;
			if( 0x2E == i )
				return -2;
			if( 189 == i )
			{
				state=(GetKeyState(VK_RSHIFT)&0xfffe);
				state=(GetKeyState(VK_LSHIFT)&0xfffe)|state;
				if( state )
					return 95;		//'_'
				else
					return 45;		//'-'
			}
			if( ' ' == i )
				return ' ';
			if( VK_RETURN == i )
				return -3;

			if( VK_LEFT == i )
				return -4;
			if( VK_RIGHT == i )
				return -5;
			if( VK_UP == i )
				return -6;
			if( VK_DOWN == i )
				return -7;
		}
	}
	return 0;
}
