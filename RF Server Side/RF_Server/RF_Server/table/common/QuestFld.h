#ifndef _Quest_h_
#define _Quest_h_

#include "BaseFld.h"
#include "CharacterDB.h"

#define action_node_num max_quest_act_num
#define reward_item_num 6 
#define	reward_mastery_num 2

struct _action_node
{
	int         m_nActType;             // 행위
	_str_code   m_strActSub;            // 행위대상
	int         m_nReqAct;              // 요구행위회수
};

struct _quest_reward_item
{
	_str_code   m_strConsITCode;        // 보상아이템코드
	int         m_nConsITCnt;           // 보상아이템수량
};

struct _quest_reward_mastery
{
	int         m_nConsMasteryID;       // 보상숙련도인덱스
	int         m_nConsMasterySubID;    // 보상숙련도서브인덱스
	int         m_nConsMasteryCnt;      // 보상숙련도상승값
};

struct _Quest_fld : public _base_fld
{ 
	int         m_nLimLv;                // 수행제한레벨
	int			m_nLimMin;				 // 수행제한시간(분)
	_action_node	m_ActionNode[action_node_num];
	int         m_nConsExp;              // 경험치보상
	int         m_nConsDalant;           // 달란트보상
	int         m_nConsGold;             // 골드보상
	_quest_reward_item	m_RewardItem[reward_item_num];
	_quest_reward_mastery m_RewardMastery[reward_mastery_num];
	_str_code   m_strConsSkillCode;      // 보상스킬코드
	int         m_nConsSkillCnt;         // 보상스킬상승값
	_str_code   m_strConsForceCode;      // 보상포스코드
	int         m_nConsForceCnt;         // 보상포스상승값
	_str_code   m_strLink;               // 링크퀘스트
	_txt_code   m_txtQTExp;              // 퀘스트설명

	_Quest_fld()
	{
		ZeroMemory( this, sizeof( *this ) );
	}
};

#endif
