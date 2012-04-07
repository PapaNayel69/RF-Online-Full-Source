#include "MoveEffectManager.h"
#include "EffectUtil.h"
#include "EffectCore.h"
#include "MonsterSpecialEffect.h"
#include "PlayerSpecialEffect.h"

//---------------------------------- Move Effect Manager ---------------------------------------------
#define _MAX_MOVE_EFFECT_MANAGER	64	//최대동시 움직이는 이펙트 관리 


#define _ARROW_PRE_DIST	3

#define _OUT_STRING_COLOR	0xff6f2000
#define _ROOT_FORCE_SOUND	100

extern DWORD stDamageIndexTable[];

//움직이는 스타일의 효과관련 매니져이다.
class CMoveEffectManager{
public:
	//인자로 받는거 
	EffectInfo mEInfo;	//이펙트 정보.
	BYTE mMoveStyle;		//움직이는 스타일이다.
	DWORD mUniqueID;	//요거 다룰거 유닉크 아이디다.
	DWORD mAttachPosID;		//어느 더미에 붙이는가.
	float mAttachTime;			//	박혀있는 시간.0이면 박힐게 없다.
	Vector3f mStartPos;		//시작 좌표다.
	Vector3f mCtrl0;		//커브 시작 좌표다.
	Vector3f mCtrl1;		//커브 2번째 좌표다.
	Vector3f mCtrl2;		//커브 3번째 좌표다.
	Vector3f mPos;			//현재 좌표다.
	Vector3f mTarget;		//타겟 좌표다.
	BOOL mbArrowType;		//화살은 맞을 당시 각도를 탄다.

	float mSrcFirstYangle;	//발생자 최초의 y좌표다 커브를 위한...
/*
	DWORD mSrcID;		//캐릭터나 몬스터의 경우 어떤 건지 아이디...
	DWORD mTarID;
*/
	DWORD mSrcMat;		//캐릭터나 몬스터의 경우 머터리얼 종류...
	DWORD mTarMat;

	//내부에서 쓰는거.
	WORD	mFlag;		//플래그
//	DWORD	mTargetID[2];		// 이펙트의 타겟이 되는 캐릭터 ID
	Vector3f mRandPos;	//해당 좌표의 렌덤으로 뿌려진 좌표다.
	Vector3f mOldPos;	//이전 좌표다.
	Vector4f mNormal;	//가고 있는 방향의 타겟 노멀이다.
	union{
	float mAlpha;		//가속도를 위한 카운팅.
	float mHideTime;	//플래그가 켜진후 이 숫자가 0이되면 사라짐.
	};
	float mSpeed;		//가속도가 있는 경우는 틀려진다.
	DWORD mBulletIDForDamage;	//총알 데미지.

	BOOL GetMatrixFromTargetID(float get_mat[4][4],BOOL random=0);
	void GetMatrixFromOneMoveDirection(float get_mat[4][4]);
	BOOL AdjustNormalFromTarget();
	BOOL GetMatrixFromSourceID(float get_mat[4][4]);
	BOOL CalcAccelAlpha();	//가속에대한 알파를 구한다. FALSE면 다도착해서 alpha 1.0
	void GetMatrixFromCurve(float get_mat[4][4] );//커브에 관한매트릭스다.
};


static CMoveEffectManager stMoveEM[_MAX_MOVE_EFFECT_MANAGER];
static int stMEMcnt=0;


static BOOL GetNormalMoveEffect(Vector4f normal,Vector3f src,Vector3f tar)
{
	if( src[0] == tar[0] && src[1] == tar[1] && src[2] == tar[2] )
	{
		normal[0]=0;
		normal[1]=0;
		normal[2]=1;
		normal[3]=0;
		return FALSE;
	}
	//해당 방향 노멀구하기 4f인 이유는 타겟점을 지나는 판정을 위해서....
	Vector3fSub(tar,src,normal);
	Normalize(normal);
	normal[3] = normal[0]*tar[0]+normal[1]*tar[1]+normal[2]*tar[2];
	return TRUE;
}

static void DeleteMoveEffectManager(DWORD id)
{
	if( stMEMcnt <= 0 )
		return;
//	SetMagicState(stMoveEM[id].mUniqueID,_EF_STATE_START_DISAPEAR);
	SetDisapearTimeMagicEffect(stMoveEM[id].mUniqueID,0);
	memcpy(&stMoveEM[id],&stMoveEM[id+1]
		,sizeof(CMoveEffectManager)*(stMEMcnt-id-1));
	stMEMcnt--;
}

