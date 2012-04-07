#ifndef _pt_logspy_control_h_
#define _pt_logspy_control_h_

#include "CharacterDB.h"

#pragma pack(push, 1)

#define history_filecount_request_cols 1
struct _history_filecount_request_cols {

	DWORD	dwConCenterSerial;
	DWORD	dwSerial;
	WORD	wYear;
	BYTE	byMonth;
	BYTE	byDay;
	
	int size() { return sizeof(*this);	}
};

#define max_history_file_count 64

#define history_filecount_result_lsco 2
struct _history_filecount_result_lsco {

	DWORD	dwConCenterSerial;
	DWORD	dwSerial;
	WORD	wFileCount;

	struct __history_file_info
	{
		WORD	wYear;
		BYTE	byMonth;
		BYTE	byDay;
		BYTE	byHour;
		char	szFileName[16];
	};

	__history_file_info HistoryFile[max_history_file_count];

	int size() { return sizeof(*this)-sizeof(__history_file_info)*(max_history_file_count-wFileCount);}
};

#define history_fileget_request_cols 3
struct _history_fileget_request_cols {

	DWORD	dwConCenterSerial;
	WORD	wYear;
	BYTE	byMonth;
	BYTE	byDay;
	BYTE	byHour;
	char	szFileName[16];

	int size() { return sizeof(*this);	}
};

#define max_hitoryfile_length 512

#define history_fileget_result_lsco 4
struct _history_fileget_result_lsco {

	DWORD	dwConCenterSerial;
	DWORD	dwFileBodyLength;
	char	szFileBody[max_hitoryfile_length];
	
	int size() { return sizeof(*this)-sizeof(char)*(max_hitoryfile_length-dwFileBodyLength);}
};

#define history_filemore_request_cols 5
struct _history_filemore_request_cols {

	DWORD	dwConCenterSerial;

	int size() { return sizeof(*this);	}

};


#pragma pack(pop)

#endif
