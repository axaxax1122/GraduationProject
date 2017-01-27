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
	// �÷��̾� ������Ʈ�� ���´�.
	CDx11GameObject*	pPlayerObj = m_pScene->FindGameObjectFromTag("PlayerObject");
	CDx11Transform*		pTr = pPlayerObj->GetTransform();

	// �÷��̾��� ��ġ�� ���´�.
	Vec3 vPlayerPos = pTr->GetWorldPos();
	// m_vDir�� ���Ѵ�.
	SetDirection(vPlayerPos - m_pTransform->GetWorldPos());

	// m_vDir�� ������ Y���� ����
	float fDot = m_vDir.Dot(m_pTransform->GetWorldAxis(AXIS_Y));
	float fAngle = acos(fDot / m_vDir.Length());

	// ������ Y��� m_vDir�� �����ؼ� ȸ�� ���� ���Ѵ�.
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
