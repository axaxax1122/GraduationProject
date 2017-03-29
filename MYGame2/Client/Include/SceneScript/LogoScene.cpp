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
#include "Component/Dx11UIBack.h"
#include "Component/Dx11UIButton.h"

CLogoScene::CLogoScene()
{
}

CLogoScene::~CLogoScene()
{
}

bool CLogoScene::Init()
{
	CDx11Layer*	pLayer = m_pScene->FindLayer("UILayer");

	if (!pLayer)
		return false;

	//----------------------------- Background ------------------------------------
	CDx11GameObject*	pBack = CDx11GameObject::Create("BackObj");

	//ÄÄÆ÷³ÍÆ® ºÎÂø
	CDx11Transform*	pTransform = pBack->GetTransform();
	RESOLUTION_TYPE	eType = DX11_RESOLUTION;
	pTransform->SetWorldScale(g_tRS[eType].iWidth, g_tRS[eType].iHeight, 1.f);
	SAFE_RELEASE(pTransform);

	//ÄÄÆ÷³ÍÆ® ºÎÂø
	CDx11Renderer*	pRenderer = pBack->AddComponent<CDx11Renderer>("BackRenderer");
	pRenderer->SetMesh(RECT_UI_MESH);
	pRenderer->SetShader(UI_SHADER);
	pRenderer->CreateMaterial("LogoBack", L"Logo.png");
	pRenderer->SetRenderState(DX11_RS_DEPTH_DISABLE);
	SAFE_RELEASE(pRenderer);

	//ÄÄÆ÷³ÍÆ® ºÎÂø
	CDx11UIBack* pUIBack = pBack->AddComponent<CDx11UIBack>("UIBack");
	SAFE_RELEASE(pUIBack);

	//·¹ÀÌ¾î Ãß°¡
	pLayer->AddObject(pBack);
	SAFE_RELEASE(pBack);

	//------------------------ ¹öÆ° -----------------------------------------

	CDx11GameObject* pStartButton = CDx11GameObject::Create("StartButtonObj");

	//ÄÄÆ÷³ÍÆ® Ãß°¡
	pTransform = pStartButton->GetTransform();
	pTransform->SetWorldPos(590.f, 500.f, 0.f);
	pTransform->SetWorldScale(100.f, 50.f, 1.f);
	SAFE_RELEASE(pTransform);

	//ÄÄÆ÷³ÍÆ® ºÎÂø
	CDx11UIButton*	pButton = pStartButton->AddComponent<CDx11UIButton>("Button");
	pButton->SetCallback(this, &CLogoScene::StartCallBack);
	SAFE_RELEASE(pButton);

	//·¹ÀÌ¾î Ãß°¡
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
	CDx11Scene*	pScene = DX11_GET_SINGLE(CDx11SceneManager)->CreateScene("MainScene");
	CMainScene*	pMainScene = pScene->CreateSceneScript<CMainScene>("MainScene");
	SAFE_RELEASE(pMainScene);

	//¾ÀÀüÈ¯.
	DX11_GET_SINGLE(CDx11SceneManager)->ChangeScene(pScene);

	return 0;
}
