////////////////////////////////////////////////////////////////////////////////
//
// CForceBoard Class Header
//
////////////////////////////////////////////////////////////////////////////////
#ifdef _NEW_UI_

#ifndef __FORCE_BOARD_H__
#define __FORCE_BOARD_H__

#include "../Common/CBoard.h"
#include "CSFBoard.h"
#include "../../../UserInterface/GameWindow/SkillForce/GUIForceWindow.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CForceBoard : public CSFBoard
{
protected :

	CGUIForceWindow			m_uiForceWindow;

public:
	CForceBoard();
	virtual ~CForceBoard();

			void	InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY );
	virtual	void	CreateBoardObject( BYTE pi_byTotalBoardNum, BYTE pi_byTotalMasteryNum, BYTE pi_byTotalSFNum );

protected :

	virtual	BOOL	UI_CheckBeforeAdd_CheckEmptySocket( void );
	virtual	BOOL	UI_CheckBeforeAdd_CheckHoverObject( void );
	virtual	BOOL	UI_CheckBeforeAdd_CheckTargetObject( void );

	virtual	BOOL	UI_AddRequest_ProcessObject( void );
	virtual	void	UI_AddResult_ProcessObject( void );

	virtual	BOOL	UI_CheckBeforeRemove_CheckOriginalObject( BYTE pi_byQuantity );


public :			

	virtual	BOOL	IsUsableMastery( BYTE pi_byMasteryType, BYTE pi_byStepType );

	virtual	BYTE	UI_GetClassType( BYTE pi_byMasteryType );	

			BYTE	CalculateForceExpertness( DWORD * pi_pSuccessCountOfMastery = NULL );
private :

			DWORD	GetSuccessCountForEachExpertnessLevel( BYTE pi_byExpertness );			

public :

	virtual	LRESULT	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );	
};

#endif // __FORCE_BOARD_H__

#endif // _NEW_UI
