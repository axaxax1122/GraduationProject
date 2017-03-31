#include "Dx11Camera.h"
#include "../Core/Dx11Device.h"
#include "Dx11Transform.h"
#include "../GameObject/Dx11GameObject.h"
#include "Dx11Mouse.h"
#include "../Core/Dx11Input.h"
#include "Dx11Terrain.h"
//#include "../Core/Dx11Debug.h"

DX11_USING

CDx11Camera::CDx11Camera()	:
	m_iShakeTimes(0),
	m_pTarget(NULL),
	m_pTerrain(NULL)
{
	m_eComType = CT_CAMERA;
	m_eProjType = PT_PERSPECTIVE;
	m_eCameraType = CVT_FIRST;
	SetTypeName<CDx11Camera>();
}

CDx11Camera::CDx11Camera(const CDx11Camera & camera)	:
	CDx11Component(camera)
{
	*this = camera;
	m_iRefCount = 1;

	if (m_pTarget)
		m_pTarget->AddRef();
}


CDx11Camera::~CDx11Camera()
{
	SAFE_RELEASE(m_pTarget);
	SAFE_RELEASE(m_pTerrain);
}

Matrix CDx11Camera::GetViewMatrix()
{
	return m_matView;
}

Matrix CDx11Camera::GetProjMatrix()
{
	return m_matProj;
}


int CDx11Camera::GetShakeTimes()
{
	return m_iShakeTimes;
}

CDx11GameObject * CDx11Camera::GetTarget() const
{
	if (!m_pTarget)
		return NULL;

	m_pTarget->AddRef();
	return m_pTarget;
}

Vec2 CDx11Camera::GetMouseGap() const
{
	return m_vMouseGap;
}

Frustrum CDx11Camera::GetFrustrum() const
{
	return m_tFrustrum;
}

void CDx11Camera::SetTarget(CDx11GameObject * pTarget)
{
	SAFE_RELEASE(m_pTarget);
	
	m_pTarget = pTarget;
	m_pTarget->AddRef();

	CDx11Transform*	pTr = m_pTarget->GetTransform();
	m_vPrevPos = pTr->GetWorldPos();
	SAFE_RELEASE(pTr);

}

void CDx11Camera::ShakeCamera()
{
	if (m_iShakeTimes % 4 == 0 || m_iShakeTimes % 4 == 3)
	{
		m_pTransform->MoveWorld(-0.1f, 1.f, AXIS_X);
	}
	else if (m_iShakeTimes % 4 == 1 || m_iShakeTimes % 4 == 2)
	{
		m_pTransform->MoveWorld(+0.1f, 1.f, AXIS_X);
	}
	--m_iShakeTimes;
}

void CDx11Camera::TraceTarget()
{
	if (m_pTarget)
	{
		CDx11Transform*	pTr = m_pTarget->GetTransform();
		Vec3	vPos = pTr->GetWorldPos();
		Vec3	vMove = vPos - m_vPrevPos;
		m_vPrevPos = vPos;

		m_pTransform->MoveWorld(vMove);
		SAFE_RELEASE(pTr);
	}
}

void CDx11Camera::SetTerrain(CDx11Terrain * pTerrain)
{
	m_pTerrain = pTerrain;
	m_pTerrain->AddRef();
}

