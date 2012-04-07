////////////////////////////////////////////////////////////////////////////
//
// CGP_LogIn Class Implementation
//
//////////////////////////////////////////////////CHARACTERDRAW//////////////////////////

#pragma comment ( lib, "ziplibm.lib" )
#pragma comment ( lib, "ZipArchive_STL_M.lib" )
#pragma comment ( lib, "Wininet.lib" )

//
char* CheckFiles();
//
//--------------------------------------------------------------------------

#include "../GameObject/Manager/CCharacterMgr.h"
#include "../GameObject/Manager/CGroundItemManager.h"
#include "../CLand.h"
#include "../CCamera.h"

#include "../Input/IMEEdit.h"

#include "../DataTable/CResourceDataMgr.h"

//
#include "R3Sound.h"
//
#include "../DataFile.h"
#include "../Global.h"
#include "../UserInterface/GUIController/GUIScreen.h"
#include "../UserInterface/LogInWindow/GUILoginWindow.h"
#include "../UserInterface/LogInWindow/GUILoadingWindow.h"
#include "../UserInterface/LogInWindow/GUISelectCharacter.h"
#include "../UserInterface/LogInWindow/GUISelectRace.h"
#include "../UserInterface/LogInWindow/GUICreateCharacter.h"
#include "../UserInterface/GUIController/GUIMessagebox.h"
#include "../UserInterface/GameWindow/EtcWin/GUIMapMoveWindow.h"
#include "../UserInterface/SpriteMgr.h"
#include "../UserInterface/UIGlobal.h"
#include "../AddCodeByYun/ParticleList.h"

//--------------------------------------------------------------------------

#include <windows.h>
#include <process.h>
#include <direct.h>

#include "CGP_LogIn.h"
#include "../MainApp.h"

//---------------------------------------------------------------------------
// UI
CGUILoginWindow			m_uiLoginWindow;
CGUILoadingWindow		m_uiLoadingWindow;
CGUISelectCharacter		m_uiSelectCharacter;
CGUISelectRace			m_uiSelectRace;
CGUICreateCharacter		m_uiCreateCharacter;
CGUISelectAttribute		m_uiSelectAttribute;
CGUIMapMoveWindow		m_uiWorldSelectWindow;

enum	MoveWindowType
{
	SELECT_CHAR_WIN,
	SELECT_RACE_WIN,
	CREATE_CHAR_WIN,
	SELECT_ATTR_WIN,
	NONE_WIN
};

MoveWindowType	g_eMoveWinType = NONE_WIN;	
BOOL			g_bClosingWindow = FALSE;
BOOL			g_bUIDefaultStart = FALSE;

char			m_gClassTypeString[8];

void	UI_SetMoveWindow( MoveWindowType pi_eWinType, BOOL pi_bIsOpening )
{
	switch( pi_eWinType )
	{
	case SELECT_CHAR_WIN:
		if( pi_bIsOpening )
		{
			g_cMoveObjectMgr.AddMoveObject( m_uiSelectCharacter.GetLowerBoard(), MOVE_AXIS_Y,
											_GetUIScreen()->m_ptSize.y,
											_GetUIScreen()->m_ptSize.y - m_uiSelectCharacter.GetLowerBoard()->m_ptSize.y - 10, 500 );

			g_cMoveObjectMgr.AddMoveObject( m_uiSelectCharacter.GetUpperBoard(), MOVE_AXIS_Y, 
											_GetUIScreen()->m_ptPos.y - m_uiSelectCharacter.GetUpperBoard()->m_ptSize.y,
											_GetUIScreen()->m_ptPos.y + 10, 500 );							
			
		}
		else
		{
			g_cMoveObjectMgr.AddMoveObject( m_uiSelectCharacter.GetLowerBoard(), MOVE_AXIS_Y,
											_GetUIScreen()->m_ptSize.y - m_uiSelectCharacter.GetLowerBoard()->m_ptSize.y - 10,
											_GetUIScreen()->m_ptSize.y,
											300 );			
						   
			g_cMoveObjectMgr.AddMoveObject( m_uiSelectCharacter.GetUpperBoard(), MOVE_AXIS_Y,
											_GetUIScreen()->m_ptPos.y + 10,
											_GetUIScreen()->m_ptPos.y - m_uiSelectCharacter.GetUpperBoard()->m_ptSize.y,
											300 );			
		}
		
		g_cMoveObjectMgr.StartMove( m_uiSelectCharacter.GetLowerBoard() );		
		g_cMoveObjectMgr.StartMove( m_uiSelectCharacter.GetUpperBoard() );

		break;

	case SELECT_RACE_WIN:
		if( pi_bIsOpening )
		{
			g_cMoveObjectMgr.AddMoveObject( m_uiSelectRace.GetLowerBoard(), MOVE_AXIS_Y,
						   _GetUIScreen()->m_ptSize.y,
						   _GetUIScreen()->m_ptSize.y - m_uiSelectRace.GetLowerBoard()->m_ptSize.y - 10, 500 );
			g_cMoveObjectMgr.AddMoveObject( m_uiSelectRace.GetUpperBoard(), MOVE_AXIS_Y, 
						   _GetUIScreen()->m_ptPos.y - m_uiSelectRace.GetUpperBoard()->m_ptSize.y,
						   _GetUIScreen()->m_ptPos.y + 10, 500 );
		}
		else
		{
			g_cMoveObjectMgr.AddMoveObject( m_uiSelectRace.GetLowerBoard(), MOVE_AXIS_Y,
						   _GetUIScreen()->m_ptSize.y - m_uiSelectRace.GetLowerBoard()->m_ptSize.y - 10,
						   _GetUIScreen()->m_ptSize.y,
						   300 );
			g_cMoveObjectMgr.AddMoveObject( m_uiSelectRace.GetUpperBoard(), MOVE_AXIS_Y, 
						   _GetUIScreen()->m_ptPos.y + 10,
						   _GetUIScreen()->m_ptPos.y - m_uiSelectRace.GetUpperBoard()->m_ptSize.y,
						   300 );
		}
		
		g_cMoveObjectMgr.StartMove( m_uiSelectRace.GetLowerBoard() );
		g_cMoveObjectMgr.StartMove( m_uiSelectRace.GetUpperBoard() );

		break;

	case CREATE_CHAR_WIN:
		if( pi_bIsOpening )
		{
			g_cMoveObjectMgr.AddMoveObject( &m_uiCreateCharacter, MOVE_AXIS_X,
											_GetUIScreen()->m_ptSize.x,
											_GetUIScreen()->m_ptSize.x - m_uiCreateCharacter.m_ptSize.x - 30,
											500 );
		}
		else
		{
			g_cMoveObjectMgr.AddMoveObject( &m_uiCreateCharacter, MOVE_AXIS_X,
											_GetUIScreen()->m_ptSize.x - m_uiCreateCharacter.m_ptSize.x - 30,
											_GetUIScreen()->m_ptSize.x,
											300 );	
		}

		g_cMoveObjectMgr.StartMove( &m_uiCreateCharacter );
			
		break;

	case SELECT_ATTR_WIN:
		if( pi_bIsOpening )
		{
			g_cMoveObjectMgr.AddMoveObject( &m_uiSelectAttribute, MOVE_AXIS_X,
											_GetUIScreen()->m_ptSize.x,
											_GetUIScreen()->m_ptSize.x - m_uiSelectAttribute.m_ptSize.x - 30,
											500 );
		}
		else
		{
			g_cMoveObjectMgr.AddMoveObject( &m_uiSelectAttribute, MOVE_AXIS_X,
											_GetUIScreen()->m_ptSize.x - m_uiSelectAttribute.m_ptSize.x - 30,
											_GetUIScreen()->m_ptSize.x,
											300 );	
		}

		g_cMoveObjectMgr.StartMove( &m_uiSelectAttribute );

		break;
	}

	g_eMoveWinType = pi_eWinType;

	if( pi_bIsOpening )
		g_bClosingWindow = FALSE;
	else
		g_bClosingWindow = TRUE;
}

void	UI_ClosingWindow( void )
{

	switch( g_eMoveWinType )
	{
	case SELECT_CHAR_WIN:
		if( g_cMoveObjectMgr.IsFinishedMove( m_uiSelectCharacter.GetLowerBoard() ) )
		{
			g_eMoveWinType = NONE_WIN;

			if( g_bClosingWindow )
			{
				m_uiSelectCharacter.Close();
				g_bClosingWindow = FALSE;
			}
		}
		break;
	case SELECT_RACE_WIN:
		if( g_cMoveObjectMgr.IsFinishedMove( m_uiSelectRace.GetLowerBoard() ) )
		{
			g_eMoveWinType = NONE_WIN;

			if( g_bClosingWindow )
			{
				m_uiSelectRace.Close();
				g_bClosingWindow = FALSE;
			}
		}
		break;
	case CREATE_CHAR_WIN:
		if( g_cMoveObjectMgr.IsFinishedMove( &m_uiCreateCharacter ) )
		{
			g_eMoveWinType = NONE_WIN;

			if( g_bClosingWindow )
			{
				m_uiCreateCharacter.Close();
				g_bClosingWindow = FALSE;
			}
		}
		break;
	case SELECT_ATTR_WIN:
		if( g_cMoveObjectMgr.IsFinishedMove( &m_uiSelectAttribute ) )
		{
			g_eMoveWinType = NONE_WIN;

			if( g_bClosingWindow )
			{
				m_uiSelectAttribute.Close();
				g_bClosingWindow = FALSE;
			}
		}
		break;
	}
	
}
//---------------------------------------------------------------------------------------

void SetWaveVolAndPan(DWORD id,float vol,float pan);

enum	LOGINSTATE {		LGS_CONNECTSERVER,	
							LGS_IDANDPASSWORD,
							LGS_CREATEACCOUNT,
	LGS_REQUEST_WORLDSERVERLIST, LGS_SELECT_WORLDSERVER, LGS_CONNECT_WORLDSERVER, LGS_ENTER_WORLDSERVER, LGS_MOVEIN_WORLDSERVER,
							LGS_DISCONNECT_ACCOUNT,
							LGS_CHARACTERLOBBY,

	LGS_SELECTRACE,			LGS_DELETECHARACTER,		LGS_JOINGAME,
	LGS_SELECTATTRIBUTE, // BY NADIA
	LGS_CREATECHARACTER,
							LGS_ERRORMESSAGE,
							LGS_EXIT
};
enum	RACE {
		_BELLATOR,
		_CORA,
		_ACCRETIAN
};
enum	SEX {
		_MALE,
		_FEMALE
};

#define DebugBreak()    _asm { int 3 }
////////////////////////////////////////////////////////////////////////////
//
BOOL		g_bIsSendQuitMsg = FALSE;
BOOL		g_bIsEnterWorldServer = FALSE;

//CParticleManager	g_Particle;
BOOL		m_bRequestConnect = FALSE;
LOGINSTATE	g_LoginState = LGS_CONNECTSERVER;
LOGINSTATE	m_BefState;
RACE		m_Race	= _BELLATOR;
SEX			m_Sex	= _MALE;
BOOL		m_LoadDefault	= false;
BOOL		m_LoadDefaultStart = false;
BOOL		m_LoadEquip	= false;
BOOL		m_LoadEquipStart = false;
BOOL		m_CameraAni = false;
BOOL		m_HaveCharacter[3] = {false,false,false};
CAniCamera	m_AniCamera;
DWORD		m_RandomStart = 1;
#define		MSGBOXALPHA		0xaaffffff
#define		MAX_KEYFRAME	6

#define		CHARACTERDRAW
enum		CameraAni
{
	CAM_TOP_TO_LOGINA, CAM_TOP_TO_LOGINB, CAM_TOP_TO_LOGINC,
	CAM_SEL_A_TO_B,	CAM_SEL_B_TO_C, CAM_SEL_C_TO_A,
	CAM_SEL_B_TO_A,	CAM_SEL_C_TO_B, CAM_SEL_A_TO_C,
	CAM_SEL_A_TO_SELRACE, CAM_SEL_B_TO_SELRACE,CAM_SEL_C_TO_SELRACE,
	CAM_SEL_SELRACE_TO_A, CAM_SEL_SELRACE_TO_B,CAM_SEL_SELRACE_TO_C,
	CAM_SELRACE_TO_CREATE,
	CAM_CREATE_TO_SELRACE,
	CAM_CREATE_TO_A, CAM_CREATE_TO_B,CAM_CREATE_TO_C,
	CAM_A_TO_JOIN, CAM_B_TO_JOIN,CAM_C_TO_JOIN,
	CAM_ATTB_TO_ZOOM = 26, CAM_ZOOM_TO_ATTB
};
enum	COURSE
{
	A_COURSE, B_COURSE,	C_COURSE
};

COURSE	m_Course = A_COURSE;
BOOL	m_CharAlphaFlag = false;
float	m_CharAlpha = 0.0f;

char * g_pClassType[] ={ "BWB0", "BRB0", "BFB0", "BSB0",
						 "CWB0", "CRB0", "CFB0", "CSB0",
						 "AWB0", "ARB0", "ASB0", "ASB0" };

void LoadAttribute(CGUISelectAttribute * attb)
{
	/*
	static bool start = false;
	if (start ) return;
	start = true;
	*/

	for (int r = 0; r<3;r ++)
	{
		for (int at = 0; at< 4; at++ )
		{			
			CPlayerClass *data= _GetCharDataMgr()->GetClassData( g_pClassType[(r*4)+at]  );
			float hp = data->GetHP() / 200.0f;
			float fp = data->GetFP() / 200.0f;
			float sp = data->GetSP() / 200.0f;

			attb->SetBasicPoint( r, at, 0, hp);
			attb->SetBasicPoint( r, at, 1, fp);
			attb->SetBasicPoint( r, at, 2, sp);

#define		ATTB_MAX 20.0f

			/*
			attb->SetBattleSkill( r, at, 0, (float)data->GetExpertness(EVT_MELEE) /ATTB_MAX);
			attb->SetBattleSkill( r, at, 1, (float)data->GetExpertness(EVT_BOW) /ATTB_MAX);
			attb->SetBattleSkill( r, at, 2, (float)data->GetExpertness(EVT_FIREARM) /ATTB_MAX);
			*/
			attb->SetBattleSkill( r, at, 0, (float)data->GetExpertness(EVT_NEAR) /ATTB_MAX);
			attb->SetBattleSkill( r, at, 1, (float)data->GetExpertness(EVT_FAR) /ATTB_MAX);
			attb->SetBattleSkill( r, at, 2, (float)data->GetExpertness(EVT_RACE_SKILL) /ATTB_MAX);
			attb->SetBattleSkill( r, at, 3, (float)data->GetExpertness(EVT_FORCE) /ATTB_MAX);
			attb->SetBattleSkill( r, at, 4, (float)data->GetExpertness(EVT_SHIELD) /ATTB_MAX);
			attb->SetBattleSkill( r, at, 5, (float)data->GetExpertness(EVT_DEFENCE) /ATTB_MAX);

			attb->SetMakeSkill( r, at, 0, (float)data->GetExpertness(EVT_MAKE_WEAPON) /ATTB_MAX);
			attb->SetMakeSkill( r, at, 1, (float)data->GetExpertness(EVT_MAKE_ARMOR) /ATTB_MAX);
			attb->SetMakeSkill( r, at, 2, (float)data->GetExpertness(EVT_MAKE_BULLET) /ATTB_MAX);

			/*
			if( r == 0 )	// 벨라토
				attb->SetForcePoint( r, at, 0, (float)data->GetForceMastery( FCT_HOLY ) /ATTB_MAX);
			else
				attb->SetForcePoint( r, at, 0, (float)data->GetForceMastery( FCT_DARK ) /ATTB_MAX);
			attb->SetForcePoint( r, at, 1, (float)data->GetForceMastery( FCT_FIRE ) /ATTB_MAX);
			attb->SetForcePoint( r, at, 2, (float)data->GetForceMastery( FCT_AQUA ) /ATTB_MAX);
			attb->SetForcePoint( r, at, 3, (float)data->GetForceMastery( FCT_TERRA ) /ATTB_MAX);
			attb->SetForcePoint( r, at, 4, (float)data->GetForceMastery( FCT_WIND ) /ATTB_MAX);*/			
		}
	}
}

