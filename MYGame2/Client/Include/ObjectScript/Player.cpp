#include "Player.h"
#include "Component/Dx11Transform.h"
#include "GameObject/Dx11GameObject.h"
#include "Scene/Dx11Layer.h"
#include "Core/Dx11Input.h"
#include "Scene/Dx11Scene.h"

CPlayer::CPlayer()
	: GoBullet(false), count(0), OriginalPos(0.f, 0.f, 0.f),
	PlayerPos(0, 0, 0),
	PlayerRot(0, 0, 0),
	New(0, 0, 0) 	
	,m_pHPBar(NULL)
	,m_pHPSlowBar(NULL)
	, m_bUIHpBarDown(false)
	, m_bUIHpBarDown2(false)
	, m_bUIHpBarUP(false)
	, m_bUIHpBarUP2(false)
	, m_fcount(0)
	, m_fcount2(0)
	, m_fcount3(0)
	, m_fcount4(0)
	, m_fDamage(0)
	, m_fDamage2(0)
	, m_btemp(false)
	, m_btemp2(false)
	, m_fHPUP(0)
	, m_fHPUP2(0)
	, m_fHPUP3(0)
	, m_fHPUP4(0)
{
}

CPlayer::~CPlayer()
{
	SAFE_RELEASE(m_pHPSlowBar);
	SAFE_RELEASE(m_pHPBar);
	
}

void CPlayer::SetHPBar(CDx11UIBar * pBar)
{
	//Defalt HP BAR
	m_pHPBar = pBar;//어떤건지 여기서 찾네..
	pBar->AddRef();//넣어주니깐 래퍼런스 증가

	pBar->SetValueMinMax(0.f, m_iHPMax);
	pBar->SetValue(m_iHP);
}

void CPlayer::SetSlowHPBar(CDx11UIBar * pBar)
{
	//m_pHPSlowBar
	m_pHPSlowBar = pBar;//어떤건지 여기서 찾네..
	pBar->AddRef();//넣어주니깐 래퍼런스 증가

	pBar->SetValueMinMax(0.f, m_iHPMax);
	pBar->SetValue(m_iHP2);
}


void CPlayer::Damage(float iDamage)
{
	if (m_bUIHpBarDown) {
		m_fcount = 2.f;
		m_fDamage -= m_fcount;
		m_iHP -= m_fcount;

		if (m_iHP < 0)
			m_iHP = 0;

		if (m_fDamage <= 0) {
			m_bUIHpBarDown = false;
		}
		m_pHPBar->SetValue((float)m_iHP);
	}
	else {
		m_fDamage = iDamage;
	}
}

void CPlayer::SlowDamage(float iDamage)
{
	if (m_bUIHpBarDown2) {

		m_fcount2 = 2.f;
		m_fDamage2 -= m_fcount2;
		m_iHP2 -= m_fcount2;

		if (m_iHP2 < 0)
			m_iHP2 = 0;

		if (m_fDamage2 <= 0) {
			m_bUIHpBarDown2 = false;
			m_iHP2 = m_iHP;
		}

		m_pHPSlowBar->SetValue((float)m_iHP2);
	}
	else {
		m_fDamage2 = iDamage;
	}
}

void CPlayer::AddHP(float iHP)
{
	if (m_bUIHpBarUP) {
		m_fcount3 = 2.f;
		m_fHPUP3 -= m_fcount3;
		m_iHP += m_fcount3;

		if (m_iHP < 0)
			m_iHP = 0;

		if (m_fHPUP3 <= 0) {
			m_bUIHpBarUP = false;
		}
		m_pHPBar->SetValue((float)m_iHP);
	}
	else {
		m_fHPUP3 = iHP;
	}
}

void CPlayer::AddSleepHP(float iHP)
{
	if (m_bUIHpBarUP2) {
		m_fcount4 = 2.f;
		m_fHPUP4 -= m_fcount4;
		m_iHP2 += m_fcount4;

		if (m_iHP2 < 0)
			m_iHP2 = 0;

		if (m_fHPUP4 <= 0) {
			m_bUIHpBarUP2 = false;
			m_iHP2 = m_iHP;
		}
		m_pHPSlowBar->SetValue((float)m_iHP2);
	}
	else {
		m_fHPUP4 = iHP;
	}
}

void CPlayer::SleepGo() {
	static float temp;

	temp++;
	if (temp > 20) {
		m_bUIHpBarDown2 = true;
		temp = 0;
		m_btemp = false;
	}
	else {
		return;
	}
}
void CPlayer::SleepAddGo()
{
	static float temp2;

	temp2++;
	if (temp2 > 20) {
		m_bUIHpBarUP2 = true;
		temp2 = 0;
		m_btemp2 = false;
	}
	else {
		return;
	}

}

