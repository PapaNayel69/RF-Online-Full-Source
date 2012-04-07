////////////////////////////////////////////////////////////////////////////
//
// CGUIMapLoadingWindow Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUIMAPLOADINGWINDOW_H__
#define __GUIMAPLOADINGWINDOW_H__

#include "../../GUIController/GUIWindow.h"

/*//////////////////////////////////////////////////////////////////////////

[ CGUIMapLoadingWindow ]   

  - 맵 이동할 때 로딩 화면

//////////////////////////////////////////////////////////////////////////*/
class CGUIMapLoadingWindow : public CGUIWindow
{
// < Data Member > ---------------------------------------------------------
private:
	CGUIObject		m_uiSrcMapName;
	CGUIObject		m_uiDestMapName;

	POINT			m_ptDestMapNameOrgSize;
	POINT			m_ptDestMapNameOrgPos;
	DWORD			m_dwAniTime;

	BOOL			m_bCreated;	

// < Member Function > -----------------------------------------------------
public:
	CGUIMapLoadingWindow();
	virtual ~CGUIMapLoadingWindow();

	void	Init( void );
	void	SetSprite( void );

	void	Open( void );
	void	Close( void );

	// 현재 있는 맵과 이동할 맵을 설정한다.
	void	SetMapMove( BYTE pi_bySrcMapID, BYTE pi_byDestMapID );

	void	Update( void );
};

#endif // __GUIMAPLOADINGWINDOW_H__
