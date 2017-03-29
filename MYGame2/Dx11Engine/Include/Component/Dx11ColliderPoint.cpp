#include "Dx11ColliderPoint.h"
#include "Dx11Transform.h"
#include "Dx11ColliderRC.h"

#if defined(_DEBUG) | defined(DEBUG)
#include "../Resources/Dx11Mesh.h"
#include "../Resources/Dx11ResourcesManager.h"
#include "../Rendering/Dx11Shader.h"
#include "../Scene/Dx11Scene.h"
#include "Dx11Camera.h"
#include "../GameObject/Dx11GameObject.h"
#endif

DX11_USING

CDx11ColliderPoint::CDx11ColliderPoint()	:
	m_vPos(0.f, 0.f)
{
	m_eSubType = CST_POINT;
	SetTypeName<CDx11ColliderPoint>();
}

CDx11ColliderPoint::CDx11ColliderPoint(const CDx11ColliderPoint & collider) :
	CDx11Collider(collider)
{
	m_vPos = collider.m_vPos;
}

CDx11ColliderPoint::~CDx11ColliderPoint()
{
}

Vec2 CDx11ColliderPoint::GetPoint() const
{
	return m_vPos;
}

void CDx11ColliderPoint::SetPoint(const Vec2 & vPoint)
{
	m_vPos = vPoint;
}

void CDx11ColliderPoint::SetPoint(const POINT & pt)
{
	m_vPos = pt;
}

bool CDx11ColliderPoint::Init()
{
	return true;
}

void CDx11ColliderPoint::Input(float fTime)
{
}

int CDx11ColliderPoint::Update(float fTime)
{
	return 0;
}

int CDx11ColliderPoint::LateUpdate(float fTime)
{
	Vec3 vPos = m_pTransform->GetWorldPos();
	m_vPos.x = vPos.x;
	m_vPos.y = vPos.y;

	return 0;
}

void CDx11ColliderPoint::Render(float fTime)
{
}

CDx11ColliderPoint * CDx11ColliderPoint::Clone()
{
	return new CDx11ColliderPoint(*this);
}

bool CDx11ColliderPoint::Collision(CDx11Collider * pColl)
{
	switch (pColl->GetColliderType())
	{
	case CST_RECT:
		return CollisionRectToPoint(((CDx11ColliderRC*)pColl)->GetInfo(),
			m_vPos);
	}

	return false;
}
