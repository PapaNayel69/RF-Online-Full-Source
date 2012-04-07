////////////////////////////////////////////////////////////////////////////
//
// CPath Class Implementation
//
////////////////////////////////////////////////////////////////////////////

#include "BaseHeader.h"

#include "Global.h"
#include "CLand.h"
#include "CPath.h"

////////////////////////////////////////////////////////////////////////////
//
CPath::CPath()
{
	Init();
}

CPath::~CPath()
{
}

////////////////////////////////////////////////////////////////////////////
//

//--------------------------------------------------------------------------
// < Function >
//   o Path Stack 을 초기화 시킨다.
// < Parameter >
//   o None
// < Return Value >
//   o None
//--------------------------------------------------------------------------
void
CPath::Init( void )
{
	ZeroMemory( m_vecPathStack, ( sizeof( float ) * 3 * MAX_PATH_OF_STACK ) );

	m_nTotalPath = 0;
	m_nCurPath	 = 0;
}

//--------------------------------------------------------------------------
// < Function >
//   o Path Stack 을 설정한다. Path Finder 를 이용해 정보를 채운다.
// < Parameter >
//	 o pi_vecPos		- 현재위치
//	 o pi_vecTargetPos	- 타겟 위치
// < Return Value >
//   o None
//--------------------------------------------------------------------------
void
CPath::Create( Vector3f pi_vecPos, Vector3f pi_vecTargetPos )
{
	if( !_GetLevel()->IsLoadedBsp() )
		return;

	Init();

	//---디버그용...
	//Vector3f temp,t_temp;
	//Vector3fCopy(pi_vecTargetPos,t_temp);

	SetPickPoint( pi_vecTargetPos );
	m_nPathResult = _GetLevel()->GetPath( pi_vecPos, pi_vecTargetPos, m_vecPathStack, &m_nTotalPath, MAX_PATH_OF_STACK);
	m_nCurPath = 0;

	//디버그용...
	//Vector3fSub(pi_vecPos,t_temp,temp);
	//float dist = Vector3fDist(temp);
	//if( dist < 199 && t_temp[0] != pi_vecTargetPos[0] 
	//	 && t_temp[1] != pi_vecTargetPos[1]
	//	  && t_temp[2] != pi_vecTargetPos[2])
	//{
	//	Vector3fCopy(t_temp,pi_vecTargetPos);
	//	_GetLevel()->GetPath( pi_vecPos, pi_vecTargetPos, m_vecPathStack, &m_nTotalPath );
	//	Vector3fCopy(t_temp,pi_vecTargetPos);
	//	_GetLevel()->GetPath( pi_vecPos, pi_vecTargetPos, m_vecPathStack, &m_nTotalPath );
	//}
}

