#include "Dx11Texture.h"
#include "../Core/Dx11FilePathManager.h"
#include "../Core/Dx11Device.h"

// 텍스쳐 관련 블로그
// http://m.blog.naver.com/sorkelf/220846504837

DX11_USING

CDx11Texture::CDx11Texture()	:
	m_pRSView(NULL),
	m_pTexture(NULL)
{
}


CDx11Texture::~CDx11Texture()
{
	SAFE_RELEASE(m_pRSView);
	SAFE_RELEASE(m_pTexture);
}

bool CDx11Texture::LoadTexture(wchar_t * pFileName, const string & strPathKey)
{	
//	파일이름에서 확장자만 추출
	int iLength = lstrlen(pFileName);
	wchar_t	strExt[16] = {};

	for (int i = iLength - 1; i >= 0; --i)
	{
		if (pFileName[i] == '.')
		{
			lstrcpy(strExt, &pFileName[i + 1]);
			break;
		}
	}

//	풀 경로를 만들어준다.
	wstring strPath = DX11_GET_SINGLE(CDx11FilePathManager)->FindPath(strPathKey);
	strPath += pFileName;

	if (lstrcmp(strExt, L"dds") == 0 ||
		lstrcmp(strExt, L"DDS") == 0)
	{
		if (FAILED(LoadFromDDSFile(strPath.c_str(), DDS_FLAGS_NONE,
			NULL, m_tImage)))
			return false;
	}
	else if (lstrcmp(strExt, L"tga") == 0 ||
		lstrcmp(strExt, L"TGA") == 0)
	{
		if (FAILED(LoadFromTGAFile(strPath.c_str(), NULL, m_tImage)))
			return false;
	}
	else
	{
		if (FAILED(LoadFromWICFile(strPath.c_str(), WIC_FLAGS_NONE, NULL,
			m_tImage)))
			return false;
	}

	if (FAILED(CreateShaderResourceView(DX11_DEVICE, m_tImage.GetImages(),
		m_tImage.GetImageCount(), m_tImage.GetMetadata(), &m_pRSView)))
		return false;

	return true;
}

bool CDx11Texture::LoadTexture(const string & strPath)
{
	int	iLength = strPath.length();

	char	strExt[16] = {};

	for (int i = iLength - 1; i >= 0; --i)
	{
		if (strPath[i] == '.')
		{
			strcpy_s(strExt, strPath.c_str() + (i + 1));
			break;
		}
	}

	wchar_t	strName[MAX_PATH] = {};

	MultiByteToWideChar(CP_ACP, 0, strPath.c_str(), -1, strName, strPath.length() * 2);

	if (strcmp(strExt, "dds") == 0 ||
		strcmp(strExt, "DDS") == 0)
	{
		if (FAILED(LoadFromDDSFile(strName, DDS_FLAGS_NONE,
			NULL, m_tImage)))
			return false;
	}

	else if (strcmp(strExt, "tga") == 0 ||
		strcmp(strExt, "TGA") == 0)
	{
		if (FAILED(LoadFromTGAFile(strName, NULL, m_tImage)))
			return false;
	}

	else
	{
		if (FAILED(LoadFromWICFile(strName, WIC_FLAGS_NONE,
			NULL, m_tImage)))
			return false;
	}

	if (FAILED(CreateShaderResourceView(DX11_DEVICE, m_tImage.GetImages(),
		m_tImage.GetImageCount(), m_tImage.GetMetadata(), &m_pRSView)))
		return false;

	return true;
}

bool CDx11Texture::CreateTexture(UINT iWidth, UINT iHeight, UINT iArraySize, DXGI_FORMAT eFmt, D3D11_USAGE eUsage, D3D11_BIND_FLAG eBindFlag, int iCpuFlag)
{
	m_tImage.Initialize2D(eFmt, iWidth, iHeight, iArraySize, 1);

	if (FAILED(CreateShaderResourceViewEx(DX11_DEVICE, m_tImage.GetImages(),
		m_tImage.GetImageCount(), m_tImage.GetMetadata(),
		eUsage, eBindFlag, iCpuFlag, 0, false, &m_pRSView)))
		return false;

	return true;
}

void CDx11Texture::SetTexture(UINT iRegister)
{
	DX11_CONTEXT->PSSetShaderResources(iRegister, 1, &m_pRSView);
}

void CDx11Texture::SetData(void * pData, UINT iSize)
{
	ID3D11Texture2D*	pTexture = NULL;
	m_pRSView->GetResource((ID3D11Resource**)&pTexture);

	D3D11_MAPPED_SUBRESOURCE	tMap;
	DX11_CONTEXT->Map(pTexture, 0, D3D11_MAP_WRITE, 0, &tMap);

	memcpy(tMap.pData, pData, iSize);

	DX11_CONTEXT->Unmap(pTexture, 0);

	SAFE_RELEASE(pTexture);
}
