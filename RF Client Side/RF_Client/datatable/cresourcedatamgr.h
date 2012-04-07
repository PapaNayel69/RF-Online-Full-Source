////////////////////////////////////////////////////////////////////////////////
//
// CCharResDataMgr
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __RESOURCE_DATA_MGR_H__
#define __RESOURCE_DATA_MGR_H__

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////    Resoruce Data List    ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct BONE_DATA
{
	DWORD	dwID;
	char	pPathName[128];
	char	pFileName[64];
	char	pBBoxName[64];
};

struct MESH_DATA
{
	DWORD	dwID;
	DWORD	dwBoneID;
	char	pPathName[128];
	char	pFileName[64];
	char	pTexturePath[128];
};

struct ANI_DATA
{
	DWORD	dwID;
	DWORD	dwNum;							// 애니메이션의 갯수
	char	pPathName[128];
	char	pFileName[64];

	DWORD	dwEffectNum;					// 이펙트 프레임의 갯수
	DWORD	dwEffectFrame[10];				// 최대 10개
};

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

class CCharResData
{
	DWORD		m_dwBoneNum;
	DWORD		m_dwMeshNum;
	DWORD		m_dwAniNum;

	BONE_DATA *	m_listBone;
	MESH_DATA *	m_listMesh;
	ANI_DATA *	m_listAni;

public :

	CCharResData();
	~CCharResData();

			BOOL		LoadBoneDataList( BYTE pi_byResFileType, CDataString * pi_pDataString );
			BOOL		LoadMeshDataList( BYTE pi_byResFileType, CDataString * pi_pDataString );
			BOOL		LoadAniDataList( BYTE pi_byResFileType, CDataString * pi_pDataString );

			BONE_DATA *	GetBoneData( DWORD pi_dwIndex );
			MESH_DATA *	GetMeshData( DWORD pi_dwIndex );
			ANI_DATA *	GetAniData( DWORD pi_dwIndex );

			BONE_DATA *	GetBoneDataByOrder( DWORD pi_dwIndex );
			MESH_DATA *	GetMeshDataByOrder( DWORD pi_dwIndex );
			ANI_DATA *	GetAniDataByOrder( DWORD pi_dwIndex );

	inline	void		SetTotalBoneNum( DWORD pi_dwNum )	{ m_dwBoneNum = pi_dwNum; }
	inline	void		SetTotalMeshNum( DWORD pi_dwNum )	{ m_dwMeshNum = pi_dwNum; }
	inline	void		SetTotalAniNum( DWORD pi_dwNum )	{ m_dwAniNum = pi_dwNum; }

	inline	DWORD		GetTotalBoneNum( void )				{ return m_dwBoneNum; }
	inline	DWORD		GetTotalMeshNum( void )				{ return m_dwMeshNum; }
	inline	DWORD		GetTotalAniNum( void )				{ return m_dwAniNum; }

	inline	void		SetBoneData( BONE_DATA * pi_pData )	{ m_listBone = pi_pData; }
	inline	void		SetMeshData( MESH_DATA * pi_pData )	{ m_listMesh = pi_pData; }
	inline	void		SetAniData( ANI_DATA * pi_pData )	{ m_listAni = pi_pData; }

	inline	BONE_DATA *	GetBoneData( void )					{ return m_listBone; }
	inline	MESH_DATA *	GetMeshData( void )					{ return m_listMesh; }
	inline	ANI_DATA *	GetAniData( void )					{ return m_listAni; }
};

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

class CCharResDataMgr
{
	CCharResData	m_listResData[MAX_RES_DATA_LIST];

public :

	CCharResDataMgr();
	~CCharResDataMgr();

	BOOL	LoadData();

	inline	CCharResData *	GetResourceList( BYTE pi_byType ){ &m_listResData[pi_byType]; }
};

#endif __RESOURCE_DATA_MGR_H__