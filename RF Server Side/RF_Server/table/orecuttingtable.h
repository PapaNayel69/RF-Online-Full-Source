#ifndef _OreCuttingTable_h_
#define _OreCuttingTable_h_

#include "OreCuttingFld.h"

class COreCuttingTable
{
public:

	CRecordData m_tblOreCutting;

	struct _ore_cut_list{

		struct _res_list{
			WORD wResIndex;
			DWORD dwRate;
			DWORD dwCumRate;//´©ÀûÈ®·ü
		};

		int nResNum;
		DWORD dwTotalRate;	//È®·üÃÑÇÕ
		_res_list ResList[100];

		_ore_cut_list(){
			nResNum = 0;
			dwTotalRate = 0;
		}
	};

	int m_nOreNum;
	_ore_cut_list* pOreList;

public:
	COreCuttingTable(){
		m_nOreNum = 0;
		pOreList = NULL;
	}

	virtual ~COreCuttingTable(){
		if(pOreList)
			delete [] pOreList;
	}

	bool ReadRecord(char* szFile, CRecordData* pOreRec, CRecordData* pResRec){
		if(!m_tblOreCutting.ReadRecord(szFile, sizeof(_OreCutting_fld)))
			return false;

		m_nOreNum = pOreRec->GetRecordNum();

		if(m_nOreNum <= 0)
			return false;

		pOreList = new _ore_cut_list [m_nOreNum];
		return Indexing(pOreRec, pResRec);		
	}

	DWORD GetOreIndexFromRate(DWORD dwOreIndex, DWORD dwRate){
		for(DWORD i = 0; i < (DWORD)pOreList[dwOreIndex].nResNum; i++)
		{
			if(dwRate < pOreList[dwOreIndex].ResList[i].dwCumRate)
				return pOreList[dwOreIndex].ResList[i].wResIndex;
		}
		return 0;//xFFFFFFFF;
	}

private:
	bool Indexing(CRecordData* pOreRec, CRecordData* pResRec)
	{
		for(int i = 0; i < m_nOreNum; i++)
		{
			_base_fld* pFld = pOreRec->GetRecord(i);
			int nOreIndex = pFld->m_dwIndex;
			int nResNum = 0;
			DWORD dwTotalRate = 0;

			for(int r = 0; r < m_tblOreCutting.GetRecordNum(); r++)
			{
				_OreCutting_fld* pOreCutFld = (_OreCutting_fld*)m_tblOreCutting.GetRecord(r);

				if(strcmp(pOreCutFld->m_strCode, pFld->m_strCode))
					continue;

				_base_fld* pResFld = pResRec->GetRecord(pOreCutFld->m_strResource_Item);
				if(!pResFld)
					return false;

				pOreList[nOreIndex].ResList[nResNum].wResIndex = (WORD)pResFld->m_dwIndex;
				pOreList[nOreIndex].ResList[nResNum].dwRate = pOreCutFld->m_nCutting_probability;
				dwTotalRate += pOreCutFld->m_nCutting_probability;
				pOreList[nOreIndex].ResList[nResNum].dwCumRate = dwTotalRate;
				nResNum++;

			}
			pOreList[nOreIndex].nResNum = nResNum;
			pOreList[nOreIndex].dwTotalRate = dwTotalRate;
		}
		return true;
	}
};


#endif