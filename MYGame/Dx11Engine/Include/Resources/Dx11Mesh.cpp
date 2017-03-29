#include "Dx11Mesh.h"
#include "../Core/Dx11Device.h"
#include "../Core/Dx11FilePathManager.h"
#include "Dx11FbxLoader.h"
#include "Dx11Mesh.h"
#include "../Component/Dx11Material.h"

DX11_USING

CDx11Mesh::CDx11Mesh() :
	m_pLoader(NULL),
	m_iSubsetCount(1),
	m_iContainerCount(1)
{
	SetTypeName<CDx11Mesh>();
}

CDx11Mesh::CDx11Mesh(CDx11Mesh * Mesh)
{
	m_vecBuffer = Mesh->m_vecBuffer;
	m_iContainerCount = Mesh->m_iContainerCount;
	m_iSubsetCount = Mesh->m_iSubsetCount;
	m_eTopology = Mesh->m_eTopology;

	m_pLoader = new CDx11FbxLoader;
	m_pLoader = Mesh->m_pLoader;
}

CDx11Mesh::~CDx11Mesh()
{
	SAFE_RELEASE(m_pLoader);

	for (size_t i = 0; i < m_vecBuffer.size(); ++i)
	{
		SAFE_RELEASE(m_vecBuffer[i].tVB.pBuffer);

		for (size_t j = 0; j < m_vecBuffer[i].vecIB.size(); ++j)
		{
			SAFE_RELEASE(m_vecBuffer[i].vecIB[j].pBuffer);
		}

		Safe_Release_VecList(m_vecBuffer[i].vecMaterial);
	}
}

UINT CDx11Mesh::GetContainerCount()
{
	return m_vecBuffer.size();
}

UINT CDx11Mesh::GetSubsetCount(UINT iContainer)
{
	return m_vecBuffer[iContainer].vecIB.size();
}

CDx11Material * CDx11Mesh::CloneMaterial(int iContainer, int iSubset)
{
	return m_vecBuffer[iContainer].vecMaterial[iSubset]->Clone();
}

bool CDx11Mesh::IsMaterial()
{
	return !m_vecBuffer[0].vecMaterial.empty();
}

vector<BUFFERINFO> CDx11Mesh::GetBufferInfo()
{
	return m_vecBuffer;
}

void CDx11Mesh::SetBufferInfo(vector<BUFFERINFO> Bufferinfo)
{
	m_vecBuffer = Bufferinfo;
}

bool CDx11Mesh::CreateMesh(UINT iVtxSize, UINT iVtxCount,
	void * pVertices, UINT iIdxSize, UINT iIdxCount,
	void * pIndices, DXGI_FORMAT eIdxFmt,
	D3D11_PRIMITIVE_TOPOLOGY eTopology)
{
	m_eTopology = eTopology;

	BUFFERINFO	tBufferInfo = {};

	if (!CreateVertexBuffer(iVtxSize, iVtxCount, pVertices, &tBufferInfo.tVB))
		return false;

	INDEXBUFFER	tIB = {};
	if (!CreateIndexBuffer(iIdxSize, iIdxCount, pIndices, eIdxFmt, &tIB))
		return false;

	tBufferInfo.vecIB.push_back(tIB);

	m_vecBuffer.push_back(tBufferInfo);

	m_iSubsetCount = 1;

	return true;
}

