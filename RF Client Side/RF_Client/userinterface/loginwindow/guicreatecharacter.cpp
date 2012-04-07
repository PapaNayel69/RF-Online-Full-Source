

#include "GUICreateCharacter.h"
#include "../SpriteMgr.h"
#include "../GUIController/GUIMessageBox.h"
#include "../../define.h"

//


////////////////////////////////////////////////////////////////////////////
//
// CGUICreateCharacter Class Implementation
//
////////////////////////////////////////////////////////////////////////////
///*MAYUN
#define INPUTLEN	16
void EventLeftSelectSex();
void EventRightSelectSex();
void EventLeftSelectFace();
void EventRightSelectFace();
void EventLeftSelectHead();
void EventRightSelectHead();
void EventLeftSelectUpper();
void EventRightSelectUpper();
void EventLeftSelectLower();
void EventRightSelectLower();
void EventLeftSelectGloves();
void EventRightSelectGloves();
void EventLeftSelectShoes();
void EventRightSelectShoes();
void EventCreateCharacterOK();
void EventCreateCharacterCancel();
void EventSelectAttributeOK();
void EventSelectAttributeCancel();

///*MAYUN
void EventSelectAttribute(DWORD race, DWORD attb);
void LoadAttribute(CGUISelectAttribute * attb);

//*/
//*/

CGUICreateCharacter::CGUICreateCharacter()
{
	m_bIsCreated = FALSE;
}

CGUICreateCharacter::~CGUICreateCharacter()
{
	
}

void
CGUICreateCharacter::Init( void )
{		
	SetSize( 178, 326 );

	m_cTitleBase.SetSize( 146, 26 );
	m_cLowerBase.SetSize( 50, 37 );

	m_cOKButton.SetSize( 89, 31 );
	m_cCancelButton.SetSize( 89, 31 );	

	POINT l_ptSize;
	l_ptSize.x = 21;
	l_ptSize.y = 17;
	for( int i=0; i<SELECT_TYPE_NO; ++i )
		m_cSelectItem[i].Init( l_ptSize, 71 );

	m_cSelectItem[Sex].SetItemBoardWidth( 29 );

	m_cNameInput.SetSize( 102, 12 );
	
	m_cTitleStr.SetString( "캐릭터 생성" );
	m_cCancelButton.SetLabel( "취 소" );
	m_cOKButton.SetLabel( "확 인" );

	m_cSexStr.SetString( "성 별" );
	m_cNameStr.SetString( "이 름" );

	// -----------------------------------------------------------------
	m_cTitleBase.SetPos( m_ptPos.x + 16, m_ptPos.y + 4 );
	m_cLowerBase.SetPos( m_ptPos.x + 65, m_ptPos.y + 289 );

	m_cTitleStr.SetPos( m_cTitleBase.m_ptPos.x + (m_cTitleBase.m_ptSize.x - m_cTitleStr.m_ptSize.x)/2,
						m_cTitleBase.m_ptPos.y + 7 );

	m_cOKButton.SetPos( m_ptPos.x, m_ptPos.y + m_ptSize.y - m_cOKButton.m_ptSize.y );	
	m_cCancelButton.SetPos( m_ptPos.x + m_ptSize.x - m_cCancelButton.m_ptSize.x, m_cOKButton.m_ptPos.y );


	m_cOKButton.SetLabelPos( 29, 11 );
	m_cCancelButton.SetLabelPos( 31, 11 );


	m_cSelectItem[Sex].SetPos( m_ptPos.x + 53, m_ptPos.y + 52 );
	
	for( i=1; i<7; ++i )
	{		
		m_cSelectItem[i].SetPos( m_ptPos.x + 31, m_ptPos.y + 94 + (i-1) * 22 );
	}	

	m_cNameInput.SetPos( m_ptPos.x + 38, m_ptPos.y + 257 );
	
	m_cSexStr.SetPos( m_ptPos.x + (m_ptSize.x - m_cSexStr.m_ptSize.x)/2, m_ptPos.y + 38 );	
	m_cNameStr.SetPos( m_ptPos.x + (m_ptSize.x - m_cNameStr.m_ptSize.x)/2, m_ptPos.y + 241 );	


	// -----------------------------------------------------------------
	Add( &m_cTitleBase );
	Add( &m_cTitleStr );

	m_cOKButton.AddMouseEventHandler( this );
	m_cCancelButton.AddMouseEventHandler( this );
	Add( &m_cOKButton );
	Add( &m_cCancelButton );

	Add( &m_cLowerBase );

	Add( &m_cSexStr );
	Add( &m_cNameStr );
	
	for( i=0; i<SELECT_TYPE_NO; ++i )
	{		
		m_cSelectItem[i].AddSelectItemHandler( this );
		Add( &m_cSelectItem[i] );
	}

	m_cNameInput.AddMouseEventHandler( this );
	m_cNameInput.AddKeyEventHandler( this );
	Add( &m_cNameInput );	
	
	// -----------------------------------------------------------------
	SetSprite();
	
	SetARGB( 0xc0ffffff );

#define TEXT_COLOR		D3DCOLOR_XRGB(211,211,211)

	m_cTitleStr.GetFont()->dwColor = TEXT_COLOR;	
	m_cCancelButton.SetLabelColor( TEXT_COLOR );
	m_cOKButton.SetLabelColor( TEXT_COLOR );
	
	m_cSexStr.GetFont()->dwColor = TEXT_COLOR;	
	m_cNameStr.GetFont()->dwColor = TEXT_COLOR;	

	for( i=0; i<SELECT_TYPE_NO; ++i )
	{		
		m_cSelectItem[i].SetItemStrColor( TEXT_COLOR );
	}	

	// -----------------------------------------------------------
	// insert item string	
	m_cSelectItem[Sex].InsertItem( "남자" );
	m_cSelectItem[Sex].InsertItem( "여자" );	

	m_cSelectItem[Hair].InsertItem( "헤어 스타일" );
	m_cSelectItem[Face].InsertItem( "얼굴 생김새" );
	m_cSelectItem[Coat].InsertItem( "기본 상의" );
	m_cSelectItem[Pants].InsertItem( "기본 하의" );
	m_cSelectItem[Glove].InsertItem( "기본 장갑" );
	m_cSelectItem[Shoes].InsertItem( "기본 신발" );
	
	
	///*MAYUN
	m_cNameInput.SetInputLimit(INPUTLEN);
	//*/

	m_cNameInput.Clear();

	SetFocusObject( &m_cNameInput );
	

	m_cOKButton.SetSoundID( SND_ID_LOGIN_OK );
	m_cCancelButton.SetSoundID( SND_ID_LOGIN_CANCEL );
		

	m_bIsCreated = TRUE;
}

