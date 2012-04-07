#ifndef _OreCutting_h_
#define _OreCutting_h_

#include "BaseFld.h"

struct _OreCutting_fld : public _base_fld
{ 
	_str_code   m_strResource_Item;      // 자원이름
	int         m_nCutting_probability;  // 가공확률

	_OreCutting_fld()
	{
		ZeroMemory( this, sizeof( *this ) );
	}
};

#endif