void StartMoveEffectManager(_MOVE_EF_FUNC *move_ef_func)
//EffectInfo *e_info,DWORD unique_id,Vector3f pos,Vector3f tar,BYTE move_style,float speed)
{
	if( stMEMcnt > _MAX_MOVE_EFFECT_MANAGER-1 )
	{
		stMEMcnt=_MAX_MOVE_EFFECT_MANAGER-1;
		DeleteMoveEffectManager(0);	//오래된거를 없애자...
		stMEMcnt=_MAX_MOVE_EFFECT_MANAGER-1;
	}
	memcpy(&stMoveEM[stMEMcnt].mEInfo,move_ef_func->e_info,sizeof(EffectInfo));	//이펙트 정보는 그대로 카피.
	stMoveEM[stMEMcnt].mUniqueID = move_ef_func->unique_id;
	Vector3fCopy(move_ef_func->pos,stMoveEM[stMEMcnt].mPos);
	Vector3fCopy(move_ef_func->pos,stMoveEM[stMEMcnt].mStartPos);
	stMoveEM[stMEMcnt].mAlpha=0;

	stMoveEM[stMEMcnt].mAttachPosID=move_ef_func->attach_type_id;
	Vector3fCopy(move_ef_func->pos,stMoveEM[stMEMcnt].mOldPos);
//	Vector3fCopy(tar,stMoveEM[stMEMcnt].mTarget);	
	stMoveEM[stMEMcnt].mMoveStyle = move_ef_func->move_style;
	stMoveEM[stMEMcnt].mSpeed = move_ef_func->speed;
	stMoveEM[stMEMcnt].mAttachTime = move_ef_func->attach_time;
	stMoveEM[stMEMcnt].mFlag = move_ef_func->flag;
	CCharacter	*ch_src = _GetCharMgr()->GetCharacter(  stMoveEM[stMEMcnt].mEInfo.dwMyself[0]
		,stMoveEM[stMEMcnt].mEInfo.dwMyself[1]);
	if( ch_src )
		stMoveEM[stMEMcnt].mSrcFirstYangle = ch_src->GetRotY();
	else
		stMoveEM[stMEMcnt].mSrcFirstYangle = 0;

	if(move_ef_func->move_style == _MEM_MS_CURVE )
	{
		_MAGIC_TRACK *mtrack = GetMagicTrack(move_ef_func->unique_id);

		if( mtrack == NULL )
			return;
		DWORD id = _MAX_MAGIC_TRACK-3;
		Vector3fCopy(mtrack[id].Pos,stMoveEM[stMEMcnt].mCtrl0);
		id = _MAX_MAGIC_TRACK-2;
		Vector3fCopy(mtrack[id].Pos,stMoveEM[stMEMcnt].mCtrl1);
		id = _MAX_MAGIC_TRACK-1;
		Vector3fCopy(mtrack[id].Pos,stMoveEM[stMEMcnt].mCtrl2);
	}

/*
	stMoveEM[stMEMcnt].mSrcID = move_ef_func->src_ID;		//캐릭터나 몬스터의 경우 어떤 건지 아이디...
	stMoveEM[stMEMcnt].mTarID = move_ef_func->tar_ID;
*/
	stMoveEM[stMEMcnt].mSrcMat = move_ef_func->src_mat;		//캐릭터나 몬스터의 경우 머터리얼 종류...
	stMoveEM[stMEMcnt].mTarMat = move_ef_func->tar_mat;

	stMoveEM[stMEMcnt].mBulletIDForDamage = move_ef_func->bullet_ID_for_damage;

	//해당 방향 노멀구하기 4f인 이유는 타겟점을 지나는 판정을 위해서....
	GetNormalMoveEffect(stMoveEM[stMEMcnt].mNormal,move_ef_func->pos,move_ef_func->tar);
	//좀 앞으로 땡긴다.
	Vector3fMul(stMoveEM[stMEMcnt].mNormal,_ARROW_PRE_DIST,stMoveEM[stMEMcnt].mTarget);
	Vector3fAdd(move_ef_func->tar,stMoveEM[stMEMcnt].mTarget,stMoveEM[stMEMcnt].mTarget);

	stMEMcnt++;
}


