////////////////////////////////////////////////////////////////////////////
//
// CGUISkillForceQueue Class Implementation
//
////////////////////////////////////////////////////////////////////////////
#include "GUISkillForceQueue.h"
#include "../../GUIController/GUIScreen.h"
#include "../../SpriteMgr.h"
#include "../../UIGlobal.h"

const int SF_ITEM_SIZE = 32;

CGUISkillForceQueue::CGUISkillForceQueue()
{
	for( int i = 0; i < MAX_CONT_TYPE_NUM; ++i )
	{
		m_uiSFQueueBoard[i].SetSize( SF_ITEM_SIZE, SF_ITEM_SIZE * SF_QUEUE_SIZE );
		
		for( int j = 0; j < SF_QUEUE_SIZE; ++j )
		{
			m_qSFQueueUnit[i][j].uiItem.SetSize( SF_ITEM_SIZE, SF_ITEM_SIZE );
		}
	}

	ClearQueue( SF_CONT_DAMAGE );
	ClearQueue( SF_CONT_HELP );
}

CGUISkillForceQueue::~CGUISkillForceQueue()
{

}

void
CGUISkillForceQueue::SetSprite( void )
{
	CSprite * pSprite;	
	pSprite = gSpriteMgr.GetSprite( SP_ID_ITEM );
	if( !pSprite )
		return;

	for( int i = 0; i < MAX_CONT_TYPE_NUM; ++i )
	{
		for( int j = 0; j < SF_QUEUE_SIZE; ++j )
		{
			if( m_qSFQueueUnit[i][j].byExistUnit )
				m_qSFQueueUnit[i][j].uiItem.GetSpriteInfo()->pSprite = pSprite;
		}
	}
}

// ==================================================================================

BOOL
CGUISkillForceQueue::InsertSF( CONT_SF_INFO * pi_pSFInfo )
{
	BYTE byContType;
	byContType = pi_pSFInfo->byContType - SF_CONT_DAMAGE;
	if( byContType >= MAX_CONT_TYPE_NUM )
		return FALSE;

	for( int i = 0; i < SF_QUEUE_SIZE; ++i )
	{
		if( !m_qSFQueueUnit[byContType][i].byExistUnit )
		{
			// insert SF
			m_qSFQueueUnit[byContType][i].byExistUnit = TRUE;

			m_qSFQueueUnit[byContType][i].dwStartTime = timeGetTime();
			m_qSFQueueUnit[byContType][i].dwTotalTime = pi_pSFInfo->dwKeepingTime;

			m_qSFQueueUnit[byContType][i].dwBlinkedTime = 0;

			m_qSFQueueUnit[byContType][i].uiItem.SetItemID( pi_pSFInfo->dwSFID );
			m_qSFQueueUnit[byContType][i].uiItem.SetSpriteID( pi_pSFInfo->dwSpriteID );

			if( pi_pSFInfo->bySFType == 0 )
				m_qSFQueueUnit[byContType][i].uiItem.SetIconType( SKILL_ICON );
			else
				m_qSFQueueUnit[byContType][i].uiItem.SetIconType( FORCE_ICON );

			m_qSFQueueUnit[byContType][i].uiItem.SetSprite();

			// add queue board
			if( !IsAddedObject( &m_uiSFQueueBoard[byContType] ) )
			{
				// 다른보드가 있으면 뒤에 붙인다.
				if( IsAddedObject( &m_uiSFQueueBoard[(byContType+1)%2] ) )
				{
					// 이득보조가 앞에 오도록
					if( byContType == CONT_HELP )
					{
						m_uiSFQueueBoard[byContType].SetPos( m_ptPos.x, m_ptPos.y );
						m_uiSFQueueBoard[(byContType+1)%2].SetPos( m_ptPos.x + SF_ITEM_SIZE, m_ptPos.y );
					}
					else
					{
						m_uiSFQueueBoard[(byContType+1)%2].SetPos( m_ptPos.x, m_ptPos.y );
						m_uiSFQueueBoard[byContType].SetPos( m_ptPos.x + SF_ITEM_SIZE, m_ptPos.y );
					}
										

					// resize window
					SetSize( SF_ITEM_SIZE * 2, SF_ITEM_SIZE * SF_QUEUE_SIZE );
				}
				else
				{
					SetSize( SF_ITEM_SIZE, SF_ITEM_SIZE * SF_QUEUE_SIZE );

					m_uiSFQueueBoard[byContType].SetPos( m_ptPos.x, m_ptPos.y );
				}

				m_uiSFQueueBoard[byContType].Show( IsShow() );
				Add( &m_uiSFQueueBoard[byContType] );

				// repos window
				SetPos( gScreen.m_ptSize.x - m_ptSize.x, m_ptPos.y );
			}
			
			// 맨뒤에 추가한다.
			m_qSFQueueUnit[byContType][i].uiItem.SetPos( m_uiSFQueueBoard[byContType].m_ptPos.x,
														 m_uiSFQueueBoard[byContType].m_ptPos.y + m_uiSFQueueBoard[byContType].GetAddedObjectNum() * SF_ITEM_SIZE );

			m_qSFQueueUnit[byContType][i].uiItem.Show( IsShow() );
			m_uiSFQueueBoard[byContType].Add( &m_qSFQueueUnit[byContType][i].uiItem );

			if( !IsShow() )
				Show( TRUE );			

			return TRUE;
		}
	}

	return FALSE;
}