bool CDx11Mesh::LoadMesh(const string& strKey, TCHAR * pFileName, const string & strPathKey)
{
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	wstring	strPath1 = DX11_GET_SINGLE(CDx11FilePathManager)->FindPath(strPathKey);
	strPath1 += pFileName;

	char	strPath[MAX_PATH] = {};

	WideCharToMultiByte(CP_ACP, 0, strPath1.c_str(), -1, strPath, strPath1.length() * 2,
		0, 0);

	m_pLoader = new CDx11FbxLoader;

	if (!m_pLoader->LoadMesh(strPath))
		return false;

	vector<PMESHDATA>&	vecMeshContainer = m_pLoader->GetMeshContainer();

	m_iContainerCount = vecMeshContainer.size();

	for (size_t i = 0; i < vecMeshContainer.size(); ++i)
	{
		BUFFERINFO	tBufferInfo = {};

		void*	pVertices = NULL;
		UINT	iVtxSize;

		// 정점에 애니메이션이 있는지를 구분한다.
		// 애니메이션이 있을 경우
		if (vecMeshContainer[i]->bAnimation)
		{
			// Bump 정보가 있는지 판단한다.
			if (vecMeshContainer[i]->bBump)
			{
				vector<ANIMBUMPVERTEX>	vecVtx;

				for (int j = 0; j < vecMeshContainer[i]->vecPos.size(); ++j)
				{
					ANIMBUMPVERTEX	tVtx = {};

					tVtx.vPos = vecMeshContainer[i]->vecPos[j];
					tVtx.vNormal = vecMeshContainer[i]->vecNormal[j];
					tVtx.vUV = vecMeshContainer[i]->vecUV[j];
					tVtx.vWeights = vecMeshContainer[i]->vecWeights[j];
					tVtx.vIndices = vecMeshContainer[i]->vecIndices[j];
					tVtx.vTangent = vecMeshContainer[i]->vecTangent[j];

					vecVtx.push_back(tVtx);
				}

				pVertices = &vecVtx[0];
				iVtxSize = sizeof(ANIMBUMPVERTEX);

				if (!CreateVertexBuffer(iVtxSize, vecMeshContainer[i]->vecPos.size(),
					pVertices, &tBufferInfo.tVB))
					return false;
			}

			// 일반 정점일 경우
			else
			{
				vector<ANIMTEXVERTEX>	vecVtx;

				for (int j = 0; j < vecMeshContainer[i]->vecPos.size(); ++j)
				{
					ANIMTEXVERTEX	tVtx = {};

					tVtx.vPos = vecMeshContainer[i]->vecPos[j];
					tVtx.vNormal = vecMeshContainer[i]->vecNormal[j];
					tVtx.vUV = vecMeshContainer[i]->vecUV[j];
					tVtx.vWeights = vecMeshContainer[i]->vecWeights[j];
					tVtx.vIndices = vecMeshContainer[i]->vecIndices[j];

					vecVtx.push_back(tVtx);
				}

				pVertices = &vecVtx[0];
				iVtxSize = sizeof(ANIMTEXVERTEX);

				if (!CreateVertexBuffer(iVtxSize, vecMeshContainer[i]->vecPos.size(),
					pVertices, &tBufferInfo.tVB))
					return false;
			}
		}

		// 애니메이션이 없을 경우
		else
		{
			// Bump 정보가 있는지 판단한다.
			if (vecMeshContainer[i]->bBump)
			{
				vector<BUMPVERTEX>	vecVtx;

				for (int j = 0; j < vecMeshContainer[i]->vecPos.size(); ++j)
				{
					BUMPVERTEX	tVtx = {};

					tVtx.vPos = vecMeshContainer[i]->vecPos[j];
					tVtx.vNormal = vecMeshContainer[i]->vecNormal[j];
					tVtx.vUV = vecMeshContainer[i]->vecUV[j];
					tVtx.vTangent = vecMeshContainer[i]->vecTangent[j];
					tVtx.vBinormal = vecMeshContainer[i]->vecBinormal[j];

					vecVtx.push_back(tVtx);
				}

				pVertices = &vecVtx[0];
				iVtxSize = sizeof(BUMPVERTEX);

				if (!CreateVertexBuffer(iVtxSize, vecMeshContainer[i]->vecPos.size(),
					pVertices, &tBufferInfo.tVB))
					return false;
			}

			// 일반 정점일 경우
			else
			{
				vector<TEXVERTEX>	vecVtx;

				for (int j = 0; j < vecMeshContainer[i]->vecPos.size(); ++j)
				{
					TEXVERTEX	tVtx = {};

					tVtx.vPos = vecMeshContainer[i]->vecPos[j];
					tVtx.vNormal = vecMeshContainer[i]->vecNormal[j];
					tVtx.vUV = vecMeshContainer[i]->vecUV[j];

					vecVtx.push_back(tVtx);
				}

				pVertices = &vecVtx[0];
				iVtxSize = sizeof(TEXVERTEX);

				if (!CreateVertexBuffer(iVtxSize, vecMeshContainer[i]->vecPos.size(),
					pVertices, &tBufferInfo.tVB))
					return false;
			}
		}

		for (int j = 0; j < m_pLoader->GetSubsetCount(i); ++j)
		{
			INDEXBUFFER	tIB;
			if (!CreateIndexBuffer(sizeof(UINT), vecMeshContainer[i]->vecIndex[j].size(),
				&vecMeshContainer[i]->vecIndex[j][0], DXGI_FORMAT_R32_UINT,
				&tIB))
				return false;
			tBufferInfo.vecIB.push_back(tIB);
		}

		for (int j = 0; j < vecMeshContainer[i]->vecMaterial.size(); ++j)
		{
			CDx11Material*	pMaterial = new CDx11Material;

			if (!pMaterial->Init())
			{
				SAFE_RELEASE(pMaterial);
				return false;
			}

			pMaterial->SetInfo(vecMeshContainer[i]->vecMaterial[j]->vDif,
				vecMeshContainer[i]->vecMaterial[j]->vAmb,
				vecMeshContainer[i]->vecMaterial[j]->vSpc,
				vecMeshContainer[i]->vecMaterial[j]->vEmv,
				vecMeshContainer[i]->vecMaterial[j]->fSpcPower);

			// 텍스쳐 키를 만들어준다.
			char	strTexKey[256] = {};

			// diffuseTex
			if (!vecMeshContainer[i]->vecMaterial[j]->strDiffuseTex.empty())
			{
				sprintf_s(strTexKey, "%s_%dCon_%dSub_Dif", strKey.c_str(), i, j);
				pMaterial->SetDiffuseTexture(strTexKey,
					vecMeshContainer[i]->vecMaterial[j]->strDiffuseTex);
				pMaterial->SetDiffuseSampler(DEFAULT_SAMPLER);
			}

			// SpecularTex
			if (!vecMeshContainer[i]->vecMaterial[j]->strSpecularTex.empty())
			{
				sprintf_s(strTexKey, "%s_%dCon_%dSub_Spc", strKey.c_str(), i, j);
				pMaterial->SetSpecularTexture(strTexKey,
					vecMeshContainer[i]->vecMaterial[j]->strSpecularTex);
			}

			// NormalMap Tex
			if (!vecMeshContainer[i]->vecMaterial[j]->strNormalMapTex.empty())
			{
				sprintf_s(strTexKey, "%s_%dCon_%dSub_Nrm", strKey.c_str(), i, j);
				pMaterial->SetNormalMapTexture(strTexKey,
					vecMeshContainer[i]->vecMaterial[j]->strNormalMapTex);
			}

			tBufferInfo.vecMaterial.push_back(pMaterial);
		}

		m_vecBuffer.push_back(tBufferInfo);
	}

	return true;
}

