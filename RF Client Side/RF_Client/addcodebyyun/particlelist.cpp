

#include "Stdio.h"
#include "ParticleList.h"
#include "RenderParticle.h"
#include "R3Util.h"
#include "core.h"

#define		MAX_SURFACE 500

CParticleManager	g_Particle;
CAnimusEffect		g_AniEff;
//CParticleManager	*CParticleManager::m_pThis = NULL;

LPDIRECT3DSURFACE8	psurface[MAX_SURFACE] = {NULL,};
BYTE				g_SurfaceUse[MAX_SURFACE] = {0,};



D3DXMATRIX yzmat
					(	1.0,		0.0,	0.0,	0.0,
						0.0,		0.0,	1.0,	0.0,
						0.0,		1.0,	0.0,	0.0,
						0.0,		0.0,	0.0,	1.0);

static Vector3f st_vecArgv[10];

void SetCharacterctEffectArgv(DWORD id,Vector3f pos)	//이펙트더미의 인자들...
{
	Vector3fCopy(pos,st_vecArgv[id]);
}

void DrawCharacterctEffect(CHARACTEROBJECT *pObj,float Pos[3],float Rot,float alpha,float scale)
{
	static	float enscroll = 0.0f;
	//DWORD	ctime = timeGetTime();
	float	ctime = R3GetTime();
	static	float	btime = 0.0f;
	//static	DWORD	btime = 0.0f;
	if (btime==0.0f)btime = ctime;

	float delta = ((float)(ctime - btime)) * 0.5f;
	btime = ctime;
	enscroll+= delta*30;
	//
	D3DXMATRIX	w_matrix;
	D3DXMATRIX	r_matrix;
	D3DXMATRIX	v_matrix;
	D3DXMATRIX	b_matrix;
	D3DXMATRIX	matrix;
	//
	
	DWORD		MaxObj		= pObj->m_MaxObj;
	ObjectMesh	*pMesh		= pObj->m_pMesh;

	DWORD alphad = ((DWORD)(alpha * 0xff000000) & 0xff000000 )+  0X00ffffff;
							
	
	for (int i=0;i<MaxObj;i++)
	{
		

		for (int j =0;j<MAXPARTICLE;j++)
		{
			CParticle *p = NULL;
			if (pMesh[i].m_ParticleID[j].m_ID)
			{
				p = g_Particle.GetParticle(pMesh[i].m_ParticleID[j].m_ID);

				if (p)
				{
					CHPARTICLE *ch = g_Particle.GetCHParticle(pMesh[i].m_ParticleID[j].m_ID);

					Vector3fCopy(st_vecArgv[0],p->mSpecialARGV[0]);	//이펙트 더미 인자.
					Vector3fCopy(st_vecArgv[1],p->mSpecialARGV[1]);
					if (ch->m_bAbsAxis)
					{
						
						D3DXMatrixIdentity(&w_matrix);
						D3DXMatrixRotationY(&r_matrix,D3DXToRadian(-(180 - Rot)));
						D3DXMatrixMultiply(&w_matrix,&r_matrix,&w_matrix);

						D3DXVECTOR4	v(pMesh[i].m_MatResult._41*scale,
							pMesh[i].m_MatResult._43*scale,
							pMesh[i].m_MatResult._42*scale,0);

						D3DXVec3Transform(&v,(D3DXVECTOR3*)&v,&r_matrix);
						w_matrix._41 = Pos[0] + v.x + pMesh[i].m_ParticleID[j].m_X;
						w_matrix._42 = Pos[1] + v.y + pMesh[i].m_ParticleID[j].m_Y;
						w_matrix._43 = Pos[2] + v.z + pMesh[i].m_ParticleID[j].m_Z;

						if(p->Loop() ==0 )	return ;
						memcpy(p->mRotMat,r_matrix,sizeof(float)*16);
						p->SetCreatePos(w_matrix.m[3]);	//충돌 할땐 필요하다...
						DrawCParticle(p,p->mEntity,w_matrix.m,alphad);	//파티클일경우 파티클을 뿌려주자.
					
					}
					else
					{
						D3DXMATRIX	rot;
						D3DXMatrixIdentity(&w_matrix);

						w_matrix._11 *= scale;
						w_matrix._22 *= scale;
						w_matrix._33 *= scale;

						D3DXMatrixRotationZ(&r_matrix,D3DXToRadian((180 - Rot)));
						D3DXMatrixMultiply(&w_matrix,&r_matrix,&w_matrix);
						D3DXMatrixMultiply(&w_matrix,&w_matrix,&yzmat);
						D3DXMatrixMultiply(&w_matrix,&pMesh[i].m_MatResult,&w_matrix);
						

						w_matrix._41 = Pos[0] + w_matrix._41 + pMesh[i].m_ParticleID[j].m_X;
						w_matrix._42 = Pos[1] + w_matrix._42 + pMesh[i].m_ParticleID[j].m_Y;
						w_matrix._43 = Pos[2] + w_matrix._43 + pMesh[i].m_ParticleID[j].m_Z;
						memcpy(r_matrix,w_matrix,sizeof(float)*16);
						r_matrix._41 = r_matrix._42 = r_matrix._43 = 0;
						if(p->Loop() ==0 )	return ;
						memcpy(p->mRotMat,r_matrix,sizeof(float)*16);
						p->SetCreatePos(w_matrix.m[3]);	//충돌 할땐 필요하다...
						DrawCParticle(p,p->mEntity,w_matrix.m,alphad);	//파티클일경우 파티클을 뿌려주자.
					
					}
				}
			}
		}
		if (pMesh[i].m_Entity.m_ID)
		{
			CEntity *e = g_Particle.GetEntity(pMesh[i].m_Entity.m_ID);
			if (e)
			{
				CHENTITY *ch = g_Particle.GetCHEntity(pMesh[i].m_Entity.m_ID);
				if (ch->m_bAbsAxis)
				{
					D3DXMatrixIdentity(&w_matrix);
					//w_matrix._11 = CParticleManager::m_pThis->GetEntityScale(pMesh[i].m_Entity.m_ID)[0];
					//w_matrix._22 = CParticleManager::m_pThis->GetEntityScale(pMesh[i].m_Entity.m_ID)[1];
					//w_matrix._33 = CParticleManager::m_pThis->GetEntityScale(pMesh[i].m_Entity.m_ID)[2];
					w_matrix._11 *= scale;
					w_matrix._22 *= scale;
					w_matrix._33 *= scale;

					D3DXMatrixRotationY(&r_matrix,D3DXToRadian(-(180 - Rot)));
					D3DXMatrixMultiply(&w_matrix,&r_matrix,&w_matrix);
					
					D3DXVECTOR4	v(pMesh[i].m_MatResult._41,
						pMesh[i].m_MatResult._43,
						pMesh[i].m_MatResult._42,0);
					D3DXVec3Transform(&v,(D3DXVECTOR3*)&v,&r_matrix);
					w_matrix._41 = Pos[0] + v.x + pMesh[i].m_Entity.m_X;
					w_matrix._42 = Pos[1] + v.y + pMesh[i].m_Entity.m_Y;
					w_matrix._43 = Pos[2] + v.z + pMesh[i].m_Entity.m_Z;
					e->DrawEntity((w_matrix.m),alphad,enscroll);
				}
				else
				{
					
					D3DXMatrixIdentity(&w_matrix);
						
						
					//w_matrix._11 = ch->m_fScale[0];
					//w_matrix._22 = ch->m_fScale[1];
					//w_matrix._33 = ch->m_fScale[2];

					w_matrix._11 *= scale;
					w_matrix._22 *= scale;
					w_matrix._33 *= scale;

					D3DXMatrixRotationZ(&r_matrix,D3DXToRadian((180 - Rot)));
					D3DXMatrixMultiply(&w_matrix,&r_matrix,&w_matrix);
					
					w_matrix._41 = Pos[0] + pMesh[i].m_Entity.m_X;
					w_matrix._43 = Pos[1] + pMesh[i].m_Entity.m_Y;
					w_matrix._42 = Pos[2] + pMesh[i].m_Entity.m_Z;

					D3DXMatrixMultiply(&w_matrix,&w_matrix,&yzmat);
					D3DXMatrixMultiply(&w_matrix,&pMesh[i].m_MatResult,&w_matrix);

					

					D3DXMATRIX inv;
					D3DXMatrixInverse(&inv,NULL,&yzmat);
					D3DXMatrixMultiply(&w_matrix,&inv,&w_matrix);
					memcpy(r_matrix,w_matrix,sizeof(float)*16);
					r_matrix._41 = r_matrix._42 = r_matrix._43 = 0;
					
					e->DrawEntity((w_matrix.m),alphad,enscroll);
					
				}
			}
		}
		
	}

}
/*
{
	static	float scroll = 0.0f;
	static	float enscroll = 0.0f;
	DWORD	ctime = timeGetTime();
	static	DWORD	btime = 0.0f;
	if (!btime)btime = ctime;

	float delta = ((float)(ctime - btime)) * 0.0005f;
	
	btime = ctime;
	scroll += delta ;
	enscroll+= delta*30;
			
	if (scroll>1.0)	scroll = 0.0;

	//CRender::GetRender().DrawCharacter(cobj,pos,rot,1.0f,scroll);	
	//CRender::GetRender().UnSetState();	

	//
	D3DXMATRIX	w_matrix;
	D3DXMATRIX	r_matrix;
	D3DXMATRIX	v_matrix;
	//
	DWORD		MaxObj		= cobj->m_MaxObj;
	ObjectMesh	*pMesh		= cobj->m_pMesh;
	for (int i=0;i<MaxObj;i++)
	{
		for (int j =0;j<MAXPARTICLE;j++)
		{
			CParticle *p = NULL;
			if (pMesh[i].m_ParticleID[j].m_ID)
			{
				p = CParticleManager::m_pThis->GetParticle(pMesh[i].m_ParticleID[j].m_ID);
				if (p)
				{
					CHPARTICLE *ch = CParticleManager::m_pThis->GetCHParticle(pMesh[i].m_ParticleID[j].m_ID);
					if (ch->m_bAbsAxis)
					{
						if(p->Loop() ==0 )	return ;
						memcpy(p->mRotMat,r_matrix,sizeof(float)*16);
						p->SetCreatePos(w_matrix.m[3]);	//충돌 할땐 필요하다...
						DrawCParticle(p,p->mEntity,w_matrix.m,0xffffffff);	//파티클일경우 파티클을 뿌려주자.
						//
						D3DXMatrixIdentity(&w_matrix);
						D3DXMatrixRotationY(&r_matrix,D3DXToRadian(-(180 - rot)));
						D3DXMatrixMultiply(&w_matrix,&r_matrix,&w_matrix);
						D3DXVECTOR4	v(pMesh[i].m_MatResult._41*scale,
							pMesh[i].m_MatResult._43*scale,
							pMesh[i].m_MatResult._42*scale,0);
						D3DXVec3Transform(&v,(D3DXVECTOR3*)&v,&r_matrix);
						w_matrix._41 = pos[0] + v.x + pMesh[i].m_ParticleID[j].m_X;
						w_matrix._42 = pos[1] + v.y + pMesh[i].m_ParticleID[j].m_Y;
						w_matrix._43 = pos[2] + v.z + pMesh[i].m_ParticleID[j].m_Z;

						if(p->Loop() ==0 )	return ;
						memcpy(p->mRotMat,r_matrix,sizeof(float)*16);
						p->SetCreatePos(w_matrix.m[3]);	//충돌 할땐 필요하다...
						DrawCParticle(p,p->mEntity,w_matrix.m,0xffffffff);	//파티클일경우 파티클을 뿌려주자.
					}
					else
					{
						

						D3DXMatrixIdentity(&w_matrix);
						w_matrix._11 = ch->m_fScale[0];
						w_matrix._22 = ch->m_fScale[1];
						w_matrix._33 = ch->m_fScale[2];
						D3DXMatrixRotationZ(&r_matrix,D3DXToRadian((180 - rot)));
						D3DXMatrixMultiply(&w_matrix,&r_matrix,&w_matrix);
						D3DXMatrixMultiply(&w_matrix,&w_matrix,&yzmat);
						D3DXMatrixMultiply(&w_matrix,&pMesh[i].m_MatResult,&w_matrix);
						
						
						w_matrix._41 = pos[0] + w_matrix._41 + pMesh[i].m_ParticleID[j].m_X;
						w_matrix._42 = pos[1] + w_matrix._42 + pMesh[i].m_ParticleID[j].m_Y;
						w_matrix._43 = pos[2] + w_matrix._43 + pMesh[i].m_ParticleID[j].m_Z;

						memcpy(r_matrix,w_matrix,sizeof(float)*16);
						r_matrix._41 = r_matrix._42 = r_matrix._43 = 0;

						if(p->Loop() ==0 )	return ;
						memcpy(p->mRotMat,r_matrix,sizeof(float)*16);
						DrawCParticle(p,p->mEntity,w_matrix.m,0xffffffff);	//파티클일경우 파티클을 뿌려주자.
					}
				}
			}
		}
		if (pMesh[i].m_Entity.m_ID)
		{
			CEntity *e = CParticleManager::m_pThis->GetEntity(pMesh[i].m_Entity.m_ID);
			if (e)
			{
				CHENTITY *ch = CParticleManager::m_pThis->GetCHEntity(pMesh[i].m_Entity.m_ID);
				if (ch->m_bAbsAxis)
				{
					D3DXMatrixIdentity(&w_matrix);
					w_matrix._11 = CParticleManager::m_pThis->GetEntityScale(pMesh[i].m_Entity.m_ID)[0];
					w_matrix._22 = CParticleManager::m_pThis->GetEntityScale(pMesh[i].m_Entity.m_ID)[1];
					w_matrix._33 = CParticleManager::m_pThis->GetEntityScale(pMesh[i].m_Entity.m_ID)[2];
					D3DXMatrixRotationY(&r_matrix,D3DXToRadian(-(180 - rot)));
					D3DXMatrixMultiply(&w_matrix,&r_matrix,&w_matrix);
					
					D3DXVECTOR4	v(pMesh[i].m_MatResult._41,
						pMesh[i].m_MatResult._43,
						pMesh[i].m_MatResult._42,0);
					D3DXVec3Transform(&v,(D3DXVECTOR3*)&v,&r_matrix);
					w_matrix._41 = pos[0] + v.x + pMesh[i].m_Entity.m_X;
					w_matrix._42 = pos[1] + v.y + pMesh[i].m_Entity.m_Y;
					w_matrix._43 = pos[2] + v.z + pMesh[i].m_Entity.m_Z;
					e->DrawEntity((w_matrix.m),0xffffffff,enscroll);
				}
				else
				{

					D3DXMatrixIdentity(&w_matrix);
						
						
					w_matrix._11 = ch->m_fScale[0];
					w_matrix._22 = ch->m_fScale[1];
					w_matrix._33 = ch->m_fScale[2];
					D3DXMatrixRotationZ(&r_matrix,D3DXToRadian((180 - rot)));
					D3DXMatrixMultiply(&w_matrix,&r_matrix,&w_matrix);

					//
					
					//
					D3DXMatrixMultiply(&w_matrix,&w_matrix,&yzmat);
					D3DXMatrixMultiply(&w_matrix,&pMesh[i].m_MatResult,&w_matrix);
					
					
					
					
					
					w_matrix._41 = pos[0] + w_matrix._41 + pMesh[i].m_Entity.m_X;
					w_matrix._42 = pos[1] + w_matrix._42 + pMesh[i].m_Entity.m_Y;
					w_matrix._43 = pos[2] + w_matrix._43 + pMesh[i].m_Entity.m_Z;

					
					D3DXMATRIX inv;
					D3DXMatrixInverse(&inv,NULL,&yzmat);
					D3DXMatrixMultiply(&w_matrix,&inv,&w_matrix);
					memcpy(r_matrix,w_matrix,sizeof(float)*16);
					r_matrix._41 = r_matrix._42 = r_matrix._43 = 0;
					
					e->DrawEntity((w_matrix.m),0xffffffff,enscroll);
					
				}
			}
		}
	}
	//CRender::GetRender().SetState();	
	//
	//CRender::GetRender().SetState();	
	//CRender::GetRender().DrawCharacter( cobj, pos, rot );
	//CRender::GetRender().DrawCharacter(cobj,pos,rot,1.0f,scroll);
	//CRender::GetRender().UnSetState();	
	//
}*/
void	CParticleManager::AddParticle(char *name,DWORD	id)
{
	if (id>=MAXLOADPARTICLE)
	{
		//MessageBox(NULL,"최대 파티클 초과","",0);
		return ;
	}
	if (m_Particle[id].m_bLoad) 
	{
		char string[128];
		sprintf(string,"이미 로딩된 Index:%d",id);
		//MessageBox(NULL,string,"",0);
	}

	m_Particle[id].m_sFileName = name;
	m_Particle[id].m_bLoad = true;
	m_Counter++;

	/*
	if( m_Particle[id].m_Particle.LoadParticleSPT((char*)name,_DO_NOT_MANAGE_TEXTURE ))
	{	
		m_Particle[id].m_Particle.InitParticle();
		m_Particle[id].m_Particle.SetParticleState(_PARTICLE_STATE_START);
		m_Particle[id].m_Particle.SetCreatePos(D3DXVECTOR3(0,0,0));
		m_Particle[id].m_bLoad = true;
		m_Counter++;
	}
	else
	{
		char string[128];
		sprintf(string,"파티클 생성 실패Index:%d",id);
		//MessageBox(NULL,string,"",0);
	}
	*/
}

