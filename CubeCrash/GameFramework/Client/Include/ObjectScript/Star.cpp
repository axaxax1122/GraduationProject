#include "Star.h"
#include "GameObject/Dx11GameObject.h"
#include "Component/Dx11Transform.h"

CStar::CStar()
{
}


CStar::~CStar()
{
}

CStar::CStar(const CStar & star)
{
}

bool CStar::Init()
{
	m_fSpeed = 3.f;
	m_fDist = 20.f;

	return true;
}

void CStar::Input(float fTime)
{
}

int CStar::Update(float fTime)
{
	//이동&회전
	//m_pTransform->RotationWorld(30.f*DX11_PI / 180.f, fTime * 10, AXIS_Z);
	m_pTransform->MoveWorld(20.f, fTime * 10, AXIS_Y);

	//나가면 원점처리
	if (m_pTransform->GetWorldPos().y > 1000) {
		m_pTransform->SetWorldPos(m_pTransform->GetWorldPos().x, 0.f, m_pTransform->GetWorldPos().z);
	}
	if (m_pTransform->GetWorldPos().x > 1500) {
		m_pTransform->SetWorldPos(0, m_pTransform->GetWorldPos().y, m_pTransform->GetWorldPos().z);
	}
	
	return 0;
}

int CStar::LateUpdate(float fTime)
{
	
	return 0;
}

void CStar::Render(float fTime)
{
}

CStar * CStar::Clone()
{
	return new CStar(*this);
}