bool CDx11Mesh::ExistingLoadMesh(const string & strKey, TCHAR * pFileName, const string & strPathKey)
{	
	//
	//m_vecBuffer=(this->m_vecBuffer);

	return true;
}


bool CDx11Mesh::CreateSphere(float fRadius, UINT iSliceCount, UINT iStackCount)
{
	vector<BUMPVERTEX>		vecVertices;
	vector<UINT>			vecIndices;

	BUMPVERTEX topVertex(0.0f, fRadius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	BUMPVERTEX bottomVertex(0.0f, -fRadius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

	vecVertices.push_back(topVertex);

	float phiStep = DX11_PI / iStackCount;
	float thetaStep = 2.0f*DX11_PI / iSliceCount;

	// Compute vertices for each stack ring (do not count the poles as rings).
	for (UINT i = 1; i <= iStackCount - 1; ++i)
	{
		float phi = i*phiStep;

		// Vertices of ring.
		for (UINT j = 0; j <= iSliceCount; ++j)
		{
			float theta = j*thetaStep;

			BUMPVERTEX v;

			// spherical to cartesian
			v.vPos.x = fRadius*sinf(phi)*cosf(theta);
			v.vPos.y = fRadius*cosf(phi);
			v.vPos.z = fRadius*sinf(phi)*sinf(theta);

			// Partial derivative of P with respect to theta
			v.vTangent.x = -fRadius*sinf(phi)*sinf(theta);
			v.vTangent.y = 0.0f;
			v.vTangent.z = fRadius*sinf(phi)*cosf(theta);

			XMVECTOR T = XMLoadFloat3(&v.vTangent);
			XMStoreFloat3(&v.vTangent, XMVector3Normalize(T));

			XMVECTOR p = XMLoadFloat3(&v.vPos);
			XMStoreFloat3(&v.vNormal, XMVector3Normalize(p));

			v.vUV.x = theta / XM_2PI;
			v.vUV.y = phi / XM_PI;

			vecVertices.push_back(v);
		}
	}

	vecVertices.push_back(bottomVertex);

	for (UINT i = 1; i <= iSliceCount; ++i)
	{
		vecIndices.push_back(0);
		vecIndices.push_back(i + 1);
		vecIndices.push_back(i);
	}

	UINT baseIndex = 1;
	UINT ringVertexCount = iSliceCount + 1;
	for (UINT i = 0; i < iStackCount - 2; ++i)
	{
		for (UINT j = 0; j < iSliceCount; ++j)
		{
			vecIndices.push_back(baseIndex + i*ringVertexCount + j);
			vecIndices.push_back(baseIndex + i*ringVertexCount + j + 1);
			vecIndices.push_back(baseIndex + (i + 1)*ringVertexCount + j);

			vecIndices.push_back(baseIndex + (i + 1)*ringVertexCount + j);
			vecIndices.push_back(baseIndex + i*ringVertexCount + j + 1);
			vecIndices.push_back(baseIndex + (i + 1)*ringVertexCount + j + 1);
		}
	}

	UINT southPoleIndex = (UINT)vecVertices.size() - 1;

	// Offset the indices to the index of the first vertex in the last ring.
	baseIndex = southPoleIndex - ringVertexCount;

	for (UINT i = 0; i < iSliceCount; ++i)
	{
		vecIndices.push_back(southPoleIndex);
		vecIndices.push_back(baseIndex + i);
		vecIndices.push_back(baseIndex + i + 1);
	}


	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	BUFFERINFO	tBufferInfo = {};
	INDEXBUFFER	tIB = {};

	CreateVertexBuffer(sizeof(BUMPVERTEX), vecVertices.size(),
		&vecVertices[0], &tBufferInfo.tVB);
	CreateIndexBuffer(sizeof(UINT), vecIndices.size(),
		&vecIndices[0], DXGI_FORMAT_R32_UINT, &tIB);

	tBufferInfo.vecIB.push_back(tIB);
	m_vecBuffer.push_back(tBufferInfo);

	m_iSubsetCount = 1;

	return true;
}

bool CDx11Mesh::CreateVertexBuffer(UINT iVtxSize, UINT iVtxCount,
	void * pVertices, PVERTEXBUFFER pVB)
{

	pVB->iVtxSize = iVtxSize;
	pVB->iVtxCount = iVtxCount;

	D3D11_BUFFER_DESC	tDesc = {};

	tDesc.ByteWidth = iVtxSize * iVtxCount;
	tDesc.Usage = D3D11_USAGE_DEFAULT;
	tDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA	tData = {};
	tData.pSysMem = pVertices;

	if (FAILED(DX11_DEVICE->CreateBuffer(&tDesc, &tData,
		&pVB->pBuffer)))
	{
		SAFE_RELEASE(pVB->pBuffer);
		return false;
	}

	return true;
}

bool CDx11Mesh::CreateIndexBuffer(UINT iIdxSize, UINT iIdxCount,
	void * pIndices, DXGI_FORMAT eIdxFmt, PINDEXBUFFER pIB)
{
	pIB->iIdxSize = iIdxSize;
	pIB->iIdxCount = iIdxCount;
	pIB->eIdxFmt = eIdxFmt;
	D3D11_BUFFER_DESC	tDesc = {};

	tDesc.ByteWidth = iIdxSize * iIdxCount;
	tDesc.Usage = D3D11_USAGE_DEFAULT;
	tDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA	tData = {};
	tData.pSysMem = pIndices;

	if (FAILED(DX11_DEVICE->CreateBuffer(&tDesc, &tData,
		&pIB->pBuffer)))
	{
		SAFE_RELEASE(pIB->pBuffer);
		return false;
	}

	return true;
}

void CDx11Mesh::Render()
{
	for (UINT i = 0; i < m_vecBuffer.size(); ++i)
	{
		PVERTEXBUFFER	pVB = &m_vecBuffer[i].tVB;
		for (int j = 0; j < m_vecBuffer[i].vecIB.size(); ++j)
		{
			PINDEXBUFFER	pIB = &m_vecBuffer[i].vecIB[j];

			UINT	iOffset = 0;
			DX11_CONTEXT->IASetPrimitiveTopology(m_eTopology);
			DX11_CONTEXT->IASetVertexBuffers(0, 1, &pVB->pBuffer, &pVB->iVtxSize, &iOffset);
			DX11_CONTEXT->IASetIndexBuffer(pIB->pBuffer, pIB->eIdxFmt, 0);
			DX11_CONTEXT->DrawIndexed(pIB->iIdxCount, 0, 0);
		}
	}
}

void CDx11Mesh::Render(int iContainer, int iSubset)
{
	PVERTEXBUFFER	pVB = &m_vecBuffer[iContainer].tVB;
	PINDEXBUFFER	pIB = &m_vecBuffer[iContainer].vecIB[iSubset];

	UINT	iOffset = 0;
	DX11_CONTEXT->IASetPrimitiveTopology(m_eTopology);
	DX11_CONTEXT->IASetVertexBuffers(0, 1, &pVB->pBuffer, &pVB->iVtxSize, &iOffset);
	DX11_CONTEXT->IASetIndexBuffer(pIB->pBuffer, pIB->eIdxFmt, 0);
	DX11_CONTEXT->DrawIndexed(pIB->iIdxCount, 0, 0);
}