void CDx11Camera::CalculateFrustrum()
{
	// �� ��� * ���� ���� ����� ������� ���Ѵ�.
	Matrix matInverse = m_matView * m_matProj;
	XMVECTOR	vDeterminent = XMMatrixDeterminant(matInverse);
	matInverse = XMMatrixInverse(&vDeterminent, matInverse);

	// ����Ʈ ���� 
	Vec3	vPlaneVtx[8];
	//�ո�
	vPlaneVtx[0] = Vec3(-1.f, 1.f, 0.f);
	vPlaneVtx[1] = Vec3(1.f, 1.f, 0.f);
	vPlaneVtx[2] = Vec3(1.f, -1.f, 0.f);
	vPlaneVtx[3] = Vec3(-1.f, -1.f, 0.f);

	//�޸�
	vPlaneVtx[4] = Vec3(-1.f, 1.f, 1.f);
	vPlaneVtx[5] = Vec3(1.f, 1.f, 1.f);
	vPlaneVtx[6] = Vec3(1.f, -1.f, 1.f);
	vPlaneVtx[7] = Vec3(-1.f, -1.f, 1.f);

	// ������� �����Ͽ� ����ü�� �� ������ ���� �������� ��ȯ
	XMVECTOR v;
	for (UINT i = 0; i < 8; ++i)
	{
		v = XMVector3TransformCoord(vPlaneVtx[i].Convert(), matInverse);
		vPlaneVtx[i].Convert(v);
	}

	// �������� �� ����� ���Ѵ�.
	// �ٱ������� �븻�� �����ϱ� ���� �ð� ��������.
	m_tFrustrum.m_vPlane[0] = XMPlaneFromPoints(vPlaneVtx[0].Convert(), vPlaneVtx[1].Convert(), vPlaneVtx[2].Convert());
	m_tFrustrum.m_vPlane[1] = XMPlaneFromPoints(vPlaneVtx[4].Convert(), vPlaneVtx[7].Convert(), vPlaneVtx[6].Convert());
	m_tFrustrum.m_vPlane[2] = XMPlaneFromPoints(vPlaneVtx[0].Convert(), vPlaneVtx[3].Convert(), vPlaneVtx[7].Convert());
	m_tFrustrum.m_vPlane[3] = XMPlaneFromPoints(vPlaneVtx[1].Convert(), vPlaneVtx[5].Convert(), vPlaneVtx[6].Convert());
	m_tFrustrum.m_vPlane[4] = XMPlaneFromPoints(vPlaneVtx[0].Convert(), vPlaneVtx[4].Convert(), vPlaneVtx[5].Convert());
	m_tFrustrum.m_vPlane[5] = XMPlaneFromPoints(vPlaneVtx[3].Convert(), vPlaneVtx[2].Convert(), vPlaneVtx[6].Convert());
}

void CDx11Camera::SetMinMax(float fMin, float fMax)
{
	m_fMin = fMin;
	m_fMax = fMax;
}

void CDx11Camera::ZoomInOut(CDx11Mouse* pMouse)
{
	short sWheelDir = pMouse->GetWheelDir();

	if (sWheelDir)
	{
		// �پ�[+]Ȯ�� , �ٴٿ�[-]���
		float fMove = sWheelDir * 0.1f;

		CDx11Transform* pTr = m_pTarget->GetTransform();

		Vec3 vTargetPos = pTr->GetWorldPos();
		Vec3 vPos = m_pTransform->GetWorldPos();

		// Ÿ�ٰ� ī�޶� ������ �Ÿ�
		float fDist = vTargetPos.Distance(vPos);
		// fMove��ŭ �̵��ϹǷ� �Ÿ��� �׸�ŭ ��������ų� �־�����.
		fDist -= fMove;

		// �ִ�, �ּ� �˻�
		if (fDist < m_fMin)
		{
			fMove = 0.f;
		}
		else if (fDist > m_fMax)
		{
			fMove = 0.f;
		}

		m_pTransform->MoveWorld(fMove, 1.f, AXIS_Z);
		SAFE_RELEASE(pTr);
	}
}

void CDx11Camera::RotateCamera(CDx11Mouse * pMouse)
{
	if (pMouse->GetRightDrag())
	{
		Vec2 vMouseGap = pMouse->GetMouseGap();
		vMouseGap *= m_fMouseSens;

		if (vMouseGap.x != 0.f)
		{
			RotateLeftRight(vMouseGap.x);
		}

		if (vMouseGap.y != 0.f)
		{
			RotateUpDown(vMouseGap.y);
		}



	}
}