void
CGUICreateCharacter::SetSprite( void )
{
	if( !m_bIsCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_LOGIN );

		l_spriteInfo.wActionNo = 4;
		
		l_spriteInfo.wFrameNo = 8;
		SetSpriteInfo( &l_spriteInfo );	

		l_spriteInfo.wFrameNo = 0;
		m_cTitleBase.SetSpriteInfo( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 1;
		m_cLowerBase.SetSpriteInfo( &l_spriteInfo );

		// ok button
		l_spriteInfo.wFrameNo = 2;
		m_cOKButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 3;
		m_cOKButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 4;
		m_cOKButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		
		// cancel button
		l_spriteInfo.wFrameNo = 5;
		m_cCancelButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 6;
		m_cCancelButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 7;
		m_cCancelButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );		

		
		// select button		
		SPRITE_INFO l_spriteButton[2];	
		memcpy( &l_spriteButton[0], &l_spriteInfo, sizeof(SPRITE_INFO) );
		memcpy( &l_spriteButton[1], &l_spriteInfo, sizeof(SPRITE_INFO) );

		l_spriteButton[0].wFrameNo = 9;
		l_spriteButton[1].wFrameNo = 10;
		for( int i=0; i<SELECT_TYPE_NO; ++i )	
			m_cSelectItem[i].SetDownButtonSpriteInfo( l_spriteButton );				
		
		l_spriteButton[0].wFrameNo = 11;
		l_spriteButton[1].wFrameNo = 12;
		for( i=0; i<SELECT_TYPE_NO; ++i )	
			m_cSelectItem[i].SetUpButtonSpriteInfo( l_spriteButton );	
		
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_LOGIN );
		if( !l_pSprite )
			return;	
		
		GetSpriteInfo()->pSprite = l_pSprite;	

		m_cTitleBase.GetSpriteInfo()->pSprite = l_pSprite;			
		m_cLowerBase.GetSpriteInfo()->pSprite = l_pSprite;

		m_cCancelButton.SetSprite( l_pSprite );
		m_cOKButton.SetSprite( l_pSprite );

		
		for( int i=0; i<SELECT_TYPE_NO; ++i )	
			m_cSelectItem[i].SetSprite( l_pSprite );	
	}	
}

