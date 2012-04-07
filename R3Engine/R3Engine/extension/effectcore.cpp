#include "EffectUtil.h"
#include "EffectCore.h"
#include "CMagicScript.h"
#include "EffectEntityList.h"
#include "r3util.h"
#include "r3math.h"
#include "Commonutil.h"
#include "Core.h"
#include "Cjcache.h"
#include "renderparticle.h"
#include "../global.h"


//#define _NONE_CACHE	//캐쉬를 안쓰고 실시간 로딩할때


//-------이펙트 관리 매니저다.
//#define _MAX_EFFECT_RESOURCE 256 // 이펙트 리소스관리..
#define _MAX_EFFECT_CNT	256	// 동시 발현 이펙트수 
#define _MAX_MAGIC_CACHE_SIZE	256		//최대 매직 캐쉬 사이즈


#define _MAX_SIMULTARIOUS_WEAPON 128	//동시 발현 무기표현.

#define _MAX_WEAPON 128	//최대 무기..

#define _MAX_MAGIC_EFFECT_CNT	256	// 동시 발현 마법 이펙트수 

//BYTE EffectCache[_MAX_EFFECT_RESOURCE];

static CEffectList stEffectList[_MAX_EFFECT_CNT];
static DWORD stELcnt=0;

//실제 매직에 관한거다. 이것을 캐쉬로 돌려서 쓰자...
//static CProcessMagic stProcessMagic[10];	

static CMagicList stMagicList[_MAX_MAGIC_EFFECT_CNT];
static CMagicList *NowMagicListPtr;
static DWORD stMLcnt=0;
static CJcache stMagicCache;	//매직 캐쉬를 위한거...
class CBsp;
static CBsp *stBsp=NULL;


static Vector3f stVectorARGV1,stVectorARGV2;

void SpecialARGV1(Vector3f pos)
{
	Vector3fCopy(pos,stVectorARGV1);
}
void GetSpecialARGV1(Vector3f pos)
{
	Vector3fCopy(stVectorARGV1,pos);
}
void SpecialARGV2(Vector3f pos)
{
	Vector3fCopy(pos,stVectorARGV2);
}
void GetSpecialARGV2(Vector3f pos)
{
	Vector3fCopy(stVectorARGV2,pos);
}

void SetBspPtr(void *bsp)
{
	stBsp=(CBsp *)bsp;
}

void ResetEffectManager()
{
	stELcnt=0;
	stMLcnt=0;
}

CEffectList* GetEffectList()
{
	return stEffectList;
}
DWORD GetEffectListCnt()
{
	return stELcnt;
}
CMagicList* GetMagicList()
{
	return stMagicList;
}
DWORD GetMagicListCnt()
{
	return stMLcnt;
}

DWORD GetEffectCacheSize()	//현재 사용되고잇는 이펙트 캐쉬 사이즈를 알아낸다.
{
	DWORD size = 0;
	size += _MAX_EFFECT_CNT*sizeof(CEffectList);
	size += _MAX_MAGIC_EFFECT_CNT*sizeof(CMagicList);
	size += _MAX_MAGIC_CACHE_SIZE*sizeof(CProcessMagic);

	return size;
}

DWORD GetEffectCacheTotalNum()	//현재 사용되고잇는 이펙트 캐쉬 갯수를 알아낸다.
{
	return _MAX_MAGIC_CACHE_SIZE;
}

DWORD GetEffectCacheNum()	//현재 이펙트 캐쉬 총 갯수를 알아낸다.
{
	return stMagicCache.GetCacheCnt();
}

static void DeleteEffectList(DWORD id)
{
	if( stELcnt <= 0 )
		return;
	if((stELcnt-id-1) > 0)
	{
		memcpy(&stEffectList[id],&stEffectList[id+1]
			,sizeof(CEffectList)*(stELcnt-id-1));
	}
	stELcnt--;
}

static DWORD stUniqueID=1;

DWORD GetNextUniqueID()
{
	if( stUniqueID > 0x00ffffff )	//혹시 넘칠경우를 대비해서..
		stUniqueID = 1;
	return stUniqueID++;
}


void StartEffect(float mat[4][4], DWORD effect_id,DWORD state
	,DWORD color,float scale,DWORD flag,float frame,DWORD unique_id)
{
	if( stELcnt > _MAX_EFFECT_CNT-1 )
	{
	//	stELcnt=_MAX_EFFECT_CNT-1;
		DeleteEffectList(0);	//오래된거를 없애자...
		stELcnt=_MAX_EFFECT_CNT-1;
	}
	stEffectList[stELcnt].mFrame=frame;
	stEffectList[stELcnt].mEffectID=effect_id;
	stEffectList[stELcnt].mSpecialID=0;

	stEffectList[stELcnt].mFlag = flag;
	//파티클과 엔티티를 로딩한다.
	if( flag & _MT_FLAG_PARTICLE )
	{
		stEffectList[stELcnt].mParticle = GetParticleFromEffectEntityList(effect_id,mat);
		if( stEffectList[stELcnt].mParticle == NULL )
			return;
		if( flag & _MT_FLAG_Z_DISABLE )
			stEffectList[stELcnt].mParticle->mFlag |= _PARTICLE_ATTR_ZDISABLE;
	}
	else
	{
		stEffectList[stELcnt].mEntity = GetEntityFromEffectEntityList(effect_id);
		if( stEffectList[stELcnt].mEntity == NULL )
			return;
		if( flag & _MT_FLAG_Z_DISABLE )
			stEffectList[stELcnt].mEntity->mFlag |= _ENTITY_Z_DISABLE;

		if( flag & _MT_FLAG_BILLBOARD )	//빌보드의 경우.
			stEffectList[stELcnt].mEntity->AddFlag(_EF_BILLBOARD);
		stEffectList[stELcnt].mEntity->SetStartTimer();			//텍스쳐 애니를 위한것.
	}

	MatrixCopy(stEffectList[stELcnt].mMat,mat);
//	Vector3fCopy(pos,stEffectList[stELcnt].mPos);
//	stEffectList[stELcnt].mYAngle = y_angle;
	if( !unique_id )	//유닉 아이디를 지정 안해주면 가져온다.
		unique_id=GetNextUniqueID();
	stEffectList[stELcnt].mUniqueID=unique_id;
	stEffectList[stELcnt].mState = state;
	stEffectList[stELcnt].mAlphaDisapear=0;
	stEffectList[stELcnt].mAlphaApear=0;
	stEffectList[stELcnt].mColor = color;
	stEffectList[stELcnt].mScale = scale;

	stELcnt++;
}
void StartEffect(Vector3f pos,float y_angle, DWORD effect_id,DWORD state
	,DWORD color,float scale,DWORD flag,float frame,DWORD unique_id)
{
	float mat[4][4];

	if( _MT_FLAG_Y_RANDOM & flag )
	{
		union{
		WORD yrand[2];
		float goc;
		}_st;
		_st.goc=y_angle;
		y_angle=(WORD)rand()%(_st.yrand[1]-_st.yrand[0])+_st.yrand[0];
	}
	MatrixRotate(mat,0,y_angle,0); //단위는 360도
	mat[3][0] = pos[0];
	mat[3][1] = pos[1];
	mat[3][2] = pos[2];
	StartEffect(mat,effect_id,state,color,scale,flag,frame,unique_id);
}

