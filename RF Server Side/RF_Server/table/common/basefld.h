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

	enum	{	max_row = 10000 };

	bool	m_bLoad;
	DWORD	m_dwTotalSize;
	_record_bin_header m_Header;
	char*	m_psRecord[max_row];
	
	CRecordData(){
		m_bLoad = false;
		m_dwTotalSize = 0;
		memset(m_psRecord, NULL, sizeof(m_psRecord));
	}

	virtual ~CRecordData(){		
		for(int i = 0; i < max_row; i++)
		{
			if(m_psRecord[i])
			{
				delete [] m_psRecord[i];
				m_psRecord[i] = NULL;
			}
		}
	}

	///////////////////////////////////////////
	//Record binary file Load//////////////////
	///////////////////////////////////////////
	bool ReadRecord(char* szFile, DWORD dwStructSize)
	{
		bool bSuc = true;

		HANDLE hFile = ::CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
			bSuc = false;

		else if(!LoadRecordHeader(hFile))
			bSuc = false;

		else if(!LoadRecordData(hFile))
			bSuc = false;

		if(hFile != INVALID_HANDLE_VALUE)
			::CloseHandle(hFile);
		if(!bSuc)
		{
			char szMsg[128];
			sprintf(szMsg, "%s파일의 읽기 실패", szFile);
			::MessageBox(NULL, szMsg, "CRecordData", MB_OK);
			return false;
		}

		DWORD dwFileSize = FileSize(szFile);
		if(m_dwTotalSize != dwFileSize)
		{
			char szMsg[128];
			sprintf(szMsg, "%s파일의 헤더(%d)랑 실제 사이즈(%d)가 다르다", szFile, m_dwTotalSize, dwFileSize);
			::MessageBox(NULL, szMsg, "CRecordData", MB_OK);
			return false;
		}

		if(dwStructSize != (DWORD)m_Header.m_nRecordSize)
		{
			char szMsg[128];
			sprintf(szMsg, "%s파일의 헤더(%d)랑 구조체 사이즈(%d)가 다르다", szFile, m_Header.m_nRecordSize, dwStructSize);
		//	::MessageBox(NULL, szMsg, "CRecordData", MB_OK);
		//	return false;
		}

		m_bLoad = true;

		return true;
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
/*
	///////////////////////////////////////////
	//Get Record pointer with Code name////////
	///////////////////////////////////////////
	_base_fld* GetRecordAllCase(LPCTSTR szRecordCode){
		_str_code szParam;
		char* pszUpper = strdup(szRecordCode);
		strcpy(szParam, pszUpper);

		for(int i = 0; i < m_Header.m_nRecordNum; i++)
		{
			_base_fld* pSet = GetRecord(i);

			if(!strcmp(strdup(pSet->m_strCode), szParam))
				return pSet;
		}
		return NULL;
	}*/

	///////////////////////////////////////////
	//Get Record Number////////////////////////
	///////////////////////////////////////////
	int GetRecordNum(){
		return m_Header.m_nRecordNum;
	}

	bool IsTableOpen(){
		return m_bLoad;
	}

	DWORD FileSize(char* szFile)		//파일 사이즈를 알아낸다.
	{
		HANDLE hFile = ::CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
			return 0;

		DWORD dwStart = ::SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
		DWORD dwEnd = ::SetFilePointer(hFile, 0, NULL, FILE_END);

		::CloseHandle(hFile);

		return dwEnd-dwStart;
	}

private:
	bool LoadRecordHeader(HANDLE hFile)
	{
		DWORD dwReadByte;
		::ReadFile(hFile, &m_Header.m_nRecordNum, sizeof(int), &dwReadByte, NULL);
		::ReadFile(hFile, &m_Header.m_nFieldNum, sizeof(int), &dwReadByte, NULL);
		::ReadFile(hFile, &m_Header.m_nRecordSize, sizeof(int), &dwReadByte, NULL);

		m_dwTotalSize = sizeof(m_Header) + m_Header.m_nRecordNum * m_Header.m_nRecordSize;

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
				::ReadFile(hFile, m_psRecord[i], m_Header.m_nRecordSize, &dwReadByte, NULL);

			}
		}
		return true;
	}
};

#endif