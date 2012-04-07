
#include "MonsterSpecialEffect.h"
#include "foreffect.h"
#include "effectUtil.h"
#include "effectCore.h"
//---------------


//-----------------------------------------------------------------------------------------------------
#define CompareFrame(a,b,c) ( c > a && c<= b)

void Default2Wav(CMonster *mob,DWORD base)	//공격과 죽음에 관한 효과음.
{
	Vector3f pos;

//	DWORD frame = mob->GetBoneFrame();
//	DWORD old_frame = mob->GetOldBoneFrame();
//	if( CompareFrame(old_frame,frame,200) )
	{
		if( mob->GetAction() == CAI_COMBAT_ATTACK )
		{
			mob->GetPosition(pos);	//몬스터 위치.
			::Play3DSound(base+0,pos);
		}
		else
		if( mob->GetAction() == CAI_DEATH_READY )
		{
			mob->GetPosition(pos);	//몬스터 위치.
			::Play3DSound(base+12,pos);
		}
	}
}

static void LarbaSpecialEffect(CMonster *mob)
{
	Vector3f pos;

	if( mob->GetMeshID() == 0x5d )	//라르바 
	{
		if( mob->GetAction() == CAI_MOVE_MOVING )	//이동중이라면..
		{
			mob->GetPosition(pos);	//몬스터 위치.

			float dist = GetDistFromCamera(pos);
			if( dist < _RENDER_DIST )	//렌더링 영역안에 들면.
			{
				DWORD frame = mob->GetBoneFrame();
				DWORD old_frame = mob->GetOldBoneFrame();
				if( CompareFrame(old_frame,frame,160*20) || CompareFrame(old_frame,frame,160*58) )
				{
					float vol = GetVolume(pos);
					::Play3DSound(14212,pos);
					::R3EnvironmentQuake(.4f,1.0f*vol);
				}
			}
		}
		else
		if( mob->GetAction() == CAI_DEATH_READY )	//죽을 때 쿵.
		{
			mob->GetPosition(pos);	//몬스터 위치.

			float dist = GetDistFromCamera(pos);
			if( dist < _RENDER_DIST )	//렌더링 영역안에 들면.
			{
				DWORD frame = mob->GetBoneFrame();
				DWORD old_frame = mob->GetOldBoneFrame();
				if( CompareFrame(old_frame,frame,160*77) )
				{
					float vol = GetVolume(pos);
					::Play3DSound(14212,pos);
					::R3EnvironmentQuake(.4f,1.5f*vol);
				}
			}
		}
		/*
		DWORD frame = mob->GetBoneFrame();
		DWORD old_frame = mob->GetOldBoneFrame();
		if( mob->GetAction() == CAI_COMBAT_ATTACK )	//공격중 이라면..
		{
			if( CompareFrame(old_frame,frame,200) )
			{
				mob->GetPosition(pos);	//몬스터 위치.
				::Play3DSound(14131,pos);
			}
		}
		if( mob->GetAction() == CAI_DEATH_READY )	//죽는다 .
		{
			if( CompareFrame(old_frame,frame,200) )
			{
				mob->GetPosition(pos);	//몬스터 위치.
				vol = GetVolume(pos);
				pan = ::GetPan(pos);
				::StopWave(13003);
				::PlayWave(13003,0,vol,pan);
			}
		}*/
	}
}

void CMonsterSpecialEffectFrameMove(void* monster,DWORD unique_id)	//몬스터에 붙어있는 효과 처리.
{
}

void CMonsterSpecialEffectCreateDummy( void* mon, CHARACTEROBJECT *co )	//몬스터 에 있을 부분을 미리 준비 해줘야된다.
{
	//CMonster *monster = (CMonster *)mon;
	CBaseNpc *monster = (CBaseNpc *)mon;

	DWORD		MaxObj		= co->m_MaxObj;
	ObjectMesh	*pMesh		= co->m_pMesh;

	char buf[256];
	int m;
/*
	if( monster->GetMeshID() == 55 ||  monster->GetMeshID() == 56 )
	{
		int dfsdf=0;
	}
*/
	for ( m=0; m<MAX_EFFECT_DUMMYS; m++)
		monster->SetDummyIndex(m,0xff);

	//더미들을 찾아보자.
	for ( m=0;m<MaxObj;m++)
	{
		_strcpy(buf,pMesh[m].m_Name,20);
		strlwr(buf);
/*
//		if(!strcmp(buf,"damage1"))
//			monster->SetDummyIndex(_DUMMY_DAMAGE1,m);
//		else
		else
		if(!strcmp(buf,"damage3"))
			monster->SetDummyIndex(_DUMMY_DAMAGE3,m);
		else
		if(!strcmp(buf,"damage4"))
			monster->SetDummyIndex(_DUMMY_DAMAGE4,m);
*/
		if(!strcmp(buf,"effect1"))
			monster->SetDummyIndex(_DUMMY_TRACE1,m);
		else
		if(!strcmp(buf,"effect2"))
			monster->SetDummyIndex(_DUMMY_TRACE2,m);

		if(!strcmp(buf,"damage2"))
			monster->SetDummyIndex(_DUMMY_DAMAGE2,m);
		else
		if(!strcmp(buf,"fire1"))
			monster->SetDummyIndex(_DUMMY_FIRE1,m);
	}

	MaxObj=0;

	if( !monster->GetDummyIndex(_DUMMY_TRACE1))
		monster->SetDummyIndex(_DUMMY_TRACE1,MaxObj);
	if( !monster->GetDummyIndex(_DUMMY_TRACE2))
		monster->SetDummyIndex(_DUMMY_TRACE2,MaxObj);

//	if( !monster->GetDummyIndex(_DUMMY_DAMAGE1))
//		monster->SetDummyIndex(_DUMMY_DAMAGE1,MaxObj);
	if( !monster->GetDummyIndex(_DUMMY_DAMAGE2))
		monster->SetDummyIndex(_DUMMY_DAMAGE2,MaxObj);
//	if( !monster->GetDummyIndex(_DUMMY_DAMAGE3))
//		monster->SetDummyIndex(_DUMMY_DAMAGE3,MaxObj);
//	if( !monster->GetDummyIndex(_DUMMY_DAMAGE4))
//		monster->SetDummyIndex(_DUMMY_DAMAGE4,MaxObj);
//	if( !monster->GetDummyIndex(_DUMMY_AXIS))
//		monster->SetDummyIndex(_DUMMY_AXIS,MaxObj);
	if( !monster->GetDummyIndex(_DUMMY_FIRE1))
		monster->SetDummyIndex(_DUMMY_FIRE1,MaxObj);

}

