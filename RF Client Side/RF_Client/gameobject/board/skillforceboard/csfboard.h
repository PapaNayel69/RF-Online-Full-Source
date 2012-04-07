////////////////////////////////////////////////////////////////////////////////
//
// CSFBoard Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __SF_BOARD_H__
#define __SF_BOARD_H__

#include "../Common/CBoard.h"
#include "../../../UserInterface/GameWindow/SkillForce/GUISkillWindow.h"
#include "../../../Item/CItemData.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CNetMsgProcessor_ItemArrangement;

class CSFBoard : public CObjectBoard
{
protected :

	CNetMsgProcessor_ItemArrangement *	m_pNetMsgProcessor_Arrange;

	BYTE		m_byTotalMasteryNum;
	DWORD *		m_dwSuccessCountOfMastery;		// 마스터리의 성공 횟수
	BYTE *		m_byPointOfMastery;				// 마스터리의 누적 포인트 ( 백분율 )
	BYTE *		m_bIsUsableMastery;				// 사용 가능한 마스터리인가?

	BYTE		m_byTotalSFNum;
	DWORD *		m_dwCastTime;	

public :
	CSFBoard();
	virtual ~CSFBoard();			

	virtual	void		CreateBoardObject( BYTE pi_byTotalBoardNum, BYTE pi_byTotalMasteryNum, BYTE pi_byTotalSFNum );

protected :

	virtual	BOOL		Add_ProcessExistObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject );
	virtual	BOOL		Add_ProcessNewObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject );

	virtual	BOOL		Remove_ProcessRemoveQuantity( CBoardObject * pi_pOrgObject, BYTE pi_byQuantity );
	virtual	BOOL		Remove_ProcessRemoveObject( BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardObject * pi_pOrgObject );	
	
	virtual	BOOL		UI_CheckBeforeRemove_CheckOriginalObject( BYTE pi_byQuantity );
	virtual	BOOL		UI_RemoveRequest_ProcessObject( BYTE pi_byQuantity );

	virtual	BOOL		UI_SwapRequest_ProcessObject( void );
	virtual	void		UI_SwapResult_ProcessObject( void );

	virtual	BOOL		UI_CheckBeforeUse_CheckOriginalObject( void );
	virtual	BOOL		UI_UseRequest_ProcessObject( void );
	virtual	void		UI_UseResult_ProcessObject( void );

protected :

			BOOL		IsEqualSFTargetTypeAndAvatarTarget( CSFItem * pi_pSFItem, CCharacter * pi_pTarget );
			BOOL		InvokeSkillForce( CSFItem * pi_pSFItem, CCharacter * pi_pTarget );			
			void		ChangeCursor( CSFItem * pi_pSFItem );	

public :

	virtual	void		FrameMove( void );
	virtual	LRESULT		MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );	

public :
	
			CSFItem *	GetSF( BYTE pi_bySocketNo );
			CSFItem *	GetSFByIndex( DWORD pi_dwIndex );
			CSFItem *	GetSFByDTIndex(	DWORD pi_dwDTIndex );
			CSFItem *	GetSFByDTCode( DWORD pi_dwDTCode );

	virtual	BYTE		UI_GetClassType( BYTE pi_byMasteryType ) = 0;

			BOOL		IsUsableSF( CSFItem * pi_pSFItem );
	virtual	BOOL		IsUsableMastery( BYTE pi_byMasteryType, BYTE pi_byStepType );

			BYTE		GetPointOfMastery( BYTE pi_byMasteryType );
			BYTE		GetPointOfMasteryByDTCode( DWORD pi_dwDTCode );
			BYTE		GetSFExpertness( BYTE pi_byUIClassTypeNo );
			BYTE		GetSFExpertness( BYTE pi_byUIClassTypeNo, DWORD * pi_pSuccessCountList );

			void		SetSuccessCountOfMastery( BYTE pi_byMasteryType, BYTE pi_byStepType, DWORD pi_dwCount, BOOL pi_bIsAdd );
			BYTE		CalculateSuccessCountOfMastery( BYTE pi_byMasteryType, BYTE pi_byStepType, DWORD pi_dwCount, BOOL pi_bIsAdd );

	inline	void		SetCastTime( BYTE pi_byOrderIndex, DWORD pi_dwTime )	{ m_dwCastTime[pi_byOrderIndex] = pi_dwTime; }
	inline	DWORD		GetCastTime( BYTE pi_byOrderIndex )						{ return m_dwCastTime[pi_byOrderIndex]; }

			void		UseSkillForce_Request( CSFItem * pi_pSFItem, CCharacter * pi_pTarget, BOOL pi_bEventInLinkBoard = FALSE );			

			void		UI_Link_By_Hotkey( DWORD pi_dwIndex );
			void		Unlink( DWORD pi_dwIndex );
};

#endif // __SF_BOARD_H__