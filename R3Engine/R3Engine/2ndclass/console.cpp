#include "CommonUtil.h"
#include "R3D3dtex.h"
#include "R3Input.h"
#include "Console.h"
#include "R3Render.h"
#include "R3Text.h"
#include "Level.h"
#include "AniCamera.h"
#include "R3EngineKernel.h"
#include "R3Util.h"
#include "RTMovie.h"
#include "Core.h"
#include <stdio.h>


//------속성..
#define _NONE	0
#define _BOOL	1
#define _FLOAT	2
#define _INT	3
#define _STRING	4

typedef struct{
	char name[14];
	float fValue;
	int attr;	
}_CON_OP;

_CON_OP stConOp[]={
	"none op"		,0			,_NONE,
	"list"			,1			,_NONE,
	"open"			,1			,_STRING,
	"fov"			,(3.141592f/(2.8f))		,_FLOAT,
	"open_cam"		,1			,_STRING,
	"all_event_ani"	,0			,_BOOL,
	"play_cam"		,0			,_BOOL,
	"screen_shot"	,0			,_NONE,
	"view_map_info"	,0			,_BOOL,
	"capture_cam"	,0			,_NONE,
	"fly"			,0			,_BOOL,
	"show_cface"	,0			,_BOOL,
	"show_leaf_box"	,0			,_BOOL,
	"gamma"			,1.0f		,_FLOAT,
	"fly_speed"		,4.0f		,_FLOAT,
	"show_lightmap"	,0			,_BOOL,
	"show_mat_box"	,0			,_BOOL,
};
static const int stOpNum = sizeof(stConOp)/sizeof(_CON_OP);
//-----------------



//static int stConsoleBackID=-1;
static LPDIRECT3DTEXTURE8 stConsoleTex;

static int stConsoleMode=0;
static int stConsoleScroll=0;
static float stConsolePos=0;
static float stConsoleAdd=0;

#define _MAX_QUEUE	100
#define _MAX_ONE_ROW	80

#define xRatio(a)	1.0f
#define yRatio(a)	1.0f	

static char stStringQueue[_MAX_QUEUE][_MAX_ONE_ROW],stNowString[_MAX_ONE_ROW];
static int stQPointer=0,stRowCnt;
static BOOL stOperation=FALSE;

//-------------------------------------static void ---------------------------------------
static void GetToken(char *buf)
{
	int buf_len=strlen(buf),i;

	strlwr(buf);	//일단 소문자로...
	for(i=0;i<buf_len;i++)
	{
		if(buf[i]==' ' || buf[i]=='\n')
		{
			buf[i]=0;
			break;
		}
	}
}

static float stRetNumToken;
static char stRetStringToken[256];
#define NONE_VALUE	-640000.0f

static float GetNumToken(char *numbuf)
{
	int buf_len=strlen(numbuf),i,j;
	float ret =NONE_VALUE;
	char buf[256];

	strcpy(buf,numbuf);
	strlwr(buf);	//일단 소문자로...
	for(i=0;i<buf_len+1;i++)
	{
		if(buf[i]==' ')
			break;
		if( buf[i]==0)
			return NONE_VALUE;
	}
	i++;
	for(j=i;j<buf_len;j++)
	{
		buf[j-i]=buf[j];
		if(buf[j]==' ' || buf[j]=='\n')
		{
			break;
		}
	}
	buf[j-i]=0;

	buf_len=strlen(buf);
	if( buf_len == 0 )
		return NONE_VALUE;

	for(i=0;i<buf_len;i++)
	{
		if(!( buf[i]=='.' || (buf[i] >= '0' && buf[i] <= '9')))
		{
			return NONE_VALUE;
		}
	}
	return (float)atof(buf);
}

