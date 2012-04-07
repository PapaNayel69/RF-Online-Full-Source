#ifndef __SHADOW_LIST__
#define	__SHADOW_LIST__


#include <windows.h>
#define		CMAX_SHADOW 20
class CShadow
{
	float	m_Vec[CMAX_SHADOW][3];
	DWORD	m_ShadowNum;
public:
	CShadow()
	{
		m_ShadowNum  = 0;
	}
	DWORD SetShadow(float *vec)
	{
		if (m_ShadowNum>=CMAX_SHADOW) return 0;
		m_Vec[m_ShadowNum][0] = vec[0];
		m_Vec[m_ShadowNum][1] = vec[1];
		m_Vec[m_ShadowNum][2] = vec[2];
		m_ShadowNum++;
		return m_ShadowNum;
	}
	void ResetShadow()
	{
		m_ShadowNum = 0;
	}
	DWORD GetShadowNum() {	return m_ShadowNum; }
	float *GetShadowPos( DWORD n ) { return m_Vec[n]; }
	
};
extern CShadow g_ShadowList;
#endif