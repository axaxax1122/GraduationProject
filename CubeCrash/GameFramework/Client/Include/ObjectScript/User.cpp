#include "User.h"
#include "Player.h"
#include "Component/Dx11Transform.h"
#include "GameObject/Dx11GameObject.h"
#include "Scene/Dx11Layer.h"
#include "Scene/Dx11Scene.h"
#include "Core/Dx11Input.h"
#include "Component/Dx11Camera.h"
#include "GuidedMissile.h"
#include "Monster.h"
#include "Component/Dx11LightPoint.h"
#include "Component/Dx11Collider.h"
#include "Laser.h"
#include "Core/Dx11Debug.h"
#include "Component/Dx11Light.h"
#include "Component/Dx11Mouse.h"
#include "Component/Dx11UIBar.h"
#include "Component/Dx11BillBoard.h"
#include "Component/Dx11Renderer.h"
#include "Scene/Dx11SceneManager.h"
#include "../Network/NetworkManager.h"

CUser::CUser()	:
	m_iHP(0),
	m_iHPMax(1),
	m_pHPBar(NULL),
	m_pShadowBar(NULL),
	m_iSerialNumber(0)
{
}

CUser::CUser(const CUser & user) :
	CDx11Script(user)
{
}

CUser::~CUser()
{
	SAFE_RELEASE(m_pHPBar);
	SAFE_RELEASE(m_pShadowBar);
}

void CUser::AddUserObj(CUser * pObj)
{
	m_UserObj.push_back(pObj);
}

CUser * CUser::FindUserObj(const INT64 SerialNumber)
{
	list<CUser*>::iterator iter;
	list<CUser*>::iterator iterEnd;

	for (iter = m_UserObj.begin(); iter != m_UserObj.end(); ++iter)
	{
		if ((*iter)->GetSerialNumber() == SerialNumber)
		{
			// 마찬가지로 참조 카운트 1증가
			(*iter)->AddRef();
			return *iter;
		}
	}
	return nullptr;
}

bool CUser::Init()
{
	m_iHP = 10;
	m_iHPMax = 10;
	m_fGap = 0.f;
	m_fValueAcc = 0.f;

	CreateBar();
	return true;
}

void CUser::Input(float fTime)
{
}

int CUser::Update(float fTime)
{
	// 쉐도우 바 업데이트
	if (m_fGap < 0.f)
	{
		Vec3	vScale;

		if (m_fGap > -2.f)
		{
			m_fGap += fTime * 2;
			vScale = Vec3(1 / 10.f * -fTime * 2, 0.f, 0.f);
		}
		else
		{
			m_fGap += fTime * 4;
			vScale = Vec3(1 / 10.f * -fTime * 4, 0.f, 0.f);
		}

		CDx11Transform*		pTr = m_pShadowBar->GetTransform();
		pTr->ScaleWorld(vScale);
		SAFE_RELEASE(pTr);
	}
	else
	{
		m_fGap = 0.f;
	}
	return 0;
}

int CUser::LateUpdate(float fTime)
{
	return 0;
}

void CUser::Render(float fTime)
{
}

CUser * CUser::Clone()
{
	return new CUser(*this);
}

void CUser::OnCollisionEnter(CDx11Collider * pColl, float fTime)
{
	if (pColl->GetTag() == "MyLaserColl")
	{/*
		pColl->SetColor(Red);
		CDx11GameObject*	pObj = pColl->GetGameObject();
		CLaser*				pLaser = (CLaser*)pObj->FindComponentFromType(CT_SCRIPT);
		pLaser->DeathMotion();
		pObj->Death();
		this->SubtractHP(1);
		SAFE_RELEASE(pLaser);
		SAFE_RELEASE(pObj);*/
	}
}

void CUser::OnCollision(CDx11Collider * pColl, float fTime)
{
}

void CUser::OnCollisionExit(CDx11Collider * pColl, float fTime)
{
}

