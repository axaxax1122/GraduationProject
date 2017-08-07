#include "Laser.h"
#include "Scene/Dx11Layer.h"
#include "Scene/Dx11Scene.h"
#include "Component/Dx11Transform.h"
#include "GameObject/Dx11GameObject.h"
#include "Component/Dx11Animation2D.h"
#include "Component/Dx11Renderer.h"
#include "Component/Dx11BillBoard.h"
#include "Component/Dx11Collider.h"
#include "User.h"
#include "../Network/NetworkManager.h"

CLaser::CLaser() :
	m_fSpeed(0),
	m_fDist(0),
	m_vDir(Vec3Zero)
{
}

CLaser::CLaser(const CLaser & laser)	:
	CDx11Script(laser)
{
	*this = laser;
}


CLaser::~CLaser()
{
}

void CLaser::SetSpeed(float fSpeed)
{
	m_fSpeed = fSpeed;
}

void CLaser::SetDistance(float fDist)
{
	m_fDist = fDist;
}

void CLaser::SetDirection(const Vec3 & vDir)
{
	m_vDir = vDir;
}

void CLaser::DeathMotion()
{
	CDx11GameObject*	pSmallEffectObj = CDx11GameObject::Create("SmallEffectObject");
	CDx11Transform*	pEffectTr = pSmallEffectObj->GetTransform();
	pEffectTr->SetWorldScale(5.f, 5.f, 5.f);
	pEffectTr->SetWorldPos(m_pTransform->GetWorldPos());
	SAFE_RELEASE(pEffectTr);

	CDx11Renderer* pRenderer = pSmallEffectObj->AddComponent<CDx11Renderer>("SmallRenderer");
	pRenderer->CreateMaterial("SmallExplosion1");
	SAFE_RELEASE(pRenderer);

	CDx11BillBoard* pBillBoard = pSmallEffectObj->AddComponent<CDx11BillBoard>("SmallBillBoard");
	SAFE_RELEASE(pBillBoard);

	CDx11Animation2D* pAnimation2D = pSmallEffectObj->AddComponent<CDx11Animation2D>("SmallAnim2D");
	pAnimation2D->Init();
	pAnimation2D->SetMaxFrame(89, 1);
	pAnimation2D->SetAnimation2DOption(A2DO_ONCE);
	pAnimation2D->SetAnimationLimitTime(0.75f);
	for (int i = 1; i <= 89; ++i)
	{
		char	strKey[256] = {};
		sprintf_s(strKey, "Explosion%d", i);
		wchar_t	strPath[MAX_PATH] = {};
		wsprintf(strPath, L"Effect\\Explosion\\64x48\\Explosion%d.png", i);
		pAnimation2D->AddFrameTexture(strKey, strPath);

	}

	m_pLayer->AddObject(pSmallEffectObj);
	SAFE_RELEASE(pAnimation2D);
	SAFE_RELEASE(pSmallEffectObj);
}

bool CLaser::Init()
{
	m_fSpeed = 100.f;
	m_fDist = 200.f;
	return true;
}

void CLaser::Input(float fTime)
{
}

int CLaser::Update(float fTime)
{
	m_vDir = m_pTransform->GetWorldView();
	Vec3	vAxis = m_pTransform->GetWorldAxis(AXIS_Y);

	// m_vDir과 미사일의 Y축을 내적
	float fDot = m_vDir.Dot(vAxis);
	float fAngle = acos(fDot);

	// 미사일의 Y축과 m_vDir을 외적해서 회전 축을 구한다.
	Vec3 vRotation = vAxis.Cross(m_vDir);
	vRotation = vRotation.Normalize();

	if (fAngle < -DX11_PI / 180.f || fAngle > DX11_PI / 180.f)
		m_pTransform->RotationWorld(fAngle, 1.f, vRotation);

	m_pTransform->MoveWorld(m_fSpeed, fTime);


	// 총알이 일정거리만큼 이동하면 삭제된다. 
	m_fDist -= (m_fSpeed * fTime);
	if (m_fDist <= 0.f)
	{
		m_pGameObject->Death();
	}

	if (m_fDist <= 195.f)
	{
		m_pTransform->SetWorldScale(0.5f, 0.5f, 0.5f);
		m_pTransform->RotationWorld(DX11_PI / 1.5f, fTime, m_pTransform->GetWorldAxis(AXIS_Y));
	}

	return 0;
}

int CLaser::LateUpdate(float fTime)
{
	return 0;
}

void CLaser::Render(float fTime)
{
}

CLaser * CLaser::Clone()
{
	return new CLaser(*this);
}

void CLaser::OnCollisionEnter(CDx11Collider * pColl, float fTime)
{
	if ((pColl->GetTag() == "BoxColl"))
	{
		pColl->SetColor(Red);
		DeathMotion();
		m_pGameObject->Death();
	}

	if ((pColl->GetTag() == "UserColl"))
	{

		GET_SINGLE(CNetworkManager)->HeatPlayer();
		CDx11GameObject*	pObj = pColl->GetGameObject();
		CUser* pUser = (CUser*)pObj->FindComponentFromType(CT_SCRIPT);
		pUser->SubtractHP(1);

		pColl->SetColor(Red);
		DeathMotion();
		m_pGameObject->Death();

		SAFE_RELEASE(pUser);
		SAFE_RELEASE(pObj);
	}
}

void CLaser::OnCollision(CDx11Collider * pColl, float fTime)
{
}

void CLaser::OnCollisionExit(CDx11Collider * pColl, float fTime)
{
}
