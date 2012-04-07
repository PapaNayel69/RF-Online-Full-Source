
#include "PlayerSpecialEffect.h"
#include "itemSpecialEffect.h"
#include "foreffect.h"
#include "effectUtil.h"
#include "effectCore.h"
#include "effectEntityList.h"
#include "..\\ccamera.h"
#include "..\\global.h"


//-----------------------------------------------------------------------------------------------------

#define CompareFrame(a,b,c) ( c > a && c<= b)
/*
		DWORD frame = mob->GetBoneFrame();
		DWORD old_frame = mob->GetOldBoneFrame();
		if( mob->GetAction() == CAI_COMBAT_ATTACK )	//공격중 이라면..
		{
			if( CompareFrame(old_frame,frame,200) )
*/

BOOL ProcessSwordTraceFrameMove( CPlayer *pl )
{
	if( !pl->m_cSwordTrace.IsRealTrace() )	//트레이스중이 아니라면..
		return TRUE;	//정상적인..상태이므로

	int m;
	Vector3f trace_pos[2];
	DWORD	weapon_mesh_id = ID_INVALID;

	CHARACTEROBJECT *co;
	DWORD		MaxObj;
	ObjectMesh	*pMesh;
	D3DXMATRIX yrot,ytemp,*m_temp;
	D3DXVECTOR3 ppp3;
	D3DXVECTOR4 pOut[2];

	CEquipItem *item = pl->GetEquipItem( IEPT_WEAPON );
	if( item )
		weapon_mesh_id = item->GetMeshID();
	else
		return FALSE;

	DWORD trace_id = ( weapon_mesh_id & 0x0000ff00 )>>8;
	if( trace_id > 0x0e ||  trace_id == 5 || trace_id == 8 )	//궤적없는 무기는 보낸다.
		return TRUE;

	co = item->GetMesh();
//	co = _GetMeshMgr()->GetMesh( RLI_ITEM, weapon_mesh_id );
	if( co == NULL )
		return FALSE;

	int id0= item->GetDummyIndex(_DUMMY_TRACE1);
	int id1= item->GetDummyIndex(_DUMMY_TRACE2);

	MaxObj		= co->m_MaxObj;
	pMesh		= co->m_pMesh;
	if( id1 == -1)	//일단 하나짜리는 중단...
		return FALSE;
	m=id1;
	MatrixRotate(yrot.m,0,0,360-pl->GetRotY());
	ppp3.x=0;
	ppp3.y=0;
	ppp3.z=0;
	m_temp=pl->GetResultMat(CEPT_WEAPON,m);
	if( m_temp == NULL )
		return FALSE;
	ytemp = *m_temp*yrot;
	D3DXVec3Transform(&pOut[0],&ppp3,&ytemp);

	m=id0;
	m_temp=pl->GetResultMat(CEPT_WEAPON,m);
	if( m_temp == NULL )
		return FALSE;
	ytemp = *m_temp*yrot;
	D3DXVec3Transform(&pOut[1],&ppp3,&ytemp);

	trace_pos[0][0] = pl->GetPosX()-(pOut[0].x);
	trace_pos[0][1] = pl->GetPosY()-(-pOut[0].z);
	trace_pos[0][2] = pl->GetPosZ()-(+pOut[0].y);

	trace_pos[1][0] = pl->GetPosX()-(pOut[1].x);
	trace_pos[1][1] = pl->GetPosY()-(-pOut[1].z);
	trace_pos[1][2] = pl->GetPosZ()-(+pOut[1].y);

//	pl->m_cSwordTrace.mEntity = GetEntityFromEffectEntityList( GetRealID(weapon_mesh_id & 0x00ffff00));
	pl->m_cSwordTrace.InsertSwordTraceQueue(trace_pos[0],trace_pos[1]);
	pl->m_cSwordTrace.SetTraceColor(0xffffffff);
	//stWeaponTraceColor[real_id%_MAX_WEAPON_COLOR]);
	return TRUE;
}

