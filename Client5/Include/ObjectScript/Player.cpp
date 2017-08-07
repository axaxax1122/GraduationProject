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
#include "Scene/QuadTree.h"
#include "Component/Dx11ColliderOBB.h"
#include "Component/Dx11ColliderRay.h"
#include "../Network/NetworkManager.h"
#include "Component/Dx11UIBackground.h"
#include "Core/Dx11Device.h"
#include "User.h"
#include "Core/Dx11SoundManager.h"
extern bool UpdateFlag;
extern bool moveFlag;

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
	m_eDir = DIR_NOMAL;
}


CPlayer::~CPlayer()
{
	SAFE_RELEASE(m_pHPBar);
	SAFE_RELEASE(m_pShadowBar);
}

void CPlayer::ShootMissile(const Vec3 & vView, float fTime)
{
	//	총알 객체 생성
	CDx11GameObject*	pLaser = CreateLaser(m_pTransform, vView);
	SAFE_RELEASE(pLaser);
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
		m_pTransform->RotationWorld(fAngle, fTime * 8, vCross);
	}

	SAFE_RELEASE(pCamTr);
	SAFE_RELEASE(pCameraObj);
}

void CPlayer::TurnToDirection(DIRECTION eDir, float fTime)
{
	m_eDir = eDir;
	CDx11GameObject*	pCameraObj = m_pScene->GetMainCamera();
	CDx11Transform*		pCamTr = pCameraObj->GetTransform();

	Vec3 vAxisX = pCamTr->GetWorldAxis(AXIS_X);
	vAxisX.y = 0.f;
	vAxisX = vAxisX.Normalize();

	Vec3	vPlayerAxis[AXIS_MAX];
	for (int i = 0; i < AXIS_MAX; ++i)
	{
		vPlayerAxis[i] = m_pTransform->GetWorldAxis((AXIS)i);
		vPlayerAxis[i].y = 0.f;
		vPlayerAxis[i] = vPlayerAxis[i].Normalize();
	}

	Vec3 vCross;
	float fDot;
	float fAngle;

	SAFE_RELEASE(pCamTr);
	SAFE_RELEASE(pCameraObj);

	if (m_eDir == DIR_FRONT)
	{
		fDot = vAxisX.Dot(vPlayerAxis[AXIS_X]);
		fAngle = acos(fDot);
		vCross = vPlayerAxis[AXIS_X].Cross(vAxisX);
	}
	else if (m_eDir == DIR_BACK)
	{
		vAxisX *= -1;
		fDot = vAxisX.Dot(vPlayerAxis[AXIS_X]);
		fAngle = acos(fDot);
		vCross = vPlayerAxis[AXIS_X].Cross(vAxisX);
	}
	else if (m_eDir == DIR_LEFT)
	{
		vAxisX *= -1;
		vAxisX.y *= -1;
		vPlayerAxis[AXIS_Z].y = vAxisX.y;
		vPlayerAxis[AXIS_Z] = vPlayerAxis[AXIS_Z].Normalize();

		fDot = vAxisX.Dot(vPlayerAxis[AXIS_Z]);
		fAngle = acos(fDot);
		vCross = vPlayerAxis[AXIS_Z].Cross(vAxisX);
	}
	else if (m_eDir == DIR_RIGHT)
	{
		vPlayerAxis[AXIS_Z].y = vAxisX.y;
		vPlayerAxis[AXIS_Z] = vPlayerAxis[AXIS_Z].Normalize();

		fDot = vAxisX.Dot(vPlayerAxis[AXIS_Z]);
		fAngle = acos(fDot);
		vCross = vPlayerAxis[AXIS_Z].Cross(vAxisX);
	}
	else if (m_eDir == DIR_FRONT_LEFT)
	{
		Vec3 vFL = vAxisX;
		vFL *= -1;
		vFL.y *= -1;

		Matrix matRotate = XMMatrixRotationY(DX11_PI / 4.f);
		Vec v = XMVector3TransformNormal(vFL.Convert(), matRotate);
		vFL.Convert(v);
		vFL = vFL.Normalize();
		
		vPlayerAxis[AXIS_Z].y = vFL.y;
		vPlayerAxis[AXIS_Z] = vPlayerAxis[AXIS_Z].Normalize();

		fDot = vFL.Dot(vPlayerAxis[AXIS_Z]);
		fAngle = acos(fDot);
		vCross = vPlayerAxis[AXIS_Z].Cross(vFL);
	}
	else if (m_eDir == DIR_FRONT_RIGHT)
	{
		Vec3 vFL = vAxisX;

		Matrix matRotate = XMMatrixRotationY(-DX11_PI / 4.f);
		Vec v = XMVector3TransformNormal(vFL.Convert(), matRotate);
		vFL.Convert(v);
		vFL = vFL.Normalize();

		vPlayerAxis[AXIS_Z].y = vFL.y;
		vPlayerAxis[AXIS_Z] = vPlayerAxis[AXIS_Z].Normalize();

		fDot = vFL.Dot(vPlayerAxis[AXIS_Z]);
		fAngle = acos(fDot);
		vCross = vPlayerAxis[AXIS_Z].Cross(vFL);
	}
	else if (m_eDir == DIR_BACK_LEFT)
	{
		Vec3 vFL = vAxisX;
		vFL *= -1;
		vFL.y *= -1;

		Matrix matRotate = XMMatrixRotationY(-DX11_PI / 4.f);
		Vec v = XMVector3TransformNormal(vFL.Convert(), matRotate);
		vFL.Convert(v);
		vFL = vFL.Normalize();

		vPlayerAxis[AXIS_Z].y = vFL.y;
		vPlayerAxis[AXIS_Z] = vPlayerAxis[AXIS_Z].Normalize();

		fDot = vFL.Dot(vPlayerAxis[AXIS_Z]);
		fAngle = acos(fDot);
		vCross = vPlayerAxis[AXIS_Z].Cross(vFL);
	}
	else if (m_eDir == DIR_BACK_RIGHT)
	{
		Vec3 vFL = vAxisX;

		Matrix matRotate = XMMatrixRotationY(DX11_PI / 4.f);
		Vec v = XMVector3TransformNormal(vFL.Convert(), matRotate);
		vFL.Convert(v);
		vFL = vFL.Normalize();

		vPlayerAxis[AXIS_Z].y = vFL.y;
		vPlayerAxis[AXIS_Z] = vPlayerAxis[AXIS_Z].Normalize();

		fDot = vFL.Dot(vPlayerAxis[AXIS_Z]);
		fAngle = acos(fDot);
		vCross = vPlayerAxis[AXIS_Z].Cross(vFL);
	}


	if (fAngle > DX11_PI / 180.f || fAngle < -DX11_PI / 180.f)
	{
		vCross = vCross.Normalize();
		RotatePlayer(fAngle, fTime * 4, vCross);

	}

	
}

