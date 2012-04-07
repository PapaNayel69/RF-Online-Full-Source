#ifndef _QuestEvent_h_
#define _QuestEvent_h_

#include "BaseFld.h"

#define quest_node_per_event	5
#define quest_cont_per_node		5
#define quest_link_per_node		5

struct _event_condition_node
{
	int         m_nAcepIDXType;       // 수락조건
	int         m_nAcepSub;           // 수락대상
	_str_code   m_strAcepVal;         // 수락값
};

struct _event_node
{
	bool        m_bActiveType;			 // 사용여부
	int         m_nQTType;               // 퀘스트구분
	bool        m_bSelectQuestManual;   // 링크타입
	int         m_nAcepProNum;          // 퀘스트수락확률분자
	int         m_nAcepProDen;          // 퀘스트수락확률분모
	_event_condition_node	m_CondNode[quest_cont_per_node];
	_str_code   m_strLink[quest_link_per_node];            // 링크퀘스트
};

struct _QuestEvent_fld : public _base_fld
{ 
	_event_node	m_Node[quest_node_per_event];

	_QuestEvent_fld()
	{
		ZeroMemory( this, sizeof( *this ) );
	}
};

#endif