void
CGUICreateCharacter::MouseEvent( _mouse_event & event )
{
	if( event.id == MOUSE_CLICKED )
	{
		if( event.source == &m_cOKButton )
		{
			// ################## 코드 추가 시킬 곳 ###########################
			// 캐릭터를 생성한다.
			EventCreateCharacterOK();			
		}
		else if( event.source == &m_cCancelButton )
		{
			// ################## 코드 추가 시킬 곳 ###########################			
			// 성향 선택창으로 돌아간다.
			EventCreateCharacterCancel();			
		}		
	}
}

void
CGUICreateCharacter::KeyEvent( _key_event & event )
{
	if( event.source == &m_cNameInput &&
		event.key == VK_RETURN &&
		strlen( m_cNameInput.GetText() ) > 0 )
	{		
		// 캐릭터를 생성한다.
		EventCreateCharacterOK();	
	}
}

void
CGUICreateCharacter::SelectItemEvent( _select_event & event )
{
	for( int i=0; i<SELECT_TYPE_NO; ++i )
	{
		if( event.source == &m_cSelectItem[i] )
		{
			switch (i)
			{
			case	Sex:
				if ( event.select  == UP_BUTTON) EventLeftSelectSex();
				else EventRightSelectSex();
				break;
			
			case	Hair:
				if ( event.select  == UP_BUTTON) EventLeftSelectHead();
				else EventRightSelectHead();
				
				break;
			case	Face:
				if ( event.select  == UP_BUTTON) EventLeftSelectFace();
				else EventRightSelectFace();
				break;
			case	Coat:
				if ( event.select  == UP_BUTTON) EventLeftSelectUpper();
				else EventRightSelectUpper();
				break;
			case	Pants:
				if ( event.select  == UP_BUTTON) EventLeftSelectLower();
				else EventRightSelectLower();
				break;
			case	Glove:
				if ( event.select  == UP_BUTTON) EventLeftSelectGloves();
				else EventRightSelectGloves();
				break;
			case	Shoes:
				if ( event.select  == UP_BUTTON) EventLeftSelectShoes();
				else EventRightSelectShoes();
				break;			
			}
			// ################## 코드 추가 시킬 곳 ###########################
			// 스타일을 바꿨다.. 
			// ################################################################			
		}
	}
}

LRESULT
CGUICreateCharacter::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !m_listDialogBox.empty() )
	{		
		CGUIDialogBoxList::reverse_iterator i;
		i = m_listDialogBox.rbegin();

		CGUIDialogBox * l_pDlg;
		l_pDlg = *i;
		return l_pDlg->MsgProc( hWnd, uMsg, wParam, lParam );
	}
	else
	{
		if( !IsActive() )
			return 0;

		switch( uMsg )
		{				
		case WM_IME_NOTIFY:	
			// 한자나 특수 문자 입력을 막는다.			
						
			// 특수문자 리스트 창이 떳을 때 바로 닫아버린다.
			if( wParam == IMN_SETCONVERSIONMODE )
			{
				HIMC  hIMC;
				hIMC = ImmGetContext( gInputMgr.m_hWnd );

				// close candidate list window
				ImmNotifyIME( hIMC, NI_CLOSECANDIDATE, 0, 0 );
			}
			
			return 1;

		case WM_CHAR:
			if( wParam == ' ' )			
				return 1;				
			
			break;
		}		


		CGUIContainer::MsgProc( hWnd, uMsg, wParam, lParam );
	}

	return 1;
}


////////////////////////////////////////////////////////////////////////////
//
// CGUISelectAttribute Class Implementation
//
////////////////////////////////////////////////////////////////////////////
CGUISelectAttribute::CGUISelectAttribute()
{
	m_byAttributeType = 0xFF;

	m_byRaceType = 0;

	m_bIsCreated = FALSE;
}

CGUISelectAttribute::~CGUISelectAttribute()
{
	
}

