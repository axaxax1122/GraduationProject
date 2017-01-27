#include "Monster.h"
#include "GameObject/Dx11GameObject.h"
#include "Component/Dx11Transform.h"
#include "Player.h"
#include "Scene/Dx11Layer.h"
#include "Scene/Dx11Scene.h"


CMonster::CMonster()	:
	m_fSpeed(0),
	m_vDir(Vec3Zero)
{
}

CMonster::CMonster(const CMonster & monster)	:
	CDx11Script(monster)
{
	m_fSpeed = monster.m_fSpeed;
}


CMonster::~CMonster()
{
}

void CMonster::SetSpeed(float fSpeed)
{
	m_fSpeed = fSpeed;
}

void CMonster::SetDirection(const Vec3 & v)
{
	m_vDir = v;
}

bool CMonster::Init()
{
	m_fSpeed = 1.f;
	m_vDir = Vec3Y;

	return true;
}

void CMonster::Input(float fTime)
{
}

int CMonster::Update(float fTime)
{
	// 플레이어 오브젝트를 얻어온다.
	CDx11GameObject*	pPlayerObj = m_pScene->FindGameObjectFromTag("PlayerObject");
	CDx11Transform*		pTr = pPlayerObj->GetTransform();

	// 플레이어의 위치를 얻어온다.
	Vec3 vPlayerPos = pTr->GetWorldPos();
	// m_vDir을 구한다.
	SetDirection(vPlayerPos - m_pTransform->GetWorldPos());

	// m_vDir과 몬스터의 Y축을 내적
	float fDot = m_vDir.Dot(m_pTransform->GetWorldAxis(AXIS_Y));
	float fAngle = acos(fDot / m_vDir.Length());

	// 몬스터의 Y축과 m_vDir을 외적해서 회전 축을 구한다.
	Vec3 vRotation = m_pTransform->GetWorldAxis(AXIS_Y).Cross(m_vDir);

	m_pTransform->RotationWorld(fAngle, fTime, vRotation);
	SAFE_RELEASE(pTr);
	SAFE_RELEASE(pPlayerObj);
	return 0;
}

int CMonster::LateUpdate(float fTime)
{
	return 0;
}

void CMonster::Render(float fTime)
{
}

CMonster * CMonster::Clone()
{
	return new CMonster(*this);
}
