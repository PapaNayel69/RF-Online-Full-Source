#ifndef _MonBlockFld_h_
#define _MonBlockFld_h_

#include "BaseFld.h"

struct _mon_block_fld : public _base_fld
{
	struct _dummy_info{
		_str_code m_strDummyCode;
		DWORD m_dwSelectProp;
	};

	enum { _max_list = 20 };

	DWORD m_dwDummyNum;
	_dummy_info m_DummyInfo[_max_list];

	_mon_block_fld()
	{
		ZeroMemory(this, sizeof(*this));
	}
};

struct _mon_active_fld : public _base_fld
{
	DWORD m_dwRegenTime;
	DWORD m_dwRegenLimNum;
	DWORD m_dwRegenProp;

	DWORD m_dwRegenMinNum;
	DWORD m_dwStdKill;	
	DWORD m_dwRegenMaxNum;

	_mon_active_fld()
	{
		ZeroMemory(this, sizeof(*this));
	}
};

#endif