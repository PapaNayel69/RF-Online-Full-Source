#ifndef __RPARTICLELIST__
#define __RPARTICLELIST__

#ifdef RTMOVIE

#include "R3Particle.h"

#define		MAXLOADPARTICLE	1024	

struct CHPARTICLE_
{
	BOOL		m_bLoad;
	CParticle	m_Particle;
	BOOL		m_bAbsAxis;
	float		m_fScale[3];
	CHPARTICLE_()
	{
		m_fScale[0] = m_fScale[1] = m_fScale[2] = 1.0f;
		m_bLoad		= false;
		m_bAbsAxis	= true;
	}
	void Release()
	{
		m_Particle.ReleaseParticle();
		m_Particle.ReleaseEntity();
		m_bLoad = false;
	}
};

struct CHENTITY_
{
	BOOL		m_bLoad;
	CEntity		m_Entity;
	BOOL		m_bAbsAxis;
	float		m_fScale[3];
	float		m_fRot[3];
	CHENTITY_()
	{
		m_fScale[0] = m_fScale[1]	= m_fScale[2]	= 0.0f;
		m_fRot[0]   = m_fRot[1]		= m_fRot[2]		= 0.0f;
		m_bLoad		 = false;
		m_bAbsAxis	 = true;
	}
	void Release()
	{
		m_Entity.ReleaseEntity();
		m_bLoad = false;
	}
};
class	CParticleManager_
{

	DWORD		m_Counter;
	//
	CHPARTICLE_	m_Particle[MAXLOADPARTICLE];
		
	
	CHENTITY_	m_Entity[MAXLOADPARTICLE];
	
	void	AddParticle(char *name,DWORD	id); 
	void	AddParticle(char *name,DWORD	id,CHPARTICLE_ pt); 
	void	AddEntity(char *name,DWORD	id,float scale[3]);
	void	AddEntity(char *name,DWORD	id,CHENTITY_ en);
	void	RestoreAll();
	
public:
	CParticleManager_()
	{
		
		m_pThis		= this;
		m_Counter	= 0;
		
	}
static		CParticleManager_	*m_pThis;
	CParticle *GetParticle(DWORD ID)
	{
		return &m_Particle[ID].m_Particle;
	}
	CEntity *GetEntity(DWORD ID)
	{
		return &m_Entity[ID].m_Entity;
	}
	CHPARTICLE_ *GetCHParticle(DWORD ID)
	{
		return &m_Particle[ID];
	}
	CHENTITY_ *GetCHEntity(DWORD ID)
	{
		return &m_Entity[ID];
	}
	float*	GetEntityScale(DWORD ID) 
	{
		return m_Entity[ID].m_fScale;
	}
	BOOL	LoadParticleIni(char *filename);
	BOOL	LoadEntityIni(char *filename);
	void	ReloadParticle(char *filename);
	void	ReloadEntity(char *filename);
	void	ReleaseParticle();
	void	ReleaseEntity();
};

#endif

#endif