void CPlayer::MoveFront(float fTime)
{
	int iStateFlag = m_pGameObject->GetStateFlag();
	if (iStateFlag != STATE_ROLL)
	{
		if(moveFlag == true)
			m_pTransform->MoveWorld(15.f, fTime, m_pTransform->GetWorldAxis(AXIS_Z));
		m_pGameObject->DeleteStateFlag(STATE_IDLE);
		m_pGameObject->AddStateFlag(STATE_WALK);
//		GET_SINGLE(CNetworkManager)->AnimState(STATE_WALK);

	}
}

void CPlayer::MoveDash(float fTime)
{
	if (moveFlag == true) {
		m_pTransform->MoveWorld(30.f, fTime, m_pTransform->GetWorldAxis(AXIS_Z));
	}
}

void CPlayer::RotatePlayer(float fAngle, float fTime, const Vec3& vCross)
{
	m_pTransform->RotationWorld(fAngle, fTime * 8, vCross);
}	

void CPlayer::RotatePlayer(float fAngle, float fTime, const AXIS vAxis)
{
	m_pTransform->RotationWorld(fAngle, fTime, vAxis);
}


void CPlayer::SubtractHP(int iHP)
{
	m_iHP -= iHP;
	if (m_iHP < 0)
	{
		m_iHP = 0;
		
	}

	if (!m_pHPBar)
		return;
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
			pTr2->ScaleWorld((vScale1 - vScale2) / 300.f);
		}

		SAFE_RELEASE(pTr2);
		SAFE_RELEASE(pTr);

		m_fGap += 1.f;

	}
}

void CPlayer::SubtractHP_User(int iHP)
{
	m_iHP -= iHP;
	if (m_iHP < 0)
	{
		m_iHP = 0;

	}

	if (!m_pHPBar_User)
		return;
	else
	{
		CDx11Transform* pTr = m_pHPBar_User->GetTransform();
		Vec3		vScale(-iHP / 10.f, 0.f, 0.f);
		pTr->ScaleWorld(vScale);
		SAFE_RELEASE(pTr);

		m_fGap -= 1.f;
	}
}

void CPlayer::AddHP_User(int iHP)
{
	m_iHP += iHP;
	if (m_iHP > m_iHPMax)
	{
		m_iHP = m_iHPMax;
	}
	else
	{
		CDx11Transform* pTr = m_pHPBar_User->GetTransform();
		Vec3		vScale(+iHP / 10.f, 0.f, 0.f);
		pTr->ScaleWorld(vScale);

		CDx11Transform* pTr2 = m_pShadowBar_User->GetTransform();

		Vec3 vScale1 = pTr->GetWorldScale();
		Vec3 vScale2 = pTr2->GetWorldScale();
		if (vScale1.x > vScale2.x)
		{
			pTr2->ScaleWorld((vScale1 - vScale2) / 300.f);
		}

		SAFE_RELEASE(pTr2);
		SAFE_RELEASE(pTr);

		m_fGap += 1.f;

	}
}

void CPlayer::UpdateUserHP()
{
	//hp 겟
	CDx11GameObject*	pUserObj = m_pScene->FindGameObjectFromTag("UserObject");
	if (!pUserObj)
		return;
	CUser*	pUserComponent = (CUser*)pUserObj->FindComponentFromTag("User");
	int UserHP = pUserComponent->GetHP();
	SAFE_RELEASE(pUserComponent);
	SAFE_RELEASE(pUserObj);

	
	if (m_iHP_User == UserHP)//같을때!
		return;
	//else if(m_iHP_User > UserHP) {//감소
	//	int hp = m_iHP_User - UserHP;
	//	SubtractHP_User(hp);
	//}
	//else {//증가
	//	int hp = UserHP - m_iHP_User;
	//	AddHP_User(hp);
	//}

}