static char *GetStringToken(char *strbuf)
{
	int buf_len=strlen(strbuf),i,j;
	float ret =NONE_VALUE;
	char buf[256];

	stRetStringToken[0] = NULL;
	strcpy(buf,strbuf);
	strlwr(buf);	//일단 소문자로...
	for(i=0;i<buf_len+1;i++)
	{
		if(buf[i]==' ')
			break;
		if( buf[i]==0)
			return NULL;
	}
	i++;
	for(j=i;j<buf_len;j++)
	{
		buf[j-i]=buf[j];
		if(buf[j]==' ' || buf[j]=='\n')
		{
			break;
		}
	}
	buf[j-i]=0;

	buf_len=strlen(buf);
	if( buf_len == 0 )
		return NULL;

	strcpy(stRetStringToken,buf);
	return stRetStringToken;
}

static int GetConsolOp(char *buf)
{
	int ret = OP_NON_OP,i;
	char t_buf[256];
	char t2_buf[256];
	strcpy(t_buf,buf);
	strcpy(t2_buf,buf);

	GetToken(t_buf);
	GetStringToken(t2_buf);
	stRetNumToken = GetNumToken(t2_buf);

	for(i=0; i<stOpNum; i++)
	{
		if(!strcmp(stConOp[i].name,t_buf))
		{
			return i;
		}
	}
	return ret;
}

static void DefaultPrint(int op)
{
	char buf[256];

	int gop = (int)(stConOp[op].fValue-(int)(stConOp[op].fValue)*1000);
	if( gop )
		sprintf(buf,"%s Value '%.3f'",stConOp[op].name,stConOp[op].fValue);
	else
		sprintf(buf,"%s Value '%d'",stConOp[op].name,stConOp[op].fValue);
	InsertConsoleStringQ(buf);
}


static int stOpStack=OP_NON_OP;
void SetOpStack(int op)
{
	stOpStack = op;
}
static void RunOperation(int op);
CLevel *stCLevel;

void ProgressConsole(CLevel *level)
{
	int i;

    GetD3dDevice()->SetRenderState( D3DRS_FOGENABLE, FALSE );

	stCLevel=level;
//----------- op code 수행...
	if( stOpStack != OP_NON_OP )	//스택먼저...
	{
		int temp = stOpStack;
		stOpStack = OP_NON_OP;
		RunOperation(temp);
	}
	else
	if(stOperation)	//콘솔 명령어 수행...
	{
		RunOperation(GetConsolOp(stNowString));
	}
//--------------------------------------------

	_R3ENGINE_STATE *state= GetR3State();

	float console_xl = state->mScreenXsize;
	float console_yl = state->mScreenYsize;

/*
	for(i=0; i<256; i++)	//키의 아스키를 알아낸다.
	{
		if( gAsci[i] )
			int dd=0;
	}
*/

	if( gAsci[VK_WAVE] )
	{
		gAsci[VK_WAVE]=0;
		stConsoleMode=!stConsoleMode;
		if( stConsoleMode )
		{
			stConsoleAdd=1;
			stConsoleScroll=1;
		}
		else
		{
			stConsoleAdd=-1;
			memset(stNowString,0,_MAX_ONE_ROW);
			stRowCnt=0;
		}
	}
	float add=stConsoleAdd*R3GetLoopTime()*3.0f;
	stConsolePos += add;
	if( stConsoleAdd > 0 )
	{
		if(stConsolePos > 1)
			stConsolePos=1.0f;
	}
	if( stConsoleAdd < 0 )
	{
		if(stConsolePos < 0)
		{
			stConsolePos=0.0f;
			stConsoleScroll=0;
		}
	}

	int key=GetCharFromKey();	//키를 받아온다.
	if( stConsoleScroll )	//콘솔이 작동된다.
	{
		// 받은 키값을 척척..
		if( key )
		{
			switch(key)
			{
				case ' ':
					if( stRowCnt < _MAX_ONE_ROW-1)
					{
						//memcpy(,_MAX_ONE_ROW);
						stNowString[stRowCnt++] = ' ';
					}
					break;
				case -1://백스페이스
					if( stRowCnt > 0)
					{
						stRowCnt--;
						stNowString[stRowCnt] = NULL;
					}
					break;
				case -2://DELETE
					if( stRowCnt > 0)
						stRowCnt--;
					break;
				case -3://ENTER
					stOperation = TRUE;
					break;
				case -4:
					if( stRowCnt > 0)
						stRowCnt--;
					break;
				case -5:
					if( stRowCnt < _MAX_ONE_ROW-1)
						stRowCnt++;
					break;
				case -6:
					break;
				case -7:
					break;
				default:
					if( stRowCnt < _MAX_ONE_ROW-1)
					{
						stNowString[stRowCnt++] = key;
					}
			}
		}

		//DWORD mode;
	    if( SUCCEEDED( GetD3dDevice()->BeginScene() ) )
		{
			float ypos=-console_yl+console_yl*stConsolePos;

//			Draw2DSprite(0,ypos, console_xl, console_yl/2, stConsoleBackID,0xffffffff);
			Draw2DSprite(0,ypos, console_xl, console_yl/2, (void *)stConsoleTex,0xffffffff);

			for(i=0; i<_MAX_QUEUE;i++)	//큐에 저장된리스트들을 뿌린다.
			{
				int q_num = (_MAX_QUEUE-i+stQPointer)%_MAX_QUEUE;
				if( stStringQueue[q_num][0])
				{
					float y_pos = ypos+console_yl/2.0f-20-16*i;
					if( y_pos > -16 )
					{
						DrawR3Hangul( 8, (LONG)y_pos, ">",0xff00ffff );
						DrawR3Hangul( 16, (LONG)y_pos, stStringQueue[q_num],0xffffffff );
					}
				}
			}
			DrawR3Hangul( 8,(LONG)(ypos+console_yl/2.0f - 20), ">",0xff00ffff );
			DrawR3Hangul( 16, (LONG)(ypos+console_yl/2.0f - 20), stNowString,0xffffffff );
			
			//커서...
			DrawR3Hangul( 16+6*stRowCnt, (LONG)(ypos+console_yl/2.0f - 20.0f), "_",0xff00ffff );
//			GetD3dDevice()->SetRenderState( D3DRS_CULLMODE,   mode );
	        GetD3dDevice()->EndScene();// End the scene.
		}
		
	}
}
static int stOnePass=0;

