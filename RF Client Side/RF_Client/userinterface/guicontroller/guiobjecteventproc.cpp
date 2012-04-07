#include "GUIObject.h"
#include "GUIWindow.h"

void
CGUIObject::CreateEventQueue( WORD pi_wSize/*=10*/ )
{
	if( pi_wSize == 0 )
		s_qEventList.Create( 10 );
	else
		s_qEventList.Create( pi_wSize );
}

void
CGUIObject::SendEvent( _event_queue_data & pi_EventInfo )
{
	// 중복 체크
	for( int i=0; i<s_qEventList.GetCurNum(); ++i )
	{
		if( s_qEventList[i].pSource == pi_EventInfo.pSource &&
			s_qEventList[i].wEventID == pi_EventInfo.wEventID )
			return;		
	}

	s_qEventList.Push( pi_EventInfo );
}

void
CGUIObject::RemoveEvent( CGUIObject * pi_pEventObj )
{
	// event source이거나 eventhandler이면 queue에서 삭제한다.
	for( int i=0; i<s_qEventList.GetCurNum(); ++i )
	{			
		if( s_qEventList[i].pSource == pi_pEventObj ||
			s_qEventList[i].pEventHandler == pi_pEventObj )			
		{			
/*			// test
			if( s_qEventList[i].pEventHandler == pi_pEventObj )
			{
				int temp;
				temp = 100;
			}
*/
			s_qEventList.Remove( i );			
		}
	}
}

void
CGUIObject::EventProcess( void )
{
	_event_queue_data * l_pEvent;	

	for( int i=0; i < s_qEventList.GetCurNum(); ++i )
	{
		l_pEvent = &s_qEventList[i];
		if( l_pEvent->wEventID < 10 )
		{
			// mouse event
			_mouse_event event;
			event.id = l_pEvent->wEventID;
			event.source = l_pEvent->pSource;
			event.xPos = l_pEvent->nPosX;
			event.yPos = l_pEvent->nPosY;

			IMouseObserver * l_pOsr = (IMouseObserver *)l_pEvent->pEventHandler;

			l_pOsr->MouseEvent( event );
		}
		else if( l_pEvent->wEventID < 20 )
		{
			// mouse motion event
			_mouse_motion_event event;
			event.id = l_pEvent->wEventID;
			event.source = l_pEvent->pSource;
			event.xPos = l_pEvent->nPosX;
			event.yPos = l_pEvent->nPosY;
			event.xMove = l_pEvent->nMoveX;
			event.yMove = l_pEvent->nMoveY;

			IMouseMotionObserver * l_pOsr = (IMouseMotionObserver *)l_pEvent->pEventHandler;

			l_pOsr->MouseMotionEvent( event );				
		}
		else if( l_pEvent->wEventID < 30 )
		{
			// key event
			_key_event event;
			event.id = l_pEvent->wEventID;
			event.source = l_pEvent->pSource;
			event.key = l_pEvent->byKeyCode;

			IKeyObserver * l_pOsr = (IKeyObserver *)l_pEvent->pEventHandler;

			l_pOsr->KeyEvent( event );
		}
		else if( l_pEvent->wEventID < 40 )
		{
			_scroll_event event;
			event.id = l_pEvent->wEventID;
			event.source = l_pEvent->pSource; 
			IScrollObserver * l_pOsr = (IScrollObserver *)l_pEvent->pEventHandler;

			l_pOsr->ScrollEvent( event );	
		}
		else if( l_pEvent->wEventID < 50 )
		{
			_select_event event;
			event.id = l_pEvent->wEventID;
			event.source = l_pEvent->pSource;
			event.select = l_pEvent->wSelectIndex;			

			ISelectItemObserver * l_pOsr = (ISelectItemObserver *)l_pEvent->pEventHandler;

			l_pOsr->SelectItemEvent( event );
		}
		else if( l_pEvent->wEventID < 60 )
		{
			CGUIWindow	*	l_pWindow;
			l_pWindow = (CGUIWindow *)l_pEvent->pEventHandler;			

			if( l_pEvent->wEventID == DLG_BOX_CLOSED )
			{
				l_pWindow->ClosingDialogBox( (CGUIDialogBox *)l_pEvent->pSource );
			}
		}
		else if( l_pEvent->wEventID < 70 )
		{
			CGUIWindow	*	l_pWindow;
			l_pWindow = (CGUIWindow *)l_pEvent->pEventHandler;			

			if( l_pEvent->wEventID == WINDOW_OPENED )
			{
				l_pWindow->Open();
			}
			else if( l_pEvent->wEventID == WINDOW_CLOSED )
			{
				l_pWindow->Close();
			}
		}
	}

	s_qEventList.Clear();
}

void
CGUIObject::ClearMouseEventBuffer( void )
{	
	s_pMouseDetectedObject = NULL;
}