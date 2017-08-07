#include "LogoScene.h"
#include "GameObject/Dx11GameObject.h"
#include "Component/Dx11Transform.h"
#include "Component/Dx11Renderer.h"
#include "Scene/Dx11SceneManager.h"
#include "Scene/Dx11Scene.h"
#include "Scene/Dx11Layer.h"
#include "Resources/Dx11ResourcesManager.h"
#include "Resources/Dx11Texture.h"
#include "Component/Dx11Material.h"
#include "LoadingScene.h"
#include "MainScene.h"
#include "Core/Dx11Device.h"
#include "Component/Dx11UIBackground.h"
#include "Component/Dx11UIButton.h"
#include "Resources/Dx11ResourcesManager.h"
#include "Resources/Dx11Mesh.h"
#include "Scene/QuadTree.h"
#include "Core/Dx11SoundManager.h"

CLogoScene::CLogoScene()
{
	m_eState = FADE_NONE;
	m_eFlag = FLAG_NONE;
	m_fFadeAmount = 0.f;
}


CLogoScene::~CLogoScene()
{
}

bool CLogoScene::Init()
{
	DX11_GET_SINGLE(Dx11SoundManager)->InitSound();
	DX11_GET_SINGLE(Dx11SoundManager)->PlayBGM1();

	CDx11Layer*	pLayer = m_pScene->FindLayer("UILayer");

	if (!pLayer)
		return false;

	// 배경 UI
	CDx11GameObject*	pBackground = CDx11GameObject::Create("BackObj");
	CDx11Transform*		pTransform = pBackground->GetTransform();
	RESOLUTION_TYPE		eType = DX11_RESOLUTION;
	pTransform->SetWorldScale(g_tRS[eType].iWidth, g_tRS[eType].iHeight, 1.f);
	SAFE_RELEASE(pTransform);


	CDx11Renderer*	pRenderer = pBackground->AddComponent<CDx11Renderer>("BackRenderer");
	pRenderer->SetMesh(RECT_UI_MESH);
	pRenderer->SetShader(UI_SHADER);
	pRenderer->SetRenderState(DX11_RS_DEPTH_DISABLE);
	pRenderer->CreateMaterial("LogoBack", L"UI\\Background3.png");

	CDx11Material* pMtrl = pRenderer->GetMaterial();
	pMtrl->SetFadeAmount(0.f);
	SAFE_RELEASE(pMtrl);
	SAFE_RELEASE(pRenderer);


	CDx11UIBackground*	pUIBack = pBackground->AddComponent<CDx11UIBackground>("UIBack");
	SAFE_RELEASE(pUIBack);

	pLayer->AddObject(pBackground);
	SAFE_RELEASE(pBackground);

	// 시작 버튼 UI
	CDx11GameObject*	pStartButton = CDx11GameObject::Create("StartButtonObj");
	pTransform = pStartButton->GetTransform();
	pTransform->SetWorldPos(590.f, 290.f, 0.f);
	pTransform->SetWorldScale(100.f, 50.f, 1.f);
	SAFE_RELEASE(pTransform);

	CDx11UIButton*	pButton = pStartButton->AddComponent<CDx11UIButton>("Button");
	pButton->SetCallBack(this, &CLogoScene::StartCallBack);
	pButton->SetButtonSize(Vec2(100.f, 50.f));

	pRenderer = (CDx11Renderer*)pStartButton->FindComponentFromType(CT_RENDERER);
	pRenderer->CreateMaterial("PlayButton", L"UI\\Play.png", 0, TEXTURE_PATH);
	SAFE_RELEASE(pRenderer);

	pButton->AddButtonTexture(BS_NORMAL, "PlayButton");
	SAFE_RELEASE(pButton);

	pLayer->AddObject(pStartButton);
	SAFE_RELEASE(pStartButton);



	// 프로필 버튼 UI
	CDx11GameObject*	pProfileButton = CDx11GameObject::Create("ProfileButtonObj");
	pTransform = pProfileButton->GetTransform();
	pTransform->SetWorldPos(590.f, 420.f, 0.f);
	pTransform->SetWorldScale(100.f, 50.f, 1.f);
	SAFE_RELEASE(pTransform);

	pButton = pProfileButton->AddComponent<CDx11UIButton>("Button");
	//	pButton->SetCallBack(this, &CLogoScene::StartCallBack);
	pButton->SetButtonSize(Vec2(100.f, 50.f));

	pRenderer = (CDx11Renderer*)pProfileButton->FindComponentFromType(CT_RENDERER);
	pRenderer->CreateMaterial("ProfileButton", L"UI\\Profile.png", 0, TEXTURE_PATH);
	SAFE_RELEASE(pRenderer);

	pButton->AddButtonTexture(BS_NORMAL, "ProfileButton");
	SAFE_RELEASE(pButton);

	pLayer->AddObject(pProfileButton);
	SAFE_RELEASE(pProfileButton);



	// 옵션 버튼 UI
	CDx11GameObject*	pOptionButton = CDx11GameObject::Create("OptionButtonObj");
	pTransform = pOptionButton->GetTransform();
	pTransform->SetWorldPos(590.f, 500.f, 0.f);
	pTransform->SetWorldScale(100.f, 50.f, 1.f);
	SAFE_RELEASE(pTransform);

	pButton = pOptionButton->AddComponent<CDx11UIButton>("Button");
	//	pButton->SetCallBack(this, &CLogoScene::StartCallBack);
	pButton->SetButtonSize(Vec2(100.f, 50.f));

	pRenderer = (CDx11Renderer*)pOptionButton->FindComponentFromType(CT_RENDERER);
	pRenderer->CreateMaterial("OptionButton", L"UI\\Option.png", 0, TEXTURE_PATH);
	SAFE_RELEASE(pRenderer);

	pButton->AddButtonTexture(BS_NORMAL, "OptionButton");
	SAFE_RELEASE(pButton);

	pLayer->AddObject(pOptionButton);
	SAFE_RELEASE(pOptionButton);

	// 종료 버튼 UI
	CDx11GameObject*	pQuitButton = CDx11GameObject::Create("QuitButtonObj");
	pTransform = pQuitButton->GetTransform();
	pTransform->SetWorldPos(590.f, 580.f, 0.f);
	pTransform->SetWorldScale(100.f, 50.f, 1.f);
	SAFE_RELEASE(pTransform);

	pButton = pQuitButton->AddComponent<CDx11UIButton>("Button");
	pButton->SetCallBack(this, &CLogoScene::ExitCallBack);
	pButton->SetButtonSize(Vec2(100.f, 50.f));

	pRenderer = (CDx11Renderer*)pQuitButton->FindComponentFromType(CT_RENDERER);
	pRenderer->CreateMaterial("QuitButton", L"UI\\Exit.png", 0, TEXTURE_PATH);
	SAFE_RELEASE(pRenderer);

	pButton->AddButtonTexture(BS_NORMAL, "QuitButton");
	SAFE_RELEASE(pButton);

	pLayer->AddObject(pQuitButton);
	SAFE_RELEASE(pQuitButton);



	SAFE_RELEASE(pLayer);
	return true;
}

