#include "CSwordTrace.h"


//#include "..//CNetworkMgr.h"
//#include "..//CCharacterMgr.h"
//#include "..//BaseHeader.h"
//#include "..//Global.h"
//#include "..//Cland.h"

#include "r3d3dtex.h"
#include "r3math.h"
#include "entity.h"
#include "level.h"
#include "r3util.h"
#include "r3render.h"
#include "Commonutil.h"

#include "EffectEntityList.h"

#define _TRACE_FLAG_STOP	0
#define _TRACE_FLAG_RUN		1

CSwordTrace::CSwordTrace()
{
	memset(this,0,sizeof(this));
	mColor=(65<<16)|(132<<8)|183;
//	mColor=(192<<16)|(187<<8)|56;
	mCnt=0;	
	mQueue=0;
	mIsTrace=0;
	mEntity=NULL;
	mIsRealTrace=0;
}

static void QuaternionFromRotation( Vector4f quat,Vector3f v, float fTheta )
{
	FLOAT sinValue = (FLOAT)sin(fTheta/2);
    quat[0] = sinValue * v[0];
    quat[1] = sinValue * v[1];
    quat[2] = sinValue * v[2];
    quat[3] = (FLOAT)cos(fTheta/2);
}
//기준벡터는 {0,1,0}
static void GetQuaternionFromVector(Vector4f quat,Vector3f vec)
{
	//축과 각도를 계산해볼까나...
	Vector3f b={0,1,0},cross,temp;
	float angle;

	Vector3fCopy(vec,temp);
	Normalize(temp);
	Vector3fCross(temp,b,cross);	//이것이 축.
	Normalize(cross);
	angle = (float)acos(Vector3fDot(temp,b));	//각도 구하고.

	QuaternionFromRotation(quat,cross,angle);
}
static DWORD GetTraceSplit()	//실제 추적할 버퍼갯수.
{
	DWORD aa = (DWORD)((0.40f/R3GetLoopTime()) + 1);
	if( aa == 1)
		aa=2;
	return aa;
}
void CSwordTrace::SetTraceState(BOOL trace)	//칼베는 동작 사용할때 TRUE 끝날때 FALSE..
{
	if( mIsTrace == trace )
		return;
	mIsTrace=trace;
	if( !mIsTrace )
	{
		mIsRealTrace=GetTraceSplit();	//실제 처리할 버퍼를 천천히 줄이기위해...
	}
	else
		mIsRealTrace=_MAX_REAL_TRACE_CNT;
}
BOOL CSwordTrace::IsTrace()	//현재 궤적을 그려야하는가? 
{
	if( mIsTrace && mIsRealTrace )
		return TRUE;
	return FALSE;
}

BOOL CSwordTrace::IsDraw()	//현재 궤적을 그리는 중인지...mcnt  질문
{
	if( mCnt > 1 )	//2개는 되야 폴리곤을 그리지.
		return TRUE;
	return FALSE;
}

static float CalcBi_n(int n, int i, double u)	//베지어 함수
{
	int    j;
	double Bin, nCi, Pn, Pi, Pni;

	if(n == 0 || n == 1)
		Pn  = 1;
	else
	{
		Pn  = (double)n;

		for(j = 1; j < n; j++)
			Pn = Pn * (double)(n - j);	
	}
	
	if(i == 0 || i == 1)
		Pi = 1;
	else
	{
		Pi = (double)i;

		for(j = 1; j < i; j++)
			Pi = Pi * (double)(i - j);
	}

	if((n-i) == 0 || (n-i) == 1)
		Pni = 1;
	else
	{
		Pni = (double)(n - i);

		for(j = 1; j < n-i; j++)
			Pni = Pni * (double)((n-i) - j);
	}

	nCi = Pn / (Pi * Pni);
	Bin = nCi * pow(u, i) * pow((1-u), n-i);

	return (float)Bin;
}