void
CGUISkillForceQueue::UpdateSF( CONT_SF_INFO * pi_pSFInfo )
{
	BYTE byContType;
	byContType = pi_pSFInfo->byContType - SF_CONT_DAMAGE;
	if( byContType >= MAX_CONT_TYPE_NUM )
		return;

	BYTE bySFType;
	if( pi_pSFInfo->bySFType == 0 )
		bySFType = SKILL_ICON;
	else
		bySFType = FORCE_ICON;

	for( int i = 0; i < SF_QUEUE_SIZE; ++i )
	{
		if( !m_qSFQueueUnit[byContType][i].byExistUnit )
			continue;

		if( m_qSFQueueUnit[byContType][i].uiItem.GetIconType() == bySFType &&
			m_qSFQueueUnit[byContType][i].uiItem.GetItemID() == pi_pSFInfo->dwSFID )
		{
			m_qSFQueueUnit[byContType][i].dwStartTime	= timeGetTime();
			m_qSFQueueUnit[byContType][i].dwTotalTime	= pi_pSFInfo->dwKeepingTime;
			m_qSFQueueUnit[byContType][i].dwBlinkedTime	= 0;
			return;
		}
	}
}

// ==================================================================================

BOOL
CGUISkillForceQueue::DeleteSF( BYTE pi_byContType, DWORD pi_dwSFID )
{
	BYTE byContType;
	byContType = pi_byContType - SF_CONT_DAMAGE;
	if( byContType >= MAX_CONT_TYPE_NUM )
		return FALSE;

	for( int i = 0; i < SF_QUEUE_SIZE; ++i )
	{
		if( !m_qSFQueueUnit[byContType][i].byExistUnit )
			continue;

		if( m_qSFQueueUnit[byContType][i].uiItem.GetItemID() == pi_dwSFID )
		{
			// remove sf
			m_uiSFQueueBoard[byContType].Remove( &m_qSFQueueUnit[byContType][i].uiItem );

			m_qSFQueueUnit[byContType][i].byExistUnit = FALSE;


			// 보드에 SF가 없으면 보드를 제거한다.
			if( !m_uiSFQueueBoard[byContType].GetAddedObjectNum() )
			{
				Remove( &m_uiSFQueueBoard[byContType] );

				// 다른 보드가 있으면 왼쪽으로 밀어준다.
				if( IsAddedObject( &m_uiSFQueueBoard[(byContType+1)%2] ) )
				{
					SetSize( SF_ITEM_SIZE, m_ptSize.y );
						
					m_uiSFQueueBoard[(byContType+1)%2].SetPos( m_ptPos.x, m_ptPos.y );
				}
				else
				{
					SetSize( 0, m_ptSize.y );
				}
				
				// repos window
				SetPos( gScreen.m_ptSize.x - m_ptSize.x, m_ptPos.y );
			}
			// SF 제거됐을 때 빈칸을 없애기 위해 위치를 다시 잡아준다.
			else
			{
				CGUIObject * pSFObj;
				for( int j = 0; j < m_uiSFQueueBoard[byContType].GetAddedObjectNum(); ++j )
				{
					pSFObj = m_uiSFQueueBoard[byContType].GetAddedObject( j );
					if( pSFObj )
					{
						pSFObj->SetPos( m_uiSFQueueBoard[byContType].m_ptPos.x,
										m_uiSFQueueBoard[byContType].m_ptPos.y + j * SF_ITEM_SIZE );
					}
				}
			}

			// sf가 존재하지 않으면 close
			if( !m_ptSize.x )
				Close();
			
			return TRUE;
		}

	}

	return FALSE;
}