void CPlayer::CreateBar()
{
	//// Shadow Bar
	m_pShadowBar = CDx11GameObject::Create("MyShadowBar");

	CDx11Renderer*	pRenderer = m_pShadowBar->AddComponent<CDx11Renderer>("ShadowBarRenderer");
	pRenderer->CreateMaterial("ShadowBarRed", L"UI/Unit Frames/UnitFrame_Energy_Bar_Shadow_Red.png");
	pRenderer->SetMesh(RECT_UI_MESH);
	pRenderer->SetShader(UI_SHADER);
	pRenderer->SetRenderState(DX11_RS_ALPHABLEND);
	pRenderer->SetRenderState(DX11_RS_DEPTH_DISABLE);
	SAFE_RELEASE(pRenderer);

	CDx11Transform*	pTransform = m_pShadowBar->GetTransform();
	pTransform->SetLocalScale(300.f, 800.f, 1.f);
	pTransform->SetWorldScale(1.25f, 0.1f, 1.f);

	RESOLUTION_TYPE eType = DX11_RESOLUTION;
	pTransform->SetWorldPos(50.f+100.f, g_tRS[eType].iHeight - 80.f, 0.f);
	SAFE_RELEASE(pTransform);

	CDx11UIBackground*	pUI = m_pShadowBar->AddComponent<CDx11UIBackground>("UIBack");
	SAFE_RELEASE(pUI);

	CDx11Layer* pLayer = m_pScene->FindLayer("UILayer");
	pLayer->AddObject(m_pShadowBar);
	SAFE_RELEASE(pLayer);


	// HP Bar
	m_pHPBar = CDx11GameObject::Create("MyHPBar");

	pRenderer = m_pHPBar->AddComponent<CDx11Renderer>("HPBarRenderer");
	pRenderer->CreateMaterial("HPBarRed", L"UI/Unit Frames/UnitFrame_Energy_Bar_Fill_Red.png");
	pRenderer->SetMesh(RECT_UI_MESH);
	pRenderer->SetShader(UI_SHADER);
	pRenderer->SetRenderState(DX11_RS_ALPHABLEND);
	pRenderer->SetRenderState(DX11_RS_DEPTH_DISABLE);
	SAFE_RELEASE(pRenderer);

	pTransform = m_pHPBar->GetTransform();
	pTransform->SetLocalScale(300.f, 800.f, 1.f);
	pTransform->SetWorldScale(1.25f, 0.1f, 1.f);

	pTransform->SetWorldPos(50.f +100.f, g_tRS[eType].iHeight - 80.f, 0.f);
	SAFE_RELEASE(pTransform);

	pUI = m_pHPBar->AddComponent<CDx11UIBackground>("UIBack");
	SAFE_RELEASE(pUI);

	pLayer = m_pScene->FindLayer("UILayer");
	pLayer->AddObject(m_pHPBar);
	SAFE_RELEASE(pLayer);


	// Bar Edge
	CDx11GameObject* pBarEdgeObj = CDx11GameObject::Create("MyBarEdge");

	pRenderer = pBarEdgeObj->AddComponent<CDx11Renderer>("BarEdgeRenderer");
	pRenderer->CreateMaterial("BarEdge", L"UI/Unit Frames/UnitFrame_Energy_Bar_Empty.png");
	pRenderer->SetMesh(RECT_UI_MESH);
	pRenderer->SetShader(UI_SHADER);
	pRenderer->SetRenderState(DX11_RS_ALPHABLEND);
	pRenderer->SetRenderState(DX11_RS_DEPTH_DISABLE);
	SAFE_RELEASE(pRenderer);

	pTransform = pBarEdgeObj->GetTransform();
	pTransform->SetLocalScale(305.f, 800.f, 1.f);
	pTransform->SetWorldScale(1.25f, 0.1f, 1.f);

	pTransform->SetWorldPos(47.5f + 100.f, g_tRS[eType].iHeight - 80.f, 0.f);
	SAFE_RELEASE(pTransform);

	pUI = pBarEdgeObj->AddComponent<CDx11UIBackground>("UIBack");
	SAFE_RELEASE(pUI);

	pLayer = m_pScene->FindLayer("UILayer");
	pLayer->AddObject(pBarEdgeObj);
	SAFE_RELEASE(pLayer);


}

