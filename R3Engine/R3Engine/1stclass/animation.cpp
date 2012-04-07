#include "R3math.h"
#include "CommonUtil.h"
#include "Animation.h"

//--------------------------------- 짜증나는 애니메이션이다----------------------------------
static int FR_root,FR_next;
//max_frame은 1이상이다.
static float GetFrameAlpha(int max_frame,float *frame_buf,int st_num,float now_frame,int frames,WORD flag)
{
	int i;
	FR_root=-1;
	FR_next=-1;

	for(i=0; i<max_frame-1; i++)
	{	
		if( frame_buf[i*st_num] <= now_frame && frame_buf[(i+1)*st_num] > now_frame)
		{
			FR_root=i;
			FR_next=i+1;
			break;
		}
	}
	if( FR_root == -1 )	//맨 마지막 프래임이후 값이다.
	{	//언제나 루프 
//		if(flag & _LOOP_ANIMATION )	//루프되는 경우
		if(0)
		{//맨 마지막 프래임과 첫프래임과의 사이값이다.
			FR_root=max_frame-1;
			FR_next=0;
			return  (now_frame-frame_buf[FR_root*st_num])
				/(frames+frame_buf[FR_next*st_num]-frame_buf[FR_root*st_num] );
		}
		else
		{
			FR_root=max_frame-1;
			FR_next=max_frame-1;
		}
		return 0;
  	}
	return ( (now_frame-frame_buf[FR_root*st_num])
		/(frame_buf[FR_next*st_num]-frame_buf[FR_root*st_num]) );
}
void GetAniMatrix(float ani_mat[4][4],_ANI_OBJECT *obj,float now_frame)
{
	FLOAT alpha,Qx,Qy,Qz,Qw;

	MatrixIdentity(ani_mat);
//	if( 1 )
	if( obj->Pos_cnt == 0 && obj->Rot_cnt == 0 && obj->Scale_cnt == 0)
	{	//애니키를 하나도 안가지고 있으면....
		MatrixFromQuaternion(ani_mat,obj->quat[0],obj->quat[1],obj->quat[2],obj->quat[3]);
		MatrixMultiply( ani_mat, ani_mat,obj->s_matrix);
		ani_mat[3][0]=obj->pos[0];
		ani_mat[3][1]=obj->pos[1];
		ani_mat[3][2]=obj->pos[2];
	}
	else
	{

//-------------------- 회전 트랙 
		if(obj->Rot_cnt==0)	// 회전키를 하나도 안가지고 있을때
		{
			MatrixFromQuaternion(ani_mat,obj->quat[0],obj->quat[1],obj->quat[2],obj->quat[3]);
		}
		else
		{
			int next_num=0;
			alpha =GetFrameAlpha(obj->Rot_cnt,(float*)obj->Rot,sizeof(_ROT_TRACK)/4,now_frame,obj->frames,obj->flag);
			QuaternionSlerp( Qx,Qy,Qz,Qw,obj->Rot[FR_root].quat[0]
						,obj->Rot[FR_root].quat[1],obj->Rot[FR_root].quat[2]
						,obj->Rot[FR_root].quat[3],obj->Rot[FR_next].quat[0]
						,obj->Rot[FR_next].quat[1],obj->Rot[FR_next].quat[2]
						,obj->Rot[FR_next].quat[3],alpha);
			MatrixFromQuaternion(ani_mat,Qx,Qy,Qz,Qw);

		}

//-------------------- 스케일 트랙 계산부터다.
		if(obj->Scale_cnt==0)
			MatrixMultiply( ani_mat, ani_mat,obj->s_matrix);	//-----스케일 로컬 매트릭스와 곱한다.
		else
		{
			float temp[4][4];
			float scale_matrix[4][4];
			float inv_sq_matrix[4][4],sq_matrix[4][4];

			alpha =GetFrameAlpha(obj->Scale_cnt,(float*)obj->Scale,sizeof(_SCALE_TRACK)/4,now_frame,obj->frames,obj->flag);

//			alpha=0;
//			FR_root=0;
//			FR_next=1;
			//스케일 매트릭스 완성.
			MatrixIdentity( scale_matrix );
			scale_matrix[0][0] = (obj->Scale[FR_next].scale[0]-obj->Scale[FR_root].scale[0])*alpha
				+obj->Scale[FR_root].scale[0];
			scale_matrix[1][1] = (obj->Scale[FR_next].scale[1]-obj->Scale[FR_root].scale[1])*alpha
				+obj->Scale[FR_root].scale[1];
			scale_matrix[2][2] = (obj->Scale[FR_next].scale[2]-obj->Scale[FR_root].scale[2])*alpha
				+obj->Scale[FR_root].scale[2];

			// 스케일축 매트릭스 
			QuaternionSlerp( Qx,Qy,Qz,Qw,obj->Scale[FR_root].scale_axis[0]
						,obj->Scale[FR_root].scale_axis[1],obj->Scale[FR_root].scale_axis[2]
						,obj->Scale[FR_root].scale_axis[3],obj->Scale[FR_next].scale_axis[0]
						,obj->Scale[FR_next].scale_axis[1],obj->Scale[FR_next].scale_axis[2]
						,obj->Scale[FR_next].scale_axis[3],alpha);
			MatrixFromQuaternion(sq_matrix,Qx,Qy,Qz,Qw);
			MatrixInvert(inv_sq_matrix, sq_matrix);
			
			MatrixMultiply( temp, scale_matrix, inv_sq_matrix );
			MatrixMultiply( temp,sq_matrix,temp );
			MatrixMultiply( ani_mat, ani_mat,temp);	//-----스케일 최종 매트릭스와 곱한다.
		}
//-------------------- 이동 트랙 이다.
		if(obj->Pos_cnt==0)
		{
			ani_mat[3][0]=obj->pos[0];
			ani_mat[3][1]=obj->pos[1];
			ani_mat[3][2]=obj->pos[2];
		}
		else
		{
			alpha =GetFrameAlpha(obj->Pos_cnt,(float*)obj->Pos,sizeof(_POS_TRACK)/4,now_frame,obj->frames,obj->flag);
			ani_mat[3][0]=(obj->Pos[FR_next].pos[0]-obj->Pos[FR_root].pos[0])*alpha
				+obj->Pos[FR_root].pos[0];
			ani_mat[3][1]=(obj->Pos[FR_next].pos[1]-obj->Pos[FR_root].pos[1])*alpha
				+obj->Pos[FR_root].pos[1];
			ani_mat[3][2]=(obj->Pos[FR_next].pos[2]-obj->Pos[FR_root].pos[2])*alpha
				+obj->Pos[FR_root].pos[2];
		}
	}
}