void CMonsterSpecialEffectDestroy(void *monster)	//몬스터 릴리즈시 초기화 실행.
{
}

void CMonsterDamageBackup(void *character)	//몬스터 관련 특별한 효과 이벤트 처리함수 프래임무브.
{
	D3DXMATRIX rot;
	CHARACTEROBJECT *bn;

	CMonster *mob = (CMonster *)character;
	MatrixIdentity(rot.m);
	bn = g_pBoneMgr->GetBone( RLI_MONSTER, mob->GetBoneID() );
	if( bn == NULL )
		return;
	_GetCharIF()->SetEyeVec("Bip01 Spine1",bn,rot);
	CMesh * co = _GetMeshMgr()->GetMesh( RLI_MONSTER, mob->GetMeshID());
	if( co == NULL )
		return;

// 	_GetCharIF()->SetEyeVec("RF SMW 001",bn,rot);
}

//#define _DAMAGE_TIME	0.6f
//#define _DELAY_TIME_1	0.5f
//#define _DELAY_TIME_2	0.40f
//#define _DELAY_TIME_3	0.30f
/*
#define _DAMAGE_TIME	0.51f
#define _DELAY_TIME_1	0.34f
#define _DELAY_TIME_2	0.27f
#define _DELAY_TIME_3	0.20f
*/
//#define _DAMAGE_TIME	0.27f
#define _DAMAGE_TIME	0.27f
#define _DELAY_TIME_1	0.20f

#define _DELAY_TIME	0.1f

void CMonsterSpecialEffectDamageAction(void *monster) //해당몬스터만 꺽어지게 해라...
{
	CCharacter *ch = (CCharacter *)monster;
	ch->SetDamageTimeForShake(_DAMAGE_TIME);	//X초간 허리제끼기...
	

	BBOX bbox;
	Vector3f pos;

	EFUtil_GetDummyPos( pos, ch,_DUMMY_DAMAGE2);
	ch->GetBoundBox(bbox);

	float radious = (bbox.pMax[1]-bbox.pMin[1])*2.0f;

	ch->Set2ndLightRadius(radious);
	ch->Set2ndLightPos(pos);
	ch->Set2ndDensity(1.0f);	//타격시 맞는 놈의 라이팅....
}

void CMonsterSpecialEffectSetAction(void *monster)	//액션이 변할때 효과넣는함수.
{
	CMonster *mob = (CMonster *)monster;
	Vector3f pos;
/*
	if( mob->GetMeshID() == 55 ||  mob->GetMeshID() == 56 )
	{
		Vector3f get,d_pos;
		d_pos[0]=0;			d_pos[1]=0;			d_pos[2]=0;
	}
*/
	Default2Wav(mob,_MOB_BASE_SOUND + (mob->GetEffectValue())*40);	//공격과 죽음에 관한 효과음.
//	CMonsterSpecialEffectCommon(mob);

	if( mob->GetAction() == CAI_DEATH_DIE )	//죽었다 시작이면... 몬스터소멸....
	{
//		DWORD frame = mob->GetBoneFrame();
//		DWORD old_frame = mob->GetOldBoneFrame();

		mob->GetPosition(pos);	//몬스터 위치.
//		if( CompareFrame(old_frame,frame,160) )
		{
			DWORD m_mat  = EFUtil_GetDefenceMaterial(mob);
			
			Vector3f get,d_pos;
			d_pos[0]=0;			d_pos[1]=0;			d_pos[2]=0;
			float mon_scale = mob->GetScale();
			
			if( EFUtil_GetTransformFromCharacter(&get,mob,_DUMMY_DAMAGE2,d_pos))
			{
				pos[0] += (get[0])*mon_scale;
				pos[1] += (get[1])*mon_scale;
				pos[2] += (get[2])*mon_scale;
			}
			m_mat = EFUTIL_GetRandomMat(m_mat);
			StartMagicEffect(pos,0, 0x80000020+m_mat);
			::Play3DSound(276000+m_mat*3,pos);	//죽음소멸..
		}
	}
}

