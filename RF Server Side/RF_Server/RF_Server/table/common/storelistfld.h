#ifndef _StoreList_h_
#define _StoreList_h_

#include "BaseFld.h"

#define max_store_list 240

struct _StoreList_fld : public _base_fld
{ 
	_str_code   m_strStore_NPCcode;      // NPC식별코드
	_str_code   m_strStore_NPCname;      // NPC이름
	_str_code   m_strStore_MAPcode;      // 맵Code
	int         m_nStore_trade;          // NPC종류
	int         m_nStore_NPCangle;       // NPC각도
	int         m_nNpc_Class1;           // npc기능1
	int         m_nNpc_Class2;           // npc기능2
	int         m_nNpc_Class3;           // npc기능3
	int         m_nNpc_Class4;           // npc기능4
	int         m_nNpc_Class5;           // npc기능5
	int         m_nStore_LISTcount;      // List개수

	_str_code   m_strItemlist[max_store_list];

	_StoreList_fld()
	{
		ZeroMemory( this, sizeof( *this ) );
	}
};

#endif
