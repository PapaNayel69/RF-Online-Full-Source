
#include "windows.h"
#include "r3math.h"
#include "CharacterObj.h"

#ifdef RTMOVIE

static D3DXMATRIX yzmat
					(	1.0,		0.0,	0.0,	0.0,
						0.0,		0.0,	1.0,	0.0,
						0.0,		1.0,	0.0,	0.0,
						0.0,		0.0,	0.0,	1.0);


void DrawEffect(CHARACTEROBJECT *obj,Vector3f pos,float g_Rot)
{
	static	float enscroll = 0.0f;
	DWORD	ctime = timeGetTime();
	static	DWORD	btime = 0.0f;
	if (!btime)btime = ctime;
	float delta = ((float)(ctime - btime)) * 0.0005f;
	btime = ctime;
	enscroll+= delta*30;
	

	

	//
	D3DXMATRIX	w_matrix;
	D3DXMATRIX	r_matrix;
	D3DXMATRIX	v_matrix;
	D3DXMATRIX	b_matrix;
	D3DXMATRIX	matrix;
	//
	DWORD		MaxObj		= obj->m_MaxObj;
	ObjectMesh	*pMesh		= obj->m_pMesh;
	for (int i=0;i<MaxObj;i++)
	{
		

		for (int j =0;j<MAXPARTICLE;j++)
		{
			CParticle *p = NULL;
			if (pMesh[i].m_ParticleID[j].m_ID)
			{
				p = CParticleManager_::m_pThis->GetParticle(pMesh[i].m_ParticleID[j].m_ID);
				if (p)
				{
					CHPARTICLE_ *ch = CParticleManager_::m_pThis->GetCHParticle(pMesh[i].m_ParticleID[j].m_ID);
					if (ch->m_bAbsAxis)
					{
						
						D3DXMatrixIdentity(&w_matrix);
						D3DXMatrixRotationY(&r_matrix,D3DXToRadian(-(180 - g_Rot)));
						D3DXMatrixMultiply(&w_matrix,&r_matrix,&w_matrix);
						D3DXVECTOR4	v(pMesh[i].m_MatResult._41,
							pMesh[i].m_MatResult._43,
							pMesh[i].m_MatResult._42,0);
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
						D3DXMATRIX	rot;
						D3DXMatrixIdentity(&w_matrix);
						D3DXMatrixRotationZ(&r_matrix,D3DXToRadian((180 - g_Rot)));
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
						p->SetCreatePos(w_matrix.m[3]);	//충돌 할땐 필요하다...
						DrawCParticle(p,p->mEntity,w_matrix.m,0xffffffff);	//파티클일경우 파티클을 뿌려주자.
					}
				}
			}
		}
		if (pMesh[i].m_Entity.m_ID)
		{
			CEntity *e = CParticleManager_::m_pThis->GetEntity(pMesh[i].m_Entity.m_ID);
			if (e)
			{
				CHENTITY_ *ch = CParticleManager_::m_pThis->GetCHEntity(pMesh[i].m_Entity.m_ID);
				if (ch->m_bAbsAxis)
				{
					D3DXMatrixIdentity(&w_matrix);
					w_matrix._11 = CParticleManager_::m_pThis->GetEntityScale(pMesh[i].m_Entity.m_ID)[0];
					w_matrix._22 = CParticleManager_::m_pThis->GetEntityScale(pMesh[i].m_Entity.m_ID)[1];
					w_matrix._33 = CParticleManager_::m_pThis->GetEntityScale(pMesh[i].m_Entity.m_ID)[2];
					D3DXMatrixRotationY(&r_matrix,D3DXToRadian(-(180 - g_Rot)));
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
					D3DXMatrixRotationZ(&r_matrix,D3DXToRadian((180 - g_Rot)));
					D3DXMatrixMultiply(&w_matrix,&r_matrix,&w_matrix);

					//
					
					//
					D3DXMatrixMultiply(&w_matrix,&w_matrix,&yzmat);
					D3DXMatrixMultiply(&w_matrix,&pMesh[i].m_MatResult,&w_matrix);
					
					
					
					
					//memcpy(r_matrix,w_matrix,sizeof(float)*16);
					w_matrix._41 = pos[0] + w_matrix._41 + pMesh[i].m_Entity.m_X;
					w_matrix._42 = pos[1] + w_matrix._42 + pMesh[i].m_Entity.m_Y;
					w_matrix._43 = pos[2] + w_matrix._43 + pMesh[i].m_Entity.m_Z;

					//w_matrix._41 = pos[0]+ w_matrix._41;
					//w_matrix._42 = pos[1]+ w_matrix._42;
					//w_matrix._43 = pos[2]+ w_matrix._43;

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
}

void DrawEffectMat(CHARACTEROBJECT *pObj,D3DXMATRIX *w_matrix,DWORD color)
{
	static	float enscroll = 0.0f;
	DWORD	ctime = timeGetTime();
	static	DWORD	btime = 0.0f;
	if (!btime)btime = ctime;
	float delta = ((float)(ctime - btime)) * 0.0005f;
	btime = ctime;
	enscroll+= delta*30;

	DWORD		MaxObj		= pObj->m_MaxObj;
	ObjectMesh	*pMesh		= pObj->m_pMesh;

	D3DXMATRIX	r_matrix;
	D3DXMATRIX	mat;

	for (int i=0;i<MaxObj;i++)
	{
		for (int j =0;j<MAXPARTICLE;j++)
		{
			
			CParticle *p = NULL;
			if (pMesh[i].m_ParticleID[j].m_ID)
			{
				p = CParticleManager_::m_pThis->GetParticle(pMesh[i].m_ParticleID[j].m_ID);
				if (p)
				{
					CHPARTICLE_ *ch = CParticleManager_::m_pThis->GetCHParticle(pMesh[i].m_ParticleID[j].m_ID);

					//D3DXMatrixMultiply(&mat,&w_matrix,&yzmat);
					D3DXMatrixMultiply(&mat,&pMesh[i].m_MatResult,w_matrix);

					memcpy(r_matrix,mat,sizeof(float)*16);
					//D3DXMatrixIdentity(&r_matrix);
					r_matrix._41 = r_matrix._42 = r_matrix._43 = 0;
				

					if(p->Loop() ==0 )	return ;
					memcpy(p->mRotMat,r_matrix,sizeof(float)*16);
					p->SetCreatePos(mat.m[3]);	//충돌 할땐 필요하다...
					color |=0x00ffffff;
					DrawCParticle(p,p->mEntity,mat.m,color);	//파티클일경우 파티클을 뿌려주자.
				}
			}
			
		}
		if (pMesh[i].m_Entity.m_ID)
		{
			CEntity *e = CParticleManager_::m_pThis->GetEntity(pMesh[i].m_Entity.m_ID);
			if (e)
			{
				CHENTITY_ *ch = CParticleManager_::m_pThis->GetCHEntity(pMesh[i].m_Entity.m_ID);
				
				//D3DXMatrixMultiply(&w_matrix,&w_matrix,&yzmat);
				D3DXMatrixMultiply(&mat,&pMesh[i].m_MatResult,w_matrix);
				


				D3DXMATRIX inv;
				D3DXMatrixInverse(&inv,NULL,&yzmat);
				D3DXMatrixMultiply(&mat,&inv,&mat);

				//D3DXMatrixMultiply(&mat,&yzmat,&mat);
				//D3DXMatrixMultiply(&w_matrix,&w_matrix,&yzmat);
				
				e->DrawEntity((mat.m),color,enscroll);
			}
		}
		
	}
}


#endif