static void RunOperation(int op)
{
	int i;
	char buf[256];
	char buf1[256];
	_R3ENGINE_STATE *state = GetR3State();
	DWORD flag;

	switch( op )
	{
		case OP_FLY_SPEED:
			if( NONE_VALUE == stRetNumToken )
				DefaultPrint(op);
			else
			{
				state = GetR3State();
				state->mFlySpeed= stRetNumToken;
				stConOp[op].fValue=stRetNumToken;
			}
			break;

		case OP_GAMMA:
			if( NONE_VALUE == stRetNumToken )
				DefaultPrint(op);
			else
			{
				state->mGamma = stRetNumToken;
				SetGamma(state->mGamma);
				stConOp[op].fValue=stRetNumToken;
			}
			break;
		case OP_FLY:
			if( stRetNumToken != stConOp[op].fValue )
			{
				if( stConOp[op].fValue == 0)
				{
					InsertConsoleStringQ("fly mode!");
					stConOp[op].fValue = 1;
				}
				else
				{
					InsertConsoleStringQ("mouse mode!");
					stConOp[op].fValue = 0;
				}
			}
			break;
		case 0xffffff:	//캡춰중...
			SetOpStack(0xffffff);
			if( stOnePass == 1 )
			{
				stCLevel->ScreenShot();
			}
			if( stOnePass == 0 )
			{
				stOnePass=1;
				//R3SetMinFPS(30);	//최소 프래임 레이트를 정의한다.
				//R3SetTime(0);
			}
//			if( stOnePass == -1 )
			if( !IsRTMoviePlaying())
			{
				stOnePass=0;
				SetOpStack(OP_NON_OP);
				InsertConsoleStringQ("End capture screen from camera ani! ");
			}
/*
			if(IsAniCameraState() == ANI_END_CAM && stOnePass == 1 )
			{
				stOnePass=-1;
			}*/
			break;
		case OP_CAPTURE_CAM:
			if( IsLoadedRTCamera() == FALSE && IsLoadedRTMovie() == FALSE )
			{
				InsertConsoleStringQ("not Loaded Play Animation Camera.");
				break;
			}
			stConOp[OP_PLAY_CAM].fValue = 1;	//플레이카메라 모드로..
			InsertConsoleStringQ("Start capture screen from camera ani! ");
			flag = RTMovieGetState();
			flag &= (~RTM_STATE_NO_CAMERAMOVE);
			RTMovieSetState(flag);
			RTMovieStartPlay(TRUE);
//			stCLevel->mAutoAniCam.SetPrePlayCamera(0);
			
			SetOpStack(0xffffff);
			R3SetMinFPS(30);	//최소 프래임 레이트를 정의한다.
			//R3SetTime(0);
			break;
		case OP_VIEW_MAP_INFO:
			if( stRetNumToken != stConOp[op].fValue )
			{
				if( stConOp[op].fValue == 0)
				{
					InsertConsoleStringQ("display map info!");
					stConOp[op].fValue = 1;
				}
				else
				{
					InsertConsoleStringQ("Hide map info!");
					stConOp[op].fValue = 0;
				}
			}
			break;
		case OP_SCREEN_SHOT:
			InsertConsoleStringQ("capture screen!");
			stCLevel->ScreenShot();
			break;
		case OP_PLAY_CAM:
			if( NONE_VALUE == stRetNumToken )
			{
				DefaultPrint(op);
				break;
			}
			/*
			if( stCLevel->mAutoAniCam.IsLoadedAniCamera() == FALSE )
			{
				InsertConsoleStringQ("not Loaded Play Animation Camera.");
				break;
			}
			if( stRetNumToken != stConOp[op].fValue )
			{
				if( stConOp[op].fValue == 0)
				{
					InsertConsoleStringQ("Play Animation Camera.");
					stConOp[op].fValue = 1;
					stCLevel->mAutoAniCam.SetPrePlayCamera(0);
				}
				else
				{
					InsertConsoleStringQ("Stop Animation Camera.");
					stConOp[op].fValue = 0;
				}
			}
			else
			*/

			if( IsLoadedRTCamera() == FALSE && IsLoadedRTMovie() == FALSE )
			{
				InsertConsoleStringQ("not Loaded Play Animation Camera.");
				break;
			}
			if( stRetNumToken != stConOp[op].fValue )
			{
				if( stConOp[op].fValue == 0)
				{
					InsertConsoleStringQ("Play Animation Camera.");
					stConOp[op].fValue = 1;
					DWORD flag = RTMovieGetState();
					flag &= (~RTM_STATE_NO_CAMERAMOVE);
					RTMovieSetState(flag);
					RTMovieStartPlay(TRUE);
				}
				else
				{
					DWORD flag = RTMovieGetState();
					flag |= RTM_STATE_NO_CAMERAMOVE;
					RTMovieSetState(flag);	//카메라를 끈다.
					RTMovieStartPlay(TRUE);	//다른 액션은 구동 시키려구
					InsertConsoleStringQ("Stop Animation Camera.");
					stConOp[op].fValue = 0;
				}
			}
			else
			if( stRetNumToken == 1)
				InsertConsoleStringQ("Now Play Animation Camera.");
			else
				InsertConsoleStringQ("Now Stop Animation Camera.");
			break;
		case OP_ALL_EVENT_ID:
//			stCLevel->ReLoadAllMaterial();
			if( stRetNumToken != stConOp[op].fValue )
			{
				if( stConOp[op].fValue == 0)
				{
					InsertConsoleStringQ("Play Front Event Animation.");
					stCLevel->mBsp->SetAllAnimationState(_EVENT_ANI_STATE_FRONT_START);
					stConOp[op].fValue = 1;
				}
				else
				{
					InsertConsoleStringQ("Play Back Event Animation.");
					stCLevel->mBsp->SetAllAnimationState(_EVENT_ANI_STATE_BACK_START);
					stConOp[op].fValue = 0;
				}
			}
			else
			if( stRetNumToken == 1)
				InsertConsoleStringQ("Now Play Front Event Animation.");
			else
				InsertConsoleStringQ("Now Play Back Event Animation.");
			break;
		case OP_OPEN_CAM:
			if( stRetStringToken[0] == NULL )
			{
				sprintf(buf,"open_cam filename");
				InsertConsoleStringQ(buf);
				break;
			}
			strcpy(buf,".\\map\\");
			strcat(buf,stRetStringToken);
			strcat(buf,".cam");
			if( IsExistFile(buf) == NULL )
			{
				sprintf(buf1,"can't open %s",buf);
				InsertConsoleStringQ(buf1);
				break;
			}
			sprintf(buf1,"%s opened!",buf);
			InsertConsoleStringQ(buf1);
			stCLevel->mAutoAniCam.ReleaseAniCamera();
			stCLevel->mAutoAniCam.LoadAniCamera(buf);
			break;
		case OP_OPEN:
			if( stRetStringToken[0] != NULL )
			{
				strcpy(buf,".\\map\\");
				strcat(buf,stRetStringToken);
				strcat(buf,".bsp");
			}
			else
			{
				//--------------------- Bsp 파일 오픈----------------------
				static OPENFILENAME ofn; // zero the ofn members out
				char name[256]=".\\Map\\test.bsp";

				ofn.lStructSize = sizeof(ofn);
				ofn.lpstrFile = name;
				ofn.nMaxFile = sizeof(name);
				ofn.lpstrFilter = "R3BSP (*.BSP)\0*.BSP\0All (*.*)\0*.*\0";
				ofn.lpstrTitle = "Select BSP file";
				ofn.Flags = OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

				if (!GetOpenFileName(&ofn))
				{
					sprintf(buf,"can't open");
					InsertConsoleStringQ(buf);
					break;
				}
				strcpy(buf,name);
				//---------------------------------------------------------
			}
			if( IsExistFile(buf) == NULL )
			{
				sprintf(buf,"can't open %s",buf);
				InsertConsoleStringQ(buf);
				break;
			}
			ReleaseConsole();
			R3ReleaseTextTexture();	//한글 텍스쳐내리고
			R3InvalidateDevice();	//텍스쳐 메모리 내리고
			
			stCLevel->ReleaseLevel();		//맵과 기타 등등 레벨을 릴리즈.
			InitConsole();
			R3LoadTextTexture();	//텍스트도 텍스쳐메모리를 쓴다. 올리고내려줘야된다.
			stCLevel->LoadLevel(buf);		//맵과 기타 등등 레벨을 로드한다.
			R3RestoreDevice();				//텍스쳐 전부 올리고..
			sprintf(buf,"%s Load Ok",buf);
			InsertConsoleStringQ(buf);
			break;

		case OP_FOV:
			if( NONE_VALUE == stRetNumToken )
				DefaultPrint(op);
			else
			{
				stConOp[op].fValue=stRetNumToken;
				SetFov(stRetNumToken);
			}
			break;
		case OP_LIST:
			sprintf(buf,"------ OP list ------");
			InsertConsoleStringQ(buf);
			for(i=1; i<stOpNum; i++)
			{
				InsertConsoleStringQ(stConOp[i].name);
			}
			sprintf(buf,"-------  End  -------");
			InsertConsoleStringQ(buf);
			break;
		case OP_NON_OP:
			sprintf(buf," '%s' is not Op code!",stNowString);
			InsertConsoleStringQ(buf);
			break;
		case OP_SHOW_LIGHTMAP:
			state->mShowLightMap=!state->mShowLightMap;
		default:
			//BOOL type은 여기서 처리하자.
			if( stRetNumToken != stConOp[op].fValue )
			{
				if( stConOp[op].fValue == 0)
					stConOp[op].fValue = 1;
				else
					stConOp[op].fValue = 0;

				sprintf(buf," '%s' is '%d'Value!",stConOp[op].name,(int)stConOp[op].fValue);
				InsertConsoleStringQ(buf);
			}
			break;
	}
	memset(stNowString,0,_MAX_ONE_ROW);
	stRowCnt=0;
	stOperation = FALSE;
}