void CLogoScene::Input(float fTime)
{
	static bool		bReturn;

	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		bReturn = true;
	}
	else if (bReturn)
	{
		bReturn = false;
		m_eState = FADE_OUT_START;
		m_eFlag = FLAG_START;
	}
}

int CLogoScene::Update(float fTime)
{
	
	if (m_eState == FADE_NONE)
	{
		m_fFadeAmount = 0.f;
		m_eState = FADE_IN_START;
		

	}
	else if (m_eState == FADE_OUT_START)
	{
		FadeOut(fTime);
		static int temp = 0;
		if (temp == 0) {
			temp++;
			DX11_GET_SINGLE(Dx11SoundManager)->ReleaseSound();
		}

	}
	else if (m_eState == FADE_IN_START)
	{
		
		FadeIn(fTime);
		
	}

	return 0;
}

int CLogoScene::LateUpdate(float fTime)
{
	return 0;
}

void CLogoScene::Render(float fTime)
{

}

CLogoScene * CLogoScene::Clone()
{
	return new CLogoScene(*this);
}

int CLogoScene::StartCallBack(CDx11UIButton * pBtn, float fTime)
{
	m_eState = FADE_OUT_START;
	m_eFlag = FLAG_START;
	return 0;
}

int CLogoScene::ExitCallBack(CDx11UIButton * pBtn, float fTime)
{
	m_eState = FADE_OUT_START;
	m_eFlag = FLAG_EXIT;
	return 0;
}