void calcNaturalCubic(Vector4f *get_abcd,int n, float *vptr)
{
	float gamma[1000];
	float delta[1000];
	float D[1000];
	int i;
	typedef struct{
		float r;
		float no_use[2];
	}_X;
	_X *x=(_X *)vptr;

	gamma[0] = 1.0f/2.0f;
	for(i=1; i<n; i++)
	{
		gamma[i] = 1/(4-gamma[i-1]);
	}
	gamma[n] = 1/(2-gamma[n-1]);
	
	delta[0] = 3*(x[1].r-x[0].r)*gamma[0];
	for(i=1; i<n; i++)
	{
		delta[i] = (3*(x[i+1].r-x[i-1].r)-delta[i-1])*gamma[i];
	}
	delta[n] = (3*(x[n].r-x[n-1].r)-delta[n-1])*gamma[n];
	
	D[n] = delta[n];
	for(i=n-1; i>=0; i--)
	{
		D[i] = delta[i] - gamma[i]*D[i+1];
	}

	for(i=0; i<n; i++)
	{
		get_abcd[i][0] = (float)x[i].r;
		get_abcd[i][1] = (float)D[i];
		get_abcd[i][2] = (float)3*(x[i+1].r - x[i].r) - 2*D[i] - D[i+1];
		get_abcd[i][3] = (2.0f*(x[i].r - x[i+1].r) + D[i] + D[i+1]);
	}
}
inline float Eval(Vector4f v,float u)
{
	return ((((v[3]*u) + v[2])*u + v[1])*u + v[0]);
}


//CEntity *stEntity;