BOOL CheckSocketModule()
{
	char sname[256] = "Tp1\\01-goo";//"Ws2_32.dll"
	int len = strlen (sname);
	for (int  i = 0;i< len;i++)
	{
		sname[i] ^= 3;
	}
	
	char sock[256] = "",system[256] = "";
	GetSystemDirectory(system,256);
	HMODULE h = GetModuleHandle(sname);
	GetModuleFileName(h,sock,256);
	char *end = NULL;
	end = strrchr(sock,'\\');
	if (!end) return true;
	*end = NULL;
	
	if (strcmp(sock,system)!=0) return false;// 시스템 디렉토리가 아니라면 실행하지 않는다.
	// 
	FARPROC recv_ = (FARPROC) recv;
	FARPROC send_ = (FARPROC) send;
	if ( ( DWORD )recv_ < ( DWORD )h || ( DWORD )recv_ > (( DWORD )h + 0x10000  ) ) return false;
	if ( ( DWORD )send_ < ( DWORD )h || ( DWORD )send_ > (( DWORD )h + 0x10000  ) ) return false;
	
	return true;
}
void CharacterAlphaProcess()
{
	DWORD		curtime = timeGetTime();
	static		DWORD beftime  = 0;
	if (!beftime)beftime = curtime;
	float		delta	= (curtime-beftime)*0.002;
	beftime		= curtime;
	if ( m_CharAlphaFlag )
	{
		if (m_CharAlpha<1.0f)
		{
			m_CharAlpha += delta;
		}
		else
			m_CharAlpha = 1.0f;

		
	}
	else
	{
		if (m_CharAlpha>0.0f)
		{
			m_CharAlpha -= delta;
		}
		else
			m_CharAlpha = 0.0f;
	}
}
/*
float CharacterTopPos[3][3] = 
{
	
	{0.000f  , 83.106f,	-29.352f	},
	{25.420f , 83.106f,	14.676f		},
	{-25.42f ,  83.106f,  14.676f	}

};
*/
float CharacterTopPos[3][3] = 
{
	
	{0.200f  ,	225.0f,	-64.0f	},
	{56.0f ,	225.0f,	30.0f		},
	{-56.0f ,	225.0f,	30.0f	  	}

};
char *CamName [256]= {
	"",//0
	"",//1
	"C_01_01",//2
	"C_01_02",//3
	"C_01_03",//4
	
	"C_02_01",//5
	"C_02_02",//6
	"C_02_03",//7
	
	"C_02_01_01",//8
	"C_02_02_01",//9
	"C_02_03_01",//10
	
	"C_03_01",//11
	"C_03_02",//12
	"C_03_03",//13

	"C_04_03",//14
	"C_04_01",//15
	"C_04_02",//16

	"C_02_01_IN",//17
	"C_02_02_IN",//18
	"C_02_03_IN",//19

	"C_05_01",//20
	"C_05_02",//21
	"C_05_03",//22
	"C_04_01_IN"//23
};
/*
char *CamName [256]= {
	"",
	"",
	"C01-01",
	"C01-02",
	"C01-03",
	
	"C02-01",
	
	"C03-01",
	"C03-02",
	"C03-03", 
	
	"C04-01",

	"C05-01",
	"C05-02",
	"C05-03",
};
*/
DWORD	m_AniCamKeyFrame[35][4] = 
{
	{ CAM_TOP_TO_LOGINA, 2, 0, 45 },
	{ CAM_TOP_TO_LOGINB, 3, 0, 40 },
	{ CAM_TOP_TO_LOGINC, 4, 0, 40 },

	{ CAM_SEL_A_TO_B,	 5, 0,	30 },
	{ CAM_SEL_B_TO_C,	 6, 0,  30 },
	{ CAM_SEL_C_TO_A,	 7, 0,  30 },

	{ CAM_SEL_B_TO_A,	 5, 30,	0 },
	{ CAM_SEL_C_TO_B,	 6, 30, 0 },
	{ CAM_SEL_A_TO_C,	 7, 30, 0 },

	{ CAM_SEL_A_TO_SELRACE,	8,	0,	40},
	{ CAM_SEL_B_TO_SELRACE,	9,	0,	44},
	{ CAM_SEL_C_TO_SELRACE,	10,	0,	44},

	{ CAM_SEL_SELRACE_TO_A,	8,	40,	0},
	{ CAM_SEL_SELRACE_TO_B,	9,	44,	0},
	{ CAM_SEL_SELRACE_TO_C,	10,	44,	0},

	{ CAM_SELRACE_TO_CREATE,11,	0,	70},
	{ CAM_CREATE_TO_SELRACE,11,	60,	0},

	{ CAM_CREATE_TO_A,	14,	0,	110},	
	{ CAM_CREATE_TO_B,	15,	0,	110},
	{ CAM_CREATE_TO_C,	16,	0,	110},

	{ CAM_A_TO_JOIN,	17,	0,	20},
	{ CAM_B_TO_JOIN,	18,	0,	20},
	{ CAM_C_TO_JOIN,	19,	0,	20},

	{ CAM_TOP_TO_LOGINA, 20, 0, 70 },
	{ CAM_TOP_TO_LOGINB, 21, 0, 70 },
	{ CAM_TOP_TO_LOGINC, 22, 0, 55 },

	{ CAM_ATTB_TO_ZOOM, 23, 0, 10 },
	{ CAM_ZOOM_TO_ATTB, 23, 10, 0 },
	
};

void	EventSelectAttribute(DWORD race, DWORD attb)
{
	if( ( race > 2 ) || ( attb > 4 ) )
		return;

	ZeroMemory( m_gClassTypeString, 8 );

	int nClassType = ( race * 4 ) + attb;
	sprintf( m_gClassTypeString, g_pClassType[nClassType] );
}
/*
BOOL
LoadDummyResouce( void )
{
	CItem * l_pItem = NULL;
	CMesh * l_pMesh = NULL;

	int cn = 0;
	// dummy
	
	for( int i = 0; i < 3; ++i )
	{
		// item
		for( int j = 0; j < MAX_EQUIP_PART; ++j )
		{
			l_pItem = _GetAvatarDummy( i )->GetEquipItem( j );
			if( !l_pItem )
				continue;

			l_pMesh = l_pItem->GetMesh();
			if( !l_pMesh )
			{
				continue;
			}

			if(!_GetCharIF()->LoadRealData( l_pMesh,&_GetCharIF()->m_TM,true))
				cn++;
		}
	}
	if (cn==0) 	return TRUE;
	else	return FALSE;

}
*/
BOOL
LoadDummyResouce( void )
{
	CEquipItem *	pEquipItem = NULL;
	CMesh *			pMesh = NULL;

	int cn = 0;
	// dummy
	
	for( int i = 0; i < 3; ++i )
	{
		// item
		for( int j = 0; j < MAX_EQUIP_PART; ++j )
		{
			pEquipItem = _GetAvatarDummy( i )->GetEquipItem( j );
			if( !pEquipItem )
				continue;

			pMesh = pEquipItem->GetMesh();
			if( !pMesh )
			{
				continue;
			}

			//if(!_GetCharIF()->LoadRealData( pMesh, &_GetCharIF()->m_TM, true))
				cn++;
		}
	}
	//if (cn==0) 	return TRUE;
	//else	return FALSE;
	return true;

}
BOOL	LoadDefaultMesh()
{
	
	
//	CItem	l_cItem;
//	CMesh *	l_pMesh;
	
	int cn = 0;
	for (int i = 0;i<5;i++)
	{
		CBone * l_pBone = _GetBoneMgr()->GetBone( RLI_PLAYER, i );
		//if (!l_pBone)continue;
		//if (!_GetCharIF()->LoadRealData(	l_pBone,&_GetCharIF()->m_TM),true)cn++;
	}
	/*
	for(  i = 0; i < 25; ++i )
	{
		for(int j = 0; j < MAX_DEFAULT_PART; ++j )
		{
			_GetItemDataMgr()->GetData( j, i, l_cItem );
			l_pMesh = l_cItem.GetMesh();
			//if (!l_pMesh)continue;
			//if (!_GetCharIF()->LoadRealData(	l_pMesh,&_GetCharIF()->m_TM),true)cn++;
		}
	}

	//if (cn==0) 	return TRUE;
	//else	return FALSE;
	*/
	return true;
	
}

int		m_CurAniKey = 0;
void PlayCamera(CameraAni	ani,DWORD	course = 0)
{
#define		CAM_START_WAV	32001  
#define		CAM_END_WAV		32003
#define		CAM_MIDDLE_WAV	32002
	//
	
	PlayWave( CAM_START_WAV,0,1.0f,0.0f);	
	//PlayWave( CAM_MIDDLE_WAV,_PLAY_WAVE_LOOP,1.0f,0.0f);	
	//
	DWORD	a  = ani + course;
	m_CurAniKey = a;
	DWORD	cam = 0;
	if ( m_RandomStart && g_LoginState==LGS_ENTER_WORLDSERVER)
	{
		a += 23;
	}
	
	cam =m_AniCamera.GetCameraIndex(CamName[m_AniCamKeyFrame[a][1]]);
	
	
	m_AniCamera.SetPrePlayCamera( 
		cam,
		m_AniCamKeyFrame[a][2],
		m_AniCamKeyFrame[a][3],
		_CAM_FLAG_FINAL_STOP);
}
/*
DWORD	m_AniKeyFrame[MAX_KEYFRAME] = 
{
		0,//FRAME_IDANDPASSWORD
		56,
		129,//FRAME_CHARACTERLOBBY
		199,//FRAME_SELECTRACE
		312,//FRAME_CREATECHARACTER
		335,//FRAME_JOIN
		//440//FRAME_END
};
*/
DWORD		m_MP3ElapseTime = 0;
DWORD		m_SelectFrame = 0;
DWORD		m_SelectCam = 1;
//float		m_vecPos[3] = { 0.000f, 82.676f, -32.000f };
float		m_vecPos[3] = { 26.995f, -125.913f, 15.673f };
float		m_CharRot	= 180;

//
//float					m_CharacterAlpha = 1.0;
//DWORD					m_CharShowDelay  = 0;
//DWORD					m_CharStartTime  = 0;
//BOOL					m_CharHide		 = false;
D3DXVECTOR3				m_CurPos;
LPDIRECT3DTEXTURE8		g_LogoTexture;
struct	Sound3D
{
	//bool		m_Play;
	DWORD		m_FadeOutDelay;
	DWORD		m_ElapseTime;
	D3DXVECTOR3	m_Pos;
	DWORD		m_ID;
	float		m_MinDist;
	float		m_MaxDist;
	float		m_MaxVolume;
	BOOL		m_bPlay;
	Sound3D()
	{
	//	m_Play = true;
		m_FadeOutDelay = 1000;
		m_MaxVolume = 1.0f;
		m_ElapseTime = 0;
		m_bPlay		= false;
	}
};
typedef vector<Sound3D>	 SOUNDLIST;
SOUNDLIST	m_SoundList;
void LoadSoundIni()
{
	char filename[256] = ".\\CHEF\\Sound.ini";
	int id = 0;
	Sound3D		sd;
	int max = GetPrivateProfileInt( "CONFIG ", "MAX_SOUND", 0, filename );
	char sound[256];
	for (int i = 0; i<max ;i ++)
	{
		sprintf(sound,"SOUND_%d",i+1); 
		sd.m_ID			= GetPrivateProfileInt( sound, "SOUND_ID", 0, filename );

		int x = ( int )GetPrivateProfileInt( sound, "SOUND_POSX", 0, filename );
		int y = ( int )GetPrivateProfileInt( sound, "SOUND_POSY", 0, filename );
		int z = ( int )GetPrivateProfileInt( sound, "SOUND_POSZ", 0, filename );
		//sd.m_Pos.x		= ( int )GetPrivateProfileInt( sound, "SOUND_POSX", 0, filename );
		//sd.m_Pos.y		= ( int )GetPrivateProfileInt( sound, "SOUND_POSY", 0, filename );
		//sd.m_Pos.z		= ( int )GetPrivateProfileInt( sound, "SOUND_POSZ", 0, filename );
		sd.m_Pos.x		= x;
		sd.m_Pos.y		= y;
		sd.m_Pos.z		= z;


		sd.m_MaxDist	= ( int )GetPrivateProfileInt( sound, "SOUND_MAXDIST", 100, filename );
		sd.m_MinDist	= ( int )GetPrivateProfileInt( sound, "SOUND_MINDIST", 0, filename );
		sd.m_MaxVolume	= (float)GetPrivateProfileInt( sound, "SOUND_MAXVOLUME", 100, filename ) / 100.0f;
		
		PlayWave( sd.m_ID,_PLAY_WAVE_LOOP,0.0f,1.0f);
		m_SoundList.push_back(sd);
	}
}
void InitLoginSound()
{
	//LoadSoundIni();
	Sound3D		sd;
	sd.m_ID	  	= 32004;
	sd.m_Pos.x	= 0;
	sd.m_Pos.y	= 238;
	sd.m_Pos.z	= 0;
	sd.m_MaxDist	= 700;
	sd.m_MinDist	= 50;
	sd.m_MaxVolume	= 1.0f;
	PlayWave( sd.m_ID,_PLAY_WAVE_LOOP,0.0f,1.0f);
	m_SoundList.push_back(sd);

	m_SoundList.reserve(10);
	
}

static CPlayMP3 st_PlayMP3;

