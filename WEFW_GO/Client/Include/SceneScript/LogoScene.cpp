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
#include "MainScene.h"
#include "Core/Dx11Device.h"
#include "Component/Dx11UIBackground.h"
#include "Component/Dx11UIButton.h"
#include "Resources/Dx11ResourcesManager.h"
#include "Resources/Dx11Mesh.h"


CLogoScene::CLogoScene()
{
	m_eState = FADE_IN_START;
	m_fFadeAmount = 0.f;
}


CLogoScene::~CLogoScene()
{
}

bool CLogoScene::Init()
{
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
	pRenderer->CreateMaterial("LogoBack", L"UI\\Main_Menu_Overlay.png");
	SAFE_RELEASE(pRenderer);

	CDx11UIBackground*	pUIBack = pBackground->AddComponent<CDx11UIBackground>("UIBack");
	SAFE_RELEASE(pUIBack);

	pLayer->AddObject(pBackground);
	SAFE_RELEASE(pBackground);

	// 시작 버튼 UI
	CDx11GameObject*	pStartButton = CDx11GameObject::Create("StartButtonObj");
	pTransform = pStartButton->GetTransform();
	pTransform->SetWorldPos(590.f, 500.f, 0.f);
	pTransform->SetWorldScale(100.f, 50.f, 1.f);
	SAFE_RELEASE(pTransform);

	CDx11UIButton*	pButton = pStartButton->AddComponent<CDx11UIButton>("Button");
	pButton->SetCallBack(this, &CLogoScene::StartCallBack);
	pButton->SetButtonSize(Vec2(100.f, 50.f));
	SAFE_RELEASE(pButton);

	pLayer->AddObject(pStartButton);
	SAFE_RELEASE(pStartButton);

	SAFE_RELEASE(pLayer);
	return true;
}

void CLogoScene::Input(float fTime)
{
}

int CLogoScene::Update(float fTime)
{
	if (m_eState == FADE_OUT_START)
	{
		FadeOut(fTime);
	
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

	return 0;
}

void CLogoScene::FadeOut(float fTime)
{
	CDx11Layer*	pLayer = m_pScene->FindLayer("UILayer");

	list<CDx11GameObject*>*	pObjList = pLayer->GetGameObjectList();
	list<CDx11GameObject*>::iterator	iter;
	list<CDx11GameObject*>::iterator	iterEnd = pObjList->end();

	for (iter = pObjList->begin(); iter != iterEnd; ++iter)
	{
		CDx11Renderer*	pRenderer = (CDx11Renderer*)(*iter)->FindComponentFromType(CT_RENDERER);
		CDx11Material*	pMtrl = pRenderer->GetMaterial();

		pMtrl->SetFadeAmount(m_fFadeAmount);

		SAFE_RELEASE(pMtrl);
		SAFE_RELEASE(pRenderer);
	}

	if (m_fFadeAmount > 0.f)
	{
		m_fFadeAmount -= fTime;
	}
	else
	{
		m_eState = FADE_OUT_END;
		CDx11Scene*	pScene = DX11_GET_SINGLE(CDx11SceneManager)->CreateScene("MainScene");
		CMainScene* pMainScene = pScene->CreateSceneScript<CMainScene>("MainScene");
		SAFE_RELEASE(pMainScene);
		DX11_GET_SINGLE(CDx11SceneManager)->ChangeScene(pScene);
	}

	SAFE_RELEASE(pLayer);
}

void CLogoScene::FadeIn(float fTime)
{
	CDx11Layer*	pLayer = m_pScene->FindLayer("UILayer");

	list<CDx11GameObject*>*	pObjList = pLayer->GetGameObjectList();
	list<CDx11GameObject*>::iterator	iter;
	list<CDx11GameObject*>::iterator	iterEnd = pObjList->end();

	for (iter = pObjList->begin(); iter != iterEnd; ++iter)
	{
		CDx11Renderer*	pRenderer = (CDx11Renderer*)(*iter)->FindComponentFromType(CT_RENDERER);
		CDx11Material*	pMtrl = pRenderer->GetMaterial();

		pMtrl->SetFadeAmount(m_fFadeAmount);

		SAFE_RELEASE(pMtrl);
		SAFE_RELEASE(pRenderer);
	}

	if (m_fFadeAmount < 1.f)
	{
		m_fFadeAmount += fTime;
	}
	else
	{
		m_eState = FADE_IN_END;
	}

	SAFE_RELEASE(pLayer);
}
