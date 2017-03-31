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

CPlayer::CPlayer()	:
	m_fShotDelay(0),
	m_bShot(false),
	m_iPowerUP(0),
	m_iHP(0),
	m_iHPMax(1),
	m_pHPBar(NULL),
	m_pShadowBar(NULL),
	m_pTerrain(NULL),
	m_bLeft(false)
{
}


CPlayer::~CPlayer()
{
	SAFE_RELEASE(m_pHPBar);
	SAFE_RELEASE(m_pShadowBar);
}

void CPlayer::ShootMissile(const Vec3 & vView, float fTime)
{
	//	총알 객체 생성
//	CDx11GameObject*	pLaser = CreateLaser(m_pTransform, vView);
//	SAFE_RELEASE(pLaser);
}

void CPlayer::ShootGuidedMissile(const Vec3& vView, float fTime)
{
	CDx11GameObject*	pMissile = CreateGuidedMissile(m_pTransform, vView);
	SAFE_RELEASE(pMissile);
}


void CPlayer::ShakeCamera()
{
	// 씬에서 메인 카메라 얻어오기
	CDx11GameObject*	pCameraObj = m_pScene->GetMainCamera();
	CDx11Camera*	pCamera = (CDx11Camera*)pCameraObj->FindComponentFromTypeName("CDx11Camera");
	if (pCamera->GetShakeTimes() == 0)
		pCamera->SetShakeTimes(96);
	SAFE_RELEASE(pCamera);
	SAFE_RELEASE(pCameraObj);
}

void CPlayer::ComputeShotDelay(bool& bShot, float& fShotDelay, float fTime, float fDelay)
{
	if (bShot)
	{
		if (fShotDelay < fDelay)
		{
			fShotDelay += fTime;
		}
		else
		{
			fShotDelay = 0.f;
			bShot = false;
		}
	}
}

void CPlayer::TraceCameraView(float fTime)
{
	CDx11GameObject*	pCameraObj = m_pScene->GetMainCamera();
	CDx11Transform*		pCamTr = pCameraObj->GetTransform();

	Vec3	vAxis[AXIS_MAX];
	for (int i = 0; i < AXIS_MAX; ++i)
	{
		vAxis[i] = pCamTr->GetWorldAxis((AXIS)i);
		vAxis[i] = vAxis[i].Normalize();
//		m_pTransform->SetWorldAxis(vAxis[i], (AXIS)i);
	}
	// y는 동일하게 설정.
	vAxis[AXIS_X].y = m_pTransform->GetWorldAxis(AXIS_X).y;

	Vec3	vX = m_pTransform->GetWorldAxis(AXIS_X);
	vX = vX.Normalize();
	float fDot = vAxis[AXIS_X].Dot(vX);
	float fAngle = acos(fDot);

	if (fAngle > DX11_PI / 180.f || fAngle < -DX11_PI / 180.f)
	{
		Vec3	vCross = vX.Cross(vAxis[AXIS_X]);
		vCross = vCross.Normalize();
		m_pTransform->RotationWorld(fAngle, fTime * 16, vCross);
	}

	SAFE_RELEASE(pCamTr);
	SAFE_RELEASE(pCameraObj);
}


void CPlayer::SubtractHP(int iHP)
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

void CPlayer::AddHP(int iHP)
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
			pTr2->ScaleWorld(vScale1 - vScale2);
		}

		SAFE_RELEASE(pTr2);
		SAFE_RELEASE(pTr);

	}
}

void CPlayer::CreateBar()
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
	pTransform->SetWorldScale(1.f, 0.1f, 1.f);
	Vec3 vecPivot = pTransform->GetWorldScale();
	SAFE_RELEASE(pTransform);

	CDx11BillBoard*	pBillBoard = m_pShadowBar->AddComponent<CDx11BillBoard>("ShadowHPBarBillBoard");
	pBillBoard->SetPivot(Vec3(vecPivot.x * 0.5f, -1.2f, 0.0f));
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
	pTransform->SetWorldScale(1.f, 0.1f, 1.f);
	vecPivot = pTransform->GetWorldScale();
	SAFE_RELEASE(pTransform);

	pBillBoard = m_pHPBar->AddComponent<CDx11BillBoard>("HPBarBillBoard");
	pBillBoard->SetPivot(Vec3(vecPivot.x * 0.5f, -1.2f, 0.0f));
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
	pTransform->SetWorldScale(1.f, 0.1f, 1.f);
	vecPivot = pTransform->GetWorldScale();
	SAFE_RELEASE(pTransform);

	pBillBoard = pBarEdgeObj->AddComponent<CDx11BillBoard>("BarEdgeBillBoard");
	pBillBoard->SetPivot(Vec3(vecPivot.x * 0.5f, -1.2f, 0.0f));
	SAFE_RELEASE(pBillBoard);

	SAFE_RELEASE(pBarEdgeObj);
}