void CPlayer::CreatePlayeruUI()
{
	//// 1P 얼굴
	CDx11GameObject*	p1PFACE = CDx11GameObject::Create("1PUIBar");

	CDx11Renderer*	pRenderer = p1PFACE->AddComponent<CDx11Renderer>("ShadowBarRenderer");
	pRenderer->CreateMaterial("1PUI", L"UI/ch_blue.png");
	pRenderer->SetMesh(RECT_UI_MESH);
	pRenderer->SetShader(UI_SHADER);
	pRenderer->SetRenderState(DX11_RS_ALPHABLEND);
	pRenderer->SetRenderState(DX11_RS_DEPTH_DISABLE);
	SAFE_RELEASE(pRenderer);

	CDx11Transform*	pTransform = p1PFACE->GetTransform();
	pTransform->SetLocalScale(124.f, 118.f, 1.f);//원본크기
	pTransform->SetWorldScale(1.f, 1.f, 1.f);//조정크기

	RESOLUTION_TYPE eType = DX11_RESOLUTION;
	pTransform->SetWorldPos(20.f, g_tRS[eType].iHeight - 120.f, 0.f);//위치
	SAFE_RELEASE(pTransform);

	CDx11UIBackground*	pUI = p1PFACE->AddComponent<CDx11UIBackground>("UIBack");
	SAFE_RELEASE(pUI);

	CDx11Layer* pLayer = m_pScene->FindLayer("UILayer");
	pLayer->AddObject(p1PFACE);
	SAFE_RELEASE(pLayer);
	SAFE_RELEASE(p1PFACE);

	//무기 ui
	CDx11GameObject*	p1MSSILE = CDx11GameObject::Create("mssile");

	pRenderer = p1MSSILE->AddComponent<CDx11Renderer>("HPBarRenderer");
	pRenderer->CreateMaterial("mssile", L"UI/miisle0.png");
	pRenderer->SetMesh(RECT_UI_MESH);
	pRenderer->SetShader(UI_SHADER);
	pRenderer->SetRenderState(DX11_RS_ALPHABLEND);
	pRenderer->SetRenderState(DX11_RS_DEPTH_DISABLE);
	SAFE_RELEASE(pRenderer);

	pTransform = p1MSSILE->GetTransform();
	pTransform->SetLocalScale(158.f, 100.f, 1.f);
	pTransform->SetWorldScale(1.0f, 1.25f, 1.f);

	eType = DX11_RESOLUTION;
	pTransform->SetWorldPos(1100.f, g_tRS[eType].iHeight - 130.f, 0.f);
	SAFE_RELEASE(pTransform);

	pUI = p1MSSILE->AddComponent<CDx11UIBackground>("UIBack");
	SAFE_RELEASE(pUI);

	pLayer = m_pScene->FindLayer("UILayer");
	pLayer->AddObject(p1MSSILE);
	SAFE_RELEASE(pLayer);
	SAFE_RELEASE(p1MSSILE);

	//왼쪽 날개
	CDx11GameObject*	pleft_wing = CDx11GameObject::Create("left_wing");

	pRenderer = pleft_wing->AddComponent<CDx11Renderer>("left_wingRenderer");
	pRenderer->CreateMaterial("left_wing", L"UI/left_wing.png");
	pRenderer->SetMesh(RECT_UI_MESH);
	pRenderer->SetShader(UI_SHADER);
	pRenderer->SetRenderState(DX11_RS_ALPHABLEND);
	pRenderer->SetRenderState(DX11_RS_DEPTH_DISABLE);
	SAFE_RELEASE(pRenderer);

	pTransform = pleft_wing->GetTransform();
	pTransform->SetLocalScale(44.f, 138.f, 1.f);
	pTransform->SetWorldScale(0.9f, 1.8f, 1.f);

	eType = DX11_RESOLUTION;
	pTransform->SetWorldPos(100.f, g_tRS[eType].iHeight - 500.f, 0.f);
	SAFE_RELEASE(pTransform);

	pUI = pleft_wing->AddComponent<CDx11UIBackground>("UIBack");
	SAFE_RELEASE(pUI);

	pLayer = m_pScene->FindLayer("UILayer");
	pLayer->AddObject(pleft_wing);
	SAFE_RELEASE(pLayer);
	SAFE_RELEASE(pleft_wing);

	//오른쪽 날개
	CDx11GameObject*	pRight_wing = CDx11GameObject::Create("right_wing");

	pRenderer = pRight_wing->AddComponent<CDx11Renderer>("right_wingRenderer");
	pRenderer->CreateMaterial("right_wing", L"UI/right_wing.png");
	pRenderer->SetMesh(RECT_UI_MESH);
	pRenderer->SetShader(UI_SHADER);
	pRenderer->SetRenderState(DX11_RS_ALPHABLEND);
	pRenderer->SetRenderState(DX11_RS_DEPTH_DISABLE);
	SAFE_RELEASE(pRenderer);

	pTransform = pRight_wing->GetTransform();
	pTransform->SetLocalScale(44.f, 138.f, 1.f);
	pTransform->SetWorldScale(0.9f, 1.8f, 1.f);

	eType = DX11_RESOLUTION;
	pTransform->SetWorldPos(1280.f-150.f, g_tRS[eType].iHeight - 500.f, 0.f);
	SAFE_RELEASE(pTransform);

	pUI = pRight_wing->AddComponent<CDx11UIBackground>("UIBack");
	SAFE_RELEASE(pUI);

	pLayer = m_pScene->FindLayer("UILayer");
	pLayer->AddObject(pRight_wing);
	SAFE_RELEASE(pLayer);
	SAFE_RELEASE(pRight_wing);

	//player1 표시창
	CDx11GameObject*	pPlayer_1 = CDx11GameObject::Create("Player_1");

	pRenderer = pPlayer_1->AddComponent<CDx11Renderer>("player1Renderer");
	pRenderer->CreateMaterial("Player_1", L"UI/player1.png");
	pRenderer->SetMesh(RECT_UI_MESH);
	pRenderer->SetShader(UI_SHADER);
	pRenderer->SetRenderState(DX11_RS_ALPHABLEND);
	pRenderer->SetRenderState(DX11_RS_DEPTH_DISABLE);
	SAFE_RELEASE(pRenderer);

	pTransform = pPlayer_1->GetTransform();
	pTransform->SetLocalScale(553.f, 100.f, 1.f);
	pTransform->SetWorldScale(0.25f, 0.3f, 1.f);

	eType = DX11_RESOLUTION;
	pTransform->SetWorldPos(157.f, g_tRS[eType].iHeight - 110.f, 0.f);
	SAFE_RELEASE(pTransform);

	pUI = pPlayer_1->AddComponent<CDx11UIBackground>("UIBack");
	SAFE_RELEASE(pUI);

	pLayer = m_pScene->FindLayer("UILayer");
	pLayer->AddObject(pPlayer_1);
	SAFE_RELEASE(pLayer);
	SAFE_RELEASE(pPlayer_1);

	//player2 표시창
	CDx11GameObject*	pPlayer_2 = CDx11GameObject::Create("player2");

	pRenderer = pPlayer_2->AddComponent<CDx11Renderer>("player2Renderer");
	pRenderer->CreateMaterial("player2", L"UI/player2.png");
	pRenderer->SetMesh(RECT_UI_MESH);
	pRenderer->SetShader(UI_SHADER);
	pRenderer->SetRenderState(DX11_RS_ALPHABLEND);
	pRenderer->SetRenderState(DX11_RS_DEPTH_DISABLE);
	SAFE_RELEASE(pRenderer);

	pTransform = pPlayer_2->GetTransform();
	pTransform->SetLocalScale(553.f, 100.f, 1.f);
	pTransform->SetWorldScale(0.15f, 0.15f, 1.f);

	eType = DX11_RESOLUTION;
	pTransform->SetWorldPos(13.f, g_tRS[eType].iHeight - 650.f, 0.f);
	SAFE_RELEASE(pTransform);

	pUI = pPlayer_2->AddComponent<CDx11UIBackground>("UIBack");
	SAFE_RELEASE(pUI);

	pLayer = m_pScene->FindLayer("UILayer");
	pLayer->AddObject(pPlayer_2);
	SAFE_RELEASE(pLayer);
	SAFE_RELEASE(pPlayer_2);

	//조작키 UI
	CDx11GameObject*	pINTERFACE = CDx11GameObject::Create("INTERFACE");

	pRenderer = pINTERFACE->AddComponent<CDx11Renderer>("INTERFACERenderer");
	pRenderer->CreateMaterial("INTERFACE", L"UI/75.png");
	pRenderer->SetMesh(RECT_UI_MESH);
	pRenderer->SetShader(UI_SHADER);
	pRenderer->SetRenderState(DX11_RS_ALPHABLEND);
	pRenderer->SetRenderState(DX11_RS_DEPTH_DISABLE);
	SAFE_RELEASE(pRenderer);

	pTransform = pINTERFACE->GetTransform();
	pTransform->SetLocalScale(256.f, 193.f, 1.f);
	pTransform->SetWorldScale(1.f, 1.f, 1.f);

	eType = DX11_RESOLUTION;
	pTransform->SetWorldPos(1020.f, g_tRS[eType].iHeight - 300.f, 0.f);
	SAFE_RELEASE(pTransform);

	pUI = pINTERFACE->AddComponent<CDx11UIBackground>("UIBack");
	SAFE_RELEASE(pUI);

	pLayer = m_pScene->FindLayer("UILayer");
	pLayer->AddObject(pINTERFACE);
	SAFE_RELEASE(pLayer);
	SAFE_RELEASE(pINTERFACE);
}

