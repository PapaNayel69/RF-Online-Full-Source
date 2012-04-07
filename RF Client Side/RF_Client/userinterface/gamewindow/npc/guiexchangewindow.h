////////////////////////////////////////////////////////////////////////////
//
// CGUIExchangeWindow Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUIEXCHANGEWINDOW_H__
#define __GUIEXCHANGEWINDOW_H__

#include "../../GUIController/GUIWindow.h"
#include "../../GUIController/GUIButton.h"
#include "../../GUIController/GUIInput.h"
#include "../../GUIController/GUIScrollbar.h"

//////////////////////////////////////////////////////////////////////////////

#define	MSGBOX_ID_EXCHANGE		400

//////////////////////////////////////////////////////////////////////////////

class CGUIEconomyRateGraph : public CGUIContainer
{
private :
	BYTE			m_byGraphPointNum;
	float *			m_pGraphPointList[2];
	Vector4f*		m_pCtrlPointList[2];

	int				m_nPointRange[2];
	int				m_nPointWidth;

	BYTE			m_byCurveStep;

	int				m_nRenderPointNum;
	Vector2f *		m_pRenderPointList;

	DWORD			m_dwColor;

public :
	CGUIEconomyRateGraph();
	virtual ~CGUIEconomyRateGraph();

			void	SetPointRange( int pi_nMinPoint, int pi_nMaxPoint );
	inline	void	SetColor( DWORD pi_dwColor ) { m_dwColor = pi_dwColor; }

			void	InitPoint( BYTE pi_byPointNum, WORD * pi_pPointList, int pi_nCurveStep );
			void	InsertPoint( WORD pi_wPoint );

	virtual	void	SetPos( LONG pi_nPosX, LONG pi_nPosY );
	virtual	void	SetPos( POINT pi_ptPoint );

			BOOL	Draw( void );

private :
			void	ClearPointList( void );
			void	ClearRenderPointList( void );

			void	SetCurveStep( BYTE pi_byStep );

			void	SetPoint( BYTE pi_byListIndex, int pi_nPointX, int pi_nPointY );
			void	CalcGraph( void );

};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CGUIExchangeWindow : public CGUIWindow,
						   public IScrollObserver
{
// < Data Member > ---------------------------------------------------------
private:
	CGUIText			m_uiTitle;
	CGUIObjectAni		m_uiAniTitleBar;	
	CGUIButton			m_uiCloseButton;
	CGUIButton			m_uiHelpButton;

	// ------------------------------------------------------------------
	// 경제 지표
	CGUIObject			m_uiEcoBase;
	CGUIText			m_uiEcoTitleStr;	
	
	CGUIObject			m_uiEcoGraphBase;
	CGUIText			m_uiEcoGraphTitleStr;
	CGUIText			m_uiEcoGraphRaceStr;	

	CGUIText			m_uiEcoGraphMsg[2];

	CGUIEconomyRateGraph	m_uiEconomyGraph[3];
	CGUIText			m_uiEcoGraphHourStr;


	CGUIObject			m_uiRaceColor[3];
	CGUIText			m_uiRaceStr[3];

	CGUIObject			m_uiEcoRateBase;
	CGUIText			m_uiEcoRateTitleStr;
	CGUIText			m_uiEcoRateRaceStr;	

	CGUIObject			m_uiTaxRateBase;
	CGUIText			m_uiTaxRateTitleStr;
	CGUIText			m_uiTaxRateStr;
	CGUIText			m_uiTaxRateMsg[2];

	CGUIObject			m_uiExchangeRateBase;
	CGUIText			m_uiExchangeRateTitleStr;
	CGUIText			m_uiExchangeRateStr;
	CGUIText			m_uiExchangeRateMsg[2];	


	// ------------------------------------------------------------------
	// 환전
	CGUIObject			m_uiMoneyBase;
	CGUIText			m_uiMoneyTitleStr;	
	CGUIText			m_uiGoldStr;
	CGUIText			m_uiGold;
	CGUIText			m_uiDalantStr;
	CGUIText			m_uiDalant;

	DWORD				m_dwGold;
	DWORD				m_dwDalant;

	CGUIText			m_uiExchangeAdjStr;
	CGUIScrollbar		m_uiExchangeAdjScrollbar;

	CGUIButtonEx		m_uiExchangeButton;
	CGUIButtonEx		m_uiRecoveryButton;

	DWORD				m_dwDalantPerGold;
	float				m_fExchangeCommission;
	

	BOOL				m_bIsCreated;

// < Member Function > -----------------------------------------------------
public:
	CGUIExchangeWindow();
	virtual ~CGUIExchangeWindow();

			void	Init( void );
			void	SetSprite( void );	

			void	SetRaceType( BYTE pi_byRaceType );

			//----------------------------------------------------------------------
			// 경제 지표 그래프
			void	InitEconomyRatePoint( BYTE pi_byRaceType, BYTE pi_byPointNum, WORD * pi_pEconomyPointList ); 
			void	InsertEconomyRatePoint( BYTE pi_byRaceType, int pi_nEconomyPoint );

			void	SetExchangeRate( DWORD pi_dwDalantPerGold );					// 1골드에 몇달란트인지 환율을 세팅한다.
			void	SetExchangeCommission( float pi_fExchangeCommission );			// 환전 수수료
			void	SetExchangeAdjustbar( void );

			void	SetTaxRate( float pi_fTaxRate );								// 세율 0 ~ 100

			void	SetGold( DWORD pi_dwGold );
			void	SetDalant( DWORD pi_dwDalant );

	inline	DWORD	GetGold( void )					{ return m_dwGold; }
	inline	DWORD	GetDalant( void )				{ return m_dwDalant; }
	
			void	EnableExchange( BOOL pi_bEnable );
			void	EnableRecovery( BOOL pi_bEnable );

			//----------------------------------------------------------------------

			void	MouseEvent( _mouse_event & event );
			void	ScrollEvent( _scroll_event & event );

			void	ClosingDialogBox( CGUIDialogBox * pi_pDlg );

};

#endif // __GUIEXCHANGEWINDOW_H__
