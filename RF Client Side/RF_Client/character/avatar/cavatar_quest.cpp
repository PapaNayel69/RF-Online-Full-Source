////////////////////////////////////////////////////////////////////////////////
//
// Quest Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../../GameObject/Manager/CCharacterMgr.h"
#include "../../GameObject/Board/Common/CBoardManager.h"
#include "CAvatar_Quest.h"

#include "../../CCamera.h"
#include "../../GameProgress/CGP_MainGame.h"

#include "../../Network/NetMsgProcessor/Combat/CNetMsgProcessor_Combat.h"

#include "../../DataFile.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Structure          //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

QBI_BASE_DATA::QBI_BASE_DATA()
{
	Init();
}

QBI_BASE_DATA::~QBI_BASE_DATA()
{
}

void
QBI_BASE_DATA::Init( void )
{
	m_wID		= ID_INVALID;
	m_byFlag	= 0;
}

//------------------------------------------------------------------------------

QBI_MESSAGE_DATA::QBI_MESSAGE_DATA()
{
	Init();
}

QBI_MESSAGE_DATA::~QBI_MESSAGE_DATA()
{
	//delete [] m_pMessage;
	delete m_pMessage;
}

void
QBI_MESSAGE_DATA::Init( void )
{
	m_pMessage = NULL;
}
//============================================================================//
//                                  Boundary                                  //
//============================================================================//

QUEST::QUEST()
{
	Init();
}

QUEST::~QUEST()
{
	for( int i = 0; i < m_wTotalQBINum; ++i )
	{
		if( m_listQBI[i] != NULL )
			delete m_listQBI[i];
	}

	delete [] m_listQBI;
}

void
QUEST::Init( void )
{
	m_dwID			= ID_INVALID;
	m_byFlag		= 0;

	m_wCurQBINum	= 0;
	m_wTotalQBINum	= 0;

	m_listQBI		= NULL;
}