BOOL SetEffectState(DWORD unique_id,DWORD state)
{
	DWORD i;

	for( i=0; i<stELcnt; i++ )
	{
		if( stEffectList[i].mUniqueID == unique_id )
		{
			stEffectList[i].mState = state;
			return TRUE;
		}
	}
	return FALSE;
}

_MAGIC_TRACK *GetMagicTrack(DWORD unique_id)
{
	DWORD i;

	for( i=0; i<stMLcnt; i++ )
	{
		if( stMagicList[i].mUniqueID == unique_id )
		{
			return ((CProcessMagic *)stMagicList[i].mMagicPtr)->mMagicTrack;
		}
	}
	return NULL;
}

CProcessMagic *GetMagic(DWORD unique_id)
{
	DWORD i;

	for( i=0; i<stMLcnt; i++ )
	{
		if( stMagicList[i].mUniqueID == unique_id )
		{
			return (CProcessMagic *)stMagicList[i].mMagicPtr;
		}
	}
	return NULL;
}
CMagicList* GetMagicListFromID(DWORD unique_id)
{
	DWORD i;

	for( i=0; i<stMLcnt; i++ )
	{
		if( stMagicList[i].mUniqueID == unique_id )
		{
			return &stMagicList[i];
		}
	}
	return NULL;
}

void SetCurveMagicTrackRandom(_MAGIC_TRACK *mtrack)	//현재 매직에서 쓰는 커브를 랜덤으로 사용한다.
{
	DWORD i,id;
	float from,at;

	for(i=0; i<3; i++)
	{
		id = _MAX_MAGIC_TRACK-3;
		from = mtrack[id].Rand1[i];
		at = mtrack[id].Rand2[i];
		mtrack[id].Pos[i] = GetRandNum(from,at);

		id = _MAX_MAGIC_TRACK-2;
		from = mtrack[id].Rand1[i];
		at = mtrack[id].Rand2[i];
		mtrack[id].Pos[i] = GetRandNum(from,at);

		id = _MAX_MAGIC_TRACK-1;
		from = mtrack[id].Rand1[i];
		at = mtrack[id].Rand2[i];
		mtrack[id].Pos[i] = GetRandNum(from,at);
	}
}
/*
//해당 매직에서 사용되는 스페셜 id중 하나를 얻어낸다.
DWORD GetSpecialIDFromMagic(DWORD unique_id)
{
	DWORD i;

	for( i=0; i<stMLcnt; i++ )
	{
		if( stMagicList[i].mUniqueID == unique_id )	//해당되는 엔티티들을 죄다...
		{
			stMagicList[i].mState = state;
			for( int j=0; j<stMagicList[i].mEntityUniqueIDCnt; j++)
				SetEffectState(stMagicList[i].mEntityUniqueID[j],state);

			return TRUE;
		}
	}
	return 0;
}
*/

BOOL CopyEffectARGV(DWORD unique_id)
{
	DWORD i;

	for( i=0; i<stELcnt; i++ )
	{
		if( stEffectList[i].mUniqueID == unique_id )
		{
			Vector3fCopy(stVectorARGV1,stEffectList[i].mVectorArgv1);
			Vector3fCopy(stVectorARGV2,stEffectList[i].mVectorArgv2);
			return TRUE;
		}
	}
	return FALSE;
}

//해당 매직에서 사용되는 스페셜 id중 하나를 얻어낸다.
BOOL SetSpecialIDFromMagic(DWORD unique_id,DWORD special_id)
{
	DWORD i;

	for( i=0; i<stMLcnt; i++ )
	{
		if( stMagicList[i].mUniqueID == unique_id )	//해당되는 엔티티들을 죄다...
		{
			stMagicList[i].mSpecialID = special_id;
			Vector3fCopy(stVectorARGV1,stMagicList[i].mVectorArgv1);
			Vector3fCopy(stVectorARGV2,stMagicList[i].mVectorArgv2);
			return TRUE;
		}
	}
	return FALSE;
}

DWORD GetSpecialIDFromMagic(DWORD unique_id)
{
	DWORD i;

	for( i=0; i<stMLcnt; i++ )
	{
		if( stMagicList[i].mUniqueID == unique_id )	//해당되는 엔티티들을 죄다...
		{
			return stMagicList[i].mSpecialID;
		}
	}
	return 0;
}

BOOL CopySpecialIDFromMagic(DWORD unique_id)	//매직안 이펙트들의 인자를 ..	
{
	DWORD i;

	for( i=0; i<stMLcnt; i++ )
	{
		if( stMagicList[i].mUniqueID == unique_id )	//해당되는 엔티티들을 죄다...
		{
			if( !stMagicList[i].mSpecialID )	//스페셜할게 없다.
				return TRUE;

			for( int j=0; j<stMagicList[i].mEntityUniqueIDCnt; j++)
				CopyEffectARGV(stMagicList[i].mEntityUniqueID[j]);

			return TRUE;
		}
	}
	return FALSE;
}


BOOL SetMagicState(DWORD unique_id,DWORD state)
{
	DWORD i;

	for( i=0; i<stMLcnt; i++ )
	{
		if( stMagicList[i].mUniqueID == unique_id )	//해당되는 엔티티들을 죄다...
		{
			if( state == stMagicList[i].mState )
				return TRUE;

			stMagicList[i].mState = state;
			for( int j=0; j<stMagicList[i].mEntityUniqueIDCnt; j++)
				SetEffectState(stMagicList[i].mEntityUniqueID[j],state);

			return TRUE;
		}
	}
	return FALSE;
}