void CPlayer::CreateUserUI()//임시(2피틀기힘듬 ㅠㅠ)
{
	// 2P 얼굴
	CDx11GameObject*	p2PFACE = CDx11GameObject::Create("2PUIBar");

	CDx11Renderer*	pRenderer = p2PFACE->AddComponent<CDx11Renderer>("2PUIRenderer");
	pRenderer->CreateMaterial("2PUI", L"UI/ch_red.png");
	pRenderer->SetMesh(RECT_UI_MESH);
	pRenderer->SetShader(UI_SHADER);
	pRenderer->SetRenderState(DX11_RS_ALPHABLEND);
	pRenderer->SetRenderState(DX11_RS_DEPTH_DISABLE);
	SAFE_RELEASE(pRenderer);

	CDx11Transform*	pTransform = p2PFACE->GetTransform();
	pTransform->SetLocalScale(124.f, 118.f, 1.f);//원본크기
	pTransform->SetWorldScale(0.6f, 0.5f, 1.f);//조정크기

	RESOLUTION_TYPE eType = DX11_RESOLUTION;
	pTransform->SetWorldPos(10.f, g_tRS[eType].iHeight - 710.f, 0.f);//위치
	SAFE_RELEASE(pTransform); 
	
	CDx11UIBackground*	pUI = p2PFACE->AddComponent<CDx11UIBackground>("UIBack");
	SAFE_RELEASE(pUI);

	CDx11Layer* pLayer = m_pScene->FindLayer("UILayer");
	pLayer->AddObject(p2PFACE);
	SAFE_RELEASE(pLayer);

	SAFE_RELEASE(p2PFACE);

	//2p 피UI@@	
	//m_pHPBar_User = CDx11GameObject::Create("HPBar_User");

	//pRenderer = m_pHPBar_User->AddComponent<CDx11Renderer>("HPBar_UserRenderer");
	//pRenderer->CreateMaterial("HPBarRed", L"UI/Unit Frames/UnitFrame_Energy_Bar_Fill_Red.png");
	//pRenderer->SetMesh(RECT_UI_MESH);
	//pRenderer->SetShader(UI_SHADER);
	//pRenderer->SetRenderState(DX11_RS_ALPHABLEND);
	//pRenderer->SetRenderState(DX11_RS_DEPTH_DISABLE);
	//SAFE_RELEASE(pRenderer);

	//pTransform = m_pHPBar_User->GetTransform();
	//pTransform->SetLocalScale(300.f, 800.f, 1.f);
	//pTransform->SetWorldScale(0.75f, 0.05f, 1.f);

	//pTransform->SetWorldPos(50.f + 40.f, g_tRS[eType].iHeight - 710.f, 0.f);
	//SAFE_RELEASE(pTransform);

	//pUI = m_pHPBar_User->AddComponent<CDx11UIBackground>("UIBack");
	//SAFE_RELEASE(pUI);

	//pLayer = m_pScene->FindLayer("UILayer");
	//pLayer->AddObject(m_pHPBar_User);
	//SAFE_RELEASE(pLayer);


	////// Bar Edge
	//CDx11GameObject* pBarEdgeObj_User = CDx11GameObject::Create("EdgeObj_User");

	//pRenderer = pBarEdgeObj_User->AddComponent<CDx11Renderer>("BarEdgeRenderer");
	//pRenderer->CreateMaterial("BarEdge", L"UI/Unit Frames/UnitFrame_Energy_Bar_Empty.png");
	//pRenderer->SetMesh(RECT_UI_MESH);
	//pRenderer->SetShader(UI_SHADER);
	//pRenderer->SetRenderState(DX11_RS_ALPHABLEND);
	//pRenderer->SetRenderState(DX11_RS_DEPTH_DISABLE);
	//SAFE_RELEASE(pRenderer);

	//pTransform = pBarEdgeObj_User->GetTransform();
	//pTransform->SetLocalScale(305.f, 800.f, 1.f);
	//pTransform->SetWorldScale(0.75f, 0.05f, 1.f);

	//pTransform->SetWorldPos(47.5f + 40.f, g_tRS[eType].iHeight - 710.f, 0.f);
	//SAFE_RELEASE(pTransform);

	//pUI = pBarEdgeObj_User->AddComponent<CDx11UIBackground>("UIBack");
	//SAFE_RELEASE(pUI);

	//pLayer = m_pScene->FindLayer("UILayer");
	//pLayer->AddObject(pBarEdgeObj_User);
	//SAFE_RELEASE(pLayer);


}