void CMonsterSpecialEffectFrameMove(void *monster)	//몬스터 관련 특별한 효과 이벤트 처리함수 프래임무브.
{
	//CMonster *mob = (CMonster *)monster;
	CBaseNpc *mob = (CBaseNpc *)monster;
	D3DXMATRIX rot;
	CHARACTEROBJECT *bn;
	float damage_time;
	DWORD		MaxObj;
	ObjectMesh	*pMesh;
	CMesh * co = NULL;

 	damage_time=mob->GetDamageTimeForShake();	//맞을때 허리제끼기...
	if( damage_time > 0 )
	{
		if( _DELAY_TIME_1 < damage_time )
		{
			float su=_DAMAGE_TIME-_DELAY_TIME_1;
			float gac = ((damage_time-_DELAY_TIME_1)/su)*50.0f;
			MatrixRotate(rot.m,0,0,-50+gac);
		}/*
		else
		if( _DELAY_TIME_2 < damage_time )
		{
			float su=_DELAY_TIME_1-_DELAY_TIME_2;
			float gac = ((damage_time-_DELAY_TIME_2)/su)*50.0f;
			MatrixRotate(rot.m,0,0,-gac);
		}
		else 
		if( _DELAY_TIME_3 < damage_time )
		{
			float su=_DELAY_TIME_2-_DELAY_TIME_3;
			float gac = ((damage_time-_DELAY_TIME_3)/su)*30.0f;
			MatrixRotate(rot.m,0,0,-30+gac);
		}
		else			
		{
			float gac = (damage_time/_DELAY_TIME_3)*30.0f;
			MatrixRotate(rot.m,0,0,-gac);
		}*/
		else			
		{
			float gac = (damage_time/_DELAY_TIME_1)*30.0f;
			MatrixRotate(rot.m,0,0,-gac);
		}
		//co = _GetMeshMgr()->GetMesh( RLI_MONSTER, mob->GetMeshID());
		co = mob->GetMesh();
       if( co  == NULL )
           return;

	//	CreateWeaponEffect(CHARACTEROBJECT *co);	//무기의경우 궤적이 있기때문에 미리 시작을 해줘야된다.
		MaxObj		= co->m_MaxObj;
		pMesh		= co->m_pMesh;

		//bn = g_pBoneMgr->GetBone( RLI_MONSTER, mob->GetBoneID() );
		bn = mob->GetBone();
//		_GetCharIF()->SetEyeVec("Bip01 Spine",bn,rot);
		//허리 꺽을 녀석을 찾아보자.
	 	//_GetCharIF()->SetEyeVec("RF SMW 001",co,rot);
//	 		_GetCharIF()->SetEyeVec("Bip01 R Calf",bn,rot);
	 	_GetCharIF()->SetEyeVec("Bip01 Spine1",bn,rot);
		damage_time-=R3GetLoopTime();
		mob->SetDamageTimeForShake(damage_time);
	}
	if( mob->IsEqualCharType( CTI_MONSTER ) )
		LarbaSpecialEffect((CMonster *)mob);	//라르바 전용 함수.

//	Default2Wav(mob,13000+mob->GetMeshID()*13);	//공격과 죽음에 관한 효과음.
//	CMonsterSpecialEffectCommon(mob);
}


void CMonsterSpecialEffectRender(void *monster)	//몬스터 관련 특별한 효과 이벤트 처리함수 프래임무브.
{

	CMonster *mob = (CMonster *)monster;
	if( !mob->IsEnable( ROSF_RENDER_BBOX ) )	//바운드 박스 뿌릴때 
		return;

	D3DXMATRIX matWorld;

	g_ifCharIF->UnSetState();
	D3DXMatrixIdentity( &matWorld );
	GetD3dDevice()->SetTransform( D3DTS_WORLD, &matWorld );


//	DrawLineDummy(mob,_DUMMY_DAMAGE1,0xffff0000);
	EFUTIL_DrawLineDummy(mob,_DUMMY_DAMAGE2,0xff00ff00);
//	DrawLineDummy(mob,_DUMMY_DAMAGE3,0xff0000ff);
//	DrawLineDummy(mob,_DUMMY_DAMAGE4,0xffffff00);
//	DrawLineDummy(mob,_DUMMY_AXIS,0xffffffff);
/*
	float box[3]={1.5f,1.5f,1.5f};
//	float box[3]={1,1,1};
	g_pLevel->DrawTestBox(box,a,0xffffffff);		//사람테스트할 박스유틸...
	g_pLevel->DrawTestBox(box,b,0xffffffff);		//사람테스트할 박스유틸...
*/
	g_ifCharIF->SetState();
}