void CDx11Camera::RotateUpDown(float fGap)
{
	CDx11Transform* pTr = m_pTarget->GetTransform();

	Vec3	vPos = m_pTransform->GetWorldPos();
	Vec3	vTargetPos = pTr->GetWorldPos();
	Vec3	vDist = vPos - vTargetPos;
	Vec3	vAxis = m_pTransform->GetWorldAxis(AXIS_X);

	if (m_pTerrain)
	{
		if (vPos.y < m_pTerrain->GetHeight(vPos) + 0.3f)
		{
			fGap = 0.001f;
		}
	}

	// XMMatrixRotationAxis �Լ��� �̿�
	// X������ ȸ������� �����
	// ȸ������� �ּҸ� ����Ű�� ������ ����

	Matrix	matRot = XMMatrixRotationAxis(vAxis.Convert(), fGap);
	float*	pMatrix = (float*)&matRot;

	// ����� Translate �κ��� ����
	memcpy(pMatrix + 12, &vTargetPos, sizeof(Vec3));

	// ī�޶� ������Ű�� ���Ͽ� TransformCoord�Լ��� �̿�
	// XMVECTOR Ÿ���� �����ϴ� TransformCoord �Լ��� XMFLOAT���� ��ȯ�Ͽ� vPos�� ����
	vPos.Convert(XMVector3TransformCoord(vDist.Convert(), matRot));

	// ī�޶��� y - �÷��̾��� y�� vDist�� �������� ī�޶�� �÷��̾��� ��Ȯ�� �������� ������
	if(vDist.Length() - 0.3f > (vPos.y - vTargetPos.y))
	{
		m_pTransform->SetWorldPos(vPos);
		m_vMouseGap.y = fGap;
	}
	else
	{
		RotateUpDown(-0.001f);
		return;
	}


	// ���� ��������
	for (int i = 0; i < AXIS_MAX; ++i)
	{
		vAxis = m_pTransform->GetWorldAxis((AXIS)i);
		vAxis.Convert(XMVector3TransformNormal(vAxis.Convert(), matRot));
		vAxis = vAxis.Normalize();
		m_pTransform->SetWorldAxis(vAxis, (AXIS)i);
	}

	SAFE_RELEASE(pTr);
}

void CDx11Camera::RotateLeftRight(float fGap)
{
	CDx11Transform* pTr = m_pTarget->GetTransform();

	Vec3	vPos = m_pTransform->GetWorldPos();
	Vec3	vTargetPos = pTr->GetWorldPos();
	Vec3	vDist = vPos - vTargetPos;
	Vec3	vAxis = m_pTransform->GetWorldAxis(AXIS_Y);

	// Y������ ȸ������� �����
	// ȸ������� �ּҸ� ����Ű�� ������ ����
	Matrix	matRotY = XMMatrixRotationY(fGap);
	float*	pMatrix = (float*)&matRotY;

	// ����� Translate �κ��� ����
	memcpy(pMatrix + 12, &vTargetPos, sizeof(Vec3));

	// ī�޶� ������Ű�� ���Ͽ� TransformCoord�Լ��� �̿�
	// XMVECTOR Ÿ���� �����ϴ� TransformCoord �Լ��� XMFLOAT���� ��ȯ�Ͽ� vPos�� ����
	vPos.Convert(XMVector3TransformCoord(vDist.Convert(), matRotY));
	
	m_pTransform->SetWorldPos(vPos);
	m_vMouseGap.x = fGap;

	// ���� ��������
	for (int i = 0; i < AXIS_MAX; ++i)
	{
		vAxis = m_pTransform->GetWorldAxis((AXIS)i);
		vAxis.Convert(XMVector3TransformNormal(vAxis.Convert(), matRotY));
		m_pTransform->SetWorldAxis(vAxis, (AXIS)i);
	}

	SAFE_RELEASE(pTr);
}

