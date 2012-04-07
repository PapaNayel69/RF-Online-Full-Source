#ifndef __DEFINE_MESSAGE_H__
#define __DEFINE_MESSAGE_H__

union UI_MOUSE_MSG	// ( sizeof( UI_MOUSE_MSG ) == UINT )
{
	DWORD		m_dwData;

	struct
	{
		BYTE	m_byPackNo;
		BYTE	m_bySocketNo;
		BYTE	m_byQuantity;
		BYTE	m_byExtra;
	};

	UI_MOUSE_MSG()
	{
		m_dwData		= 0;
	}

	UI_MOUSE_MSG( DWORD pi_dwData )
	{
		m_dwData		= pi_dwData;
	}

	UI_MOUSE_MSG( BYTE pi_byPackNo, BYTE pi_bySocketNo, BYTE pi_byQuantity, BYTE pi_byExtra )
	{
		m_byPackNo		= pi_byPackNo;
		m_bySocketNo	= pi_bySocketNo;
		m_byQuantity	= pi_byQuantity;
		m_byExtra		= pi_byExtra;
	}
};

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

#define WM_EVENTWND_MOUSE_HOVER						( WM_USER + 0x0000 )
#define WM_EVENTWND_MOUSE_CLICK						( WM_USER + 0x0001 )
#define WM_EVENTWND_CLICK_OK						( WM_USER + 0x0002 )
#define WM_EVENTWND_CLICK_CANCEL					( WM_USER + 0x0003 )

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

#define WM_PARTY_INVITE_OTHER_PLAYER				( WM_USER + 0x0010 )
#define WM_PARTY_JOIN_OTHER_PARTY					( WM_USER + 0x0011 )
#define	WM_PARTY_ACCEPT_INVITE						( WM_USER + 0x0012 )
#define	WM_PARTY_ACCEPT_JOIN						( WM_USER + 0x0013 )
#define	WM_PARTY_CHANGE_LEADER						( WM_USER + 0x0014 )
#define	WM_PARTY_BANISH_PLAYER						( WM_USER + 0x0016 )
#define	WM_PARTY_LEAVE_PARTY						( WM_USER + 0x0017 )
#define	WM_PARTY_LOCKON_ADDMEMBER					( WM_USER + 0x0018 )
#define	WM_PARTY_SELECT_MEMBER						( WM_USER + 0x001F )

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

#define WM_ACCEPT_TRADE_REQUEST						( WM_USER + 0x0020 )

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

#define	WM_ENABLE_CLASS_CHANGE						( WM_USER + 0x0040 )

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

#define	WM_OPEN_WINDOW								( WM_USER + 0x0E00 )
#define	WM_CLOSE_WINDOW								( WM_USER + 0x0E01 )
#define	WM_CLOSING_WINDOW							( WM_USER + 0x0E02 )
#define	WM_OPEN_TOOLTIP								( WM_USER + 0x0E03 )
#define	WM_OPEN_WINDOW_HELP							( WM_USER + 0x0E04 )
#define	WM_OPEN_WINDOW_HELP_TOOLTIP					( WM_USER + 0x0E05 )

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

#define	WM_BOARD_CLICK_OBJECT						( WM_USER + 0x0100 )
#define	WM_BOARD_HOVER_OBJECT						( WM_USER + 0x0101 )
#define	WM_BOARD_ADD_OBJECT							( WM_USER + 0x0102 )
#define	WM_BOARD_REMOVE_OBJECT						( WM_USER + 0x0103 )
#define	WM_BOARD_CHECK_REMOVE_OBJECT				( WM_USER + 0x0104 )
#define	WM_BOARD_OK									( WM_USER + 0x0105 )
#define	WM_BOARD_CANCEL								( WM_USER + 0x0106 )
#define	WM_BOARD_USE_OBJECT							( WM_USER + 0x0107 )
#define	WM_BOARD_DROP_OBJECT						( WM_USER + 0x0108 )
#define	WM_BOARD_OPEN_PACK							( WM_USER + 0x0109 )

#define	WM_BOARD_EQUIP_HOT_KEY						( WM_USER + 0x0110 )
#define	WM_BOARD_TRADE_HOT_KEY						( WM_USER + 0x0111 )

#define	WM_BOARD_TRADE								( WM_USER + 0x0112 )
#define	WM_BOARD_CANCEL_TRADE						( WM_USER + 0x0113 )
#define	WM_BOARD_CHANGE_TRADE_MODE					( WM_USER + 0x0114 )

#define	WM_BOARD_OPEN_SHOP_WINDOW					( WM_USER + 0x0115 )

