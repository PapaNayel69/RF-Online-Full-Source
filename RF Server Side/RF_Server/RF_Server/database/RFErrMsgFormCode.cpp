#include "stdafx.h"
#include "CharacterDB.h"

static char g_szRFErrMsgFromCode[128];

char* GetMsgFromRFErrorCode(DWORD dwErrorCode)
{
	switch(dwErrorCode)
	{
	case RET_CODE_SUCCESS:				sprintf(g_szRFErrMsgFromCode, "성공"); break;
	case RET_CODE_ASYNC_ERR:			sprintf(g_szRFErrMsgFromCode, "동기 에러"); break;
	case RET_CODE_CLOSE_WORLD:			sprintf(g_szRFErrMsgFromCode, "월드접속이 끊긴상태"); break;
	case RET_CODE_BEFORE_LOGIN:			sprintf(g_szRFErrMsgFromCode, "아직 로그인하지않았음"); break;
	case RET_CODE_FAULT_SERIAL:			sprintf(g_szRFErrMsgFromCode, "월드시리얼이 다름"); break;
	case RET_CODE_ALREADY_LOGIN:		sprintf(g_szRFErrMsgFromCode, "이미 로그인햇음"); break;
	case RET_CODE_WRONG_USERID:			sprintf(g_szRFErrMsgFromCode, "계정이 틀림");	break;
	case RET_CODE_WRONG_PASSWD:			sprintf(g_szRFErrMsgFromCode, "비번이 틀림"); break;
	case RET_CODE_USER_FULL:			sprintf(g_szRFErrMsgFromCode, "월드에 접속자가 꽉참"); break;
	case RET_CODE_NONEXIST_WAIT:		sprintf(g_szRFErrMsgFromCode, "존이동중인자의 리스트에 없음"); break;
	case RET_CODE_NONEXIST_MAP:			sprintf(g_szRFErrMsgFromCode, "이동하고자하는 맵이없는것임"); break;
	case RET_CODE_ALREADY_REGED:		sprintf(g_szRFErrMsgFromCode, "이미 등록아바타에대한 정보를 받았음"); break;
	case RET_CODE_BEFORE_REGED:			sprintf(g_szRFErrMsgFromCode, "등록아바타에대한 정보를 받기전임"); break;
	case RET_CODE_SLOT_ERR:				sprintf(g_szRFErrMsgFromCode, "아바타 슬롯인덱스에대한에러"); break;
	case RET_CODE_CLASS_ERR:			sprintf(g_szRFErrMsgFromCode, "아바타 슬롯인덱스에대한에러"); break;
	case RET_CODE_EMPTY_SLOT:			sprintf(g_szRFErrMsgFromCode, "빈 아바타 슬롯인덱스임"); break;
	case RET_CODE_NOT_REGED:			sprintf(g_szRFErrMsgFromCode, "등록됀 아바타가 아님"); break;
	case RET_CODE_ALREADY_SEL:			sprintf(g_szRFErrMsgFromCode, "이미 아바타를 선택햇음"); break;	
	case RET_CODE_ALREADY_EXIT:			sprintf(g_szRFErrMsgFromCode, "이미 접속을 종료했음"); break;
	case RET_CODE_NONEXIST_ZONE:		sprintf(g_szRFErrMsgFromCode, "해당 존에 존재하지 않음"); break;
	case RET_CODE_NONNEXT_ZONE:			sprintf(g_szRFErrMsgFromCode, "이동할 존이 존재하지 않음"); break;
	case RET_CODE_ALREADY_MOVE_OUT:		sprintf(g_szRFErrMsgFromCode, "이미 이동요청을 햇음"); break;
	case RET_CODE_WRONG_MASTERKEY:		sprintf(g_szRFErrMsgFromCode, "마스터키가 틀림"); break;
	case RET_CODE_DB_OPEN_ERR:			sprintf(g_szRFErrMsgFromCode, "데이터베이스가 열리지 않음"); break;
	case RET_CODE_REGED_ACCOUNT:		sprintf(g_szRFErrMsgFromCode, "이미등록됀계정임"); break;
	case RET_CODE_DB_QUERY_ERR:			sprintf(g_szRFErrMsgFromCode, "데이터베이스 쿼리 실패함"); break;
	case RET_CODE_REGED_AVATOR:			sprintf(g_szRFErrMsgFromCode, "이미 등록됀 아바타명이 잇음"); break;
	case RET_CODE_FULL_LOGIN:			sprintf(g_szRFErrMsgFromCode, "서비스가능 로그인수를 넘엇음"); break;
	case RET_CODE_BEFORE_NOT_EXIST:		sprintf(g_szRFErrMsgFromCode, "강제로 끊을려고하는 먼저접속자가 이미 접속을 끊엇음"); break;
	case RET_CODE_ACCOUNT_NOT_SUIT:		sprintf(g_szRFErrMsgFromCode, "계정이 유효한 문자조합이 아니다."); break;
	case RET_CODE_NOT_OPEN_WORLD:		sprintf(g_szRFErrMsgFromCode, "선택한 월드가 닫힌 월드임"); break;
	case RET_CODE_NOT_EQUAL_VERSION:	sprintf(g_szRFErrMsgFromCode, "클라이언트프로그램의 버젼이 틀리다."); break; 
	case RET_CODE_BEFORE_WORLD_LIST:	sprintf(g_szRFErrMsgFromCode, "이미 월드리스트를 가져감"); break;
	case RET_CODE_NOT_WORLD_LIST:		sprintf(g_szRFErrMsgFromCode, "아직 월드리스트를 안가져감"); break;
	case RET_CODE_NOT_WAIT_MODE:		sprintf(g_szRFErrMsgFromCode, "서버이동 대기모드가 아님"); break;
	case RET_CODE_FAULT_ZONE_MOVE:		sprintf(g_szRFErrMsgFromCode, "존이동 실패"); break;
	case RET_CODE_ALREADY_SELECT_AVATOR:sprintf(g_szRFErrMsgFromCode, "아바타 이미 선택했음"); break;
	case RET_CODE_NONEXIST_DB_RECORD:	sprintf(g_szRFErrMsgFromCode, "데이터베이스에 없는 레코드"); break;
	case RET_CODE_NOTUSEABLE_AVATORNAME:sprintf(g_szRFErrMsgFromCode, "사용할수없는 아바타이름"); break;
	case RET_CODE_NOT_ADMIN:			sprintf(g_szRFErrMsgFromCode, "관리자가 아님"); break;
	case RET_CODE_BLOCK_AVATOR:			sprintf(g_szRFErrMsgFromCode, "블럭아바타"); break;
	case RET_CODE_INTERNAL_ERROR:		sprintf(g_szRFErrMsgFromCode, "포괄적인 내부에러"); break;
	case RET_CODE_NET_PROCESS_ERROR:	sprintf(g_szRFErrMsgFromCode, "네트웤명령상의 절차적 에러"); break;
	case RET_CODE_PROGRAM_EXIT:			sprintf(g_szRFErrMsgFromCode, "내부사정으로 프로그램 종료함"); break;
	default:							sprintf(g_szRFErrMsgFromCode, "무슨 에런지 모르것네 "); break;
	}

	return g_szRFErrMsgFromCode;
}