void CPlayerDummyRender(void *player)	//몬스터 관련 특별한 효과 이벤트 처리함수 프래임무브.
{
	CPlayer *pl = (CPlayer *)player;
	if( !pl->IsEnable( ROSF_RENDER_BBOX ) )	//바운드 박스 뿌릴때 
		return;

	D3DXMATRIX matWorld;

	g_ifCharIF->UnSetState();
	D3DXMatrixIdentity( &matWorld );
	GetD3dDevice()->SetTransform( D3DTS_WORLD, &matWorld );

	EFUTIL_DrawLineDummyWeapon(pl,_DUMMY_TRACE1,0xffffffff);
	EFUTIL_DrawLineDummyWeapon(pl,_DUMMY_TRACE2,0xff0000ff);
	EFUTIL_DrawLineDummyWeapon(pl,_DUMMY_FIRE1,0xffffff00);

//	EFUTIL_DrawLineDummy(pl,_DUMMY_FIRE1,0xffffff00);
//	EFUTIL_DrawLineDummy(pl,_DUMMY_FIRE2,0xffffff00);
//	EFUTIL_DrawLineDummy(pl,_DUMMY_FIRE3,0xff00ff70);
//	EFUTIL_DrawLineDummy(pl,_DUMMY_FIRE4,0xff00ff70);
	EFUTIL_DrawLineDummy(pl,_DUMMY_SPECIAL1,0xffffff00);
	EFUTIL_DrawLineDummy(pl,_DUMMY_FOOT1,0xff0000ff);
	EFUTIL_DrawLineDummy(pl,_DUMMY_FOOT2,0xff0000ff);
	EFUTIL_DrawLineDummy(pl,_DUMMY_EXAUST1,0xffff0000);
	EFUTIL_DrawLineDummy(pl,_DUMMY_EXAUST2,0xffff0000);
	EFUTIL_DrawLineDummy(pl,_DUMMY_DAMAGE2,0xffffffff);

	g_ifCharIF->SetState();
}

void CPlayerSpecialEffectRender(void *player)	//플레이어 관련 특별한 효과 이벤트 처리함수.
{
	CPlayer *pl = (CPlayer *)player;
/*
	Vector3f a;
	Vector3f b;
	a[0] = pl->GetPosX()+20;
	a[1] = pl->GetPosY()+10;
	a[2] = pl->GetPosZ();
	b[0] = pl->GetPosX()-20;
	b[1] = pl->GetPosY()+10;
	b[2] = pl->GetPosZ();
//	SetDefaultFlags();
//	BlendOff();
//	MutilTexOff();
//	g_ifCharIF->UnSetState();

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	GetD3dDevice()->SetTransform( D3DTS_WORLD, &matWorld );

	R3DrawLine(a,b,0xffffff00);
	GetD3dDevice()->SetRenderState( D3DRS_FOGENABLE, FALSE );
*/
	CPlayerDummyRender(pl);
	if( pl->m_cSwordTrace.IsDraw() )	//2개는 되야 폴리곤을 그리지.
	{
		CEquipItem *item = pl->GetEquipItem( IEPT_WEAPON );
		DWORD weapon_mesh_id;
		if( item )
		{
			weapon_mesh_id = item->GetMeshID();
			DWORD trace_id = ( weapon_mesh_id & 0x0000ff00 )>>8;
			if( trace_id > 0x0e ||  trace_id == 5 || trace_id == 8 )	//궤적없는 무기는 보낸다.
			{
				pl->m_cSwordTrace.mEntity=0;
				pl->m_cSwordTrace.SetDisable();
				return;
			}
			pl->m_cSwordTrace.mEntity = GetEntityFromEffectEntityList( GetRealID(weapon_mesh_id & 0x00ffff00));
		}
		else
			pl->m_cSwordTrace.mEntity=0;
	}
	pl->m_cSwordTrace.UpdateSwordTrace();



}

