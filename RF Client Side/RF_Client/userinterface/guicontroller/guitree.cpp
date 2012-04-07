////////////////////////////////////////////////////////////////////////////
//
// CGUITree Class Implementation
//
////////////////////////////////////////////////////////////////////////////

#include "GUITree.h"

#define INDENTATION		20
#define NODE_HEIGHT		20

CGUITree::CGUITree()
{
	m_listNode = NULL;
}

CGUITree::~CGUITree()
{
	Clear();
}

void
CGUITree::InsertNodeByID( char * pi_pNodeName, DWORD pi_dwNodeID, DWORD pi_dwParentID, DWORD pi_dwInsertBeforeNodeID )
{	
	NodeInfo *	l_pInsertNode = NULL;
	NodeInfo *	l_pParentNode = NULL;

	// set new node
	l_pInsertNode = new NodeInfo;
	l_pInsertNode->dwID = pi_dwNodeID;
	l_pInsertNode->uiName.SetString( pi_pNodeName );

	l_pInsertNode->uiNodeHeader.AddMouseEventHandler( this );
	l_pInsertNode->uiName.AddMouseEventHandler( this );	
	Add( &l_pInsertNode->uiNodeHeader );
	Add( &l_pInsertNode->uiName );

	l_pInsertNode->bIsOpened = TRUE;
	l_pInsertNode->uiNodeHeader.SetString( "   " );


	if( !m_listNode )
	{
		l_pInsertNode->pPrevNode = NULL;
		l_pInsertNode->pNextNode = NULL;
		l_pInsertNode->pChildNode = NULL;
		l_pInsertNode->pParentNode= NULL;

		m_listNode = l_pInsertNode;

		// 첫번째 노드는 위치를 정해준다.		
		l_pInsertNode->uiNodeHeader.SetPos( m_ptPos );
		l_pInsertNode->uiName.SetPos( m_ptPos.x + l_pInsertNode->uiNodeHeader.m_ptSize.x, m_ptPos.y );
	}
	else
	{		
		NodeInfo *	l_pCurNode = NULL;
		NodeInfo *	l_pPrevNode = NULL;

		// search parent node
		if( pi_dwParentID != -1L )
		{
			l_pParentNode = GetNode( pi_dwParentID, m_listNode );
		}				
		
		if( l_pParentNode )
			l_pCurNode = l_pParentNode->pChildNode;
		else
			l_pCurNode = m_listNode;


		while( l_pCurNode )
		{
			if( l_pCurNode->dwID == pi_dwInsertBeforeNodeID )			
				break;
			
			l_pPrevNode = l_pCurNode;

			l_pCurNode = l_pCurNode->pNextNode;
		}
		
		// l_pCurNode앞에 추가한다.
		if( l_pCurNode )
		{
			l_pInsertNode->pPrevNode = l_pCurNode->pPrevNode;
			l_pInsertNode->pNextNode = l_pCurNode;
			l_pInsertNode->pParentNode = l_pParentNode;
			l_pInsertNode->pChildNode = NULL;

			// 맨앞에 추가하는거면 child node를 insertnode로
			if( !l_pCurNode->pPrevNode && l_pParentNode )
				l_pParentNode->pChildNode = l_pInsertNode;
			// 중간에 추가하는거면 전 node의 nextnode를 insertnode로
			else if( l_pCurNode->pPrevNode )
				l_pCurNode->pPrevNode->pNextNode = l_pInsertNode;

			l_pCurNode->pPrevNode = l_pInsertNode;			
		}
		// child node가 하나도 없거나 맨끝에 추가시킬 때
		else 
		{
			l_pInsertNode->pPrevNode = l_pPrevNode;
			l_pInsertNode->pNextNode = NULL;
			l_pInsertNode->pParentNode = l_pParentNode;
			l_pInsertNode->pChildNode = NULL;	
			
			if( l_pPrevNode )
				l_pPrevNode->pNextNode = l_pInsertNode;

			if( !l_pPrevNode && l_pParentNode )
				l_pParentNode->pChildNode = l_pInsertNode;
		}		
	}	

	// node update
	if( l_pParentNode )
	{
		OpenNode( l_pParentNode, TRUE );
	}	
	else
	{
		OpenNode( m_listNode, TRUE );
	}
}

