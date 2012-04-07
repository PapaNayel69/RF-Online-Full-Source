#ifndef __INPUT_PROCESS_MGR_H__
#define __INPUT_PROCESS_MGR_H__

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define MAX_KEY_MAP		( 100 )

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CInputProcessMgr
{
	BOOL	m_bShiftKeyDown;
	BOOL	m_bControlKeyDown;
	BOOL	m_bAltKeyDown;

	DWORD	m_dwPressedAltKeyTime;

	DWORD	m_listKey[MAX_KEY_MAP][2];
	DWORD	m_dwTotalKeyNum;

	BOOL	m_bIsPressedKey;

public :

	CInputProcessMgr();
	~CInputProcessMgr();

	BOOL	LoadData( void );
	void	SetMenuHotKeyString( void );

	BOOL	InputProcess_Keyboard( void );

	LRESULT	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

protected :

	void	IntHotKeyToStrHotKey( char * po_strHotKey, DWORD * pi_pKey );
	void	IntNormalKeyToStrNormalKey( char * po_strKey, DWORD pi_pKey );

	BOOL	GetHotKeyFromString( DWORD * po_pKeyMap, char * pi_pOrgString );
	BOOL	GetHotKeyFromSplitedStringKey( DWORD * po_pKeyMap, char * pi_pSystemKeyString, char * pi_pNormalKeyString );
	BOOL	SplitOrgKeyString( char * po_pSystemKeyString, char * po_pNormalKeyString, char * pi_pOrgString );

	//--------------------------------------------------------------------------

	BOOL	IsPressedSystemKey( DWORD pi_dwSystemKey );
	BOOL	IsPressedKey( DWORD pi_dwKey );

	BOOL	InputProcess_Keyboard_CancelTarget( void );
	BOOL	InputProcess_Keyboard_ChangeChatMode( void );
	BOOL	InputProcess_Keyboard_ScreenShot( void );
	BOOL	InputProcess_Keyboard_OpenInventoryWindow( void );
	BOOL	InputProcess_Keyboard_OpenForceWindow( void );
	BOOL	InputProcess_Keyboard_OpenSkillWindow( void );
	BOOL	InputProcess_Keyboard_OpenAvatarInfoWindow( void );
	BOOL	InputProcess_Keyboard_OpenPartyWindow( void );
	BOOL	InputProcess_Keyboard_OpenAnimusWindow( void );
	BOOL	InputProcess_Keyboard_OpenSelectAttackPartWindow( void );
	BOOL	InputProcess_Keyboard_OpenMinimapWindow( void );
	BOOL	InputProcess_Keyboard_OpenRebornWindow( void );
	BOOL	InputProcess_Keyboard_ChangeCombatMode( void );
	BOOL	InputProcess_Keyboard_ChangeMoveMode( void );
	BOOL	InputProcess_Keyboard_ChangeAutoAttackMode( void );
	BOOL	InputProcess_Keyboard_ItemLooting( void );
	BOOL	InputProcess_Keyboard_Attack( void );
	BOOL	InputProcess_Keyboard_ChangeTargetFromLatestTarget( void );
	BOOL	InputProcess_Keyboard_AvatarMoveByKeyboard( void );
	BOOL	InputProcess_Keyboard_DisplayPrevSendChatMsg( void );
	BOOL	InputProcess_Keyboard_DisplayPrevWhisperCharName( void );
	BOOL	InputProcess_Keyboard_Logout( void );
	BOOL	InputProcess_Keyboard_ChangeToFreeTargetMode( void );
	BOOL	InputProcess_Keyboard_ChangeToAutoMouseMode( void );
	BOOL	InputProcess_Keyboard_ChangeToAutoTargetMode( void );
	BOOL	InputProcess_Keyboard_RenderShadow( void );
	BOOL	InputProcess_Keyboard_OpenOptionWindow( void );
	BOOL	InputProcess_Keyboard_HideUI( void );
	BOOL	InputProcess_Keyboard_ChangeToObserver( void );
	BOOL	InputProcess_Keyboard_ChangeObserverMode( void );
	BOOL	InputProcess_Keyboard_PlayRealtimeMovie( void );
	BOOL	InputProcess_Keyboard_ChangeRealtimeWideMode( void );
	BOOL	InputProcess_Keyboard_ChangeRealtimeCameraMode( void );

	//--------------------------------------------------------------------------

	BOOL	InputProcess_Keyboard_Etc( void );
	BOOL	InputProcess_Keyboard_Etc_WithShift();
	BOOL	InputProcess_Keyboard_Etc_SelectTargetByRotate( void );
	BOOL	InputProcess_Keyboard_Etc_WithAlt( void );
	BOOL	InputProcess_Keyboard_Etc_WithControl( void );
	void	InputProcess_Keyboard_Etc_SetTarget( void );
	void	InputProcess_Keyboard_Etc_DisplayInfo( void );
	BOOL	InputProcess_Keyboard_Etc_Normal( void );
	void	InputProcess_Keyboard_Etc_SelectTarget( void );
	BOOL	InputProcess_Keyboard_Etc_ChangeChatModeByEnter( void );
	BOOL	InputProcess_Keyboard_Etc_WithControlAndNormal( void );

public :

	BOOL	IsMoveKey( DWORD pi_lParam );
	BOOL	IsPressedMoveKey( void );
	void	IsPressedMoveKey( BOOL & pi_bLeftKey, BOOL & pi_bUpKey, BOOL & pi_bRightKey, BOOL & pi_bDownKey );
};

#endif // __INPUT_PROCESS_MGR_H__