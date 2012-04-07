#ifndef __FOREFFECT_H__
#define __FOREFFECT_H__


#define _RENDER_DIST (340-40)
#define _MAX_HEAR_DIST (_RENDER_DIST/4.0f)

float GetVolume(Vector3f pos);
float GetDistFromCamera(Vector3f pos);

void CreateEffect();	//이펙트 초기화.	//새로 맵을 오픈할경우.지우고 다시 Create한다.
void DestroyEffect(); //이펙트 릴리즈		//

void FrameMoveEffect();	//매루프마다...모든 이펙트 관리.
void RenderEffect();	//매루프마다... 모든 이펙트 뿌려주기.
void DrawCheatDisplay();


void MonsterSpecialEffect(void *monster);



#endif