//==============================================================================

#define	WM_PLAYER_TRADE_REQUEST						( WM_USER +	0x0120 )

#define	WM_PLAYER_TRADE_BOARD_LOCK					( WM_USER + 0x0121 )
#define	WM_PLAYER_TRADE_BOARD_ACCEPT				( WM_USER + 0x0122 )
#define	WM_PLAYER_TRADE_BOARD_CANCEL				( WM_USER + 0x0123 )

//==============================================================================

#define	WM_ANIMUSCONTROL_BOARD_COMMAND				( WM_USER + 0x0130 )

#define	WM_TUNING_BOARD_REPAIR						( WM_USER + 0x0140 )
#define	WM_TUNING_BOARD_TUNING						( WM_USER + 0x0141 )
#define	WM_TUNING_BOARD_CANCEL						( WM_USER + 0x0142 )
#define	WM_TUNING_BOARD_MAINFRAME					( WM_USER + 0x0143 )

//==============================================================================

#define	WM_ORE_PROC_BOARD_PROCESS					( WM_USER + 0x0150 )
#define	WM_ORE_PROC_BOARD_SELL						( WM_USER + 0x0151 )

//==============================================================================

#define	WM_ITEM_WORK_BOARD_PROCESS					( WM_USER + 0x0160 )
#define	WM_ITEM_WORK_BOARD_CANCEL					( WM_USER + 0x0161 )

//==============================================================================

#define	WM_ITEM_UPGRADE_BOARD_PROCESS				( WM_USER + 0x0170 )
#define	WM_ITEM_UPGRADE_BOARD_CANCEL				( WM_USER + 0x0171 )

//==============================================================================

#define	WM_MINING_BOARD_SELECT_ORE					( WM_USER + 0x0180 )
#define	WM_MINING_BOARD_MINING						( WM_USER + 0x0181 )

//==============================================================================

#define	WM_EXCHANGE_BOARD_EXCHANGE					( WM_USER + 0x0190 )
#define	WM_EXCHANGE_BOARD_EXCHANGE_CHECK			( WM_USER + 0x0191 )
#define	WM_EXCHANGE_BOARD_CANCEL					( WM_USER + 0x0192 )


//============================================================================//
//                                  Boundary                                  //
//============================================================================//

#define	WM_OPTION_TOGGLE_HINT						( WM_USER + 0x0200 )
#define	WM_OPTION_TOGGLE_AUTO_VF_ADJUST				( WM_USER + 0x0201 )
#define	WM_OPTION_CHANGE_VF_DIST					( WM_USER + 0x0202 )
#define	WM_OPTION_DETAIL_TEXTURE					( WM_USER + 0x0203 )

#define	WM_OPTION_CHANGE_CAMERA_MODE				( WM_USER + 0x0204 )

#define	WM_OPTION_ADJUST_EFFECT_SOUND				( WM_USER + 0x0205 )
#define	WM_OPTION_ADJUST_BGM_SOUND					( WM_USER + 0x0206 )

#define	WM_OPTION_ADJUST_GAMMA						( WM_USER + 0x0207 )
#define	WM_OPTION_ADJUST_DYNAMIC_LIGHTING			( WM_USER + 0x0208 )
#define	WM_OPTION_ADJUST_SHADOW						( WM_USER + 0x0209 )

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

#define	WM_EXIT_KEYBOARD_MACRO						( WM_USER + 0x0210 )

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

#define	WM_AVATAR_TOGGLE_COMBAT_MODE				( WM_USER + 0x0F00 )
#define	WM_AVATAR_TOGGLE_MOVE_MODE					( WM_USER + 0x0F01 )
#define	WM_AVATAR_TOGGLE_AUTO_ATTACK				( WM_USER + 0x0F02 )

#define	WM_AVATAR_UNIT_DELIVERY						( WM_USER + 0x0F10 )
#define	WM_AVATAR_UNIT_KEEPING						( WM_USER + 0x0F11 )
#define	WM_AVATAR_UNIT_MOUNT						( WM_USER + 0x0F12 )
#define	WM_AVATAR_UNIT_DISMOUNT						( WM_USER + 0x0F13 )
#define	WM_AVATAR_UNIT_TRACTION						( WM_USER + 0x0F14 )
#define	WM_AVATAR_UNIT_SELECT_WEAPON				( WM_USER + 0x0F15 )
#define	WM_AVATAR_GUARDTOWER_TAKEIN					( WM_USER + 0x0F16 )

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

#endif // __DEFINE_MESSAGE_H__