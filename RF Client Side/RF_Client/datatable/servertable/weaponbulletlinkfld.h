#ifndef _WeaponBulletLinkFld_h_
#define _WeaponBulletLinkFld_h_

#include "BaseFld.h"

struct _weapon_bullet_link_fld : public _base_fld
{	
	enum { max_list = 15 };

	int m_nUsingWeaponNum;
	_str_code m_strUsingWeaponCode[max_list];

	_weapon_bullet_link_fld()
	{
		ZeroMemory(this, sizeof( *this ));
	}
};

#endif