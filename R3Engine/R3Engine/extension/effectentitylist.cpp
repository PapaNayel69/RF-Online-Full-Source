#include "EffectEntityList.h"
#include "R3math.h"

typedef struct{
	char name[90];
	DWORD id;
	union{
	CEntity *Entity;
	CParticle *Particle;
	};
}_EFFECT_ENTITY_LIST;
static _EFFECT_ENTITY_LIST *stEffectEntityList=NULL;
static DWORD stMaxEntityNum;	//최대 엔티티 갯수.. 동적 로딩을 위한 
static DWORD stEntityNum;	//엔티티 갯수 


DWORD GetAllEffectEntityTextureSize()	//현재 이펙트 엔티티에 사용되는 모든 텍스쳐수.
{
	DWORD i;
	DWORD size=0;

	for(i=0; i<stEntityNum; i++)
	{
		if( stEffectEntityList[i].Entity )	//null이면 로딩.
		{
			if( IsParticle(i) )
			{
				if( stEffectEntityList[i].Particle->mEntity )
					size += stEffectEntityList[i].Particle->mEntity->mTextureSize;
			}
			else
				size += stEffectEntityList[i].Entity->mTextureSize;
		}
	}
	return size;
}

DWORD GetAllMemorySizeEffectEntity() //현재 대략 사용되는 effectentitylist 메모리량	텍스쳐 제외
{
	DWORD size=0;
	size += stMaxEntityNum*sizeof(_EFFECT_ENTITY_LIST);
	DWORD i;

	for(i=0; i<stEntityNum; i++)
	{
		if( stEffectEntityList[i].Entity )
		{
			if( IsParticle(i) )
				size += (sizeof(CParticle)+sizeof(CEntity));
			else
				size += sizeof(CEntity);
		}
	}
	return size;
}

DWORD GetEffectEntityNum()	//이펙트 엔티티 수.
{
	return stEntityNum;
}
DWORD GetLoadedEffectEntityNum()	//로딩된 이펙트 엔티티 수.
{
	DWORD i,cnt=0;

	for(i=0; i<stEntityNum; i++)
	{
		if( stEffectEntityList[i].Entity )
			cnt++;
	}
	return cnt;
}

DWORD GetRealID(DWORD id)	//지정된아이디의 실제 인덱스를 리턴.
{
	for(DWORD i=0; i<stEntityNum; i++ )
	{
		if( stEffectEntityList[i].id == id )
			return i;
	}
	char buf[256];
	sprintf(buf,"%d",id);
	Warning(buf,"<-이 아이디가 EffectEntityList.spt에 없습니다.");
	return 0;
}

BOOL IsParticle(DWORD real_id)
{
	int leng = strlen(stEffectEntityList[real_id].name);
	if( stEffectEntityList[real_id].name[leng-1] == 't'
	&& stEffectEntityList[real_id].name[leng-2] == 'p' 
	&& stEffectEntityList[real_id].name[leng-3] == 's' )
		return TRUE;
	return FALSE;
}

//엔티티리스트에서 엔티티 포인터를 얻어온다.
//인자는 반드시 실제 인덱스를 넣어줘야한다.
//지금은 로딩 안되면.. 엔티티를 읽지만 실제는 캐쉬를 만들어야한다.

CEntity *GetEntityFromEffectEntityList(DWORD real_id)	
{
	if( !stEffectEntityList[real_id].Entity )	//null이면 로딩.
	{
		stEffectEntityList[real_id].Entity = new CEntity;
		memset(stEffectEntityList[real_id].Entity,0,sizeof(CEntity));
		if( stEffectEntityList[real_id].Entity->LoadEntity(stEffectEntityList[real_id].name) )
			stEffectEntityList[real_id].Entity->RestoreTexMem();
		else
		{
			delete stEffectEntityList[real_id].Entity;
			stEffectEntityList[real_id].Entity=NULL;
			return NULL;
		}
	}
	return stEffectEntityList[real_id].Entity;
}

//엔티티 아이디를 인자로 넣으면..첫번째 텍스쳐를 얻어낸다.
void *GetFirstTexureFromEffectEntityList(DWORD entity_id)
{
//	entity_id = GetRealID(entity_id);	//지정된아이디의 실제 인덱스를 리턴.
	if( !entity_id)
		return 0;
	CEntity *entity = GetEntityFromEffectEntityList(entity_id);
	if( !entity)
		return 0;

	return R3GetSurface(entity->mMat->m_Layer[0].m_iSurface);
}

