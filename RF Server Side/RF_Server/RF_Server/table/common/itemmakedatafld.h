#ifndef _ItemMakeData_h_
#define _ItemMakeData_h_

#include "BaseFld.h"

struct _ItemMakeData_fld : public _base_fld
{ 
	int			m_nMakeMastery;				//숙련도
	_str_code   m_strCivil;              // 사용가능종족
	enum { max_material_num = 5 };

	struct _material_list{
		_str_code   m_strPdMat;             // 제작재료명ID
		int         m_nPdMatNum;            // 제작필요개수
	};

	_material_list m_listMaterial[max_material_num];

	_ItemMakeData_fld()
	{
		ZeroMemory( this, sizeof( *this ) );
	}
};

#endif