void	CParticleManager::AddParticle(char *name,DWORD	id,CHPARTICLE pt)
{
	if (id>=MAXLOADPARTICLE) 
	{
		//MessageBox(NULL,"최대 엔티티 초과","",0);
		return ;
	}
	if (m_Particle[id].m_bLoad)
	{
		char string[128];
		sprintf(string,"이미 로딩된 Index:%d",id);
		//MessageBox(NULL,string,"",0);
	}
	m_Particle[id].m_sFileName = name;
	m_Particle[id].m_fScale[0] = pt.m_fScale[0];
	m_Particle[id].m_fScale[1] = pt.m_fScale[1];
	m_Particle[id].m_fScale[2] = pt.m_fScale[2];
	m_Particle[id].m_bAbsAxis  = pt.m_bAbsAxis;
	m_Particle[id].m_bLoad = true;
	m_Counter++;
	/*

	if( m_Particle[id].m_Particle.LoadParticleSPT( (char*)name,_DO_NOT_MANAGE_TEXTURE ))
	{	
		m_Particle[id].m_Particle.InitParticle();
		m_Particle[id].m_Particle.SetParticleState(_PARTICLE_STATE_START);
		m_Particle[id].m_Particle.SetCreatePos(D3DXVECTOR3(0,0,0));
		m_Particle[id].m_fScale[0] = pt.m_fScale[0];
		m_Particle[id].m_fScale[1] = pt.m_fScale[1];
		m_Particle[id].m_fScale[2] = pt.m_fScale[2];
		m_Particle[id].m_bAbsAxis  = pt.m_bAbsAxis;
		m_Particle[id].m_bLoad = true;
		m_Counter++;
	}
	else
	{
		char string[128];
		sprintf(string,"파티클 생성 실패Index:%d",id);
		//MessageBox(NULL,string,"",0);
	}
	*/
}
void	CParticleManager::AddEntity(char *name,DWORD	id,CHENTITY en)
{
	if (id>=MAXLOADPARTICLE) {
		//MessageBox(NULL,"최대 엔티티 초과","",0);
		return ;
	}
	if (m_Entity[id].m_bLoad){
		char string[128];
		sprintf(string,"이미 로딩된 Index:%d",id);
		//MessageBox(NULL,string,"",0);
		return ;
	}
	m_Entity[id].m_sFileName = name;
	m_Entity[id].m_fScale[0] = en.m_fScale[0];
	m_Entity[id].m_fScale[1] = en.m_fScale[1];
	m_Entity[id].m_fScale[2] = en.m_fScale[2];
	memcpy(m_Entity[id].m_fRot ,en.m_fRot ,sizeof(float) *3);
	m_Entity[id].m_bAbsAxis	 = en.m_bAbsAxis;
	m_Entity[id].m_bLoad  = true;

	/*
	if( m_Entity[id].m_Entity.LoadEntity(name,_DO_NOT_MANAGE_TEXTURE) )
	{
		m_Entity[id].m_Entity.RestoreTexMem();	
		
		m_Entity[id].m_fScale[0] = en.m_fScale[0];
		m_Entity[id].m_fScale[1] = en.m_fScale[1];
		m_Entity[id].m_fScale[2] = en.m_fScale[2];
		memcpy(m_Entity[id].m_fRot ,en.m_fRot ,sizeof(float) *3);
		m_Entity[id].m_bAbsAxis	 = en.m_bAbsAxis;
		m_Entity[id].m_bLoad  = true;
		
		m_Counter++;
	}
	else
	{
		char string[128];
		sprintf(string,"파티클 생성 실패Index:%d",id);
		//MessageBox(NULL,string,"",0);
	}*/
}
void	CParticleManager::AddEntity(char *name,DWORD	id,float scale[3])
{


	if (id>=MAXLOADPARTICLE) {
		////MessageBox(NULL,"최대 엔티티 초과","",0);
		return ;
	}
	if (m_Entity[id].m_bLoad) {
		char string[128];
		sprintf(string,"이미 로딩된 Index:%d",id);
		//MessageBox(NULL,string,"",0);
	}

	m_Entity[id].m_sFileName = name;
	m_Entity[id].m_fScale[0] = scale[0];
	m_Entity[id].m_fScale[1] = scale[1];
	m_Entity[id].m_fScale[2] = scale[2];
	m_Entity[id].m_bLoad  = true;
	
	/*
	if( m_Entity[id].m_Entity.LoadEntity(name) )
	{
		m_Entity[id].m_Entity.RestoreTexMem();	
		m_Entity[id].m_fScale[0] = scale[0];
		m_Entity[id].m_fScale[1] = scale[1];
		m_Entity[id].m_fScale[2] = scale[2];
		
		m_Entity[id].m_bLoad  = true;
		m_Counter++;
	}
	else
	{
		char string[128];
		sprintf(string,"파티클 생성 실패Index:%d",id);
		//MessageBox(NULL,string,"",0);
	}
	*/
}
BOOL	CParticleManager::LoadParticleIni(char *filename)
{
	char section[256]= "PART";
	DWORD	maxparticle = GetPrivateProfileInt("PARTICLE_INFO", "MAXPARTICLE", 0 , filename);
	for (int i=0;i<maxparticle;i++)
	{
		char fname[256],scale[256];//,bname[256];
		CHPARTICLE	en;
		DWORD		index = 0;
		sprintf(section,"PARTICLE%d",i+1);
		//
		GetPrivateProfileString(section,"INDEX","0",fname,256, filename);
		sscanf(fname,"%d", &index);
		//
		GetPrivateProfileString(section,"PARTICLE","NULL",fname,256, filename);
		
		GetPrivateProfileString(section,"SCALEX","1.0",scale,256, filename);
		sscanf(scale,"%f", &en.m_fScale[0]);
		GetPrivateProfileString(section,"SCALEY","1.0",scale,256, filename);
		sscanf(scale,"%f", &en.m_fScale[1]);
		GetPrivateProfileString(section,"SCALEZ","1.0",scale,256, filename);
		sscanf(scale,"%f", &en.m_fScale[2]);
		GetPrivateProfileString(section,"ABSAXIS","1",scale,256, filename);
		sscanf(scale,"%f", &en.m_bAbsAxis);
		AddParticle(fname,index,en);
		//AddParticle(fname,i+1,en);
		//AddParticle(fname,i+1);
	
	}
	return true;
}