DWORD stBeamMeshID[]=
{
	//나이프
	0xA10474,
	//소드
	0xA10274,
	//스태프
	0xA10D6C,
	//액스
	0xA10644,
	0xA1094C,
	0xA10754,
	0xA10774,
	0xA10788,
	//메이스
	0xA10A4C,
	0xA10A74,
	0xA10B9C,
	0xA10BB0,
	//스피어
	0xA10E4C,
	0xA10E60,
	0xA10E88,
};
BOOL IsBeamWeapon(DWORD m_id)
{
	for( DWORD i=0; i<(sizeof(stBeamMeshID)/sizeof(DWORD)); i++)
	{
		if( m_id == stBeamMeshID[i] )
			return TRUE;
	}
	return FALSE;
}

void CUnitSpecialEffectFrameMove(CUnitProc *unit)
{
	if( !unit )
		return;
	CPlayer *pl = unit->GetPlayer();
	if( !pl )
		return;
	DWORD bone_id= pl->GetBoneID();
	Vector3f pos;

	DWORD action = pl->GetAction();
	if( bone_id == 0 )	//골리앗.
	{
		DWORD frame = pl->GetBoneFrame();
		DWORD old_frame = pl->GetOldBoneFrame();

		if( action == CAI_COMBAT_ATTACK )
		{
			if( ( pl->GetAnimationID()&0xf )== 1 )		//점프동작공격이다.
			{
				if( CompareFrame(old_frame,frame,35*160))
				{
					pl->GetPosition(pos);
					Play3DSound(0X6F00000F,pos);	//걷는소리다.
					float density = GetDensityFromPos(pos);
					::R3EnvironmentQuake(.6f,0.7f*density);
					EFUTIL_GetPosCharFromBone(&pos,pl,"Bip01 R Toe0");
					::StartMagicEffect(pos,0,0X6F00000F);	//발자국 이펙트
					EFUTIL_GetPosCharFromBone(&pos,pl,"Bip01 L Toe0");
					::StartMagicEffect(pos,0,0X6F00000F);	//발자국 이펙트
				}
			}
		}
		else
		if( action == CAI_MOVE_MOVING )
		{
			if( pl->GetWalkMode() == CMM_MOVE_WALK )
			{
				if( CompareFrame(old_frame,frame,3*160) || CompareFrame(old_frame,frame,26*160) )
				{
					pl->GetPosition(pos);
					Play3DSound(0X6F00000F,pos);	//걷는소리다.
					float density = GetDensityFromPos(pos);
					::R3EnvironmentQuake(.5f,0.3f*density);
					if( CompareFrame(old_frame,frame,3*160) )
						EFUTIL_GetPosCharFromBone(&pos,pl,"Bip01 R Toe0");
					else
						EFUTIL_GetPosCharFromBone(&pos,pl,"Bip01 L Toe0");
					::StartMagicEffect(pos,0,0X6F00000F);	//발자국 이펙트
				}
			}
			if( pl->GetWalkMode() == CMM_MOVE_RUN )		//부스터
			{
				if( !pl->GetEffectID(1) )
				{
					pl->GetPosition(pos);
					DWORD dwEffectID = StartMagicEffectType((CCharacter *)pl,pos,pl->GetRotY(),0X6F00000E,NULL,_EF_STATE_ALWAYS_LIVE);
					pl->SetEffectID(1,dwEffectID);
				}
				else
				{
					EFUTIL_GetPosTypeFromChar(pos,(CCharacter *)pl,pl->GetEffectID(1));
					MovePosMagicEffect(pl->GetEffectID(1),pos,pl->GetRotY());
				}
			}
		}
		if( pl->GetEffectID(1) )
		{
			if( action != CAI_MOVE_MOVING || pl->GetWalkMode() != CMM_MOVE_RUN )
			{
				SetDisapearTimeMagicEffect(pl->GetEffectID(1),0);
				pl->SetEffectID(1,0);
			}
		}
	}

}
void CUnitSpecialEffectSecAction(CUnitProc *unit)	//유닛 액션처리 특별한 함수.
{
	if( !unit )
		return;
	CPlayer *pl = unit->GetPlayer();
	if( !pl )
		return;

//	Vector3f pos;
//	DWORD pi_dwActionID = pl->GetAction(),ef_id;
	DWORD pi_dwActionID = pl->GetAction();
	switch( pi_dwActionID )
	{
		case CAI_COMBAT_ATTACK :
			if( pl->GetCharTypeID() == CTI_AVATAR )	
				EFUTIL_SetBattleMusicState();	//아바타라면 배틀 사운드를 위한.
//			ef_id = 0x6ff00000|unit->GetAnimationID();
			break;
	}
}


