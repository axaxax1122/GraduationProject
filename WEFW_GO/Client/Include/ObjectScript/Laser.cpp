#include "Laser.h"
#include "Component/Dx11Transform.h"
#include "GameObject/Dx11GameObject.h"


CLaser::CLaser() :
	m_fSpeed(0),
	m_fDist(0)
{
}

CLaser::CLaser(const CLaser & laser)	:
	CDx11Script(laser)
{
	m_fSpeed = laser.m_fSpeed;
	m_fDist = laser.m_fDist;
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

bool CLaser::Init()
{
	m_fSpeed = 6.f;
	m_fDist = 10.f;
	return true;
}

void CLaser::Input(float fTime)
{
}

int CLaser::Update(float fTime)
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
