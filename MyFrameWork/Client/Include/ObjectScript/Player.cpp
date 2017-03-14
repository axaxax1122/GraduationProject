#include "Player.h"
#include "Component/Dx11Transform.h"
#include "GameObject/Dx11GameObject.h"
#include "Scene/Dx11Layer.h"
#include "Scene/Dx11Scene.h"
#include "Core/Dx11Input.h"
#include "Component/Dx11Camera.h"


CPlayer::CPlayer()	:
	m_fUpdateTime(0)
{
}


CPlayer::~CPlayer()
{
}

void CPlayer::ShootMissile(const Vec3 & vView)
{
//	총알 객체 생성
	CDx11GameObject*	pBullet = CreateBullet(m_pTransform, vView);
	SAFE_RELEASE(pBullet);


//	차일드 객체에서 함수 호출
	CDx11GameObject*	pChildObj = m_pGameObject->FindChildFromTag("LeftChildObject");
	CDx11Transform*		pChildTr = pChildObj->GetTransform();
	pBullet = CreateBullet(pChildTr, pChildTr->GetWorldAxis(AXIS_Y));

	SAFE_RELEASE(pChildTr);
	SAFE_RELEASE(pChildObj);
	SAFE_RELEASE(pBullet);


	pChildObj = m_pGameObject->FindChildFromTag("RightChildObject");
	pChildTr = pChildObj->GetTransform();
	pBullet = CreateBullet(pChildTr, pChildTr->GetWorldAxis(AXIS_Y));
	
	SAFE_RELEASE(pChildTr);
	SAFE_RELEASE(pChildObj);
	SAFE_RELEASE(pBullet);

//	카메라 효과
//	ShakeCamera();
}

void CPlayer::ShakeCamera()
{
	// 씬에서 메인 카메라 얻어오기
	CDx11GameObject*	pCameraObj = m_pScene->GetMainCamera();
	CDx11Camera*	pCamera = (CDx11Camera*)pCameraObj->FindComponentFromTypeName("CDx11Camera");
	if (pCamera->GetShakeTimes() == 0)
		pCamera->SetShakeTimes(32);
	SAFE_RELEASE(pCamera);
	SAFE_RELEASE(pCameraObj);
}

// 총알 객체 생성함수
CDx11GameObject * CPlayer::CreateBullet(CDx11Transform * pTransform, const Vec3& vDir)
{
	CDx11GameObject*	pBullet = CDx11GameObject::Create("BulletPrototype",
		OT_CLONE);

	CDx11Transform*		pBulletTr = pBullet->GetTransform();
	pBulletTr->Copy(pTransform);
	pBulletTr->MoveWorld(0.5f, 1.f, AXIS_Y);
	pBulletTr->SetWorldScale(0.1f, 0.1f, 0.1f);
	pBulletTr->SetWorldView(vDir);
	
	m_pLayer->AddObject(pBullet);
	
	SAFE_RELEASE(pBulletTr);
	SAFE_RELEASE(pBullet);
	
	return pBullet;
}

bool CPlayer::Init()
{

	return true;
}

void CPlayer::Input(float fTime)
{
	if (DX11_GET_SINGLE(CDx11Input)->KeyDown("RotZBack") ||
		DX11_GET_SINGLE(CDx11Input)->KeyPush("RotZBack"))
	{
		m_pTransform->RotationWorld(DX11_PI, fTime, AXIS_Z);
	}
	if (DX11_GET_SINGLE(CDx11Input)->KeyDown("RotZFront") ||
		DX11_GET_SINGLE(CDx11Input)->KeyPush("RotZFront"))
	{
		m_pTransform->RotationWorld(-DX11_PI, fTime, AXIS_Z);
	}
	if (DX11_GET_SINGLE(CDx11Input)->KeyDown("MoveFront") ||
		DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveFront"))
	{
		m_pTransform->MoveWorld(5.f, fTime, AXIS_Y);
	}
	if (DX11_GET_SINGLE(CDx11Input)->KeyDown("MoveBack") ||
		DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveBack"))
	{
		m_pTransform->MoveWorld(-5.f, fTime, AXIS_Y);
	}
	if (DX11_GET_SINGLE(CDx11Input)->KeyDown("RotYBack") ||
		DX11_GET_SINGLE(CDx11Input)->KeyPush("RotYBack"))
	{
		m_pTransform->RotationWorld(DX11_PI, fTime, AXIS_Y);
	}
	if (DX11_GET_SINGLE(CDx11Input)->KeyDown("RotXBack") ||
		DX11_GET_SINGLE(CDx11Input)->KeyPush("RotXBack"))
	{
		m_pTransform->RotationWorld(DX11_PI, fTime, AXIS_X);
	}
	if (DX11_GET_SINGLE(CDx11Input)->KeyDown("Fire"))
	{
		ShootMissile(m_pTransform->GetWorldAxis(AXIS_Y));
	}
	if (DX11_GET_SINGLE(CDx11Input)->KeyDown("Skill1"))
	{
		for (int i = -1; i <= 1; ++i)
		{
			Vec3 vView = m_pTransform->GetWorldAxis(AXIS_Y);
			vView.x += (i * DX11_PI / 4);
			Vec v = XMVector3Normalize(vView.Convert());	//XMVECTOR3
			vView.Convert(v);								//XMFLOAT3

			ShootMissile(vView);
		}
	}
}

int CPlayer::Update(float fTime)
{
	m_fUpdateTime += fTime;
	if (m_fUpdateTime >= 0.01f)
	{
		CDx11GameObject*	pPlayerObj = m_pScene->FindGameObjectFromTag("PlayerObject");

		CDx11GameObject*	pChildObj = pPlayerObj->FindChildFromTag("LeftChildObject");
		CDx11Transform*		pChildTr = pChildObj->GetTransform();
		pChildTr->RevolutionWorld(-DX11_PI / 180, 1.f, m_pTransform->GetWorldAxis(AXIS_Z));
		SAFE_RELEASE(pChildTr);
		SAFE_RELEASE(pChildObj);

		pChildObj = pPlayerObj->FindChildFromTag("RightChildObject");
		pChildTr = pChildObj->GetTransform();
//		pChildTr->RevolutionWorld(-DX11_PI / 180, 1.f, m_pTransform->GetWorldAxis(AXIS_Z));

		SAFE_RELEASE(pChildTr);
		SAFE_RELEASE(pChildObj);
		SAFE_RELEASE(pPlayerObj);
		m_fUpdateTime = 0;
	}
	return 0;
}

int CPlayer::LateUpdate(float fTime)
{

	return 0;
}

void CPlayer::Render(float fTime)
{
}

CPlayer * CPlayer::Clone()
{
	return new CPlayer(*this);
}