BOOL CMoveEffectManager::CalcAccelAlpha()	//가속에대한 알파를 구한다. FALSE면 다도착해서 alpha 1.0
{
	mAlpha += (R3GetLoopTime()*mSpeed);
	mSpeed += R3GetLoopTime()*0.6f;
	if( mAlpha > 1 )	//벌써 도착했다.
		return FALSE;
	return TRUE;
}
void CMoveEffectManager::GetMatrixFromCurve(float get_mat[4][4] )//커브에 관한매트릭스다.
{
	Vector3f ctrl_pos[4];
	Vector3f ctrl_temp[4];
	float mat[4][4];

	Vector3fCopy( mCtrl0,ctrl_temp[0] );
	Vector3fCopy( mCtrl1,ctrl_temp[1] );
	Vector3fCopy( mCtrl2,ctrl_temp[2] );
//	GetMagicCTRLPoint012(mUniqueID,ctrl_temp);	//0,1,2ctrl포인트를 얻어온다.

	//최초 발사시 캐릭터의 회전값에 맞춰야한다.
	MatrixRotate(mat,0,mSrcFirstYangle,0);
	Vector3fTransform(ctrl_pos[0],ctrl_temp[0],mat);
	Vector3fTransform(ctrl_pos[1],ctrl_temp[1],mat);
	Vector3fTransform(ctrl_pos[2],ctrl_temp[2],mat);


	Vector3fAdd(mStartPos,ctrl_pos[0],ctrl_pos[0]);	//상대좌표므로 절대좌표로 변환.
	Vector3fAdd(mStartPos,ctrl_pos[1],ctrl_pos[1]);
	Vector3fAdd(mStartPos,ctrl_pos[2],ctrl_pos[2]);
	//마지막 타겟좌표를 얻어온다.
	CCharacter	*ch_tar = _GetCharMgr()->GetCharacter( mEInfo.dwTarget[0],mEInfo.dwTarget[1]);
	if( ch_tar != NULL )
	{
		EFUtil_GetDummyPos( ctrl_pos[3], ch_tar,_DUMMY_DAMAGE2);
	}
	else
	{
		Vector3fCopy(mTarget,ctrl_pos[3]);
//		ch_tar->GetPosition(ctrl_pos[3]);
	}

	Vector3f temp_pos;
	Vector3f axis={0,0,-1};

	::GetBezierPoint(&mPos,ctrl_pos,4,mAlpha);
	Vector3fSub(mPos,mOldPos,temp_pos);
	Normalize(temp_pos);
	GetMatrixFromVector(get_mat,temp_pos,axis);


/*	float t_mat[4][4];
	MatrixRotate(t_mat,0,180,0);
	MatrixMultiply(get_mat,get_mat,t_mat);*/
	Vector3fCopy(mPos,get_mat[3]);

	Vector3fCopy(mPos,mOldPos);
}

//화살이 박힌경우라든지 원하는 타겟아이디로부터 
BOOL CMoveEffectManager::GetMatrixFromTargetID(float get_mat[4][4],BOOL random)
{
	CCharacter	*ch_tar;

	ch_tar=_GetCharMgr()->GetCharacter( mEInfo.dwTarget[0],mEInfo.dwTarget[1]);
	//그녀석의 공격부분의 위치를 받아서 부모 매트릭스를 얻어낸다.
	if( ch_tar == NULL )
		return FALSE;

	if( random )
	{
	//원래 있던 회전과 위치를 틀어준다.
		float axis[3]={0,0,-1};
		float mat[4][4];

		MatrixRotate(get_mat,0,ch_tar->GetRotY(),0);

		EFUTIL_GetPosType(get_mat[3],ch_tar,mAttachPosID);
		GetMatrixFromVector(mat,mNormal,axis);
		MatrixMultiply(get_mat,get_mat,mat);

		Vector3fAdd(mRandPos,get_mat[3],get_mat[3]);	//렌덤값을위해.
	}
	else
	{
		CCharacter	*ch_src=_GetCharMgr()->GetCharacter( mEInfo.dwMyself[0],mEInfo.dwMyself[1]);
		float yangle = EFUtil_GetAngle( ch_src,ch_tar);
		MatrixRotateY(get_mat,yangle);
		if( EFUTIL_GetPosType(get_mat[3],ch_tar,mAttachPosID) )
		{
			DWORD special_id = GetSpecialIDFromMagic(mUniqueID);
			if( special_id == 1 )	//현재는 1번아이디가..
			{
				SpecialARGV1(get_mat[3]);
				CopySpecialIDFromMagic(mUniqueID);
			}
		}
	}
	return TRUE;
}

BOOL CMoveEffectManager::GetMatrixFromSourceID(float get_mat[4][4])
{
	CCharacter	*ch_src=_GetCharMgr()->GetCharacter( mEInfo.dwMyself[0],mEInfo.dwMyself[1]);
	//그녀석의 공격부분의 위치를 받아서 부모 매트릭스를 얻어낸다.
	if( ch_src == NULL )
		return FALSE;

	CCharacter	*ch_tar=_GetCharMgr()->GetCharacter( mEInfo.dwTarget[0],mEInfo.dwTarget[1]);
	
	float yangle;
	yangle = EFUtil_GetAngle( ch_src,ch_tar)+180;
	MatrixRotateY(get_mat,yangle);


	if( EFUTIL_GetPosType(get_mat[3],ch_src,mAttachPosID) )
	{
		Vector3f temp;
//		Vector3fCopy(get_mat[3],temp);
		if( ch_tar )
		{
			ch_tar->GetPosition(temp);
			DWORD special_id = GetSpecialIDFromMagic(mUniqueID);
			if( special_id == 1 )	//현재는 1번아이디가..
			{
				SpecialARGV1(temp);
				CopySpecialIDFromMagic(mUniqueID);
			}
		}
	}

	return TRUE;
}