QBI_BASE_DATA *
QUEST::GetQBI( WORD pi_wID )
{
	if( pi_wID == 0xFFFF )
		return m_listQBI[m_wCurQBINum];

	if( pi_wID >= m_wTotalQBINum )
		return NULL;

	return m_listQBI[pi_wID];
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

QUEST_INFO::QUEST_INFO()
{
	Init();
}

QUEST_INFO::~QUEST_INFO()
{
	Destroy();
}

void
QUEST_INFO::Init( void )
{
	ZeroMemory( this, sizeof( QUEST_INFO ) );

	QSV.m_byInputButton	= QBI_WAIT_DATA::WAIT_INPUT_NONE;

	QSV.m_byAttackType	= ID_INVALID;
	QSV.m_dwSFIndex		= ID_INVALID;
	QSV.m_dwTarget[0]	= ID_INVALID;
	QSV.m_dwTarget[1]	= ID_INVALID;

	QSV.m_pCurDisplayMsg = NULL;
}

void
QUEST_INFO::Destroy( void )
{
	for( int i = 0; i < m_dwTotalQuestNum; ++i )
	{
		if( m_listQuest[i] != NULL )
			delete m_listQuest[i];
	}

	delete [] m_listQuest;

	Init();
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

QUEST *
QUEST_INFO::GetQuest( DWORD pi_dwID )
{
	if( m_dwTotalQuestNum <= 0 )
		return NULL;

	if( pi_dwID == ID_INVALID )
		return m_listQuest[m_dwCurQuestNum];

	if( pi_dwID >= m_dwTotalQuestNum )
		return NULL;

	return m_listQuest[pi_dwID];
}

BOOL
QUEST_INFO::RefreshQuest( DWORD pi_dwID )
{
	QUEST * l_pQuest;

	if( m_dwTotalQuestNum <= 0 )
		return FALSE;

	if( pi_dwID == ID_INVALID )
	{
		l_pQuest = m_listQuest[m_dwCurQuestNum];
	}
	else
	{
		if( pi_dwID >= m_dwTotalQuestNum )
			return FALSE;
		l_pQuest = m_listQuest[pi_dwID];
	}

	if( l_pQuest )
	{
		l_pQuest->m_byFlag = QBIF_BEGIN;
		l_pQuest->m_wCurQBINum = 0;

		for( int i = 0; i < l_pQuest->m_wTotalQBINum; ++i )
		{
			if( l_pQuest->m_listQBI[i] )
				l_pQuest->m_listQBI[i]->m_byFlag = 0;
		}
	}

	return TRUE;
}

QBI_BASE_DATA *
LoadQuestInfoFile_QBI( FILE * pi_pFile )
{
	if( !pi_pFile )
		return FALSE;

	QBI_BASE_DATA * l_pQBI = NULL;
	BYTE			l_byType;
	//WORD			l_wLength;
	fread( &l_byType, 1, 1, pi_pFile );

	switch( l_byType )
	{
		case QBI_MESSAGE				:
			l_pQBI = new QBI_MESSAGE_DATA;
			fread( &l_pQBI->m_byType, sizeof( BYTE ), 1, pi_pFile );
			fread( &l_pQBI->m_wID, sizeof( WORD ), 1, pi_pFile );
			{
				QBI_MESSAGE_DATA * l_pMsgQBI = ( QBI_MESSAGE_DATA * )l_pQBI;
				l_pMsgQBI->m_pMessage = new HSD_HINT;
				fread( &l_pMsgQBI->m_pMessage->m_dwID, sizeof( DWORD ), 1, pi_pFile );
				fread( &l_pMsgQBI->m_pMessage->m_byLevelMin, sizeof( BYTE ), 1, pi_pFile );
				fread( &l_pMsgQBI->m_pMessage->m_byLevelMax, sizeof( BYTE ), 1, pi_pFile );
				fread( &l_pMsgQBI->m_pMessage->m_byMapNo, sizeof( BYTE ), 1, pi_pFile );
				fread( &l_pMsgQBI->m_pMessage->m_dwActionID, sizeof( DWORD ), 1, pi_pFile );
				fread( &l_pMsgQBI->m_pMessage->m_dwTime, sizeof( DWORD ), 1, pi_pFile );
				fread( &l_pMsgQBI->m_pMessage->m_bIsDispaly, sizeof( BOOL ), 1, pi_pFile );

				fread( &l_pMsgQBI->m_pMessage->m_byTotalStringNum, sizeof( BYTE ), 1, pi_pFile );
				l_pMsgQBI->m_pMessage->m_listString = new HSD_STRING[l_pMsgQBI->m_pMessage->m_byTotalStringNum];
				for( int j = 0; j < l_pMsgQBI->m_pMessage->m_byTotalStringNum; ++j )
				{
					fread( &l_pMsgQBI->m_pMessage->m_listString[j].m_stFont, sizeof( HSD_FONT ), 1, pi_pFile );
					fread( &l_pMsgQBI->m_pMessage->m_listString[j].m_wLength, sizeof( WORD ), 1, pi_pFile );
					l_pMsgQBI->m_pMessage->m_listString[j].m_pString = new char[l_pMsgQBI->m_pMessage->m_listString[j].m_wLength+1];
					ZeroMemory( l_pMsgQBI->m_pMessage->m_listString[j].m_pString, l_pMsgQBI->m_pMessage->m_listString[j].m_wLength+1 );
					fread( l_pMsgQBI->m_pMessage->m_listString[j].m_pString, l_pMsgQBI->m_pMessage->m_listString[j].m_wLength, 1, pi_pFile );
				}
			}
			/*
			fread( &l_wLength, sizeof( WORD ), 1, pi_pFile );
			( ( QBI_MESSAGE_DATA * )l_pQBI )->m_pMessage = new char[l_wLength+1];
			ZeroMemory( ( ( QBI_MESSAGE_DATA * )l_pQBI )->m_pMessage, l_wLength+1 );
			fread( ( ( QBI_MESSAGE_DATA * )l_pQBI )->m_pMessage, l_wLength, 1, pi_pFile );
			*/
			break;
		case QBI_WAITING_FOR			:
			l_pQBI = new QBI_WAIT_DATA;
			fread( l_pQBI, sizeof( QBI_WAIT_DATA ), 1, pi_pFile );
			break;
		case QBI_MOVE					:
		case QBI_MOVE_RQ				:
			l_pQBI = new QBI_MOVE_DATA;
			fread( l_pQBI, sizeof( QBI_MOVE_DATA ), 1, pi_pFile );
			break;
		case QBI_SET_POSITION			:
		case QBI_SET_ROTATION			:
		case QBI_SET_CAM_ROTATION		:
		case QBI_CAM_ROTATE				:
		case QBI_CAM_ROTATE_RQ			:
			l_pQBI = new QBI_POSITION_DATA;
			fread( l_pQBI, sizeof( QBI_POSITION_DATA ), 1, pi_pFile );
			break;

		case QBI_OPEN_WINDOW			:
		case QBI_OPEN_WINDOW_RQ			:
		case QBI_CLOSE_WINDOW			:
		case QBI_CLOSE_WINDOW_RQ		:
			l_pQBI = new QBI_UI_DATA;
			fread( l_pQBI, sizeof( QBI_UI_DATA ), 1, pi_pFile );
			break;

		case QBI_PUT_ON_EQUIPMENT		:
		case QBI_PUT_ON_EQUIPMENT_RQ	:
		case QBI_PUT_OFF_EQUIPMENT		:
		case QBI_PUT_OFF_EQUIPMENT_RQ	:
			l_pQBI = new QBI_UI_EQUIP_DATA;
			fread( l_pQBI, sizeof( QBI_UI_EQUIP_DATA ), 1, pi_pFile );
			break;

		case QBI_SET_CURSOR_POS1		:
		case QBI_SET_CURSOR_POS2		:
		case QBI_SET_CURSOR_POS3		:
		case QBI_SET_CURSOR_POS2UI		:
		case QBI_MOVE_CURSOR1			:
		case QBI_MOVE_CURSOR2			:
		case QBI_MOVE_CURSOR3			:
		case QBI_MOVE_CURSOR2UI			:
		case QBI_MOUSE_CLICK			:
			l_pQBI = new QBI_CURSOR_DATA;
			fread( l_pQBI, sizeof( QBI_CURSOR_DATA ), 1, pi_pFile );
			break;

		case QBI_COMBAT					:
		case QBI_COMBAT_RQ				:
			l_pQBI = new QBI_COMBAT_DATA;
			fread( l_pQBI, sizeof( QBI_COMBAT_DATA ), 1, pi_pFile );
			break;
	}

	return l_pQBI;
}

BOOL
QUEST_INFO::LoadQuestInfoFile( char * pi_pFileName )
{
	if( !pi_pFileName )
		return FALSE;

	FILE * l_pFile = fopen( pi_pFileName, "rb" );
	if( !l_pFile )
		return FALSE;

	//--------------------------------------------------------------------------

	fread( &m_dwTotalQuestNum, sizeof( DWORD ), 1, l_pFile );

	m_listQuest = new QUEST*[m_dwTotalQuestNum];

	for( int i = 0; i < m_dwTotalQuestNum; ++i )
	{
		m_listQuest[i] = new QUEST;

		fread( &m_listQuest[i]->m_dwID, sizeof( DWORD ), 1, l_pFile );
		fread( m_listQuest[i]->m_pName, 64, 1, l_pFile );

		fread( &m_listQuest[i]->m_wTotalQBINum, sizeof( WORD ), 1, l_pFile );
		m_listQuest[i]->m_listQBI = new QBI_BASE_DATA*[m_listQuest[i]->m_wTotalQBINum];
		for( int j = 0; j < m_listQuest[i]->m_wTotalQBINum; ++j )
		{
			m_listQuest[i]->m_listQBI[j] = LoadQuestInfoFile_QBI( l_pFile );
		}
	}

	fclose( l_pFile );

	return TRUE;
}

BOOL
QUEST_INFO::UnLoadQuestInfoFile( void )
{
	Destroy();

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Structure          //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
QUEST_INFO::ProcessQuest( void )
{
	if( m_dwTotalQuestNum <= 0 )
		return;

	QUEST *	l_pQuest = GetQuest();
	if( !l_pQuest )
		return;

	if( ( l_pQuest->m_byFlag & QBIF_BEGIN ) && !( l_pQuest->m_byFlag & QBIF_FINISH ) )
	{
		if( l_pQuest->m_wCurQBINum >= l_pQuest->m_wTotalQBINum )
			return;

		QBI_BASE_DATA * l_pQBI = l_pQuest->GetQBI();
		if( !l_pQBI )
			return;

		ProcessQuest_StartQBI( l_pQuest, l_pQBI );

		//----------------------------------------------------------------------

		// 현재 실행중인 QBI의 처리 ( l_pQuest->m_wCurQBINum까지 포함 )
		for( int i = 0; i <= l_pQuest->m_wCurQBINum; ++i )
		{
			QBI_BASE_DATA * l_pPrevQBI = l_pQuest->GetQBI( i );
			if( !l_pPrevQBI )
				return;

			if( ( l_pPrevQBI->m_byFlag & QBIF_BEGIN ) && !( l_pPrevQBI->m_byFlag & QBIF_FINISH ) )
				ProcessQuest_ProcessQBI( l_pQuest, l_pPrevQBI );
		}

		//----------------------------------------------------------------------

		if( ( l_pQuest->m_wCurQBINum >= l_pQuest->m_wTotalQBINum - 1 ) && !( l_pQuest->m_byFlag & QBIF_FINISH ) )
		{
			if( l_pQuest->m_listQBI[l_pQuest->m_wTotalQBINum - 1]->m_byFlag & QBIF_FINISH )
			{
				l_pQuest->m_byFlag |= QBIF_FINISH;
				l_pQuest->m_wCurQBINum = 0;
			}
		}
	}
}

void
QUEST_INFO::ProcessQuest_StartQBI( QUEST * pi_pQuest, QBI_BASE_DATA * pi_pQBI )
{
	if( !pi_pQuest || !pi_pQBI )
		return;

	if( ( pi_pQBI->m_byFlag & QBIF_BEGIN ) || ( pi_pQBI->m_byFlag & QBIF_FINISH ) )
		return;

	//--------------------------------------------------------------------------

	if( pi_pQBI->m_byType == QBI_MESSAGE )
	{
		QBI_MESSAGE_DATA * l_pMsgQBI = ( QBI_MESSAGE_DATA * )pi_pQBI;
		QSV.m_pCurDisplayMsg = l_pMsgQBI;
		l_pMsgQBI->m_byFlag |= QBIF_BEGIN | QBIF_FINISH;
		++pi_pQuest->m_wCurQBINum;
	}

	//--------------------------------------------------------------------------

	else if( pi_pQBI->m_byType == QBI_WAITING_FOR )
	{
		QBI_WAIT_DATA * l_pWaitQBI = ( QBI_WAIT_DATA * )pi_pQBI;
		if( l_pWaitQBI->m_byWaitType == QBI_WAIT_DATA::WAIT_INPUT )
		{
			QSV.m_byInputButton = QBI_WAIT_DATA::WAIT_INPUT_NONE;
			l_pWaitQBI->m_byFlag |= QBIF_BEGIN;
		}
		else if( l_pWaitQBI->m_byWaitType == QBI_WAIT_DATA::WAIT_TIME )
		{
			QSV.m_dwStartTime	= timeGetTime();
			l_pWaitQBI->m_byFlag |= QBIF_BEGIN;
		}
		else if( l_pWaitQBI->m_byWaitType == QBI_WAIT_DATA::WAIT_QBI )
		{
			l_pWaitQBI->m_byFlag |= QBIF_BEGIN;
		}
	}

	//--------------------------------------------------------------------------

	else if( ( pi_pQBI->m_byType == QBI_MOVE ) || ( pi_pQBI->m_byType == QBI_MOVE_RQ ) )
	{
		QBI_MOVE_DATA * l_pMoveQBI = ( QBI_MOVE_DATA * )pi_pQBI;
		if( l_pMoveQBI->m_byMoveType == QBI_MOVE_DATA::MOVE_POSITION )
		{
		}
		else if( l_pMoveQBI->m_byMoveType == QBI_MOVE_DATA::MOVE_DISTANCE )
		{
			QSV.m_fTargetDistance	= l_pMoveQBI->m_dwDistance;
			l_pMoveQBI->m_byFlag |= QBIF_BEGIN;

			if( pi_pQBI->m_byType == QBI_MOVE )
			{
				Vector3f l_vecTargetPos;
				l_vecTargetPos[0] = _GetAvatar()->GetPosX() + ( (float)sin( AngleToPi( _GetAvatar()->GetRotY() /*360 - _GetCamera()->GetPitch()*/ ) ) * ( l_pMoveQBI->m_dwDistance + 5 ) );
				l_vecTargetPos[1] = _GetAvatar()->GetPosY();
				l_vecTargetPos[2] = _GetAvatar()->GetPosZ() + ( (float)cos( AngleToPi( _GetAvatar()->GetRotY() /*360 - _GetCamera()->GetPitch()*/ ) ) * ( l_pMoveQBI->m_dwDistance + 5 ) );

				_GetAvatar()->CreatePath( l_vecTargetPos );
				//_GetAvatar()->SetRotY( GetYAngle( _GetAvatar()->GetPosition(), l_vecTargetPos ) );
				_GetAvatar()->SetRotY( _GetAvatar()->GetPosition(), l_vecTargetPos );
				_GetAvatar()->SetAction( CAI_MOVE_MOVING );
			}

			++pi_pQuest->m_wCurQBINum;
		}
		else if( l_pMoveQBI->m_byMoveType == QBI_MOVE_DATA::MOVE_PORTAL )
		{
		}
	}

	//--------------------------------------------------------------------------

	else if( pi_pQBI->m_byType == QBI_SET_POSITION )
	{
		QBI_POSITION_DATA * l_pPosQBI = ( QBI_POSITION_DATA * )pi_pQBI;
		_GetAvatar()->SetPosition( l_pPosQBI->fPosition, TRUE, FALSE );
		l_pPosQBI->m_byFlag |= QBIF_BEGIN | QBIF_FINISH;
		++pi_pQuest->m_wCurQBINum;
	}

	//--------------------------------------------------------------------------

	else if( pi_pQBI->m_byType == QBI_SET_ROTATION )
	{
		QBI_POSITION_DATA * l_pRotQBI = ( QBI_POSITION_DATA * )pi_pQBI;
		_GetAvatar()->SetRotY( l_pRotQBI->fPosition[1] );
		l_pRotQBI->m_byFlag |= QBIF_BEGIN | QBIF_FINISH;
		++pi_pQuest->m_wCurQBINum;
	}

	//--------------------------------------------------------------------------

	else if( pi_pQBI->m_byType == QBI_SET_CAM_ROTATION )
	{
		QBI_POSITION_DATA * l_pRotQBI = ( QBI_POSITION_DATA * )pi_pQBI;
		_GetCamera()->SetYaw( l_pRotQBI->fPosition[0] );
		_GetCamera()->SetPitch( l_pRotQBI->fPosition[1] );
		_GetCamera()->SetDistance( l_pRotQBI->fPosition[2] );
		_GetCamera()->SetRotatedByKeyboard( TRUE );
		l_pRotQBI->m_byFlag |= QBIF_BEGIN | QBIF_FINISH;
		++pi_pQuest->m_wCurQBINum;
	}

	//--------------------------------------------------------------------------

	else if( ( pi_pQBI->m_byType == QBI_CAM_ROTATE ) || ( pi_pQBI->m_byType == QBI_CAM_ROTATE_RQ ) )
	{
		QBI_POSITION_DATA * l_pRotQBI = ( QBI_POSITION_DATA * )pi_pQBI;
		
		QSV.m_fCameraRotate[0] = fabs( l_pRotQBI->fPosition[0] );
		QSV.m_fCameraRotate[1] = fabs( l_pRotQBI->fPosition[1] );
		QSV.m_fCameraRotate[2] = fabs( l_pRotQBI->fPosition[2] );

		l_pRotQBI->m_byFlag |= QBIF_BEGIN;

		if( l_pRotQBI->m_byType == QBI_CAM_ROTATE )
			_GetCamera()->SetRotate( l_pRotQBI->fPosition );

		++pi_pQuest->m_wCurQBINum;
	}

	//--------------------------------------------------------------------------

	else if( ( pi_pQBI->m_byType == QBI_OPEN_WINDOW ) || ( pi_pQBI->m_byType == QBI_OPEN_WINDOW_RQ ) )
	{
		QBI_UI_DATA * l_pUiQBI = ( QBI_UI_DATA * )pi_pQBI;

		l_pUiQBI->m_byFlag |= QBIF_BEGIN;

		if( l_pUiQBI->m_byType == QBI_OPEN_WINDOW )
			_GetGBM()->OpenWindow( l_pUiQBI->byWindowID );

		++pi_pQuest->m_wCurQBINum;
	}

	//--------------------------------------------------------------------------

	else if( ( pi_pQBI->m_byType == QBI_CLOSE_WINDOW ) || ( pi_pQBI->m_byType == QBI_CLOSE_WINDOW_RQ ) )
	{
		QBI_UI_DATA * l_pUiQBI = ( QBI_UI_DATA * )pi_pQBI;

		l_pUiQBI->m_byFlag |= QBIF_BEGIN;

		if( l_pUiQBI->m_byType == QBI_CLOSE_WINDOW )
			_GetGBM()->CloseWindow( l_pUiQBI->byWindowID );

		++pi_pQuest->m_wCurQBINum;
	}

	//--------------------------------------------------------------------------

	else if( ( pi_pQBI->m_byType == QBI_PUT_ON_EQUIPMENT ) || ( pi_pQBI->m_byType == QBI_PUT_ON_EQUIPMENT_RQ ) )
	{
		QBI_UI_EQUIP_DATA * l_pUiEquipQBI = ( QBI_UI_EQUIP_DATA * )pi_pQBI;

		l_pUiEquipQBI->m_byFlag |= QBIF_BEGIN;

		if( l_pUiEquipQBI->m_byType == QBI_PUT_ON_EQUIPMENT )
		{
			// byPackNo, bySocketNo 가 0xFF라면 아무 아이템이나 찾아서 장착. 없으면 그냥 넘어간다.
			if( ( l_pUiEquipQBI->byPackNo != 0xFF ) && ( l_pUiEquipQBI->bySocketNo != 0xFF ) )
			{
				_GetInventoryBoard()->UI_PutOnEquipment_By_HotKey( l_pUiEquipQBI->byPackNo, l_pUiEquipQBI->bySocketNo );
			}
			else
			{
				BYTE l_byPackNo, l_bySocketNo;
				if( _GetInventoryBoard()->FindEquipmentPosition( l_byPackNo, l_bySocketNo ) )
					_GetInventoryBoard()->UI_PutOnEquipment_By_HotKey( l_byPackNo, l_bySocketNo );
				else
					l_pUiEquipQBI->m_byFlag |= QBIF_FINISH;
			}
		}

		++pi_pQuest->m_wCurQBINum;
	}

	else if( ( pi_pQBI->m_byType == QBI_PUT_OFF_EQUIPMENT ) || ( pi_pQBI->m_byType == QBI_PUT_OFF_EQUIPMENT_RQ ) )
	{
		QBI_UI_EQUIP_DATA * l_pUiEquipQBI = ( QBI_UI_EQUIP_DATA * )pi_pQBI;

		l_pUiEquipQBI->m_byFlag |= QBIF_BEGIN;

		// bySocketNo 가 0xFF라면 아무 아이템이나 찾아서 장착. 없으면 그냥 넘어간다.
		if( l_pUiEquipQBI->m_byType == QBI_PUT_OFF_EQUIPMENT )
		{
			if( l_pUiEquipQBI->byPackNo != 0xFF )
			{
				_GetEquipBoard()->UI_PutOffEquipment_By_HotKey( l_pUiEquipQBI->bySocketNo );
			}
			else
			{
				BYTE l_byPackNo, l_bySocketNo;
				if( _GetEquipBoard()->FindEquipmentPosition( l_byPackNo, l_bySocketNo ) )
					_GetEquipBoard()->UI_PutOffEquipment_By_HotKey( l_bySocketNo );
				else
					l_pUiEquipQBI->m_byFlag |= QBIF_FINISH;
			}
		}

		++pi_pQuest->m_wCurQBINum;
	}

	//--------------------------------------------------------------------------

	else if( ( pi_pQBI->m_byType >= QBI_SET_CURSOR_POS1 ) && ( pi_pQBI->m_byType <= QBI_MOVE_CURSOR2UI ) )
	{
		QBI_CURSOR_DATA * l_pCursorQBI = ( QBI_CURSOR_DATA * )pi_pQBI;

		l_pCursorQBI->m_byFlag |= QBIF_BEGIN;

		if( ( pi_pQBI->m_byType == QBI_SET_CURSOR_POS1 ) || ( pi_pQBI->m_byType == QBI_MOVE_CURSOR1 ) )
		{
			l_pCursorQBI->nTarX = l_pCursorQBI->nOrgX;
			l_pCursorQBI->nTarY = l_pCursorQBI->nOrgY;
		}
		else if( ( pi_pQBI->m_byType == QBI_SET_CURSOR_POS2 ) || ( pi_pQBI->m_byType == QBI_MOVE_CURSOR2 ) )
		{
			switch( l_pCursorQBI->nOrgX )
			{
				case QBI_CURSOR_DATA::MPOS_LEFT		:
				case QBI_CURSOR_DATA::MPOS_TOP		: l_pCursorQBI->nTarX = 0; break;
				case QBI_CURSOR_DATA::MPOS_RIGHT	:
				//case QBI_CURSOR_DATA::MPOS_BOTTOM	: l_pCursorQBI->nTarX = _GetClientRect()->right; break;
				//default								: l_pCursorQBI->nTarX = _GetClientRect()->right / 2; break;
				case QBI_CURSOR_DATA::MPOS_BOTTOM	: l_pCursorQBI->nTarX = GetR3State()->mScreenXsize; break;
				default								: l_pCursorQBI->nTarX = GetR3State()->mScreenXsize / 2.0f; break;
			}
			switch( l_pCursorQBI->nOrgY )
			{
				case QBI_CURSOR_DATA::MPOS_LEFT		:
				case QBI_CURSOR_DATA::MPOS_TOP		: l_pCursorQBI->nTarY = 0; break;
				case QBI_CURSOR_DATA::MPOS_RIGHT	:
				//case QBI_CURSOR_DATA::MPOS_BOTTOM	: l_pCursorQBI->nTarY = _GetClientRect()->bottom; break;
				//default								: l_pCursorQBI->nTarY = _GetClientRect()->bottom / 2; break;
				case QBI_CURSOR_DATA::MPOS_BOTTOM	: l_pCursorQBI->nTarY = GetR3State()->mScreenYsize; break;
				default								: l_pCursorQBI->nTarY = GetR3State()->mScreenYsize / 2.0f; break;
			}
		}
		else if( ( pi_pQBI->m_byType == QBI_SET_CURSOR_POS3 ) || ( pi_pQBI->m_byType == QBI_MOVE_CURSOR3 ) )
		{
			l_pCursorQBI->nTarX = gMouse.x + l_pCursorQBI->nOrgX;
			l_pCursorQBI->nTarY = gMouse.y + l_pCursorQBI->nOrgY;
		}
		else if( ( l_pCursorQBI->m_byType == QBI_SET_CURSOR_POS2UI ) || ( l_pCursorQBI->m_byType == QBI_MOVE_CURSOR2UI ) )
		{
			POINT l_ptWindowPos;

			if( l_pCursorQBI->bySocketNo >= QBI_CURSOR_DATA::IC_HOVER )
			{			
				if( !_GetGBM()->GetWindowPosByItem( l_ptWindowPos, l_pCursorQBI->byWindowID, l_pCursorQBI->bySocketNo, TRUE ) )
					l_pCursorQBI->m_byFlag |= QBIF_FINISH;
			}
			else
			{
				if( !_GetGBM()->GetWindowPos( l_ptWindowPos, l_pCursorQBI->byWindowID, l_pCursorQBI->bySocketNo, TRUE ) )
					l_pCursorQBI->m_byFlag |= QBIF_FINISH;
			}

			l_pCursorQBI->nTarX = l_ptWindowPos.x;
			l_pCursorQBI->nTarY = l_ptWindowPos.y;
		}

		// set cursor
		if( ( pi_pQBI->m_byType == QBI_SET_CURSOR_POS1 ) || ( pi_pQBI->m_byType == QBI_SET_CURSOR_POS2 ) ||
			( pi_pQBI->m_byType == QBI_SET_CURSOR_POS3 ) || ( pi_pQBI->m_byType == QBI_SET_CURSOR_POS2UI ) )
		{
			// button down
			if( l_pCursorQBI->nTarX < 0 || l_pCursorQBI->nTarY < 0 )
			{
				_GetMainGame()->MoveCursor( l_pCursorQBI->nTarX, l_pCursorQBI->nTarY, l_pCursorQBI->byButton );
			}
			// set cursor
			else
			{
				if( _IsWindowed() )
					SetCursorPos( _GetWindowRect()->left + l_pCursorQBI->nTarX, _GetWindowRect()->top + 21 + l_pCursorQBI->nTarY );	// 21은 title bar
				else
					SetCursorPos( l_pCursorQBI->nTarX, l_pCursorQBI->nTarY );
			}

			l_pCursorQBI->m_byFlag |= QBIF_FINISH;
		}
		// move cursor
		else
		{
			_GetMainGame()->MoveCursor( l_pCursorQBI->nTarX, l_pCursorQBI->nTarY, l_pCursorQBI->byButton );
		}

		++pi_pQuest->m_wCurQBINum;
	}

	//--------------------------------------------------------------------------

	else if( ( pi_pQBI->m_byType == QBI_COMBAT ) || ( pi_pQBI->m_byType == QBI_COMBAT_RQ ) )
	{
		QBI_COMBAT_DATA * l_pCombatQBI = ( QBI_COMBAT_DATA * )pi_pQBI;

		l_pCombatQBI->m_byFlag |= QBIF_BEGIN;

		if( l_pCombatQBI->m_byType == QBI_COMBAT )
		{
			// Set Attack Type
			BYTE l_byAttackType = l_pCombatQBI->m_byAttackType;
			if( l_byAttackType == 0xFF )
				l_byAttackType = rand() % 3;

			l_byAttackType = CAT_NORMAL;

			// Set SF
			if( l_byAttackType != CAT_NORMAL )
			{
				/*
				CSFItem * pSFItem = NULL;
				if( l_pCombatQBI->m_dwSFIndex == ID_INVALID )
					pSFItem = _GetLinkBoard()->GetSF( 0, rand() % MAX_SF_BELT_SOCKET );	
				else
					pSFItem = _GetLinkBoard()->GetSFByDTIndex( l_byAttackType, l_pCombatQBI->m_dwSFIndex );

				if( !pSFItem )
				{
					l_pCombatQBI->m_byFlag |= QBIF_FINISH;
					return;
				}
				if( pSFItem->GetType() != l_byAttackType )
				{
					l_pCombatQBI->m_byFlag |= QBIF_FINISH;
					return;
				}

				_GetLinkBoard()->UI_UseSkillForceRequest( 0, NULL, TRUE );
				*/
			}

			// Get Target & Attack
			CCharacter * pTarget = _GetCharMgr()->GetCharacterByDTIndex( l_pCombatQBI->m_dwTarget[0], l_pCombatQBI->m_dwTarget[1] );
			if( pTarget )
			{
			}
			else
			{
				CNetMsgProcessor_Combat * pNMP_Combat = static_cast< CNetMsgProcessor_Combat * >( _GetNetworkMgr()->GetNetMsgProcessor( NMP_COMBAT ) );
				pNMP_Combat->AttackTestRequest_clzo( l_byAttackType, _GetAvatar()->CreateDummy() );
			}
		}

		++pi_pQuest->m_wCurQBINum;
	}
}

void
QUEST_INFO::ProcessQuest_ProcessQBI( QUEST * pi_pQuest, QBI_BASE_DATA * pi_pQBI )
{
	if( !pi_pQuest || !pi_pQBI )
		return;

	//--------------------------------------------------------------------------

	if( pi_pQBI->m_byType == QBI_WAITING_FOR )
	{
		QBI_WAIT_DATA * l_pWaitQBI = ( QBI_WAIT_DATA * )pi_pQBI;
		if( l_pWaitQBI->m_byWaitType == QBI_WAIT_DATA::WAIT_INPUT )
		{
			if( QSV.m_byInputButton == l_pWaitQBI->m_dwValue )
			{
				l_pWaitQBI->m_byFlag |= QBIF_FINISH;
				++pi_pQuest->m_wCurQBINum;
			}
		}
		else if( l_pWaitQBI->m_byWaitType == QBI_WAIT_DATA::WAIT_TIME )
		{
			if( ( timeGetTime() - QSV.m_dwStartTime ) >= l_pWaitQBI->m_dwValue )
			{
				l_pWaitQBI->m_byFlag |= QBIF_FINISH;
				++pi_pQuest->m_wCurQBINum;
			}
		}
		else if( l_pWaitQBI->m_byWaitType == QBI_WAIT_DATA::WAIT_QBI )
		{
			QBI_BASE_DATA * l_pLinkQBI = pi_pQuest->GetQBI( l_pWaitQBI->m_dwValue );
			if( l_pLinkQBI )
			{
				if( l_pLinkQBI->m_byFlag & QBIF_FINISH )
				{
					l_pWaitQBI->m_byFlag |= QBIF_FINISH;
					++pi_pQuest->m_wCurQBINum;
				}
			}
		}
	}

	//--------------------------------------------------------------------------

	else if( ( pi_pQBI->m_byType == QBI_MOVE ) || ( pi_pQBI->m_byType == QBI_MOVE_RQ ) )
	{
		QBI_MOVE_DATA * l_pMoveQBI = ( QBI_MOVE_DATA * )pi_pQBI;
		if( l_pMoveQBI->m_byMoveType == QBI_MOVE_DATA::MOVE_POSITION )
		{
		}
		else if( l_pMoveQBI->m_byMoveType == QBI_MOVE_DATA::MOVE_DISTANCE )
		{
			if( QSV.m_fTargetDistance <= 1 )
				l_pMoveQBI->m_byFlag |= QBIF_FINISH;
		}
		else if( l_pMoveQBI->m_byMoveType == QBI_MOVE_DATA::MOVE_PORTAL )
		{
		}
	}

	//--------------------------------------------------------------------------

	else if( ( pi_pQBI->m_byType == QBI_CAM_ROTATE ) || ( pi_pQBI->m_byType == QBI_CAM_ROTATE_RQ ) )
	{
		QBI_POSITION_DATA * l_pRotQBI = ( QBI_POSITION_DATA * )pi_pQBI;
		if( ( QSV.m_fCameraRotate[0] <= 1 ) &&
			( QSV.m_fCameraRotate[1] <= 1 ) &&
			( QSV.m_fCameraRotate[2] <= 1 ) )
			l_pRotQBI->m_byFlag |= QBIF_FINISH;
	}

	//--------------------------------------------------------------------------

	else if( ( pi_pQBI->m_byType == QBI_COMBAT ) || ( pi_pQBI->m_byType == QBI_COMBAT_RQ ) )
	{
		QBI_COMBAT_DATA * l_pCombatQBI = ( QBI_COMBAT_DATA * )pi_pQBI;

		BOOL l_bIsOK = FALSE;

		// check attack type
		// anything
		if( l_pCombatQBI->m_byAttackType == 0xFF )
		{
			if( QSV.m_byAttackType != ID_INVALID )
				l_bIsOK = TRUE;
		}
		// normal
		else if( ( l_pCombatQBI->m_byAttackType == CAT_NORMAL ) && ( QSV.m_byAttackType == CAT_NORMAL ) )
		{
			l_bIsOK = TRUE;
		}
		// skill & force
		else if( l_pCombatQBI->m_byAttackType == QSV.m_byAttackType )
		{
			if( ( l_pCombatQBI->m_dwSFIndex == ID_INVALID ) || ( l_pCombatQBI->m_dwSFIndex == QSV.m_dwSFIndex ) )
				l_bIsOK = TRUE;
		}

		if( !l_bIsOK )
			return;

		// check target
		if( ( l_pCombatQBI->m_dwTarget[0] == ID_INVALID ) || ( l_pCombatQBI->m_dwTarget[0] == QSV.m_dwTarget[0] ) )
		{
			if( ( l_pCombatQBI->m_dwTarget[1] == ID_INVALID ) || ( l_pCombatQBI->m_dwTarget[1] == QSV.m_dwTarget[1] ) )
				l_bIsOK = TRUE;
		}

		if( !l_bIsOK )
			return;

		// init
		if( l_bIsOK )
		{
			l_pCombatQBI->m_byFlag |= QBIF_FINISH;

			QSV.m_byAttackType	= ID_INVALID;
			QSV.m_dwSFIndex		= ID_INVALID;
			QSV.m_dwTarget[0]	= ID_INVALID;
			QSV.m_dwTarget[1]	= ID_INVALID;
		}
	}

	//--------------------------------------------------------------------------
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Structure          //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
QUEST_INFO::RenderCurMsg( void )
{
	// 메세지 출력 ( 임시 )
	if( QSV.m_pCurDisplayMsg )
	{
		int l_nStringLength = 0;
		for( int i = 0; i < QSV.m_pCurDisplayMsg->m_pMessage->m_byTotalStringNum; ++i )
			l_nStringLength += QSV.m_pCurDisplayMsg->m_pMessage->m_listString[i].m_wLength;

		//int l_nX = ( _GetClientRect()->right / 2 ) - ( ( l_nStringLength * FONT_WIDTH ) / 2 );
		int l_nX = ( GetR3State()->mScreenXsize / 2 ) - ( ( l_nStringLength * FONT_WIDTH ) / 2 );
		int l_nY = 100;

		//_RenderHint_Single( QSV.m_pCurDisplayMsg->m_pMessage, l_nX, l_nY );
	}
}

void
QUEST_INFO::SetCombatInfo( BYTE pi_byAttackType, DWORD pi_dwSFIndex, BYTE pi_byTargetType, DWORD pi_dwTargetDTIndex )
{
	QSV.m_byAttackType	= pi_byAttackType;
	QSV.m_dwSFIndex		= pi_dwSFIndex;
	QSV.m_dwTarget[0]	= pi_byTargetType;
	QSV.m_dwTarget[1]	= pi_dwTargetDTIndex;
}