void CSwordTrace::InsertSwordTraceQueue(Vector3f s_pos,Vector3f e_pos)
{
	int i;

	if( mIsRealTrace == 0 )	//추적이 끝났다.
		return;

	if( mCnt == 0 ) //최초다...  최초는 첫좌표를 넣고 시작한다.
	{
		Vector3fCopy(s_pos,mStartPos[0]);
		Vector3fCopy(e_pos,mEndPos[0]);
		mQueue=1;
		mCnt=1;
		mRealCnt=1;
		Vector3fCopy(s_pos,mRealStartPos[0]);
		Vector3fCopy(e_pos,mRealEndPos[0]);
		return;
	}
/*
	if( !stEntity )
	{
		stEntity = new CEntity;
		stEntity->LoadEntity(".\\Entity\\effect\\Weapon\\SwordTrace\\SwordTrace.r3e",_LOAD_POLY_IN_MEMORY);
		stEntity->RestoreTexMem();
	}
	mEntity = stEntity;//	GetEntityFromEffectEntityList(0);asdas
*/
	if( mRealCnt >=_MAX_REAL_TRACE_CNT-1 )
	{
		memcpy(mRealStartPos,&mRealStartPos[1],sizeof(Vector3f)*(mRealCnt-1));
		memcpy(mRealEndPos,&mRealEndPos[1],sizeof(Vector3f)*(mRealCnt-1));
		mRealCnt=_MAX_REAL_TRACE_CNT-1;
	}
	Vector3fCopy(s_pos,mRealStartPos[mRealCnt]);
	Vector3fCopy(e_pos,mRealEndPos[mRealCnt]);
	mRealCnt++;

//	Sleep(60);
	int calc_cnt = GetTraceSplit();	//궤적 분할은 타임으로 한다.  실제 트레이스 좌표갯수를 결정.

	if( calc_cnt >= mRealCnt )
		calc_cnt = mRealCnt;

	if( calc_cnt >= mIsRealTrace)
		calc_cnt=mIsRealTrace;
	if( mIsTrace == 0 && mIsRealTrace > 0 )
	{
		mIsRealTrace--;
	}
	if( calc_cnt == 1 )
		return;

	Vector4f x_abcd[2][_MAX_REAL_TRACE_CNT];
	Vector4f y_abcd[2][_MAX_REAL_TRACE_CNT];
	Vector4f z_abcd[2][_MAX_REAL_TRACE_CNT];
	int step = 5;//각각의 사이는 3개씩.

	calcNaturalCubic(x_abcd[0],calc_cnt-1, &mRealStartPos[mRealCnt-calc_cnt][0]);
	calcNaturalCubic(x_abcd[1],calc_cnt-1, &mRealEndPos[mRealCnt-calc_cnt][0]);
	calcNaturalCubic(y_abcd[0],calc_cnt-1, &mRealStartPos[mRealCnt-calc_cnt][1]);
	calcNaturalCubic(y_abcd[1],calc_cnt-1, &mRealEndPos[mRealCnt-calc_cnt][1]);
	calcNaturalCubic(z_abcd[0],calc_cnt-1, &mRealStartPos[mRealCnt-calc_cnt][2]);
	calcNaturalCubic(z_abcd[1],calc_cnt-1, &mRealEndPos[mRealCnt-calc_cnt][2]);
	mCnt=(calc_cnt-1)*step;

	mStartPos[mCnt][0] = Eval(x_abcd[0][0],0);
	mEndPos[mCnt][0] = Eval(x_abcd[1][0],0);
	mStartPos[mCnt][1] =Eval(y_abcd[0][0],0);
	mEndPos[mCnt][1] = Eval(y_abcd[1][0],0);
	mStartPos[mCnt][2] =Eval(z_abcd[0][0],0);
	mEndPos[mCnt][2] = Eval(z_abcd[1][0],0);
	mCnt--;
	for(int j = 0; j < calc_cnt-1; j++)
	{
		for(i = 1; i <= step; i++)	//각각의 사이는 스탭값으로.
		{
			float u = (i) / (float)step;
			mStartPos[mCnt][0]	= Eval(x_abcd[0][j],u);
			mEndPos[mCnt][0]	= Eval(x_abcd[1][j],u);
			mStartPos[mCnt][1]	= Eval(y_abcd[0][j],u);
			mEndPos[mCnt][1]	= Eval(y_abcd[1][j],u);
			mStartPos[mCnt][2]	= Eval(z_abcd[0][j],u);
			mEndPos[mCnt][2]	= Eval(z_abcd[1][j],u);
			mCnt--;
		}
	}
	mCnt=(calc_cnt-1)*step;

	/*
	mStartPos[mCnt][0] = mRealStartPos[mRealCnt-1][0];
	mEndPos[mCnt][0] = mRealEndPos[mRealCnt-1][0];
	mStartPos[mCnt][1] = mRealStartPos[mRealCnt-1][1];
	mEndPos[mCnt][1] = mRealEndPos[mRealCnt-1][1];
	mStartPos[mCnt][2] = mRealStartPos[mRealCnt-1][2];
	mEndPos[mCnt][2] = mRealEndPos[mRealCnt-1][2];
	mCnt++;*/
	

/*
	mCnt += _MAX_TRACE_CNT/_MAX_REAL_TRACE_CNT;
	if( mCnt > _MAX_TRACE_CNT-1 )
		mCnt = _MAX_TRACE_CNT-1;

	for(int u = 0; u <= mCnt; u++)
	{
		float sx,sy,sz;
		float ex,ey,ez;
		float q_alpha = (1+u)*(1/(float)mCnt);
//		int q_cnt = (mQueue+mCnt-u-1+_MAX_TRACE_CNT)%_MAX_TRACE_CNT;

		sx=0,sy=0,sz=0;
		ex=0,ey=0,ez=0;
		for(i = 0; i < calc_cnt; i++)
		{
			float temp = CalcBi_n(calc_cnt-1, i, (u * (1.0/(double)mCnt)));

			sx = sx + temp * mRealStartPos[mRealCnt-calc_cnt+i][0];
			sy = sy + temp * mRealStartPos[mRealCnt-calc_cnt+i][1];
			sz = sz + temp * mRealStartPos[mRealCnt-calc_cnt+i][2];
			ex = ex + temp * mRealEndPos[mRealCnt-calc_cnt+i][0];
			ey = ey + temp * mRealEndPos[mRealCnt-calc_cnt+i][1];
			ez = ez + temp * mRealEndPos[mRealCnt-calc_cnt+i][2];
		}
		mStartPos[u][0] = sx;
		mStartPos[u][1] = sy;
		mStartPos[u][2] = sz;
		mEndPos[u][0] = ex;
		mEndPos[u][1] = ey;
		mEndPos[u][2] = ez;
	}
*/
	mQueue += _MAX_REAL_TRACE_CNT;
	mQueue %= _MAX_TRACE_CNT;
}
/*
#include "..\\cland.h"
extern CLand *g_pLand;				// 현재 랜드에 관한 포인터
*/
static void DrawBox(Vector3f pos,DWORD color)
{
/*	
	CLevel *level = g_pLand->GetLevel();
	float box[3]={0.5f,0.5f,0.5f};
//	float box[3]={1,1,1};
	level->DrawTestBox(box,pos,color);		//사람테스트할 박스유틸...
*/	
}

//테스트 함수..
#include "r3input.h"
extern void GetRenderState();
extern void GetTextureStage();
extern void R3DrawLine(Vector3f a,Vector3f b,DWORD color);	//직접 라인그리기..