void FadeOutMp3(DWORD tm)
{
	if (m_MP3ElapseTime)
	{
		DWORD		curTime = timeGetTime();
		DWORD		delta	= (curTime-m_MP3ElapseTime);
		if (delta<tm)
		{
			float v = (float)delta/ (float)tm;
			v = 1.0 - v;
			st_PlayMP3.SetVolumeMP3(v);
		}
	}
	else
	{
		st_PlayMP3.SetVolumeMP3(1.0);
	}
}	
void LoginSoundProcess()
{
	int size = m_SoundList.size();
	_R3ENGINE_STATE *state = GetR3State();
	D3DXVECTOR4	cur;
	D3DXVECTOR4	snd;
	D3DXMATRIX	rotmat;
	D3DXMATRIX	view =state->mMatView ;
	view._41 = view._42 = view._43 = 0;
	D3DXVec3Transform(&cur,&m_CurPos,&view);
	Sound3D *sd;
	static DWORD	befTime = 0;
	DWORD			curTime = timeGetTime();
	if (! befTime) befTime =  curTime;
	float			time = 7000;
	
	if ((curTime-befTime)>time)
	{
		befTime = curTime;
	}
	float delta = (curTime - befTime) /time;
	//float angle  = delta * 360.f;
	float angle  = 0;//delta * 360.f;
	D3DXMatrixRotationY(&rotmat,D3DXToRadian(angle));
	for (int i =0; i<size;i++)
	{
		
		sd = &m_SoundList[i];
		
		{
			float min = m_SoundList[i].m_MinDist;
			float max = m_SoundList[i].m_MaxDist;
			//D3DXVECTOR3 buf = m_SoundList[i].m_Pos  - m_CurPos;
			//float len  = D3DXVec3Length(&buf);
			//D3DXMatrixMultiply(&rotmat,&state->mMatView,&rotmat);
			D3DXMatrixMultiply(&rotmat,&rotmat,&view);
			//
			//D3DXVec3Transform(&snd,&m_SoundList[i].m_Pos,&state->mMatView);
			D3DXVec3Transform(&snd,&m_SoundList[i].m_Pos,&rotmat);
			D3DXVECTOR3 buf = snd  - cur;
			float len  = D3DXVec3Length(&buf);
			float  pan = (snd.x  - cur.x)/(max/2.0f);
			pan = min(1.0,pan);
			pan = max(-1.0,pan);
			//SetWavePan( m_SoundList[i].m_ID,1.0);
			
			//
			if ( len <=max)
			{
				float volume = 1.0 - (len/max);
				if (len < min )
					volume = 1.0;
				//volume = min( m_SoundList[i].m_MaxVolume, volume );
				volume = m_SoundList[i].m_MaxVolume * volume;
				if ( !m_SoundList[i].m_ElapseTime )
				{
					//SetWaveVolume( m_SoundList[i].m_ID,volume,pan);
					SetWaveVolAndPan( m_SoundList[i].m_ID,volume,pan);
					//PlayWave( m_SoundList[i].m_ID,_PLAY_WAVE_LOOP,volume,pan);
				}
				else
				{
					DWORD	delta = curTime - m_SoundList[i].m_ElapseTime;
					if ( delta < m_SoundList[i].m_FadeOutDelay)
					{
						float v = (float)delta/ (float)m_SoundList[i].m_FadeOutDelay;
						v = 1.0 - v;
						volume = v * volume;
						PlayWave( m_SoundList[i].m_ID,_PLAY_WAVE_LOOP,volume,pan);
					}
					else
					{
						SetWaveVolAndPan( m_SoundList[i].m_ID,0.0f,0.0f);
						//StopWave( m_SoundList[i].m_ID );
						//PlayWave( m_SoundList[i].m_ID,_PLAY_WAVE_LOOP,0.0);
					}
				}
				
			}
			else
			{
				SetWaveVolAndPan( m_SoundList[i].m_ID,0.0f,0.0f);
				//StopWave( m_SoundList[i].m_ID );
				//PlayWave( m_SoundList[i].m_ID,_PLAY_WAVE_LOOP,0.0);
			}
		}
		
	}
}
void FadeOutSound()
{
	int size = m_SoundList.size();
	DWORD	cm = timeGetTime();
	for (int i =0; i<size;i++)
	{
		m_SoundList[i].m_ElapseTime  = cm;
	}
}
void CloseLoginSound()
{
	int size = m_SoundList.size();
	for (int i =0; i<size;i++)
	{
		StopWave( m_SoundList[i].m_ID);
		IM_ReleaseWave( m_SoundList[i].m_ID);
	}
	//IM_ReleaseAllWaves();
	m_SoundList.clear();
}
/*
void SetCharacterTop()
{
	m_CharRot	= 180 + m_Course * 120;
	D3DXMATRIX	mat;
	D3DXVECTOR3 vec(0,0,-1);
	D3DXVECTOR4 out;
	D3DXMatrixRotationY(&mat,D3DXToRadian(30 + m_Course * 120));
	D3DXVec3Transform(&out,&vec,&mat);
	SetMainDirLight((D3DXVECTOR3)out);
	
	memcpy(&m_vecPos,CharacterTopPos[m_Course],sizeof(float)*3);

}
*/
void SetCharacterBottom()
{
	m_CharRot	= 180;
	D3DXMATRIX	mat;
	D3DXVECTOR3 vec(0,0,1);
	D3DXVECTOR4 out;
	//D3DXMatrixRotationY(&mat,D3DXToRadian(300));
	D3DXMatrixRotationY(&mat,D3DXToRadian(30));
	D3DXVec3Transform(&out,&vec,&mat);
	SetMainLight((D3DXVECTOR3)out);
	//선택(0.250,  -29.583,  83.149)
	//생성(0.250, -35.108, -132.051)

	//m_vecPos[0] = 0.250f;
	//m_vecPos[1] = -132.051f;
	//m_vecPos[2] = -35.108f;
	m_vecPos[0] = 1.0f;
	m_vecPos[1] = -155.0f;
	m_vecPos[2] = 29.0f;
	
}
void EventGameClose()
{
	st_PlayMP3.StopR3MP3();
	st_PlayMP3.ReleaesR3MP3();
	CloseLoginSound();
	_GetMainApp()->RequestQuitProgram();
}	
void EventLoginFunc(char *name, char *password)
{
	_GetNetworkMgr()->SetID( name );
	_GetNetworkMgr()->SetPassword( password );

	//_GetNetworkMgr()->MasterMsg_CertifyUserReq_clma();
	_GetNetworkMgr()->AccountMsg_LoginAccountRequest_cllo();


	/*
	g_LoginState	=	LGS_CHARACTERLOBBY;
	m_CameraAni		=	true;
	m_AniCamera.SetPrePlayCamera(m_SelectCam,m_AniKeyFrame[0],m_AniKeyFrame[1],_CAM_FLAG_FINAL_STOP);
	*/
	m_uiLoginWindow.Close();
	SetCharacterBottom();
	
	//m_CharRot				= 180;
	//	SetFadeOut(0x00,1,1.0f);   	

}
void EventCreateAccount(char *name, char *password)
{
	_GetNetworkMgr()->SetID( name );
	_GetNetworkMgr()->SetPassword( password );

	//_GetNetworkMgr()->MasterMsg_UserRegReq_clma();
	_GetNetworkMgr()->AccountMsg_JoinAccountRequest_cllo();
	//name[0] = password[0]	= 0;// Init
}
void SetCharacterInfo(DWORD m_Course)
{
	char buf[256];
	m_uiSelectCharacter.SetCharInfo_Name(_GetAvatarDummy(m_Course)->GetName());
	switch ( _GetAvatarDummy(m_Course)->GetRace() )
	{
		case CRT_BELLATOR_MALE:
		case CRT_BELLATOR_FEMALE:
			sprintf(buf,"Bellator");
			break;
		case CRT_CORA_MALE:
		case CRT_CORA_FEMALE:
			sprintf(buf,"Cora");
			break;
		case CRT_ACCRETIA:
			sprintf(buf,"Accretia");
			break;
	}
	m_uiSelectCharacter.SetCharInfo_Race(buf);

	switch ( _GetAvatarDummy(m_Course)->GetRace())
	{
		case CRT_BELLATOR_MALE:
		case CRT_CORA_MALE:
		case CRT_ACCRETIA:
			sprintf(buf,"Male");
			break;
		case CRT_BELLATOR_FEMALE:
		case CRT_CORA_FEMALE:
			sprintf(buf,"Female");
			break;
	}
	m_uiSelectCharacter.SetCharInfo_Sex(buf);
	sprintf( buf, "%u",_GetAvatarDummy(m_Course)->GetLevel());
	m_uiSelectCharacter.SetCharInfo_Level(buf);
	m_uiSelectCharacter.SetCharInfo_Money( _GetAvatarDummy(m_Course)->GetDalant(),
										   _GetAvatarDummy(m_Course)->GetGold(),
										   _GetAvatarDummy(m_Course)->GetRace() );
	//m_uiSelectCharacter.Show_CharInfoBoard(true);
}
void EventNextButton()
{
	if (g_LoginState == LGS_CHARACTERLOBBY)
	{
		m_CameraAni		=	true;
		switch (m_Course)
		{
		case A_COURSE:
			m_Course = B_COURSE;
			PlayCamera(CAM_SEL_A_TO_B);
			break;
		case B_COURSE:
			m_Course = C_COURSE;
			PlayCamera(CAM_SEL_B_TO_C);
			break;
		case C_COURSE:
			m_Course = A_COURSE;
			PlayCamera(CAM_SEL_C_TO_A);
			break;
		}

		//m_uiSelectCharacter.Close();
		UI_SetMoveWindow( SELECT_CHAR_WIN, FALSE );			
	}
}
void EventPreviousButton()
{
	if (g_LoginState == LGS_CHARACTERLOBBY)
	{
		m_CameraAni		=	true;
		switch (m_Course)
		{
		case A_COURSE:
			m_Course = C_COURSE;
			PlayCamera(CAM_SEL_A_TO_C);
			break;
		case B_COURSE:
			m_Course = A_COURSE;
			PlayCamera(CAM_SEL_B_TO_A);
			break;
		case C_COURSE:
			m_Course = B_COURSE;
			PlayCamera(CAM_SEL_C_TO_B);
			break;
		}

		//m_uiSelectCharacter.Close();
		UI_SetMoveWindow( SELECT_CHAR_WIN, FALSE );			
	}
}
void EventSelectBELLATOR();
void EventCreateCharacter()
{
	if ( m_HaveCharacter[m_Course] ) 
	{
		_GUIMessageBox( NULL, "생성된 캐릭터가 있습니다.",MSGBOXALPHA);
		return ;
	}
	m_LoadDefaultStart = true;
	_GUIMessageBox( NULL, "잠시 기다려주세요.",MSGBOXALPHA);
	/*
	switch(m_Course)
	{
	case A_COURSE:
		PlayCamera(CAM_SEL_A_TO_SELRACE);
		break;
	case B_COURSE:
		PlayCamera(CAM_SEL_B_TO_SELRACE);
		break;
	case C_COURSE:
		PlayCamera(CAM_SEL_C_TO_SELRACE);
		break;
	}
	*/
}
void EventDeleteCharacter()
{
	if (m_HaveCharacter[m_Course])
	{
		//_GetNetworkMgr()->MasterMsg_CharacterDelete_clma(m_Course);
		_GetNetworkMgr()->SystemMsg_DelCharRequest_zone(m_Course);
	}
}
void EventSelectRace()
{
	LoadAttribute(&m_uiSelectAttribute);

	g_LoginState	= LGS_SELECTATTRIBUTE;
	m_CameraAni		= true;
	//m_AniCamera.SetPrePlayCamera(m_SelectCam,m_AniKeyFrame[2],m_AniKeyFrame[3],_CAM_FLAG_FINAL_STOP);		
	//m_uiSelectRace.Close();
	UI_SetMoveWindow( SELECT_RACE_WIN, FALSE );
	SetCharacterBottom();


	PlayCamera(CAM_SELRACE_TO_CREATE);
	//
	m_uiSelectAttribute.SetRaceType( _GetAvatar()->GetRace() );
	//
}
void EventSelectRaceCancel()
{
	g_LoginState	= LGS_CHARACTERLOBBY;
	m_CameraAni		= true;
	//m_AniCamera.SetPrePlayCamera(m_SelectCam,m_AniKeyFrame[2],m_AniKeyFrame[1],_CAM_FLAG_FINAL_STOP);
	//m_uiSelectRace.Close();
	UI_SetMoveWindow( SELECT_RACE_WIN, FALSE );	
	//if ( m_HaveCharacter[m_Course] )
	//	SetCharacterTop();
	//else
	//	SetCharacterBottom();

	PlayCamera(CAM_SEL_SELRACE_TO_A,m_Course);
	/*
	switch(m_Course)
	{
	case A_COURSE:
		PlayCamera(CAM_SEL_SELRACE_TO_A);
		break;
	case B_COURSE:
		PlayCamera(CAM_SEL_SELRACE_TO_B);
		break;
	case C_COURSE:
		PlayCamera(CAM_SEL_SELRACE_TO_C);
		break;
	}
	*/
}
void EventJoinGameFunc()
{
	if ( g_LoginState == LGS_CHARACTERLOBBY)
	{
		if (!m_HaveCharacter[m_Course])
		{
			_GUIMessageBox( NULL, "캐릭터를 생성해주세요.",MSGBOXALPHA);
			return;
		}
		_GetNetworkMgr()->SystemMsg_SelCharRequest_zone( m_Course );
		g_LoginState = LGS_JOINGAME;
	}
	/*
	//
	m_MP3ElapseTime = timeGetTime();
	FadeOutSound();
	//
	//m_AniCamera.SetPrePlayCamera(m_SelectCam,m_AniKeyFrame[4],m_AniKeyFrame[5],_CAM_FLAG_FINAL_STOP);
	SetFadeOut(0x00, 1 ,1.0f);   	
	g_LoginState = LGS_EXIT;
	m_uiSelectCharacter.Close();
	//

	//---------------------------------------------------------------------
	// 아바타 세팅
	_GetAvatar()->SetName( _GetAvatarDummy(m_Course)->GetName() );
	_GetAvatar()->SetRace( _GetAvatarDummy(m_Course)->GetRace() );
	_GetAvatar()->SetBoneID( _GetAvatarDummy(m_Course)->GetBoneID() );
	_GetAvatar()->SetDTIndex( _GetAvatarDummy(m_Course)->GetDTIndex() );

	_GetAvatar()->SetLevel( _GetAvatarDummy(m_Course)->GetLevel(), FALSE );
	_GetAvatar()->SetExp( _GetAvatarDummy(m_Course)->GetExp(), FALSE );
	_GetAvatar()->SetDalant( _GetAvatarDummy(m_Course)->GetDalant(), FALSE );
	_GetAvatar()->SetGold( _GetAvatarDummy(m_Course)->GetGold(), FALSE );

	_GetAvatar()->SetHP( _GetAvatarDummy(m_Course)->GetHP(), FALSE );
	_GetAvatar()->SetFP( _GetAvatarDummy(m_Course)->GetFP(), FALSE );
	_GetAvatar()->SetSP( _GetAvatarDummy(m_Course)->GetSP(), FALSE );

	for( int i = 0; i < MAX_DEFAULT_PART; ++i )
		_GetAvatar()->SetDefaultItem( i, _GetAvatarDummy(m_Course)->GetDefaultItem( i ) );

	for( i = 0; i < MAX_EQUIP_PART; ++i )
		_GetAvatar()->SetEquipItem( _GetAvatarDummy(m_Course)->GetEquipItem( i ) );
	//*/

	//_GetNetworkMgr()->MasterMsg_SelectCharacter_clma();
	
	
	/*
	switch(m_Course)
	{
	case A_COURSE:
		PlayCamera(CAM_A_TO_JOIN,m_Course);
		break;
	case B_COURSE:
		PlayCamera();
		break;
	case C_COURSE:
		PlayCamera();
		break;
	}
	*/
}
void EventCreateCharacterOK()
{
	/*
	m_AniCamera.SetPrePlayCamera(m_SelectCam,m_AniKeyFrame[3],m_AniKeyFrame[4],_CAM_FLAG_FINAL_STOP);
	SetCharacterTop();
	g_LoginState	=	LGS_CHARACTERLOBBY;
	m_CameraAni		=	true;
	m_HaveCharacter	=	true;	
	*/
	char * name  = m_uiCreateCharacter.GetName();
	if ( strlen(name)>0 && strlen(name)<=16 &&
		 _IsAvailableCharName( name, _GetNetworkMgr()->GetAvatarGrade() ) ) // 캐릭터 이름 필터링
	{		
		_GetAvatar()->SetName(name);
		//
		//_GetNetworkMgr()->MasterMsg_CharacterCreate_clma(m_Course);
		_GetNetworkMgr()->SystemMsg_AddCharRequest_zone(m_Course);
	}
	else
	{
		_GUIMessageBox( &m_uiCreateCharacter, "캐릭터 이름에 문제가 있습니다. ",MSGBOXALPHA);
	}
}
void EventCreateCharacterCancel()
{
//	PlayCamera(CAM_CREATE_TO_SELRACE);
	m_CharAlphaFlag = false;
	//m_AniCamera.SetPrePlayCamera(m_SelectCam,m_AniKeyFrame[3],m_AniKeyFrame[2],_CAM_FLAG_FINAL_STOP);
	/*
	switch(m_Course)
	{
	case A_COURSE:
		PlayCamera();
		break;
	case B_COURSE:
		PlayCamera();
		break;
	case C_COURSE:
		PlayCamera();
		break;
	}
	*/
	
	//m_uiCreateCharacter.Close();
	UI_SetMoveWindow( CREATE_CHAR_WIN, FALSE );

	g_LoginState	= LGS_SELECTATTRIBUTE;	

	m_CameraAni		= true;
	PlayCamera( CAM_ZOOM_TO_ATTB );
}

