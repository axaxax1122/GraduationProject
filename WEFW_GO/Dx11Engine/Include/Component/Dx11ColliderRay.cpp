#include "Dx11ColliderRay.h"
#include "../Core/Dx11Device.h"
#include "../Scene/Dx11SceneManager.h"
#include "../Scene/Dx11Scene.h"
#include "../GameObject/Dx11GameObject.h"
#include "Dx11Camera.h"
#include "Dx11ColliderSphere.h"
#include "Dx11ColliderOBB.h"
#include "Dx11Transform.h"
#include "../Core/Dx11Input.h"

DX11_USING

CDx11ColliderRay::CDx11ColliderRay()
{
	m_eSubType = CST_RAY;
	SetTypeName<CDx11ColliderRay>();
	memset(&m_tRay, 0, sizeof(RAY));
}


CDx11ColliderRay::CDx11ColliderRay(const CDx11ColliderRay & ray)
{
	m_tRay = ray.m_tRay;
}

CDx11ColliderRay::~CDx11ColliderRay()
{
}

RAY CDx11ColliderRay::GetRay() const
{
	return m_tRay;
}

void CDx11ColliderRay::SetMousePos(const POINT & ptPos)
{
	m_vMousePos = ptPos;
}

bool CDx11ColliderRay::Init()
{
	return true;
}

int CDx11ColliderRay::Update(float fTime)
{
	// ����Ʈ�� ���´�.
	D3D11_VIEWPORT	vp;
	UINT	iVPCount = 1;
	DX11_CONTEXT->RSGetViewports(&iVPCount, &vp);

	// ī�޶� ��������� ���´�.
	CDx11Scene*	pScene = DX11_GET_SINGLE(CDx11SceneManager)->GetCurrentScene();
	CDx11GameObject*	pCameraObj = pScene->GetMainCamera();
	CDx11Camera*	pCamera = (CDx11Camera*)pCameraObj->FindComponentFromType(CT_CAMERA);

	Matrix	matProj = pCamera->GetProjMatrix();
	Matrix	matView = pCamera->GetViewMatrix();

	// �� ����� ����ķ� ������ش�.
	XMVECTOR	vDeterminent = XMMatrixDeterminant(matView);
	matView = XMMatrixInverse(&vDeterminent, matView);
	float*	pProj = (float*)&matProj;
	float*	pView = (float*)&matView;


	// �������� ��ǥ x = sx * 2/w - 1
	// �������� ��ǥ y = sy * -2/h + 1

	// �� ���������� ���� ������ ���Ѵ�.
	// ���� ����� 11, 22�� ���� ����ְ� �������� 0�̴�.
	// �׷��Ƿ� 11, 22�� ������ �� ������ x, y ��ǥ�� �����ش�.

	POINT pt = DX11_GET_SINGLE(CDx11Input)->GetMousePoint();
	m_vMousePos.x = pt.x;
	m_vMousePos.y = pt.y;

	m_tRay.vDir.x = (m_vMousePos.x * 2.f / vp.Width - 1.f) / (*pProj);
	m_tRay.vDir.y = (m_vMousePos.y * -2.f / vp.Height + 1.f) / (*(pProj + 5));
	m_tRay.vDir.z = 1.f;
	
	m_tRay.vDir = m_tRay.vDir.Normalize();
	m_tRay.vPos = Vec3Zero;

	// ���� �������� ��ȯ�Ѵ�.
	XMVECTOR	v = XMVector3TransformNormal(m_tRay.vDir.Convert(), matView);
	m_tRay.vDir.Convert(v);
	m_tRay.vDir = m_tRay.vDir.Normalize();

	v = XMVector3TransformCoord(m_tRay.vPos.Convert(), matView);
	m_tRay.vPos.Convert(v);

	SAFE_RELEASE(pCamera);
	SAFE_RELEASE(pCameraObj);
	SAFE_RELEASE(pScene);

	return 0;
}

int CDx11ColliderRay::LateUpdate(float fTime)
{
	return 0;
}

void CDx11ColliderRay::Render(float fTime)
{
}

CDx11ColliderRay * CDx11ColliderRay::Clone()
{
	return new CDx11ColliderRay(*this);
}

bool CDx11ColliderRay::Collision(CDx11Collider * pColl)
{
	switch (pColl->GetColliderType())
	{
	case CST_SPHERE:
		return CollisionRayToSphere(m_tRay,
			((CDx11ColliderSphere*)pColl)->GetCenter(),
			((CDx11ColliderSphere*)pColl)->GetRadius());
	}
	return false;
}