BOOL	CParticleManager::LoadEntityIni(char *filename)
{
	char section[256]= "PART";
	DWORD	maxparticle = GetPrivateProfileInt("ENTITY_INFO", "MAXENTITY", 0 , filename);
	for (int i=0;i<maxparticle;i++)
	{
		char fname[256],scale[256];//,bname[256];
		sprintf(section,"ENTITY%d",i+1);
		//float sc[3] = {1.0,1.0,1.0};
		CHENTITY en;
		DWORD		index = 0;
		//
		GetPrivateProfileString(section,"INDEX","0",fname,256, filename);
		sscanf(fname,"%d", &index);
		//
		GetPrivateProfileString(section,"ENTITY","NULL",fname,256, filename);
		
		GetPrivateProfileString(section,"SCALEX","1.0",scale,256, filename);
		sscanf(scale,"%f", &en.m_fScale[0]);
		GetPrivateProfileString(section,"SCALEY","1.0",scale,256, filename);
		sscanf(scale,"%f", &en.m_fScale[1]);
		GetPrivateProfileString(section,"SCALEZ","1.0",scale,256, filename);
		sscanf(scale,"%f", &en.m_fScale[2]);
		GetPrivateProfileString(section,"ABSAXIS","1",scale,256, filename);
		sscanf(scale,"%f", &en.m_bAbsAxis);
		//////////////////////////////////////////////////////////////////////////
		GetPrivateProfileString(section,"ROTX","0.0",scale,256, filename);
		sscanf(scale,"%f", &en.m_fRot[0]);
		GetPrivateProfileString(section,"ROTY","0.0",scale,256, filename);
		sscanf(scale,"%f", &en.m_fRot[1]);
		GetPrivateProfileString(section,"ROTZ","0.0",scale,256, filename);
		sscanf(scale,"%f", &en.m_fRot[2]);
		
		AddEntity(fname,index,en);
		//AddEntity(fname,i+1,en);
		//AddEntity(fname,i+1,sc);
	
	}
	return true;
}

