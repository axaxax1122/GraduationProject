#include "ClientCore.h"
#include "Dx11Core.h"
#include "SceneScript\LogoScene.h"
#include "Scene/Dx11Scene.h"
#include "Scene/Dx11SceneManager.h"

DX11_USING

DX11_DEFINITION_SINGLE(CClientCore)

CClientCore::CClientCore()
{
}

CClientCore::~CClientCore()
{
	DESTROY_SINGLE(CNetworkManager);
	DX11_DESTROY_SINGLE(CDx11Core);
}


const char * CClientCore::GetIP() const
{
	return m_strIP;
}

short CClientCore::GetPort() const
{
	return m_sPort;
}

const char * CClientCore::GetID() const
{
	return m_strID;
}

const char * CClientCore::GetPass() const
{
	return m_strPass;
}

bool CClientCore::Init(wchar_t* pTitle, wchar_t* pClass, HINSTANCE hInst,
	int iIconID, int iSmallIconID, RESOLUTION_TYPE eRT,
	bool bWindowMode, WNDPROC pProc)
{


#ifdef _DEBUG
	SetCurrentDirectory(L"../../Client/Bin_Debug/");
#else
	SetCurrentDirectory(L"../../Client/Bin/");
#endif // _DEBUG

	// 로그인 정보 파일을 읽어온다.
	FILE*	pFile = NULL;

	fopen_s(&pFile, "LoginInfo.lif", "rb");

	if (pFile)
	{
		memset(m_strIP, 0, 16);
		memset(m_strID, 0, ID_SIZE);
		memset(m_strPass, 0, PASS_SIZE);

		fread(m_strIP, 16, 1, pFile);
		fread(&m_sPort, 2, 1, pFile);

		int	iLength;
		fread(&iLength, 4, 1, pFile);
		fread(m_strID, iLength, 1, pFile);
		fread(&iLength, 4, 1, pFile);
		fread(m_strPass, iLength, 1, pFile);

		fclose(pFile);
	}

	else
	{
		//strcpy_s(m_strIP, "192.168.35.151");
		//strcpy_s(m_strIP, "192.168.10.6");
		strcpy_s(m_strIP, "127.0.0.1");
		//strcpy_s(m_strIP, "192.168.35.110"); //현규
		//strcpy_s(m_strIP, "192.168.35.110"); //제선
		m_sPort = 4551;
		strcpy_s(m_strID, "KHK3391");
		strcpy_s(m_strPass, "AP3391");
	}

	// 로그인정보 파일을 지운다. 런처에서 넘어오는 파일이다.
	int		iResult = remove("LoginInfo.lif");

	if (!DX11_GET_SINGLE(CDx11Core)->Init(pTitle, pClass,
		hInst, IDI_ICON1, IDI_ICON1, eRT, bWindowMode, pProc))
	{
		DX11_DESTROY_SINGLE(CDx11Core);
		return 0;
	}

	CDx11Scene*	pScene = DX11_GET_SINGLE(CDx11SceneManager)->GetCurrentScene();

	CLogoScene*	pLogoScene = pScene->CreateSceneScript<CLogoScene>("LogoScene");

	SAFE_RELEASE(pLogoScene);

	SAFE_RELEASE(pScene);

	return true;
}

int CClientCore::Run()
{
	int	iRet = DX11_GET_SINGLE(CDx11Core)->Run();

	return iRet;
}