BOOL SetDisapearTimeEffect(DWORD unique_id,float time)
{
	DWORD i;

	for( i=0; i<stELcnt; i++ )
	{
		if( stEffectList[i].mUniqueID == unique_id )
		{
			stEffectList[i].mState = _EF_STATE_START_DISAPEAR;	//속성 자동 셋팅.
			stEffectList[i].mDisapearTime = time;
			stEffectList[i].mDisapearFrame = stEffectList[i].mFrame;
			if( time==0 )	//즉시 없애야 되는거
			{
				if( stEffectList[i].mFlag & _MT_FLAG_PARTICLE )//파티클인경우.
				{
					CParticle *particle=stEffectList[i].mParticle;
					DWORD flag = particle->GetParticleAttr();	//루프안됨.
					particle->SetParticleAttr(_PARTICLE_ATTR_NO_LOOP|flag);	//루프안됨.
				}
				else
				{
					DeleteEffectList(i);
					i--;	//한칸씩 밀리므로...
				}
			}
			return TRUE;
		}
	}
	return FALSE;
}
//현재 unique_id가 살아있는가?
BOOL IsExistUniqueID(DWORD unique_id)
{
	DWORD i;

	for( i=0; i<stELcnt; i++ )
	{
		if( stEffectList[i].mUniqueID == unique_id )
			return TRUE;
	}
	return FALSE;
}
//특정 이펙트들을 움직일때 사용한다.  화살이나 기타 그런것들...
BOOL MoveMatrixEffect(DWORD unique_id,float mat[4][4])	//지정된 유닉아이디의 이펙트를 수정한다.
{
	DWORD i;

	for( i=0; i<stELcnt; i++ )
	{
		if( stEffectList[i].mUniqueID == unique_id )
		{
			if( mat !=0 )
			{
				if( NowMagicListPtr )
				{
					if( NowMagicListPtr->mSpecialID )
					{
						stEffectList[i].mSpecialID = stMagicList[i].mSpecialID;
						Vector3fCopy(NowMagicListPtr->mVectorArgv1,stEffectList[i].mVectorArgv1);
						Vector3fCopy(NowMagicListPtr->mVectorArgv2,stEffectList[i].mVectorArgv2);
					}
				}
				MatrixCopy(stEffectList[i].mMat,mat);
/*
				if( stEffectList[i].mFlag & _MT_FLAG_PARTICLE && stEffectList[i].mParticle )//파티클인경우.	
				{
					if( stEffectList[i].mParticle->mFlag & _PARTICLE_ATTR_SPECIAL_ID )	//spcial 인자를 넣어준다.
					{
						if( stEffectList[i].mParticle->mSpecialID == 1 )
						{
							Vector3fCopy(stEffectList[i].mVectorArgv1,stEffectList[i].mParticle->mPowerTrack[0][1]);	//타겟좌표 넣어주자.
						}
					}
				}
*/
			}
			return TRUE;
		}
	}
	return FALSE;
}


BOOL MovePosEffect(DWORD unique_id,Vector3f pos,float y_angle)	//지정된 유닉아이디의 이펙트를 수정한다.
{
	float mat[4][4];

	MatrixRotate(mat,0,y_angle,0); //단위는 360도
	mat[3][0] = pos[0];
	mat[3][1] = pos[1];
	mat[3][2] = pos[2];

	return MoveMatrixEffect(unique_id,mat);	//지정된 유닉아이디의 이펙트를 수정한다.
}
/*
static void StartEffectTrack(Vector3f pos,float y_angle,_MAGIC_TRACK *magic_track,DWORD state,float frame=0,DWORD unique_id=0)
{
	StartEffect( pos, y_angle, magic_track->EntityID,state
		,magic_track->Color,magic_track->Scale,magic_track->Flag,frame,unique_id);
	stEffectList[stELcnt-1].mAlphaDisapear=magic_track->AlphaDisapear;
}
*/
static void StartEffectTrack(float mat[4][4],_MAGIC_TRACK *magic_track,DWORD state,float frame=0,DWORD unique_id=0)
{
	StartEffect( mat, magic_track->EntityID,state
		,magic_track->Color,magic_track->Scale,magic_track->Flag,frame,unique_id);
	if( stELcnt )
	{
		stEffectList[stELcnt-1].mAlphaDisapear=magic_track->AlphaDisapear;
		stEffectList[stELcnt-1].mAlphaApear=magic_track->AlphaApear;
	}
//	stEffectList[stELcnt-1].mDynamicLight=magic_track->DynamicLight;
}

