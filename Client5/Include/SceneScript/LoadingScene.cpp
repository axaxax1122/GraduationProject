#include "LoadingScene.h"
#include "Scene/Dx11Layer.h"
#include "Scene/Dx11Scene.h"
#include "Component/Dx11Transform.h"
#include "GameObject/Dx11GameObject.h"
#include "Component/Dx11Renderer.h"
#include "Component/Dx11UIBackground.h"
#include "Component/Dx11Material.h"
#include "Core/Dx11Device.h"
#include "MainScene.h"
#include "Scene/Dx11SceneManager.h"
#include "../NetWork.h"
#include "../Network/NetworkManager.h"
#include "Core/Dx11SoundManager.h"

HANDLE logoEvent;

CLoadingScene::CLoadingScene()
{
	m_eState = FADE_NONE;
	m_fFadeAmount = 0.f;
	logoEvent = CreateEvent(NULL, false, false, NULL);
}


CLoadingScene::~CLoadingScene()
{
	CloseHandle(logoEvent);
}

bool CLoadingScene::Init()
{
	DX11_GET_SINGLE(Dx11SoundManager)->InitSound();
	DX11_GET_SINGLE(Dx11SoundManager)->PlayLoding();

	CDx11Layer*	pLayer = m_pScene->FindLayer("UILayer");

	if (!pLayer)
		return false;

	// ¹è°æ UI
	CDx11GameObject*	pBackground = CDx11GameObject::Create("LoadingBackObj");
	CDx11Transform*		pTransform = pBackground->GetTransform();
	RESOLUTION_TYPE		eType = DX11_RESOLUTION;
	pTransform->SetWorldScale(g_tRS[eType].iWidth, g_tRS[eType].iHeight, 1.f);
	SAFE_RELEASE(pTransform);


	CDx11Renderer*	pRenderer = pBackground->AddComponent<CDx11Renderer>("BackRenderer");
	pRenderer->SetMesh(RECT_UI_MESH);
	pRenderer->SetShader(UI_SHADER);
	pRenderer->SetRenderState(DX11_RS_DEPTH_DISABLE);
	pRenderer->CreateMaterial("LoadingBack", L"UI\\Main2.png");

	CDx11Material* pMtrl = pRenderer->GetMaterial();
	pMtrl->SetFadeAmount(0.f);
	SAFE_RELEASE(pMtrl);
	SAFE_RELEASE(pRenderer);

	CDx11UIBackground*	pUIBack = pBackground->AddComponent<CDx11UIBackground>("UIBack");
	SAFE_RELEASE(pUIBack);

	pLayer->AddObject(pBackground);
	SAFE_RELEASE(pBackground);


	SAFE_RELEASE(pLayer);
	return true;
}

void CLoadingScene::Input(float fTime)
{
}

int CLoadingScene::Update(float fTime)
{
	static CDx11Scene* pScene;
	static CMainScene* pMainScene;

	if (m_eState == FADE_NONE)
	{
		m_fFadeAmount = 0.f;
		m_eState = FADE_IN_START;

	}
	else if (m_eState == FADE_OUT_START)
	{
		FadeOut(fTime);

	}
	else if (m_eState == FADE_IN_START)
	{
		FadeIn(fTime);

	}
	else if (m_eState == FADE_IN_END)
	{
		m_eState = FADE_OUT_START;
		pScene = DX11_GET_SINGLE(CDx11SceneManager)->CreateScene("MainScene");
		pMainScene = new CMainScene;
		pScene->AddSceneScript(pMainScene, "MainScene");
		pMainScene->LoadResource();
		//		pMainScene->LoadMapResource();

	}
	else if (m_eState == FADE_OUT_END)
	{
		DX11_GET_SINGLE(Dx11SoundManager)->ReleaseSound();

		if (!pMainScene->Init())
			return 0;

		CNetworkManager::GetInst()->GetInGameNetwork()->SetMainScene(pMainScene);
		SetEvent(logoEvent);
		SAFE_RELEASE(pMainScene);

		
		DX11_GET_SINGLE(CDx11SceneManager)->ChangeScene(pScene);
		
	}
	return 0;
}

int CLoadingScene::LateUpdate(float fTime)
{
	return 0;
}

void CLoadingScene::Render(float fTime)
{

}

CLoadingScene * CLoadingScene::Clone()
{
	return new CLoadingScene(*this);
}

void CLoadingScene::FadeOut(float fTime)
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

void CLoadingScene::FadeIn(float fTime)
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