void
CGUITree::DeleteNodeByID( DWORD pi_dwNodeID )
{
	// 해당 노드의 하위 노드까지 다 지운다.
	NodeInfo * l_pNode = GetNode( pi_dwNodeID, m_listNode );

	if( l_pNode )
	{
		// 먼저 child node들만 지운다.
		DeleteNode( l_pNode->pChildNode );		
		
		// prev node와 next node 포인터 세팅
		if( l_pNode->pPrevNode )
		{
			l_pNode->pPrevNode->pNextNode = l_pNode->pNextNode;			
		}
		if( l_pNode->pNextNode )
		{
			l_pNode->pNextNode->pPrevNode = l_pNode->pPrevNode;			
		}

		// set first child node
		if( l_pNode->pParentNode &&
			l_pNode->pParentNode->pChildNode == l_pNode )
		{
			l_pNode->pParentNode->pChildNode = l_pNode->pNextNode;		
		}

		if( l_pNode == m_listNode )
		{
			m_listNode = l_pNode->pNextNode;
		}

		// update node position 
		if( l_pNode->pParentNode )			
			ResetNode( l_pNode->pParentNode, l_pNode->pParentNode->uiNodeHeader.m_ptPos.x, l_pNode->pParentNode->uiNodeHeader.m_ptPos.y );
		else
			ResetNode( m_listNode, m_ptPos.x, m_ptPos.y );

		// delete node
		Remove( &l_pNode->uiName );
		Remove( &l_pNode->uiNodeHeader );
		delete l_pNode;
	}
}

void
CGUITree::DeleteNode( NodeInfo * pi_pNodeList )
{
	// 해당 노드의 하위 노드까지 다 지운다.
	NodeInfo *	l_pCurNode = NULL;	
	NodeInfo *	l_pNextNode = NULL;
	
	l_pCurNode = pi_pNodeList;
	
	while( l_pCurNode )
	{
		DeleteNode( l_pCurNode->pChildNode );		
		
		// backup next node
		l_pNextNode = l_pCurNode->pNextNode;

		// delete current node
		Remove( &l_pCurNode->uiName );
		Remove( &l_pCurNode->uiNodeHeader );

		delete l_pCurNode;

		l_pCurNode = l_pNextNode;
	}
}

void
CGUITree::Clear( void )
{
	if( m_listNode )
	{
		DeleteNode( m_listNode );
		m_listNode = NULL;
	}	
}

NodeInfo * 
CGUITree::GetNode( DWORD pi_dwFindNodeID, NodeInfo * pi_pNodeList )
{
	NodeInfo *	l_pCurNode = NULL;
	NodeInfo *	l_pSearchNode = NULL;
	
	l_pCurNode = pi_pNodeList;
	
	while( l_pCurNode )
	{		
		if( l_pCurNode->dwID == pi_dwFindNodeID )						
			return l_pCurNode;

		l_pSearchNode = GetNode( pi_dwFindNodeID, l_pCurNode->pChildNode );
		if( l_pSearchNode )
			return l_pSearchNode;
		
		l_pCurNode = l_pCurNode->pNextNode;
	}	

	return NULL;
}

NodeInfo *
CGUITree::GetNodeByEventObj( CGUIObject* pi_pEventObj, NodeInfo * pi_pNodeList )
{
	NodeInfo *	l_pCurNode = NULL;
	NodeInfo *	l_pSearchNode = NULL;
	
	l_pCurNode = pi_pNodeList;
	
	while( l_pCurNode )
	{		
		if( &l_pCurNode->uiName == pi_pEventObj ||
			&l_pCurNode->uiNodeHeader == pi_pEventObj )
			return l_pCurNode;

		l_pSearchNode = GetNodeByEventObj( pi_pEventObj, l_pCurNode->pChildNode );
		if( l_pSearchNode )
			return l_pSearchNode;
		
		l_pCurNode = l_pCurNode->pNextNode;
	}	

	return NULL;
}

