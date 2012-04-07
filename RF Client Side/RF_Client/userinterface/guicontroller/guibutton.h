////////////////////////////////////////////////////////////////////////////
//
// CGUIButton Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUIBUTTON_H__
#define __GUIBUTTON_H__

#include "GUIObject.h"
#include "GUIContainer.h"
#include "GUIText.h"

/*//////////////////////////////////////////////////////////////////////////

[ CGUIButton ]

//////////////////////////////////////////////////////////////////////////*/
class CGUIButton : public CGUIContainer
{
// < Data Member > ---------------------------------------------------------
private:		
	SPRITE_INFO		m_sSpriteInfoEx[4];	

	BOOL			m_bSoundOn;
	DWORD			m_dwSoundID;

// < Member Function > -----------------------------------------------------
public:
	CGUIButton();
	~CGUIButton();
	
			void	SetSpriteInfoEx( SPRITE_INFO * pi_spriteInfo );
			void	SetSpriteInfoEx( WORD pi_wState, SPRITE_INFO * pi_spriteInfo );

	virtual	void	SetState( WORD pi_wState );

			void	SetSprite( CSprite * pi_pSprite );	


	inline	void	SoundOn( BOOL pi_bSoundOnFlag ) { m_bSoundOn = pi_bSoundOnFlag; }
	inline	void	SetSoundID( DWORD pi_dwSoundID ) { m_dwSoundID = pi_dwSoundID; }				

protected :
			WORD	GetSpriteIndexByState( void );
};

/*//////////////////////////////////////////////////////////////////////////

[ CGUIButtonEx ]

//////////////////////////////////////////////////////////////////////////*/
class CGUIButtonEx : public CGUIButton
{
// < Data Member > ---------------------------------------------------------
private:
	CGUIText		m_cLabel;	

	POINT			m_ptLabelPos[2];

	DWORD			m_dwLabelColor[2];

// < Member Function > -----------------------------------------------------
public:
	CGUIButtonEx();
	~CGUIButtonEx();

	void		SetState( WORD pi_wState );	
	BOOL		InputProc( void );

	void		SetLabel( char * pi_pLabel );
	char *		GetLabel( void ) { return m_cLabel.GetString(); }

	void		SetLabelColor( DWORD pi_dwColor );
	void		SetLabelHoveredColor( DWORD pi_dwColor );

	void		SetLabelStyle( DWORD pi_dwStyle );

	void		SetLabelPos( int pi_nPosX, int pi_nPosY );
	void		SetLabelPressedPos( int pi_nPosX, int pi_nPosY );	

	void		GetLabePos( POINT & po_ptPos )			{ po_ptPos = m_ptLabelPos[0]; }
	void		GetLabePressedPos( POINT & po_ptPos )	{ po_ptPos = m_ptLabelPos[1]; }

private:
	void		ResetLabelColor( void );
	void		ResetLabelPos( void );		
};


#endif // __GUIBUTTON_H__
