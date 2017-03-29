#include "Bullet.h"
#include "GameObject/Dx11GameObject.h"
#include "Component/Dx11Transform.h"
#include "Component/Dx11Collider.h"


CBullet::CBullet()
{
}

CBullet::CBullet(const CBullet & bullet)	:
	CDx11Script(bullet)
{
	m_fSpeed = bullet.m_fSpeed;
	m_fDist = bullet.m_fDist;
}

CBullet::~CBullet()
{
	SAFE_RELEASE(m_pTransform);
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
	m_fSpeed = 3.f;
	m_fDist = 10.f;

	return true;
}

void CBullet::Input(float fTime)
{
}

int CBullet::Update(float fTime)
{
	m_pTransform->MoveWorld(m_fSpeed, fTime);
	//m_pTransform->RotationWorld(DX11_PI * 3.f, fTime, AXIS_Z);

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

void CBullet::OnCollisionEnter(CDx11Collider * pColl, float fTime)
{
	if (pColl->GetTag() == "MonsterColl")
	{
		CDx11GameObject*	pObj = pColl->GetGameObject();
      	pObj->Death();//총알 삭제
		SAFE_RELEASE(pObj);

		//자기자신 삭제.
		m_pGameObject->Death();
	}

}

void CBullet::OnCollision(CDx11Collider * pColl, float fTime)
{
}

void CBullet::OnCollisionExit(CDx11Collider * pColl, float fTime)
{
}
