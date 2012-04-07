#ifndef _ItemUpgradeTable_h_
#define _ItemUpgradeTable_h_

#include "ItemUpgradeFld.h"

class CItemUpgradeTable
{
private:

	CRecordData m_tblItemUpgrade;
	int m_nResNum;
	WORD* m_pwResIndex;

public:
	CItemUpgradeTable(){
		m_nResNum = 0;
		m_pwResIndex = NULL;
	}

	virtual ~CItemUpgradeTable(){
		if(m_pwResIndex)
			delete [] m_pwResIndex;
	}

	bool ReadRecord(char* szFile, CRecordData* pResRec){
		if(!m_tblItemUpgrade.ReadRecord(szFile, sizeof(_ItemUpgrade_fld)))
			return false;

		m_nResNum = m_tblItemUpgrade.GetRecordNum();

		if(m_nResNum <= 0)
			return false;

		m_pwResIndex = new WORD [m_nResNum];
		return Indexing(pResRec);		
	}
	
	_ItemUpgrade_fld* GetRecordFromRes(DWORD dwResIndex){
		for(int i = 0; i < m_nResNum; i++)
		{
			if(m_pwResIndex[i] == dwResIndex)
				return (_ItemUpgrade_fld*)m_tblItemUpgrade.GetRecord(i);
		}
		return NULL;
	}

	_ItemUpgrade_fld* GetRecord(DWORD dwIndex){
		if(dwIndex >= (DWORD)m_nResNum)
			return NULL;
		
		return (_ItemUpgrade_fld*)m_tblItemUpgrade.GetRecord(dwIndex);
	}

	int GetSize(){
		return m_nResNum;
	}

private:
	bool Indexing(CRecordData* pResRec)
	{		
		for(int i = 0; i < m_nResNum; i++)
		{
			_base_fld* pDataFld = m_tblItemUpgrade.GetRecord(i);
			_base_fld* pResFld = pResRec->GetRecord(pDataFld->m_strCode);
			if(!pResFld)
				return false;

			m_pwResIndex[i] = (WORD)pResFld->m_dwIndex;
		}
		return true;
	}

};


#endif