void EventSelectAttributeOK()
{
	// 캐릭터 생성으로 이동
	m_CameraAni		= true;
	PlayCamera( CAM_ATTB_TO_ZOOM );

	UI_SetMoveWindow( SELECT_ATTR_WIN, FALSE );

	g_LoginState	= LGS_CREATECHARACTER;	
}

void EventSelectAttributeCancel()
{	
	// 종족 선택으로 이동

	UI_SetMoveWindow( SELECT_ATTR_WIN, FALSE );

	g_LoginState	= LGS_SELECTRACE;
	m_CameraAni		= true;

	PlayCamera( CAM_CREATE_TO_SELRACE );
}


void EventSelectBELLATORMale()
{
	_GetAvatar()->SetRace( CRT_BELLATOR_MALE );
	_GetAvatar()->SetBone( ID_DEFAULT_BONE_BM );
	_GetAvatar()->SetDTIndex( CRT_BELLATOR_MALE );
	
	_GetAvatar()->PutOnEquipment( CEPT_HAIR, ID_DEFAULT_ARMOR_BM );
	_GetAvatar()->PutOnEquipment( IEPT_UPPER_PART, ID_DEFAULT_ARMOR_BM );
	_GetAvatar()->PutOnEquipment( IEPT_LOWER_PART, ID_DEFAULT_ARMOR_BM );
	_GetAvatar()->PutOnEquipment( IEPT_GLOVES, ID_DEFAULT_ARMOR_BM );
	_GetAvatar()->PutOnEquipment( IEPT_SHOES, ID_DEFAULT_ARMOR_BM );
	_GetAvatar()->PutOnEquipment( IEPT_FACE, ID_DEFAULT_ARMOR_BM );
	
	_GetAvatar()->SetLightColor( D3DCOLOR_XRGB(128,128,128) );
}

void EventSelectBELLATORFemale()
{
	_GetAvatar()->SetRace( CRT_BELLATOR_FEMALE );
	_GetAvatar()->SetBone( ID_DEFAULT_BONE_BF );
	_GetAvatar()->SetDTIndex( CRT_BELLATOR_FEMALE );
	
	_GetAvatar()->PutOnEquipment( CEPT_HAIR, ID_DEFAULT_ARMOR_BF );
	_GetAvatar()->PutOnEquipment( IEPT_UPPER_PART, ID_DEFAULT_ARMOR_BF );
	_GetAvatar()->PutOnEquipment( IEPT_LOWER_PART, ID_DEFAULT_ARMOR_BF );
	_GetAvatar()->PutOnEquipment( IEPT_GLOVES, ID_DEFAULT_ARMOR_BF );
	_GetAvatar()->PutOnEquipment( IEPT_SHOES, ID_DEFAULT_ARMOR_BF );
	_GetAvatar()->PutOnEquipment( IEPT_FACE, ID_DEFAULT_ARMOR_BF );

	_GetAvatar()->SetLightColor( D3DCOLOR_XRGB(128,128,128) );
}

void EventSelectCORAMale()
{
	_GetAvatar()->SetRace( CRT_CORA_MALE );
	_GetAvatar()->SetBone( ID_DEFAULT_BONE_CM );
	_GetAvatar()->SetDTIndex( CRT_CORA_MALE );
	
	_GetAvatar()->PutOnEquipment( CEPT_HAIR, ID_DEFAULT_ARMOR_CM );
	_GetAvatar()->PutOnEquipment( IEPT_UPPER_PART, ID_DEFAULT_ARMOR_CM );
	_GetAvatar()->PutOnEquipment( IEPT_LOWER_PART, ID_DEFAULT_ARMOR_CM );
	_GetAvatar()->PutOnEquipment( IEPT_GLOVES, ID_DEFAULT_ARMOR_CM );
	_GetAvatar()->PutOnEquipment( IEPT_SHOES, ID_DEFAULT_ARMOR_CM );
	_GetAvatar()->PutOnEquipment( IEPT_FACE, ID_DEFAULT_ARMOR_CM );

	_GetAvatar()->SetLightColor( D3DCOLOR_XRGB(128,128,128) );
}
void EventSelectCORAFemale()
{
	_GetAvatar()->SetRace( CRT_CORA_FEMALE );
	_GetAvatar()->SetBone( ID_DEFAULT_BONE_CF );
	_GetAvatar()->SetDTIndex( CRT_CORA_FEMALE );
	
	_GetAvatar()->PutOnEquipment( CEPT_HAIR, ID_DEFAULT_ARMOR_CF );
	_GetAvatar()->PutOnEquipment( IEPT_UPPER_PART, ID_DEFAULT_ARMOR_CF );
	_GetAvatar()->PutOnEquipment( IEPT_LOWER_PART, ID_DEFAULT_ARMOR_CF );
	_GetAvatar()->PutOnEquipment( IEPT_GLOVES, ID_DEFAULT_ARMOR_CF );
	_GetAvatar()->PutOnEquipment( IEPT_SHOES, ID_DEFAULT_ARMOR_CF );
	_GetAvatar()->PutOnEquipment( IEPT_FACE, ID_DEFAULT_ARMOR_CF );

	_GetAvatar()->SetLightColor( D3DCOLOR_XRGB(128,128,128) );
}

void EventSelectACCRETIAN()
{
	m_Race = _ACCRETIAN;

	_GetAvatar()->SetRace( CRT_ACCRETIA );
	_GetAvatar()->SetBone( ID_DEFAULT_BONE_AC );
	_GetAvatar()->SetDTIndex( CRT_ACCRETIA );
	
	_GetAvatar()->PutOnEquipment( CEPT_HAIR, ID_DEFAULT_ARMOR_AC );
	_GetAvatar()->PutOnEquipment( IEPT_UPPER_PART, ID_DEFAULT_ARMOR_AC );
	_GetAvatar()->PutOnEquipment( IEPT_LOWER_PART, ID_DEFAULT_ARMOR_AC );
	_GetAvatar()->PutOnEquipment( IEPT_GLOVES, ID_DEFAULT_ARMOR_AC );
	_GetAvatar()->PutOnEquipment( IEPT_SHOES, ID_DEFAULT_ARMOR_AC );
	_GetAvatar()->PutOnEquipment( IEPT_FACE, ID_DEFAULT_ARMOR_AC );

	_GetAvatar()->SetLightColor( D3DCOLOR_XRGB(128,128,128) );
	//
	m_uiCreateCharacter.SetSelectItem(0,0);
	//
	/*
	CItem	l_cItem;
	CMesh *	l_pMesh;
	bool	loop = true;
	while (true)
	{
		int cn = 0;
		for( int i = 20; i < 5; ++i )
		{
			for( int j = 0; j < MAX_DEFAULT_PART; ++j )
			{
				_GetItemDataMgr()->GetData( j, i, l_cItem );
				l_pMesh = l_cItem.GetMesh();
				if (!l_pMesh)
				{
					cn++;
					continue;
				}
				if (_GetCharIF()->LoadRealData(	l_pMesh,&_GetCharIF()->m_TM))cn++;
			}
		}
		if (cn==5 * MAX_DEFAULT_PART) break;
		Sleep(1);
	}
	*/
}
void EventSelectBELLATOR()
{
	
	
	m_Race = _BELLATOR;
	EventSelectBELLATORMale();
	m_uiCreateCharacter.SetSelectItem(0,0);
}
void EventSelectCORA()
{
	/*
	CItem	l_cItem;
	CMesh *	l_pMesh;
	bool	loop = true;
	while (true)
	{
		int cn = 0;
		for( int i = 10; i < 20; ++i )
		{
			for( int j = 0; j < MAX_DEFAULT_PART; ++j )
			{
				_GetItemDataMgr()->GetData( j, i, l_cItem );
				l_pMesh = l_cItem.GetMesh();
				if (!l_pMesh)
				{
					cn++;
					continue;
				}
				if (_GetCharIF()->LoadRealData(	l_pMesh,&_GetCharIF()->m_TM))cn++;
			}
		}
		if (cn==10 * MAX_DEFAULT_PART) break;
		Sleep(1);
	}
	*/
	m_Race = _CORA;
	EventSelectCORAMale();
	m_uiCreateCharacter.SetSelectItem(0,0);
}
void EventLeftSelectSex()
{
	m_Sex = _MALE;
	switch (m_Race)
	{
	case _BELLATOR:
		EventSelectBELLATORMale();
		break;
	case _CORA:
		EventSelectCORAMale();
		break;
	case _ACCRETIAN :
		EventSelectACCRETIAN();
		break;
	}
}
void EventRightSelectSex()
{
	m_Sex = _FEMALE;
	switch (m_Race)
	{
	case _BELLATOR:
		EventSelectBELLATORFemale();
		break;
	case _CORA:
		EventSelectCORAFemale();
		break;
	case _ACCRETIAN :
		EventSelectACCRETIAN();
		break;
	}

}
#define MAX_LOGIN_DEFAULT_ITEM_NUM	5

void EventLeftSelectFace()
{
	DWORD	l_dwTemp = _GetAvatar()->GetEquipItem( CDPT_FACE )->GetDTIndex() - 1;
	if( l_dwTemp < ( ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ]  ) ||
		_GetAvatar()->GetEquipItem( CDPT_FACE )->GetDTIndex()==0)
		l_dwTemp = ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ] + MAX_LOGIN_DEFAULT_ITEM_NUM -1;

	_GetAvatar()->PutOnEquipment( CDPT_FACE, l_dwTemp );
	_GetAvatar()->SetLightColor( D3DCOLOR_XRGB(128,128,128) );
}

void EventRightSelectFace()
{
	DWORD	l_dwTemp = _GetAvatar()->GetEquipItem( CDPT_FACE )->GetDTIndex() + 1;
	if( l_dwTemp >= ( ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ] + MAX_LOGIN_DEFAULT_ITEM_NUM ) )
		l_dwTemp = ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ];

	_GetAvatar()->PutOnEquipment( CDPT_FACE, l_dwTemp );
	_GetAvatar()->SetLightColor( D3DCOLOR_XRGB(128,128,128) );
}
void EventLeftSelectHead()
{
	DWORD	l_dwTemp = _GetAvatar()->GetEquipItem( CEPT_HAIR )->GetDTIndex() - 1;
	if( l_dwTemp <  ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ]||
		_GetAvatar()->GetEquipItem( CEPT_HAIR )->GetDTIndex()==0)
	{
		l_dwTemp = ( ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ] + MAX_LOGIN_DEFAULT_ITEM_NUM ) - 1 ;
	}
	_GetAvatar()->PutOnEquipment( CEPT_HAIR, l_dwTemp );
	_GetAvatar()->SetLightColor( D3DCOLOR_XRGB(128,128,128) );
}

