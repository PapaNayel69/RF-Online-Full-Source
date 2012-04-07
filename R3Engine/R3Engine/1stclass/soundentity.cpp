#include "SoundEntity.h"
#include "R3Util.h"
#include "R3Math.h"
#include "core.h"
#include "collision.h"


float _SOUND_ENTITIES_LIST::GetVolume(Vector3f pos)
{
	Vector3f cam_pos,temp;
	float density,dist;

	::GetCameraPos(&cam_pos);
	Vector3fSub(pos,cam_pos,temp);
	dist = Vector3fDist(temp);	//사운드 포인트와의 거리.

	dist = max(dist,Scale);
	dist = min(dist,Attn);
	density = (Attn-dist)/(Attn-Scale);

//	return 1;	//테스트..
	return density;
}
float _SOUND_ENTITIES_LIST::GetPan(Vector3f src)	//맵상의 위치하나로 효과음에 쓰일 팬값을 구한다.  -1은 왼쪽에치우침.1은 오른쪽에 치우침.
{
	Vector3f tar_pos;
	Vector3f view_vec;
	_R3ENGINE_STATE *state = GetR3State();

	view_vec[0] = state->mMatView.m[2][2];	//쳐다보고있는 각도와 
	view_vec[1] = state->mMatView.m[2][1];
	view_vec[2] = state->mMatView.m[2][0];
	Normalize(view_vec);

	tar_pos[0] = state->mInvMatView.m[3][0];
	tar_pos[1] = state->mInvMatView.m[3][1];
	tar_pos[2] = state->mInvMatView.m[3][2];	//실제 로컬 위치와의 벡터로....
	Vector3fSub(tar_pos,src,tar_pos);
	Normalize(tar_pos);

	return -Vector3fDot(tar_pos,view_vec);	//Dot가 바로 팬이 된다.
}
float _SOUND_ENTITIES_LIST::GetBoxIntensity(Vector3f pos)		//박스 영역에서의 볼륨을 알려준다. 0-1사이
{
	int i;
	float intensity=1;
	Vector3f get;
	Vector3fTransform(get,pos,mInvMat);
	
	if( BoxAttn == 1 )
	{
		Vector3f bb_min={-1,-1,-1};
		Vector3f bb_max={1,1,1};
		if( IsCollisionBBoxPoint(bb_min,bb_max,get) )
			return 1.0f;
		else
			return 0.0f;
	}


	for(i=0; i<3; i++)
	{
		if( get[i] > 1 )
			return 0;
		if( get[i] > BoxAttn )
			intensity = min(intensity,((1-get[i])/(1-BoxAttn)));
		if( get[i] < -1 )
			return 0;
		if( get[i] < -BoxAttn )
			intensity = min(intensity,((1+get[i])/(1-BoxAttn)));
	}
	return intensity;
}