static void FrameMoveEffectsList()
{
	DWORD i,j;

	// camera rendering 임시.....
//	GetCamera()->Render();
//	if( g_pCamera->GetPosY() > 35 )
//		int iii=0;
	for( i=0; i<stELcnt; i++ )
	{
/*
		switch( stEffectList[i].mEffectID )
		{
		case _EF_BLOOD01:
			//stEffectList[i].stBlood
			break;
		default:;
		}
*/
		if( stEffectList[i].mState == _EF_STATE_DEATH )//잠시 죽여놓는다.
			continue;
			
		if( stEffectList[i].mFlag & _MT_FLAG_PARTICLE )//파티클인경우.
		{
			CParticle *particle = stEffectList[i].mParticle;
			particle->SetBspPtr(stBsp);
			if( stEffectList[i].mFlag &	_MT_FLAG_ENTITY_FADE )
				particle->SetCreatePos(GetR3State()->mInvMatView.m[3]);	//충돌 할땐 필요하다...
			else
				particle->SetCreatePos(stEffectList[i].mMat[3]);	//충돌 할땐 필요하다...

			stEffectList[i].mFrame += R3GetLoopTime()*30;
			if( stEffectList[i].mState == _EF_STATE_START_DISAPEAR )
			{
				if( stEffectList[i].mFrame-stEffectList[i].mDisapearFrame > stEffectList[i].mDisapearTime*30 ) //사라지는 시간보다 크면...
				{
					DWORD flag = particle->GetParticleAttr();	//루프안됨.
					particle->SetParticleAttr(_PARTICLE_ATTR_NO_LOOP|flag);	//루프안됨.
				}
			}
			if( particle->mSpecialID )	//스페샬일경우 처리.
			{
				if( particle->mSpecialID == 1 )
				{
					Vector3fCopy(stEffectList[i].mVectorArgv1,particle->mPowerTrack[0][1]);	//타겟좌표 넣어주자.
				}
				else
				if( particle->mSpecialID >= 3 )
				{
					Vector3fCopy(stEffectList[i].mVectorArgv1,particle->mSpecialARGV[0]);	//타겟좌표 넣어주자.
					Vector3fCopy(stEffectList[i].mVectorArgv2,particle->mSpecialARGV[1]);	//타겟좌표 넣어주자.
				}
			}
			if( particle->Loop() ==0 )
			{
				delete particle;	//파티클은 매번 생성되므로 지워줘야한다.
				stEffectList[i].mParticle=NULL;
				DeleteEffectList(i);
				i--;	//한칸씩 밀리므로...
			}
			continue;
		}

		if( stEffectList[i].mFlag & _MT_FLAG_DYNAMIC_LIGHT )//다이나믹 라이트 속성인경우.
		{
			if( stEffectList[i].mEntity == NULL )
				continue;

			float f_alpha,temp_f;
			DWORD alpha;
			if( stEffectList[i].mEntity->mObject->frames !=0 )
			{
				f_alpha = 1-stEffectList[i].mFrame/(float)stEffectList[i].mEntity->mObject->frames;
				if( stEffectList[i].mAlphaDisapear != 1 )
					f_alpha=(f_alpha)/(1-stEffectList[i].mAlphaDisapear);
				else
					f_alpha=1;
				f_alpha=min(f_alpha,1);

				if( stEffectList[i].mAlphaApear != 0 )
				{
					temp_f = stEffectList[i].mFrame/(float)stEffectList[i].mEntity->mObject->frames;
					temp_f=(temp_f)/(stEffectList[i].mAlphaApear);
					f_alpha=min(f_alpha,temp_f);
				}
			}
			else
				f_alpha=1;
			if( stEffectList[i].mState == _EF_STATE_ALWAYS_LIVE )
				f_alpha=1;
			else
			if( stEffectList[i].mState == _EF_STATE_START_DISAPEAR ) //사라지기 시작하는 시간.
			{
				f_alpha = 1-(stEffectList[i].mFrame-stEffectList[i].mDisapearFrame)/(stEffectList[i].mDisapearTime*30);
				f_alpha=min(f_alpha,1);
			}
			
			Vector3f pos;
			float scale;

			stEffectList[i].mEntity->PrepareAnimation();
			float w_matrix[4][4];
			for(j=0; j<stEffectList[i].mEntity->mMatGroupNum; j++)
			{
				GetObjectMatrix(w_matrix,stEffectList[i].mEntity->mMatGroup[j].ObjectId,
					stEffectList[i].mEntity->mObject,stEffectList[i].mFrame);	//애니메이션 매트릭스가 있으면 가져온다.
			}

			if( stEffectList[i].mEntity->mObjectNum == 0 )
			{
				Vector3fSub(stEffectList[i].mEntity->mMatGroup[0].BBMax
					,stEffectList[i].mEntity->mMatGroup[0].BBMin,pos);
				scale = pos[0]/2;	//절반..
				pos[0]=stEffectList[i].mMat[3][0];
				pos[1]=stEffectList[i].mMat[3][1];
				pos[2]=stEffectList[i].mMat[3][2];
			}
			else
			{
				Vector3fAdd(stEffectList[i].mMat[3],stEffectList[i].mEntity->mObject->f_matrix[3],pos);
				scale = stEffectList[i].mEntity->mObject->f_matrix[0][0];
			}
			f_alpha = f_alpha*((stEffectList[i].mColor>>24)/255.0f);	//지정된 알파를 사용.
			alpha = ((DWORD)(f_alpha*255))<<24;
			scale *= stEffectList[i].mScale;

			void *tex;
			if( stEffectList[i].mEntity->mMatGroup )
			{
				if( stEffectList[i].mEntity->mMatGroup->MtlId == -1 )
					tex=NULL;
				else
					tex = (LPDIRECT3DTEXTURE8)R3GetSurface(stEffectList[i].mEntity->mMat->m_Layer[0].m_iSurface);
			}
			else
				tex=NULL;

			BOOL magic_light,alpha_type;
			
			if( _MT_FLAG_MAGIC_LIGHT & stEffectList[i].mFlag )
			{
				magic_light = TRUE;
				alpha_type = stEffectList[i].mEntity->mMat->m_Layer[0].m_dwAlphaType;
			}
			else
			{
				magic_light = FALSE;
				alpha_type = _BLEND_BRIGHT;
			}

			//첫번째 오브젝트의 스케일값을 참조한다....
			scale=max(0.1f,scale);	//-를 막자.깨진다.
			InsertDynamicLight(pos,scale
				,((stEffectList[i].mColor&0x00ffffff)|alpha),tex,magic_light,alpha_type);
//			InsertDynamicLight(pos,(stEffectList[i].mDynamicLight>>24)*4
//				,((stEffectList[i].mDynamicLight&0x00ffffff)|alpha));
		}
		
		if( !stEffectList[i].mEntity->IsLoadedEntity() )	//엔티티가 로딩이 안됬따.있을 필요가 없다.
		{
			DeleteEffectList(i);
			i--;	//한칸씩 밀리므로...
			continue;
		}

		stEffectList[i].mFrame += R3GetLoopTime()*30;


		if( stEffectList[i].mState == _EF_STATE_AUTO_DISAPEAR ||stEffectList[i].mState == _EF_STATE_MYSELF_LIVE)
		{
			if( stEffectList[i].mEntity->mObjectNum == 0 )	//애니가 없으면 항상 살아 있게한다.
				continue;
			if( stEffectList[i].mFrame > stEffectList[i].mEntity->mObject->frames )//애니가 끝나면..
			{
				DeleteEffectList(i);
				i--;	//한칸씩 밀리므로...
			}
		}
		else
		if( stEffectList[i].mState == _EF_STATE_START_DISAPEAR )
		{
			if( stEffectList[i].mFrame-stEffectList[i].mDisapearFrame > stEffectList[i].mDisapearTime*30 ) //사라지는 시간보다 크면...
			{
				DeleteEffectList(i);
				i--;	//한칸씩 밀리므로...
			}
		}
	}
}
/*
BOOL SetMagicEffectState(DWORD magic_id,DWORD state)	//마법이펙트 상태 셋팅한다.
{
	CProcessMagic *get_magic;
	if( !stMagicCache.GetCache(magic_id,(void**)&get_magic) )
		return FALSE;
	get_magic->mState = state;
	for(DWORD i=0; i<get_magic->mTrackNum; i++)
	{
		get_magic->mMagicTrack[i].EntityID;
	}
}

DWORD GetMagicEffectState(DWORD magic_id)	//마법이펙트 상태를 얻어온다.
{
	CProcessMagic *get_magic;
	if( !stMagicCache.GetCache(magic_id,(void**)&get_magic) )
		return 0;
	return get_magic->mState;
}
*/
void DeleteMagicList(DWORD id)
{
	if( stMLcnt <= 0 )
		return;
	memcpy(&stMagicList[id],&stMagicList[id+1]
		,sizeof(CMagicList)*(stMLcnt-id-1));
	stMLcnt--;
}