void
CGUISelectAttribute::Init( void )
{
	SetSize( 211, 466 );

	m_cTitleBase.SetSize( 146, 26 );
	m_cLowerBase.SetSize( 50, 37 );

	m_cOKButton.SetSize( 89, 31 );
	m_cCancelButton.SetSize( 89, 31 );	

	for( int i = 0; i < ATTR_TYPE_NUM; ++i )
		m_cAttrButton[i].SetSize( 32, 32 );

	for( i = 0; i < BASIC_POINT_NUM; ++i )
	{
		m_cBasicPointBase[i].SetSize( 106, 8 );
		m_cBasicPointGauge[i].SetSize( 0, 6 );		
	}

	for( i = 0; i < MAKE_SKILL_NUM; ++i )
	{
		m_cMakeSkillBase[i].SetSize( 106, 8 );
		m_cMakeSkillGauge[i].SetSize( 0, 6 );
	}

	for( i = 0; i < BATTLE_SKILL_NUM; ++i )
	{
		m_cBattleSkillBase[i].SetSize( 106, 8 );		
		m_cBattleSkillGauge[i].SetSize( 0, 6 );		
	}


	m_cTitleStr.SetString( "캐릭터 생성" );
	m_cCancelButton.SetLabel( "취 소" );
	m_cOKButton.SetLabel( "확 인" );

	m_cAttStr.SetString( "성 향" );

	m_cGaugeTitle[0].SetString( "기본 능력치" );
	m_cGaugeTitle[1].SetString( "전투기술 보너스" );
	m_cGaugeTitle[2].SetString( "제작기술 보너스" );

	m_cBasicPointStr[0].SetString( "생 명 력(HP)" );
	m_cBasicPointStr[1].SetString( "포 스 력(FP)" );
	m_cBasicPointStr[2].SetString( "스테미너(SP)" );

	m_cBattleSkillStr[0].SetString( "근  접" );
	m_cBattleSkillStr[1].SetString( "원거리" );
	m_cBattleSkillStr[2].SetString( "유  닛" );
	m_cBattleSkillStr[3].SetString( "포  스" );
	m_cBattleSkillStr[4].SetString( "방  패" );
	m_cBattleSkillStr[5].SetString( "방  어" );

	m_cMakeSkillStr[0].SetString( "무기/방패" );
	m_cMakeSkillStr[1].SetString( "방어구" );
	m_cMakeSkillStr[2].SetString( "발사물" );

	// -----------------------------------------------------------------------------------
	m_cTitleBase.SetPos( m_ptPos.x + 32, m_ptPos.y + 4 );
	m_cLowerBase.SetPos( m_ptPos.x + 81, m_ptPos.y + 428 );

	m_cTitleStr.SetPos( m_cTitleBase.m_ptPos.x + (m_cTitleBase.m_ptSize.x - m_cTitleStr.m_ptSize.x)/2,
						m_cTitleBase.m_ptPos.y + 7 );
	m_cOKButton.SetPos( m_ptPos.x + 16, m_ptPos.y + 432 );	
	m_cCancelButton.SetPos( m_ptPos.x + 106, m_ptPos.y + 432 );

	m_cOKButton.SetLabelPos( 29, 11 );
	m_cCancelButton.SetLabelPos( 31, 11 );

	m_cAttStr.SetPos( m_ptPos.x + 87, m_ptPos.y + 35 );

	for( i = 0; i < ATTR_TYPE_NUM; ++i )
		m_cAttrButton[i].SetPos( m_ptPos.x + 33 + ( m_cAttrButton[0].m_ptSize.x + 6 ) * i, m_ptPos.y + 52 );

	m_cDescStr[0].SetPos( m_ptPos.x + 15, m_ptPos.y + 98 );
	m_cDescStr[1].SetPos( m_ptPos.x + 15, m_ptPos.y + 117 );

	m_cGaugeTitle[0].SetPos( m_ptPos.x + 75, m_ptPos.y + 146 );
	m_cGaugeTitle[1].SetPos( m_ptPos.x + 62, m_ptPos.y + 229 );
	m_cGaugeTitle[2].SetPos( m_ptPos.x + 60, m_ptPos.y + 348 );	


	for( i = 0; i < BASIC_POINT_NUM; ++i )
	{
		m_cBasicPointStr[i].SetPos( m_ptPos.x + 16, m_ptPos.y + 171 + (i * 15) );
		m_cBasicPointBase[i].SetPos( m_ptPos.x + 90, m_ptPos.y + 174 + (i * 14) );
		m_cBasicPointGauge[i].SetPos( m_cBasicPointBase[i].m_ptPos.x + 1, m_cBasicPointBase[i].m_ptPos.y + 1 );
	}	

	for( i=0; i<BATTLE_SKILL_NUM; ++i )
	{
		m_cBattleSkillStr[i].SetPos( m_ptPos.x + 16, m_ptPos.y + 250 + (i * 14) );
		m_cBattleSkillBase[i].SetPos( m_ptPos.x + 90, m_ptPos.y + 252 + (i * 14) );
		m_cBattleSkillGauge[i].SetPos( m_cBattleSkillBase[i].m_ptPos.x + 1, m_cBattleSkillBase[i].m_ptPos.y + 1 );
	}

	for( i = 0; i < MAKE_SKILL_NUM; ++i )
	{
		m_cMakeSkillStr[i].SetPos( m_ptPos.x + 16, m_ptPos.y + 370 + (i * 15) );
		m_cMakeSkillBase[i].SetPos( m_ptPos.x + 90, m_ptPos.y + 372 + (i * 14) );
		m_cMakeSkillGauge[i].SetPos( m_cMakeSkillBase[i].m_ptPos.x + 1, m_cMakeSkillBase[i].m_ptPos.y + 1 );
	}

	// -----------------------------------------------------------------------------------

	Add( &m_cTitleBase );
	Add( &m_cTitleStr );

	m_cOKButton.AddMouseEventHandler( this );
	m_cCancelButton.AddMouseEventHandler( this );
	Add( &m_cOKButton );
	Add( &m_cCancelButton );

	Add( &m_cLowerBase );

	Add( &m_cAttStr );

	for( i = 0; i < ATTR_TYPE_NUM; ++i )
	{
		m_cAttrButton[i].AddMouseEventHandler( this );
//		Add( &m_cAttrButton[i] );		
	}

	for( i = 0; i < 3; ++i )
		Add( &m_cGaugeTitle[i] );

	Add( &m_cDescStr[0] );
	Add( &m_cDescStr[1] );

	for( i = 0; i < BASIC_POINT_NUM; ++i )
	{
		Add( &m_cBasicPointStr[i] );
		Add( &m_cBasicPointBase[i] );
		Add( &m_cBasicPointGauge[i] );
	}

	for( i = 0; i < MAKE_SKILL_NUM; ++i )
	{
		Add( &m_cMakeSkillStr[i] );
		Add( &m_cMakeSkillBase[i] );
		Add( &m_cMakeSkillGauge[i] );
	}

	for( i = 0; i < BATTLE_SKILL_NUM; ++i )
	{
		Add( &m_cBattleSkillStr[i] );
		Add( &m_cBattleSkillBase[i] );
		Add( &m_cBattleSkillGauge[i] );
	}

	// -------------------------------------------------------------------------
	SetSprite();

	SetARGB( 0xc0ffffff );

#define TEXT_COLOR		D3DCOLOR_XRGB(211,211,211)
#define TITLE_COLOR		D3DCOLOR_XRGB(244,216,192)

	m_cTitleStr.GetFont()->dwColor = TEXT_COLOR;	
	m_cCancelButton.SetLabelColor( TEXT_COLOR );
	m_cOKButton.SetLabelColor( TEXT_COLOR );	

	for( i = 0; i < 3; ++i )
		m_cGaugeTitle[i].GetFont()->dwColor = TITLE_COLOR;	

	for( i = 0; i < BASIC_POINT_NUM; ++i )
	{
		m_cBasicPointStr[i].GetFont()->dwColor = TEXT_COLOR;	
	}

	for( i = 0; i < MAKE_SKILL_NUM; ++i )
	{
		m_cMakeSkillStr[i].GetFont()->dwColor = TEXT_COLOR;	
	}

	for( i = 0; i < BATTLE_SKILL_NUM; ++i )
	{
		m_cBattleSkillStr[i].GetFont()->dwColor = TEXT_COLOR;			
	}

	m_cDescStr[0].GetFont()->dwColor = D3DCOLOR_XRGB( 180, 218, 225 );
	m_cDescStr[1].GetFont()->dwColor = D3DCOLOR_XRGB( 180, 218, 225 );

	
	// init attribute valaue
	for( int r = 0; r < RACE_TYPE_NUM; ++r )
	{
		for( int att = 0; att < ATTR_TYPE_NUM; ++att )
		{
			for( i = 0; i < 5; ++i )
			{
				if( i < 3 )
				{
					SetBasicPoint( r, att, i, 0.f );
					SetMakeSkill( r, att, i, 0.f );
				}
				
				SetBattleSkill( r, att, i, 0.f );
				SetForcePoint( r, att, i, 0.f );
			}
		}
	}

	// default
//	SetRaceType( 0 );
//	SetAttributeTab( 0 );

	for( i = 0; i < ATTR_TYPE_NUM; ++i )
	{
		m_cAttrButton[i].SetSoundID( SND_ID_LOGIN_SELECT );
	}

	m_cOKButton.SetSoundID( SND_ID_LOGIN_OK );
	m_cCancelButton.SetSoundID( SND_ID_LOGIN_CANCEL );

	m_bIsCreated = TRUE;
}

