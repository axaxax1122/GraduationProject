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
	//�������� ����Ʈ ���̾ ã�´�.
	CDx11Layer*	pLayer = m_pScene->FindLayer("DefaultLayer");
	if (!pLayer)
		return false;

	// ���� �ؽ�ó �ε�
	CDx11Texture*	pTexture = DX11_GET_SINGLE(CDx11ResourcesManager)->LoadTexture("�Ķ�", L"�Ķ�.png");
	SAFE_RELEASE(pTexture);

	// �÷��̾� ��ü�� �����Ѵ�.
	CDx11GameObject*	pPlayer = CDx11GameObject::Create("PlayerObject");

	// �������� �����Ѵ�.
	CDx11Renderer*	pRenderer = pPlayer->AddComponent<CDx11Renderer>("PlayerRenderer");
	pRenderer->SetMesh("RectMesh");
	pRenderer->SetShader(DEFAULT_TEX_SHADER);
	// ������ �����Ѵ�.
	pRenderer->CreateMaterial("�Ķ�", DEFAULT_SAMPLER);
	SAFE_RELEASE(pRenderer);
	
	// �÷��̾� ��ũ��Ʈ�� ������Ʈ�� �߰��Ѵ�.
	CPlayer*	pPlayerScript = pPlayer->AddComponent<CPlayer>("PlayerScript");
	SAFE_RELEASE(pPlayerScript);

	// ���̾ �߰��Ѵ�.
	pLayer->AddObject(pPlayer);


	// ���� ���ϵ带 �߰��Ѵ�.
	CDx11GameObject*	pPlayerChild = CDx11GameObject::Create("LeftChildObject");

	// �������� �����Ѵ�.
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

	// ������ ���ϵ�
	pPlayerChild = CDx11GameObject::Create("RightChildObject");

	// �������� �����Ѵ�.
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



	// �ҷ� ������Ÿ���� �����.
	CDx11GameObject*	pBullet = CDx11GameObject::Create("BulletPrototype", OT_PROTOTYPE);

	// �������� �����Ѵ�.
	pRenderer = pBullet->AddComponent<CDx11Renderer>("BulletRenderer");
	pRenderer->SetMesh(PYRAMID_MESH);
	pRenderer->SetShader(DEFAULT_SHADER);
	SAFE_RELEASE(pRenderer);

	// �Ѿ� ��ũ��Ʈ�� ������Ʈ�� �߰��Ѵ�.
	CBullet*	pBulletSc = (CBullet*)pBullet->AddComponent<CBullet>("BulletScript");
	SAFE_RELEASE(pBulletSc);



	// ���� ������Ÿ���� �����.
	CDx11GameObject*	pMonster = CDx11GameObject::Create("MonsterPrototype", OT_PROTOTYPE);

	// �������� �����Ѵ�.
	pRenderer = pMonster->AddComponent<CDx11Renderer>("MonsterRenderer");
	pRenderer->SetMesh(PYRAMID_MESH);
	pRenderer->SetShader(DEFAULT_SHADER);
	SAFE_RELEASE(pRenderer);

	// ���� ��ũ��Ʈ�� ������Ʈ�� �߰��Ѵ�.
	CMonster*	pMonsterSc = (CMonster*)pMonster->AddComponent<CMonster>("MonsterScript");
	SAFE_RELEASE(pMonsterSc);

	SAFE_RELEASE(pLayer);


	// 1�ʸ��� ���͸� �����ϴ� �����췯�� �����.
	DX11_GET_SINGLE(CDx11Scheduler)->CreateSchedule(this, &CMainScene::CreateMonster, 1.f, SO_LOOP);


	// ���� ī�޶� �߰��Ѵ�.
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
	// Ű�Է����� ī�޶��� ��ġ�� �����Ѵ�.

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
//	���� 5��� ���� �����췯 ����
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
