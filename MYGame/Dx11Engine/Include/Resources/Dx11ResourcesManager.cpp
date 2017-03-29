#include "Dx11ResourcesManager.h"
#include "Dx11Mesh.h"
#include "Dx11Texture.h"
#include "Dx11Sampler.h"
#include "../Component/Dx11Material.h"

DX11_USING

DX11_DEFINITION_SINGLE(CDx11ResourcesManager)

CDx11ResourcesManager::CDx11ResourcesManager()
{
	SetTypeName<CDx11ResourcesManager>();
}

CDx11ResourcesManager::~CDx11ResourcesManager()
{
	Safe_Release_Map(m_mapSampler);
	Safe_Release_Map(m_mapTexture);
	Safe_Release_Map(m_mapMesh);
}

bool CDx11ResourcesManager::Init()
{
	//------------------- ★샘플러 생성★------------------------
	CDx11Sampler*	pSampler = CreateSampler(DEFAULT_SAMPLER);
	SAFE_RELEASE(pSampler);

	// ---------------------TRIANGLE_MESH 생성---------------------
	COLORVERTEX	tColorVtx[3] =
	{
		{ Vec3(0.f, 0.5f, 0.f), Vec4(1.f, 0.f, 0.f, 1.f) },
		{ Vec3(0.5f, -0.5f, 0.f), Vec4(0.f, 1.f, 0.f, 1.f) },
		{ Vec3(-0.5f, -0.5f, 0.f), Vec4(0.f, 0.f, 1.f, 1.f) }
	};
	UINT	iIndex[3] = { 0, 1, 2 };
	CDx11Mesh*	pMesh = CreateMesh(TRIANGLE_MESH, sizeof(COLORVERTEX),
		3, tColorVtx, sizeof(UINT), 3, iIndex,
		DXGI_FORMAT_R32_UINT);
	SAFE_RELEASE(pMesh);

	// ---------------------RECT_TEX_MESH 생성----------------------
	TEXVERTEX	tTexVtx[4] =
	{
		{ Vec3(-0.5f, 0.5f, 0.f), Vec3(0.f, 0.f, -1.f), Vec2(0.f, 0.f) },
		{ Vec3(0.5f, 0.5f, 0.f), Vec3(0.f, 0.f, -1.f), Vec2(1.f, 0.f) },
		{ Vec3(-0.5f, -0.5f, 0.f), Vec3(0.f, 0.f, -1.f), Vec2(0.f, 1.f) },
		{ Vec3(0.5f, -0.5f, 0.f), Vec3(0.f, 0.f, -1.f), Vec2(1.f, 1.f) }
	};
	UINT	iIndex1[6] = { 0, 1, 3, 0, 3, 2 };
	pMesh = CreateMesh(RECT_TEX_MESH, sizeof(TEXVERTEX),
		4, tTexVtx, sizeof(UINT), 6, iIndex1,
		DXGI_FORMAT_R32_UINT);
	SAFE_RELEASE(pMesh);

	// -----------------------------삼각뿔-------------------------
	COLORVERTEX	tColorVtx2[18] =
	{
		//앞면
		{ Vec3(0.0f,0.5f, 0.f), Vec4(0.f, 0.f, 1.f, 1.f) },
		{ Vec3(0.5f, -0.5f, -0.5f), Vec4(1.f, 0.f, 0.f, 1.f) },
		{ Vec3(-0.5f, -0.5f, -0.5f), Vec4(0.f, 1.f, 0.f, 1.f) },

		{ Vec3(0.0f,0.5f, 0.f), Vec4(0.f, 0.f, 1.f, 1.f) },
		{ Vec3(0.5f, -0.5f, 0.5f), Vec4(1.f, 0.f, 0.f, 1.f) },
		{ Vec3(0.5f, -0.5f, -0.5f), Vec4(0.f, 1.f, 0.f, 1.f) },

		{ Vec3(0.0f,0.5f, 0.f), Vec4(0.f, 0.f, 1.f, 1.f) },
		{ Vec3(-0.5f, -0.5f, 0.5f), Vec4(1.f, 0.f, 0.f, 1.f) },
		{ Vec3(0.5f, -0.5f, 0.5f), Vec4(0.f, 1.f, 0.f, 1.f) },

		{ Vec3(0.0f,0.5f, 0.f), Vec4(0.f, 0.f, 1.f, 1.f) },
		{ Vec3(-0.5f, -0.5f, -0.5f), Vec4(1.f, 0.f, 0.f, 1.f) },
		{ Vec3(-0.5f, -0.5f, 0.5f), Vec4(0.f, 1.f, 0.f, 1.f) },

		{ Vec3(-0.5f, -0.5f, 0.5f), Vec4(0.f, 0.f, 1.f, 1.f) },
		{ Vec3(-0.5f, -0.5f, -0.5f), Vec4(1.f, 0.f, 0.f, 1.f) },
		{ Vec3(0.5f, -0.5f, 0.5f), Vec4(0.f, 1.f, 0.f, 1.f) },

		{ Vec3(0.5f,-0.5f, -0.5f), Vec4(0.f, 0.f, 1.f, 1.f) },
		{ Vec3(0.5f, -0.5f, 0.5f), Vec4(1.f, 0.f, 0.f, 1.f) },
		{ Vec3(-0.5f, -0.5f, -0.5f), Vec4(0.f, 1.f, 0.f, 1.f) },

	};

	UINT	iIndex2[18] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17 };//,4,5,6,7,8,9,10,11,12,13,14,15,16,17

	CDx11Mesh*	pMesh2 = CreateMesh(PYRAMID_MESH, sizeof(COLORVERTEX),
		18, tColorVtx2, sizeof(UINT), 18, iIndex2,
		DXGI_FORMAT_R32_UINT);
	SAFE_RELEASE(pMesh2);

	// -----------------------------육각형---------------------------------
	COLORVERTEX	tColorVtx4[8] =
	{
		//0
		{ Vec3(-0.5f,0.5f,-0.5f),Vec4(0.f, 0.f, 1.f,1.f) },

		//1					
		{ Vec3(0.5f,0.5f, -0.5f),Vec4(1.f, 0.f, 0.f,1.f) },

		//2						
		{ Vec3(0.5f,-0.5f, -0.5f),Vec4(0.f, 1.f, 0.f,1.f) },

		//3						
		{ Vec3(-0.5f,-0.5f, -0.5f),Vec4(1.f, 0.f, 1.f,1.f) },

		//4						
		{ Vec3(-0.5f,0.5f,0.5f),Vec4(1.f, 1.f, 0.f,1.f) },

		//5						 
		{ Vec3(0.5f,0.5f, 0.5f),Vec4(1.f, 1.f, 1.f,1.f) },

		//6						
		{ Vec3(0.5f,-0.5f, 0.5f),Vec4(1.f, 0.f, 0.f,1.f) },

		//7						
		{ Vec3(-0.5f,-0.5f, 0.5f),Vec4(0.f, 0.f, 0.f,1.f) }
	};

	UINT	iIndex3[36] = {
		0,1,2,	//가운데
		0,2,3,
		4,0,3,	//왼쪽
		4,3,7,
		1,5,6,	//오른쪽
		1,6,2,
		3,2,7,	//아래
		2,6,7,
		4,5,1,	//위
		4,1,0,
		5,4,6, //뒤(반시계)
		4,7,6 };

	CDx11Mesh*	pMesh3 = CreateMesh(HEXAGON_MESH, sizeof(COLORVERTEX),
		8, tColorVtx4, sizeof(UINT), 36, iIndex3,
		DXGI_FORMAT_R32_UINT);
	SAFE_RELEASE(pMesh3);

	// ----------------------- HEXAGON_TEX_MESH -------------------------------
	TEXVERTEX	tColorVtx3[36] =
	{
		//앞면
		{ Vec3(-0.5f,0.5f,-0.5f),Vec3(0.f, 0.f, -1.f), Vec2(0.f, 0.f) },
		{ Vec3(0.5f,0.5f, -0.5f),Vec3(0.f, 0.f, -1.f), Vec2(1.f, 0.f) },
		{ Vec3(0.5f,-0.5f, -0.5f),Vec3(0.f, 0.f, -1.f), Vec2(1.f, 1.f) },
		{ Vec3(-0.5f,0.5f,-0.5f),Vec3(0.f, 0.f, -1.f), Vec2(0.f, 0.f) },
		{ Vec3(0.5f,-0.5f, -0.5f),Vec3(0.f, 0.f, -1.f), Vec2(1.f, 1.f) },
		{ Vec3(-0.5f,-0.5f, -0.5f),Vec3(0.f, 0.f, -1.f), Vec2(0.f, 1.f) },

		//왼면
		{ Vec3(-0.5f,0.5f,0.5f),Vec3(-1.f, 0.f, -1.f), Vec2(0.f, 0.f) },
		{ Vec3(-0.5f,0.5f, -0.5f),Vec3(-1.f, 0.f, -1.f), Vec2(1.f, 0.f) },
		{ Vec3(-0.5f,-0.5f, -0.5f),Vec3(-1.f, 0.f, -1.f), Vec2(1.f, 1.f) },
		{ Vec3(-0.5f,0.5f,0.5f),Vec3(-1.f, 0.f, -1.f), Vec2(0.f, 0.f) },
		{ Vec3(-0.5f,-0.5f, -0.5f),Vec3(-1.f, 0.f, -1.f), Vec2(1.f, 1.f) },
		{ Vec3(-0.5f,-0.5f, 0.5f),Vec3(-1.f, 0.f, -1.f), Vec2(0.f, 1.f) },

		//오른쪽
		{ Vec3(0.5f,0.5f,-0.5f),Vec3(0.f, 0.f, -1.f), Vec2(0.f, 0.f) },
		{ Vec3(0.5f,0.5f, 0.5f),Vec3(0.f, 0.f, -1.f), Vec2(1.f, 0.f) },
		{ Vec3(0.5f,-0.5f, 0.5f),Vec3(0.f, 0.f, -1.f), Vec2(1.f, 1.f) },

		{ Vec3(0.5f,0.5f,-0.5f),Vec3(0.f, 0.f, -1.f), Vec2(0.f, 0.f) },
		{ Vec3(0.5f,-0.5f, 0.5f),Vec3(0.f, 0.f, -1.f), Vec2(1.f, 1.f) },
		{ Vec3(0.5f,-0.5f, -0.5f),Vec3(0.f, 0.f, -1.f), Vec2(0.f, 1.f) },

		//아래
		{ Vec3(-0.5f,-0.5f,-0.5f),Vec3(0.f, 0.f, -1.f), Vec2(0.f, 0.f) },
		{ Vec3(0.5f,-0.5f, -0.5f),Vec3(0.f, 0.f, -1.f), Vec2(1.f, 0.f) },
		{ Vec3(0.5f,-0.5f, 0.5f),Vec3(0.f, 0.f, -1.f), Vec2(1.f, 1.f) },

		{ Vec3(-0.5f,-0.5f,-0.5f),Vec3(0.f, 0.f, -1.f), Vec2(0.f, 0.f) },
		{ Vec3(0.5f,-0.5f, 0.5f),Vec3(0.f, 0.f, -1.f), Vec2(1.f, 1.f) },
		{ Vec3(-0.5f,-0.5f, 0.5f),Vec3(0.f, 0.f, -1.f), Vec2(0.f, 1.f) },

		//위
		{ Vec3(-0.5f,0.5f,0.5f),Vec3(0.f, 0.f, -1.f), Vec2(0.f, 0.f) },
		{ Vec3(0.5f,0.5f,0.5f),Vec3(0.f, 0.f, -1.f), Vec2(1.f, 0.f) },
		{ Vec3(0.5f,0.5f, -0.5f),Vec3(0.f, 0.f, -1.f), Vec2(1.f, 1.f) },

		{ Vec3(-0.5f,0.5f,0.5f),Vec3(0.f, 0.f, -1.f), Vec2(0.f, 0.f) },
		{ Vec3(0.5f,0.5f, -0.5f),Vec3(0.f, 0.f, -1.f), Vec2(1.f, 1.f) },
		{ Vec3(-0.5f,0.5f,-0.5f),Vec3(0.f, 0.f, -1.f), Vec2(0.f, 1.f) },

		//뒤(반시계)
		{ Vec3(0.5f,0.5f,0.5f),Vec3(0.f, 0.f, -1.f), Vec2(0.f, 0.f) },
		{ Vec3(-0.5f,0.5f,0.5f),Vec3(0.f, 0.f, -1.f), Vec2(1.f, 0.f) },
		{ Vec3(-0.5f,-0.5f,0.5f),Vec3(0.f, 0.f, -1.f), Vec2(1.f, 1.f) },

		{ Vec3(0.5f,0.5f,0.5f),Vec3(0.f, 0.f, -1.f), Vec2(0.f, 0.f) },
		{ Vec3(-0.5f,-0.5f,0.5f),Vec3(0.f, 0.f, -1.f), Vec2(1.f, 1.f) },
		{ Vec3(0.5f,-0.5f, 0.5f),Vec3(0.f, 0.f, -1.f), Vec2(0.f, 1.f) },
	};

	UINT iIndex4[36] = {
		0,1,2,	//가운데
		3,4,5,
		6,7,8,	//왼쪽
		9,10,11,
		12,13,14,	//오른쪽
		15,16,17,
		18,19,20,	//아래
		21,22,23,
		24,25,26,	//위
		27,28,29,
		30,31,32, //뒤(반시계)
		33,34,35 };

	CDx11Mesh*	pMesh4 = CreateMesh(HEXAGON_TEX_MESH, sizeof(TEXVERTEX),
		36, tColorVtx3, sizeof(UINT), 36, iIndex4, DXGI_FORMAT_R32_UINT);
	SAFE_RELEASE(pMesh4);

	// --------------------------- UI RECT---------------------------
	UIVERTEX	tUIVtx[4] =
	{
		{ Vec3(0.f, 0.f, 0.f), Vec2(0.f, 0.f) },
		{ Vec3(1.f, 0.f, 0.f), Vec2(1.f, 0.f) },
		{ Vec3(0.f, 1.f, 0.f), Vec2(0.f, 1.f) },
		{ Vec3(1.f, 1.f, 0.f), Vec2(1.f, 1.f) }
	};

	UINT	iIndex5[6] = { 0, 1, 3, 0, 3, 2 };

	pMesh = CreateMesh(RECT_UI_MESH, sizeof(UIVERTEX),
		4, tUIVtx, sizeof(UINT), 6, iIndex5,
		DXGI_FORMAT_R32_UINT);

	SAFE_RELEASE(pMesh);

	// --------------------------- SPHERE_MESH ----------------------------
	pMesh = CreateSphere(SPHERE_MESH, 0.5f, 100, 100);
	SAFE_RELEASE(pMesh);

	// -----------------Effect 센터(사각형) ---------------------------
	VERTEX2D tEffectCenterVtx[4] =
	{
		{ Vec3(-0.5f, 0.5f, 0.f), Vec2(0.f, 0.f) },
		{ Vec3(0.5f, 0.5f, 0.f), Vec2(1.f, 0.f) },
		{ Vec3(-0.5f, -0.5f, 0.f), Vec2(0.f, 1.f) },
		{ Vec3(0.5f, -0.5f, 0.f), Vec2(1.f, 1.f) }
	};

	WORD	wEffectCenterIndex[6] = { 0, 1, 3, 0, 3, 2 };

	pMesh = CreateMesh(EffectCenterMesh, sizeof(VERTEX2D),
		4, tEffectCenterVtx, sizeof(WORD), 6, wEffectCenterIndex,
		DXGI_FORMAT_R16_UINT);

	SAFE_RELEASE(pMesh);
	return true;
}