void
CGUISelectAttribute::SetSprite( void )
{
	if( !m_bIsCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_LOGIN );

		l_spriteInfo.wActionNo = 4;
		
		l_spriteInfo.wFrameNo = 13;
		SetSpriteInfo( &l_spriteInfo );	

		l_spriteInfo.wFrameNo = 0;
		m_cTitleBase.SetSpriteInfo( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 1;
		m_cLowerBase.SetSpriteInfo( &l_spriteInfo );

		// ok button
		l_spriteInfo.wFrameNo = 2;
		m_cOKButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 3;
		m_cOKButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 4;
		m_cOKButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		
		// cancel button
		l_spriteInfo.wFrameNo = 5;
		m_cCancelButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 6;
		m_cCancelButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 7;
		m_cCancelButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );		
		
		
		for( int i=0; i<ATTR_TYPE_NUM; ++i )
		{
			l_spriteInfo.wFrameNo = 14 + i * 3;
			m_cAttrButton[i].SetSpriteInfoEx( &l_spriteInfo );
			l_spriteInfo.wFrameNo = 14 + i * 3 + 1;
			m_cAttrButton[i].SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		}		

		for( i=0; i<BASIC_POINT_NUM; ++i )
		{
			l_spriteInfo.wFrameNo = 27 + i;
			m_cBasicPointGauge[i].SetSpriteInfo( &l_spriteInfo ); 

			l_spriteInfo.wFrameNo = 26;
			m_cBasicPointBase[i].SetSpriteInfo( &l_spriteInfo );

			m_cBasicPointGauge[i].SetSpriteV( 0.f, 1.f );
		}

		for( i=0; i<MAKE_SKILL_NUM; ++i )
		{
			l_spriteInfo.wFrameNo = 30;
			m_cMakeSkillGauge[i].SetSpriteInfo( &l_spriteInfo ); 

			l_spriteInfo.wFrameNo = 26;			
			m_cMakeSkillBase[i].SetSpriteInfo( &l_spriteInfo ); 
			
			m_cMakeSkillGauge[i].SetSpriteV( 0.f, 1.f );			
		}
		
		for( i=0; i<BATTLE_SKILL_NUM; ++i )
		{
			l_spriteInfo.wFrameNo = 30;

			m_cBattleSkillGauge[i].SetSpriteInfo( &l_spriteInfo ); 
			l_spriteInfo.wFrameNo = 26;

			m_cBattleSkillBase[i].SetSpriteInfo( &l_spriteInfo ); 
			
			m_cBattleSkillGauge[i].SetSpriteV( 0.f, 1.f );
		}		
		
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_LOGIN );
		if( !l_pSprite )
			return;	
		
		GetSpriteInfo()->pSprite = l_pSprite;	

		m_cTitleBase.GetSpriteInfo()->pSprite = l_pSprite;			
		m_cLowerBase.GetSpriteInfo()->pSprite = l_pSprite;

		m_cCancelButton.SetSprite( l_pSprite );
		m_cOKButton.SetSprite( l_pSprite );
		

		for( int i=0; i<ATTR_TYPE_NUM; ++i )		
			m_cAttrButton[i].SetSprite( l_pSprite );

		for( i=0; i<BASIC_POINT_NUM; ++i )
		{			
			m_cBasicPointGauge[i].GetSpriteInfo()->pSprite = l_pSprite;						
			m_cBasicPointBase[i].GetSpriteInfo()->pSprite = l_pSprite;			
		}

		for( i=0; i<MAKE_SKILL_NUM; ++i )
		{
			m_cMakeSkillGauge[i].GetSpriteInfo()->pSprite = l_pSprite;			
			m_cMakeSkillBase[i].GetSpriteInfo()->pSprite = l_pSprite;
		}
		
		for( i=0; i<BATTLE_SKILL_NUM; ++i )
		{
			m_cBattleSkillGauge[i].GetSpriteInfo()->pSprite = l_pSprite;
			m_cBattleSkillBase[i].GetSpriteInfo()->pSprite = l_pSprite;
		}

	}
}