void CPlayerSpecialEffectSecAction(void *player)	//플레이어 액션처리 특별한 함수.
{
	if( !player )
	{
		return;	
	}
	CPlayer *pl = (CPlayer *)player;
	Vector3f pos;

	pl->CleanSkillAtackCnt();	//동작이 바뀌면 어택 
	DWORD pi_dwActionID = pl->GetAction();
	switch( pi_dwActionID )
	{
     	case CAI_SKILL_INVOKE :
//			if( pl->GetCharTypeID() == CTI_AVATAR )	
//				EFUTIL_SetBattleMusicState();	//아바타라면 배틀 사운드를 위한.
//			pl->m_cSwordTrace.SetTraceState(TRUE);
			break;
		case CAI_COMBAT_ATTACK :
			if( pl->GetCharTypeID() == CTI_AVATAR )	
				EFUTIL_SetBattleMusicState();	//아바타라면 배틀 사운드를 위한.
			if( pl->GetEquipItem( CEPT_WEAPON ) )
			{
				//pl->m_cSwordTrace.SetTraceState(TRUE);
				pl->SetTraceOfWeapon(TRUE);
				DWORD l_dwMeshIDEff = EFUtil_GetAttackMaterial(pl);
				DWORD l_dwWeaponMeshID = ( pl->GetEquipItem( CEPT_WEAPON )->GetMeshID() >> 8 ) & 0x000000FF;

				DWORD l_dwMotionID=pl->GetAnimationID();
				//--장언일 추가코드 동작들어가면 효과음 바로 나간다.
				if( l_dwWeaponMeshID <= 0x0e)//타이밍있는 있는 원거리경우를 제외하고 모션 시작할때 나간다.
				{
					Vector3f pos;
					pl->GetPosition(pos);					
					DWORD add_id = ((l_dwMotionID&0xf0)>>4);
					if( add_id == 2 )
						add_id = 3;
					add_id = (add_id+(l_dwMotionID&0xf))*3;
					Play3DSound(_ROOT_START_ATTACK_SOUND + l_dwWeaponMeshID*50 + rand()%2 + add_id,pos);	//무기 사용시 해당 무기아이디와 위치를 날려주면플레이...
				}
			}
			break;
		case CAI_DEATH_READY:	//장언일 추가코드 캐릭터 죽는 사운드를 넣는다.
			if( pl->GetCharTypeID() == CTI_AVATAR )
			{
				_GetCamera()->SetSpecialCameraMode(5);
			}
			pl->GetPosition(pos);	//3d사운드를 위한 위치를 얻어온다.
			if( pl->GetRace() == CRT_ACCRETIA )
				::Play3DSound(277060,pos);
			else
			if( pl->GetRace() == CRT_CORA_MALE ) 
				::Play3DSound(277054,pos);
			else
			if( pl->GetRace() == CRT_CORA_FEMALE ) 
				::Play3DSound(277057,pos);
			else
			if( pl->GetRace() == CRT_BELLATOR_MALE ) 
				::Play3DSound(277048,pos);
			else
				::Play3DSound(277051,pos);
			pl->SetTraceOfWeapon(FALSE);
			break;
		case CAI_ITEM_USE :
			/*
			pl->GetPosition(pos);	//3d사운드를 위한 위치를 얻어온다.
			//포션이다.
			if( pl->GetRace() == CRT_ACCRETIA )
			{
				::Play3DSound(12002,pos);
				::StartMagicEffect(pos,pl->GetRotY(),10002);
			}
			else
			if( ( pl->GetRace() == CRT_CORA_MALE ) || ( pl->GetRace() == CRT_CORA_FEMALE ) )
			{
				::Play3DSound(12001,pos);
				::StartMagicEffect(pos,pl->GetRotY(),10001);
			}
			else	//벨라토 
			{
				::Play3DSound(12000,pos);
				::StartMagicEffect(pos,pl->GetRotY(),10000);
			}*/
			break;
		case CAI_MOVE_STOP :
		case CAI_DEATH_DIE :
		case CAI_MOVE_MOVING:
			pl->SetTraceOfWeapon(FALSE);
			break;
	}
}