CDx11Mesh * CDx11ResourcesManager::CreateMesh(const string & strKey,
	UINT iVtxSize, UINT iVtxCount, void * pVertices,
	UINT iIdxSize, UINT iIdxCount, void * pIndices,
	DXGI_FORMAT eIdxFmt,
	D3D11_PRIMITIVE_TOPOLOGY eTopology)
{
	CDx11Mesh*	pMesh = FindMesh(strKey);

	if (pMesh)
		return pMesh;

	pMesh = new CDx11Mesh;

	if (!pMesh->CreateMesh(iVtxSize, iVtxCount, pVertices,
		iIdxSize, iIdxCount, pIndices, eIdxFmt, eTopology))
	{
		SAFE_RELEASE(pMesh);
		return NULL;
	}

	pMesh->AddRef();
	m_mapMesh.insert(make_pair(strKey, pMesh));

	return pMesh;
}

CDx11Mesh * CDx11ResourcesManager::LoadMesh(const string & strKey, TCHAR * pFileName,
	const string & strPathKey)
{
	CDx11Mesh*	pOldMesh = FindMesh(strKey);

	//기존 메쉬의 텍+샘만 복사하면되지않을까?
	if (pOldMesh) {				
		CDx11Mesh *pMesh(pOldMesh);		
		pMesh->AddRef();
		m_mapMesh.insert(make_pair(strKey, pMesh));
		return pMesh;
	}
	else{
		CDx11Mesh* pMesh = new CDx11Mesh;
		if (!pMesh->LoadMesh(strKey, pFileName, strPathKey))
		{
			SAFE_RELEASE(pMesh);
			return NULL;
		}
		pMesh->AddRef();
		m_mapMesh.insert(make_pair(strKey, pMesh));
		return pMesh;
	}	
}

