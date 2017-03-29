#include "Dx11Mesh.h"
#include "../Core/Dx11Device.h"

DX11_USING

CDx11Mesh::CDx11Mesh()	:
	m_pVB(NULL),
	m_pIB(NULL)
{
	SetTypeName<CDx11Mesh>();
}

CDx11Mesh::~CDx11Mesh()
{
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
}

bool CDx11Mesh::CreateMesh(UINT iVtxSize, UINT iVtxCount, 
	void * pVertices, UINT iIdxSize, UINT iIdxCount, 
	void * pIndices, DXGI_FORMAT eIdxFmt,
	D3D11_PRIMITIVE_TOPOLOGY eTopology)
{
	m_iVtxSize = iVtxSize;
	m_iVtxCount = iVtxCount;
	m_iIdxSize = iIdxSize;
	m_iIdxCount = iIdxCount;
	m_eIdxFmt = eIdxFmt;
	m_eTopology = eTopology;

	if (!CreateVertexBuffer(iVtxSize, iVtxCount, pVertices))
		return false;
	
	if (!CreateIndexBuffer(iIdxSize, iIdxCount, pIndices, eIdxFmt))
		return false;

	return true;
}

bool CDx11Mesh::CreateVertexBuffer(UINT iVtxSize, UINT iVtxCount,
	void * pVertices)
{
	D3D11_BUFFER_DESC	tDesc = {};

	tDesc.ByteWidth = iVtxSize * iVtxCount;
	tDesc.Usage = D3D11_USAGE_DEFAULT;
	tDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA	tData = {};
	tData.pSysMem = pVertices;

	if (FAILED(DX11_DEVICE->CreateBuffer(&tDesc, &tData,
		&m_pVB)))
	{
		SAFE_RELEASE(m_pVB);
		return false;
	}

	return true;
}

bool CDx11Mesh::CreateIndexBuffer(UINT iIdxSize, UINT iIdxCount,
	void * pIndices, DXGI_FORMAT eIdxFmt)
{
	D3D11_BUFFER_DESC	tDesc = {};

	tDesc.ByteWidth = iIdxSize * iIdxCount;
	tDesc.Usage = D3D11_USAGE_DEFAULT;
	tDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA	tData = {};
	tData.pSysMem = pIndices;

	if (FAILED(DX11_DEVICE->CreateBuffer(&tDesc, &tData,
		&m_pIB)))
	{
		SAFE_RELEASE(m_pIB);
		return false;
	}

	return true;
}

void CDx11Mesh::Render()
{
	UINT	iOffset = 0;
	DX11_CONTEXT->IASetPrimitiveTopology(m_eTopology);
	DX11_CONTEXT->IASetVertexBuffers(0, 1, &m_pVB, &m_iVtxSize, &iOffset);
	DX11_CONTEXT->IASetIndexBuffer(m_pIB, m_eIdxFmt, 0);
	DX11_CONTEXT->DrawIndexed(m_iIdxCount, 0, 0);
}