void	CParticleManager::ReleaseParticle()
{
	for (int i=0;i<MAXLOADPARTICLE;i++)
	{
		if ( m_Particle[i].m_bRealLoad )
		{
			m_Particle[i].Release();
		}
	}
}
void	CParticleManager::ReleaseEntity()
{
	for (int i=0;i<MAXLOADPARTICLE;i++)
	{
		if ( m_Entity[i].m_bRealLoad )
		{
			m_Entity[i].Release();
		}
	}

}

void	CParticleManager::ReloadParticle(char *filename)
{
	ReleaseParticle();
	LoadParticleIni(filename);
}

void	CParticleManager::ReloadEntity(char *filename)
{
	ReleaseEntity();
	LoadEntityIni(filename);
}
void	CParticleManager::RestoreAll()
{
	for (int i=0;i<MAXLOADPARTICLE;i++)
	{
		m_Particle[i].Release();
	}
}

void  SaveScreenData(void  *fn)
{
	DWORD			count = 0;
	D3DSURFACE_DESC SDesc;
	DWORD			tcount = 0;
	char  filename[256];
	RECT r;
	while (true)
	{
		if (g_SurfaceUse[count])
		{
			//
			psurface[count]->GetDesc(&SDesc);
			///*
			r.bottom = SDesc.Height;
			r.left = 0;
			r.right = SDesc.Width;
			r.top = 0;
			//.\\SCREENSHOTS
			sprintf(filename,".\\SCREENSHOTS\\%d%d%d%d.BMP"
					,(tcount%10000)/1000,(tcount%1000)/100,(tcount%100)/10,tcount%10);

			int rs = 0;
			rs = D3DXSaveSurfaceToFile(filename,D3DXIFF_BMP ,psurface[count],NULL,&r);
			if(rs==D3D_OK)
				tcount++;
			else	break;
			g_SurfaceUse[count] = 0;
			count++;
			if (count>=MAX_SURFACE) count = 0;
		}
		Sleep(0);
	}
}
void SurfaceToBMP()
{

	HRESULT hr;
	LPDIRECT3DDEVICE8 pd3dDevice=GetD3dDevice();

    
	static DWORD				count = 0;

    LPDIRECT3DSURFACE8 backbuf = NULL;

	_R3ENGINE_STATE *state = GetR3State();

	//--lock
	pd3dDevice->GetRenderTarget( &backbuf );

	D3DSURFACE_DESC Desc;
	D3DSURFACE_DESC SDesc;
	backbuf->GetDesc(&Desc);
	

		
	if(count >=	MAX_SURFACE) 
		count = 0;
	if(g_SurfaceUse[count])
	{
		//return;
		//MessageBox(NULL,"더 이상 캡춰를 할 수 없습니다. 아직 저장중이니 기다려 주세요",0,0);
		exit(1);
	}	
	
	
	if (!psurface[0])
	{
		for (int i = 0; i<MAX_SURFACE; i++)
		{
			if( FAILED( hr = pd3dDevice->CreateImageSurface( (DWORD)state->mScreenXsize, (DWORD)state->mScreenYsize, 
				Desc.Format, &psurface[i] ) ) )
			{
				return;
			}
		}
	}
	//*/
	
	psurface[count]->GetDesc(&SDesc);


	RECT r;
	POINT pt = {0,0};
	r.bottom = Desc.Height;
	r.left = 0;
	r.right = Desc.Width;
	r.top = 0;

	
	DWORD result = pd3dDevice->CopyRects(backbuf,&r,1,psurface[count],&pt);
	g_SurfaceUse[count] = 1;
	backbuf->Release();
	count++;
	
}

