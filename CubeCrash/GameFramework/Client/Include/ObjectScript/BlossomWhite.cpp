#include "BlossomWhite.h"
#include "GameObject/Dx11GameObject.h"
#include "Component/Dx11Transform.h"


CBlossomWhite::CBlossomWhite()
{
}


CBlossomWhite::~CBlossomWhite()
{
}

CBlossomWhite::CBlossomWhite(const CBlossomWhite & BW)
{
}

bool CBlossomWhite::Init()
{
	m_fSpeed = 3.f;
	m_fDist = 20.f;
	return true;
}

void CBlossomWhite::Input(float fTime)
{
}

int CBlossomWhite::Update(float fTime)
{	
	//이동&회전
	//m_pTransform->RotationWorld(30.f*DX11_PI / 180.f, fTime * 10, AXIS_Z);
	m_pTransform->MoveWorld(-15.f, fTime * 3, AXIS_X);
	m_pTransform->MoveWorld(8.f, fTime * 5, AXIS_Y);

	//나가면 원점처리
	if (m_pTransform->GetWorldPos().y > 1000) {
		m_pTransform->SetWorldPos(m_pTransform->GetWorldPos().x, 0.f, m_pTransform->GetWorldPos().z);
	}
	if (m_pTransform->GetWorldPos().x < 0) {
		m_pTransform->SetWorldPos(1600, m_pTransform->GetWorldPos().y, m_pTransform->GetWorldPos().z);
	}

	

	return 0;
}

int CBlossomWhite::LateUpdate(float fTime)
{
	return 0;
}

void CBlossomWhite::Render(float fTime)
{
}

CBlossomWhite * CBlossomWhite::Clone()
{
	return new CBlossomWhite(*this);
}