void EventRightSelectHead()
{
	DWORD	l_dwTemp = _GetAvatar()->GetEquipItem( CEPT_HAIR )->GetDTIndex() + 1;
	if( l_dwTemp >= ( ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ] + MAX_LOGIN_DEFAULT_ITEM_NUM ) )
		l_dwTemp = ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ];

	_GetAvatar()->PutOnEquipment( CEPT_HAIR, l_dwTemp );
	_GetAvatar()->SetLightColor( D3DCOLOR_XRGB(128,128,128) );
}
void EventLeftSelectUpper()
{
	DWORD	l_dwTemp = _GetAvatar()->GetEquipItem( CDPT_UPPER_PART )->GetDTIndex() - 1;
	if( l_dwTemp < ( ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ]  )||
		_GetAvatar()->GetEquipItem( CDPT_UPPER_PART )->GetDTIndex()==0)
		l_dwTemp = ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ]+ MAX_LOGIN_DEFAULT_ITEM_NUM -1;

	_GetAvatar()->PutOnEquipment( CDPT_UPPER_PART, l_dwTemp );
	_GetAvatar()->SetLightColor( D3DCOLOR_XRGB(128,128,128) );
}
void EventRightSelectUpper()
{
	DWORD	l_dwTemp = _GetAvatar()->GetEquipItem( CDPT_UPPER_PART )->GetDTIndex() + 1;
	if( l_dwTemp >= ( ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ] + MAX_LOGIN_DEFAULT_ITEM_NUM ) )
		l_dwTemp = ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ];

	_GetAvatar()->PutOnEquipment( CDPT_UPPER_PART, l_dwTemp );
	_GetAvatar()->SetLightColor( D3DCOLOR_XRGB(128,128,128) );
}
void EventLeftSelectLower()
{
	DWORD	l_dwTemp = _GetAvatar()->GetEquipItem( CDPT_LOWER_PART )->GetDTIndex() - 1;
	if( l_dwTemp < ( ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ] )||
		_GetAvatar()->GetEquipItem( CDPT_LOWER_PART )->GetDTIndex()==0)
		l_dwTemp = ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ] + MAX_LOGIN_DEFAULT_ITEM_NUM-1;

	_GetAvatar()->PutOnEquipment( CDPT_LOWER_PART, l_dwTemp );
	_GetAvatar()->SetLightColor( D3DCOLOR_XRGB(128,128,128) );
}
void EventRightSelectLower()
{
	DWORD	l_dwTemp = _GetAvatar()->GetEquipItem( CDPT_LOWER_PART )->GetDTIndex() + 1;
	if( l_dwTemp >= ( ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ] + MAX_LOGIN_DEFAULT_ITEM_NUM ) )
		l_dwTemp = ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ];

	_GetAvatar()->PutOnEquipment( CDPT_LOWER_PART, l_dwTemp );
	_GetAvatar()->SetLightColor( D3DCOLOR_XRGB(128,128,128) );
}
void EventLeftSelectGloves()
{
	DWORD	l_dwTemp = _GetAvatar()->GetEquipItem( CDPT_GLOVES )->GetDTIndex() - 1;
	if( l_dwTemp < ( ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ]  ) ||
		_GetAvatar()->GetEquipItem( CDPT_GLOVES )->GetDTIndex()==0)
		l_dwTemp = ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ] + MAX_LOGIN_DEFAULT_ITEM_NUM -1;

	_GetAvatar()->PutOnEquipment( CDPT_GLOVES, l_dwTemp );
	_GetAvatar()->SetLightColor( D3DCOLOR_XRGB(128,128,128) );
}
void EventRightSelectGloves()
{
	DWORD	l_dwTemp = _GetAvatar()->GetEquipItem( CDPT_GLOVES )->GetDTIndex() + 1;
	if( l_dwTemp >= ( ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ] + MAX_LOGIN_DEFAULT_ITEM_NUM ) )
		l_dwTemp = ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ];

	_GetAvatar()->PutOnEquipment( CDPT_GLOVES, l_dwTemp );
	_GetAvatar()->SetLightColor( D3DCOLOR_XRGB(128,128,128) );
}
void EventLeftSelectShoes()
{
	DWORD	l_dwTemp = _GetAvatar()->GetEquipItem( CDPT_SHOES )->GetDTIndex() - 1;
	if( l_dwTemp < ( ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ]  ) ||
		_GetAvatar()->GetEquipItem( CDPT_SHOES )->GetDTIndex()==0)
		l_dwTemp = ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ] + MAX_LOGIN_DEFAULT_ITEM_NUM -1;

	_GetAvatar()->PutOnEquipment( CDPT_SHOES, l_dwTemp );
	_GetAvatar()->SetLightColor( D3DCOLOR_XRGB(128,128,128) );
}
void EventRightSelectShoes()
{
	DWORD	l_dwTemp = _GetAvatar()->GetEquipItem( CDPT_SHOES )->GetDTIndex() + 1;
	if( l_dwTemp >= ( ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ] + MAX_LOGIN_DEFAULT_ITEM_NUM ) )
		l_dwTemp = ID_DEFAULT_ARMOR[ _GetAvatar()->GetRace() ];

	_GetAvatar()->PutOnEquipment( CDPT_SHOES, l_dwTemp );
	_GetAvatar()->SetLightColor( D3DCOLOR_XRGB(128,128,128) );
}
/*
float CharacterAlphaProcess()
{
	DWORD			curtime = timeGetTime();

	float			delta = 0.0;
	if (m_CharShowDelay)
		delta = (float)(curtime - m_CharStartTime)/(float)m_CharShowDelay;
	if (delta>1.0) 
	{
		delta = 1.0;
		m_CharShowDelay		= 0;
		if (m_CharHide)
			m_CharacterAlpha	= 0;	
		else
			m_CharacterAlpha	= 1.0f;	
	}	
	else
	{
		if (m_CharHide)
			m_CharacterAlpha = delta;
		else
			m_CharacterAlpha = 1 - delta;
	}
	return m_CharacterAlpha;
}
void HideCharacter(DWORD time)
{
	m_CharHide		= true;
	m_CharShowDelay	= time;
	m_CharStartTime = timeGetTime();
}
void ShowCharacter(DWORD time)
{
	m_CharHide		= false;
	m_CharShowDelay	= time;
	m_CharStartTime = timeGetTime();
}

*/


CGP_LogIn::CGP_LogIn()
{
	Init();
}

CGP_LogIn::~CGP_LogIn()
{
	Destroy();
}

void
CGP_LogIn::Init( void )
{
	ZeroMemory( m_pInputString, GPL_MAX_ID_LENGTH * GPL_MAX_INPUT_FOCUS );

	m_dwInputFocus				= GPL_INPUT_ID;

	m_nMaxStringLength[0]		= GPL_MAX_ID_LENGTH;
	m_nMaxStringLength[1]		= GPL_MAX_PASSWORD_LENGTH;

	m_ptInputFocusPosition[0].x	= 400;
	m_ptInputFocusPosition[0].y = 250;

	m_ptInputFocusPosition[1].x = 400;
	m_ptInputFocusPosition[1].y = 300;
}

BOOL
CGP_LogIn::Create( void )
{
	if( m_bIsCreated )
		return TRUE;

	if( !CGameProgress::Create() )
		return FALSE;

	g_bIsSendQuitMsg		= FALSE;
	g_bIsEnterWorldServer	= FALSE;
	m_bRequestConnect		= FALSE;
	m_HaveCharacter[0]		= FALSE;
	m_HaveCharacter[1]		= FALSE;
	m_HaveCharacter[2]		= FALSE;

	//--------------------------------------------------------------------------
	// IME 세팅
	m_cIME.IMESetEnglish( g_hWnd );
	m_cIME.IMEEditInit( m_ptInputFocusPosition[m_dwInputFocus],
						12, D3DCOLOR_XRGB( 50, 200, 255 ) );
	m_cIME.SetCaretColor( D3DCOLOR_XRGB( 50, 50, 255 ) );

	//--------------------------------------------------------------------------

	_GetCharMgr()->CreateAvatar();
	_CreateAvatarDummy();

	//--------------------------------------------------------------------------

	//_GetUIScreen()->SetMouseState( CGUIMouse::NORMAL );

	//--------------------------------------------------------------------------

	if( !LoadData() )
		return FALSE;

	//--------------------------------------------------------------------------

	m_bIsCreated = TRUE;
	
	return TRUE;
}

BOOL
CGP_LogIn::Destroy( void )
{	
	if( !m_bIsCreated )
		return TRUE;

	if( !CGameProgress::Destroy() )
		return FALSE;

	//--------------------------------------------------------------------------

	D3DXMATRIX mat;
	float get_cam_pos[3];

	D3DXMatrixIdentity(&mat);
	if (GetD3dDevice())
	{
		::R3SetCameraMatrix(get_cam_pos,mat.m);
		SetFov(1.122f);
	}

	//--------------------------------------------------------------------------

	if( !UnloadData() )
		return FALSE;

	g_cMoveObjectMgr.RemoveAllObject();	

	m_bIsCreated = FALSE;

	return TRUE;
}

HRESULT
CGP_LogIn::FrameMove( void )
{
	// 네트웍 메세지 체크 - by J.S.H
	if( _GetNetworkMgr()->IsSendRequestMsg() )
	{
		if( !g_bIsSendQuitMsg )
		{
			if( _GetNetworkMgr()->IsPassed_RequestLimitTime() )
			{
				if( _IsOpenedDialogBox( NULL, MSGBOX_ID_GAMEPROGRESS_LOGIN ) )
					_CloseDialogBox( NULL, MSGBOX_ID_GAMEPROGRESS_LOGIN );

				_GetMainApp()->RequestQuitProgram( 2000, "서버에서 응답이 없습니다. 프로그램을 종료합니다.\n잠시후 다시 접속해 주십시오." );

				g_bIsSendQuitMsg = TRUE;
			}
		}
	}

	//--------------------------------------------------------------------------
	
	if (m_LoadEquipStart)
	{
		if (LoadDummyResouce())
		{
			m_LoadEquipStart	= false;
			m_LoadEquip			= true;
			m_CameraAni			= true;

			_CloseDialogBox();
#ifdef _DEVELOPER_
			PlayCamera(CAM_TOP_TO_LOGINA,m_Course);
			g_LoginState	=	LGS_CHARACTERLOBBY;
#endif 
		}
	}
	if (m_LoadDefaultStart)
	{
		if (LoadDefaultMesh())
		{
			m_LoadDefaultStart = false;
			m_LoadDefault	= true;
			_CloseDialogBox();
			//
			g_LoginState	= LGS_SELECTRACE;
			m_CameraAni		= true;
			//m_uiSelectCharacter.Close();
			UI_SetMoveWindow( SELECT_CHAR_WIN, FALSE );
			EventSelectBELLATOR();
			m_uiSelectRace.ChangeRace(CGUISelectRace::Bellato);
			PlayCamera(CAM_SEL_A_TO_SELRACE, m_Course);
			//
			
		}
		
	}
	
	///*MAYUN
	float get_mat[4][4];    //카메라 매트릭스
	m_CameraAni = m_AniCamera.PlayAniCamera(get_mat,1.0f);
	float get_cam_pos[3];
	
	::R3SetCameraMatrix(get_cam_pos,get_mat);
	memcpy(m_CurPos,get_cam_pos,sizeof(float)*3);
	//*/

	LoginSoundProcess();
	//
	FadeOutMp3(1000);
	//
	// 맵 렌더링전에 준비할것들...
	if( _GetLevel()->IsLoadedBsp() )
		_GetLevel()->FrameMove();

	//*/

	_GetUIScreen()->Update();	
	
	UI_ClosingWindow();


#ifdef	CHARACTERDRAW
	
	_GetAvatar()->Disable( ROSF_ANIMATION_INTERPOLATION );
	_GetAvatar()->SetAction( CAI_MOVE_STOP,TRUE );
	_GetAvatar()->FrameMove();
	_GetAvatar()->Animation();
	
	CharacterAlphaProcess();
	for (int i = 0;i <3; i++)
	{
		if ( _GetAvatarDummy(i)->GetIndex()!=ID_INVALID)
		{
			_GetAvatarDummy(i)->FrameMove();
			_GetAvatarDummy(i)->Animation();
		}
	}
	
#endif

	return S_OK;
}

HRESULT
CGP_LogIn::Render( void )
{
	_R3ENGINE_STATE *state = GetR3State();
	/*
	_GetCharIF()->BeginShadow( );
		_GetCharIF()->SetState();

			
			//GetD3dDevice()->SetRenderState( D3DRS_ZENABLE, FALSE );
			//
			for (int i = 0;i <3; i++)
			{
				if ( _GetAvatarDummy(i)->GetIndex()!=ID_INVALID)
				{
					D3DXMATRIX	mat;
					D3DXVECTOR3 vec(0,0,-1);
					
					D3DXVECTOR4 out;
					D3DXMatrixRotationY(&mat,D3DXToRadian(30 - i * 120));
					
					SetMainDirLight((D3DXVECTOR3)out);
					memcpy(&m_vecPos,CharacterTopPos[i],sizeof(float)*3);
					
					m_CharRot	= 180 - i * 120;

					
					_GetAvatarDummy(i)->SetPosition( m_vecPos );
					_GetAvatarDummy(i)->SetRotY( m_CharRot, FALSE );
					//_GetAvatarDummy(i)->Render();
					//
					CItem * l_pItem = NULL;
					CMesh * l_pMesh = NULL;
					CItem	l_cTempItem;

					for( int j = 0; j < MAX_EQUIP_PART; j++ )
					{
						

						l_pItem = _GetAvatarDummy(i)->GetEquipItem( j );

						// 아이템이 없고 RENDER_DEFAULT_MESH가 세팅되었을 경우에는 디폴트 아이템으로 적용시킨다.
						//if( !l_pItem && IsEnable( ROSF_RENDER_DEFAULT_MESH ) )
						{
							if( _GetAvatarDummy(i)->GetDefaultSettingItem( j, l_cTempItem ) )
							{
								_GetAvatarDummy(i)->SetEquipItem( j, l_cTempItem.GetDTIndex() );
								l_pItem = &l_cTempItem;
							}
						}

						// 렌더링
						if( l_pItem )
						{
							l_pMesh = l_pItem->GetMesh();
							
							l_pItem->SetPosition( m_vecPos );
							l_pItem->SetRotY( m_CharRot );
							_GetCharIF()->DrawCharacterShadow( l_pMesh,m_vecPos, m_CharRot,1.0f );
			
						}
					}
					//
				}
			}
			//GetD3dDevice()->SetRenderState( D3DRS_ZENABLE, TRUE );
		_GetCharIF()->UnSetState();
	_GetCharIF()->EndShadow( );
	*/
	
	if( R3BeginScene() )
	{
		R3ClearFrameBuffer();
		Vector3f l_vecCamPos;
		_GetCamera()->GetPosition( l_vecCamPos );
		_GetLand()->Render( l_vecCamPos );
		_GetLand()->RenderAlpha( l_vecCamPos );

		//_GetCharIF()->DrawShadow(800,600);
		
		//----------------------------------------------------------------------
		// 캐릭터 렌더링
#ifdef	CHARACTERDRAW
		_GetCharIF()->SetState();
		_GetItemEffectRenderer()->Clear();
		for (int i = 0;i <3; i++)
		{
			if ( _GetAvatarDummy(i)->GetIndex()!=ID_INVALID)
			{
				//--------------------------------------------------------------

				CEquipItem * pWeapon = _GetAvatarDummy(i)->GetEquipItem( IEPT_WEAPON );
				CEquipItem * pShield = _GetAvatarDummy(i)->GetEquipItem( IEPT_SHIELD );

				if( pWeapon )
				{
					pWeapon->SetFadeMode( FRM_NONE );
					pWeapon->SetAlphaDensity( 1.0f );
					pWeapon->Disable( ROSF_RENDER_ALPHA );
				}
				if( pShield )
				{
					pShield->SetFadeMode( FRM_NONE );
					pShield->SetAlphaDensity( 1.0f );
					pShield->Disable( ROSF_RENDER_ALPHA );
				}

				//--------------------------------------------------------------

				//
				D3DXMATRIX	mat;
				D3DXVECTOR3 vec(0,0,-1);
				D3DXVECTOR4 out;
				D3DXMatrixRotationY(&mat,D3DXToRadian(30 - i * 120));
				D3DXVec3Transform(&out,&vec,&mat);
				SetMainLight((D3DXVECTOR3)out);
				memcpy(&m_vecPos,CharacterTopPos[i],sizeof(float)*3);
				m_CharRot	= 180 - i * 120;

				_GetAvatarDummy(i)->SetPosition( m_vecPos );
				_GetAvatarDummy(i)->SetRotY( m_CharRot, FALSE );
				_GetAvatarDummy(i)->Render();
			}
		}
		CharacterAlphaProcess();
		SetCharacterBottom();
		_GetAvatar()->Enable( ROSF_RENDER_ALPHA );
		_GetAvatar()->SetAction( CAI_MOVE_STOP );
		_GetAvatar()->SetPosition( m_vecPos );
		_GetAvatar()->SetAlphaDensity(m_CharAlpha);
		_GetAvatar()->SetRotY( 0, FALSE );
		_GetAvatar()->Render();
		
		_GetItemEffectRenderer()->Render();
		g_AMeshManager.DrawAlpahMesh();
		_GetCharIF()->UnSetState();

#endif 
		//----------------------------------------------------------------------
		//------------------------------------------------------------------
		// User Interface
		//m_uiLoginWindow.SetARGB(0x77ffffff);
		if (m_HaveCharacter[m_Course])
		{
			SetCharacterInfo(m_Course);
		}
		else
		{			
			m_uiSelectCharacter.SetEmptyInfoBoard();
		}		
		_GetUIScreen()->Draw();

#ifndef _DEVELOPER_
		{
			if( ( m_LoadEquip ) && ( g_LoginState <= LGS_MOVEIN_WORLDSERVER ) )
			{
				{
					static DWORD	BEFTIME = 0;
					DWORD			CURTIME = timeGetTime();
					if(!BEFTIME)
					{
						BEFTIME		= CURTIME;
						m_CameraAni	= false;
					}
					DWORD			DELTA   = CURTIME - BEFTIME;

					const	static DWORD	LOGODELAY = 3000;
					BYTE flag = GetAsyncKeyState(VK_SPACE)|| GetAsyncKeyState(VK_LBUTTON) ;
					if ( flag ) {
						BEFTIME = CURTIME - LOGODELAY;
					};
					if( DELTA < LOGODELAY )
					{
						_R3ENGINE_STATE *state = GetR3State();
						int width	= 256;
						int height	= 256;
						int x = 10;//GetR3State()->mScreenXsize / 2 - width/2;
						int y = state->mScreenYsize - height - 10;
						//int y = 70;
						Draw2DTexture(x,y,x + width ,height + y,0xfeffffff,g_LogoTexture);
					}
					else
					{
						if( ( g_LoginState == LGS_ENTER_WORLDSERVER ) ||
							( g_LoginState == LGS_MOVEIN_WORLDSERVER ) )
						{
							PlayCamera(CAM_TOP_TO_LOGINA,m_Course);
							g_LoginState		= LGS_CHARACTERLOBBY;
							m_CameraAni			= true;
							BEFTIME				= 0;
						}
					}
				}
			}
		}
#endif
		// User Interface
		//------------------------------------------------------------------
		//

		if (g_LoginState==LGS_IDANDPASSWORD)
		{
			_R3ENGINE_STATE *state = GetR3State();
			int width	= 256;
			int height	= 256;
			int x = 10;//GetR3State()->mScreenXsize / 2 - width/2;
			int y = state->mScreenYsize - height - 10;
			Draw2DTexture(x,y,x + width ,height + y,0xfeffffff,g_LogoTexture);
		}
		DrawFadeScreen(); 

		//----------------------------------------------------------------------
		// version
		char l_pBuf[32];
		sprintf( l_pBuf, "Closed Beta v %.3f", _GetMainApp()->GetVersion() );		
		DrawR3Hangul( _GetUIScreen()->m_ptSize.x - ( strlen( l_pBuf ) * FONT_WIDTH ) - 20,
					  _GetUIScreen()->m_ptSize.y - 70,
					  l_pBuf, 0xFFFFC0C0, R3_HAN_OUTLINE );

		/*Screen Shot Test
		static float time = 0;
		static float sec = 0;
		static float bef = 0;
		static DWORD sccount = 0;
		sec	 += R3GetLoopTime();
		if ((sec - bef)>1.0f)
		{
			//sccount = 0;
			bef		= sec;
		}

		time += R3GetLoopTime();
		float deg = (1.0f/15);
		if (time> deg )
		{
			ScreenShot();
			time -= deg;
			sccount++;
		}
		sprintf( l_pBuf, "Screen Shot : %d", sccount );
		DrawR3Hangul( l_vecPos[0], _GetWindowClientRect()->bottom - 60, l_pBuf, 0xFFFFC0C0 );
		//*/
	}
	
	
	R3EndScene();

	return S_OK;
}

