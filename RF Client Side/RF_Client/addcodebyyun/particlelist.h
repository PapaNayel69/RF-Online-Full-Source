#ifndef __PARTICLELIST__
#define __PARTICLELIST__

#include "R3Particle.h"
#include "Character.h"

#define		MAXLOADPARTICLE	1024	

#define		MAX_DUMMY_PARTICLE	5
struct CHPARTICLE
{
	BOOL		m_bLoad;
	BOOL		m_bRealLoad;
	CParticle	m_Particle[MAX_DUMMY_PARTICLE];
	BOOL		m_bAbsAxis;
	float		m_fScale[3];
	string		m_sFileName;
	DWORD		m_Count;
	
	CHPARTICLE()
	{
		m_Count		= 0;
		m_bRealLoad	= false; 
		m_fScale[0] = m_fScale[1] = m_fScale[2] = 1.0f;
		m_bLoad		= false;
		m_bAbsAxis	= true;
	}
	void Release()
	{
		/*
		for (int i = 0;i<MAX_DUMMY_PARTICLE;i++)
		{
			m_Particle[i].ReleaseParticle();
			m_Particle[i].ReleaseEntity();
		}
		*/
		m_Particle[0].ReleaseParticle();
		m_Particle[0].ReleaseEntity();
		m_bLoad		= false;
		m_bRealLoad	= false; 
	}
	BOOL RealLoad()
	{
		if( m_Particle[0].LoadParticleSPT( (char*)m_sFileName.data(),_DO_NOT_MANAGE_TEXTURE ))
		{	
			m_Particle[0].InitParticle();
			m_Particle[0].SetParticleState(_PARTICLE_STATE_START);
			m_Particle[0].SetCreatePos(D3DXVECTOR3(0,0,0));
			for ( int i = 1; i < MAX_DUMMY_PARTICLE; i++)
			{
				memcpy(&m_Particle[i],&m_Particle[0],sizeof(CParticle));
				m_Particle[i].InitParticle();
				m_Particle[i].SetParticleState(_PARTICLE_STATE_START);
				m_Particle[i].SetCreatePos(D3DXVECTOR3(0,0,0));
			}
			m_bRealLoad = true;
			return true;
		}
		else
		{
			return false;
		}
	}
};

struct CHENTITY
{
	BOOL		m_bLoad;
	BOOL		m_bRealLoad;
	CEntity		m_Entity;
	BOOL		m_bAbsAxis;
	float		m_fScale[3];
	float		m_fRot[3];
	string		m_sFileName;
	CHENTITY()
	{
		m_bRealLoad	= false; 
		m_fScale[0] = m_fScale[1]	= m_fScale[2]	= 0.0f;
		m_fRot[0]   = m_fRot[1]		= m_fRot[2]		= 0.0f;
		m_bLoad		 = false;
		m_bAbsAxis	 = true;
	}
	void Release()
	{
		m_Entity.ReleaseEntity();
		m_bLoad		= false;
		m_bRealLoad	= false; 
	}
	BOOL RealLoad()
	{
		if( m_Entity.LoadEntity( (char*)m_sFileName.data(),_DO_NOT_MANAGE_TEXTURE) )
		{
			m_Entity.RestoreTexMem();	
			m_bRealLoad  = true;
		}
		else
		{
			return false;
		}
		return true;
	}
};
class	CParticleManager
{

	DWORD		m_Counter;
	//
	CHPARTICLE	m_Particle[MAXLOADPARTICLE];
		
	
	CHENTITY	m_Entity[MAXLOADPARTICLE];
	
