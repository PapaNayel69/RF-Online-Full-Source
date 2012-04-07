#include "ItemSpecialEffect.h"
#include "foreffect.h"
#include "EffectUtil.h"
#include "EffectCore.h"
//---------------


void CItemSpecialEffectFrameMove(void* ptr,DWORD unique_id)	//아이템은 프래임무브만 있다.
{
	CMeshItem *item = (CMeshItem *)ptr;

	if( item->IsEnable(ROSF_OUT_OF_VISUAL_FIELD_CLIENT) )	//영역 바깥으로 나가면 일단 죽인다.
		SetMagicState(unique_id,_EF_STATE_DEATH);
	else
		SetMagicState(unique_id,_EF_STATE_ALWAYS_LIVE);
	
	if( item->IsEqualItemType( BOT_GROUND_ITEM ) )				//그라운드 아이템일경우...
	{
		Vector3f pos;
		item->GetPosition(pos);
		float rand_num=item->GetRandRot();
		pos[1]+=sin(R3GetTime()*4+rand_num/60.0f)*0.3f;
		MovePosMagicEffect(unique_id,pos,item->GetRotY()+R3GetTime()*50+rand_num);
	}
}

void CItemSpecialEffectCreateDummy( void* ptr, CHARACTEROBJECT *co )	//장언일 추가 코드 무기의경우 궤적이 있기때문에 미리 준비를 해줘야된다.
{
	CMeshItem *item = (CMeshItem *)ptr;
	char buf[256];
	int m;

	DWORD		MaxObj		= co->m_MaxObj;
	ObjectMesh	*pMesh		= co->m_pMesh;

	for ( m=0; m<MAX_EFFECT_DUMMY; m++)
		item->SetDummyIndex(m,0xff);

	//실제 index로 바꾸쟈
	for ( m=0;m<MaxObj;m++)
	{
		_strcpy(buf,pMesh[m].m_Name, 256 );
		strlwr(buf);
		if(!strcmp(pMesh[m].m_Name,"effect1"))
			item->SetDummyIndex(_DUMMY_TRACE1,m);
		else
		if(!strcmp(pMesh[m].m_Name,"effect2"))
			item->SetDummyIndex(_DUMMY_TRACE2,m);
		else
		if(!strcmp(pMesh[m].m_Name,"fire1"))
			item->SetDummyIndex(_DUMMY_FIRE1,m);
		else
		if(!strcmp(pMesh[m].m_Name,"fire2"))
			item->SetDummyIndex(_DUMMY_FIRE2,m);
		else
		if(!strcmp(pMesh[m].m_Name,"fire3"))
			item->SetDummyIndex(_DUMMY_FIRE3,m);
		else
		if(!strcmp(pMesh[m].m_Name,"fire4"))
			item->SetDummyIndex(_DUMMY_FIRE4,m);
/*
		else
		if(!strcmp(pMesh[m].m_Name,"damage1"))
			item->m_byDummy[_DUMMY_DAMAGE1]=m;
		else
		if(!strcmp(pMesh[m].m_Name,"damage2"))
			item->m_byDummy[_DUMMY_DAMAGE2]=m;
		else
		if(!strcmp(pMesh[m].m_Name,"damage3"))
			item->m_byDummy[_DUMMY_DAMAGE3]=m;
		else
		if(!strcmp(pMesh[m].m_Name,"damage4"))
			item->m_byDummy[_DUMMY_DAMAGE4]=m;
*/
	}

	MaxObj=0;
	if( !item->GetDummyIndex(_DUMMY_TRACE1))
		item->SetDummyIndex(_DUMMY_TRACE1,MaxObj);
	if( !item->GetDummyIndex(_DUMMY_TRACE2))
		item->SetDummyIndex(_DUMMY_TRACE2,MaxObj);
}
static void EFUTIL_DrawLineDummy(CMeshItem *item,DWORD dummy,DWORD color)
{
	Vector3f pos;
	item->GetPosition(pos);	//몬스터 위치.
//	Vector3f get,d_pos;
//	d_pos[0]=0;			d_pos[1]=0;			d_pos[2]=0;

	Vector3f a,b;
	a[0]=pos[0]+10;
	a[1]=pos[1];
	a[2]=pos[2];
	b[0]=pos[0]-10;
	b[1]=pos[1];
	b[2]=pos[2];
	R3DrawLine(a,b,color);

	a[0]=pos[0];
	a[1]=pos[1];
	a[2]=pos[2]+10;
	b[0]=pos[0];
	b[1]=pos[1];
	b[2]=pos[2]-10;
	R3DrawLine(a,b,color);
}
/*
void CItemSpecialEffectRender(void *ptr)	//몬스터 관련 특별한 효과 이벤트 처리함수 프래임무브.
{
	CItem *item = (CItem *)ptr;

	Vector3f pos;
	EFUtil_GetDummyPos( pos,item,_DUMMY_FIRE1);
	CMonster *mob = (CMonster *)monster;
	if( !item->IsEnable( ROSF_RENDER_BBOX ) )	//바운드 박스 뿌릴때 
		return;

	D3DXMATRIX matWorld;

	g_ifCharIF->UnSetState();
	D3DXMatrixIdentity( &matWorld );
	GetD3dDevice()->SetTransform( D3DTS_WORLD, &matWorld );


//	DrawLineDummy(mob,_DUMMY_DAMAGE1,0xffff0000);
	EFUTIL_DrawLineDummy(mob,_DUMMY_DAMAGE2,0xff00ff00);
//	DrawLineDummy(mob,_DUMMY_DAMAGE3,0xff0000ff);
//	DrawLineDummy(mob,_DUMMY_DAMAGE4,0xffffff00);
	g_ifCharIF->SetState();
}

*/