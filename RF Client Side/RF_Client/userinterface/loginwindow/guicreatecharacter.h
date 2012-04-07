// GUICreateCharactor.h: interface for the CGUICreateCharacter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUICREATECHARACTOR_H__08313A3E_ACA4_4996_8B7F_EB0A15371E97__INCLUDED_)
#define AFX_GUICREATECHARACTOR_H__08313A3E_ACA4_4996_8B7F_EB0A15371E97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../GUIController/GUIWindow.h"
#include "../GUIController/GUIButton.h"
#include "../GUIController/GUISelectItem.h"
#include "../GUIController/GUIInput.h"
#include "../GUIController/GUITextBoard.h"


/*//////////////////////////////////////////////////////////////////////////

[ CGUICreateCharacter ]

//////////////////////////////////////////////////////////////////////////*/
class CGUICreateCharacter : public CGUIWindow,
							public ISelectItemObserver							
{
public:
	enum SelectType {
		Sex,
		Hair,
		Face,
		Coat,
		Pants,
		Glove,
		Shoes,
		SELECT_TYPE_NO
	};	

private:	
	
	CGUIObject			m_cTitleBase;
	CGUIText			m_cTitleStr;

	CGUIObject			m_cLowerBase;
	CGUIButtonEx		m_cOKButton;
	CGUIButtonEx		m_cCancelButton;	
	

	CGUIText			m_cSexStr;
	CGUIText			m_cNameStr;

	CGUISelectItem		m_cSelectItem[SELECT_TYPE_NO];

	CGUIInput			m_cNameInput;


	BOOL				m_bIsCreated;

public:
	CGUICreateCharacter();
	~CGUICreateCharacter();

	void	Init( void );
	void	SetSprite( void );
	
	
	///*MAYUN
	char*	GetName() { return m_cNameInput.GetText();}
	void	ResetName() { m_cNameInput.Clear(); }
	//*/		
	void	SetSelectItem(DWORD type,DWORD index)
	{
		m_cSelectItem[type].SetCurrentItem(index);
	}	
	
	void	MouseEvent( _mouse_event & event );
	void	KeyEvent( _key_event & event );
	void	SelectItemEvent( _select_event & event );
	LRESULT	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};

/*//////////////////////////////////////////////////////////////////////////

[ CGUISelectAttribute ]
 
   - 성향 선택창

//////////////////////////////////////////////////////////////////////////*/

#define	RACE_TYPE_NUM		3

#define	BASIC_POINT_NUM		3
#define	BATTLE_SKILL_NUM	6
#define MAKE_SKILL_NUM		3
#define	FORCE_NUM			5


#define	ATTR_TYPE_NUM	4

#define	ATTR_TYPE_MELEE			0		
#define ATTR_TYPE_MISSILE		1
#define	ATTR_TYPE_FORCE			2
#define	ATTR_TYPE_KNOWLEDGE		3

/////////////////////////////////////////////////////////////////////////////

class CGUISelectAttribute : public CGUIWindow
{
private :

	CGUIObject			m_cTitleBase;
	CGUIText			m_cTitleStr;

	CGUIObject			m_cLowerBase;
	CGUIButtonEx		m_cOKButton;
	CGUIButtonEx		m_cCancelButton;

	CGUIText			m_cAttStr;
	CGUIButton			m_cAttrButton[ATTR_TYPE_NUM];	

	CGUIText			m_cDescStr[2];

	CGUIText			m_cGaugeTitle[3];
	
	// 기본 능력치
	CGUIText			m_cBasicPointStr[BASIC_POINT_NUM];	
	CGUIObject			m_cBasicPointBase[BASIC_POINT_NUM];
	CGUIObjectEx		m_cBasicPointGauge[BASIC_POINT_NUM];	

	// 전투기술 보너스
	CGUIText			m_cBattleSkillStr[BATTLE_SKILL_NUM];	
	CGUIObject			m_cBattleSkillBase[BATTLE_SKILL_NUM];
	CGUIObjectEx		m_cBattleSkillGauge[BATTLE_SKILL_NUM];	

	// 제작기술 보너스
	CGUIText			m_cMakeSkillStr[MAKE_SKILL_NUM];		
	CGUIObject			m_cMakeSkillBase[MAKE_SKILL_NUM];
	CGUIObjectEx		m_cMakeSkillGauge[MAKE_SKILL_NUM];	

	// 포스 보너스
//	CGUIText			m_cForceStr[FORCE_NUM];			
//	CGUIObject			m_cForceBase[FORCE_NUM];
//	CGUIObjectEx		m_cForceGauge[FORCE_NUM];
	

	float				m_fBasicPoint[RACE_TYPE_NUM][ATTR_TYPE_NUM][BASIC_POINT_NUM];
	float				m_fBattleSkillPoint[RACE_TYPE_NUM][ATTR_TYPE_NUM][BATTLE_SKILL_NUM];
	float				m_fMakeSkillPoint[RACE_TYPE_NUM][ATTR_TYPE_NUM][MAKE_SKILL_NUM];
//	float				m_fForcePoint[RACE_TYPE_NUM][ATTR_TYPE_NUM][FORCE_NUM];

	BYTE				m_byAttributeType;

	BYTE				m_byRaceType;

	BOOL				m_bIsCreated;

public :

	CGUISelectAttribute();
	virtual ~CGUISelectAttribute();

	void	Init( void );
	void	SetSprite( void );

	// 각 종족별 속성값 설정, point 범위 : 0.0 ~ 1.0
	void	SetBasicPoint( BYTE pi_byRaceType, BYTE pi_byAttrType, BYTE pi_byIndex, float pi_fPoint );
	void	SetBattleSkill( BYTE pi_byRaceType, BYTE pi_byAttrType, BYTE pi_byIndex, float pi_fPoint ); 
	void	SetMakeSkill( BYTE pi_byRaceType, BYTE pi_byAttrType, BYTE pi_byIndex, float pi_fPoint ); 
	void	SetForcePoint( BYTE pi_byRaceType, BYTE pi_byAttrType, BYTE pi_byIndex, float pi_fPoint ); 

	void	SetRaceType( BYTE pi_byRaceType );

	void	SetAttributeTab( BYTE pi_byTabIndex );

	void	MouseEvent( _mouse_event & event );
};							

#endif // !defined(AFX_GUICREATECHARACTOR_H__08313A3E_ACA4_4996_8B7F_EB0A15371E97__INCLUDED_)