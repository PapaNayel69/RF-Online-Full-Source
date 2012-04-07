////////////////////////////////////////////////////////////////////////////
//
// CGUISkillForceQueue Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUISKILLFORCEQUEUE_H__
#define __GUISKILLFORCEQUEUE_H__

#include "../../GUIController/GUIWindow.h"
#include "../../GUIController/GUIItem.h"

#define	SF_QUEUE_SIZE		5

struct CONT_SF_INFO 
{
	BYTE	byContType;		// 피해보조 or 이득보조?
	BYTE	bySFType;		// skill or force?
	DWORD	dwSFID;
	DWORD	dwSpriteID;
	DWORD	dwKeepingTime;	// 지속 시간
};


/*//////////////////////////////////////////////////////////////////////////

[ CGUISkillForceQueue ]

  - 아바타에 걸려있는 스킬 포스 지속 (피해&이득) 보조를 보여준다.
  - 피해, 이득 보조 큐 각각 5개씩

//////////////////////////////////////////////////////////////////////////*/
class CGUISkillForceQueue : public CGUIWindow
{
// < Data Member > --------------------------------------------------------
private:
	enum { 
		CONT_DAMAGE,
		CONT_HELP,
		
		MAX_CONT_TYPE_NUM
	};

	struct SF_QUEUE_UNIT
	{
		BYTE		byExistUnit;
		CGUIItem	uiItem;
		DWORD		dwStartTime;
		DWORD		dwTotalTime;

		char		pRemainTimeStr[8];

		DWORD		dwBlinkedTime;
	};	

	SF_QUEUE_UNIT	m_qSFQueueUnit[MAX_CONT_TYPE_NUM][SF_QUEUE_SIZE];

	CGUIContainer	m_uiSFQueueBoard[MAX_CONT_TYPE_NUM];

// < Member Function > ----------------------------------------------------
public:
	CGUISkillForceQueue();
	virtual ~CGUISkillForceQueue();	

	void	SetSprite( void );

	// ----------------------------------------------------------------------
	// 스킬 포스 추가, 삭제
	BOOL	InsertSF( CONT_SF_INFO * pi_pSFInfo );
	void	UpdateSF( CONT_SF_INFO * pi_pSFInfo );

	BOOL	DeleteSF( BYTE byContType, DWORD pi_dwSFID );	
	void	ClearQueue( BYTE pi_byContType );

	// ----------------------------------------------------------------------
	void	MouseEvent( _mouse_event & event );	
	void	Update( void );
	BOOL	Draw( void );
};

#endif // __GUISKILLFORCEQUEUE_H__