BOOL DelMagicEffect(DWORD magic_id)//캐쉬상에 있는 마법을 지운다.
{
	CProcessMagic *get_magic;
	if( !stMagicCache.GetCache(magic_id,(void**)&get_magic) )
		return FALSE;
	get_magic->DestoryMagicTrack();
	return stMagicCache.DelCache(magic_id);
}


static DWORD stMagicID[_MAX_MAGIC_ID_STACK];

void PushMagicIDList( DWORD magic_id )
{
	memcpy(&stMagicID[0],&stMagicID[1],sizeof(DWORD)*(_MAX_MAGIC_ID_STACK-1));
	stMagicID[_MAX_MAGIC_ID_STACK-1]=magic_id;
}
DWORD *GetMagicIDList()
{
	return stMagicID;
}


//마법에대한 캐쉬를.../
DWORD StartMagicEffect(float mat[4][4], DWORD magic_id,void *v_einfo,DWORD state)
{
	PushMagicIDList(magic_id);
	EffectInfo *e_Info = (EffectInfo *)v_einfo;
	if( stMLcnt > _MAX_MAGIC_EFFECT_CNT-1 )	//동시 마법발현이 넘쳤다. 
	{
		stMLcnt=_MAX_MAGIC_EFFECT_CNT-1;
		DeleteMagicList(0);	//오래된거를 없애자...
		stMLcnt=_MAX_MAGIC_EFFECT_CNT-1;
	}
	DWORD real_magic_id = GetRealIDFromMagicSpt(magic_id);		//실제 인덱스로 바꾸자.
	if( real_magic_id == -1 )
		return 0;		//마법 아이디가 없다....

	stMagicList[stMLcnt].mState = state;
	stMagicList[stMLcnt].mCharTypeID = -1;// 캐릭터 타입
	stMagicList[stMLcnt].mCharIndex = -1;// 서버 테이터 테이블의 레코드 인덱스
	stMagicList[stMLcnt].mFrame=0;
	stMagicList[stMLcnt].mMagicID=magic_id;
	stMagicList[stMLcnt].mSpecialID=0;
	if( e_Info )
		memcpy( &stMagicList[stMLcnt].mEInfo,e_Info,sizeof(EffectInfo));

	MatrixCopy(stMagicList[stMLcnt].mMat,mat);	//한방에.
//	Vector3fCopy(pos,stMagicList[stMLcnt].mPos);
//	stMagicList[stMLcnt].mYAngle = y_angle;

	//캐쉬에서 긁어 온다.없으면 새로만들어서 쓰고...
	CProcessMagic *get_magic;
	LONG result = stMagicCache.HitCache(magic_id,(void**)&get_magic);
#ifdef _NONE_CACHE
	ReleaseEffectEntityList();
	LoadEffectEntityList(".\\Effect\\EffectEntityList.spt");
	result = _HITCACHE_RET_DEL_AND_NEW;	//지웠다 다시쓴다.
#endif


	{
		char path[256];
		char *r_name = GetFilenameFromMagicSpt(magic_id);

		_R3ENGINE_STATE *state = GetR3State();
		strncpy(path,state->EffectPath,250);	//이펙트 패스 얻어오기...
		if( r_name )
			strcat(path,r_name);
		if( !IsExistFile(path))
		{
			Warning(path,"이파일없음.고쳐줘~");
			return 0;
		}
	}

	if( result == _HITCACHE_RET_NEW )
	{
		char *name = GetFilenameFromMagicSpt(magic_id);
		get_magic->CreateMagicTrack(name);
	}
	else
	if( result == _HITCACHE_RET_DEL_AND_NEW )	//지웠다 다시쓴다.
	{
		char *name = GetFilenameFromMagicSpt(magic_id);
		get_magic->DestoryMagicTrack();
		get_magic->CreateMagicTrack(name);
	}
	else// _HITCACHE_RET_OLD 기존에 있으므로 바로 쓴다.
	{
	}
	stMagicList[stMLcnt].mMagicPtr=get_magic;
	//------------------- 매직을 추후에 다루기위해서...위치좌표를 이동한다든지.. 뭐 그런거..
	stMagicList[stMLcnt].mEntityUniqueIDCnt=get_magic->mTrackNum;
	for( DWORD i=0; i<get_magic->mTrackNum; i++)	//유닉크 아이디를 할당 시켜준다.
		stMagicList[stMLcnt].mEntityUniqueID[i] = GetNextUniqueID();

	stMagicList[stMLcnt].mUniqueID = stUniqueID+1;	//마법 유닉아이디 지정.

	stMLcnt++;
	stUniqueID++;
	return stUniqueID;
}

//캐릭터 아이디를 등록한다. 특별한 상황이면 나중에 없애기 위해.
BOOL SetAttachCharMagicEffect(DWORD unique_id,DWORD char_type,DWORD char_index)
{
	DWORD i;

	for( i=0; i<stMLcnt; i++ )
	{
		if( stMagicList[i].mUniqueID == unique_id )
		{
			stMagicList[i].mCharTypeID = char_type;
			stMagicList[i].mCharIndex = char_index;			
			return TRUE;
		}
	}
	return FALSE;
}

//마법에대한 캐쉬를.../
DWORD StartMagicEffect(Vector3f pos,float y_angle, DWORD magic_id,void *v_einfo,DWORD state)
{
	EffectInfo *e_Info = (EffectInfo *)v_einfo;
	float mat[4][4];
	
	MatrixRotate(mat,0,y_angle,0); //단위는 360도
	mat[3][0] = pos[0];
	mat[3][1] = pos[1];
	mat[3][2] = pos[2];
	//--그거때문에 
	stMagicList[stMLcnt].mYAngle = y_angle;
	stMagicList[stMLcnt].mYOldAngle = y_angle;
	stMagicList[stMLcnt].mYAngleCnt = 0;
	return StartMagicEffect(mat,magic_id,e_Info,state);
}

BOOL MovePosMagicEffect(DWORD unique_id,Vector3f pos,float y_angle)	//지정된 유닉아이디의 이펙트를 수정한다.
{
	for(int i=0; i<stMLcnt; i++ )
	{
		if( stMagicList[i].mUniqueID == unique_id )	//해당되는 엔티티들을 죄다 움직인다.
		{
			DWORD ret=FALSE;
			stMagicList[i].mYOldAngle = stMagicList[i].mYAngle;
			stMagicList[i].mYAngle = y_angle;
			for( int j=0; j<stMagicList[i].mEntityUniqueIDCnt; j++)
			{
				ret|=MovePosEffect(stMagicList[i].mEntityUniqueID[j],pos,y_angle);
			}
			return TRUE;
			//return ret;	//엔티티가 전부사라진 경우는 리턴값이 FALSE다.
		}
	}
	return FALSE;
}

