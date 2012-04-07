// WeaponBulletLinkTable.h: interface for the CWeaponBulletLinkTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WEAPONBULLETLINKTABLE_H__1040F965_23D2_4B43_8688_25CF6F66E061__INCLUDED_)
#define AFX_WEAPONBULLETLINKTABLE_H__1040F965_23D2_4B43_8688_25CF6F66E061__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WeaponBulletLinkFld.h"

class CWeaponBulletLinkTable 
{
public:

	enum { max_link_num = _weapon_bullet_link_fld::max_list };

	CRecordData m_tblBullet;
	int** m_ppnWeaponIndex;	
	int m_nBulletNum;

public:
	CWeaponBulletLinkTable(){
		m_ppnWeaponIndex = NULL;
		m_nBulletNum = 0;
	}

	virtual ~CWeaponBulletLinkTable(){
		if(m_ppnWeaponIndex)
		{
			for(int i = 0; i < m_nBulletNum; i++)
			{
				if(m_ppnWeaponIndex[i])
					delete [] m_ppnWeaponIndex[i];
			}
			delete [] m_ppnWeaponIndex;
		}
	}

	bool ReadRecord(char* szFile, CRecordData* pWpRec){
		if(!m_tblBullet.ReadRecord(szFile, sizeof(_weapon_bullet_link_fld)))
			return false;

		m_nBulletNum = m_tblBullet.GetRecordNum();
		if(m_nBulletNum <= 0)
			return false;

		m_ppnWeaponIndex = new int* [m_nBulletNum];
		return Indexing(pWpRec);		
	}

	int GetWPIndexFromLinkSlot(int nBulletIndex, int nSlotIndex)
	{
		if(nBulletIndex >= m_nBulletNum)
			return -1;
		if(nSlotIndex >= max_link_num)
			return -1;

		return m_ppnWeaponIndex[nBulletIndex][nSlotIndex];
	}

private:
	
	bool Indexing(CRecordData* pWpRec)
	{
		int nRecNum = m_tblBullet.GetRecordNum();

		for(int i = 0; i < nRecNum; i++)
		{
			_weapon_bullet_link_fld* pRec = (_weapon_bullet_link_fld*)m_tblBullet.GetRecord(i);

			m_ppnWeaponIndex[i] = new int [max_link_num];
			memset(m_ppnWeaponIndex[i], -1, sizeof(int)*max_link_num);

			for(int r = 0; r < max_link_num; r++)
			{
				if(pRec->m_strUsingWeaponCode[r][0] == '0')
					continue;

				_base_fld* pFld = pWpRec->GetRecord(pRec->m_strUsingWeaponCode[r]);
				if(!pFld)
					return false;

				m_ppnWeaponIndex[i][r] = pFld->m_dwIndex;
			}
		}

		return true;
	}
};

#endif // !defined(AFX_WEAPONBULLETLINKTABLE_H__1040F965_23D2_4B43_8688_25CF6F66E061__INCLUDED_)