CDx11Mesh * CDx11ResourcesManager::CreateSphere(const string & strKey, float fRadius, UINT iSliceCount, UINT iStackCount)
{
	CDx11Mesh*	pMesh = FindMesh(strKey);

	if (pMesh)
		return pMesh;

	pMesh = new CDx11Mesh;

	if (!pMesh->CreateSphere(fRadius, iSliceCount, iStackCount))
	{
		SAFE_RELEASE(pMesh);
		return NULL;
	}

	pMesh->AddRef();
	m_mapMesh.insert(make_pair(strKey, pMesh));

	return pMesh;
}

CDx11Mesh * CDx11ResourcesManager::FindMesh(const string & strKey)
{
	unordered_map<string, class CDx11Mesh*>::iterator	iter = m_mapMesh.find(strKey);

	if (iter == m_mapMesh.end())
		return NULL;

	iter->second->AddRef();

	return iter->second;
}

CDx11Texture * CDx11ResourcesManager::LoadTexture(const string & strKey,
	TCHAR * pFileName, const string & strPathKey)
{
	CDx11Texture*	pTexture = FindTexture(strKey);

	if (pTexture)
		return pTexture;

	if (!pFileName)
		return NULL;

	pTexture = new CDx11Texture;

	if (!pTexture->LoadTexture(pFileName, strPathKey))
	{
		SAFE_RELEASE(pTexture);
		return NULL;
	}

	pTexture->AddRef();

	m_mapTexture.insert(make_pair(strKey, pTexture));

	return pTexture;
}