BOOL MoveMatrixMagicEffect(DWORD unique_id,float matrix[4][4])	//지정된 유닉아이디의 이펙트를 수정한다.
{
	Vector3f temp;

	for(int i=0; i<stMLcnt; i++ )
	{
		if( stMagicList[i].mUniqueID == unique_id )	//해당되는 엔티티들을 죄다 움직인다.
		{
			DWORD ret=FALSE;
			for( int j=0; j<stMagicList[i].mEntityUniqueIDCnt; j++)
			{
				_MAGIC_TRACK *magic_track = &((CProcessMagic *)stMagicList[i].mMagicPtr)->mMagicTrack[j];
				
				temp[0] = matrix[0][0]*magic_track->Pos[0] + matrix[1][0]*magic_track->Pos[1] + matrix[2][0]*magic_track->Pos[2];
				temp[1] = matrix[0][1]*magic_track->Pos[0] + matrix[1][1]*magic_track->Pos[1] + matrix[2][1]*magic_track->Pos[2];
				temp[2] = matrix[0][2]*magic_track->Pos[0] + matrix[1][2]*magic_track->Pos[1] + matrix[2][2]*magic_track->Pos[2];
				
				NowMagicListPtr = &stMagicList[i];
				Vector3fAdd(matrix[3],temp,matrix[3]);
				ret|=MoveMatrixEffect(stMagicList[i].mEntityUniqueID[j],matrix);
				Vector3fSub(matrix[3],temp,matrix[3]);
				NowMagicListPtr=NULL;
/*

				Vector3fAdd(matrix[3],magic_track->Pos,matrix[3]);
				ret|=MoveMatrixEffect(stMagicList[i].mEntityUniqueID[j],matrix);
				Vector3fSub(matrix[3],magic_track->Pos,matrix[3]);
*/
				ret=1;
			}
			return ret;	//엔티티가 전부사라진 경우는 리턴값이 FALSE다.
		}
	}
	return FALSE;
}

BOOL GetMagicCTRLPoint012(DWORD unique_id,Vector3f *get_pos)	//베이어 커브 4점중 마지막(타겟) 점을 제외하고 넣어준다. 
{
	DWORD i;

	for( i=0; i<stMLcnt; i++ )
	{
		if( stMagicList[i].mUniqueID == unique_id )	//해당되는 엔티티들을 죄다 움직인다.
		{
			CProcessMagic *magic = (CProcessMagic*)stMagicList[i].mMagicPtr;
			Vector3fCopy(magic->mMagicTrack[_MAX_MAGIC_TRACK-3].Pos,get_pos[0]);	//시작점.
			Vector3fCopy(magic->mMagicTrack[_MAX_MAGIC_TRACK-2].Pos,get_pos[1]);	//ctrl 1.
			Vector3fCopy(magic->mMagicTrack[_MAX_MAGIC_TRACK-1].Pos,get_pos[2]);	//ctrl 2.
			return TRUE;
		}
	}
	return FALSE;
}


//
BOOL SetDisapearTimeMagicEffect(DWORD unique_id,float time)
{
	for(int i=0; i<stMLcnt; i++ )
	{
		if( stMagicList[i].mUniqueID == unique_id )	//해당되는 엔티티들을 죄다...
		{
			stMagicList[i].mState = _EF_STATE_START_DISAPEAR;
			for( int j=0; j<stMagicList[i].mEntityUniqueIDCnt; j++)
			{
//				SetEffectState(stMagicList[i].mEntityUniqueID[j],_EF_STATE_START_DISAPEAR);
				SetDisapearTimeEffect(stMagicList[i].mEntityUniqueID[j],time);
			}
			return TRUE;
		}
	}
	return FALSE;
}


//강제적으로 다루는 이펙트들은 단체로 사라진다.
void CreateEffectsList(char *filename)		//이펙트들 초기화.
{
	stMagicCache.CreateJcache(_MAX_MAGIC_CACHE_SIZE,sizeof(CProcessMagic));
	LoadEffectEntityList(filename);	//스크립트에서 엔티티리스트를 읽는다.
}

void DestroyEffectsList()		//이펙트들 릴리즈
{
	stMagicCache.DestroyJcache();

	//파티클은 릴리즈해야 한다.... 각각의 객체가 있으므로...
	for( int i=0; i<stELcnt; i++ )
	{
		if( stEffectList[i].mFlag & _MT_FLAG_PARTICLE )//파티클인경우.
		{
			CParticle *particle = stEffectList[i].mParticle;
			delete particle;	//파티클은 매번 생성되므로 지워줘야한다.
			stEffectList[i].mParticle=NULL;
			DeleteEffectList(i);
			i--;	//한칸씩 밀리므로...
		}
	}
	stELcnt=0;	//기냥 지우자...다른 엔티티는 괜찮다.

	ReleaseEffectEntityList();
}
BOOL IsAllDisapear(CMagicList *magic_list)	//전부다 사라졌으면 TRUE
{
	for( int j=0; j<magic_list->mEntityUniqueIDCnt; j++)
	{
		DWORD id = magic_list->mEntityUniqueID[j];
		if( IsExistUniqueID(id) )
			return FALSE;
	}
	return TRUE;
}

