#ifndef _protocol_h_
#define _protocol_h_

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
#define unit_msg		23	//add(2/2)
#define recall_msg		22	//add(2/2)
#define sync_msg		20
#define account_msg		21
#define waps_msg		30 //world avatordata public system message..
#define test_msg		40
#define control_msg		50 

#define check_msg		101
#define secure_msg		102	//보안관련..
#define real_msg		222	//클라이언트체크

#define CHECK_KEY_NUM		4

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

//check_msg
#define check_query 1
struct _check_query {

	char sDum;

	int size(){	return sizeof(*this); }
};

#define check_answer 2
struct _check_answer {

	char sDum;

	int size(){	return sizeof(*this); }
};

//secure_msg
#define respon_free_request 1
struct _respon_free_request {

	DWORD dwDeveloperKey[4];//임시로 {0, 1, 2, 3} 로..

	int size(){	return sizeof(*this); }
};

#define check_speed_hack_ask 2
struct _check_speed_hack_ask {
	enum	{	ans_term	= 5000	};	//5초이상 딜레이

	char sDum;

	int size(){	return sizeof(*this); }
};

#define check_speed_hack_ans 3
struct _check_speed_hack_ans {

	char sDum;

	int size(){	return sizeof(*this); }
};


#pragma pack(pop)

#endif