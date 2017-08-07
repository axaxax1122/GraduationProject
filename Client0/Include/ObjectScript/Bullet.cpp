#include "Bullet.h"
#include "GameObject/Dx11GameObject.h"
#include "Component/Dx11Transform.h"

CBullet::CBullet()	: 
	m_fSpeed(0), 
	m_fDist(0)
{
}

// 복사 생성자
CBullet::CBullet(const CBullet & bullet)	:
	CDx11Script(bullet)
{
	*this = bullet;
	m_iRefCount = 1;
}


CBullet::~CBullet()
{
}

void CBullet::SetSpeed(float fSpeed)
{
	m_fSpeed = fSpeed;
}

void CBullet::SetDistance(float fDist)
{
	m_fDist = fDist;
}

bool CBullet::Init()
{
//	총알 변수들의 초기값
	m_fSpeed = 15.f;
	m_fDist = 50.f;

	return true;
}

void CBullet::Input(float fTime)
{
}

int CBullet::Update(float fTime)
{
	m_pTransform->MoveWorld(m_fSpeed, fTime);
//	m_pTransform->RotationWorld(DX11_PI * 3, fTime, AXIS_Z);

	// 총알이 일정거리만큼 이동하면 삭제된다. 
	m_fDist -= (m_fSpeed * fTime);
	if (m_fDist <= 0.f)
	{
		m_pGameObject->Death();
	}

	return 0;
}

int CBullet::LateUpdate(float fTime)
{
	return 0;
}

void CBullet::Render(float fTime)
{
}

CBullet * CBullet::Clone()
{
	return new CBullet(*this);
}


