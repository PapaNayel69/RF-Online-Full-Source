////////////////////////////////////////////////////////////////////////////
//
// CGUISkillWindow Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUIFORCEWINDOW_H__
#define __GUIFORCEWINDOW_H__

#include "../../GUIController/GUIWindow.h"
#include "../../GUIController/GUIButton.h"
#include "../../GUIController/GUIScrollbar.h"
#include "../../GUIController/GUIItem.h"
#include "../../GUIController/GUIText.h"
#include "GUISkillWindow.h"

#define	FORCE_MODE_NUM		5

/*//////////////////////////////////////////////////////////////////////////

[ CGUIForceWindow ]

//////////////////////////////////////////////////////////////////////////*/
class CGUIForceWindow : public CGUISFWindow
{
public:
	enum ForceMode {
		DarknHoly,
		Fire,
		Aqua,
		Terra,				
		Wind,		
		None
	};

// < Data Member > ---------------------------------------------------------
private:
		

// < Member Function > ----------------------------------------------------
public:
	CGUIForceWindow();
	virtual ~CGUIForceWindow();

	void	Init( void );
	void	SetSprite( void );		

	void	SetRaceType( BYTE pi_byRaceType );	
};

#endif // __GUIFORCEWINDOW_H__