//한번방향으로 움직인후 매트릭스를 얻어온다.
void CMoveEffectManager::GetMatrixFromOneMoveDirection(float get_mat[4][4])
{
	float f_temp;
	Vector3f v_temp;
	float axis[3]={0,0,-1};

	f_temp=(R3GetLoopTime()*mSpeed);
	Vector3fMul(mNormal,f_temp,v_temp);
	Vector3fCopy(mPos,mOldPos);
	Vector3fAdd(v_temp,mPos,mPos);
//	GetMatrixFromVector(get_mat,mNormal,axis);
//	GetMatrixFromAtoB(get_mat,mOldPos,mPos);	//a가 b를 향해 갈때의 매트릭스를 구한다.
	GetMatrixFromAtoB(get_mat,mPos,mOldPos);	//a가 b를 향해 갈때의 매트릭스를 구한다.

	get_mat[3][0] = mPos[0];
	get_mat[3][1] = mPos[1];
	get_mat[3][2] = mPos[2];
}

//다 도착하면 노멀을 좀 바꿔줌으로서 단조로움을 피한다.
BOOL CMoveEffectManager::AdjustNormalFromTarget()
{
	CCharacter	*ch_tar,*ch_src;
	D3DXMATRIX yrot,ytemp;
	D3DXVECTOR3 ppp3;
	D3DXVECTOR4 pOut;
	float axis[3]={0,0,-1};

	ch_tar=_GetCharMgr()->GetCharacter( mEInfo.dwTarget[0],mEInfo.dwTarget[1]);
	if( ch_tar == NULL )
		return FALSE;


	float get_mat[4][4];
	Vector3f temp;
	temp[0] = -mNormal[0];
	temp[1] = -mNormal[1];
	temp[2] = -mNormal[2];
	GetMatrixFromVector(get_mat,temp,axis);

	get_mat[3][0] = ch_tar->GetPosX();
	get_mat[3][1] = mTarget[1];
	get_mat[3][2] = ch_tar->GetPosZ();

	BBOX tar_bbox;
	ch_tar->GetBoundBox(tar_bbox);

	float ratio = (tar_bbox.pMax[1]-tar_bbox.pMin[1]);
//	GetDistFromScreen(get_mat[3],get_mat,ratio);
	//타겟포인터의 렌덤 좌표다.
	ratio/=5.0f;
	int i_ratio = (int)(32768/(ratio));
	mRandPos[0] = (rand()%32768-16384)/(float)i_ratio;
	mRandPos[1] = (rand()%32768-16384)/(float)i_ratio;
	mRandPos[2] = (rand()%32768-16384)/(float)i_ratio;

//----------------------------원거리공격과 타격.
//	DWORD m_id;
//	DWORD m_mat;
	DWORD ef_id;
	Vector3f temp_pos,a_pos,b_pos;

	ch_src=_GetCharMgr()->GetCharacter( mEInfo.dwMyself[0],mEInfo.dwMyself[1]);
	if( ch_src == NULL )
		return FALSE;
	DWORD effect_id = (mEInfo.dwID&0xff00ffff)|0x4F200000;
	EFUtil_GetDummyPos( temp_pos, ch_tar,_DUMMY_DAMAGE2);
	ch_src->GetPosition(a_pos);
	ch_tar->GetPosition(b_pos);
	float goc = R3API::GetYAngle( a_pos, b_pos );

	if( mBulletIDForDamage )	//총알 종류에따라 데미지 처리하는것이 우선이다..
	{
		ef_id = 0xEFF20000|mBulletIDForDamage; //바닥 데미지.
		if( IsExistFromMagicSpt(ef_id) )	//바닥데미지 존재하면.
		{
			StartMagicEffect(b_pos,goc,ef_id);
			Play3DSound(ef_id,b_pos);		
		}
		ef_id = 0xEFF10000|mBulletIDForDamage; //탄환 데미지.
		StartMagicEffect(temp_pos,goc,ef_id);
	}
	/*
	else
	if( mEInfo.dwTarget[0] == CTI_MONSTER )
	{
		m_id= ((CMonster*)ch_tar)->GetMeshID();
		m_mat  = GetMonsterMaterial(m_id);
		// 일반 데미지 효과.
		ef_id = ( m_mat|0X20000000);

		DWORD d_index=(mEInfo.dwID & 0x0000ff00)>>8;
		Play3DSound(m_mat*72+stDamageIndexTable[d_index]*4+_ROOT_DAMAGE_SOUND,mPos);
		StartMagicEffect(temp_pos,goc,ef_id);
	}
	else
	if( mEInfo.dwTarget[0] == CTI_AVATAR ||  mEInfo.dwTarget[0] == CTI_PLAYER )
	{
		m_mat  = GetCharMaterial(ch_tar);
		ef_id = ( m_mat|0X20000000);
		DWORD d_index=(mEInfo.dwID & 0x0000ff00)>>8;
		Play3DSound(m_mat*72+stDamageIndexTable[d_index]*4+_ROOT_DAMAGE_SOUND,mPos);
		StartMagicEffect(temp_pos,goc,ef_id);
	}*/
//-------------------------------------------------


	// 타격 효과.
//	ef_id = ( mEInfo.dwID|0x50F00000) & 0xfffffff0 ;
//	StartMagicEffect(temp_pos,goc,ef_id);


	MatrixRotate(yrot.m,rand()%20-10,360-ch_tar->GetRotY()+rand()%20-10,rand()%20-10);
	D3DXVec3Transform(&pOut,(D3DXVECTOR3 *)mNormal,&yrot);
	mNormal[0]=pOut.x;
	mNormal[1]=pOut.y;
	mNormal[2]=pOut.z;

	return TRUE;
}