void
EP_SelectWorldServer( BYTE pi_byIndex )
{
	_GetNetworkMgr()->AccountMsg_SelectWorldRequest_cllo( pi_byIndex );

	m_uiWorldSelectWindow.Close();
}

void
EP_Disconnect_Account( BOOL pi_bIsDisconnect )
{
	// 기존 계정의 접속을 끊음
	if( pi_bIsDisconnect )
	{
		_GetNetworkMgr()->AccountMsg_PushCloseRequest_cllo();
	}
	// 게임 종료
	else
	{
		_GetMainApp()->RequestQuitProgram();
	}
}

void NetworkInput()
{
	//----------------------------------------------------------------------
	// NETWORK MESSAGE INPUT
	int		res = -1;
	switch ( res  = _GetNetworkMgr()->GetResultOfUserRegistration() )
	{
		case USER_REG_NOT_RECIVED:
			break;
		case USER_REG_SUCCESS:
			_GUIMessageBox( &m_uiLoginWindow, "ID가 성공적으로 만들어 졌습니다.",MSGBOXALPHA);
			break;
		case USER_REG_ASYNC_ERROR:
			_GUIMessageBox( &m_uiLoginWindow, "동기화 에러입니다.", MSGBOXALPHA);
			break;
		case USER_REG_DB_ERROR:
			_GUIMessageBox( &m_uiLoginWindow, "DB 에러 입니다.", MSGBOXALPHA);
			break;
		case USER_REG_ALREADY_LOGIN:
			_GUIMessageBox( &m_uiLoginWindow, "로그인 중에는 계정을 만들수가 없습니다.", MSGBOXALPHA);
			break;
		case USER_REG_ALREADY_EXIST_ACCOUNT:
			_GUIMessageBox( &m_uiLoginWindow, "이미 등록된 계정입니다.", MSGBOXALPHA);
			break;
	}
	switch( res = _GetNetworkMgr()->GetResultOfLogin() )
	{
		case USER_LOGIN_NOT_RECIVED:
			break;
		case USER_LOGIN_SUCCESS:
			if( g_LoginState == LGS_IDANDPASSWORD )
			{
				_GetNetworkMgr()->AccountMsg_WorldListRequest_cllo();
				m_uiLoginWindow.Close();
				g_LoginState = LGS_REQUEST_WORLDSERVERLIST;
			}
			break;
		case USER_LOGIN_INVALID_ID:
			_GUIMessageBox( &m_uiLoginWindow, "아이디가 유효하지 않습니다.",MSGBOXALPHA);
			m_uiLoginWindow.Open();
			break;
		case USER_LOGIN_INVALID_PASSWORD:
			_GUIMessageBox( &m_uiLoginWindow, "아이디 인증에 실패했습니다.\n잘못된 패스워드입니다.",MSGBOXALPHA);
			m_uiLoginWindow.Open();
			break;
		case USER_LOGIN_ASYNC_ERROR:
			_GUIMessageBox( &m_uiLoginWindow, "동기화 에러 입니다.",MSGBOXALPHA);
			m_uiLoginWindow.Open();
			break;
		case USER_LOGIN_ALREADY_LOGIN:
			//_GUIMessageBox( &m_uiLoginWindow, "이미 로그인 중입니다.",MSGBOXALPHA);
			//m_uiLoginWindow.Open();
			_GUIMessageBox( NULL, "계정이 이미 로그인 중입니다.\n기존의 접속을 끊으시겠습니까?",
							0xb0ffffff, MSGBOX_ID_DISCONNECT_ACCOUNT, DLG_STYLE_OK_CANCEL );
			g_LoginState = LGS_DISCONNECT_ACCOUNT;
			break;
		case USER_LOGIN_USER_FULL:
			_GUIMessageBox( &m_uiLoginWindow, "유저수가 너무 많습니다. ",MSGBOXALPHA);
			m_uiLoginWindow.Open();
			break;
		default :
			_GetMainApp()->RequestQuitProgram( 2000, "로그인을 실패했습니다.\n잠시후 다시 접속해 주십시오." );
			break;
	}
	switch( res = _GetNetworkMgr()->GetResultOfRequestForWorldServerList() )
	{
		case WORLD_LIST_NOT_RECIVED:
			break;
		case WORLD_LIST_SUCCESS:
			if ( g_LoginState == LGS_REQUEST_WORLDSERVERLIST )
			{
				char *	l_pBuf[MAX_WORLD_SERVER];
				BYTE	l_byIndex[MAX_WORLD_SERVER];

				BYTE				l_byWorldServerNum = _GetNetworkMgr()->GetWorldServerNum();
				WORLD_SERVER_INFO *	l_pWorldServerList = _GetNetworkMgr()->GetWorldServerInfoList();
				for( int i = 0, cnt = 0; i < l_byWorldServerNum; ++i )
				{
					if( l_pWorldServerList[i].m_bIsOpen )
					{
						l_pBuf[cnt]		= l_pWorldServerList[i].m_pName;
						l_byIndex[cnt++]= i;
					}
				}

				m_uiWorldSelectWindow.AddMapName( l_pBuf, l_byIndex, cnt );
				m_uiWorldSelectWindow.Open();

				g_LoginState = LGS_SELECT_WORLDSERVER;
			}
			break;
		default :
			_GUIMessageBox( &m_uiLoginWindow, "월드 서버 리스트 요청에 실패했습니다.",MSGBOXALPHA);
			g_LoginState = LGS_EXIT;
			break;
	}

	switch( res = _GetNetworkMgr()->GetResultOfSelectWorldServer() )
	{
		case WORLD_SELECT_NOT_RECIVED:
			break;
		case WORLD_SELECT_SUCCESS:
			if ( g_LoginState == LGS_SELECT_WORLDSERVER )
			{
				g_LoginState = LGS_CONNECT_WORLDSERVER;

				_GetNetworkMgr()->Close( GST_GATE );
			}
			break;
		default :
			_GUIMessageBox( &m_uiLoginWindow, "월드 서버 선택 요청에 실패했습니다.",MSGBOXALPHA);
			g_LoginState = LGS_EXIT;
			break;
	}

	switch( res = _GetNetworkMgr()->GetResultOfEnterTheWorldServer() )
	{
		case WORLD_ENTER_NOT_RECIVED:
			break;
		case WORLD_ENTER_SUCCESS:
			if ( g_LoginState == LGS_ENTER_WORLDSERVER )
			{
				// 월드 서버 접석에 성공 후 데이터 로딩
				_GetItemDataMgr()->LoadData();
				_GetCharResDataMgr()->LoadData();
				_GetCharDataMgr()->LoadData();
				
				_GetLand()->LoadData();

				//Bone Load
				for (int i = 0;i<5;i++)
				{
					CBone * l_pBone = _GetBoneMgr()->GetBone( RLI_PLAYER, i );
					if (!l_pBone)
						continue;
					_GetCharIF()->LoadRealData(	l_pBone,&_GetCharIF()->m_TM,true);
				}
				_GetAvatar()->LoadAnimaitonData();

				_GetNetworkMgr()->SystemMsg_RegedCharRequest_zone();
			}
			break;
		default :
			_GUIMessageBox( &m_uiLoginWindow, "월드 서버 접속에 실패했습니다.",MSGBOXALPHA);
			exit(0);
			//g_LoginState = LGS_IDANDPASSWORD;
			//m_uiLoginWindow.Open();

			break;
	}

	switch( res = _GetNetworkMgr()->GetResultOfRequestForDisconnectAccount() )
	{
		case WORLD_ENTER_NOT_RECIVED:
			break;
		case WORLD_ENTER_SUCCESS:
			if ( g_LoginState == LGS_DISCONNECT_ACCOUNT )
			{
				g_LoginState = LGS_IDANDPASSWORD;
				m_uiLoginWindow.Open();
			}
			break;
		default :
			_GUIMessageBox( &m_uiLoginWindow, "디스커넥트 요청을 실패했습니다.", MSGBOXALPHA );
			break;
	}
	
	switch ( res = _GetNetworkMgr()->GetResultOfUserInfo() )
	{
		case USER_INFO_NOT_RECIVED:
			{int a = 0;}
			break;
		case USER_INFO_SUCCESS:
			//if (g_LoginState == LGS_CHARACTERLOBBY)
			_GUIMessageBox( NULL, "잠시기다려주세요.",MSGBOXALPHA);
			m_LoadEquipStart = true;
			//_CloseDialogBox();
	
			
			//SetCharacterTop();
			break;

		case USER_INFO_ASYNC_ERROR :
		case USER_INFO_DB_ERROR :
		case USER_INFO_NOT_LOGIN :
		case USER_INFO_INVALID_INDEX :
		case USER_INFO_ALREADY_REGISTER :
			if ( g_LoginState == LGS_IDANDPASSWORD)
			{
				_GUIMessageBox( NULL, "잠시기다려주세요.",MSGBOXALPHA);
				m_LoadEquipStart = true;
				
				g_LoginState	=	LGS_CHARACTERLOBBY;
				m_CameraAni		=	true;
				PlayCamera(CAM_TOP_TO_LOGINA,m_Course);
				//m_AniCamera.SetPrePlayCamera(m_SelectCam,m_AniKeyFrame[0],m_AniKeyFrame[1],_CAM_FLAG_FINAL_STOP);
				/*
				switch(m_Course)
				{
				case A_COURSE:
					PlayCamera();
					break;
				case B_COURSE:
					PlayCamera();
					break;
				case C_COURSE:
					PlayCamera();
					break;
				}
				*/
			}
			m_HaveCharacter[m_Course]	=	false;	
			SetCharacterBottom();
			break;
	}
	switch (res = _GetNetworkMgr()->GetResultOfCharacterCreate() )
	{
		case CHAR_CREATE_NOT_RECIVED:
			break;
		case CHAR_CREATE_SUCCESS:
			//if (m_HaveCharacter[m_Course]) SetCharacterTop();
			{
				//
				m_CharAlphaFlag = false;
				//
				PlayCamera(CAM_CREATE_TO_A,m_Course);
				m_CameraAni		=	true;
				g_LoginState	=	LGS_CHARACTERLOBBY;
				//m_uiCreateCharacter.Close();
				UI_SetMoveWindow( CREATE_CHAR_WIN, FALSE );
				m_uiCreateCharacter.ResetName();
				///
				m_HaveCharacter[m_Course]= true;

				_GetAvatarDummy(m_Course)->SetObjectTypeID( OTI_CHAR );
				_GetAvatarDummy(m_Course)->SetCharTypeID( CTI_AVATAR );
				_GetAvatarDummy(m_Course)->SetIndex(m_Course);

				_GetAvatarDummy(m_Course)->SetName( _GetAvatar()->GetName() );
				_GetAvatarDummy(m_Course)->SetRace( _GetAvatar()->GetRace() );
				_GetAvatarDummy(m_Course)->SetDTIndex( _GetAvatar()->GetDTIndex() );

				_GetAvatarDummy(m_Course)->SetBone( _GetAvatar()->GetBoneID() );

				for( int i = 0; i < MAX_AVATAR_CLASS_NUM; ++i )
					_GetAvatarDummy(m_Course)->SetClassDTIndex( i, _GetAvatar()->GetClassDTIndex(i) ); 

				_GetAvatarDummy(m_Course)->SetLevel( 1 );
				_GetAvatarDummy(m_Course)->SetDalant( 0 );
				_GetAvatarDummy(m_Course)->SetGold( 0 );

				_GetAvatarDummy(m_Course)->SetHP( _GetAvatar()->GetHP());
				_GetAvatarDummy(m_Course)->SetSP( _GetAvatar()->GetSP() );
				_GetAvatarDummy(m_Course)->SetFP( _GetAvatar()->GetFP() );
				
				_GetAvatarDummy(m_Course)->SetDefaultItem( CEPT_UPPER_PART,_GetAvatar()->GetEquipItem( IEPT_UPPER_PART ));
				_GetAvatarDummy(m_Course)->PutOnEquipment( _GetAvatar()->GetEquipItem( IEPT_UPPER_PART ));
				_GetAvatarDummy(m_Course)->SetDefaultItem( CEPT_LOWER_PART,_GetAvatar()->GetEquipItem( IEPT_LOWER_PART ));
				_GetAvatarDummy(m_Course)->PutOnEquipment( _GetAvatar()->GetEquipItem( IEPT_LOWER_PART ));
				_GetAvatarDummy(m_Course)->SetDefaultItem( CEPT_GLOVES,_GetAvatar()->GetEquipItem( IEPT_GLOVES ));
				_GetAvatarDummy(m_Course)->PutOnEquipment( _GetAvatar()->GetEquipItem( IEPT_GLOVES ));
				_GetAvatarDummy(m_Course)->SetDefaultItem( CEPT_SHOES,_GetAvatar()->GetEquipItem( IEPT_SHOES ));
				_GetAvatarDummy(m_Course)->PutOnEquipment( _GetAvatar()->GetEquipItem( IEPT_SHOES ));
				_GetAvatarDummy(m_Course)->SetDefaultItem( CEPT_HAIR,_GetAvatar()->GetEquipItem( CEPT_HAIR ));
				_GetAvatarDummy(m_Course)->PutOnEquipment( _GetAvatar()->GetEquipItem( CEPT_HAIR ));
				_GetAvatarDummy(m_Course)->SetDefaultItem( CEPT_FACE,_GetAvatar()->GetEquipItem( IEPT_FACE ));
				_GetAvatarDummy(m_Course)->PutOnEquipment( _GetAvatar()->GetEquipItem( IEPT_FACE ));
				_GetAvatarDummy(m_Course)->PutOffEquipment( IEPT_HELMET );
				_GetAvatarDummy(m_Course)->PutOffEquipment( IEPT_WEAPON );
				_GetAvatarDummy(m_Course)->PutOffEquipment( IEPT_SHIELD );

				_GetAvatarDummy(m_Course)->SetCombatMode( CSI_COMBAT );

				_GetAvatarDummy(m_Course)->SetAction(CAI_MOVE_STOP);
				
				_GUIMessageBox( NULL, "캐릭터가 성공적으로 생성되었습니다.",MSGBOXALPHA);
			}
			break;
		case CHAR_CREATE_ALREADY_EXIST_AVATAR:
			{
				_GUIMessageBox( NULL, "이미 같은 캐릭터 이름이 있습니다",MSGBOXALPHA);
			}
			break;
		case CHAR_CREATE_ASYNC_ERROR:
		case CHAR_CREATE_DB_ERROR:
		case CHAR_CREATE_SLOT_ERROR:
		case CHAR_CREATE_NOT_LOGIN:
		case CHAR_CREATE_INVALID_INDEX:
			{
				_GUIMessageBox( NULL, "캐릭터 생성 에러",MSGBOXALPHA);
			}
			break;
		default : 
			{
				_GUIMessageBox( NULL, "캐릭터 생성 에러",MSGBOXALPHA);
			}
			break;
	}
	switch (res = _GetNetworkMgr()->GetResultOfCharacterSelect())
	{
		case CHAR_SELECT_NOT_RECIVED:
			break;
		case CHAR_SELECT_SUCCESS:
			{
				//_GetNetworkMgr()->MasterMsg_SelCharacterResultAck_clma(0);
				_GetNetworkMgr()->SystemMsg_MoveoutUserRequest_zone( ZONE_TYPE_MAP );
			}
			break;
		default:
			_GUIMessageBox( NULL, "캐릭터 선택 오류발생.",MSGBOXALPHA);
			break;
	}
	switch (res = _GetNetworkMgr()->GetResultOfCharacterDelete())
	{
		case CHAR_DELETE_NOT_RECIVED:
			break;
		case CHAR_DELETE_SUCCESS:
			//
			_GetAvatarDummy(m_Course)->SetIndex(ID_INVALID) ;
			{
				for( int i = 0; i < MAX_EQUIP_PART + 1; ++i )
					_GetAvatarDummy(m_Course)->PutOffEquipment( i );
				_GetAvatarDummy(m_Course)->RefreshConditionInfo();
			}

			_GUIMessageBox( NULL, "캐릭터가 삭제되었습니다.");
			//
			m_HaveCharacter[m_Course] = false;
			SetCharacterBottom();
			break;
		case CHAR_DELETE_ASYNC_ERROR:
		case CHAR_DELETE_DB_ERROR:
		case CHAR_DELETE_NOT_LOGIN:
		case CHAR_DELETE_EMPTY_SLOT:
		case CHAR_DELETE_INVALID_INDEX:
			_GUIMessageBox( NULL, "캐릭터 삭제 오류발생.",MSGBOXALPHA);
			break;
	}
	switch ( res = _GetNetworkMgr()->GetResultOfUserInfo())
	{
		case USER_INFO_NOT_RECIVED:
			{int a = 0;}
			break;
		case USER_INFO_SUCCESS:
			//if (g_LoginState == LGS_CHARACTERLOBBY)
			if (!m_HaveCharacter[m_Course])
			{
				/*
				m_HaveCharacter[m_Course]	=	true;	
				SetCharacterTop();
				*/
				if (_GetAvatarDummy(0)->GetIndex() != ID_INVALID)
				{
					_GetAvatarDummy(0)->SetCombatMode( CSI_COMBAT );
					m_HaveCharacter[0]	=	true;	
				}
				if (_GetAvatarDummy(1)->GetIndex() != ID_INVALID)
				{
					_GetAvatarDummy(1)->SetCombatMode( CSI_COMBAT );
					m_HaveCharacter[1]	=	true;	
				}
				if (_GetAvatarDummy(2)->GetIndex() != ID_INVALID)
				{
					_GetAvatarDummy(2)->SetCombatMode( CSI_COMBAT );
					m_HaveCharacter[2]	=	true;	
				}
			}
			if ( g_LoginState == LGS_ENTER_WORLDSERVER )
			//if ( g_LoginState == LGS_IDANDPASSWORD)
			{
				for (int i  = 0; i<3; i++)
				{
					if (_GetNetworkMgr()->GetLastestAvatarIndex()!=ID_INVALID)
					{
						 m_Course = (COURSE)_GetNetworkMgr()->GetLastestAvatarIndex();
					}
					else
					{
						 if (m_HaveCharacter[i])
						 {
							 m_Course = (COURSE)i;
							 break;
						 }
					}
				}
				
			}

			break;
		case USER_INFO_ASYNC_ERROR:
		case USER_INFO_DB_ERROR:
		case USER_INFO_NOT_LOGIN:
		case USER_INFO_INVALID_INDEX:
		case USER_INFO_ALREADY_REGISTER:
			_GUIMessageBox( NULL, "같은 이름의 캐릭터가 있습니다.",MSGBOXALPHA);
			break;
	}

	switch ( res = _GetNetworkMgr()->GetResultOfMoveOutFromServer() )
	{
		case MOVEOUT_FROM_SERVER_NOT_RECIVED:
			break;
		case MOVEOUT_FROM_SERVER_SUCCESS:
			if (m_HaveCharacter[m_Course])
			{
				PlayCamera(CAM_A_TO_JOIN,m_Course);

				m_MP3ElapseTime = timeGetTime();
				FadeOutSound();
				//
				//m_AniCamera.SetPrePlayCamera(m_SelectCam,m_AniKeyFrame[4],m_AniKeyFrame[5],_CAM_FLAG_FINAL_STOP);
				SetFadeOut(0x00, 1 ,1.0f);   	
				g_LoginState = LGS_EXIT;
				//m_uiSelectCharacter.Close();
				UI_SetMoveWindow( SELECT_CHAR_WIN, FALSE );

				//

				//---------------------------------------------------------------------
				// 아바타 세팅
				_GetAvatar()->SetName( _GetAvatarDummy(m_Course)->GetName() );
				_GetAvatar()->SetRace( _GetAvatarDummy(m_Course)->GetRace() );
				_GetAvatar()->SetDTIndex( _GetAvatarDummy(m_Course)->GetDTIndex() );

				_GetAvatar()->SetBone( _GetAvatarDummy(m_Course)->GetBoneID() );

				for( int i = 0; i < MAX_AVATAR_CLASS_NUM; ++i )
					_GetAvatar()->SetClassDTIndex( i, _GetAvatarDummy(m_Course)->GetClassDTIndex(i) );

				_GetAvatar()->SetLevel( _GetAvatarDummy(m_Course)->GetLevel() );
				_GetAvatar()->SetDalant( _GetAvatarDummy(m_Course)->GetDalant() );
				_GetAvatar()->SetGold( _GetAvatarDummy(m_Course)->GetGold() );

				for( i = 0; i < MAX_DEFAULT_PART; ++i )
					_GetAvatar()->SetDefaultItem( i, _GetAvatarDummy(m_Course)->GetDefaultItem( i ) );

				for( i = 0; i < MAX_EQUIP_PART + 1; ++i )	// +1은 hair
					_GetAvatar()->PutOnEquipment( _GetAvatarDummy(m_Course)->GetEquipItem( i ) );

				_GetNetworkMgr()->Close( GST_WORLD );
			}
			break;
	}

	// 캐릭터 선택부분이라면 월드 연결후 movein함수를 호출 => 이후 캐릭터 정보 요청
	if( _GetMainApp()->IsRequestLogout() )
	{
		switch ( res = _GetNetworkMgr()->GetResultOfMoveIntoServer() )
		{
			case MOVEINTO_SERVER_NOT_RECIVED :
				break;
			case MOVEINTO_SERVER_SUCCESS :
				_GetMainApp()->SetRequestLogout( FALSE ); // 이후부터는 동일하다.
				_GetNetworkMgr()->SystemMsg_RegedCharRequest_zone();
				break;
		}
	}

	// NETWORK MESSAGE INPUT
	//----------------------------------------------------------------------

	//RESET
	_GetNetworkMgr()->SetResultOfUserRegistration( USER_REG_NOT_RECIVED );
	_GetNetworkMgr()->SetResultOfLogin( USER_LOGIN_NOT_RECIVED );
	_GetNetworkMgr()->SetResultOfRequestForWorldServerList( WORLD_LIST_NOT_RECIVED );
	_GetNetworkMgr()->SetResultOfSelectWorldServer( WORLD_SELECT_NOT_RECIVED );
	_GetNetworkMgr()->SetResultOfEnterTheWorldServer( WORLD_ENTER_NOT_RECIVED );
	_GetNetworkMgr()->SetResultOfRequestForDisconnectAccount( WORLD_DISCONNECT_ACCOUNT_NOT_RECIVED );
	_GetNetworkMgr()->SetResultOfUserInfo( USER_INFO_NOT_RECIVED );
	_GetNetworkMgr()->SetResultOfCharacterCreate( CHAR_CREATE_NOT_RECIVED );
	_GetNetworkMgr()->SetResultOfCharacterSelect( CHAR_SELECT_NOT_RECIVED );
	_GetNetworkMgr()->SetResultOfCharacterDelete( CHAR_DELETE_NOT_RECIVED );
	_GetNetworkMgr()->SetResultOfMoveOutFromServer( MOVEOUT_FROM_SERVER_NOT_RECIVED );
	_GetNetworkMgr()->SetResultOfMoveIntoServer( MOVEINTO_SERVER_NOT_RECIVED );
}
BOOL
CGP_LogIn::InputProcess( void )
{
	//----------------------------------------------------------------------
	// User Interface
	_GetUIMouseDev()->Update();
	_GetUIScreen()->InputProc();

	// User Interface
	NetworkInput();
	
	static float endframe = 0;
	static endwav = true;
	
	if (m_CameraAni)
	{
		if (endwav)
		{
			float end= 0;
			float wavelen = 0.2f;
			float frame	=m_AniCamera.GetNowFrame();
			if ( m_AniCamKeyFrame[m_CurAniKey][2]<=m_AniCamKeyFrame[m_CurAniKey][3])
			{
				//end = m_AniCamKeyFrame[m_CurAniKey][3] - m_AniCamKeyFrame[m_CurAniKey][2];
				end = m_AniCamKeyFrame[m_CurAniKey][3];
				float max	= ((end)*160.0f /5.0)/1000.0f;
				if (frame > ( max - wavelen )&&frame<=max&&frame>0.0f )
				{
					m_CurAniKey = 0;
					PlayWave( CAM_END_WAV,0,1.0f,0.0f);	
					endwav = false;
					/*
					char buf[256];
					sprintf(buf,"M:%0.2f F:%0.2f\n",( max - wavelen ),frame);
					OutputDebugString(buf);
					*/
				}
			}
			else
			{
				//end = m_AniCamKeyFrame[m_CurAniKey][2] - m_AniCamKeyFrame[m_CurAniKey][3];
				end = m_AniCamKeyFrame[m_CurAniKey][3];
				float max = ((end)*160.0f /5.0)/1000.0f;
				//end = m_AniCamKeyFrame[m_CurAniKey][3];
				if (frame < ( max + wavelen ) &&frame>0.0f)
				{
					m_CurAniKey = 0;
					PlayWave( CAM_END_WAV,0,1.0f,0.0f);	
					endwav = false;
					//
					/*
					char buf[256];
					sprintf(buf,"M:%0.2f F:%0.2f\n",( max + wavelen ),frame);
					OutputDebugString(buf);
					*/
					//
				}
			}
			
		}
	}
	else
	{
		endframe = m_AniCamera.GetNowFrame();
		StopWave( CAM_MIDDLE_WAV );	
		
		if (!endwav)
		{
			/*
			char buf[256];
			sprintf(buf,"End:%0.2f\n",endframe);
			OutputDebugString(buf);
			*/
		}
		endwav = true;
	}
	if (!m_CameraAni&&g_LoginState == LGS_CHARACTERLOBBY	)
	{
		if( !m_uiSelectCharacter.IsShow() )
		{
			m_uiSelectCharacter.Open();	
			
			UI_SetMoveWindow( SELECT_CHAR_WIN, TRUE );
		}
	}
	
	if (!m_CameraAni&&g_LoginState == LGS_SELECTRACE	)
	{
		if( !m_uiSelectRace.IsShow() )
		{
			m_uiSelectRace.Open();		
			
			UI_SetMoveWindow( SELECT_RACE_WIN, TRUE );

			if( g_bUIDefaultStart )
			{
				EventSelectBELLATOR();
				m_uiSelectRace.ChangeRace(CGUISelectRace::Bellato);
				g_bUIDefaultStart = FALSE;
			}
		}
	}
	if (!m_CameraAni&&g_LoginState == LGS_SELECTATTRIBUTE	)
	{
		if( !m_uiCreateCharacter.IsShow() &&
			!m_uiSelectAttribute.IsShow() )
		{
			m_uiSelectAttribute.Open();

			UI_SetMoveWindow( SELECT_ATTR_WIN, TRUE );
		}
	}
	if (!m_CameraAni&&g_LoginState == LGS_CREATECHARACTER	)
	{
		m_CharAlphaFlag = true;

		if( !m_uiSelectAttribute.IsShow() &&
			!m_uiCreateCharacter.IsShow() )
		{
			m_uiCreateCharacter.Open();		

			UI_SetMoveWindow( CREATE_CHAR_WIN, TRUE );
		}
	}
	

#ifdef _DEVELOPER_
	// 캐릭터 선택부분부터 시작이 아니라면 로그인 창을 오픈한다.
	if( !_GetMainApp()->IsRequestLogout() )
	{
		// 게이트 서버 접속을 끊고, 월드 서버로 접속
		if( ( g_LoginState == LGS_CONNECT_WORLDSERVER ) && !_GetNetworkMgr()->IsConnectedToServer( GST_GATE ) )
		{
			_GetNetworkMgr()->Connect( GST_WORLD );

			g_LoginState = LGS_ENTER_WORLDSERVER;
		}
	}
#endif
	
	// 캐릭터 선택부분부터 시작이 아니라면 => 월드 서버에 연결이 후, SystemMsg_EnterWorldRequest_zone()을 호출
	if( !_GetMainApp()->IsRequestLogout() )
	{
		if( ( g_LoginState == LGS_ENTER_WORLDSERVER ) &&
			_GetNetworkMgr()->IsConnectedToServer( GST_WORLD ) && 
			!g_bIsEnterWorldServer )
		{
			SetCharacterBottom();
			_GetNetworkMgr()->SystemMsg_EnterWorldRequest_zone();
			g_bIsEnterWorldServer = TRUE;
		}
	}

	///*
	if( IsFadeOut() && ( g_LoginState == LGS_EXIT ) && ( !m_bRequestConnect ) )
	{
		st_PlayMP3.StopR3MP3();
		st_PlayMP3.ReleaesR3MP3();
		//Sleep( 2000 );
		//LoadR3MP3(".\\Snd\\BGM\\BellatoBattle.mp3");
		//PlayR3MP3();
		CloseLoginSound();

		if ( !_GetNetworkMgr()->IsConnectedToServer( GST_WORLD ) )
		{
			_GetNetworkMgr()->Connect( GST_ZONE );
			m_bRequestConnect = TRUE;
		}

	}//*/
	// 존 서버에 연결이 되었다면 다음 단계로 넘어간다.
	if( ( _GetNetworkMgr()->IsConnectedToServer( GST_ZONE ) ) )
	{
		m_dwInputFocus = GPL_INPUT_END;
		m_bExit = TRUE;
		_GetAvatar()->SetCombatMode( CSI_PEACE );	// 임시코드
		_GetAvatar()->SetLightColor( ID_INVALID );
	}
	//*/
	/*
	if ( g_LoginState!= LGS_CONNECTSERVER && g_LoginState!=LGS_EXIT)
	{
		if ( ( !_GetNetworkMgr()->IsConnectedToServer(GST_GATE) ) )
		{
			MessageBox(NULL,"서버와 연결이 종료되었습니다.","WARNING",MB_OK);
			exit( 0 );
		}
	}*/

	// 로그인 서버에 연결이 되었다면 다음 단계로 넘어간다.
	//if( ( GetNetworkMgr()->IsConnetedToLogInServer() ) && ( m_dwInputFocus == GPL_INPUT_END ) )
	if( m_dwInputFocus == GPL_INPUT_END )
	{
		m_bExit = TRUE;
	}

	return TRUE;
}