void
CGUISelectAttribute::SetRaceType( BYTE pi_byRaceType )
{
	// 아크레시아이면 포스탭을 disable한다.
	if( pi_byRaceType == CRT_ACCRETIA ) 
	{
		m_cAttrButton[ATTR_TYPE_FORCE].Disable();
		m_cAttrButton[ATTR_TYPE_FORCE].SetARGB( 0xa01B1B1B );

		Remove( &m_cAttrButton[ATTR_TYPE_FORCE] );
		
		m_cAttrButton[ATTR_TYPE_MELEE].SetPos( m_ptPos.x + 51, m_ptPos.y + 52 );
		m_cAttrButton[ATTR_TYPE_MISSILE].SetPos( m_ptPos.x + 89, m_ptPos.y + 52 );
		m_cAttrButton[ATTR_TYPE_KNOWLEDGE].SetPos( m_ptPos.x + 127, m_ptPos.y + 52 );
		
		m_cAttrButton[ATTR_TYPE_MELEE].Show( IsShow() );
		m_cAttrButton[ATTR_TYPE_MISSILE].Show( IsShow() );
		m_cAttrButton[ATTR_TYPE_KNOWLEDGE].Show( IsShow() );

		Add( &m_cAttrButton[ATTR_TYPE_MELEE] );
		Add( &m_cAttrButton[ATTR_TYPE_MISSILE] );
		Add( &m_cAttrButton[ATTR_TYPE_KNOWLEDGE] );		
	}
	else
	{
		for( int i=0; i<ATTR_TYPE_NUM; ++i )
		{
			m_cAttrButton[i].SetPos( m_ptPos.x + 33 + ( m_cAttrButton[0].m_ptSize.x + 6 ) * i, m_ptPos.y + 52 );
			m_cAttrButton[i].Show( IsShow() );
			Add( &m_cAttrButton[i] );
		}

		m_cAttrButton[ATTR_TYPE_FORCE].Enable();
		m_cAttrButton[ATTR_TYPE_FORCE].SetARGB( 0xFFFFFFFF );
	}

	// set race type
	m_byRaceType = pi_byRaceType / 2; // 세가지 타입으로 나눈다.

	if( m_byRaceType > RT_ACCRETIA )
		m_byRaceType = RT_BELLATO;

	char l_pRaceSkillStr[3][8] = { "유  닛", "소  환", "런  처" };

	m_cBattleSkillStr[2].SetString( l_pRaceSkillStr[m_byRaceType] );

	// update attribute tab
	SetAttributeTab( 1 );
	SetAttributeTab( 0 );
}


