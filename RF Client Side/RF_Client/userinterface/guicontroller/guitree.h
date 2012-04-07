////////////////////////////////////////////////////////////////////////////
//
// CGUITree Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUITREE_H__
#define __GUITREE_H__

#include "GUIContainer.h"
#include "GUIText.h"

struct NodeInfo 
{
	DWORD		dwID;
	CGUIText	uiName;
	BOOL		bIsOpened;
	CGUIText	uiNodeHeader;

	NodeInfo *	pPrevNode;
	NodeInfo *	pNextNode;

	NodeInfo *	pParentNode;
	NodeInfo *	pChildNode;
};

/*//////////////////////////////////////////////////////////////////////////

[ CGUITree ]

//////////////////////////////////////////////////////////////////////////*/
class CGUITree : public CGUIContainerEx,
				 public IMouseObserver
{
// < Data Member > ---------------------------------------------------------
private:
	NodeInfo *		m_listNode;

// < Member Function > -----------------------------------------------------
public:
	CGUITree();
	virtual ~CGUITree();

	// 노드 삽입
	void	InsertNodeByID( char * pi_pNodeName, DWORD pi_dwNodeID, DWORD pi_dwParentID = -1L, DWORD pi_dwInsertBeforeNodeID = -1L );	

	// 노드 삭제
	void	DeleteNodeByID( DWORD pi_dwNodeID );	
	void	Clear( void );


	void	OpenNode( NodeInfo * pi_pNode, BOOL pi_bOpen );	

	void	MouseEvent( _mouse_event & event );
	void	Show( BOOL pi_bShow );


private:
	NodeInfo * GetNode( DWORD pi_dwFindNodeID, NodeInfo * pi_pNodeList );
	NodeInfo * GetNodeByEventObj( CGUIObject* pi_pEventObj, NodeInfo * pi_pNodeList );	

	int		ResetNode( NodeInfo * pi_pNodeList, int pi_nNodePosX, int pi_nNodePosY );

	void	HideNode( NodeInfo * pi_pNodeList );

	void	DeleteNode( NodeInfo * pi_pNode );
};

#endif // __GUITREE_H__