static int stFunctionKey[24];	//f12까지만. 값은 op인덱스
//-------------extern void 

void InitConsole()
{
	if( !stConsoleTex )
		stConsoleTex = R3LoadDDSAndTextureMem(".\\System\\console.jpg");
//	stConsoleBackID =R3GetPreTextureId(".\\System\\console.jpg");//처음에 로딩할 텍스쳐 아이디를 얻어낸다.실제로 텍스쳐 메모리에 얹지는 않는다.
//	R3LoadTextureMem(stConsoleBackID);	//특정 텍스쳐를 텍스쳐메모리에 얹는다.
}
void ReleaseConsole()
{
	if( stConsoleTex )
	{
		stConsoleTex->Release();
		stConsoleTex=NULL;
	}
//	R3ReleaseTextureMem(stConsoleBackID);	//특정 텍스쳐를 텍스쳐메모리에 내린다.
//	stConsoleBackID = -1;
}

BOOL GetConsoleStateB(int op)
{
	if( stConOp[op].fValue == 0 )
		return FALSE;
	else
		return TRUE;
}
void SetConsoleStateB(int op,BOOL b_value)
{
	if( b_value == FALSE )
		stConOp[op].fValue = 0;
	else
		stConOp[op].fValue = 1;
}
void InsertConsoleStringQ(char *buf)
{
	memcpy(stStringQueue[stQPointer],buf,_MAX_ONE_ROW);
	stQPointer++;
	stQPointer%=_MAX_QUEUE;
}


