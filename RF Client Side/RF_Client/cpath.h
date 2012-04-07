////////////////////////////////////////////////////////////////////////////
//
// CPath Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __PATHFINDER_H__
#define __PATHFINDER_H__

////////////////////////////////////////////////////////////////////////////
//
#define MAX_PATH_OF_STACK	( 8 )

////////////////////////////////////////////////////////////////////////////
//

/*--------------------------------------------------------------------------
[ CPath ]
- 캐릭터나 카메라등의 이동 경로를 저장
- R3 엔진의 Path Finder를 사용
--------------------------------------------------------------------------*/
class CPath
{
// < Data Member > ---------------------------------------------------------
private :
	Vector3f	m_vecPathStack[MAX_PATH_OF_STACK];		// path 를 저장할 공간

	DWORD		m_nTotalPath;							// path 의 총 갯수
	int			m_nCurPath;								// 현재 path 의 번호

	// JUI추가 코드.(멀리 찍을때 멀리 가게하기위한거..)
	DWORD		m_nPathResult;							// 현재 얻어진 패스의 상태. 너무멀리찍은경우 _PATH_TOO_LONG_CLIP 값을 갖고있다.
	Vector3f	m_vecPickPoint;							// 최근에 찍었던 위치..

// < Member Function > -----------------------------------------------------
public :
	CPath();
	~CPath();

	// ---------------------------------------------------------------------
	void	Init( void );
	void	Create( Vector3f pi_vecPos, Vector3f pi_vecTargetPos );

	// ---------------------------------------------------------------------

	inline	BOOL	GetNextPath( Vector3f po_vecPath );
	inline	BOOL	GetPrevPath( Vector3f po_vecPath );
	inline	BOOL	GetCurPath( Vector3f po_vecPath );
	inline	BOOL	GetPath( Vector3f po_vecPath, int pi_nPathNum );

	inline	void	IncPath( void )			{ if( m_nCurPath < m_nTotalPath -1 ) ++m_nCurPath; }
	inline	void	DecPath( void )			{ if( m_nCurPath > 0 )				 --m_nCurPath; }

	// ---------------------------------------------------------------------

	inline	int		GetTotalPathNo( void )	{ return m_nTotalPath; }
	inline	int		GetCurPathNo( void )	{ return m_nCurPath; }

	inline	DWORD	GetPathResult( void )	{ return m_nPathResult; }
	inline	void	GetPickPoint( Vector3f po_vecPickPoint );
	inline	void	SetPickPoint( Vector3f po_vecPickPoint );
	// ---------------------------------------------------------------------
	/*
	inline	BOOL	SetNextPath( Vector3f po_vecPath );
	inline	BOOL	SetPrevPath( Vector3f po_vecPath );
	inline	BOOL	SetCurPath( Vector3f po_vecPath );
	inline	BOOL	SetPath( Vector3f po_vecPath, int pi_nPathNum );
	inline	void	SetTotalPath( Vector3f pi_vecPath );
	*/
};

////////////////////////////////////////////////////////////////////////////
//
inline BOOL
CPath::GetNextPath( Vector3f po_vecPath )
{
	if( m_nCurPath >= ( m_nTotalPath - 1 ) )
		return FALSE;

	Vector3fCopy( m_vecPathStack[ m_nCurPath + 1 ], po_vecPath );

	return TRUE;
}

inline BOOL
CPath::GetPrevPath( Vector3f po_vecPath )
{
	if( m_nCurPath < 0 )
		return FALSE;

	Vector3fCopy( m_vecPathStack[ m_nCurPath - 1 ], po_vecPath );

	return TRUE;
}

inline BOOL
CPath::GetCurPath( Vector3f po_vecPath )
{
	if( ( m_nCurPath < 0 ) || ( m_nCurPath >= m_nTotalPath ) )
		return FALSE;

	Vector3fCopy( m_vecPathStack[ m_nCurPath ], po_vecPath );

	return TRUE;
}

inline BOOL
CPath::GetPath( Vector3f po_vecPath, int pi_nPathNum )
{
	if( ( pi_nPathNum < 0 ) || ( pi_nPathNum >= m_nTotalPath ) )
		return FALSE;

	Vector3fCopy( m_vecPathStack[ pi_nPathNum ], po_vecPath );

	return TRUE;
}

inline void	
CPath::GetPickPoint( Vector3f po_vecPickPoint )
{
	Vector3fCopy( m_vecPickPoint, po_vecPickPoint );
}

inline void	
CPath::SetPickPoint( Vector3f po_vecPickPoint )
{
	Vector3fCopy( po_vecPickPoint,m_vecPickPoint );
}

////////////////////////////////////////////////////////////////////////////
//
/*
inline BOOL
CPath::SetNextPath( Vector3f pi_vecPath )
{
	Vector3fCopy( pi_vecPath, m_vecPathStack[ m_nCurPath + 1 ] );

	return TRUE;
}

inline BOOL
CPath::SetPrevPath( Vector3f pi_vecPath )
{
	Vector3fCopy( pi_vecPath, m_vecPathStack[ m_nCurPath - 1 ] );

	return TRUE;
}

inline BOOL
CPath::SetCurPath( Vector3f pi_vecPath )
{
	Vector3fCopy( pi_vecPath, m_vecPathStack[ m_nCurPath ] );

	return TRUE;
}

inline BOOL
CPath::SetPath( Vector3f pi_vecPath, int pi_nPathNum )
{
	Vector3fCopy( pi_vecPath, m_vecPathStack[ pi_nPathNum ] );

	return TRUE;
}

inline void
CPath::SetTotalPath( Vector3f pi_vecPath )
{
	m_nTotalPath	= 1;
	m_nCurPath		= 0;

	Vector3fCopy( pi_vecPath, m_vecPathStack[ m_nCurPath ] );
}*/

#endif // __PATHFINDER_H__