LRESULT
CGP_LogIn::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( _GetUIScreen() &&
		g_eMoveWinType == NONE_WIN )
		_GetUIScreen()->MsgProc( hWnd, uMsg, wParam, lParam );

	return 0;
}

BOOL
CGP_LogIn::LoadData( void )
{
	if( m_bIsLoadedData )
		return TRUE;


	
	//--------------------------------------------------------------------------

	LoadGameSetting( "./RF_Setting.ini" );
	
	//--------------------------------------------------------------------------

#ifdef _DEVELOPER_
	BYTE l_byServerType = GST_GATE;

	// 캐릭터 선택부분부터 시작이라면 GST_WORLD 로 세팅
	if( _GetMainApp()->IsRequestLogout() )
		l_byServerType = GST_WORLD;
#else
	BYTE l_byServerType = GST_WORLD;
#endif
#ifndef _DEVELOPER_
	_GetNetworkMgr()->Connect( l_byServerType );
	DWORD contime = timeGetTime();
	while(true)
	{
		DWORD	curtime = timeGetTime();
		_GetNetworkMgr()->OnLoop();

		if( _GetNetworkMgr()->IsConnectedToServer( l_byServerType ) )
			break;

		if( ( curtime - contime ) > 3000 )//3초간 연락이 없으면 끊는다.
		{
			_GUIMessageBox( &m_uiCreateCharacter, "서버에 접속할 수 없습니다.", MSGBOXALPHA );
			_GetMainApp()->RequestQuitProgram();
			return 0;
		}

		Sleep(4);
	}
#endif

	///*MAYUN
	//
	_GetCharIF()->SetMipMap(GetR3State()->mTextureDetail);

	//
	
	//MP3 LOAD
	st_PlayMP3.LoadR3MP3(".\\Snd\\BGM\\Title.mp3");
	st_PlayMP3.PlayR3MP3();
	
	// UI sprite
	UI_LoadSprite();	

//	_GetUIScreen()->Init();

	// User Interface
	m_uiLoginWindow.Init();
	m_uiSelectCharacter.Init();
	m_uiLoadingWindow.Init();
	m_uiSelectRace.Init();
	m_uiCreateCharacter.Init();
	m_uiSelectAttribute.Init();
	m_uiWorldSelectWindow.Init();
	m_uiWorldSelectWindow.SetUseForWorldSelect( TRUE );

	// register window at screen
	_GetUIScreen()->Add( &m_uiLoginWindow );
	_GetUIScreen()->Add( &m_uiSelectCharacter );
	_GetUIScreen()->Add( &m_uiLoadingWindow );
	_GetUIScreen()->Add( &m_uiSelectRace );
	_GetUIScreen()->Add( &m_uiCreateCharacter );	
	_GetUIScreen()->Add( &m_uiSelectAttribute );
	_GetUIScreen()->Add( &m_uiWorldSelectWindow );	
	
	// set window position	
	m_uiLoginWindow.SetPos( 0, 0 );
	m_uiSelectCharacter.SetPos( 0, 0 );
	m_uiSelectRace.SetPos( 0, 0 );	
	m_uiCreateCharacter.SetPos( _GetUIScreen()->m_ptSize.x - 200,
		( _GetUIScreen()->m_ptSize.y - m_uiCreateCharacter.m_ptSize.y ) * 0.4 );
	m_uiSelectAttribute.SetPos( 0,
		( _GetUIScreen()->m_ptSize.y - m_uiSelectAttribute.m_ptSize.y ) * 0.2 );

	POINT l_sSize;
	m_uiWorldSelectWindow.GetSize( l_sSize );
	m_uiWorldSelectWindow.SetPos( ( _GetUIScreen()->m_ptSize.x - l_sSize.x ) / 2, 100 );

	// loading 화면을 띄운다.
	m_uiLoadingWindow.Open();

	if( R3BeginScene() )
	{
		R3ClearFrameBuffer();
		/*
		char buf[256];
		sprintf( buf, "로딩 중 입니다. 잠시 기다려 주세요!!" );
		DrawR3Hangul( 300, 500, buf, D3DCOLOR_XRGB( 255, 100, 5 ) );
		*/		
		_GetUIScreen()->Draw();
		//SetFadeFactor(0x000000,0.5f);
		//DrawFadeScreen(); 
	}
	R3EndScene();
	GetD3dDevice()->Present( NULL, NULL, NULL, NULL );

	/////////////////////////////////////////////////////
	//_GetNetworkMgr()->ConnectToMasterServer();	
	
#ifdef _DEVELOPER_
	_GetNetworkMgr()->Connect( l_byServerType );
	DWORD contime = timeGetTime();
	while(true)
	{
		DWORD	curtime = timeGetTime();
		_GetNetworkMgr()->OnLoop();

		if( _GetNetworkMgr()->IsConnectedToServer( l_byServerType ) )
			break;

		if( ( curtime - contime ) > 3000 )//3초간 연락이 없으면 끊는다.
		{
			_GUIMessageBox( &m_uiCreateCharacter, "서버에 접속할 수 없습니다.", MSGBOXALPHA );
			_GetMainApp()->RequestQuitProgram();
			return 0;
		}

		Sleep(4);
	}
#endif
	//
#ifndef _DEVELOPER_	
	/*
	if (!CheckSocketModule()) 
	{
		_GetMainApp()->RequestQuitProgram();
		MessageBox(NULL,"Error : Socket Module",0,0);
		return 0;	
	}
	//*/
	

	char *check = CheckFiles();

    if ( check ) //무결성 검사
	{
		char buf[256];
		sprintf( buf, "%s 패치를 받아주세요",check);
		_GetMainApp()->RequestQuitProgram();
		MessageBox(NULL,buf,0,0);
		return 0;	
	}
#endif
	//
	/////////////////////////////////////////////////////
	_GetLand()->LoadLevel( ".\\map\\lobby\\lobby.BSP" );
	m_AniCamera.LoadAniCamera(".\\map\\lobby\\lobby.cam");
	
	//
	if ( FAILED(D3DXCreateTextureFromFile( GetD3dDevice(),".\\system\\titlelogo.tga", &g_LogoTexture )))
	{
		int a= 0;
	}
	//g_LogoTexture = R3LoadDDSAndTextureMem(".\\system\\logo.tga");

	//m_AniCamera.SetPrePlayCamera(m_SelectCam,5000,0);
	//m_AniCamera.SetPrePlayCamera(0,0,0,_CAM_FLAG_FINAL_STOP);
	srand(timeGetTime());
	m_RandomStart = rand()%2;
	DWORD	cam = 0;
	if ( m_RandomStart )
	{
		cam =m_AniCamera.GetCameraIndex("C_05");
	}
	else
	{
		cam =m_AniCamera.GetCameraIndex("C_01");
	}
	m_AniCamera.SetPrePlayCamera(cam,0,0,_CAM_FLAG_FINAL_STOP);
	//
	InitLoginSound();
	
	
	g_Particle.LoadParticleIni(".\\CHEF\\Particle.INI");
	g_Particle.LoadEntityIni(".\\CHEF\\Entity.INI");
	//
	g_EffectFileList.LoadEffectFileList(".\\CHEF\\EffectFileList.txt");
	g_PatternList.LoadEffectPatternList(".\\CHEF\\PatternList.txt");
	g_EffectItemList.LoadItemListFile(".\\CHEF\\ItemEffectList.txt");
	//
	g_EFFMonList.LoadEffectFileList(".\\CHEF\\Monster.ini");
	g_EFFUnitList.LoadEffectFileList(".\\CHEF\\Unit.ini");

	g_AniEff.LoadAEffectList(".\\CHEF\\recall.ini");

	

	

	//_beginthread( SaveScreenData, 0, (void *)NULL );	Fast ScreenShot
#ifdef	CHARACTERDRAW

	// 아바타 기타 세팅
	_GetAvatar()->SetLightColor( D3DCOLOR_XRGB(128,128,128) );

#endif

	//
	R3ClearFrameBuffer();
	GetD3dDevice()->Present( NULL, NULL, NULL, NULL );
	//
	m_uiLoadingWindow.Disable();
	_GetUIScreen()->Remove( &m_uiLoadingWindow );
	_GetUISpriteMgr()->UnloadSprite( SP_ID_LOADING );

	//
	SetFadeOut(0x00,0,1.0f);
	SetFadeIn(0x00,3.0,1.0f);
	//

	//로그인 윈도우를 보여준다
#ifdef _DEVELOPER_
	// 캐릭터 선택부분부터 시작이라면 LGS_MOVEIN_WORLDSERVER 로 세팅
	// 월드로 MoveIn 요청
	if( _GetMainApp()->IsRequestLogout() )
	{
		g_LoginState = LGS_MOVEIN_WORLDSERVER;
		_GetNetworkMgr()->SystemMsg_MoveinUserRequest_zone();
	}
	else
	{
		g_LoginState = LGS_IDANDPASSWORD;
		m_uiLoginWindow.Open();
	}
#else
	// 캐릭터 선택부분부터 시작이라면 LGS_MOVEIN_WORLDSERVER 로 세팅
	// 월드로 MoveIn 요청
	if( _GetMainApp()->IsRequestLogout() )
	{
		g_LoginState = LGS_MOVEIN_WORLDSERVER;
		_GetNetworkMgr()->SystemMsg_MoveinUserRequest_zone();
	}
	else
	{
		g_LoginState = LGS_ENTER_WORLDSERVER;
	}
#endif
	
	m_bIsLoadedData = TRUE;

	return TRUE;
}