void ScreenShot()
{
//	char name[256];

	_R3ENGINE_STATE *state = GetR3State();
	SurfaceToBMP();
	/*
	for(int i=0; i<9999; i++)
	{
		sprintf(name,"%s%s%04d.bmp",state->ScreenShotsPath,"T",i);
		if( IsExistFile(name) == FALSE )
		{
			SurfaceToBMP(name);
			break;
		}
	}
	*/
}
/*
CParticleManager	*CParticleManager::m_pThis = NULL;


void	CParticleManager::AddParticle(char *name,DWORD	id)
{
	if (id>=MAXLOADPARTICLE) return ;
	if (m_LoadParticle[id]) return ;

	

	if( m_Particle[id].LoadParticleSPT((char*)name),_DO_NOT_MANAGE_TEXTURE )
	{	
		m_Particle[id].InitParticle();
		m_Particle[id].SetParticleState(_PARTICLE_STATE_START);
		m_Particle[id].SetCreatePos(D3DXVECTOR3(0,0,0));
		m_Counter++;
	}
	

}
BOOL	CParticleManager::LoadParticleIni(char *filename)
{
	char section[256]= "PART";
	int	maxparticle = GetPrivateProfileInt("PARTICLE_INFO", "MAXPARTICLE", 0 , filename);
	for (int i=0;i<maxparticle;i++)
	{
		char fname[256];
		sprintf(section,"PARTICLE%d",i+1);
		GetPrivateProfileString(section,"PARTICLE","NULL",fname,256, filename);
		AddParticle(fname,i+1);
	
	}
	return true;
}
*/


void CAnimusEffect::LoadAEffectList(char *filename)
{
	FILE *fp =	fopen(filename,"rt");
	if (fp)
	{
		
		char	string[256];
		int		n  = 0;
		int		n2 = 0;
		char	buffer[256];
		int		line = 0;
		string[255]=0;
		
		while ( !feof(fp) )
		{
			line++;
			if( fscanf(fp,"%s",buffer)==EOF)	
				break;
			n = atoi(buffer);
			if( fscanf(fp,"%s",buffer)==EOF)	
				break;
			n2 = atoi(buffer);
			if( fscanf(fp,"%s",string)==EOF)	
				break;

			LoadEffect(n,n2,string);
			
		}
		fclose(fp);
	}
}