CParticle *GetParticleFromEffectEntityList(DWORD real_id,float mat[4][4])	
{
	CParticle *particle;
	if( !stEffectEntityList[real_id].Particle )	//null이면 로딩.
	{
		particle=new CParticle;
		stEffectEntityList[real_id].Particle = particle;
		if(!particle->LoadParticleSPT(stEffectEntityList[real_id].name) )
		{
			delete particle;
			stEffectEntityList[real_id].Particle=NULL;
			return NULL;
		}
		//파티클은 하나는 로딩해놓고 클론체를 사용해야한다. 
		particle=new CParticle;
		memcpy(particle,stEffectEntityList[real_id].Particle,sizeof(CParticle));
		MatrixCopy(particle->mRotMat,mat);
		particle->mRotMat[3][0]=0;
		particle->mRotMat[3][1]=0;
		particle->mRotMat[3][2]=0;
		particle->InitParticle();
		particle->SetParticleState(_PARTICLE_STATE_START);
//		stEffectEntityList[real_id].Entity->LoadEntity(stEffectEntityList[real_id].name);
//		stEffectEntityList[real_id].Entity->RestoreTexMem();
	}
	else 
	{
		//파티클은 로딩되있는 모체로부터 복사한 클론체를 사용해야한다. 
//		particle=stEffectEntityList[real_id].Particle;
		particle=new CParticle;
		memcpy(particle,stEffectEntityList[real_id].Particle,sizeof(CParticle));
		MatrixCopy(particle->mRotMat,mat);
		particle->mRotMat[3][0]=0;
		particle->mRotMat[3][1]=0;
		particle->mRotMat[3][2]=0;
		particle->InitParticle();
		particle->SetParticleState(_PARTICLE_STATE_START);
	}

	DWORD flag = particle->GetParticleAttr();	//루프안됨.
	flag = (flag&~_PARTICLE_ATTR_END);
	if( !(flag & _PARTICLE_ATTR_EMIT_TIME ) && !(flag & _PARTICLE_ATTR_ALWAYS_LIVE ))
		particle->SetParticleAttr(_PARTICLE_ATTR_NO_LOOP|flag);	//루프안됨.
	return particle;
}
static DWORD ConvertHexa(char *hole)
{
	int i;

	strlwr(hole);
	if( hole[0] != '0' || hole[1] != 'x' )
		return 0;
	DWORD su=0,temp,cnt=0;
	for(i=strlen(hole)-1; i>1; i--)
	{
		if( '0' <= hole[i] && hole[i] <= '9')
		{
			temp = hole[i]-'0';
		}
		else
		if( 'a' <= hole[i] && hole[i] <= 'f')
		{
			temp = hole[i]-'a'+10;
		}
		else
			return 0;
		su += (temp<<(cnt*4));
		cnt++;
	}
	return su;
}

//EffectEntityList.spt
void LoadEffectEntityList(char *name)
{
	FILE *fp;
	if(( fp =fopen(name,"rt"))==NULL) 
	{
		Error(name,"<-이 파일이없습니다.");
		return;
	}

	stMaxEntityNum=768;
	stEffectEntityList=(_EFFECT_ENTITY_LIST * )Dmalloc( sizeof(_EFFECT_ENTITY_LIST)*stMaxEntityNum );
	memset(stEffectEntityList,0,sizeof(_EFFECT_ENTITY_LIST)*stMaxEntityNum );

	char hole[256];

    while(1)	//앞부분 스킵.
    {
		if( fscanf(fp,"%s",hole)==EOF)	
			break;
        if (!strcmp(hole,"[EffectEntityList]"))
		{
			break;
		}
	}  

	stEntityNum=0;
    while(1)
    {
		if( fscanf(fp,"%s",hole)==EOF)	
			break;
		int id=atoi(hole);
		
		if( id == 0)
		{
			id=ConvertHexa(hole);
			if( id == 0)	//에러다.
				continue;
		}
		stEffectEntityList[stEntityNum].id=id;
		fscanf(fp,"%s",hole);
		strlwr(hole);
		if( strlen(hole) > 89  )
		{
			Warning("hole","<-엔티티패스가 너무깁니다.89자이내로 해주세요.");
			continue;
		}
		strncpy(stEffectEntityList[stEntityNum].name,hole,89);
		stEntityNum++;
		if( stEntityNum >= stMaxEntityNum)	//리얼록.
		{
			stEffectEntityList = (_EFFECT_ENTITY_LIST *)ReAlloc(stEffectEntityList
				,sizeof(_EFFECT_ENTITY_LIST)*(stMaxEntityNum)
				,sizeof(_EFFECT_ENTITY_LIST)*(stMaxEntityNum+32));
			memset(stEffectEntityList+stMaxEntityNum,0,sizeof(_EFFECT_ENTITY_LIST)*32 );
			stMaxEntityNum+=32;
		}
	}  
	fclose(fp);
}

void ReleaseEffectEntityList()
{
	DWORD i;
	if( stEntityNum == 0)
		return;
	for(i=0; i<stEntityNum; i++)
	{
		if( stEffectEntityList[i].Entity )	//null이면 로딩.
		{
			if( IsParticle(i) )
			{
				stEffectEntityList[i].Particle->ReleaseParticle();
				if( stEffectEntityList[i].Particle->mEntity )
				{
					stEffectEntityList[i].Particle->mEntity->ReleaseTexMem();
					stEffectEntityList[i].Particle->mEntity->ReleaseEntity();
					delete stEffectEntityList[i].Particle->mEntity;
				}
				delete stEffectEntityList[i].Particle;
			}
			else
			{
				stEffectEntityList[i].Entity->ReleaseTexMem();
				stEffectEntityList[i].Entity->ReleaseEntity();
				delete stEffectEntityList[i].Entity;
			}
		}
	}

	if( stEffectEntityList )
		Dfree(stEffectEntityList);
	stEffectEntityList=NULL;
	stEntityNum=0;
}