CDx11Texture * CDx11ResourcesManager::LoadTexture(const string & strKey, string strPath)
{
	CDx11Texture*	pTexture = FindTexture(strKey);

	if (pTexture)
		return pTexture;

	pTexture = new CDx11Texture;

	if (!pTexture->LoadTexture(strPath))
	{
		SAFE_RELEASE(pTexture);
		return NULL;
	}

	pTexture->AddRef();

	m_mapTexture.insert(make_pair(strKey, pTexture));

	return pTexture;
}

CDx11Texture * CDx11ResourcesManager::FindTexture(const string & strKey)
{
	unordered_map<string, CDx11Texture*>::iterator	iter = m_mapTexture.find(strKey);

	if (iter == m_mapTexture.end())
		return NULL;

	iter->second->AddRef();

	return iter->second;
}

CDx11Sampler * CDx11ResourcesManager::CreateSampler(const string & strKey,
	D3D11_FILTER eFilter, D3D11_TEXTURE_ADDRESS_MODE u,
	D3D11_TEXTURE_ADDRESS_MODE v, D3D11_TEXTURE_ADDRESS_MODE w,
	D3D11_COMPARISON_FUNC eFunc)
{
	CDx11Sampler*	pSampler = FindSampler(strKey);

	if (pSampler)
		return pSampler;

	pSampler = new CDx11Sampler;

	if (!pSampler->CreateSampler(eFilter, u, v, w, eFunc))
	{
		SAFE_RELEASE(pSampler);
		return NULL;
	}

	pSampler->AddRef();

	m_mapSampler.insert(make_pair(strKey, pSampler));

	return pSampler;
}

CDx11Sampler * CDx11ResourcesManager::FindSampler(const string & strKey)
{
	unordered_map<string, class CDx11Sampler*>::iterator	iter = m_mapSampler.find(strKey);

	if (iter == m_mapSampler.end())
		return NULL;

	iter->second->AddRef();

	return iter->second;
}