void CPlayer::SetTerrain(CDx11Terrain * pTerrain)
{
	m_pTerrain = pTerrain;
}





CDx11GameObject * CPlayer::CreateLaser(CDx11Transform * pTransform, const Vec3 & vDir)
{
	CDx11GameObject*	pLaser = CDx11GameObject::Create("LaserPrototype", OT_CLONE);

	CDx11Transform*		pLaserTr = pLaser->GetTransform();
	pLaserTr->Copy(pTransform);
	pLaserTr->MoveWorld(0.5f, 1.f, AXIS_Y);
	pLaserTr->MoveWorld(vDir);
	pLaserTr->SetWorldScale(0.03f, 0.3f, 0.03f);
	pLaserTr->SetWorldView(m_pTransform->GetWorldAxis(AXIS_Z));
	SAFE_RELEASE(pLaserTr);

	m_pLayer->AddObject(pLaser);
	return pLaser;
}

CDx11GameObject * CPlayer::CreateGuidedMissile(CDx11Transform * pTransform, const Vec3 & vDir)
{
	CDx11GameObject*	pMissile = CDx11GameObject::Create("MissilePrototype", OT_CLONE);
	CDx11Transform*		pMissileTr = pMissile->GetTransform();
	pMissileTr->Copy(pTransform);

	pMissileTr->MoveWorld(0.8f, 1.f, AXIS_Z);
	pMissileTr->SetWorldScale(0.03f, 0.04f, 0.03f);
	pMissileTr->SetWorldView(vDir);
	SAFE_RELEASE(pMissileTr);
	
	m_pLayer->AddObject(pMissile);
	return pMissile;
}


bool CPlayer::Init()
{
	m_bShot = false;
	m_fShotDelay = 0.f;
	m_bMissile = false;
	m_fMissileDelay = 0.f;
	m_iHP = 10;
	m_iHPMax = 10;
	m_iPowerUP = 0;
	m_fGap = 0.f;
	m_fValueAcc = 0.f;

	CreateBar();

	return true;
}

