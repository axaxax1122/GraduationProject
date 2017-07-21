#pragma once
#include "../Dx11Object.h"

DX11_BEGIN

class DX11_DLL CDx11ResourcesManager :
	public CDx11Object
{
	DX11_DECLARE_SINGLE(CDx11ResourcesManager)

private:
	unordered_map<string, class CDx11Mesh*>		m_mapMesh;
	unordered_map<string, class CDx11Texture*>	m_mapTexture;
	unordered_map<string, class CDx11Sampler*>	m_mapSampler;

public:
	bool Init();
	
//	매쉬
	class CDx11Mesh* CreateMesh(const string& strKey, UINT iVtxSize,
		UINT iVtxCount, void* pVertices, UINT iIdxSize,
		UINT iIdxCount, void* pIndices, DXGI_FORMAT	eIdxFmt,
		D3D11_PRIMITIVE_TOPOLOGY eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	class CDx11Mesh* LoadMesh(const string& strKey, wchar_t* pFileName,
		const string& strPathKey = MESH_PATH);
	class CDx11Mesh* LoadMesh(const string& strKey, wchar_t* pFileName,
		vector<vector<wchar_t*>>& vecTexName, const string& strPathKey = MESH_PATH);
	class CDx11Mesh* CreateSphere(const string& strKey, float fRadius,
		UINT iSliceCount, UINT iStackCount);
	class CDx11Mesh* FindMesh(const string& strKey);

//	텍스쳐
	class CDx11Texture* LoadTexture(const string& strKey, wchar_t* pFileName,
		const string& strPathKey = TEXTURE_PATH);
	class CDx11Texture* LoadTexture(const string& strKey, string strPath);
	class CDx11Texture* FindTexture(const string& strKey);
	class CDx11Texture*	CreateTexture(const string& strKey, 
		UINT iWidth, UINT iHeight, UINT iArraySize, 
		DXGI_FORMAT eFmt, D3D11_USAGE eUsage,
		D3D11_BIND_FLAG eBindFlag, int iCpuFlag);

//	샘플러
	class CDx11Sampler* CreateSampler(const string& strKey,
		D3D11_FILTER eFilter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_MODE u = D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_MODE v = D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_MODE w = D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_COMPARISON_FUNC eFunc = D3D11_COMPARISON_NEVER);
	class CDx11Sampler* FindSampler(const string& strKey);
};

DX11_END