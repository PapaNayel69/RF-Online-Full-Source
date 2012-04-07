#ifndef _ExpFld_h_
#define _ExpFld_h_

#include "BaseFld.h"

struct _exp_fld : public _base_fld
{
	_str_code   m_strBellato;            // 벨라토
	_str_code   m_strCora;               // 코라
	_str_code   m_strAccretia;           // 아크레시아

	_exp_fld()
	{
		ZeroMemory( this, sizeof( *this ) );
	}
};

#endif