void CPlayer::Input(float fTime)
{
	if (DX11_GET_SINGLE(CDx11Input)->KeyDown("MouseLButton"))
	{
		if (!m_bLeft)
		{
			m_bLeft = true;
			m_ptOldMouse = DX11_GET_SINGLE(CDx11Input)->GetMousePoint();
		}
	}
	if (DX11_GET_SINGLE(CDx11Input)->KeyUp("MouseLButton"))
	{
		m_bLeft = false;
	}

	if (DX11_GET_SINGLE(CDx11Input)->KeyDown("MoveLeft") ||
		DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveLeft"))
	{
//		m_pTransform->SetLocalRotY(-DX11_PI);
		m_pTransform->MoveWorld(-30.f, fTime, m_pTransform->GetWorldAxis(AXIS_X));
	}
	if (DX11_GET_SINGLE(CDx11Input)->KeyDown("MoveRight") ||
		DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveRight"))
	{
//		m_pTransform->SetLocalRotY(0.f);
		m_pTransform->MoveWorld(30.f, fTime, m_pTransform->GetWorldAxis(AXIS_X));
	}
	if (DX11_GET_SINGLE(CDx11Input)->KeyDown("MoveFront") ||
		DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveFront"))
	{
//		m_pTransform->SetLocalRotY(-DX11_PI / 2.f);
		m_pTransform->MoveWorld(50.f, fTime, m_pTransform->GetWorldAxis(AXIS_Z));
		
		CDx11Mouse* pMouse = DX11_GET_SINGLE(CDx11Input)->GetMouse();
		if(pMouse->GetRightDrag())
			TraceCameraView(fTime);

		SAFE_RELEASE(pMouse);
	}
	if (DX11_GET_SINGLE(CDx11Input)->KeyDown("MoveBack") ||
		DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveBack"))
	{
//		m_pTransform->SetLocalRotY(DX11_PI / 2.f);
		m_pTransform->MoveWorld(-50.f, fTime, m_pTransform->GetWorldAxis(AXIS_Z));
	}
	if (DX11_GET_SINGLE(CDx11Input)->KeyDown("RotYBack") ||
		DX11_GET_SINGLE(CDx11Input)->KeyPush("RotYBack"))
	{
		m_pTransform->RotationWorld(DX11_PI / 2.f, fTime, AXIS_Y);
	}
	if (DX11_GET_SINGLE(CDx11Input)->KeyDown("RotYFront") ||
		DX11_GET_SINGLE(CDx11Input)->KeyPush("RotYFront"))
	{
		m_pTransform->RotationWorld(-DX11_PI / 2.f, fTime, AXIS_Y);
	}
	if (DX11_GET_SINGLE(CDx11Input)->KeyDown("RotXBack") ||
		DX11_GET_SINGLE(CDx11Input)->KeyPush("RotXBack"))
	{
		m_pTransform->RotationWorld(DX11_PI/ 2.f, fTime, AXIS_X);
	}
	if (DX11_GET_SINGLE(CDx11Input)->KeyDown("RotXFront") ||
		DX11_GET_SINGLE(CDx11Input)->KeyPush("RotXFront"))
	{
		m_pTransform->RotationWorld(-DX11_PI / 2.f, fTime, AXIS_X);
	}
	if (DX11_GET_SINGLE(CDx11Input)->KeyDown("Skill1"))
	{
		if (m_iPowerUP < 2)
			++m_iPowerUP;
		else
			m_iPowerUP = 0;
	}
	if (DX11_GET_SINGLE(CDx11Input)->KeyDown("Fire") ||
		DX11_GET_SINGLE(CDx11Input)->KeyPush("Fire"))
	{
		if (!m_bShot)
		{
			if (m_iPowerUP == 0)
			{
				ShootMissile(Vec3(0.f, 0.f, 0.f), fTime);
			}
			else if (m_iPowerUP == 1)
			{
				for (int i = -1; i <= 1; ++i)
				{
					if (i == 0)
						continue;
					ShootMissile(Vec3(i / 10.f, 0.f, 0.f), fTime);
				}
			}
			else if (m_iPowerUP == 2)
			{
				for (int i = -1; i <= 1; ++i)
				{
					ShootMissile(Vec3(i / 10.f, 0.f, 0.f), fTime);
				}
			}
		
			m_bShot = true;
		}

		if (!m_bMissile)
		{
			for (int i = -1; i <= 1; ++i)
			{
				if (i == 0)
					continue;
				Vec3 vView = m_pTransform->GetWorldAxis(AXIS_Y);
				Matrix matRotate = XMMatrixRotationZ(i * DX11_PI / 2.5f);
				Vec v = XMVector3TransformNormal(vView.Convert(), matRotate);
				vView.Convert(v);

				ShootGuidedMissile(vView, fTime);
			}
			m_bMissile = true;
		}
	}

	static bool	bF7, bF8;

	if (GetAsyncKeyState(VK_F7) & 0x8000)
	{
		bF7 = true;
	}

	else if (bF7)
	{
		bF7 = false;
		SubtractHP(1);
	}

	if (GetAsyncKeyState(VK_F8) & 0x8000)
	{
		bF8 = true;
	}

	else if (bF8)
	{
		bF8 = false;
		AddHP(1);
	}
}

int CPlayer::Update(float fTime)
{
	// 미사일 발사 딜레이 조정
	ComputeShotDelay(m_bShot, m_fShotDelay, fTime, 0.2f);
	ComputeShotDelay(m_bMissile, m_fMissileDelay, fTime, 1.f);


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


	// 지형 타기
	if (m_pTerrain)
	{
		Vec3	vPos = m_pTransform->GetWorldPos();
		if (vPos.y > m_pTerrain->GetHeight(m_pTransform->GetWorldPos()) + 0.3f)
		{
			vPos.y -= fTime;
			m_pTransform->SetWorldPos(vPos);
		}
		else if (vPos.y < m_pTerrain->GetHeight(m_pTransform->GetWorldPos()))
		{
			vPos.y += fTime;
			m_pTransform->SetWorldPos(vPos);
		}
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

void CPlayer::OnCollisionEnter(CDx11Collider * pColl, float fTime)
{
	if (pColl->GetTag() == "BulletColl")
	{
		string str = "Player : Bullet\n";
		char* pStr = (char*)str.c_str();
		DX11_GET_SINGLE(CDx11Debug)->OutputConsole(pStr);

		CDx11GameObject*	pObj = pColl->GetGameObject();
		pObj->Death();
//		ShakeCamera();
		SubtractHP(1);
		SAFE_RELEASE(pObj);
	}
}

void CPlayer::OnCollision(CDx11Collider * pColl, float fTime)
{
	if (pColl->GetTag() == "RayCollider")
	{
		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			MessageBoxA(NULL, "충돌", "광선충돌", MB_OK);
		}
	}
}

void CPlayer::OnCollisionExit(CDx11Collider * pColl, float fTime)
{
}
