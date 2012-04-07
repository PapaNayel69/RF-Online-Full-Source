#ifndef _BaseFld_h_
#define _BaseFld_h_

#define code_max_length	64
typedef char _str_code[code_max_length]; 
#define txt_max_length	1280
typedef char _txt_code[txt_max_length]; 

struct _base_fld
{
	DWORD m_dwIndex;
	_str_code m_strCode;

	_base_fld()
	{
		ZeroMemory(this, sizeof(*this));
	}
};

struct _record_bin_header
{
	int m_nRecordNum;
	int m_nFieldNum;
	int m_nRecordSize;

	_record_bin_header()
	{
		ZeroMemory(this, sizeof(*this));
	}
};

class CRecordData
{
public:
	_record_bin_header m_Header;
	char** m_psRecord;
	
	CRecordData(){
		m_psRecord = NULL;
	}

	virtual ~CRecordData(){		
		if(m_psRecord)
		{
			for(int i = 0; i < m_Header.m_nRecordNum; i++)
			{
				if(m_psRecord[i])
					delete [] m_psRecord[i];
			}

			delete [] m_psRecord;
		}
	}

	///////////////////////////////////////////
	//Record binary file Load//////////////////
	///////////////////////////////////////////
	bool ReadRecord(char* szFile)
	{
		bool bSuc = true;

		HANDLE hFile = CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
			bSuc = false;

		else if(!LoadRecordHeader(hFile))
			bSuc = false;

		else if(!LoadRecordData(hFile))
			bSuc = false;

		if(hFile != INVALID_HANDLE_VALUE)
			CloseHandle(hFile);
		if(!bSuc)
			::MessageBox(NULL, szFile, "read error", MB_OK);
		return bSuc;
	}

	///////////////////////////////////////////
	//Get Record pointer with index////////////
	///////////////////////////////////////////
	_base_fld* GetRecord(int n){
		if(n < 0 || n >= m_Header.m_nRecordNum)
			return NULL;

		return(_base_fld*)m_psRecord[n];	
	}

	///////////////////////////////////////////
	//Get Record pointer with Code name////////
	///////////////////////////////////////////
	_base_fld* GetRecord(LPCTSTR szRecordCode){
		for(int i = 0; i < m_Header.m_nRecordNum; i++)
		{
			_base_fld* pSet = GetRecord(i);

			if(!strcmp(pSet->m_strCode, szRecordCode))
				return pSet;
		}
		return NULL;
	}

	///////////////////////////////////////////
	//Get Record Number////////////////////////
	///////////////////////////////////////////
	int GetRecordNum(){
		return m_Header.m_nRecordNum;
	}

	bool IsTableOpen(){
		if(!m_psRecord)
			return false;
		return true;
	}


private:
	bool LoadRecordHeader(HANDLE hFile)
	{
		DWORD dwReadByte;
		ReadFile(hFile, &m_Header.m_nRecordNum, sizeof(int), &dwReadByte, NULL);
		ReadFile(hFile, &m_Header.m_nFieldNum, sizeof(int), &dwReadByte, NULL);
		ReadFile(hFile, &m_Header.m_nRecordSize, sizeof(int), &dwReadByte, NULL);

		m_psRecord = new char* [m_Header.m_nRecordNum];
		for(int i = 0; i < m_Header.m_nRecordNum; i++)
			m_psRecord[i] = NULL;

		return true;
	}

	bool LoadRecordData(HANDLE hFile)
	{
		if(m_Header.m_nRecordNum && m_Header.m_nFieldNum && m_Header.m_nRecordSize)
		{
			for(int i = 0; i < m_Header.m_nRecordNum; i++)
			{
				if(m_psRecord[i])
					return false;

				m_psRecord[i] = new char [m_Header.m_nRecordSize];
				DWORD dwReadByte;
				ReadFile(hFile, m_psRecord[i], m_Header.m_nRecordSize, &dwReadByte, NULL);

			}
		}
		return true;
	}
};

#endif