static char s_pAttributeDesc[8][128] = 
{
	"용기는 전사들이 가진",
	"또 다른 심장!",

	"시선이 멈출 때마다",
	"하나의 생명이 소멸한다.",

	"Deus, Baptizare Ego Honor!",
	"신이여, 나의 명예를 지켜주소서!",

	"생각하라! 그것이",
	"우리를 자유롭게 할 것이다."
};	

void
CGUISelectAttribute::SetAttributeTab( BYTE pi_byTabIndex )
{
	if( m_byAttributeType == pi_byTabIndex )
		return;

	BYTE l_byPrevActiveTab = m_byAttributeType;

	// set attrtype
	m_byAttributeType = pi_byTabIndex;

	// button sprite setting
	SPRITE_INFO		l_spriteInfo;
	memcpy( &l_spriteInfo, m_cAttrButton[0].GetSpriteInfo(), sizeof( SPRITE_INFO ) );
	if( l_byPrevActiveTab >= 0 && l_byPrevActiveTab <= 4 )
	{
		l_spriteInfo.wFrameNo = 14 + l_byPrevActiveTab * 3;
		m_cAttrButton[l_byPrevActiveTab].SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 14 + l_byPrevActiveTab * 3 + 1;
		m_cAttrButton[l_byPrevActiveTab].SetSpriteInfoEx( PRESSED, &l_spriteInfo );
	}

	l_spriteInfo.wFrameNo = 14 + m_byAttributeType * 3 + 2;
	m_cAttrButton[m_byAttributeType].SetSpriteInfoEx( &l_spriteInfo );


	// set description
	m_cDescStr[0].SetString( s_pAttributeDesc[pi_byTabIndex*2] );	
	m_cDescStr[1].SetString( s_pAttributeDesc[pi_byTabIndex*2+1] );	

	// 가운데 정렬
	m_cDescStr[0].SetPos( m_ptPos.x + (m_ptSize.x - m_cDescStr[0].m_ptSize.x)/2, m_cDescStr[0].m_ptPos.y );
	m_cDescStr[1].SetPos( m_ptPos.x + (m_ptSize.x - m_cDescStr[1].m_ptSize.x)/2, m_cDescStr[1].m_ptPos.y );

	// -------------------------------------------------------------
	// gauge setting
	for( int i=0; i<BASIC_POINT_NUM; ++i )
	{
		m_cBasicPointGauge[i].SetSize( m_fBasicPoint[m_byRaceType][m_byAttributeType][i] * (m_cBasicPointBase[i].m_ptSize.x - 2),
												m_cBasicPointGauge[i].m_ptSize.y );
		m_cBasicPointGauge[i].SetSpriteU( 0.f, m_fBasicPoint[m_byRaceType][m_byAttributeType][i] );
	}

	for( i=0; i<MAKE_SKILL_NUM; ++i )
	{
		m_cMakeSkillGauge[i].SetSize( m_fMakeSkillPoint[m_byRaceType][m_byAttributeType][i] * (m_cMakeSkillBase[i].m_ptSize.x - 2),
												m_cMakeSkillGauge[i].m_ptSize.y );
		m_cMakeSkillGauge[i].SetSpriteU( 0.f, m_fMakeSkillPoint[m_byRaceType][m_byAttributeType][i] );
	}

	for( i=0; i<BATTLE_SKILL_NUM; ++i )
	{
		m_cBattleSkillGauge[i].SetSize( m_fBattleSkillPoint[m_byRaceType][m_byAttributeType][i] * (m_cBattleSkillBase[i].m_ptSize.x - 2),
												m_cBattleSkillGauge[i].m_ptSize.y );
		m_cBattleSkillGauge[i].SetSpriteU( 0.f, m_fBattleSkillPoint[m_byRaceType][m_byAttributeType][i] );		
	}

}

