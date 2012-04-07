#ifndef _ItemMakeDataTable_h_
#define _ItemMakeDataTable_h_

#include "ItemMakeDataFld.h"

class CItemMakeDataTable
{
public:

	struct _material_list{

		BYTE byItemTblCode;
		WORD wItemIndex;
		DWORD dwPreserveKey;
		int nMaterNum;

		struct _res_list{
			BYTE byTblCode;
			WORD byItemIndex;
			BYTE byConsumNum;
		};

		_res_list ResList[_ItemMakeData_fld::max_material_num];

		_material_list(){
			nMaterNum = 0;
			dwPreserveKey = 0;
		}
	};

private:

	CRecordData m_tblItemMakeData;
	int m_nItemListNum;
	_material_list* m_pListMaterial;

public:
	CItemMakeDataTable(){
		m_nItemListNum = 0;
		m_pListMaterial = NULL;
	}

	virtual ~CItemMakeDataTable(){
		if(m_pListMaterial)
			delete [] m_pListMaterial;
	}

	bool ReadRecord(char* szFile, CRecordData* pItemRec){
		if(!m_tblItemMakeData.ReadRecord(szFile, sizeof(_ItemMakeData_fld)))
			return false;

		m_nItemListNum = m_tblItemMakeData.GetRecordNum();

		if(m_nItemListNum <= 0)
			return false;

		m_pListMaterial = new _material_list [m_nItemListNum];
		return Indexing(pItemRec);		
	}
	
	_material_list* GetRecord(DWORD i){
		if(i >= (DWORD)m_nItemListNum)
			return NULL;

		return &m_pListMaterial[i];
	}

	_ItemMakeData_fld* GetRecordFld(DWORD i){
		return (_ItemMakeData_fld*)m_tblItemMakeData.GetRecord(i);
	}

	int GetSize(){
		return m_nItemListNum;
	}

private:
	bool Indexing(CRecordData* pItemRec)
	{
		for(int i = 0; i < m_nItemListNum; i++)
		{
			_ItemMakeData_fld* pMakeData = (_ItemMakeData_fld*)m_tblItemMakeData.GetRecord(i);
			m_pListMaterial[i].byItemTblCode = ::GetItemTableCode(pMakeData->m_strCode);
			if(m_pListMaterial[i].byItemTblCode == 0xFF)
				return false;
			_base_fld* pFld = pItemRec[m_pListMaterial[i].byItemTblCode].GetRecord(pMakeData->m_strCode);
			if(!pFld)
				return false;
			m_pListMaterial[i].wItemIndex = (WORD)pFld->m_dwIndex;
			
			int nMaterialCnt = 0;
			DWORD dwPreserveKey = 0;
			for(int m = 0; m < _ItemMakeData_fld::max_material_num; m++)
			{
				_ItemMakeData_fld::_material_list* pMaterial = &pMakeData->m_listMaterial[m];
				BYTE byTblCode = ::GetItemTableCode(pMaterial->m_strPdMat);
				if(byTblCode == 0xFF)//0란인경우
					continue;
				_base_fld* pMaterialFld = pItemRec[byTblCode].GetRecord(pMaterial->m_strPdMat);
				if(!pMaterialFld)
					return false;
				WORD wConsum = pMaterial->m_nPdMatNum;
				if(wConsum == 0)
					return false;

				m_pListMaterial[i].ResList[nMaterialCnt].byTblCode = byTblCode;
				m_pListMaterial[i].ResList[nMaterialCnt].byItemIndex = (BYTE)pMaterialFld->m_dwIndex;
				m_pListMaterial[i].ResList[nMaterialCnt].byConsumNum = (BYTE)wConsum;

				nMaterialCnt++;
				dwPreserveKey += ((byTblCode+1) * (pMaterialFld->m_dwIndex+1) * wConsum);
			}
			m_pListMaterial[i].nMaterNum = nMaterialCnt;
			m_pListMaterial[i].dwPreserveKey = dwPreserveKey;
		}
		return true;
	}

};


#endif