void
CGUISkillForceQueue::ClearQueue( BYTE pi_byContType )
{
	BYTE byContType;
	byContType = pi_byContType - SF_CONT_DAMAGE;
	if( byContType >= MAX_CONT_TYPE_NUM )
		return;

	for( int i = 0; i < SF_QUEUE_SIZE; ++i )
	{
		m_qSFQueueUnit[byContType][i].byExistUnit = FALSE;
	}

	m_uiSFQueueBoard[byContType].RemoveAll();
}

// ==================================================================================

void
CGUISkillForceQueue::MouseEvent( _mouse_event & event )
{
	CGUIWindow::MouseEvent( event );

	if( event.id == MOUSE_HOVERED )
	{
		for( int i = 0; i < MAX_CONT_TYPE_NUM; ++i )
		{
			for( int j = 0; j < SF_QUEUE_SIZE; ++j )
			{
				if( m_qSFQueueUnit[i][j].byExistUnit &&
					event.source == &m_qSFQueueUnit[i][j].uiItem )
				{
					// tool tip
					return;
				}
			}
		}
	}	
}

// ==================================================================================

#define	BLINK_INTERVAL		800
#define	BLINK_INTERVAL_HALF	400

void
CGUISkillForceQueue::Update( void )
{
	DWORD	dwCurTime;
	float	fCountdown;
	BYTE	byAlpha;
	float	fBlinkValue;	

	// check finishing time
	for( int i = 0; i < MAX_CONT_TYPE_NUM; ++i )
	{
		for( int j = 0; j < SF_QUEUE_SIZE; ++j )
		{
			if( !m_qSFQueueUnit[i][j].byExistUnit )
				continue;

			dwCurTime = timeGetTime() - m_qSFQueueUnit[i][j].dwStartTime;

			if( dwCurTime < m_qSFQueueUnit[i][j].dwTotalTime )
			{	
				fCountdown = ( m_qSFQueueUnit[i][j].dwTotalTime - dwCurTime ) / 1000.f;
				if( fCountdown - (int)fCountdown > 0.f ) // 반올림
					++fCountdown;

				// set time
				itoa( (int)fCountdown, m_qSFQueueUnit[i][j].pRemainTimeStr, 10 );

				if( ( i == CONT_HELP ) && 
					( dwCurTime > m_qSFQueueUnit[i][j].dwTotalTime * 0.9 ) )
				{
					fBlinkValue = ( 1.f - ((float)dwCurTime / m_qSFQueueUnit[i][j].dwTotalTime) );

					if( timeGetTime() - m_qSFQueueUnit[i][j].dwBlinkedTime > BLINK_INTERVAL )
					{
						m_qSFQueueUnit[i][j].dwBlinkedTime = timeGetTime();
					}

					if( timeGetTime() - m_qSFQueueUnit[i][j].dwBlinkedTime < BLINK_INTERVAL_HALF )
					{
						fBlinkValue += ( 1 - fBlinkValue ) * (float)(timeGetTime()-m_qSFQueueUnit[i][j].dwBlinkedTime) / BLINK_INTERVAL_HALF;
					}
					else
					{
						fBlinkValue += ( 1 - fBlinkValue ) * ( - (float)(timeGetTime()-m_qSFQueueUnit[i][j].dwBlinkedTime) / BLINK_INTERVAL_HALF );
					}

					byAlpha = 0x50 + 0xAF * fBlinkValue;
				}
				else
				{
					byAlpha = 0x5A + 0xA5 * ( 1.f - ((float)dwCurTime / m_qSFQueueUnit[i][j].dwTotalTime) );
				}

				// set alpha
				m_qSFQueueUnit[i][j].uiItem.SetARGB( byAlpha << 24 | 0x00ffffff );
			}
		}
	}
}

// ==================================================================================

BOOL
CGUISkillForceQueue::Draw( void )
{
	if( !IsShow() )
		return FALSE;

	CGUIContainer::Draw();

	// sf rest time
	for( int i = 0; i < MAX_CONT_TYPE_NUM; ++i )
	{
		for( int j = 0; j < SF_QUEUE_SIZE; ++j )
		{
			if( m_qSFQueueUnit[i][j].byExistUnit )
			{
				DrawR3Hangul( m_qSFQueueUnit[i][j].uiItem.m_ptPos.x,
							  m_qSFQueueUnit[i][j].uiItem.m_ptPos.y + 20,
							  m_qSFQueueUnit[i][j].pRemainTimeStr,
							  D3DCOLOR_XRGB( 255, 255, 255),
							  R3_HAN_OUTLINE );
			}
		}
	}				 

	DrawDialogBox();

	return TRUE;
}