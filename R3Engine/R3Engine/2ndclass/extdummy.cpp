#include <stdio.h>
#include "R3math.h"
#include "Jmalloc.h"
#include "collision.h"
#include "extdummy.h"
#include "r3render.h"
#include "core.h"


CExtDummy::CExtDummy()
{
	memset(this,0,sizeof(CExtDummy));
}

CExtDummy::~CExtDummy()
{
}
_EXT_DUMMY *CExtDummy::GetDummy( DWORD index )
{
	if( !mDummy || !mNum)
		return NULL;
	index = min( index, mNum-1 );

	return &mDummy[index];
}

BOOL CExtDummy::IsInBBox(DWORD i,Vector3f pos)
{
	if( !mDummy || !mNum || i >= mNum )
		return FALSE;

	Vector3f get_pos;
	Vector3fTransform( get_pos,pos, mDummy[i].mInvMat );

	if(IsCollisionBBoxPoint(mDummy[i].mBBmin,mDummy[i].mBBmax,get_pos) )
		return TRUE;
	return FALSE;
}

//회전 되지 않은 더미안의 local_pos좌표와 해당 인덱스를 넘겨주면 월드상의 좌표 get_pos를 얻어낸다.
BOOL CExtDummy::GetWorldFromLocal(Vector3f *get_pos,DWORD i,Vector3f local_pos)
{
	if( !mDummy || !mNum || i >= mNum )
		return FALSE;

	Vector3fTransform( get_pos[0],local_pos, mDummy[i].mMat );
	return TRUE;
}

//월드 상의 좌표를 넘겨주면 더미안의 local_pos좌표를 얻어낸다.
BOOL CExtDummy::GetLocalFromWorld(Vector3f *get_pos,DWORD i,Vector3f world_pos)
{
	if( !mDummy || !mNum || i >= mNum )
		return FALSE;

	Vector3fTransform( get_pos[0],world_pos, mDummy[i].mInvMat );
	return TRUE;
}

void CExtDummy::GetDummyList(DWORD flag,DWORD *cnt,DWORD *index_list)	
{
	DWORD i;

	*cnt=0;
	if( !mDummy || !mNum)
		return;

	for(i=0; i<mNum; i++)
	{
		if( flag & mDummy[i].mFlag)
		{
			index_list[*cnt]=i;
			(*cnt)++;
		}
	}
}

BOOL CExtDummy::LoadExtDummy(char *name)
{
	FILE *fp;
	char hole[256];
	DWORD i;

	if(( fp =fopen( name,"rt" ))==NULL) 
		return FALSE;

	memset(this,0,sizeof(CExtDummy));
	mMaxNum=16;
	mDummy = (_EXT_DUMMY *)Dmalloc(sizeof(_EXT_DUMMY)*mMaxNum);

    while(1)	//앞부분 스킵.
    {
		if( fscanf(fp,"%s",hole)==EOF)	
		{
			fclose(fp);
			return FALSE;
		}
        if (!strcmp(hole,"script_begin"))
			break;
	}
	mNum=0;
    while(1)
    {
		if( fscanf(fp,"%s",hole)==EOF)	
			break;
		strlwr(hole);
_EXT_DUMMY_RELOAD:
        if (!strcmp(hole,"script_end"))
			break;
        if ( '*' == hole[0] )
		{
			memset(&mDummy[mNum],0,sizeof(_EXT_DUMMY));
			strncpy(mDummy[mNum].mName,&hole[1],31);
			MatrixIdentity(mDummy[mNum].mMat);
			for(i=0; i<3; i++)
			{
				fscanf(fp,"%s",hole);
				mDummy[mNum].mBBmin[i] = (float)atof(hole);
			}
			for(i=0; i<3; i++)
			{
				fscanf(fp,"%s",hole);
				mDummy[mNum].mBBmax[i] = (float)atof(hole);
			}
			while( 1 )
			{				
				if( fscanf(fp,"%s",hole)==EOF)	
					break;
				if (!strcmp(hole,"script_end"))
					break;

				if (!strcmp(hole,"-node_tm"))
				{
					float *tm = (float *)&mDummy[mNum].mMat[0][0];
					for(i=0; i<16; i++)
					{
						fscanf(fp,"%s",hole);
						tm[i] = (float)atof(hole);
					}
				    MatrixInvert( mDummy[mNum].mInvMat,mDummy[mNum].mMat );
				}
				if (!strcmp(hole,"-id"))
				{
					fscanf(fp,"%s",hole);
					mDummy[mNum].mID = (DWORD)atoi(hole);
				}
				if (!strcmp(hole,"-music"))
					mDummy[mNum].mFlag |= _DUMMY_FLAG_MUSIC;

				if (!strcmp(hole,"-fog_color"))
				{
					DWORD r,g,b;
					mDummy[mNum].mFlag |= _DUMMY_FLAG_FOG;
					fscanf(fp,"%s",hole);
					r = (DWORD)atoi(hole);
					fscanf(fp,"%s",hole);
					g = (DWORD)atoi(hole);
					fscanf(fp,"%s",hole);
					b = (DWORD)atoi(hole);
					mDummy[mNum].mdwArgv[0] = 0xff000000|(r<<16)|(g<<8)|b;
					mDummy[mNum].mfArgv[1] = GetR3State()->mFogStart;
					mDummy[mNum].mfArgv[2] = GetR3State()->mFogEnd;
				}
				if (!strcmp(hole,"-fog_start"))
				{
					fscanf(fp,"%s",hole);
					mDummy[mNum].mfArgv[1] = (float)atof(hole);
				}
				if (!strcmp(hole,"-fog_end"))
				{
					fscanf(fp,"%s",hole);
					mDummy[mNum].mfArgv[2] = (float)atof(hole);
				}
				if( '*' == hole[0] )
					break;
			}
			mNum++;
			if( mNum >= mMaxNum)	//리얼록.
			{
				mDummy = (_EXT_DUMMY *)ReAlloc(mDummy
					,sizeof(_EXT_DUMMY)*(mMaxNum)
					,sizeof(_EXT_DUMMY)*(mMaxNum+32));
				memset(mDummy+mMaxNum,0,sizeof(_EXT_DUMMY)*32 );
				mMaxNum+=32;
			}
			goto _EXT_DUMMY_RELOAD;	//혹시 모르니까.
		}
	}
	fclose(fp);
	return TRUE;
}