static void GetSubObjectMatrix(float w_matrix[4][4],WORD id,_ANI_OBJECT *ani_object,float now_frame)
{
	float p_matrix[4][4];	//부모
	float m_matrix[4][4];	//내매트릭스

	if( id==0 )
	{
		MatrixIdentity( w_matrix );		
	}
	else
	{/*
		ani_object[id-1].now_frame +=R3GetLoopTime()*30.0f;	//초당 30프래임
		if( ani_object[id-1].frames && ani_object[id-1].now_frame > ani_object[id-1].frames )
		{
			while(ani_object[id-1].now_frame > ani_object[id-1].frames)
				ani_object[id-1].now_frame -= ani_object[id-1].frames;
		}*/
//		if( ani_object[id-1].flag &)

		GetSubObjectMatrix(p_matrix,ani_object[id-1].parent,ani_object,now_frame);
//		GetAniMatrix(m_matrix,&ani_object[id-1],ani_object[id-1].now_frame);
		GetAniMatrix(m_matrix,&ani_object[id-1],now_frame);
		MatrixMultiply(w_matrix,p_matrix,m_matrix);
	}
}
float GetFloatMod(float su,float mod)	//우선 mod가 0이 아니어야한다.
{
	if( mod ==0 )	//이런경우 생기면 안되는데..
		return 0;
	if( su < 0 )	//이런경우 생기면 안되는데..
		su=-su;
	while( 1 )
	{
		if( su < 32768 )
			break;
		su-=32768;
	}
	LONG a = (LONG)(su*32768.0f);
	LONG b = (LONG)(mod*32768.0f);
	return ((a%b)/32768.0f);
}