BOOL
CGP_LogIn::UnloadData( void )
{
	if( !m_bIsLoadedData )
		return TRUE;

	if( g_LogoTexture )
	{
		g_LogoTexture->Release();
		g_LogoTexture=NULL;
	}
	m_AniCamera.ReleaseAniCamera();
	_GetLand()->UnloadLevel();
	_DestroyAvatarDummy();

	UI_UnloadSprite();

	m_bIsLoadedData = FALSE;
	_GetAvatar()->Enable( ROSF_ANIMATION_INTERPOLATION );
	return TRUE;
}

HRESULT
CGP_LogIn::RestoreDeviceObjects( void )
{
	_GetLand()->LoadLevel( ".\\map\\lobby\\lobby.BSP" );

	UI_LoadSprite();

	// sprite를 새로 로딩할 때마다 각 window마다 RestoreSprite()를 호출한다.
	_GetUIScreen()->RestoreSprite();
	
	return 0;
}

HRESULT
CGP_LogIn::InvalidateDeviceObjects( void )
{
	_GetLand()->UnloadLevel();

	UI_UnloadSprite();

	st_PlayMP3.StopR3MP3();
	st_PlayMP3.ReleaesR3MP3();
	CloseLoginSound();

	return 0;
}

void
CGP_LogIn::UI_LoadSprite( void )
{
	_GetUISpriteMgr()->UnloadSpriteAll();

	_GetUISpriteMgr()->LoadSprite( SP_ID_MOUSE );
	_GetUISpriteMgr()->LoadSprite( SP_ID_COMMON );
	_GetUISpriteMgr()->LoadSprite( SP_ID_LOGIN );
	_GetUISpriteMgr()->LoadSprite( SP_ID_LOADING );
	_GetUISpriteMgr()->LoadSprite( SP_ID_BELT ); // 월드 셀렉트 창을 위해 로딩	

	_GetUIScreen()->RestoreSprite();
}

void
CGP_LogIn::UI_UnloadSprite( void )
{
	_GetUISpriteMgr()->UnloadSpriteAll();	
}