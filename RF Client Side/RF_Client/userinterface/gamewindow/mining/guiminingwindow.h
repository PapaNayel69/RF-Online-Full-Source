/*//////////////////////////////////////////////////////////////////////////

[ CGUIMiningWindow ]
 
   - √§±º«“ Ore º±≈√, √§±º On/Off

//////////////////////////////////////////////////////////////////////////*/

#ifndef ___GUIMININGWINDOW_H__
#define ___GUIMININGWINDOW_H__

#include "../../GUIController/GUIWindow.h"
#include "../../GUIController/GUIRadioButton.h"
#include "../../GUIController/GUIButton.h"


class CGUIMiningWindow : public CGUIWindow						 
{
// < Data Member > ---------------------------------------------------------
private:
	CGUIText			m_uiTitle;

	CGUIObject			m_uiOreSelectBase;
	CGUIObjectAni		m_uiOreSelectObject[5];

	CGUIButtonEx		m_uiMiningButton;		

	BYTE				m_bySelectedOreIndex;

	BOOL				m_bIsCreated;
	
// < Member Function > -----------------------------------------------------
public:
	CGUIMiningWindow();
	virtual ~CGUIMiningWindow();

			void	Init( void );
			void	SetSprite( void );
	

			void	SelectOre( BYTE pi_byOreIndex );	

			void	Mining_On( void );
			void	Mining_Off( void );	


			void	EnableMiningButton( BOOL pi_bEnable );
			
			void	MouseEvent( _mouse_event & event );
};

#endif // ___GUIMININGWINDOW_H__
