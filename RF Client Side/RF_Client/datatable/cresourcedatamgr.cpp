////////////////////////////////////////////////////////////////////////////////
//
// CCharResDataMgr
//
////////////////////////////////////////////////////////////////////////////////

#include "../GameObject/Manager/CCharacterMgr.h"

#include "../Global.h"
#include "../DataFile.h"

#include "CResourceDataMgr.h"

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////    Resoruce Data List    ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CCharResData::CCharResData()
{
	m_dwBoneNum	= 0;
	m_dwMeshNum	= 0;
	m_dwAniNum	= 0;

	m_listBone	= NULL;
	m_listMesh	= NULL;
	m_listAni	= NULL;
}

CCharResData::~CCharResData()
{
	delete [] m_listBone;
	delete [] m_listMesh;
	delete [] m_listAni;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CCharResData::LoadBoneDataList( BYTE pi_byResFileType, CDataString * pi_pDataString )
{
	if( !pi_pDataString )
		return FALSE;

	// data read
	pi_pDataString->Read( &m_dwBoneNum, sizeof( DWORD ), 1 );
	if( m_dwBoneNum <= 0 )
		return FALSE;

	m_listBone = new BONE_DATA[m_dwBoneNum];
	pi_pDataString->Read( m_listBone, sizeof( BONE_DATA ), m_dwBoneNum );

	// load resource
	CBone *		l_pBone;
	char		l_pBuf[256];
	for( int i = 0; i < m_dwBoneNum; ++i )
	{
		sprintf( l_pBuf, "%s%s", m_listBone[i].pPathName, m_listBone[i].pFileName );
		l_pBone = _GetBoneMgr()->AddBone( pi_byResFileType, l_pBuf );
		if( l_pBone )
			l_pBone->m_ID = m_listBone[i].dwID;

		sprintf( l_pBuf, "%s%s", m_listBone[i].pPathName, m_listBone[i].pBBoxName );
		_GetBBoxMgr()->LoadBoundBox( pi_byResFileType, l_pBuf, m_listBone[i].dwID );
	}

	return TRUE;
}

BOOL
CCharResData::LoadMeshDataList( BYTE pi_byResFileType, CDataString * pi_pDataString )
{
	if( !pi_pDataString )
		return FALSE;

	// data read
	pi_pDataString->Read( &m_dwMeshNum, sizeof( DWORD ), 1 );
	if( m_dwMeshNum <= 0 )
		return FALSE;

	m_listMesh = new MESH_DATA[m_dwMeshNum];
	pi_pDataString->Read( m_listMesh, sizeof( MESH_DATA ), m_dwMeshNum );

	//--------------------------------------------------------------------------
	// load resource

	CBone *		l_pBone = NULL;
	CMesh *		l_pMesh = NULL;
	char		l_pBuf[256];
	DWORD		l_pTemp = 0;
	for( int i = 0; i < m_dwMeshNum; ++i )
	{
		// 아이템일 경우
		if( pi_byResFileType == RLI_ITEM )
		{
			// pMesh[i].dwBoneID가 -1이면 Avatar의 Bone을 사용
			// - Avatar의 Bone은 현 시점에서는 적용되어 있지 않으므로 디폴트 본으로 ID_DEFAULT_BONE_BM을 사용한다.
			// pMesh[i].dwBoneID가 -2이면 NULL을 사용 ( Model Item )
			// 나머지는 BMI_PLAYER를 사용해서 pMesh[i].dwBoneID 에 해당하는 Bone을 가져옴.
			if( m_listMesh[i].dwBoneID == -1 )
			{
				l_pBone = _GetBoneMgr()->GetBone( RLI_PLAYER, ID_DEFAULT_BONE_BM );
				if( !l_pBone )
					continue;
			}
			else if( m_listMesh[i].dwBoneID == -2 )
			{
				// Model Item은 bound box 를 읽어들여야 한다.
				l_pBone = NULL;

				m_listMesh[i].dwBoneID = m_listMesh[i].dwID;

				sprintf( l_pBuf, "%s%s", m_listMesh[i].pPathName, m_listMesh[i].pFileName );
				l_pTemp = strlen( l_pBuf );
				l_pBuf[l_pTemp - 3] = 'B';
				l_pBuf[l_pTemp - 2] = 'B';
				l_pBuf[l_pTemp - 1] = 'X';

				_GetBBoxMgr()->LoadBoundBox( pi_byResFileType, l_pBuf, m_listMesh[i].dwBoneID );
			}
			else
			{
				l_pBone = _GetBoneMgr()->GetBone( RLI_PLAYER, m_listMesh[i].dwBoneID );
				if( !l_pBone )
					continue;
			}
		}
		// 아이템이 아닌 경우
		else
		{
			l_pBone = _GetBoneMgr()->GetBone( pi_byResFileType, m_listMesh[i].dwBoneID );
			if( !l_pBone )
				continue;
		}

		// load mesh
		sprintf( l_pBuf, "%s%s", m_listMesh[i].pPathName, m_listMesh[i].pFileName );
		l_pMesh = _GetMeshMgr()->AddMesh( pi_byResFileType, l_pBuf, l_pBone );
		if( !l_pMesh )
			continue;

		l_pMesh->m_ID = m_listMesh[i].dwID;

		// texture path
		_GetCharIF()->SetTexPath( l_pMesh, m_listMesh[i].pTexturePath );
	}

	return TRUE;
}

BOOL
CCharResData::LoadAniDataList( BYTE pi_byResFileType, CDataString * pi_pDataString )
{
	if( !pi_pDataString )
		return FALSE;

	// data read
	pi_pDataString->Read( &m_dwAniNum, sizeof( DWORD ), 1 );
	if( m_dwAniNum <= 0 )
		return FALSE;

	m_listAni = new ANI_DATA[m_dwAniNum];
	pi_pDataString->Read( m_listAni, sizeof( ANI_DATA ), m_dwAniNum );

	// load resource
	CAniData *	l_pAni = NULL;
	char		l_pBuf[256];
	for( int i = 0; i < m_dwAniNum; ++i )
	{
		sprintf( l_pBuf, "%s%s", m_listAni[i].pPathName, m_listAni[i].pFileName );
		l_pAni = _GetAniMgr()->AddAni( pi_byResFileType, l_pBuf );
		if( !l_pAni )
			continue;

		l_pAni->m_Ident = m_listAni[i].dwID;

		//----------------------------------------------------------------------

		ANI_EXT_INFO * l_pExtInfo = new ANI_EXT_INFO;

		l_pExtInfo->nAniNumOfSameAction = m_listAni[i].dwNum;
		l_pExtInfo->nMaxFrameNum = m_listAni[i].dwEffectNum;
		for( int j = 0; j < l_pExtInfo->nMaxFrameNum; ++j )
			l_pExtInfo->dwEffectFrame[j] = 160 + ( m_listAni[i].dwEffectFrame[j] * 160 );  // 160 은 max 에서 쓰이는 tick count 이다. 곧, 1프레임은 160 tick

		l_pAni->m_pExtra = ( void * )l_pExtInfo;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CCharResDataMgr::CCharResDataMgr()
{
}

CCharResDataMgr::~CCharResDataMgr()
{
}

BOOL
CCharResDataMgr::LoadData( void )
{
	CDataFile l_fileResData( "./DataTable/Resource.edf" );

	CDataString * l_pSourceData = l_fileResData.GetSourceData();
	if( !l_pSourceData )
		return FALSE;
	if( !l_pSourceData->GetString() )
		return FALSE;

	for( int i = 0; i < MAX_RES_DATA_LIST; ++i )
	{
		m_listResData[i].LoadBoneDataList( i, l_pSourceData );
		( i == RLI_PLAYER ) ? m_listResData[i].LoadMeshDataList( RLI_ITEM, l_pSourceData )
							: m_listResData[i].LoadMeshDataList( i, l_pSourceData );
		m_listResData[i].LoadAniDataList( i, l_pSourceData );
	}

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BONE_DATA *
CCharResData::GetBoneData( DWORD pi_dwIndex )
{
	if( pi_dwIndex == ID_INVALID )
		return NULL;

	for( int i = 0; i < m_dwBoneNum; ++i )
	{
		if( m_listBone[i].dwID == pi_dwIndex )
		{
			return &m_listBone[pi_dwIndex];
		}
	}

	return NULL;
}

MESH_DATA *
CCharResData::GetMeshData( DWORD pi_dwIndex )
{
	if( pi_dwIndex == ID_INVALID )
		return NULL;

	for( int i = 0; i < m_dwMeshNum; ++i )
	{
		if( m_listMesh[i].dwID == pi_dwIndex )
		{
			return &m_listMesh[pi_dwIndex];
		}
	}

	return NULL;
}

ANI_DATA *
CCharResData::GetAniData( DWORD pi_dwIndex )
{
	if( pi_dwIndex == ID_INVALID )
		return NULL;

	for( int i = 0; i < m_dwAniNum; ++i )
	{
		if( m_listAni[i].dwID == pi_dwIndex )
		{
			return &m_listAni[pi_dwIndex];
		}
	}

	return NULL;
}

BONE_DATA *
CCharResData::GetBoneDataByOrder( DWORD pi_dwIndex )
{
	if( ( pi_dwIndex < 0 ) || ( pi_dwIndex >= m_dwBoneNum ) )
		return NULL;

	return &m_listBone[pi_dwIndex];
}

MESH_DATA *
CCharResData::GetMeshDataByOrder( DWORD pi_dwIndex )
{
	if( ( pi_dwIndex < 0 ) || ( pi_dwIndex >= m_dwMeshNum ) )
		return NULL;

	return &m_listMesh[pi_dwIndex];
}

ANI_DATA *
CCharResData::GetAniDataByOrder( DWORD pi_dwIndex )
{
	if( ( pi_dwIndex < 0 ) || ( pi_dwIndex >= m_dwAniNum ) )
		return NULL;

	return &m_listAni[pi_dwIndex];
}