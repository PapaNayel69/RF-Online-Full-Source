#ifndef _NPCharacter_h_
#define _NPCharacter_h_

#include "BaseFld.h"

struct _npc_fld : public _base_fld
{ 
	_str_code   m_strName;               // ÀÌ¸§
	int         m_nRace;                 // Á¾Á·
	BOOL        m_bWarType;              // ÀüÅõ¿©ºÎ
	float       m_fTarDecType;           // Type
	float       m_fLevel;                // ·¹º§
	float       m_fExt;                  // °æÇèÄ¡
	float       m_fAttFcStd;             // °ø°İ·Â±âÁØ
	float       m_fMinAFSelProb;         // ÃÖ¼Ò°ø°İ·Â¼±ÅÃÈ®·ü
	float       m_fMaxAFSelProb;         // ÃÖ´ë°ø°İ·Â¼±ÅÃÈ®·ü
	float       m_fAttSklUnit;           // °ø°İ±â¼ú¼÷·Ãµµ
	float       m_fDefSklUnit;           // ¹æ¾î±â¼ú¼÷·Ãµµ
	float       m_fWeakPart;             // ¾àÇÑºÎÀ§
	float       m_fStdDefFc;             // ±âÁØ¹æ¾î·Â
	float       m_fFireTol;              // ºÒ³»¼º
	float       m_fWaterTol;             // ¹°³»¼º
	float       m_fSoilTol;              // Èë³»¼º
	float       m_fWindTol;              // ¹Ù¶÷³»¼º
	float       m_fAttTechID1;           // °ø°İ±â¼ú1_ID
	float       m_fAttTech1UseProb;      // °ø°İ±â¼ú1_¹ßµ¿È®·ü
	float       m_fAttTechID2;           // °ø°İ±â¼ú2_ID
	float       m_fAttTech2UseProb;      // °ø°İ±â¼ú2_¹ßµ¿È®·ü
	float       m_fAttTechID3;           // °ø°İ±â¼ú3_ID
	float       m_fAttTech3UseProb;      // °ø°İ±â¼ú3_¹ßµ¿È®·ü
	float       m_fAttTechMinAFProb;     // °ø°İ±â¼úÃÖ¼Ò°ø°İ·Â³ª¿ÃÈ®·ü
	float       m_fAttTechMinAF;         // °ø°İ±â¼úÃÖ¼Ò°ø°İ·Â
	float       m_fAttTechMaxAFProb;     // °ø°İ±â¼úÃÖ´ë°ø°İ·Â³ª¿ÃÈ®·ü
	float       m_fAttTechMaxAF;         // °ø°İ±â¼úÃÖ´ë°ø°İ·Â
	float       m_fPSecTechID;           // ÇÃ·¹ÀÌ¾îº¸Á¶±â¼úID
	float       m_fMSecTechID;           // Mobº¸Á¶±â¼úID
	float       m_fMaxHP;                // ÃÖ´ëHP
	float       m_fHPRecDelay;           // HPÈ¸º¹Delay
	float       m_fHPRecUnit;            // HPÈ¸º¹·®
	float       m_fAttSpd;               // °ø°İ¼Óµµ
	float       m_fAttMoTime1;           // °ø°İ¸ğ¼Ç1½Ã°£
	float       m_fAttMoTime2;           // °ø°İ¸ğ¼Ç2½Ã°£
	float       m_fViewExt;              // ½Ã¾ß¹üÀ§
	float       m_fAttExt;               // °ø°İ¹üÀ§
	float       m_fMRefExt;              // ¸÷°Ë»ö¹üÀ§
	float       m_fCopTime;              // Çùµ¿½Ã°£
	float       m_fMovSpd;               // ÀÌµ¿¼Óµµ
	float       m_fWarMovSpd;            // ÀüÅõÀÌµ¿¼Óµµ
	float       m_fScaleRate;            // ¸÷Scale
	BOOL        m_bScaleChange;          // Scaleº¯µ¿¿©ºÎ
	float       m_fWidth;                // ¸÷Å©±â(¹Ù¿îµù¹Ú½ºÁö¸§)
	float       m_fWaitTime;             // ´ë±â½Ã°£¥ø1
	int         m_nAsitReqRate;          // µµ¿ò¿äÃ»ÇÒÈ®·ü¥ø2
	int         m_nAsitAptRate;          // µµ¿ò¿äÃ»¼ö¶ôÇÒÈ®·ü¥ø3
	_str_code   m_strChildMon;           // ÀÚ½ÄMobIndex(Default=ÀÚ½ÅÀÇIndexºÎ¸ğMobÀÌÁ×¾úÀ»¶§Regen½ÃÅ³¸¶¸®¼öEmotionType°áÁ¤
	int         m_nChildMonNum;          // ºÎ¸ğMobÀÌÁ×¾úÀ»¶§Regen½ÃÅ³¸¶¸®¼öEmotionType°áÁ¤
	float       m_fEmoType;              // EmotionType°áÁ¤
	float       m_fDamHPStd;             // ±ğÀÏHP±âÁØ°ª¥ø9
	float       m_fEmoImpStdTime;        // Emotion»óÅÂÁÁ¾ÆÁö°ÔÇÏ´Â±âÁØ½Ã°£¥ÁÁÀ½¡æº¸ÅëÃÖ´ëHP±âÁØÇöÀçHP%¥ø13
	float       m_fGoodToOrdHPPer;       // ÁÁÀ½¡æº¸ÅëÃÖ´ëHP±âÁØÇöÀçHP%¥ø13
	float       m_fOrdToBadHPPer;        // º¸Åë¡æ³ª»İÃÖ´ëHP±âÁØÇöÀçHP%¥ø14
	float       m_fBadToWorseHPPer;      // ³ª»İ¡æ¸Å¿ì³ª»İÃÖ´ëHP±âÁØÇöÀçHP%¥µµ¸Á¿©ºÎÈ®·ü¥ø16
	float       m_fEspTFProb;            // µµ¸Á¿©ºÎÈ®·ü¥ø16
	float       m_fTypeCompTerms;        // Typeº°ºñ±³Á¶°Ç¥ø17
	float       m_fPSecTechChat;         // Pº¸Á¶±â¼ú»ç¿ë½ÃChat¥ø18
	float       m_fPAttTechChat;         // °ø°İ±â¼ú»ç¿ë½ÃChat¥ø19
	float       m_fEmo0Chat;             // Emotionº¸ÅëÀÏ¶§ÀÇChat¥ø20index
	float       m_fEmo0ChatProb;         // Emotionº¸ÅëÀÏ¶§ÀÇChatÇÒÈ®·ü¥ø21
	float       m_fEmo1Chat;             // EmotionºÒÄèÀÏ¶§ÀÇChat¥ø22index
	float       m_fEmo1ChatProb;         // EmotionºÒÄèÀÏ¶§ÀÇChatÇÒÈ®·ü¥ø23
	float       m_fEmo2Chat;             // EmotionÈïºĞÀÏ¶§ÀÇChat¥ø24index
	float       m_fEmo2ChatProb;         // EmotionÈïºĞÀÏ¶§ÀÇChatÇÒÈ®·ü¥ø25
	float       m_fEmo3Chat;             // EmotionºĞ³ëÀÏ¶§ÀÇChat¥ø26index
	float       m_fEmo3ChatProb;         // EmotionºĞ³ëÀÏ¶§ÀÇChatÇÒÈ®·ü¥ø27
	float       m_fEmo4Chat;             // Emotion±¤ºĞÀÏ¶§ÀÇChat¥ø28index
	float       m_fEmo4ChatProb;         // Emotion±¤ºĞÀÏ¶§ÀÇChatÇÒÈ®·ü¥ø29
	float       m_fAsitReqSteEspChat;    // µµ¿ò¿äÃ»Stateµµ¸ÁÀÏ¶§Chat¥ø30indµµ¿ò¿äÃ»Stateµµ¸ÁÀÏ¶§ChatÇÒÈ®·ü¥µµ¿ò¿äÃ»StateHelpÀÏ¶§Chat¥ø32indµµ¿ò¿äÃ»StateHelpÀÏ¶§ChatÇÒÈ®·ü¥µµ¿ò¿äÃ»StateÇùµ¿ÀÏ¶§Chat¥ø34indµµ¿ò¿äÃ»StateÇùµ¿ÀÏ¶§ChatÇÒÈ®·ü¥ø35
	float       m_fAsitReqSteEspChatProb;// µµ¿ò¿äÃ»Stateµµ¸ÁÀÏ¶§ChatÇÒÈ®·ü¥µµ¿ò¿äÃ»StateHelpÀÏ¶§Chat¥ø32indµµ¿ò¿äÃ»StateHelpÀÏ¶§ChatÇÒÈ®·ü¥µµ¿ò¿äÃ»StateÇùµ¿ÀÏ¶§Chat¥ø34indµµ¿ò¿äÃ»StateÇùµ¿ÀÏ¶§ChatÇÒÈ®·ü¥ø35
	float       m_fAsitReqSteHelpChat;   // µµ¿ò¿äÃ»StateHelpÀÏ¶§Chat¥ø32indµµ¿ò¿äÃ»StateHelpÀÏ¶§ChatÇÒÈ®·ü¥µµ¿ò¿äÃ»StateÇùµ¿ÀÏ¶§Chat¥ø34indµµ¿ò¿äÃ»StateÇùµ¿ÀÏ¶§ChatÇÒÈ®·ü¥ø35
	float       m_fAsitReqSteHelpChatProb;// µµ¿ò¿äÃ»StateHelpÀÏ¶§ChatÇÒÈ®·ü¥µµ¿ò¿äÃ»StateÇùµ¿ÀÏ¶§Chat¥ø34indµµ¿ò¿äÃ»StateÇùµ¿ÀÏ¶§ChatÇÒÈ®·ü¥ø35
	float       m_fAsitReqSteCopChat;    // µµ¿ò¿äÃ»StateÇùµ¿ÀÏ¶§Chat¥ø34indµµ¿ò¿äÃ»StateÇùµ¿ÀÏ¶§ChatÇÒÈ®·ü¥ø35
	float       m_fAsitReqSteCopChatProb;// µµ¿ò¿äÃ»StateÇùµ¿ÀÏ¶§ChatÇÒÈ®·ü¥ø35

	_npc_fld()
	{
		ZeroMemory( this, sizeof( *this ) );
	}
};

#endif
