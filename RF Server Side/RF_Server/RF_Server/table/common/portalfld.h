#ifndef _PortalFld_h_
#define _PortalFld_h_

#include "BaseFld.h"

struct _portal_fld : public _base_fld
{
	_str_code   m_strLinkMapCode;           // 연결맵
	_str_code   m_strLinkPortalCode;        // 연결더미
	_str_code   m_szMenu;					// 이동할_맵
	int         m_nKind;					// 사용가능더미
	_str_code   m_strUseRace;				// 사용가능종족


	_portal_fld()
	{
		ZeroMemory(this, sizeof(*this));
	}
};

#endif