////////////////////////////////////////////////////////////////////////////
//
// CGUIDebuggingWin Class Implementation
//
////////////////////////////////////////////////////////////////////////////
#include "GUIDebuggingWin.h"

CGUIDebuggingWin::CGUIDebuggingWin()
{
	m_cDebugInfoTree.SetSize( 200, 400 );
	SetSize( m_cDebugInfoTree.m_ptSize );

	Add( &m_cDebugInfoTree );

	SetARGB( 0x70452233 );

	SetMoveObject( &m_cDebugInfoTree );
	SetMovable( TRUE );
}

CGUIDebuggingWin::~CGUIDebuggingWin()
{

}
