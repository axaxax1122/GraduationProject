#include "MainScene.h"
#include "GameObject/Dx11GameObject.h"
#include "Component/Dx11Transform.h"
#include "Component/Dx11Renderer.h"
#include "Scene/Dx11Scene.h"
#include "Scene/Dx11Layer.h"
#include "../ObjectScript/Player.h"
#include "../ObjectScript/Bullet.h"
#include "../ObjectScript/Monster.h"
#include "Core/Dx11Scheduler.h"
#include "Resources/Dx11ResourcesManager.h"
#include "Resources/Dx11Texture.h"
#include "Component/Dx11Material.h"

CMainScene::CMainScene()	:
	m_iMonsterCount(0)
{
}

CMainScene::~CMainScene()
{
}

int CMainScene::GetMonsterCount()
{
	return m_iMonsterCount;
}

bool CMainScene::Init()
{
	//만들어놨던 디폴트 레이어를 찾는다.
	CDx11Layer*	pLayer = m_pScene->FindLayer("DefaultLayer");
	if (!pLayer)
		return false;

	// 샘플 텍스처 로딩
	CDx11Texture*	pTexture = DX11_GET_SINGLE(CDx11ResourcesManager)->LoadTexture("파라", L"파라.png");
	SAFE_RELEASE(pTexture);

	// 플레이어 객체를 생성한다.
	CDx11GameObject*	pPlayer = CDx11GameObject::Create("PlayerObject");

	// 렌더러를 생성한다.
	CDx11Renderer*	pRenderer = pPlayer->AddComponent<CDx11Renderer>("PlayerRenderer");
	pRenderer->SetMesh("RectMesh");
	pRenderer->SetShader(DEFAULT_TEX_SHADER);
	// 재질을 설정한다.
	pRenderer->CreateMaterial("파라", DEFAULT_SAMPLER);
	SAFE_RELEASE(pRenderer);
	
	// 플레이어 스크립트를 컴포넌트에 추가한다.
	CPlayer*	pPlayerScript = pPlayer->AddComponent<CPlayer>("PlayerScript");
	SAFE_RELEASE(pPlayerScript);

	// 레이어에 추가한다.
	pLayer->AddObject(pPlayer);


	// 왼쪽 차일드를 추가한다.
	CDx11GameObject*	pPlayerChild = CDx11GameObject::Create("LeftChildObject");

	// 렌더러를 생성한다.
	pRenderer = pPlayerChild->AddComponent<CDx11Renderer>("PlayerChildRenderer");
	pRenderer->SetMesh(PYRAMID_MESH);
	pRenderer->SetShader(DEFAULT_SHADER);
	SAFE_RELEASE(pRenderer);

	CDx11Transform*	pChildTr = pPlayerChild->GetTransform();
	pChildTr->MoveWorld(-1.5f, 1.f, AXIS_X);
	pChildTr->SetWorldScale(0.4f, 0.4f, 0.4f);
	pChildTr->DeleteParentFlag(TPF_ROT);
	SAFE_RELEASE(pChildTr);

	pPlayer->AddChild(pPlayerChild);
	SAFE_RELEASE(pPlayerChild);

	// 오른쪽 차일드
	pPlayerChild = CDx11GameObject::Create("RightChildObject");

	// 렌더러를 생성한다.
	pRenderer = pPlayerChild->AddComponent<CDx11Renderer>("PlayerChildRenderer");
	pRenderer->SetMesh(PYRAMID_MESH);
	pRenderer->SetShader(DEFAULT_SHADER);
	SAFE_RELEASE(pRenderer);

	pChildTr = pPlayerChild->GetTransform();
	pChildTr->MoveWorld(+1.5f, 1.f, AXIS_X);
	pChildTr->SetWorldScale(0.4f, 0.4f, 0.4f);
//	pChildTr->DeleteParentFlag(TPF_ROT);
	SAFE_RELEASE(pChildTr);

	pPlayer->AddChild(pPlayerChild);
	SAFE_RELEASE(pPlayerChild);
	SAFE_RELEASE(pPlayer);



	// 불렛 프로토타입을 만든다.
	CDx11GameObject*	pBullet = CDx11GameObject::Create("BulletPrototype", OT_PROTOTYPE);

	// 렌더러를 생성한다.
	pRenderer = pBullet->AddComponent<CDx11Renderer>("BulletRenderer");
	pRenderer->SetMesh(PYRAMID_MESH);
	pRenderer->SetShader(DEFAULT_SHADER);
	SAFE_RELEASE(pRenderer);

	// 총알 스크립트를 컴포넌트에 추가한다.
	CBullet*	pBulletSc = (CBullet*)pBullet->AddComponent<CBullet>("BulletScript");
	SAFE_RELEASE(pBulletSc);



	// 몬스터 프로토타입을 만든다.
	CDx11GameObject*	pMonster = CDx11GameObject::Create("MonsterPrototype", OT_PROTOTYPE);

	// 렌더러를 생성한다.
	pRenderer = pMonster->AddComponent<CDx11Renderer>("MonsterRenderer");
	pRenderer->SetMesh(PYRAMID_MESH);
	pRenderer->SetShader(DEFAULT_SHADER);
	SAFE_RELEASE(pRenderer);

	// 몬스터 스크립트를 컴포넌트에 추가한다.
	CMonster*	pMonsterSc = (CMonster*)pMonster->AddComponent<CMonster>("MonsterScript");
	SAFE_RELEASE(pMonsterSc);

	SAFE_RELEASE(pLayer);


	// 1초마다 몬스터를 생성하는 스케쥴러를 만든다.
	DX11_GET_SINGLE(CDx11Scheduler)->CreateSchedule(this, &CMainScene::CreateMonster, 1.f, SO_LOOP);


	// 서브 카메라를 추가한다.
	CDx11GameObject*	pCameraObj = m_pScene->CreateCamera("SubCamera1",
		DX11_PI / 3.f, 0.1f, 1000.f);
	CDx11Transform*	pCamTr = pCameraObj->GetTransform();

	pCamTr->SetWorldPos(5.f, 0.f, -10.f);
	pCamTr->SetWorldRot(0.f, -DX11_PI / 4.f, DX11_PI / 6.f);
	SAFE_RELEASE(pCamTr);
	SAFE_RELEASE(pCameraObj);

	return true;
}