bool CPlayer::Init()
{
	DX11_GET_SINGLE(CDx11Input)->CreateKey("RotYFront", 'E');
	DX11_GET_SINGLE(CDx11Input)->CreateKey("RotYBack", 'Q');
	DX11_GET_SINGLE(CDx11Input)->CreateKey("RotXFront", 'R');
	DX11_GET_SINGLE(CDx11Input)->CreateKey("RotXBack", 'F');
	DX11_GET_SINGLE(CDx11Input)->CreateKey("Skill2", '2');

	m_fSolWSpeed = 2.f;
	m_fSolWLimit = 2.f;
	m_fSolWAcc = 0.f;
	m_bSolWEnable = false;
	m_fFireTime = 0.f;
	m_bFire = false;
	
	//UI HP
	m_fSolWSpeed = 2.f;
	m_fSolWLimit = 2.f;
	m_fSolWAcc = 0.f;
	m_bSolWEnable = false;
	m_fFireTime = 0.f;
	m_bFire = false;
	m_iHP = 1000;
	m_iHP2 = 1000;
	m_iHP3 = 1000;
	m_iHP4 = 1000;
	m_iHPMax = 1000;


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

	if (DX11_GET_SINGLE(CDx11Input)->KeyDown("RotXBack") ||
		DX11_GET_SINGLE(CDx11Input)->KeyPush("RotXBack"))
	{
		m_pTransform->RotationWorld(DX11_PI, fTime, AXIS_X);
	}

	if (DX11_GET_SINGLE(CDx11Input)->KeyDown("RotXFront") ||
		DX11_GET_SINGLE(CDx11Input)->KeyPush("RotXFront"))
	{
		m_pTransform->RotationWorld(-DX11_PI, fTime, AXIS_X);
	}

	if (DX11_GET_SINGLE(CDx11Input)->KeyDown("RotYBack") ||
		DX11_GET_SINGLE(CDx11Input)->KeyPush("RotYBack"))
	{
		m_pTransform->RotationWorld(DX11_PI, fTime, AXIS_Y);
	}

	if (DX11_GET_SINGLE(CDx11Input)->KeyDown("RotYFront") ||
		DX11_GET_SINGLE(CDx11Input)->KeyPush("RotYFront"))
	{
		m_pTransform->RotationWorld(-DX11_PI, fTime, AXIS_Y);
	}

	if (DX11_GET_SINGLE(CDx11Input)->KeyDown("MoveFront") ||
		DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveFront"))
	{
		m_pTransform->MoveWorld(50.f, fTime, AXIS_Z);
	}

	if (DX11_GET_SINGLE(CDx11Input)->KeyDown("MoveBack") ||
		DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveBack"))
	{
		m_pTransform->MoveWorld(-50.f, fTime, AXIS_Z);
	}

	if (DX11_GET_SINGLE(CDx11Input)->KeyDown("Fire") ||
		DX11_GET_SINGLE(CDx11Input)->KeyPush("Fire"))
	{
		if (!m_bFire)
		{
			Fire();
			m_bFire = true;
		}
		//카메라 흔들흔들~
		GoBullet = true;
	}

	if (DX11_GET_SINGLE(CDx11Input)->KeyDown("Skill1"))
	{
		m_bSolWEnable = true;
		//SolW(fTime);
		//Fire();
	}

	if (DX11_GET_SINGLE(CDx11Input)->KeyDown("Skill2"))
	{
		//m_bSolWEnable = true;
		Skill2();
		//SolW(fTime);
		//Fire();
	}

	//UI HP BAR Control	
	static bool	 bF7, bF8;

	if (GetAsyncKeyState(VK_F7) & 0x8000)
	{
		bF7 = true;
	}
	else if (bF7)
	{
		bF7 = false;
		m_fcount = 0;
		m_bUIHpBarDown = true;
		m_btemp = true;
	}

	if (GetAsyncKeyState(VK_F8) & 0x8000)
	{
		bF8 = true;
	}

	else if (bF8)
	{
		bF8 = false;
		m_fcount2 = 0;
		m_bUIHpBarUP = true;
		m_btemp2 = true;
	}
}

int CPlayer::Update(float fTime)
{
	if (m_bSolWEnable)
	{
		SolW(fTime);
	}

	if (m_bFire)
	{
		m_fFireTime += fTime;

		if (m_fFireTime >= 0.2f)
		{
			m_bFire = false;
			m_fFireTime = 0.f;
		}
	}
	//카메라 흔들흔들~
	/*CDx11Transform* pCamTr = m_pScene->GetMainCamera()->GetTransform();

	if (!GoBullet) {
		OriginalPos = pCamTr->GetWorldPos();
	}
	else if (GoBullet) {
		if (count < 5) {
			++count;
			Vec3 TempPos = Vec3(pCamTr->GetWorldPos().x, pCamTr->GetWorldPos().y, pCamTr->GetWorldPos().z);
			TempPos.y += 0.025;
			pCamTr->SetWorldPos(TempPos);
		}
		else {
			GoBullet = false;
			count = 0;
			pCamTr->SetWorldPos(OriginalPos);
		}
	}
	SAFE_RELEASE(pCamTr);*/

	
	return 0;
}


int CPlayer::LateUpdate(float fTime)
{

	if (m_btemp) {
		SleepGo();//m_bUIHpBarDown2 타이머 걸어주기위해...ㅠㅠ
	}
	if (m_btemp2) {
		SleepAddGo();//m_bUIHpBarDown2 타이머 걸어주기위해...ㅠㅠ
	}



	//UI HP 체력감소 + 슬로우 체력감소
	Damage(100.f);
	SlowDamage(100.f);

	//UI HP 체력증가
	AddHP(100.f);
	AddSleepHP(100.f);

	return 0;
}

void CPlayer::Render(float fTime)
{
}

CPlayer * CPlayer::Clone()
{
	return new CPlayer(*this);
}

void CPlayer::Fire()
{
	CDx11GameObject*	pBullet = CreateBullet(m_pTransform);
	SAFE_RELEASE(pBullet);

	// 자식 비행기 2명의 Transform을 얻어온다.
	CDx11GameObject*	pChild = m_pGameObject->FindChildFromTag("LeftChild");
	CDx11Transform*	pChildTr = pChild->GetTransform();
	pBullet = CreateBullet(pChildTr);
	SAFE_RELEASE(pBullet);
	SAFE_RELEASE(pChildTr);
	SAFE_RELEASE(pChild);

	pChild = m_pGameObject->FindChildFromTag("RightChild");
	pChildTr = pChild->GetTransform();
	pBullet = CreateBullet(pChildTr);
	SAFE_RELEASE(pBullet);
	SAFE_RELEASE(pChildTr);
	SAFE_RELEASE(pChild);
}

void CPlayer::SolW(float fTime)
{
	CDx11GameObject*	pChild = m_pGameObject->FindChildFromTag("LeftChild");
	CDx11Transform*	pChildTr = pChild->GetTransform();

	CDx11GameObject*	pRChild = m_pGameObject->FindChildFromTag("RightChild");
	CDx11Transform*	pRChildTr = pRChild->GetTransform();

	float	fSpeed = m_fSolWSpeed * fTime;
	m_fSolWAcc += fSpeed;

	//_cprintf("Acc : %.5f\n", m_fSolWAcc);

	Vec3	vMove(-fSpeed, 0.f, 0.f);
	Vec3	vMove1(fSpeed, 0.f, 0.f);

	if (m_fSolWAcc >= m_fSolWLimit)
	{
		m_bSolWEnable = false;
		vMove.x -= (m_fSolWAcc - m_fSolWLimit);
		vMove1.x += (m_fSolWAcc - m_fSolWLimit);
		m_fSolWAcc = 0.f;
	}

	pChildTr->MoveWorld(vMove);
	pRChildTr->MoveWorld(vMove1);

	SAFE_RELEASE(pChildTr);
	SAFE_RELEASE(pChild);

	SAFE_RELEASE(pRChildTr);
	SAFE_RELEASE(pRChild);
}

void CPlayer::Skill2()
{
	CDx11GameObject*	pSkill2 = CDx11GameObject::Create("Skill2Prototype",
		OT_CLONE);

	CDx11Transform*	pBulletTr = pSkill2->GetTransform();

	pBulletTr->Copy(m_pTransform);
	pBulletTr->SetWorldScale(0.5f, 0.5f, 0.5f);
	pBulletTr->SetWorldView(m_pTransform->GetWorldAxis(AXIS_Y));
	//pBulletTr->MoveWorld(0.f, 0.5f, AXIS_Y);

	SAFE_RELEASE(pBulletTr);

	m_pLayer->AddObject(pSkill2);

	SAFE_RELEASE(pSkill2);
}

CDx11GameObject * CPlayer::CreateBullet(CDx11Transform * pTransform)
{
	CDx11GameObject*	pBullet = CDx11GameObject::Create("BulletPrototype",
		OT_CLONE);

	CDx11Transform*	pBulletTr = pBullet->GetTransform();

	pBulletTr->Copy(pTransform);
	pBulletTr->MoveWorld(1.5f, 1.f, AXIS_Z);
	pBulletTr->SetWorldScale(100.3f, 100.3f, 100.3f);
	pBulletTr->SetWorldView(pTransform->GetWorldAxis(AXIS_Z));
	

	SAFE_RELEASE(pBulletTr);

	m_pLayer->AddObject(pBullet);

	return pBullet;
}