static void FrameMoveMagicEffects()
{
	DWORD i;
	LONG ret,j,start_j;
	float start_frame;
	_MAGIC_TRACK *magic_track=NULL;

	for( i=0; i<stMLcnt; i++ )
	{
		stMagicList[i].mOldFrame = stMagicList[i].mFrame;
		stMagicList[i].mFrame += R3GetLoopTime()*30;
		//마법 트랙을 읽어서 타이밍에 맞는 엔티티를 뿌린다.
//		ret=((CProcessMagic *)stMagicList[i].mMagicPtr)->GetEntityIDs(stMagicList[i].mOldFrame,stMagicList[i].mFrame,start_frame,&magic_track);
		ret=((CProcessMagic *)stMagicList[i].mMagicPtr)->GetEntityIDs(stMagicList[i].mOldFrame,stMagicList[i].mFrame,&start_j);
		if( ret >= 0 )	//엔티티가 있다. 뿌리자.
		{
			magic_track = ((CProcessMagic *)stMagicList[i].mMagicPtr)->mMagicTrack;

			for(j=start_j; j<ret+start_j; j++)
			{
				if( magic_track[j].Flag & _MT_FLAG_QUAKE )	//음 지진이다.
				{
					float density = GetDensityFromPos(stMagicList[i].mMat[3]);
					if( density > 0 )
						R3EnvironmentQuake(magic_track[j].QuakeTime,magic_track[j].QuakeDensity*density);
					continue;
				}
				if( magic_track[j].Flag & _MT_FLAG_SKY_FADE )	//음 하늘 페이드인이다.
				{
					SetFadeSky(magic_track[j].Color,magic_track[j].SkyFadeTime);
					continue;
				}
				if( magic_track[j].Flag & _MT_FLAG_TEX_FADE )	//음 텍스쳐 페이드인이다.
				{
					SetFadeTex(magic_track[j].Color,magic_track[j].TexFadeTime,GetFirstTexureFromEffectEntityList(magic_track[j].EntityID));
					continue;
				}
				if( magic_track[j].Flag & _MT_FLAG_CHECK_DAMAGE )	//음 데미지 타이밍이다.
				{
					PushEffectQueueDamage(&stMagicList[i].mEInfo);	//수환씨 이펙트 큐에 넣는다.
				}

				start_frame = stMagicList[i].mFrame-magic_track[j].Time*30.0f;
				float mat[4][4];
				MatrixRotate(mat,0,magic_track[j].Yangle,0); //단위는 360도
				mat[3][0] = magic_track[j].Pos[0];
				mat[3][1] = magic_track[j].Pos[1];
				mat[3][2] = magic_track[j].Pos[2];
				MatrixMultiply(mat,stMagicList[i].mMat,mat);

				if( magic_track[j].Flag & _MT_FLAG_ENTITY_FADE )	//음 엔티티 페이드인은 항상 자기화면에서 쳐다본다.
				{
					//SetFadeTex(magic_track[j].Color,magic_track[j].TexFadeTime,EFUtil_GetFirstTexure(magic_track[j].EntityID));
					MatrixIdentity(mat);
					_R3ENGINE_STATE *state= GetR3State();

					mat[3][0] = state->mMatView.m[3][0];
					mat[3][1] = state->mMatView.m[3][1];
					mat[3][1] = state->mMatView.m[3][2];
					MatrixCopy(mat,state->mInvMatView.m);
				}

				StartEffectTrack(mat,&magic_track[j],stMagicList[i].mState,start_frame
					,stMagicList[i].mEntityUniqueID[j]);
				if( stMagicList[i].mSpecialID )
				{
					stEffectList[stELcnt-1].mSpecialID = stMagicList[i].mSpecialID;
					Vector3fCopy(stMagicList[i].mVectorArgv1,stEffectList[stELcnt-1].mVectorArgv1);
					Vector3fCopy(stMagicList[i].mVectorArgv2,stEffectList[stELcnt-1].mVectorArgv2);
				}
			}
		}
		else
		if( ret == -2)
		{
			if( stMagicList[i].mState == _EF_STATE_AUTO_DISAPEAR) //자동으로 사라지는 경우만.
			{
				if( IsAllDisapear(&stMagicList[i]) )	//전부다 사라졌으면 
				{
					//마법트랙이 할일이 끝났다.
					//이경우는 마법이 끝나서 지워도 진행되는 이펙트는 자기자신이 없어져야 끝나므로 상관없다.
					DeleteMagicList(i);
					i--;	//한칸씩 밀리므로...
				}
			}
			else
			if( stMagicList[i].mState == _EF_STATE_START_DISAPEAR) //수동으로 사라지고잇는거다.
			{
				if( IsAllDisapear(&stMagicList[i]) )	//전부다 사라졌으면 
				{
					//수동 마법트랙이 할일이 끝났다.
					DeleteMagicList(i);
					i--;	//한칸씩 밀리므로...
				}
			}
		}
	}
}

void DestroyEffectsCache()		//캐쉬상의 모든 이펙트들을 릴리즈 시킨다. 새로운 레벨로딩때 필요.
{
	DWORD i;

	for( i=0; i<stELcnt; i++ )
	{
		DeleteEffectList(i);
		i--;	//한칸씩 밀리므로...
	}
	for( i=0; i<stMLcnt; i++ )
	{
		DeleteMagicList(i);
		i--;	//한칸씩 밀리므로...
	}
	stELcnt=0;
	stMLcnt=0;
}

void FrameMoveAllEffectsList()
{
	FrameMoveMagicEffects();
	FrameMoveEffectsList();
}


void RenderCommonEffect(DWORD i)
{
	D3DXMATRIX matWorld,matRot;
	float f_alpha,temp_f;
	DWORD alpha,color;

	D3DXMatrixIdentity( &matWorld );
	matWorld.m[0][0]*=stEffectList[i].mScale;
	matWorld.m[1][1]*=stEffectList[i].mScale;
	matWorld.m[2][2]*=stEffectList[i].mScale;
/*
	MatrixRotate(matRot.m,0,stEffectList[i].mYAngle,0); //단위는 360도
	matWorld=matWorld*matRot;
	matWorld.m[3][0]=stEffectList[i].mPos[0];
	matWorld.m[3][1]=stEffectList[i].mPos[1];
	matWorld.m[3][2]=stEffectList[i].mPos[2];
*/		

	if( stEffectList[i].mFlag &	_MT_FLAG_ENTITY_FADE )
	{
		MatrixCopy(matRot.m,GetR3State()->mInvMatView.m);
		matRot.m[3][0]=0;
		matRot.m[3][1]=0;
		matRot.m[3][2]=0;
		matWorld=matWorld*GetR3State()->mInvMatView;
	}
	else
	{
		MatrixCopy(matRot.m,stEffectList[i].mMat);
		matRot.m[3][0]=0;
		matRot.m[3][1]=0;
		matRot.m[3][2]=0;

		if( !(stEffectList[i].mFlag & _MT_FLAG_BILLBOARD) )	//빌보드의 경우.
			matWorld=matWorld*(*(D3DXMATRIX*)stEffectList[i].mMat);
		else
		{
			matWorld.m[3][0]=stEffectList[i].mMat[3][0];
			matWorld.m[3][1]=stEffectList[i].mMat[3][1];
			matWorld.m[3][2]=stEffectList[i].mMat[3][2];
		}
	}
/*
	//-----------디버그 코드 
	if( stEffectList[i].mFlag == 0 && stEffectList[i].mEntity->mObjectNum > 256)
	{			
		stELcnt--;
		continue;
	}
*/
	//---------------
	if( stEffectList[i].mFlag & _MT_FLAG_PARTICLE )//파티클인경우.
	{
 		CParticle *particle = stEffectList[i].mParticle;
		color = 0xff000000|(stEffectList[i].mColor&0x00ffffff);
		MatrixCopy(particle->mRotMat,matRot.m);
		DrawCParticle(particle,particle->mEntity,matWorld.m,color);	//파티클일경우 파티클을 뿌려주자.			
	}
	else	//엔티티인 경우.
	{
		if( stEffectList[i].mEntity->mObject->frames !=0 )
		{
			f_alpha = 1-stEffectList[i].mFrame/(float)stEffectList[i].mEntity->mObject->frames;
			if( stEffectList[i].mAlphaDisapear != 1 )
				f_alpha=(f_alpha)/(1-stEffectList[i].mAlphaDisapear);
			else
				f_alpha=1;
			f_alpha=min(f_alpha,1);

			if( stEffectList[i].mAlphaApear != 0 )
			{
				temp_f = stEffectList[i].mFrame/(float)stEffectList[i].mEntity->mObject->frames;
				temp_f=(temp_f)/(stEffectList[i].mAlphaApear);
				f_alpha=min(f_alpha,temp_f);
			}
		}
		else
			f_alpha=1;

		if( stEffectList[i].mState == _EF_STATE_ALWAYS_LIVE )
			f_alpha=1;
		else
		if( stEffectList[i].mState == _EF_STATE_START_DISAPEAR ) //사라지기 시작하는 시간.
		{
			f_alpha = 1-(stEffectList[i].mFrame-stEffectList[i].mDisapearFrame)/(stEffectList[i].mDisapearTime*30);
			f_alpha=min(f_alpha,1);
		}
		f_alpha = f_alpha*((stEffectList[i].mColor>>24)/255.0f);	//지정된 알파를 사용.
		alpha = ((DWORD)(f_alpha*255))<<24;
		color = alpha|(stEffectList[i].mColor&0x00ffffff);
		if( stEffectList[i].mEntity->DrawEntity(matWorld.m,color,stEffectList[i].mFrame) == FALSE )
		{	//파티클이 끝나는 경우.
			DeleteEffectList(i);
			i--;	//한칸씩 밀리므로...
		}
	}
}

