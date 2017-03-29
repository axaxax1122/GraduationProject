#pragma once
#include "Dx11Resource.h"

DX11_BEGIN

typedef struct _tagVertexBuffer
{
	ID3D11Buffer*		pBuffer;
	UINT				iVtxSize;
	UINT				iVtxCount;
}VERTEXBUFFER, *PVERTEXBUFFER;

typedef struct _tagIndexBuffer
{
	ID3D11Buffer*		pBuffer;
	UINT				iIdxSize;
	UINT				iIdxCount;
	DXGI_FORMAT			eIdxFmt;
}INDEXBUFFER, *PINDEXBUFFER;

typedef struct _tagBufferInfo
{
	VERTEXBUFFER	tVB;
	vector<INDEXBUFFER>		vecIB;
	vector<class CDx11Material*>	vecMaterial;

	_tagBufferInfo() :
		tVB({})
	{
	}
}BUFFERINFO, *PBUFFERINFO;

class DX11_DLL CDx11Mesh :
	public CDx11Resource
{
private:
	friend class CDx11ResourcesManager;

private:
	CDx11Mesh();
	CDx11Mesh(CDx11Mesh *Mesh);
	~CDx11Mesh();

private:
	vector<BUFFERINFO>			m_vecBuffer;
	UINT						m_iContainerCount;
	UINT						m_iSubsetCount;
	D3D11_PRIMITIVE_TOPOLOGY	m_eTopology;
	class CDx11FbxLoader*	m_pLoader;

public:
	UINT GetContainerCount();
	UINT GetSubsetCount(UINT iContainer);
	CDx11Material* CloneMaterial(int iContainer, int iSubset);
	bool IsMaterial();
	vector<BUFFERINFO> GetBufferInfo();
	void SetBufferInfo(vector<BUFFERINFO> Bufferinfo);
public:
	bool CreateMesh(UINT iVtxSize,
		UINT iVtxCount, void* pVertices, UINT iIdxSize,
		UINT iIdxCount, void* pIndices, DXGI_FORMAT eIdxFmt,
		D3D11_PRIMITIVE_TOPOLOGY eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	bool LoadMesh(const string& strKey, TCHAR* pFileName, const string& strPathKey = MESH_PATH);
	bool ExistingLoadMesh(const string& strKey, TCHAR* pFileName, const string& strPathKey = MESH_PATH);
	bool CreateSphere(float fRadius, UINT iSliceCount, UINT iStackCount);
	void Render();
	void Render(int iContainer, int iSubset);

private:
	bool CreateVertexBuffer(UINT iVtxSize,
		UINT iVtxCount, void* pVertices, PVERTEXBUFFER pVB);
	bool CreateIndexBuffer(UINT iIdxSize,
		UINT iIdxCount, void* pIndices, DXGI_FORMAT	eIdxFmt, PINDEXBUFFER pIB);
};

DX11_END