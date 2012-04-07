#ifndef __C_VECTOR_H__
#define __C_VECTOR_H__

#include <math.h>

class C_Vector{
public:
	union{
	float mX;
	float mV[1];	//mV[0]-mV[2]까지 사용하려고...
	};
	float mY, mZ;
};

//-------벡터 포인터
#define Vector3cp	char *
#define Vector4fp	float *
#define Vector3fp	float *
#define Vector3lp	long *
#define Vector3sp	short int *
#define Vector2fp	Vector3fp
#define Vector1fp	Vector3fp
#define Vector2lp	Vector3lp
#define Vector2sp	Vector3sp



typedef float Vector2f[2];  /* st */
typedef float Vector3f[3];  /* xyz */
typedef float Vector4f[4];  // xyzw 또는 평면 ax+by+cz=d;

typedef long Vector2l[2];  /* uv */
typedef long Vector3l[3];  /* 1,2,3 */
typedef long Vector4l[4];  /* 1,2,3,4 */

typedef char Vector2c[2];  /* uv */
typedef char Vector3c[3];  /* uv */

typedef short int Vector4s[3];		// short형 고정 소수점에 쓸 예정...
typedef short int Vector3s[3];		// short형 고정 소수점에 쓸 예정...
typedef short int Vector2s[2];		// short형 고정 소수점에 쓸 예정...

typedef unsigned short int Vector4us[3];		// short형 고정 소수점에 쓸 예정...
typedef unsigned short int Vector3us[3];		// short형 고정 소수점에 쓸 예정...
typedef unsigned short int Vector2us[2];		// short형 고정 소수점에 쓸 예정...


/*
class C_Vector3f : {
public:
	float mV[3];
};*/
//union

#define Vector4fCopy(s,d) {d[0]=s[0];d[1]=s[1];d[2]=s[2];d[3]=s[3];}

#define Vector3lCopy(s,d) {d[0]=s[0];d[1]=s[1];d[2]=s[2];}

#define Vector3fDist(v) (sqrtf(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]))
#define Vector3fDist2(v) ((v[0]*v[0]+v[1]*v[1]+v[2]*v[2]))

#define Vector3fCopy(s,d) {d[0]=s[0];d[1]=s[1];d[2]=s[2];}
#define Vector3fAdd(a,b,c) {c[0]=a[0]+b[0];c[1]=a[1]+b[1];c[2]=a[2]+b[2];}
#define Vector3fMul(a,b,c) {c[0]=a[0]*b;c[1]=a[1]*b;c[2]=a[2]*b;}
#define Vector3fDiv(a,b,c) {c[0]=a[0]/b;c[1]=a[1]/b;c[2]=a[2]/b;}
#define Vector3fSub(a,b,c) {c[0]=a[0]-b[0];c[1]=a[1]-b[1];c[2]=a[2]-b[2];} 
#define Vector3fScale(a,b,c) {c[0]=b*a[0];c[1]=b*a[1];c[2]=b*a[2];}
#define Vector3fDot(a,b) (a[0]*b[0]+a[1]*b[1]+a[2]*b[2])
#define Vector3fAvg(a,b,c) {c[0]=(a[0]+b[0])*0.5f;c[1]=(a[1]+b[1])*0.5f;c[2]=(a[2]+b[2])*0.5f;}
#define Vector3fCmp(a,b) (a[0]==b[0]&&a[1]==b[1]&&a[2]==b[2])
#define Vector3fCross(a,b,r) {  r[0] = (a[1] * b[2]) - (a[2] * b[1]); r[1] = (a[2] * b[0]) - (a[0] * b[2]); r[2] = (a[0] * b[1]) - (a[1] * b[0]);}

#define Vector2fCopy(s,d) {d[0]=s[0];d[1]=s[1];}
#define Vector2fAvg(a,b,c) {c[0]=(a[0]+b[0])*0.5f;c[1]=(a[1]+b[1])*0.5f;}

#define Vector1fCopy(s,d) {d=s;}
#define Vector1fAvg(a,b,c) {c=(a+b)*0.5f;}

inline void Normalize(Vector3f v) { double a= sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
		v[0]/=(float)a;	v[1]/=(float)a;	v[2]/=(float)a;	}

#endif 
