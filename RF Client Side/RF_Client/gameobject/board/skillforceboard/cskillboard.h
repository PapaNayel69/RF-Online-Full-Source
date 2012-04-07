////////////////////////////////////////////////////////////////////////////////
//
// CSkillBoard Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_BOARD_H__
#define __SKILL_BOARD_H__

#include "../Common/CBoard.h"
#include "CSFBoard.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CSkillBoard : public CSFBoard
{
protected :

	CGUISkillWindow		m_uiSkillWindow;

public :
	CSkillBoard();
	virtual ~CSkillBoard();

			void		InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY );
	virtual	void		CreateBoardObject( BYTE pi_byTotalBoardNum, BYTE pi_byTotalMasteryNum, BYTE pi_byTotalSFNum );
			void		CreateClassSkill( DWORD pi_dwClassDTIndex );

protected :

	virtual	BOOL		UI_CheckBeforeAdd_CheckEmptySocket( void );
	virtual	BOOL		UI_CheckBeforeAdd_CheckHoverObject( void );

	virtual	BOOL		UI_AddRequest_ProcessObject( void );
	virtual	void		UI_AddResult_ProcessObject( void );

	virtual	BOOL		UI_CheckBeforeRemove_CheckOriginalObject( BYTE pi_byQuantity );

public :

	virtual	BOOL		IsUsableMastery( BYTE pi_byMasteryType, BYTE pi_byStepType );

	virtual	BYTE		UI_GetClassType( BYTE pi_byMasteryType );

	inline	CSFItem *	GetSkill( BYTE pi_byPackNo, BYTE pi_bySocketNo )	{ return static_cast< CSFItem * >( GetObject( pi_byPackNo,  pi_bySocketNo ) ); }
			CSFItem *	GetSkillByDTCode( DWORD pi_dwDTCode );
			CSFItem *	GetGuardTowerSkill( void );

public :

	virtual	LRESULT		MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );	
};

#endif // __SKILL_BOARD_H__