////////////////////////////////////////////////////////////////////////////
//
// CGUIContainer Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUICONTAINER_H__
#define __GUICONTAINER_H__

#pragma warning( disable : 4786 )

#include "GUIObject.h"
#include "R3Input.h"
#include <list>
#include <algorithm>

using namespace std ;

/*//////////////////////////////////////////////////////////////////////////

[ CGUIContainer ]

  - CGUIContainer는 등록된 CGUIObject들을 전반적으로 관리하는 역할을 한다.
    draw, input message 처리 등을 CGUIContainer에서 관리하게 되면
	CGUIContainer는 하나의 CGUIObject처럼 처리된다.

//////////////////////////////////////////////////////////////////////////*/
class CGUIContainer : public CGUIObject
{
friend class CGUIScreen;
public:
	typedef list< CGUIObject * >		CGUIObjectList;

// < Data Member > ---------------------------------------------------------
protected:
	CGUIObjectList		m_listObject;

// < Member Function > -----------------------------------------------------
public:
			CGUIContainer();
			~CGUIContainer();
	
	virtual	void	Add( CGUIObject * pi_pObj );
	virtual	int		Add( CGUIObject * pi_pObj, WORD pi_wAddIndex );
	virtual	BOOL	Remove( CGUIObject * pi_pObj );
	virtual	void	RemoveAll( void );

			int		GetAddedObjectNum( void ) { return m_listObject.size(); }
			BOOL	IsAddedObject( CGUIObject * pi_pObj );			

	CGUIObject	*	GetAddedObject( WORD pi_wIndex );
			int		GetAddedObjectIndex( CGUIObject * pi_pObj );			

	virtual	void	SetPos( POINT & pi_ptPos );
	virtual	void	SetPos( LONG pi_nPosX, LONG pi_nPosY );
	virtual	void	SetBound( RECT & pi_bound );

			// 기본적으로 모든 msg는 container를 걷쳐 포함된 각 object에 보내지게 된다.
	virtual	BOOL	InputProc( void );
	
	virtual	LRESULT	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );			

	virtual	void	Active( void );
	virtual	void	InActive( void );
			
	virtual	void	Show( BOOL pi_bShow );
			
	virtual	BOOL	Draw( void );
	virtual	BOOL	Draw( RECT & pi_rtClipping );

	virtual	void	Update( void );

			void	SetFocus( void ) {;}

	virtual	void	Enable( void );
	virtual	void	Disable( void );

	virtual	void	EnableFocus( void );
	virtual	void	DisableFocus( void );

	virtual	void	SetSprite();
};

/*//////////////////////////////////////////////////////////////////////////

[ CGUIContainerEx ]

/*//////////////////////////////////////////////////////////////////////////
class CGUIContainerEx : public CGUIContainer
{
// < Member Function > -----------------------------------------------------
public:
			CGUIContainerEx();
			~CGUIContainerEx();

	virtual	BOOL	Draw( void );
};

#endif // __GUICONTAINER_H__