void RenderAllEffectsList()
{
	DWORD i;

	_R3ENGINE_STATE *r3_state=GetR3State();

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	GetD3dDevice()->SetTransform( D3DTS_WORLD, &matWorld );


	if( r3_state->mIsFog )
		GetD3dDevice()->SetRenderState(D3DRS_FOGENABLE,FALSE);

//---이펙트 리스트.....
	for( i=0; i<stELcnt; i++ )//알파 아닌거부터 뿌린다.
	{
		if( stEffectList[i].mState == _EF_STATE_DEATH )//잠시 죽여놓는다.
			continue;
		if( stEffectList[i].mFlag &  _MT_FLAG_PARTICLE)	//파티클은 나중에
			continue;
		if( stEffectList[i].mEntity->IsFirstAlpha() )	//알파는 나중에
			continue;
		if( stEffectList[i].mFlag &	_MT_FLAG_DYNAMIC_LIGHT )
			continue;
		if( stEffectList[i].mFlag &	_MT_FLAG_ENTITY_FADE )
			continue;
		RenderCommonEffect(i);	//렌더링걸구.
	}

	for( i=0; i<stELcnt; i++ )	//나머지 알파를 뿌리자.
	{
		if( stEffectList[i].mState == _EF_STATE_DEATH )//잠시 죽여놓는다.
			continue;
		if( !(stEffectList[i].mFlag &  _MT_FLAG_PARTICLE) )	//엔티티 인것중.
		{
			if( !stEffectList[i].mEntity->IsFirstAlpha() )	//알파 아닌거는 위에서 햇으니 뿌릴필요가없다.
				continue;
		}
		if( stEffectList[i].mFlag &	_MT_FLAG_DYNAMIC_LIGHT )
			continue;
		if( stEffectList[i].mFlag &	_MT_FLAG_ENTITY_FADE )
			continue;
		RenderCommonEffect(i);	//렌더링걸구.
	}

	//엔티티 페이드인을뿌린다.
	for( i=0; i<stELcnt; i++ )	//나머지 알파를 뿌리자.
	{
		if( stEffectList[i].mFlag &	_MT_FLAG_ENTITY_FADE )
		{
			RenderCommonEffect(i);	//렌더링걸구.
		}
	}

/*
		D3DXMatrixIdentity( &matWorld );
		matWorld.m[0][0]*=stEffectList[i].mScale;
		matWorld.m[1][1]*=stEffectList[i].mScale;
		matWorld.m[2][2]*=stEffectList[i].mScale;

		MatrixCopy(matRot.m,stEffectList[i].mMat);
		matRot.m[3][0]=0;
		matRot.m[3][1]=0;
		matRot.m[3][2]=0;
		matWorld=matWorld*(*(D3DXMATRIX*)stEffectList[i].mMat);

		if( stEffectList[i].mFlag & _MT_FLAG_PARTICLE )//파티클인경우.
		{
 			CParticle *particle = stEffectList[i].mParticle;
			color = 0xff000000|(stEffectList[i].mColor&0x00ffffff);
			MatrixCopy(particle->mRotMat,matRot.m);
			DrawCParticle(particle,particle->mEntity,matWorld.m,color);	//파티클일경우 파티클을 뿌려주자.			
		}
		else	//엔티티인 경우.
		{
			if( stEffectList[i].mEntity->mObject->frames !=0 )
			{
				f_alpha = 1-stEffectList[i].mFrame/(float)stEffectList[i].mEntity->mObject->frames;
				f_alpha=(f_alpha)/(1-stEffectList[i].mAlphaDisapear);
				f_alpha=min(f_alpha,1);
			}
			else
				f_alpha=1;

			if( stEffectList[i].mState == _EF_STATE_ALWAYS_LIVE )
				f_alpha=1;
			else
			if( stEffectList[i].mState == _EF_STATE_START_DISAPEAR ) //사라지기 시작하는 시간.
			{
				f_alpha = 1-(stEffectList[i].mFrame-stEffectList[i].mDisapearFrame)/(stEffectList[i].mDisapearTime*30);
				f_alpha=min(f_alpha,1);
			}
			f_alpha = f_alpha*((stEffectList[i].mColor>>24)/255.0f);	//지정된 알파를 사용.
			alpha = ((DWORD)(f_alpha*255))<<24;
			color = alpha|(stEffectList[i].mColor&0x00ffffff);
			if( stEffectList[i].mEntity->DrawEntity(matWorld.m,color,stEffectList[i].mFrame) == FALSE )
			{	//파티클이 끝나는 경우.
				DeleteEffectList(i);
				i--;	//한칸씩 밀리므로...
			}
		}
*/
	if( r3_state->mIsFog )
		GetD3dDevice()->SetRenderState( D3DRS_FOGENABLE, TRUE );
}

