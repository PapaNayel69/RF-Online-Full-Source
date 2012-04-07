#ifndef _protocol_h_
#define _protocol_h_

#include "NetSocket.h"

#define	system_msg		1
#define chat_msg		2
#define init_msg		3
#define	position_msg	4
#define fight_msg		5
#define item_msg		7	
#define map_msg			8
#define observer_msg	10						
#define event_msg		11
#define trade_msg		12	
#define ui_msg			13
#define resource_msg	14
#define cheat_msg		15
#define party_msg		16
#define effect_msg		17
#define dtrade_msg		18
#define dungeon_msg		19
#define unit_msg		23	
#define recall_msg		22	
#define quest_msg		24
#define sync_msg		20
#define account_msg		21
#define waps_msg		30 //world avatordata public system message..
#define test_msg		40
#define control_msg		50 

#pragma pack(push, 1)

//test_msg..
#define stress_send_msg 1
struct _stress_send_msg {

	enum {  max_size = 6400	};

	DWORD	dwSize;
	char	sData[max_size];

	int size(){		
		return sizeof(*this)-(max_size-dwSize); }
};

#define echo_send_msg 2
struct _echo_send_msg {

	enum {  max_size = 6400	};

	DWORD	dwSerial;
	DWORD	dwEchoCount;
	DWORD	dwSize;
	char	sData[max_size];

	int size(){		
		return sizeof(*this)-(max_size-dwSize); }
};





#pragma pack(pop)

#endif