////////////////////////////////////////////////////////////////////////////
//
// CGUIDebuggingWin Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUIDEBUGGINGWIN_H__
#define __GUIDEBUGGINGWIN_H__

#include "GUIController/GUIWindow.h"
#include "GUIController/GUITree.h"

/*//////////////////////////////////////////////////////////////////////////

[ CGUIDebuggingWin ]

//////////////////////////////////////////////////////////////////////////*/
class CGUIDebuggingWin : public CGUIWindow
{
// < Data Member > ---------------------------------------------------------
private:
	CGUITree	m_cDebugInfoTree;

// < Member Function > -----------------------------------------------------
public:
	CGUIDebuggingWin();
	virtual ~CGUIDebuggingWin();

	CGUITree * GetDebugInfoTree() { return &m_cDebugInfoTree; }
};

#endif // __GUIDEBUGGINGWIN_H__
