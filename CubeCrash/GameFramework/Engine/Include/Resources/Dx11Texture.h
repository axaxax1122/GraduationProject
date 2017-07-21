#pragma once
#include "Dx11Resources.h"

DX11_BEGIN

class DX11_DLL CDx11Texture :
	public CDx11Resources
{
private:
	friend class CDx11ResourcesManager;
	friend class CDx11Animation3D;

private:
	CDx11Texture();
	~CDx11Texture();

private:
	ScratchImage				m_tImage;
	ID3D11ShaderResourceView*	m_pRSView;
	ID3D11Texture2D*			m_pTexture;

public:
	bool LoadTexture(wchar_t* pFileName, const string& strPathKey = TEXTURE_PATH);
	bool LoadTexture(const string& strPath);
	bool CreateTexture(UINT iWidth, UINT iHeight, UINT iArraySize,
		DXGI_FORMAT eFmt, D3D11_USAGE eUsage,
		D3D11_BIND_FLAG eBindFlag,
		int iCpuFlag);
	void SetTexture(UINT iRegister = 0);
	void SetData(void *pData, UINT iSize);
};

DX11_END
