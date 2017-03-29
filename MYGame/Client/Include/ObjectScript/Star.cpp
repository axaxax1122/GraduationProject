#include "Star.h"
#include "GameObject/Dx11GameObject.h"
#include "Component/Dx11Transform.h"

CStar::CStar()
{
}

CStar::CStar(const CStar & star)
{
}


CStar::~CStar()
{
}

bool CStar::Init() 
{
	return true;
}

void CStar::Input(float fTime)
{
}

int CStar::Update(float fTime)
{
	m_pTransform->RotationWorld(1.f, fTime, AXIS_Y);
	m_pTransform->RotationWorld(1.f, fTime, AXIS_Z);
	//m_pTransform->SetWorldRot(45, 45, 45);
	
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

void CStar::OnCollisionEnter(CDx11Collider * pColl, float fTime)
{
}

void CStar::OnCollision(CDx11Collider * pColl, float fTime)
{
}

void CStar::OnCollisionExit(CDx11Collider * pColl, float fTime)
{
}