void CPlayer::SetTerrain(CDx11Terrain * pTerrain)
{
	m_pTerrain = pTerrain;
}


CDx11GameObject * CPlayer::CreateLaser(CDx11Transform * pTransform, const Vec3 & vDir)
{
	CDx11GameObject*	pLaser = CDx11GameObject::Create("LaserObject");
	CDx11Transform*		pLaserTr = pLaser->GetTransform();
	pLaserTr->Copy(pTransform);
	pLaserTr->MoveWorld(2.5f, 1.f, AXIS_Z);
	pLaserTr->SetWorldScale(0.05f, 0.05f, 0.05f);
	pLaserTr->SetWorldView(vDir);
	GET_SINGLE(CNetworkManager)->ShootPlayer(pLaserTr->GetWorldPos(), pLaserTr->GetWorldScale(), pLaserTr->GetWorldView());

	// 렌더러를 생성한다.
	CDx11Renderer* pRenderer = pLaser->AddComponent<CDx11Renderer>("LaserRenderer");
	pRenderer->SetMesh("MissileMesh");
	pRenderer->SetShader(DEFAULT_BUMP_SHADER);
	SAFE_RELEASE(pRenderer);

	// 총알 스크립트를 컴포넌트에 추가한다.
	CLaser*	pLaserSc = (CLaser*)pLaser->AddComponent<CLaser>("LaserScript");
	SAFE_RELEASE(pLaserSc);

	// 충돌체를 설정한다.
	CDx11ColliderOBB* pOBB = pLaser->AddComponent<CDx11ColliderOBB>("MyLaserColl");
	float fAxisLen[3];
	fAxisLen[0] = 0.5f;
	fAxisLen[1] = 1.f;
	fAxisLen[2] = 0.5f;
	pOBB->SetOBBInfo(fAxisLen, pLaserTr);
	SAFE_RELEASE(pLaserTr);
	SAFE_RELEASE(pOBB);

	m_pLayer->AddObject(pLaser);
	return pLaser;
}

CDx11GameObject * CPlayer::CreateGuidedMissile(CDx11Transform * pTransform, const Vec3 & vDir)
{
	CDx11GameObject*	pMissile = CDx11GameObject::Create("MissilePrototype", OT_CLONE);
	CDx11Transform*		pMissileTr = pMissile->GetTransform();
	pMissileTr->Copy(pTransform);

	pMissileTr->MoveWorld(2.f, 1.f, pMissileTr->GetWorldAxis(AXIS_Z));
	pMissileTr->SetWorldScale(0.15f, 0.2f, 0.15f);
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
	m_bControl = true;

	CreateBar();
	CreatePlayeruUI();
	CreateUserUI();//임시!

	return true;
}

