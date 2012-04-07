#ifndef _MapFld_h_
#define _MapFld_h_

#include "BaseFld.h"

struct _map_fld : public _base_fld
{
	_str_code	m_strFileName;
	int			m_nMapType;
	int			m_nLayerNum;
	int			m_nRaceVillageCode;
	int			m_nMonsterSetFileNum;
	_str_code	m_strServerCode;
	int			m_nProperUserNum;
	bool		m_bSizeAdjust;
	int			m_nMinX, m_nMinY, m_nMaxX, m_nMaxY;

	_map_fld()
	{
		ZeroMemory(this, sizeof(*this));
	}
};

#endif