void CDx11Camera::SetCameraType(CAMERA_VIEW_TYPE eType)
{
	m_eCameraType = eType;

	if (m_eCameraType == CVT_THIRD)
	{
		m_pTransform->MoveWorld(-3.f, 1.f, AXIS_Z);
		float fMouseGapY = 0.2f;

		RotateUpDown(fMouseGapY);
	}
}

void CDx11Camera::SetProjection(float fAngle, float fNear, float fFar)
{
//	�ػ� ������ ����̽����� ����
	RESOLUTION_TYPE eType = DX11_GET_SINGLE(CDx11Device)->GetResolutionType();
//	��Ⱦ��� ���� / ���� (floatŸ��)	
	float fAspect = g_tRS[eType].iWidth / (float)g_tRS[eType].iHeight;
//	���� ����� ���Ѵ�.
	m_matProj = XMMatrixPerspectiveFovLH(fAngle, fAspect, fNear, fFar);

	m_fAngle = fAngle;
	m_fNear = fNear;
	m_fFar = fFar;
}

void CDx11Camera::SetOrthoProjection(float fNear, float fFar)
{
	RESOLUTION_TYPE eType = DX11_GET_SINGLE(CDx11Device)->GetResolutionType();
	m_matProj = XMMatrixOrthographicOffCenterLH(0.f, (float)g_tRS[eType].iWidth,
		(float)g_tRS[eType].iHeight, 0.f, fNear, fFar);

	m_fNear = fNear;
	m_fFar = fFar;
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

void CDx11Camera::SetShakeTimes(int iTimes)
{
	m_iShakeTimes = iTimes;
}



bool CDx11Camera::Init()
{
	m_matView = XMMatrixIdentity();
	m_matProj = XMMatrixIdentity();
	m_fMin = 1.f;
	m_fMax = 10.f;
	m_fMouseSens = 0.003f;
	return true;
}

int CDx11Camera::Update(float fTime)
{
	if(m_iShakeTimes > 0)
		ShakeCamera();

	TraceTarget();

	// 3��Ī ī�޶��� ���� ���콺 �Է� ����
	if (m_eCameraType == CVT_THIRD)
	{
		
		// ���콺�� ���´�.
		CDx11Mouse*	pMouse = DX11_GET_SINGLE(CDx11Input)->GetMouse();
		
		if (m_pTarget)
		{
			ZoomInOut(pMouse);
			RotateCamera(pMouse);
		}

		SAFE_RELEASE(pMouse);
	}



	return 0;
}

int CDx11Camera::LateUpdate(float fTime)
{
	// �� ��� �ʱ�ȭ
	m_matView = XMMatrixIdentity();

	// ������ ���� ����Ѵ�.
	if (m_eProjType == PT_PERSPECTIVE)
	{
		// ȸ�� �κ��� �����.
		// pMatrix�� �� ����� ����Ű�� ������
		float* pMatrix = (float*)&m_matView;
		for (int i = 0; i < AXIS_MAX; ++i)
		{
			// ��� 1�پ�(X��, Y��, Z��) �޸� ����
			// Transform���� ���� ���´�.
			memcpy(pMatrix + (i * 4), &m_pTransform->GetWorldAxis()[i],
				sizeof(Vec3));
		}
		// ��� ��ġ
		m_matView = XMMatrixTranspose(m_matView);


		// ��ġ �κ��� �����.
		// Transform���� ��ġ�� ���´�.
		Vec3	vPos = m_pTransform->GetWorldPos();
		vPos *= -1.f;
		for (int i = 0; i < AXIS_MAX; ++i)
		{
			float fDot = vPos.Dot(m_pTransform->GetWorldAxis((AXIS)i));

			//	��ġ �κ��� 4��κ��̹Ƿ� +12���� ����
			memcpy(pMatrix + (12 + i), &fDot, sizeof(float));
		}
	}

	// ����ü ����
	CalculateFrustrum();

	return 0;
}

void CDx11Camera::Render(float fTime)
{
}

CDx11Camera * CDx11Camera::Clone()
{
	return new CDx11Camera(*this);
}