void CSwordTrace::UpdateSwordTrace()
{
	int i,v;
	_D3DR3VERTEX_TEX1 v_buf[1000];
	DWORD alpha;
	float f_alpha;

	if( mCnt <= 1 || mEntity == NULL || !mEntity->IsLoadedEntity() )	//2개는 되야 폴리곤을 그리지.
		return;

	//궤적 
	Vector3f temp;
	float dist;
	Vector3fSub(mStartPos[0],mStartPos[1],temp);
	dist = Vector3fDot(temp,temp);
	if( dist != 0 )
	{
		dist=sqrtf(dist);
		if( dist > 50)
		{
			mCnt=0;
			return;
		}
	}
	Vector3fSub(mEndPos[0],mEndPos[1],temp);
	dist = Vector3fDot(temp,temp);
	if( dist != 0 )
	{
		dist=sqrtf(dist);
		if( dist > 50)
		{
			mCnt=0;
			return;
		}
	}

	DWORD color = 0x00ffffff;

	for(i=0; i<mCnt; i++)
	{
//		v = (-i+mQueue-1 + 2*_MAX_TRACE_CNT)%_MAX_TRACE_CNT;
		v = i;
//		DrawBox(mStartPos[v],0xffffffff);
//		DrawBox(mEndPos[v],0xff00ff00);
//		R3DrawLine(mStartPos[v],mEndPos[v],0xffffffff);	//직접 라인그리기..
		f_alpha = (i/(float)mCnt);
		alpha = ((DWORD)((1-f_alpha)*255))<<24;
		v_buf[i*2].x = mStartPos[v][0];
		v_buf[i*2].y = mStartPos[v][1];
		v_buf[i*2].z = mStartPos[v][2];
		v_buf[i*2].color = color|alpha;
//		v_buf[i*2].u = 1-f_alpha;
		v_buf[i*2].u = f_alpha;
		v_buf[i*2].v = 0;
		v_buf[i*2+1].x = mEndPos[v][0];
		v_buf[i*2+1].y = mEndPos[v][1];
		v_buf[i*2+1].z = mEndPos[v][2];
		v_buf[i*2+1].color = color|alpha;
//		v_buf[i*2+1].u = 1-f_alpha;
		v_buf[i*2+1].u = f_alpha;
		v_buf[i*2+1].v = 1;
	}
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	GetD3dDevice()->SetTransform( D3DTS_WORLD, &matWorld );	
/*
//궤적을 라인으로 보기...
	for(i=1; i<mCnt-2; i++)
	{
		v = (-i+mQueue-1 + 2*_MAX_TRACE_CNT)%_MAX_TRACE_CNT;
		int nv = (-i+1+mQueue-1 + 2*_MAX_TRACE_CNT)%_MAX_TRACE_CNT;
		R3DrawLine(mStartPos[v],mStartPos[nv],0xffffffff);	//직접 라인그리기..
		R3DrawLine(mEndPos[v],mEndPos[nv],0xffffffff);	//직접 라인그리기..
	}
*/	
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();

    pd3dDevice->SetRenderState( D3DRS_LIGHTING,     FALSE );
//	pd3dDevice->LightEnable( 0,FALSE);
//	pd3dDevice->LightEnable( 1,FALSE);
	GetD3dDevice()->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	GetD3dDevice()->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	GetD3dDevice()->SetRenderState(D3DRS_ZWRITEENABLE ,FALSE);
//	GetD3dDevice()->LightEnable( 0,FALSE);
	if( mCnt*2-2 > 0 )
	{
		DrawPrimitiveUPTex1(R3GetSurface(mEntity->mMat->m_Layer[0].m_iSurface)
			,mCnt*2-2,_BLEND_BRIGHT,v_buf,D3DPT_TRIANGLESTRIP);	//미리 버텍스버퍼에 저장해야된다.
	}

//	GetD3dDevice()->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	GetD3dDevice()->SetRenderState(D3DRS_ZWRITEENABLE ,TRUE);
	GetD3dDevice()->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );
	GetD3dDevice()->SetRenderState(D3DRS_CULLMODE,D3DCULL_CW);
    pd3dDevice->SetRenderState( D3DRS_LIGHTING,     TRUE );

	if( mIsRealTrace == 0 )
		mCnt = 0;
}
