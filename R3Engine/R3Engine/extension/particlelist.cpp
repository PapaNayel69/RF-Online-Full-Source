//#include "StdAfx.h"
#include <stdio.h>
#include "ParticleList.h"

#ifdef RTMOVIE

CParticleManager_	*CParticleManager_::m_pThis = NULL;

void	CParticleManager_::AddParticle(char *name,DWORD	id)
{
	if (id>=MAXLOADPARTICLE)
	{
		MessageBox(NULL,"최대 파티클 초과","",0);
		return ;
	}
	if (m_Particle[id].m_bLoad) 
	{
		char string[128];
		sprintf(string,"이미 로딩된 Index:%d",id);
		MessageBox(NULL,string,"",0);
	}

	

	if( m_Particle[id].m_Particle.LoadParticleSPT((char*)name,_DO_NOT_MANAGE_TEXTURE ))
	{	
		m_Particle[id].m_Particle.InitParticle();
		m_Particle[id].m_Particle.SetParticleState(_PARTICLE_STATE_START);
		m_Particle[id].m_Particle.SetCreatePos(D3DXVECTOR3(0,0,0));
		m_Particle[id].m_bLoad = true;
		m_Counter++;
	}
}

void	CParticleManager_::AddParticle(char *name,DWORD	id,CHPARTICLE_ pt)
{
	if (id>=MAXLOADPARTICLE) 
	{
		MessageBox(NULL,"최대 엔티티 초과","",0);
		return ;
	}
	if (m_Particle[id].m_bLoad)
	{
		char string[128];
		sprintf(string,"이미 로딩된 Index:%d",id);
		MessageBox(NULL,string,"",0);
	}

	

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
}
void	CParticleManager_::AddEntity(char *name,DWORD	id,CHENTITY_ en)
{
	if (id>=MAXLOADPARTICLE) {
		MessageBox(NULL,"최대 엔티티 초과","",0);
		return ;
	}
	if (m_Entity[id].m_bLoad){
		char string[128];
		sprintf(string,"이미 로딩된 Index:%d",id);
		MessageBox(NULL,string,"",0);
		return ;
	}
	
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
}
void	CParticleManager_::AddEntity(char *name,DWORD	id,float scale[3])
{


	if (id>=MAXLOADPARTICLE) {
		MessageBox(NULL,"최대 엔티티 초과","",0);
		return ;
	}
	if (m_Entity[id].m_bLoad) {
		char string[128];
		sprintf(string,"이미 로딩된 Index:%d",id);
		MessageBox(NULL,string,"",0);
	}
	
	if( m_Entity[id].m_Entity.LoadEntity(name) )
	{
		m_Entity[id].m_Entity.RestoreTexMem();	
		m_Entity[id].m_fScale[0] = scale[0];
		m_Entity[id].m_fScale[1] = scale[1];
		m_Entity[id].m_fScale[2] = scale[2];
		
		m_Entity[id].m_bLoad  = true;
		m_Counter++;
	}
}
BOOL	CParticleManager_::LoadParticleIni(char *filename)
{
	char section[256]= "PART";
	DWORD	maxparticle = GetPrivateProfileInt("PARTICLE_INFO", "MAXPARTICLE", 0 , filename);
	for (DWORD i=0;i<maxparticle;i++)
	{
		char fname[256],scale[256];//,bname[256];
		CHPARTICLE_	en;
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

BOOL	CParticleManager_::LoadEntityIni(char *filename)
{
	char section[256]= "PART";
	DWORD	maxparticle = GetPrivateProfileInt("ENTITY_INFO", "MAXENTITY", 0 , filename);
	for (DWORD i=0;i<maxparticle;i++)
	{
		char fname[256],scale[256];//,bname[256];
		sprintf(section,"ENTITY%d",i+1);
		//float sc[3] = {1.0,1.0,1.0};
		CHENTITY_ en;
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
void	CParticleManager_::ReleaseParticle()
{
	for (int i=0;i<MAXLOADPARTICLE;i++)
	{
		m_Particle[i].Release();
	}
}
void	CParticleManager_::ReleaseEntity()
{
	for (int i=0;i<MAXLOADPARTICLE;i++)
	{
		m_Entity[i].Release();
	}

}

void	CParticleManager_::ReloadParticle(char *filename)
{
	ReleaseParticle();
	LoadParticleIni(filename);
}

void	CParticleManager_::ReloadEntity(char *filename)
{
	ReleaseEntity();
	LoadEntityIni(filename);
}
void	CParticleManager_::RestoreAll()
{
	for (int i=0;i<MAXLOADPARTICLE;i++)
	{
		m_Particle[i].Release();
	}
}

#endif