void
CGUITree::OpenNode( NodeInfo * pi_pNode, BOOL pi_bOpen )
{
	if( !pi_pNode )
		return;

	// update node header
	if( !pi_pNode->pChildNode )
	{
		pi_pNode->uiNodeHeader.SetString( "   " );
		pi_pNode->bIsOpened = TRUE;
	}
	else if( pi_bOpen )		
	{
		pi_pNode->uiNodeHeader.SetString( "[-]" );
		pi_pNode->bIsOpened = TRUE;
	}
	else
	{
		pi_pNode->uiNodeHeader.SetString( "[+]" );
		pi_pNode->bIsOpened = FALSE;
	}

	// pi_pNode의 최상위 parent node에서부터 아래로 다 node 위치를 update한다.
	NodeInfo * l_pParentNode = NULL;
	l_pParentNode = pi_pNode->pParentNode;
	while( l_pParentNode &&
		   l_pParentNode->pParentNode )
	{
		l_pParentNode = l_pParentNode->pParentNode;
	}

	if( l_pParentNode )
	{
		ResetNode( l_pParentNode, l_pParentNode->uiNodeHeader.m_ptPos.x, l_pParentNode->uiNodeHeader.m_ptPos.y );
	}
	else
	{
		ResetNode( pi_pNode, pi_pNode->uiNodeHeader.m_ptPos.x, pi_pNode->uiNodeHeader.m_ptPos.y );
	}	
}

int
CGUITree::ResetNode( NodeInfo * pi_pNodeList, int pi_nNodePosX, int pi_nNodePosY )
{
	// child node와 next node들의 위치를 update한다.
	NodeInfo *	l_pCurNode = NULL;
	
	l_pCurNode = pi_pNodeList;

	int l_nNodePosY = 0;	
	
	while( l_pCurNode )
	{		
		
		if( !l_pCurNode->pParentNode ||
			( l_pCurNode->pParentNode && l_pCurNode->pParentNode->bIsOpened ) )
		{
			l_pCurNode->uiNodeHeader.Show( TRUE );
			l_pCurNode->uiName.Show( TRUE );

			l_pCurNode->uiNodeHeader.SetPos( pi_nNodePosX, pi_nNodePosY + l_nNodePosY );
			l_pCurNode->uiName.SetPos( l_pCurNode->uiNodeHeader.m_ptPos.x + l_pCurNode->uiNodeHeader.m_ptSize.x,
									   l_pCurNode->uiNodeHeader.m_ptPos.y );

			// 일단 size를 정해준다.
			// resize width
//			if( m_ptSize.x < l_pCurNode->uiName.m_ptPos.x + l_pCurNode->uiName.m_ptSize.x - m_ptPos.x )		
//				m_ptSize.x = l_pCurNode->uiName.m_ptPos.x + l_pCurNode->uiName.m_ptSize.x - m_ptPos.x;

			l_nNodePosY += NODE_HEIGHT;

			l_nNodePosY += ResetNode( l_pCurNode->pChildNode, pi_nNodePosX + INDENTATION, pi_nNodePosY + l_nNodePosY );
		}
		else
		{
			l_pCurNode->uiNodeHeader.Show( FALSE );
			l_pCurNode->uiName.Show( FALSE );

			HideNode( l_pCurNode->pChildNode );
		}						

		l_pCurNode = l_pCurNode->pNextNode;
	}

	// resize height
//	if( m_ptSize.y < pi_nNodePosY + l_nNodePosY - m_ptPos.y )
//		m_ptSize.y = pi_nNodePosY + l_nNodePosY - m_ptPos.y;

	return l_nNodePosY;
}

void
CGUITree::HideNode( NodeInfo * pi_pNodeList )
{
	// child node와 next node들 모두 숨긴다.	
	NodeInfo *	l_pCurNode = NULL;
	
	l_pCurNode = pi_pNodeList;

	while( l_pCurNode )
	{
		l_pCurNode->uiNodeHeader.Show( FALSE );
		l_pCurNode->uiName.Show( FALSE );

		HideNode( l_pCurNode->pChildNode );

		l_pCurNode = l_pCurNode->pNextNode;
	}
}

void
CGUITree::MouseEvent( _mouse_event & event )
{
	if( event.id == MOUSE_CLICKED )
	{
		NodeInfo * l_pNode = GetNodeByEventObj( event.source, m_listNode );
		if( l_pNode )
		{			
			OpenNode( l_pNode, !l_pNode->bIsOpened );
		}
	}
}

void
CGUITree::Show( BOOL pi_bShow )
{
	// node들은 그대로 둔다.
	CGUIObject::Show( pi_bShow );
}