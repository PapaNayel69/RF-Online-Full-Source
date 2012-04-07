#ifndef _MapDataTable_h_
#define _MapDataTable_h_

#include "MapFld.h"

class CMapDataTable
{
private:
	_map_fld* m_pRecord;
	DWORD m_dwRecordNum;

public:
	CMapDataTable(){
		m_pRecord = NULL;	
		m_dwRecordNum = 0;
	}

	virtual ~CMapDataTable(){
		if(m_pRecord)
			delete [] m_pRecord;
	}

	bool ReadScript(char* szFileName){
		if(m_pRecord)
			return false;

		FILE *fp = fopen(szFileName, "rt");

		if(fp == NULL)
			return false;

		//calc map num

		char szData[128];
		int nMax = 0;

		while(1)
		{
			if(fscanf(fp, "%s", szData) == EOF)
				break;

			else if(!strcmp(szData, "*"))
				nMax++;
		}

		m_dwRecordNum = nMax;
		m_pRecord = new _map_fld [m_dwRecordNum];

		rewind(fp);

		//load data

		int nCnt = 0;

		while(1)
		{
			if(fscanf(fp, "%s", szData) == EOF)
				break;

			else if(!strcmp(szData, "*"))
			{
				//식별코드
				fscanf(fp, "%s", m_pRecord[nCnt].m_strCode);

				//파일이름
				fscanf(fp, "%s", m_pRecord[nCnt].m_strFileName);

				//맵타입
				fscanf(fp, "%d", &m_pRecord[nCnt].m_nMapType);

				//레이어수
				fscanf(fp, "%d", &m_pRecord[nCnt].m_nLayerNum);

				//마을종족코드
				fscanf(fp, "%d", &m_pRecord[nCnt].m_nRaceVillageCode);

				//몬스터세팅수
				fscanf(fp, "%d", &m_pRecord[nCnt].m_nMonsterSetFileNum);				

				//존서버코드
				fscanf(fp, "%s", m_pRecord[nCnt].m_strServerCode);

				//적정유저수
				fscanf(fp, "%d", &m_pRecord[nCnt].m_nProperUserNum);

				//맵조정여부
				fscanf(fp, "%d", &m_pRecord[nCnt].m_bSizeAdjust);

				//좌표들..
				fscanf(fp, "%d", &m_pRecord[nCnt].m_nMinX);
				fscanf(fp, "%d", &m_pRecord[nCnt].m_nMinY);
				fscanf(fp, "%d", &m_pRecord[nCnt].m_nMaxX);
				fscanf(fp, "%d", &m_pRecord[nCnt].m_nMaxY);

				//인덱스
				m_pRecord[nCnt].m_dwIndex = nCnt;

				nCnt++;
			}
		}

		fclose(fp);

		return true;	
	}

	int GetRecordNum(){
		return m_dwRecordNum;
	}

	_map_fld* GetRecord(char* szCode){
		for(DWORD i = 0; i < m_dwRecordNum; i++)
		{
			if(!strcmp(m_pRecord[i].m_strCode, szCode))
				return &m_pRecord[i];
		}
		return NULL;
	}

	_map_fld* GetRecord(DWORD dwIndex){
		if(dwIndex >= m_dwRecordNum)
			return NULL;

		return &m_pRecord[dwIndex];
	}

};


#endif