void CLogoScene::FadeOut(float fTime)
{
	if (fTime > 0.01f)
		fTime = 0.01f;

	if (m_fFadeAmount > 0.f)
	{
		m_fFadeAmount -= fTime;
	}
	else
	{
		m_eState = FADE_OUT_END;
		if (m_eFlag == FLAG_START)
			GameStart();
		else if (m_eFlag == FLAG_EXIT)
			GameExit();
	}

	CDx11Layer*	pLayer = m_pScene->FindLayer("UILayer");

	list<CDx11GameObject*>*	pObjList = pLayer->GetGameObjectList();
	list<CDx11GameObject*>::iterator	iter;
	list<CDx11GameObject*>::iterator	iterEnd = pObjList->end();

	for (iter = pObjList->begin(); iter != iterEnd; ++iter)
	{
		CDx11Renderer*	pRenderer = (CDx11Renderer*)(*iter)->FindComponentFromType(CT_RENDERER);
		CDx11Material*	pMtrl = pRenderer->GetMaterial();

		if (pMtrl)
			pMtrl->SetFadeAmount(m_fFadeAmount);

		SAFE_RELEASE(pMtrl);
		SAFE_RELEASE(pRenderer);
	}

	SAFE_RELEASE(pLayer);
}

void CLogoScene::FadeIn(float fTime)
{
	if (fTime > 0.01f)
		fTime = 0.01f;


	if (m_fFadeAmount < 1.f)
	{
		m_fFadeAmount += fTime;
	}
	else
	{
		m_eState = FADE_IN_END;
		m_fFadeAmount = 1.f;
	}

	CDx11Layer*	pLayer = m_pScene->FindLayer("UILayer");

	list<CDx11GameObject*>*	pObjList = pLayer->GetGameObjectList();
	list<CDx11GameObject*>::iterator	iter;
	list<CDx11GameObject*>::iterator	iterEnd = pObjList->end();

	for (iter = pObjList->begin(); iter != iterEnd; ++iter)
	{
		CDx11Renderer*	pRenderer = (CDx11Renderer*)(*iter)->FindComponentFromType(CT_RENDERER);
		CDx11Material*	pMtrl = pRenderer->GetMaterial();

		if (pMtrl)
			pMtrl->SetFadeAmount(m_fFadeAmount);

		SAFE_RELEASE(pMtrl);
		SAFE_RELEASE(pRenderer);
	}

	SAFE_RELEASE(pLayer);
}

void CLogoScene::GameStart()
{
	CDx11Scene*	pScene = DX11_GET_SINGLE(CDx11SceneManager)->CreateScene("LoadingScene");
	CLoadingScene* pLoadingScene = new CLoadingScene;

	pScene->AddSceneScript(pLoadingScene, "LoadingScene");
	if (!pLoadingScene->Init())
		return;

	SAFE_RELEASE(pLoadingScene);
	DX11_GET_SINGLE(CDx11SceneManager)->ChangeScene(pScene);
}

void CLogoScene::GameExit()
{
	DestroyWindow(DX11_GET_SINGLE(CDx11Device)->GetHWND());
}
