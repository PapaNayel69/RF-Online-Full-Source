////////////////////////////////////////////////////////////////////////////
//
// CGUIOptionWindow Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUIOPTIONWINDOW_H__
#define __GUIOPTIONWINDOW_H__

#include "../../GUIController/GUIWindow.h"
#include "../../GUIController/GUIButton.h"
#include "../../GUIController/GUIScrollbar.h"

/*//////////////////////////////////////////////////////////////////////////

[ CGUIOptionWindow ]
 
   - 옵션창
   - 사운드, 그래픽, 채팅 옵션을 설정한다.

//////////////////////////////////////////////////////////////////////////*/

#define	TOTAL_CAMERA_NUM	5

class CGUIOptionWindow : public CGUIWindow, IScrollObserver
{
public:
	enum SoundOption 
	{
		Effect_Snd,
		BGM,
		SOUND_OPTION_NUM
	};

	enum GraphicOption
	{
		Gamma,
		DynamicLighting,
		Shadow,
		SeeDist,
		GRAPHIC_OPTION_NUM
	};

	enum EtcOption
	{
		Whisper,
		Party,
		Guild,
		Trade,
		Hint,
		AutoSeeDist,
		DetailTexture,
		ETC_OPTION_NUM
	};
	

// < Data Member > ---------------------------------------------------------
private:
	CGUIText		m_uiTitle;
	CGUIObjectAni	m_uiAniTitleBar;
	CGUIButton		m_uiCloseButton;	
	CGUIButton		m_uiHelpButton;
	
	CGUIButtonEx	m_uiLogOutButton;				// 로그아웃
	CGUIButtonEx	m_uiExitButton;					// 게임 종료
	
	// sound option
	CGUIText		m_uiSoundTitleStr;
	CGUIText		m_uiSoundStr[SOUND_OPTION_NUM];
	CGUIScrollbar	m_uiSoundCtrl[SOUND_OPTION_NUM];	

	// graphic option
	CGUIText		m_uiGraphicTitleStr;
	CGUIText		m_uiGraphicStr[GRAPHIC_OPTION_NUM];
	CGUIScrollbar	m_uiGraphicCtrl[GRAPHIC_OPTION_NUM];
	CGUIObject		m_uiGraphicNum[GRAPHIC_OPTION_NUM];

	
	// etc option. switch on/off option
	CGUIText		m_uiEtcTitleStr;
	CGUIObject		m_uiEtcSwitch[ETC_OPTION_NUM];
	CGUIText		m_uiEtcSwitchStr[ETC_OPTION_NUM];

	// camera option
	CGUIText		m_uiCamTitleStr;
	CGUIObject		m_uiCamSelectObj[TOTAL_CAMERA_NUM];
	BYTE			m_bySelectedCamera;
	

	BOOL			m_bIsCreated;


// < Member Function > ----------------------------------------------------
public:
	CGUIOptionWindow();
	virtual ~CGUIOptionWindow();

	void	Init( void );
	void	SetSprite( void );

	void	Open( void );
	void	Close( void );

	// sound
	void	SetSoundOption( SoundOption pi_eSoundOption, float pi_fVolume ); // pi_fVolume : 0.f ~ 1.f
	float	GetSoundOption( SoundOption pi_eSoundOption );
	
	// graphic
	void	SetGraphicOption( GraphicOption pi_eGraphicOption, float pi_fValue );
	float	GetGraphicOption( GraphicOption pi_eGraphicOption );

	// etc
	void	SwitchOn( EtcOption pi_eOption, BOOL pi_bSwitchOn );
	BOOL	IsSwitchOn( EtcOption pi_eOption );	

	// camera
	void	SelectCamera( BYTE pi_byCamIndex );
	BYTE	GetSelectedCamera( void ) { return m_bySelectedCamera; }


	void	MouseEvent( _mouse_event & event );
	void	ScrollEvent( _scroll_event & event );	
};

#endif // __GUIOPTIONWINDOW_H__