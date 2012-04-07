// rfZonedatabase.h: interface for the CRFZoneDatabase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RFZONEDATABASE_H__608792C3_9F51_461E_9122_ECDA99F89671__INCLUDED_)
#define AFX_RFZONEDATABASE_H__608792C3_9F51_461E_9122_ECDA99F89671__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "rfnewdatabase.h"
#include "characterdb.h"


/// ControlServer DB Class
class CRFZoneDatabase   : public CRFNewDatabase
{
public:
	CRFZoneDatabase();
	virtual ~CRFZoneDatabase();

	bool	Select_RegedCharacterInfo( DWORD dwAccountSerial, _REGED* OUT pRegedList );
	

};

#endif // !defined(AFX_RFZONEDATABASE_H__608792C3_9F51_461E_9122_ECDA99F89671__INCLUDED_)
