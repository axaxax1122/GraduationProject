#include "Dx11Camera.h"
#include "../Core/Dx11Device.h"
#include "../GameObject/Dx11GameObject.h"
#include "Dx11Transform.h"
DX11_USING

CDx11Camera::CDx11Camera() :
	m_pTarget(NULL)
{
	m_eComType = CT_CAMERA;
	SetTypeName<CDx11Camera>();
}

CDx11Camera::CDx11Camera(const CDx11Camera & camera)	:
	CDx11Component(camera)
{
	m_matView = camera.m_matView;
	m_matProj = camera.m_matProj;

	m_fAngle = camera.m_fAngle;
	m_fNear = camera.m_fNear;
	m_fFar = camera.m_fFar;
}

CDx11Camera::~CDx11Camera()
{
	SAFE_RELEASE(m_pTarget);
}

Matrix CDx11Camera::GetViewMatrix()
{
	return m_matView;
}

Matrix CDx11Camera::GetProjMatrix()
{
	return m_matProj;
}



CDx11GameObject * CDx11Camera::GetTarget() const
{
	if (!m_pTarget)
		return NULL;

	m_pTarget->AddRef();
	return m_pTarget;
}

void CDx11Camera::SetTarget(CDx11GameObject * pTarget)
{
	SAFE_RELEASE(m_pTarget);
	m_pTarget = pTarget;
	m_pTarget->AddRef();

	CDx11Transform*	pTr = pTarget->GetTransform();

	m_vPrevPos = pTr->GetWorldPos();

	SAFE_RELEASE(pTr);
}

void CDx11Camera::SetProjection(float fAngle, float fNear, float fFar)
{
	RESOLUTION_TYPE	eType = DX11_GET_SINGLE(CDx11Device)->GetResolutionType();
	float	fAspect = g_tRS[eType].iWidth / (float)g_tRS[eType].iHeight;
	m_matProj = XMMatrixPerspectiveFovLH(fAngle, fAspect, fNear, fFar);

	m_fAngle = fAngle;
	m_fNear = fNear;
	m_fFar = fFar;
}
void CDx11Camera::SetOrthoProjection(float n, float f)
{
	RESOLUTION_TYPE	eType = DX11_GET_SINGLE(CDx11Device)->GetResolutionType();
	m_matProj = XMMatrixOrthographicOffCenterLH(0.f, (float)g_tRS[eType].iWidth,
		(float)g_tRS[eType].iHeight, 0.f, n, f);

	m_fNear = n;
	m_fFar = f;

	m_eProjType = PT_ORTHOGONAL;
}
void CDx11Camera::SetAngle(float fAngle)
{
	m_fAngle = fAngle;
	RESOLUTION_TYPE	eType = DX11_GET_SINGLE(CDx11Device)->GetResolutionType();
	float	fAspect = g_tRS[eType].iWidth / (float)g_tRS[eType].iHeight;
	m_matProj = XMMatrixPerspectiveFovLH(m_fAngle, fAspect, m_fNear, m_fFar);
}

void CDx11Camera::SetNear(float fNear)
{
	m_fNear = fNear;
	RESOLUTION_TYPE	eType = DX11_GET_SINGLE(CDx11Device)->GetResolutionType();
	float	fAspect = g_tRS[eType].iWidth / (float)g_tRS[eType].iHeight;
	m_matProj = XMMatrixPerspectiveFovLH(m_fAngle, fAspect, m_fNear, m_fFar);
}

void CDx11Camera::SetDistance(float fDist)
{
	m_fFar = fDist;
	RESOLUTION_TYPE	eType = DX11_GET_SINGLE(CDx11Device)->GetResolutionType();
	float	fAspect = g_tRS[eType].iWidth / (float)g_tRS[eType].iHeight;
	m_matProj = XMMatrixPerspectiveFovLH(m_fAngle, fAspect, m_fNear, m_fFar);
}


void CDx11Camera::TargetTrace()
{
	if (m_pTarget)
	{
		CDx11Transform*	pTr = m_pTarget->GetTransform();

		Vec3	vPos = pTr->GetWorldPos();

		SAFE_RELEASE(pTr);

		Vec3	vMove = vPos - m_vPrevPos;
		m_vPrevPos = vPos;

		m_pTransform->MoveWorld(vMove);
	}
}

bool CDx11Camera::Init()
{
	m_matView = XMMatrixIdentity();
	m_matProj = XMMatrixIdentity();

	return true;
}

int CDx11Camera::Update(float fTime)
{
	TargetTrace();

	return 0;
}

int CDx11Camera::LateUpdate(float fTime)
{
	// Transform에서 축과 위치를 얻어온다.
	m_matView = XMMatrixIdentity();

	Vec3	vPos = m_pTransform->GetWorldPos();

	// 회전부분을 만든다.
	float*	pMatrix = (float*)&m_matView;
	for (int i = 0; i < AXIS_MAX; ++i)
	{
		memcpy(pMatrix + (i * 4), &m_pTransform->GetWorldAxis()[i], 
			sizeof(Vec3));
	}

	m_matView = XMMatrixTranspose(m_matView);

	// 위치 부분을 만든다.
	vPos *= -1.f;
	for (int i = 0; i < AXIS_MAX; ++i)
	{
		float fDot = vPos.Dot(m_pTransform->GetWorldAxis((AXIS)i));
		memcpy(pMatrix + (12 + i), &fDot, sizeof(float));
	}
	return 0;
}

void CDx11Camera::Render(float fTime)
{
}

CDx11Camera * CDx11Camera::Clone()
{
	return new CDx11Camera(*this);
}