void CExtDummy::ReleaseExtDummy()
{
	if( mDummy )
		Dfree(mDummy);
	memset(this,0,sizeof(CExtDummy));
}


void CExtDummy::DrawDummyBBox(DWORD id)
{
	if( !mDummy || !mNum || id >= mNum )
		return;


	_EXT_DUMMY *dummy = &mDummy[id];

	Vector3f get_pos[8],pos;
	pos[0] = dummy->mBBmax[0];
	pos[1] = dummy->mBBmax[1];
	pos[2] = dummy->mBBmax[2];
	Vector3fTransform( get_pos[0],pos, dummy->mMat );
	pos[0] = dummy->mBBmin[0];
	pos[1] = dummy->mBBmax[1];
	pos[2] = dummy->mBBmax[2];
	Vector3fTransform( get_pos[1],pos, dummy->mMat );
	pos[0] = dummy->mBBmin[0];
	pos[1] = dummy->mBBmax[1];
	pos[2] = dummy->mBBmin[2];
	Vector3fTransform( get_pos[2],pos, dummy->mMat );
	pos[0] = dummy->mBBmax[0];
	pos[1] = dummy->mBBmax[1];
	pos[2] = dummy->mBBmin[2];
	Vector3fTransform( get_pos[3],pos, dummy->mMat );

	pos[0] = dummy->mBBmax[0];
	pos[1] = dummy->mBBmin[1];
	pos[2] = dummy->mBBmax[2];
	Vector3fTransform( get_pos[4],pos, dummy->mMat );
	pos[0] = dummy->mBBmin[0];
	pos[1] = dummy->mBBmin[1];
	pos[2] = dummy->mBBmax[2];
	Vector3fTransform( get_pos[5],pos, dummy->mMat );
	pos[0] = dummy->mBBmin[0];
	pos[1] = dummy->mBBmin[1];
	pos[2] = dummy->mBBmin[2];
	Vector3fTransform( get_pos[6],pos, dummy->mMat );
	pos[0] = dummy->mBBmax[0];
	pos[1] = dummy->mBBmin[1];
	pos[2] = dummy->mBBmin[2];
	Vector3fTransform( get_pos[7],pos, dummy->mMat );

	R3DrawLine(get_pos[0],get_pos[1],0xffffffff);
	R3DrawLine(get_pos[1],get_pos[2],0xffffffff);
	R3DrawLine(get_pos[2],get_pos[3],0xffffffff);
	R3DrawLine(get_pos[3],get_pos[0],0xffffffff);
	R3DrawLine(get_pos[4],get_pos[5],0xffffffff);
	R3DrawLine(get_pos[5],get_pos[6],0xffffffff);
	R3DrawLine(get_pos[6],get_pos[7],0xffffffff);
	R3DrawLine(get_pos[7],get_pos[4],0xffffffff);
	R3DrawLine(get_pos[0],get_pos[4],0xffffffff);
	R3DrawLine(get_pos[1],get_pos[5],0xffffffff);
	R3DrawLine(get_pos[2],get_pos[6],0xffffffff);
	R3DrawLine(get_pos[3],get_pos[7],0xffffffff);
}

void CExtDummy::DrawAllDummyBBox()
{
	for(DWORD i=0; i<mNum; i++)
		DrawDummyBBox(i);
}