//---펑션키 정의
void InitFunctionKey(char *name)
{
	char file[256]=".\\System\\",hole[256];
	int i;

	memset(stFunctionKey,0,sizeof(int)*12);
	if(name==NULL)
		strcpy(file,".\\system\\default.cfg");
	else
		strcat(file,name);
	FILE *fp;

	if( (fp = fopen(file,"rt")) == NULL )
	{
		InsertConsoleStringQ("can't open hotkey!");		
		return;
	}
	while( 1 )
	{
		if( fscanf(fp,"%s",&hole[0])==EOF)		
			break;

		if( !strcmp("*BindHotKey",hole ) )
		{
			char cmp_buf[256];

			while( 1 )
			{
				fscanf(fp,"%s",&hole[0]);
				if (!strcmp(hole,"}"))					
					break;
				strupr(hole);
				if( hole[0] == 'F' )
				{
					for(i=0; i<12; i++)
					{
						sprintf(cmp_buf,"F%1d",i+1);
						if (!strcmp(hole,cmp_buf))
						{
							fscanf(fp,"%s",&hole[0]);	//어떤건지 읽고...
							stFunctionKey[i] = GetConsolOp(hole);
							break;
						}
					}
				}
			}
		}
	}

	fclose(fp);
}
void ReleaseFunctionKey()
{
	memset(stFunctionKey,0,sizeof(int)*12);
}
void ProgressFunctionKey()
{
	int i;

	for(i=0; i<12; i++)
	{
		if( gAsci[VK_F1+i] )		//펑션키가 눌러져 있으면 
		{
			gAsci[VK_F1+i]=0;
			if( stFunctionKey[i] > 0)	//핫키가 있다.
			{
				if( stConOp[stFunctionKey[i]].attr == _NONE )
				{
					SetOpStack(stFunctionKey[i]);
				}
				else
				if( stConOp[stFunctionKey[i]].attr == _BOOL )
				{
					if( stConOp[stFunctionKey[i]].fValue == 0 )
						stRetNumToken=1;
					else
						stRetNumToken=0;
					SetOpStack(stFunctionKey[i]);
				}
				else
				{
					if( !strcmp(stConOp[stFunctionKey[i]].name,"open") )
					{
						stRetStringToken[0]=NULL;
						SetOpStack(stFunctionKey[i]);
					}
					else
						InsertConsoleStringQ("can't function key!");
				}
			}
		}
	}
}