void CPlayerSpecialEffectFrameMove(void *player)	//플레이어 관련 특별한 효과 이벤트 처리함수.
{
	CPlayer *pl = (CPlayer *)player;

	DWORD pi_dwActionID = pl->GetAction();
	DWORD pi_dwOldActionID = pl->GetPrevAction();
#define _DAMAGE_TIME	0.27f
#define _DELAY_TIME_1	0.20f

#define _DELAY_TIME	0.1f

 	D3DXMATRIX rot;
	CHARACTEROBJECT *bn;
	float damage_time;
	CMesh * co = NULL;

	EFUTIL_CheckContinuousSkillForce((CCharacter *)pl);

	damage_time=pl->GetDamageTimeForShake();	//맞을때 허리제끼기...
	bn = g_pBoneMgr->GetBone( RLI_PLAYER, pl->GetBoneID() );


	if( !_GetLevel()->IsLoadedBsp() )
		return;
	//--물퍼지는 효과.
	Vector3f pos,uppos,cpos,prepos;
	pl->GetPosition(pos);
	pl->GetPrevPosition(prepos);
	if( pl->m_byToggle7fps != R3GetToggle7fps() )
	if( (prepos[0] != pos[0] || prepos[2] != pos[2])  )
	{
		pl->m_byToggle7fps = R3GetToggle7fps();
		Vector3fCopy(pos,uppos);
		uppos[1]+=20;
		if( _GetLevel()->mBsp->IsInWater(uppos,pos,&cpos,0.0f) )	//물속 효과 
		{
			pl->m_fWaterFoot=4;					// 물 발자국 찍기위한 변수 
			pl->m_byIsInWater=TRUE;
			::StartMagicEffect(cpos,0,0x50FFFF50);	//물 퍼지는거
			if( cpos[1] - pos[1] < 7 )
				::StartMagicEffect(cpos,0,0x50FFFF51);	//물 튀기는거
		}
		else
		{
			//pl->m_fWaterFoot=0;
			pl->m_byIsInWater=FALSE;
		}
	}

	/*MAYUN 
	if( bn )
	{
		if( damage_time > 0 )
		{
			if( _DELAY_TIME_1 < damage_time )
			{
				float su=_DAMAGE_TIME-_DELAY_TIME_1;
				float gac = ((damage_time-_DELAY_TIME_1)/su)*50.0f;
				MatrixRotate(rot.m,0,0,-50+gac);
			}
			else			
			{
				float gac = (damage_time/_DELAY_TIME_1)*30.0f;
				MatrixRotate(rot.m,0,0,-gac);
			}
			//_GetCharIF()->SetEyeVec("Bip01 Head",bn,rot);
	 		_GetCharIF()->SetEyeVec("Bip01 Spine1",bn,rot);
			damage_time-=R3GetLoopTime();
			pl->SetDamageTimeForShake(damage_time);
			if( pl->GetCharTypeID() == CTI_AVATAR )	
				EFUTIL_SetBattleMusicState();	//아바타라면 배틀 사운드를 위한.
		}
		else
		{
			MatrixIdentity(rot.m);
			//_GetCharIF()->SetEyeVec("Bip01 Head",bn,rot);
	 		_GetCharIF()->SetEyeVec("Bip01 Spine1",bn,rot);
		}
	}
	MAYUN*/
/*
	if( pl->IsEqualUnitMountState( UMS_MOUNT ) )
	{
		ProcessUnit(pl);
	}
*/

	//걷기나 뛰기 발자국소리...
	if( pi_dwActionID == CAI_MOVE_MOVING )		//아크레시아 277009
	{
		DWORD frame = pl->GetBoneFrame();
		DWORD old_frame = pl->GetOldBoneFrame();
		if( frame - old_frame < 160*15 )	//발자국이나 뜀박질은 프래임차이가 클때는 생까자 
		{
			if( pl->GetWalkMode() == CMM_MOVE_WALK )
			{
				
				if( CompareFrame(old_frame,frame,160*10) || CompareFrame(old_frame,frame,160*25) )
				{
					Vector3f pos,temp_f;
					pl->GetPosition(pos);
					Play3DSound(277000,pos);	//걷는소리다.
					if( !pl->m_byIsInWater && pl->m_fWaterFoot > 0)	//물밖에 나왔을때 효과
					{
						DWORD water_foot;
						if( pl->GetRace() == CRT_ACCRETIA )
							water_foot=0x50FFFF73;
						else
						if( pl->GetRace() == CRT_CORA_FEMALE ) 
							water_foot=0x50FFFF72;
						else
							water_foot=0x50FFFF71;

						if( CompareFrame(old_frame,frame,160*10) )
							EFUTIL_GetPosType(temp_f,pl,_POS_TYPE_R_TOE);
						else
							EFUTIL_GetPosType(temp_f,pl,_POS_TYPE_L_TOE);
						temp_f[1] = pos[1] + 0.7f;
						StartMagicEffect(temp_f,pl->GetRotY(),water_foot);
					}
				}
			}		
			else if( pl->GetWalkMode() == CMM_MOVE_RUN )
			{
				DWORD frame_1,frame_2;

				if( pl->GetRace() == CRT_ACCRETIA )
				{
					frame_1 = 160*7;
					frame_2 = 160*18.5f;
				}
				else
				{
					frame_1 = 160*10;
					frame_2 = 160*21.5f;
				}

	//			if( CompareFrame(old_frame,frame,160*7) || CompareFrame(old_frame,frame,160*19) )
				if( CompareFrame(old_frame,frame,frame_1) || CompareFrame(old_frame,frame,frame_2) )
				{
					Vector3f pos,temp_f;
					pl->GetPosition(pos);
					Play3DSound(277006,pos);	//걷는소리다.
					
					if( !pl->m_byIsInWater && pl->m_fWaterFoot > 0)	//물밖에 나왔을때 효과
					{
						DWORD water_foot;
						if( pl->GetRace() == CRT_ACCRETIA )
							water_foot=0x50FFFF73;
						else
						if( pl->GetRace() == CRT_CORA_FEMALE ) 
							water_foot=0x50FFFF72;
						else
							water_foot=0x50FFFF71;

						if( CompareFrame(old_frame,frame,frame_1) )
							EFUTIL_GetPosType(temp_f,pl,_POS_TYPE_R_TOE);
						else
							EFUTIL_GetPosType(temp_f,pl,_POS_TYPE_L_TOE);
						temp_f[1] = pos[1] + 0.7f;
						StartMagicEffect(temp_f,pl->GetRotY(),water_foot);
					}
				}
			}
		}
		if( !pl->m_byIsInWater )
			pl->m_fWaterFoot-=R3GetLoopTime();

	}

	/*
	if( pi_dwOldActionID == pi_dwActionID )
		goto SKIP_CHANGE_ACTION;


SKIP_CHANGE_ACTION:*/
	//궤적 처리 프레임 무브 ....
	if( !ProcessSwordTraceFrameMove(pl) )
	{	//비정상적인경우 끈다.
		pl->m_cSwordTrace.SetDisable();
	}

#define _MOVE_TIME	0.5f

	if( pl->m_fMoveMap != 0 ) //포탈이동이나 맵이동시 캐릭터가 나타나고 사라지고를 보여주는 거다.
	{
		if( pl->m_fMoveMap < 0 )		//사라지는 거다.
		{
			pl->m_fMoveMap+=R3GetLoopTime()/_MOVE_TIME;
			if( pl->m_fMoveMap > 0 )
			{
				pl->m_fMoveMap=0;
				SetDisapearTimeMagicEffect(pl->m_dwMoveUniqueID,0);
			}
		}
		else
		{
			pl->m_fMoveMap-=R3GetLoopTime()/_MOVE_TIME;
			if( pl->m_fMoveMap < 0 )
			{
				pl->m_fMoveMap=0;
				SetDisapearTimeMagicEffect(pl->m_dwMoveUniqueID,0);
			}
		}
	}
}

	
void CPlayerSpecialEffectCreateDummy(void *player,CHARACTEROBJECT *co)
{
	CPlayer *pl = (CPlayer *)player;

	if( player == NULL || co == NULL )
		return;

	DWORD		MaxObj		= co->m_MaxObj;
	ObjectMesh	*pMesh		= co->m_pMesh;

	if( pMesh==NULL )
		return;

	char buf[256];

	//더미들을 찾아보자.
	for (int m=0;m<MaxObj;m++)
	{
		_strcpy(buf,pMesh[m].m_Name,20);
		strlwr(buf);
		if(!strcmp(buf,"damage2"))
			pl->SetDummyIndex(_DUMMY_DAMAGE2,m);
		else
		if(!strcmp(buf,"fire1"))
			pl->SetDummyIndex(_DUMMY_FIRE1,m);
		else
		if(!strcmp(buf,"fire2"))
			pl->SetDummyIndex(_DUMMY_FIRE2,m);
		else
		if(!strcmp(buf,"fire3"))
			pl->SetDummyIndex(_DUMMY_FIRE3,m);
		else
		if(!strcmp(buf,"fire4"))
			pl->SetDummyIndex(_DUMMY_FIRE4,m);
		else
		if(!strcmp(buf,"special1"))
			pl->SetDummyIndex(_DUMMY_SPECIAL1,m);
		else
		if(!strcmp(buf,"foot1"))
			pl->SetDummyIndex(_DUMMY_FOOT1,m);
		else
		if(!strcmp(buf,"foot2"))
			pl->SetDummyIndex(_DUMMY_FOOT2,m);
		else
		if(!strcmp(buf,"exaust1"))
			pl->SetDummyIndex(_DUMMY_EXAUST1,m);
		else
		if(!strcmp(buf,"exaust2"))
			pl->SetDummyIndex(_DUMMY_EXAUST2,m);
	}

	MaxObj=0;
	if( !pl->GetDummyIndex(_DUMMY_DAMAGE2))
		pl->SetDummyIndex(_DUMMY_DAMAGE2,MaxObj);
	if( !pl->GetDummyIndex(_DUMMY_FIRE1))
		pl->SetDummyIndex(_DUMMY_FIRE1,MaxObj);
	if( !pl->GetDummyIndex(_DUMMY_FIRE2))
		pl->SetDummyIndex(_DUMMY_FIRE2,MaxObj);
	if( !pl->GetDummyIndex(_DUMMY_FIRE3))
		pl->SetDummyIndex(_DUMMY_FIRE3,MaxObj);
	if( !pl->GetDummyIndex(_DUMMY_FIRE4))
		pl->SetDummyIndex(_DUMMY_FIRE4,MaxObj);
	if( !pl->GetDummyIndex(_DUMMY_SPECIAL1))
		pl->SetDummyIndex(_DUMMY_SPECIAL1,MaxObj);
	if( !pl->GetDummyIndex(_DUMMY_FOOT1))
		pl->SetDummyIndex(_DUMMY_FOOT1,MaxObj);
	if( !pl->GetDummyIndex(_DUMMY_FOOT2))
		pl->SetDummyIndex(_DUMMY_FOOT2,MaxObj);
	if( !pl->GetDummyIndex(_DUMMY_EXAUST1))
		pl->SetDummyIndex(_DUMMY_EXAUST1,MaxObj);
	if( !pl->GetDummyIndex(_DUMMY_EXAUST2))
		pl->SetDummyIndex(_DUMMY_EXAUST2,MaxObj);

}