void CMainScene::Input(float fTime)
{
}

int CMainScene::Update(float fTime)
{
	// 키입력으로 카메라의 위치를 조정한다.

	CDx11GameObject*	pCamera = m_pScene->GetMainCamera();
	CDx11Transform*	pTr = pCamera->GetTransform();

	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		pTr->MoveWorld(10.f, fTime, AXIS_Z);
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		pTr->MoveWorld(-10.f, fTime, AXIS_Z);
	}

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		pTr->RotationWorld(-DX11_PI / 3, fTime, AXIS_X);
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		pTr->RotationWorld(DX11_PI / 3, fTime, AXIS_X);
	}

	static string	str = "SubCamera1";
	static bool		bReturn;

	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		bReturn = true;
	}
	else if (bReturn)
	{
		bReturn = false;
		m_pScene->ChangeCamera(str);

		if (str == "SubCamera1")
			str = "MainCamera";

		else
			str = "SubCamera1";
	}

	SAFE_RELEASE(pTr);
	SAFE_RELEASE(pCamera);
	return 0;
}

int CMainScene::LateUpdate(float fTime)
{
//	몬스터 5기면 생성 스케쥴러 삭제
	if(this->GetMonsterCount() >= 5)
		DX11_GET_SINGLE(CDx11Scheduler)->DeleteSchedule(this);

	return 0;
}

void CMainScene::Render(float fTime)
{
}

CMainScene * CMainScene::Clone()
{
	return new CMainScene(*this);
}

int CMainScene::CreateMonster(float fTime)
{
	CDx11GameObject*	pMonster = CDx11GameObject::Create("MonsterPrototype", OT_CLONE);
	CDx11Transform*		pMonsterTr = pMonster->GetTransform();
	int i = GetMonsterCount() - 2;
	pMonsterTr->SetWorldScale(0.7f, 0.7f, 0.7f);
	pMonsterTr->SetWorldPos(4.f * i, 4.f, 4.f);
	
	CDx11Layer*	pLayer = m_pScene->FindLayer("DefaultLayer");

	if (!pLayer)
		return false;

	++m_iMonsterCount;
	pLayer->AddObject(pMonster);
	SAFE_RELEASE(pMonsterTr);
	SAFE_RELEASE(pMonster);
	SAFE_RELEASE(pLayer);

	return 0;
}
