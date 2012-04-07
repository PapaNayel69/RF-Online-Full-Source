#ifndef _ItemLootTable_h_
#define _ItemLootTable_h_

#include "ItemLootFld.h"
#include <stdio.h>

class CItemLootTable
{
public:

	struct _linker_code {
		BYTE byTableCode;
		WORD wItemIndex;
		BOOL bExist;
	};

	CRecordData m_tblLoot;
	_linker_code** m_ppLinkCode;
	int m_nLootNum;	

public:
	CItemLootTable(){
		m_ppLinkCode = NULL;
		m_nLootNum = 0;
	}

	virtual ~CItemLootTable(){
		if(m_ppLinkCode)
		{
			for(int i = 0; i < m_nLootNum; i++)
			{
				if(m_ppLinkCode[i])
					delete [] m_ppLinkCode[i];
			}
			delete [] m_ppLinkCode;
		}
	}

	bool ReadRecord(char* szFile, CRecordData* pItemRec){
		if(!m_tblLoot.ReadRecord(szFile, sizeof(_ItemLooting_fld)))
			return false;

		m_nLootNum = m_tblLoot.GetRecordNum();
		if(m_nLootNum <= 0)
			return false;

		m_ppLinkCode = new _linker_code* [m_nLootNum];
		return Indexing(pItemRec);		
	}

private:
	bool Indexing(CRecordData* pItemRec)
	{
		for(int i = 0; i < m_nLootNum; i++)
		{
			_ItemLooting_fld* pRec = (_ItemLooting_fld*)m_tblLoot.GetRecord(i);

			if(pRec->m_nLootListCount <= 0)
			{
				m_ppLinkCode[i] = NULL;
				continue;
			}
			m_ppLinkCode[i] = new _linker_code [max_loot_kind];
			memset(m_ppLinkCode[i], 0, sizeof(_linker_code)*pRec->m_nLootListCount);
			_str_code* pstr = &pRec->m_strLootCodeKey_1;

			for(int r = 0; r < max_loot_kind; r++)
			{
				if(!strcmp(pstr[r], "0"))
				{
					pRec->m_nLootListCount = r;
					break;
				}

				int nTable = ::GetItemTableCode(pstr[r]);
				if(nTable == -1)
				{
					char szMsg[128];
					sprintf(szMsg, "%d rec %s item no search table", i, pstr[r]);
					::MessageBox(NULL, szMsg, "CMainThread::LinkLootCode() error", MB_OK);
					return false;
				}

				_base_fld* pFld = pItemRec[nTable].GetRecord(pstr[r]);
				if(!pFld)
				{
					char szMsg[128];
					sprintf(szMsg, "%d rec %s item no search index", i, pstr[r]);
					::MessageBox(NULL, szMsg, "CMainThread::LinkLootCode() error", MB_OK);
					return false;
				}

				m_ppLinkCode[i][r].byTableCode = nTable;
				m_ppLinkCode[i][r].wItemIndex = (WORD)pFld->m_dwIndex;
				m_ppLinkCode[i][r].bExist = (BOOL)*(((char*)pFld)+sizeof(_base_fld));
			}
		}
		return true;
	}
};


#endif