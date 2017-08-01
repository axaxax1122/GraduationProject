#include "Snow.h"
#include "GameObject/Dx11GameObject.h"
#include "Component/Dx11Transform.h"

CSnow::CSnow()
{
}
CSnow::~CSnow()
{
}
CSnow::CSnow(const CSnow & snow)
{
}

bool CSnow::Init()
{
	m_fSpeed = 3.f;
	m_fDist = 20.f;

	return true;
}

void CSnow::Input(float fTime)
{
}

int CSnow::Update(float fTime)
{
	
	//이동&회전
	//m_pTransform->RotationWorld(30.f*DX11_PI / 180.f, fTime * 10, AXIS_Z);
	m_pTransform->MoveWorld(10.f, fTime * 3, AXIS_X);
	m_pTransform->MoveWorld(10.f, fTime * 5, AXIS_Y);

	//나가면 원점처리
	if (m_pTransform->GetWorldPos().y > 1000) {
		m_pTransform->SetWorldPos(m_pTransform->GetWorldPos().x, 0.f, m_pTransform->GetWorldPos().z);
	}
	if (m_pTransform->GetWorldPos().x > 1500) {
		m_pTransform->SetWorldPos(0, m_pTransform->GetWorldPos().y, m_pTransform->GetWorldPos().z);
	}
	
	/*m_fDist -= (m_fSpeed * fTime);

	if (m_fDist <= 0.f)
	{
	m_pGameObject->Death();
	}*/

	return 0;
}

int CSnow::LateUpdate(float fTime)
{
	
	return 0;
}

void CSnow::Render(float fTime)
{
}

CSnow * CSnow::Clone()
{
	return new CSnow(*this);
}

void CSnow::OnCollisionEnter(CDx11Collider * pColl, float fTime)
{

}

void CSnow::OnCollision(CDx11Collider * pColl, float fTime)
{

}

void CSnow::OnCollisionExit(CDx11Collider * pColl, float fTime)
{

}
