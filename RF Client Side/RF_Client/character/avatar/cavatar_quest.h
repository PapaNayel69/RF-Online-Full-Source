////////////////////////////////////////////////////////////////////////////////
//
// Quest Header
//
////////////////////////////////////////////////////////////////////////////////

struct HSD_HINT;

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Structure          //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// QBI Flag ( QBIF_ )
enum { QBIF_BEGIN = 0x01, QBIF_FINISH = 0x02, QBIF_ORDER = 0x4 };

// Quest Behavior Infomation ( QBI_ )
struct QBI_BASE_DATA
{
	BYTE	m_byType;
	WORD	m_wID;
	BYTE	m_byFlag;					// QBI Flag

	QBI_BASE_DATA();
	~QBI_BASE_DATA();
	void	Init( void );
};

/*
struct QBI_MESSAGE_DATA : public QBI_BASE_DATA
{
	char *	m_pMessage;

	QBI_MESSAGE_DATA();
	~QBI_MESSAGE_DATA();
	void	Init( void );
};*/

struct QBI_MESSAGE_DATA : public QBI_BASE_DATA
{
	HSD_HINT * m_pMessage;

	QBI_MESSAGE_DATA();
	~QBI_MESSAGE_DATA();
	void	Init( void );
};

struct QBI_WAIT_DATA : public QBI_BASE_DATA
{
	enum { WAIT_INPUT, WAIT_TIME, WAIT_QBI };
	enum { WAIT_INPUT_NO, WAIT_INPUT_YES, WAIT_INPUT_PREV, WAIT_INPUT_NEXT, WAIT_INPUT_NONE };

	BYTE	m_byWaitType;
	DWORD	m_dwValue;
};

struct QBI_MOVE_DATA : public QBI_BASE_DATA
{
	enum { MOVE_POSITION, MOVE_DISTANCE, MOVE_PORTAL };

	BYTE	m_byMoveType;

	union
	{
		float		m_fTargetPos[3];
		DWORD		m_dwDistance;
		struct
		{
			BYTE	m_byMapIndex;
			BYTE	m_byPortalIndex;
		};
	};
};

struct QBI_POSITION_DATA : public QBI_BASE_DATA
{
	float	fPosition[3];
};

struct QBI_UI_DATA : public QBI_BASE_DATA
{
	enum { UI_INVENTORY, UI_PACK1, UI_PACK2, UI_PACK3, UI_PACK4, UI_PACK5 };

	BYTE	byWindowID;
};

struct QBI_UI_EQUIP_DATA : public QBI_BASE_DATA
{
	BYTE	byPackNo;
	BYTE	bySocketNo;
};

struct QBI_CURSOR_DATA : public QBI_BASE_DATA
{
	enum { MBTN_NONE, MBTN_LEFT, MBTN_RIGHT, MBTN_MIDDLE };
	enum { MPOS_LEFT, MPOS_RIGHT, MPOS_CENTER, MPOS_TOP, MPOS_BOTTOM };
	enum { IC_HOVER = 200, IC_EQUIP = 201 };

	union
	{
		struct { short	nOrgXY[2]; };
		struct { short	nOrgX, nOrgY; };
	};
	union
	{
		struct { short	nTarXY[2]; };
		struct { short	nTarX, nTarY; };
	};
	BYTE	byButton;

	BYTE	byWindowID;
	BYTE	bySocketNo;
};

struct QBI_COMBAT_DATA : public QBI_BASE_DATA
{
	BYTE	m_byAttackType;
	DWORD	m_dwSFIndex;

	DWORD	m_dwTarget[2];
};

//------------------------------------------------------------------------------

struct QUEST
{
	DWORD			m_dwID;
	char			m_pName[64];
	BYTE			m_byFlag;			// QBI Flag

	WORD			m_wCurQBINum;
	WORD			m_wTotalQBINum;
	QBI_BASE_DATA**	m_listQBI;

	QUEST();
	~QUEST();
	void	Init( void );

	QBI_BASE_DATA *	GetQBI( WORD pi_wID = ID_INVALID );
};

struct QUEST_INFO
{
	DWORD			m_dwCurQuestNum;
	DWORD			m_dwTotalQuestNum;
	QUEST **		m_listQuest;

	//--------------------------------------------------------------------------

	struct QUEST_STATE_VARIABLE
	{
		DWORD		m_dwStartTime;
		float		m_fTargetDistance;
		Vector3f	m_fCameraRotate;
		BYTE		m_byInputButton;

		BYTE		m_byAttackType;
		DWORD		m_dwSFIndex;
		DWORD		m_dwTarget[2];

		QBI_MESSAGE_DATA *	m_pCurDisplayMsg;
	} QSV;

	//--------------------------------------------------------------------------

	QUEST_INFO();
	~QUEST_INFO();
	void	Init( void );
	void	Destroy( void );

	QUEST *	GetQuest( DWORD pi_dwID = ID_INVALID );
	BOOL	RefreshQuest( DWORD pi_dwID = ID_INVALID );
	BOOL	LoadQuestInfoFile( char * pi_pFileName );
	BOOL	UnLoadQuestInfoFile( void );

	void	ProcessQuest( void );
	void	RenderCurMsg( void );

	void	SetCombatInfo( BYTE pi_byAttackType, DWORD pi_dwSFIndex, BYTE pi_byTargetType, DWORD pi_dwTargetDTIndex );

protected :
	void	ProcessQuest_StartQBI( QUEST * pi_pQuest, QBI_BASE_DATA * pi_pQBI );
	void	ProcessQuest_ProcessQBI( QUEST * pi_pQuest, QBI_BASE_DATA * pi_pQBI );
};

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

#define MAX_QBI_TYPE	( 28 )
enum
{
	QBI_MESSAGE, QBI_WAITING_FOR, QBI_MOVE, QBI_MOVE_RQ,
	QBI_SET_POSITION, QBI_SET_ROTATION, QBI_SET_CAM_ROTATION, QBI_CAM_ROTATE, QBI_CAM_ROTATE_RQ,
	QBI_OPEN_WINDOW, QBI_OPEN_WINDOW_RQ, QBI_CLOSE_WINDOW, QBI_CLOSE_WINDOW_RQ,
	QBI_PUT_ON_EQUIPMENT, QBI_PUT_ON_EQUIPMENT_RQ, QBI_PUT_OFF_EQUIPMENT, QBI_PUT_OFF_EQUIPMENT_RQ,
	QBI_SET_CURSOR_POS1, QBI_SET_CURSOR_POS2, QBI_SET_CURSOR_POS3, QBI_SET_CURSOR_POS2UI,
	QBI_MOVE_CURSOR1, QBI_MOVE_CURSOR2, QBI_MOVE_CURSOR3, QBI_MOVE_CURSOR2UI, QBI_MOUSE_CLICK,
	QBI_COMBAT, QBI_COMBAT_RQ
};

//============================================================================//
//                                  Boundary                                  //
//============================================================================//