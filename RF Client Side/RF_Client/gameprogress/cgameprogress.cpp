////////////////////////////////////////////////////////////////////////////
//
// CGameProgress Class Implementation
//
////////////////////////////////////////////////////////////////////////////

#include <windows.h>

#include "CGameProgress.h"
//MAYUN
#include "../AddCodeByYun/ParticleList.h"
void Protect();
//MAYUN

////////////////////////////////////////////////////////////////////////////
//

CGameProgress::CGameProgress()
{
	Init();
}

CGameProgress::~CGameProgress()
{
	Destroy();
}

void
CGameProgress::Init( void )
{
	m_bIsCreated	= FALSE;
	m_bExit			= FALSE;
	m_bRequestExit	= FALSE;
	m_bIsLoadedData	= FALSE;
}

BOOL
CGameProgress::Create( void )
{
	//if( !LoadData() )
	//	return FALSE;

	return TRUE;
}

BOOL
CGameProgress::Destroy( void )
{
//	if( !UnloadData() )
//		return FALSE;

	return TRUE;
}

// TRUE - 메인 루프가 모두 끝났음
// FALSE - 메인 루프가 아직 끝나지 않았음
BOOL
CGameProgress::MainLoop( BOOL pi_bIsActive )
{
	// 사용자의 입력 처리
	InputProcess();

	// 현재의 게임 프로그레스 객체 종료 체크
	if( m_bExit )
		return TRUE;

	// 애니메이션
	FrameMove();

	// 렌더링
	if( pi_bIsActive )
		Render();

	//MAYUN 
	g_Particle.ResetParticleCount();
	Protect();
	return FALSE;
}