void GetObjectMatrix(float w_matrix[4][4],WORD id,_ANI_OBJECT *ani_object,float time)
{
	float p_matrix[4][4];	//부모
	float m_matrix[4][4];	//내매트릭스
	float now_frame=0;
	float parent_now_frame=0;

	if( id==0 )
	{
		MatrixIdentity( w_matrix );		
	}
	else
	{
		if( ani_object[id-1].AniFrameCache )
		{
			MatrixCopy(w_matrix,ani_object[id-1].f_matrix);
		}
		else
		{
			ani_object[id-1].AniFrameCache=1;

			if( ani_object[id-1].parent )
			{
				if( ani_object[ani_object[id-1].parent-1].frames != 0 )
					parent_now_frame = GetFloatMod(time,(float)ani_object[ani_object[id-1].parent-1].frames);
			}

			if( ani_object[id-1].frames == 0 )
				now_frame=0;
			else
				now_frame=GetFloatMod(time,(float)ani_object[id-1].frames);

			GetSubObjectMatrix(p_matrix,ani_object[id-1].parent,ani_object,parent_now_frame);
			GetAniMatrix(m_matrix,&ani_object[id-1],now_frame);
			MatrixMultiply(w_matrix,p_matrix,m_matrix);
/*
			ani_object[id-1].now_frame +=R3GetLoopTime()*30.0f;	//초당 30프래임
			if( ani_object[id-1].frames )
			{
				if( ani_object[id-1].now_frame > ani_object[id-1].frames )
				{
					while(ani_object[id-1].now_frame > ani_object[id-1].frames)
						ani_object[id-1].now_frame -= ani_object[id-1].frames;
				}
			}
			else
				ani_object[id-1].now_frame=0;		
*/
			GetMatrixFrom3DSMAXMatrix(w_matrix);
			MatrixCopy(ani_object[id-1].f_matrix,w_matrix);
//			D3DXMATRIX t_matrix;
//			GetD3dDevice()->GetTransform( D3DTS_VIEW, &t_matrix );
		}
	}
}

//리드 오브젝트를 오브젝트로 바꿔주자.
void ConvAniObject(int num,BYTE *track,_READ_ANI_OBJECT *r_object,_ANI_OBJECT *object)		//트랙인덱스를 포인터로 바꿔주자.
{
	int i;
	float temp[4][4];
	float scale_matrix[4][4];
	float sq_matrix[4][4];
	float inv_sq_matrix[4][4];

	for(i=0 ;i<num; i++)
	{
		//스케일 축 매트릭스도 구하고..읽은곳에서 저장할곳에 넣자.
		object[i].flag = r_object[i].flag;
		object[i].frames = r_object[i].frames;
		object[i].parent = r_object[i].parent;
		object[i].Pos_cnt = r_object[i].Pos_cnt;
		object[i].Rot_cnt = r_object[i].Rot_cnt;
		object[i].Scale_cnt = r_object[i].Scale_cnt;
		object[i].now_frame=0;

		Vector4fCopy(r_object[i].scale_quat,object[i].scale_quat);
		Vector3fCopy(r_object[i].scale,object[i].scale);
		Vector3fCopy(r_object[i].pos,object[i].pos);
		Vector4fCopy(r_object[i].quat,object[i].quat);
		object[i].Scale = (_SCALE_TRACK *)&(track[(int)r_object[i].scale_offset]);
		object[i].Pos = (_POS_TRACK *)&(track[(int)r_object[i].pos_offset]);
		object[i].Rot = (_ROT_TRACK *)&(track[(int)r_object[i].rot_offset]);

		MatrixIdentity( scale_matrix );
		scale_matrix[0][0] = r_object[i].scale[0];
		scale_matrix[1][1] = r_object[i].scale[1];
		scale_matrix[2][2] = r_object[i].scale[2];

		MatrixFromQuaternion(sq_matrix,r_object[i].scale_quat[0],r_object[i].scale_quat[1]
			,r_object[i].scale_quat[2],r_object[i].scale_quat[3]);
		MatrixInvert(inv_sq_matrix, sq_matrix);
		MatrixMultiply( temp, scale_matrix, inv_sq_matrix );
		MatrixMultiply( object[i].s_matrix,sq_matrix,temp );
		
//		MatrixCopy(object[i].s_matrix,r_object[i].s_matrix);
	}
}