	void	AddParticle(char *name,DWORD	id); 
	void	AddParticle(char *name,DWORD	id,CHPARTICLE pt); 
	void	AddEntity(char *name,DWORD	id,float scale[3]);
	void	AddEntity(char *name,DWORD	id,CHENTITY en);
	void	RestoreAll();
public:
	void	ReleaseParticle();
	void	ReleaseEntity();
	void	ResetParticleCount()
	{
		for (int i = 0; i<MAXLOADPARTICLE;i++)
		{
			m_Particle[i].m_Count = 0;
		}
	}
	CParticleManager()
	{
		//m_pThis		= this;
		m_Counter	= 0;
	}
	~CParticleManager()
	{
		ReleaseParticle();
		ReleaseEntity();
		//m_pThis		= NULL;
		m_Counter	= 0;
	}
//static		CParticleManager	*m_pThis;
	CParticle *GetParticle(DWORD ID)
	{
		if (!m_Particle[ID].m_bRealLoad)
		{
			m_Particle[ID].RealLoad();
		}
		DWORD c = m_Particle[ID].m_Count%MAX_DUMMY_PARTICLE;
		m_Particle[ID].m_Count++;
		return &m_Particle[ID].m_Particle[c];
	}
	CEntity *GetEntity(DWORD ID)
	{
		if (!m_Entity[ID].m_bRealLoad)
		{
			m_Entity[ID].RealLoad();
		}
		return &m_Entity[ID].m_Entity;
	}
	CHPARTICLE *GetCHParticle(DWORD ID)
	{
		return &m_Particle[ID];
	}
	CHENTITY *GetCHEntity(DWORD ID)
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
};
#define		ANI_MAX_ID	8
#define		ANI_MAX_LEV	50
class	CAnimusEffect
{
public:
	char*		m_AE[ANI_MAX_ID][ANI_MAX_LEV];
public:
	CAnimusEffect()
	{
		//memset(this,0,sizeof(CAnimusEffect));
	}
	~CAnimusEffect()
	{
		for (int i = 0;	i< ANI_MAX_ID;	i++)
		{
			for (int j = 0;	j< ANI_MAX_LEV;	j++ )
			{
				delete[] m_AE[i][j];
			}
		}
	}
	void LoadEffect(DWORD id, DWORD lv, char *str)
	{
		if ( id >= ANI_MAX_ID  ) return;
		if ( lv >= ANI_MAX_LEV ) return;
		if ( m_AE[id][lv] ) return;
		
		int len = strlen(str);
		if ( len > 256) return;
		m_AE[id][lv] = new char [len+1];
		memcpy(m_AE[id][lv],str,len+1);
		EffectObjectManager::LoadEffect(m_AE[id][lv]);
	}
	void SetEffect(CHARACTEROBJECT *cobj,DWORD id,DWORD lv)
	{
		if (m_AE[id][lv]) 
		{
			EffectObjectManager::SetPartEffect(cobj,m_AE[id][lv],MAX_PATTERN_SIZE);
		}

	}
	void LoadAEffectList(char *filename);
};

//void DrawCharacterctEffect(CHARACTEROBJECT *cobj,float pos[3],float rot);
void DrawCharacterctEffect(CHARACTEROBJECT *cobj,float pos[3],float rot,float alpha,float scale = 1.0f);
void SetCharacterctEffectArgv(DWORD id,Vector3f pos);	//이펙트더미의 인자들...

void SaveScreenData(void  *fn);
void ScreenShot();

extern CParticleManager	g_Particle;

extern CAnimusEffect	g_AniEff;
#endif
/*
#ifndef __PARTICLELIST__
#define __PARTICLELIST__

#include "R3Particle.h"

#define		MAXLOADPARTICLE	1024	

class	CParticleManager
{

	DWORD		m_Counter;
	BOOL		m_LoadParticle[MAXLOADPARTICLE];
	CParticle	m_Particle[MAXLOADPARTICLE];
	
	void	AddParticle(char *name,DWORD	id); 
	
public:
	CParticleManager()
	{
		m_pThis		= this;
		m_Counter	= 0;
		for (int i = 0; i< MAXLOADPARTICLE; i++)
		{
			m_LoadParticle[i]	= 0;
			
		}
	}
static		CParticleManager	*m_pThis;
	CParticle *GetParticle(DWORD ID)
	{
		return &m_Particle[ID];
	}
	BOOL	LoadParticleIni(char *filename);
};

#endif

  */