BOOL CollisionForceProcess(EffectInfo *e_info,Vector3f pos)
{
	CCharacter	*ch_src;
	CCharacter	*ch_tar;
	if( e_info->dwType == ETI_FORCE )	//포스 들러 데미지처리..
	{
		
		DWORD code1 = ( e_info->dwID & 0x000000ff ) >> 0;
		DWORD effect_id = ((e_info->dwID&0xff0fffff)|0x4F300000)+7;	//포스 데미지 땅 위치.
		
		//포스 바닥 데미지 처리..
		if( !IsExistFromMagicSpt(effect_id) )
			return TRUE;
		if( code1 >= 7 )
		{
			//DWORD what_a = (e_info->dwID&0x000f0000)>>16;	//어떤 포스 대분류인지....
			//DWORD what_b = (e_info->dwID&0x0000ff00)>>8;	//어떤 포스인지....
			//::Play3DSound(_ROOT_FORCE_SOUND + what_a*400 + what_b*18 + 2,pos);

			ch_src=_GetCharMgr()->GetCharacter( e_info->dwMyself[0],e_info->dwMyself[1]);
			ch_tar=_GetCharMgr()->GetCharacter( e_info->dwTarget[0],e_info->dwTarget[1]);
			float yangle = EFUtil_GetAngle(ch_src,ch_tar);
			StartMagicEffect(pos,yangle, effect_id);//포스 데미지 땅 위치.
		}
		return TRUE;
	}
	return FALSE;
}