void
CGUISelectAttribute::SetBasicPoint( BYTE pi_byRaceType, BYTE pi_byAttrType, BYTE pi_byIndex, float pi_fPoint )
{
	if( pi_byAttrType >= ATTR_TYPE_NUM || pi_byRaceType >= RACE_TYPE_NUM )
		return;	

	if( pi_byIndex >= BASIC_POINT_NUM || pi_fPoint < 0.f || pi_fPoint > 1.f )
		return;

	m_fBasicPoint[pi_byRaceType][pi_byAttrType][pi_byIndex] = pi_fPoint;
}

void
CGUISelectAttribute::SetBattleSkill( BYTE pi_byRaceType, BYTE pi_byAttrType, BYTE pi_byIndex, float pi_fPoint )
{
	if( pi_byAttrType >= ATTR_TYPE_NUM || pi_byRaceType >= RACE_TYPE_NUM )
		return;	

	if( pi_byIndex >= BATTLE_SKILL_NUM || pi_fPoint < 0.f || pi_fPoint > 1.f )
		return;

	m_fBattleSkillPoint[pi_byRaceType][pi_byAttrType][pi_byIndex] = pi_fPoint;	
}

void
CGUISelectAttribute::SetMakeSkill( BYTE pi_byRaceType, BYTE pi_byAttrType, BYTE pi_byIndex, float pi_fPoint )
{
	if( pi_byAttrType >= ATTR_TYPE_NUM || pi_byRaceType >= RACE_TYPE_NUM )
		return;	

	if( pi_byIndex >= MAKE_SKILL_NUM || pi_fPoint < 0.f || pi_fPoint > 1.f )
		return;

	m_fMakeSkillPoint[pi_byRaceType][pi_byAttrType][pi_byIndex] = pi_fPoint;
}

void
CGUISelectAttribute::SetForcePoint( BYTE pi_byRaceType, BYTE pi_byAttrType, BYTE pi_byIndex, float pi_fPoint )
{
/*
	if( pi_byAttrType >= FORCE_NUM || pi_byRaceType >= RACE_TYPE_NUM )
		return;	

	if( pi_byIndex > 4 || pi_fPoint < 0.f || pi_fPoint > 1.f )
		return;

	m_fForcePoint[pi_byRaceType][pi_byAttrType][pi_byIndex] = pi_fPoint;	
*/
}

void
CGUISelectAttribute::MouseEvent( _mouse_event & event )
{
	CGUIWindow::MouseEvent( event );	

	if( event.id == MOUSE_CLICKED )
	{
		if( event.source == &m_cOKButton )
		{			
			// 캐릭터 생성창으로
			EventSelectAttribute(m_byRaceType,m_byAttributeType);
			EventSelectAttributeOK();
		}
		else if( event.source == &m_cCancelButton )
		{
			// ################## 코드 추가 시킬 곳 ###########################
			// 취소
			EventSelectAttributeCancel();
		}				
		else
		{
			for( int i=0; i<ATTR_TYPE_NUM; ++i )
			{
				if( event.source == &m_cAttrButton[i] )
				{
					// set tab
					SetAttributeTab( i );

					return;
				}
			}
		}
	}	
}