void CPlayer::Input(float fTime)
{
	int iStateFlag = m_pGameObject->GetStateFlag();
	if ((iStateFlag != STATE_ROLL) && !(iStateFlag & STATE_JUMP) && !(iStateFlag & STATE_LYING))
		m_pGameObject->SetStateFlag(STATE_IDLE);

	if ((iStateFlag & STATE_ROLL))
		MoveDash(fTime);


	if (DX11_GET_SINGLE(CDx11Input)->KeyDown("MouseLButton"))
	{
		DX11_GET_SINGLE(Dx11SoundManager)->PlayFIRE();

		if (!m_bLeft)
		{
			m_bLeft = true;
			m_ptOldMouse = DX11_GET_SINGLE(CDx11Input)->GetMousePoint();
		}
	}
	if (DX11_GET_SINGLE(CDx11Input)->KeyUp("MouseLButton"))
	{
		m_pGameObject->DeleteStateFlag(STATE_SHOOT);
		m_bLeft = false;
	}

	m_bControl = m_pGameObject->GetControl();
	if (m_bControl)
	{


		if (DX11_GET_SINGLE(CDx11Input)->KeyPush("MouseLButton"))
		{
			static int temp = 0;
			temp++;
			if (temp % 20 == 0) {
				DX11_GET_SINGLE(Dx11SoundManager)->PlayFIRE();
			}

			m_pGameObject->AddStateFlag(STATE_SHOOT);

			if (!m_bMissile)
			{
				CDx11Layer*	pUILayer = m_pScene->FindLayer("UILayer");
				CDx11GameObject* pObj = pUILayer->FindGameObjectFromTag("AimObj");
				CDx11ColliderRay* pRay = (CDx11ColliderRay*)pObj->FindComponentFromTypeName("CDx11ColliderRay");
				pRay->SetStartPos(m_pTransform->GetWorldPos());
				RAY tRay = pRay->GetRay();

				ShootMissile(tRay.vDir, fTime);

				//QuadTree* pQuadTree = DX11_GET_SINGLE(CDx11SceneManager)->GetQuadTree();
				//pQuadTree->AddDynamicObj(pObj);
				//SAFE_RELEASE(pQuadTree);

				SAFE_RELEASE(pRay);
				SAFE_RELEASE(pObj);
				SAFE_RELEASE(pUILayer);

				m_bMissile = true;
			}
		}

		if (DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveLeft") &&
			DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveFront") &&
			!DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveRight"))
		{
			static int temp = 0;
			temp++;
			if (temp % 20 == 0) {
				DX11_GET_SINGLE(Dx11SoundManager)->PlayWALK1();
			}
			if (!m_bLeft)
			{
				TurnToDirection(DIR_FRONT_LEFT, fTime);
				MoveFront(fTime);
			}
			else
			{

			}
		}
		if (DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveRight") &&
			DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveFront") &&
			!DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveLeft"))
		{
			static int temp = 0;
			temp++;
			if (temp % 20 == 0) {
				DX11_GET_SINGLE(Dx11SoundManager)->PlayWALK1();
			}
			if (!m_bLeft)
			{
				TurnToDirection(DIR_FRONT_RIGHT, fTime);
				MoveFront(fTime);
			}
			else
			{

			}
		}
		if (DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveLeft") &&
			!DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveRight") &&
			!DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveFront") &&
			!DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveBack"))
		{
			static int temp = 0;
			temp++;
			if (temp % 20 == 0) {
				DX11_GET_SINGLE(Dx11SoundManager)->PlayWALK1();
			}
			if (!m_bLeft)
			{
				TurnToDirection(DIR_LEFT, fTime);
				MoveFront(fTime);
			}
			else
			{

			}
		}
		if (DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveRight") &&
			!DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveLeft") &&
			!DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveFront") &&
			!DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveBack"))
		{
			static int temp = 0;
			temp++;
			if (temp % 20 == 0) {
				DX11_GET_SINGLE(Dx11SoundManager)->PlayWALK1();
			}
			if (!m_bLeft)
			{
				TurnToDirection(DIR_RIGHT, fTime);
				MoveFront(fTime);
			}
			else
			{

			}
		}
		if (DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveFront") &&
			!DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveLeft") &&
			!DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveRight") &&
			!DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveBack"))
		{
			static int temp = 0;
			temp++;
			if (temp % 20 == 0) {
				DX11_GET_SINGLE(Dx11SoundManager)->PlayWALK1();
			}
			if (!m_bLeft)
			{
				TurnToDirection(DIR_FRONT, fTime);
			}

			MoveFront(fTime);
			TraceCameraView(fTime);

		}
		if (DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveBack") &&
			!DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveLeft") &&
			!DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveRight") &&
			!DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveFront"))
		{
			static int temp = 0;
			temp++;
			if (temp % 20 == 0) {
				DX11_GET_SINGLE(Dx11SoundManager)->PlayWALK1();
			}
			if (!m_bLeft)
			{
				TurnToDirection(DIR_BACK, fTime);
				MoveFront(fTime);
			}
			else
			{

			}
		}
		if (DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveLeft") &&
			DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveBack") &&
			!DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveRight"))
		{
			static int temp = 0;
			temp++;
			if (temp % 20 == 0) {
				DX11_GET_SINGLE(Dx11SoundManager)->PlayWALK1();
			}
			if (!m_bLeft)
			{
				TurnToDirection(DIR_BACK_LEFT, fTime);
				MoveFront(fTime);
			}
			else
			{

			}
		}
		if (DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveRight") &&
			DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveBack") &&
			!DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveLeft"))
		{
			static int temp = 0;
			temp++;
			if (temp % 20 == 0) {
				DX11_GET_SINGLE(Dx11SoundManager)->PlayWALK1();
			}
			if (!m_bLeft)
			{
				TurnToDirection(DIR_BACK_RIGHT, fTime);
				MoveFront(fTime);
			}
			else
			{

			}
		}

		if (DX11_GET_SINGLE(CDx11Input)->KeyDown("RotYBack") ||
			DX11_GET_SINGLE(CDx11Input)->KeyPush("RotYBack"))
		{
			RotatePlayer(DX11_PI / 2.f, fTime, AXIS_Y);
		}
		if (DX11_GET_SINGLE(CDx11Input)->KeyDown("RotYFront") ||
			DX11_GET_SINGLE(CDx11Input)->KeyPush("RotYFront"))
		{
			RotatePlayer(-DX11_PI / 2.f, fTime, AXIS_Y);
		}
		if (DX11_GET_SINGLE(CDx11Input)->KeyDown("RotXBack") ||
			DX11_GET_SINGLE(CDx11Input)->KeyPush("RotXBack"))
		{
			RotatePlayer(DX11_PI / 2.f, fTime, AXIS_X);
		}
		if (DX11_GET_SINGLE(CDx11Input)->KeyDown("RotXFront") ||
			DX11_GET_SINGLE(CDx11Input)->KeyPush("RotXFront"))
		{
			RotatePlayer(-DX11_PI / 2.f, fTime, AXIS_X);
		}
		if (DX11_GET_SINGLE(CDx11Input)->KeyDown("Skill1"))
		{
			if (m_iPowerUP < 2)
				++m_iPowerUP;
			else
				m_iPowerUP = 0;
		}
		if (DX11_GET_SINGLE(CDx11Input)->KeyDown("Jump") ||
			DX11_GET_SINGLE(CDx11Input)->KeyPush("Jump"))
		{		
			m_pGameObject->AddStateFlag(STATE_JUMP);
		}
		if (DX11_GET_SINGLE(CDx11Input)->KeyUp("Jump"))
		{		
			DX11_GET_SINGLE(Dx11SoundManager)->PlayJUMP();
			
			m_pGameObject->AddStateFlag(STATE_JUMP);
		}
		if (DX11_GET_SINGLE(CDx11Input)->KeyDown("Roll") ||
			DX11_GET_SINGLE(CDx11Input)->KeyPush("Roll"))
		{
			
			DX11_GET_SINGLE(Dx11SoundManager)->PlayDumpling();
			m_pGameObject->SetStateFlag(STATE_ROLL);
			m_pGameObject->SetControl(false);
		}
		if (DX11_GET_SINGLE(CDx11Input)->KeyDown("Lying") ||
			DX11_GET_SINGLE(CDx11Input)->KeyPush("Lying"))
		{
			m_pGameObject->AddStateFlag(STATE_LYING);
		}
	
	}

	// 미사일 발사 시 카메라 뷰로 회전
	if (iStateFlag & STATE_SHOOT)
		TraceCameraView(fTime * 8);

}

int CPlayer::Update(float fTime)
{
	// 미사일 발사 딜레이 조정
	ComputeShotDelay(m_bShot, m_fShotDelay, fTime, 0.2f);
	ComputeShotDelay(m_bMissile, m_fMissileDelay, fTime, 0.2f);


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

	//UpdateUserHP();


	// 지형 타기
	//if (m_pTerrain)
	//{
	//	Vec3	vPos = m_pTransform->GetWorldPos();
	//	if (vPos.y > m_pTerrain->GetHeight(m_pTransform->GetWorldPos()) + 0.3f)
	//	{
	//		vPos.y -= fTime;
	//		m_pTransform->SetWorldPos(vPos);
	//	}
	//	else if (vPos.y < m_pTerrain->GetHeight(m_pTransform->GetWorldPos()))
	//	{
	//		vPos.y += fTime;
	//		m_pTransform->SetWorldPos(vPos);
	//	}
	//}
	return 0;
}

int CPlayer::LateUpdate(float fTime)
{

	if (UpdateFlag == true) 
	{
	
			GET_SINGLE(CNetworkManager)->RotatePlayer(m_pTransform->GetWorldRot());
			GET_SINGLE(CNetworkManager)->MovePlayer(m_pTransform->GetWorldPos());
			GET_SINGLE(CNetworkManager)->AnimState(m_pGameObject->GetStateFlag());

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
	if (pColl->GetTag() == "UserLaserColl")
	{
		CDx11GameObject*	pObj = pColl->GetGameObject();
		//CLaser*	pLaser = (CLaser*)pObj->FindComponentFromType(CT_SCRIPT);
		//
		//pLaser->DeathMotion();
		//pObj->Death();
		//this->SubtractHP(1);
		////ShakeCamera();

		//SAFE_RELEASE(pLaser);
		//SAFE_RELEASE(pObj);
	}
}

void CPlayer::OnCollision(CDx11Collider * pColl, float fTime)
{

}

void CPlayer::OnCollisionExit(CDx11Collider * pColl, float fTime)
{

}