void CUser::CreateBar()
{
	// Shadow Bar
	m_pShadowBar = CDx11GameObject::Create("ShadowBar");
	m_pGameObject->AddChild(m_pShadowBar);

	CDx11Renderer*	pRenderer = m_pShadowBar->AddComponent<CDx11Renderer>("ShadowBarRenderer");
	pRenderer->CreateMaterial("ShadowBar", L"UI/Unit Frames/UnitFrame_Energy_Bar_Shadow.png");
	pRenderer->SetMesh(EFFECT_LT_MESH);
	pRenderer->SetShader(UI_SHADER);
	pRenderer->SetRenderState(DX11_RS_ALPHABLEND);
	pRenderer->SetRenderState(DX11_RS_DEPTH_DISABLE);
	SAFE_RELEASE(pRenderer);

	CDx11Transform*	pTransform = m_pShadowBar->GetTransform();
	pTransform->DeleteParentFlag(TPF_SCALE);
	pTransform->SetLocalScale(5.f, 10.f, 1.f);
	pTransform->SetWorldScale(1.f, 0.1f, 1.f);
	Vec3 vecPivot = pTransform->GetWorldScale();
	SAFE_RELEASE(pTransform);

	CDx11BillBoard*	pBillBoard = m_pShadowBar->AddComponent<CDx11BillBoard>("ShadowHPBarBillBoard");
	pBillBoard->SetPivot(Vec3(vecPivot.x * 0.5f, -4.f, 0.0f));
	SAFE_RELEASE(pBillBoard);

	// 플레이어 HP Bar 생성
	m_pHPBar = CDx11GameObject::Create("HPBar");
	m_pGameObject->AddChild(m_pHPBar);


	pRenderer = m_pHPBar->AddComponent<CDx11Renderer>("HPBarRenderer");
	pRenderer->CreateMaterial("HPBar", L"UI/Unit Frames/UnitFrame_Energy_Bar_Fill.png");
	pRenderer->SetMesh(EFFECT_LT_MESH);
	pRenderer->SetShader(UI_SHADER);
	pRenderer->SetRenderState(DX11_RS_ALPHABLEND);
	pRenderer->SetRenderState(DX11_RS_DEPTH_DISABLE);
	SAFE_RELEASE(pRenderer);

	pTransform = m_pHPBar->GetTransform();
	pTransform->DeleteParentFlag(TPF_SCALE);
	pTransform->SetLocalScale(5.f, 10.f, 1.f);
	pTransform->SetWorldScale(1.f, 0.1f, 1.f);
	vecPivot = pTransform->GetWorldScale();
	SAFE_RELEASE(pTransform);

	pBillBoard = m_pHPBar->AddComponent<CDx11BillBoard>("HPBarBillBoard");
	pBillBoard->SetPivot(Vec3(vecPivot.x * 0.5f, -4.f, 0.0f));
	SAFE_RELEASE(pBillBoard);

	// Bar Edge
	CDx11GameObject*	pBarEdgeObj = CDx11GameObject::Create("BarEdge");
	m_pGameObject->AddChild(pBarEdgeObj);

	pRenderer = pBarEdgeObj->AddComponent<CDx11Renderer>("BarEdgeRenderer");
	pRenderer->CreateMaterial("HPBarEdge", L"UI/Unit Frames/UnitFrame_Energy_Bar_Empty.png");
	pRenderer->SetMesh(EFFECT_LT_MESH);
	pRenderer->SetShader(UI_SHADER);
	pRenderer->SetRenderState(DX11_RS_ALPHABLEND);
	pRenderer->SetRenderState(DX11_RS_DEPTH_DISABLE);
	SAFE_RELEASE(pRenderer);


	pTransform = pBarEdgeObj->GetTransform();
	pTransform->DeleteParentFlag(TPF_SCALE);
	pTransform->SetLocalScale(5.f, 10.f, 1.f);
	pTransform->SetWorldScale(1.f, 0.1f, 1.f);
	vecPivot = pTransform->GetWorldScale();
	SAFE_RELEASE(pTransform);

	pBillBoard = pBarEdgeObj->AddComponent<CDx11BillBoard>("BarEdgeBillBoard");
	pBillBoard->SetPivot(Vec3(vecPivot.x * 0.5f, -4.f, 0.0f));
	SAFE_RELEASE(pBillBoard);

	SAFE_RELEASE(pBarEdgeObj);
}

void CUser::SubtractHP(int iHP)
{
	m_iHP -= iHP;
	if (m_iHP < 0)
	{
		m_iHP = 0;

	}
	else
	{
		CDx11Transform* pTr = m_pHPBar->GetTransform();
		Vec3		vScale(-iHP / 10.f, 0.f, 0.f);
		pTr->ScaleWorld(vScale);
		SAFE_RELEASE(pTr);

		m_fGap -= 1.f;
	}
}

void CUser::AddHP(int iHP)
{
	m_iHP += iHP;
	if (m_iHP > m_iHPMax)
	{
		m_iHP = m_iHPMax;
	}
	else
	{
		CDx11Transform* pTr = m_pHPBar->GetTransform();
		Vec3		vScale(+iHP / 10.f, 0.f, 0.f);
		pTr->ScaleWorld(vScale);

		CDx11Transform* pTr2 = m_pShadowBar->GetTransform();

		Vec3 vScale1 = pTr->GetWorldScale();
		Vec3 vScale2 = pTr2->GetWorldScale();
		if (vScale1.x > vScale2.x)
		{
			pTr2->ScaleWorld((vScale1 - vScale2) / 4.f);
		}

		SAFE_RELEASE(pTr2);
		SAFE_RELEASE(pTr);

		m_fGap += 1.f;

	}
}