void FrameMoveEffectManager()
{
	int i;
	float mat[4][4];
	CCharacter	*ch_tar;
	Vector3f temp_pos;
	float dist;
	EffectInfo *e_info;

	//-------이펙트 상태 머신....
	for(i=0; i<stMEMcnt; i++)
	{
		e_info = &stMoveEM[i].mEInfo;
		switch(stMoveEM[i].mMoveStyle)	
		{
		case _MEM_MS_STOP://정지된상태에서 사라지는 경우.
			stMoveEM[i].mHideTime-=R3GetLoopTime();
			if(stMoveEM[i].mHideTime < 0)	//자 이제 완전히 없어지자.
			{
				SetDisapearTimeMagicEffect(stMoveEM[i].mUniqueID,3); //5초동안 버텨본다.
				DeleteMoveEffectManager(i);
				i--;	//한칸씩 밀리므로...
				continue;
			}
			break;
		case _MEM_MS_TARGET_STOP_HIDE://타겟에서 붙은 상태에서 사라지는 경우.
		case _MEM_MS_TARGET_STOP:// 타겟에 박혀 있는 상태에서 멈춤.(캐릭터에 화살을 박는다든지..)
			if( !stMoveEM[i].GetMatrixFromTargetID(mat,stMoveEM[i].mbArrowType) ) //못 건져올경우.
				goto NEXT_CONTROL;
			if(!MoveMatrixMagicEffect(stMoveEM[i].mUniqueID,mat) )	//지정된 유닉아이디의 이펙트를 수정한다.
			{	//동시 마법 이펙트가 넘쳤을 경우 마법이 사라졌기때문에 없앤다.
				DeleteMoveEffectManager(i);
				i--;	//한칸씩 밀리므로...
				continue;
			}
			stMoveEM[i].mHideTime-=R3GetLoopTime();
			if( stMoveEM[i].mMoveStyle == _MEM_MS_TARGET_STOP )
			{
				if(stMoveEM[i].mHideTime < 3)	//3초 있다가 사라져 보자..
				{
					stMoveEM[i].mMoveStyle = _MEM_MS_TARGET_STOP_HIDE;
					if( !(stMoveEM[i].mFlag & _MEM_MS_FLAG_AUTO_DISPEAR) )//자동으로사라지는 경우가 아닐때
						SetDisapearTimeMagicEffect(stMoveEM[i].mUniqueID,3); //3초동안 버텨본다.
					continue;
				}
			}
			else
			{
				if(stMoveEM[i].mHideTime < 0)	//자 이제 완전히 없어지자.
				{
					//SetDisapearTimeMagicEffect(stMoveEM[i].mUniqueID,-1);
					DeleteMoveEffectManager(i);
					i--;	//한칸씩 밀리므로...
					continue;
				}
			}

			break;
		case _MEM_MS_SOURCE_STOP_HIDE:
		case _MEM_MS_SOURCE_STOP:
			if( !stMoveEM[i].GetMatrixFromSourceID(mat) ) //못 건져올경우.
				goto NEXT_CONTROL;

			if(!MoveMatrixMagicEffect(stMoveEM[i].mUniqueID,mat) )	//지정된 유닉아이디의 이펙트를 수정한다.
			{	//동시 마법 이펙트가 넘쳤을 경우 마법이 사라졌기때문에 없앤다.
				DeleteMoveEffectManager(i);
				i--;	//한칸씩 밀리므로...
				continue;
			}


			stMoveEM[i].mHideTime-=R3GetLoopTime();
			if( stMoveEM[i].mMoveStyle == _MEM_MS_SOURCE_STOP)
			{
				if(stMoveEM[i].mHideTime < 3)	//3초 있다가 사라져 보자..
				{
					stMoveEM[i].mMoveStyle = _MEM_MS_SOURCE_STOP_HIDE;
					if( !(stMoveEM[i].mFlag & _MEM_MS_FLAG_AUTO_DISPEAR) )//자동으로사라지는 경우가 아닐때
						SetDisapearTimeMagicEffect(stMoveEM[i].mUniqueID,3); //3초동안 버텨본다.
					continue;
				}
			}
			else
			{
				if(stMoveEM[i].mHideTime < 0)	//자 이제 완전히 없어지자.
				{
					
					DeleteMoveEffectManager(i);
					i--;	//한칸씩 밀리므로...
					continue;
				}
			}
			break;
		case _MEM_MS_ATTACH_TARGET:
			stMoveEM[i].mMoveStyle = _MEM_MS_TARGET_STOP;
			stMoveEM[i].mbArrowType = FALSE;
			stMoveEM[i].mHideTime =stMoveEM[i].mAttachTime;	//10초동안 붙어있는다.
//			if( !stMoveEM[i].GetMatrixFromTargetID(mat,0) ) //못 건져올경우.
//				goto NEXT_CONTROL;
			mat[3][0] = stMoveEM[i].mTarget[0];
			mat[3][1] = stMoveEM[i].mTarget[1];
			mat[3][2] = stMoveEM[i].mTarget[2];
			/*
			if(!MoveMatrixMagicEffect(stMoveEM[i].mUniqueID,mat) )	//지정된 유닉아이디의 이펙트를 수정한다.
			{	//동시 마법 이펙트가 넘쳤을 경우 마법이 사라졌기때문에 없앤다.
				DeleteMoveEffectManager(i);
				i--;	//한칸씩 밀리므로...
				continue;
			}*/
			break;
		case _MEM_MS_ATTACH_SOURCE:
			stMoveEM[i].mMoveStyle = _MEM_MS_SOURCE_STOP;
			stMoveEM[i].mHideTime =stMoveEM[i].mAttachTime;	//10초동안 붙어있는다.
			mat[3][0] = stMoveEM[i].mTarget[0];
			mat[3][1] = stMoveEM[i].mTarget[1];
			mat[3][2] = stMoveEM[i].mTarget[2];
			break;
		case _MEM_MS_DIRECTION:	//직선코스로 날아가는 경우.
			//한번의 이동으로 pos를 바꿔주고...
			stMoveEM[i].GetMatrixFromOneMoveDirection(mat);
			if( Vector3fDot(stMoveEM[i].mPos,stMoveEM[i].mNormal)-stMoveEM[i].mNormal[3] > _ARROW_PRE_DIST) //타겟점을 지났다.
			{
				//목적지까지 맞았다. 수환씨한테 돌려주자....

				if( ! (stMoveEM[i].mFlag & _MEM_MS_FLAG_COLLISION_NO_RETURN) )
					EFUtil_PushEffectValue(&stMoveEM[i].mEInfo,TRUE);

				ch_tar=_GetCharMgr()->GetCharacter( e_info->dwTarget[0],e_info->dwTarget[1]);
				if( ch_tar != NULL )
				{
					//---맞은 녀석 허리를 꺽어주자...
					CMonsterSpecialEffectDamageAction((void *)ch_tar);
				}
				if( stMoveEM[i].mAttachTime == 0 )	//들러붙는 스타일이 아니다. 없애자 .
				{
					CollisionForceProcess(e_info,stMoveEM[i].mPos);
					goto NEXT_CONTROL;	// 이펙트 지우자.
				}
				ch_tar=_GetCharMgr()->GetCharacter( e_info->dwTarget[0],e_info->dwTarget[1]);
				if( ch_tar == NULL )
					goto NEXT_CONTROL;

				//새로 효과음 작업해야될것.
				/*
				DWORD m_mat  = EFUtil_GetDefenceMaterial(ch_tar);
				if( e_info->dwTarget[0] == CTI_MONSTER )
				{
					// 일반 데미지 효과.
					DWORD d_index=(e_info->dwID & 0x0000ff00)>>8;
					Play3DSound(m_mat*72+stDamageIndexTable[d_index]*4+_ROOT_DAMAGE_SOUND,mat[3]);
				}
				else
				if( e_info->dwTarget[0] == CTI_AVATAR ||  e_info->dwTarget[0] == CTI_PLAYER )
				{
					DWORD d_index=(e_info->dwID & 0x0000ff00)>>8;
					Play3DSound(m_mat*72+stDamageIndexTable[d_index]*4+_ROOT_DAMAGE_SOUND,mat[3]);
				}*/
				stMoveEM[i].mMoveStyle = _MEM_MS_TARGET_STOP;
				stMoveEM[i].mbArrowType = TRUE;
				stMoveEM[i].mHideTime =stMoveEM[i].mAttachTime;	//7초동안 붙어있는다.
				mat[3][0] = stMoveEM[i].mTarget[0];
				mat[3][1] = stMoveEM[i].mTarget[1];
				mat[3][2] = stMoveEM[i].mTarget[2];
				DeleteParticleFromMagicEffect(stMoveEM[i].mUniqueID);

				if( !stMoveEM[i].AdjustNormalFromTarget() )//다 도착하면 노멀을 좀 바꿔줌으로서 단조로움을 피한다.
					goto NEXT_CONTROL;
				continue;
			}

			Vector3f c_point;	//충돌 지점 포인트
			if( g_pLevel->mBsp->IsCollisionFace(stMoveEM[i].mOldPos,stMoveEM[i].mPos,&c_point,0.0f))//,_ARROW_PRE_DIST) )
			{	//맵에 부딪힐 경우.

				if( ! (stMoveEM[i].mFlag & _MEM_MS_FLAG_COLLISION_NO_RETURN) )
					EFUtil_PushEffectValue(&stMoveEM[i].mEInfo,FALSE);//벽에 맞았따.
				if( CollisionForceProcess(e_info,c_point) )
					goto NEXT_CONTROL;

				stMoveEM[i].mMoveStyle = _MEM_MS_STOP;
				stMoveEM[i].mHideTime = 12;	//14초 동안 박혀있자.
				Vector3fCopy(c_point,stMoveEM[i].mPos);

				EFUtil_GetDistFromSource(mat[3],stMoveEM[i].mOldPos,c_point,10,_ARROW_PRE_DIST);
				DeleteParticleFromMagicEffect(stMoveEM[i].mUniqueID);
				MoveMatrixMagicEffect(stMoveEM[i].mUniqueID,mat);	//지정된 유닉아이디의 이펙트를 수정한다.
				continue;
			}
			if(!MoveMatrixMagicEffect(stMoveEM[i].mUniqueID,mat) )	//지정된 유닉아이디의 이펙트를 수정한다.
			{	//동시 마법 이펙트가 넘쳤을 경우 마법이 사라졌기때문에 없앤다.
				DeleteMoveEffectManager(i);
				i--;	//한칸씩 밀리므로...
				continue;
			}
			break;
		case _MEM_MS_FREE_DIRECTION:	//타겟을 특정한포인트로 날려주는경우닷. 현재는 포스만 있다.
			stMoveEM[i].GetMatrixFromOneMoveDirection(mat);
			if( Vector3fDot(stMoveEM[i].mPos,stMoveEM[i].mNormal)-stMoveEM[i].mNormal[3] > _ARROW_PRE_DIST
				|| g_pLevel->mBsp->IsCollisionFace(stMoveEM[i].mOldPos,stMoveEM[i].mPos,&c_point,0.0f))
			{
				DWORD effect_id = (e_info->dwID&0xff0fffff)|0x4F300000+7;	//포스 데미지 땅 위치.
				//포스 바닥 데미지 처리..
				if( !IsExistFromMagicSpt(effect_id) )
				{
					//DWORD what_a = (e_info->dwID&0x000f0000)>>16;	//어떤 포스 대분류인지....
					//DWORD what_b = (e_info->dwID&0x0000ff00)>>8;	//어떤 포스인지....
					//::Play3DSound(_ROOT_FORCE_SOUND + what_a*400 + what_b*18 + 2,stMoveEM[i].mPos);
					StartMagicEffect(stMoveEM[i].mPos,0, effect_id);//포스 데미지 땅 위치.
				}
				goto NEXT_CONTROL;
			}
			break;
		case _MEM_MS_TRACE_TARGET:	//타겟을 실시간을 바꾸어주는 경우 ....
			ch_tar=_GetCharMgr()->GetCharacter( e_info->dwTarget[0],e_info->dwTarget[1]);
			if( ch_tar == NULL )
				goto NEXT_CONTROL;
			EFUtil_GetDummyPos( temp_pos, ch_tar,_DUMMY_DAMAGE2);
			if( GetNormalMoveEffect(stMoveEM[i].mNormal,stMoveEM[i].mPos,temp_pos)==FALSE )//
			{ //소스 타겟이 같은경우.
				dist=0;
			}
			else
			{
				stMoveEM[i].GetMatrixFromOneMoveDirection(mat);
				dist=GetDist(stMoveEM[i].mPos,temp_pos);
			}
			if( dist <= (R3GetLoopTime()*stMoveEM[i].mSpeed) )//도착했다..
			{
				//목적지까지 맞았다. 수환씨한테 돌려주자....
				if( ! (stMoveEM[i].mFlag & _MEM_MS_FLAG_COLLISION_NO_RETURN) )
					EFUtil_PushEffectValue(&stMoveEM[i].mEInfo,TRUE);

				//---맞은 녀석 허리를 꺽어주자...
				CMonsterSpecialEffectDamageAction((void *)ch_tar);

				if( CollisionForceProcess(e_info,temp_pos) )	//포스라면 충돌처리를.
				{
					if( e_info->byRangeType_For_Effect ==  8 )	// 포스가 뚫고 계속 날아가는 경우 처리다.
					{
						stMoveEM[i].mMoveStyle = _MEM_MS_FREE_DIRECTION;
						Vector3fMul(stMoveEM[i].mNormal,1000,temp_pos);// 100미터 날려보낸다.						
						Vector3fAdd(temp_pos,stMoveEM[i].mPos,temp_pos);
						stMoveEM[i].mNormal[3] = Vector3fDot(stMoveEM[i].mNormal,temp_pos);
					}
					//뚫고 지나가는 속성을 가진 포스라서 한번더 타겟을 만들어주고 날려준다.
					goto NEXT_CONTROL;
				}
				else
				{
					if( !stMoveEM[i].AdjustNormalFromTarget() )//다 도착하면 노멀을 좀 바꿔줌으로서 단조로움을 피한다.
						goto NEXT_CONTROL;
				}
				/*
				if( e_info->dwTarget[0] == CTI_MONSTER )
				{
					DWORD d_index=(e_info->dwID & 0x0000ff00)>>8;
					Play3DSound(stMoveEM[i].mTarMat*72+stDamageIndexTable[d_index]+_ROOT_DAMAGE_SOUND,temp_pos);
				}*/
				goto NEXT_CONTROL;
			}
			if(!MoveMatrixMagicEffect(stMoveEM[i].mUniqueID,mat) )	//지정된 유닉아이디의 이펙트를 수정한다.
			{	//동시 마법 이펙트가 넘쳤을 경우 마법이 사라졌기때문에 없앤다.
				DeleteMoveEffectManager(i);
				i--;	//한칸씩 밀리므로...
				continue;
			}
			break;
		case _MEM_MS_CURVE:			
			//가속에대한 알파를 구한다.
			if( !stMoveEM[i].CalcAccelAlpha() )//도착했다..
			{
				//목적지까지 맞았다. 수환씨한테 돌려주자....
				if( ! (stMoveEM[i].mFlag & _MEM_MS_FLAG_COLLISION_NO_RETURN) )
					EFUtil_PushEffectValue(&stMoveEM[i].mEInfo,TRUE);
				CollisionForceProcess(e_info,stMoveEM[i].mPos);	//포스처리...

				DWORD effect_id = 0x60107;	//미사일 발동 데미지.
				Vector3f pos;
				ch_tar=_GetCharMgr()->GetCharacter( e_info->dwTarget[0],e_info->dwTarget[1]);
				if( ch_tar == NULL )
					goto NEXT_CONTROL;
				EFUtil_GetDummyPos( pos, ch_tar,_DUMMY_DAMAGE2);
				StartMagicEffect(pos,0,effect_id);//
				
				effect_id = 0x6010C;	//미사일 바닥 데미지.
				ch_tar->GetPosition(pos);
				if( IsExistFromMagicSpt(effect_id) )
				{
					StartMagicEffect(pos,0,effect_id);//
				}		

				goto NEXT_CONTROL;	// 이펙트 지우자.
			}
			else
				stMoveEM[i].GetMatrixFromCurve(mat);

			if(!MoveMatrixMagicEffect(stMoveEM[i].mUniqueID,mat) )	//지정된 유닉아이디의 이펙트를 수정한다.
			{	//동시 마법 이펙트가 넘쳤을 경우 마법이 사라졌기때문에 없앤다.
				DeleteMoveEffectManager(i);
				i--;	//한칸씩 밀리므로...
				continue;
			}
			break;

		default:
			break;
		}
		continue;
NEXT_CONTROL:
		SetDisapearTimeMagicEffect(stMoveEM[i].mUniqueID,0); //없앤다.
		DeleteMoveEffectManager(i);
		i--;	//한칸씩 밀리므로...
	}
}


