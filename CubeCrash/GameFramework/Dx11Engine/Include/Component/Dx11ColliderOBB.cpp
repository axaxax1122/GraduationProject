#include "Dx11ColliderOBB.h"
#include "Dx11Transform.h"

//#if defined(_DEBUG) | defined(DEBUG)
#include "../Resources/Dx11Mesh.h"
#include "../Resources/Dx11ResourcesManager.h"
#include "../Rendering/Dx11Shader.h"
#include "../Scene/Dx11Scene.h"
#include "Dx11Camera.h"
#include "../GameObject/Dx11GameObject.h"
//#endif

DX11_USING

CDx11ColliderOBB::CDx11ColliderOBB() :
	m_vCenter(Vec3Zero),
	m_vPivot(Vec3Zero),
	m_vRotation(Vec3Zero),
	m_vPrevPos(Vec3Zero),
	m_vPrevRot(Vec3Zero),
	m_fRadius(0.f)
{
	m_eSubType = CST_OBB;
	SetTypeName<CDx11ColliderOBB>();

	for (int i = 0; i < 3; ++i)
	{
		m_vAxis[i] = Vec3Zero;
		m_fAxisLen[i] = 0.f;
	}
}


CDx11ColliderOBB::CDx11ColliderOBB(const CDx11ColliderOBB & obb)	:
	CDx11Collider(obb)
{
	*this = obb;
}

CDx11ColliderOBB::~CDx11ColliderOBB()
{
}



float CDx11ColliderOBB::GetAxisLen(int i) const
{
	return m_fAxisLen[i];
}

float CDx11ColliderOBB::GetRadius() const
{
	return m_fRadius * 2;
}

Vec3 CDx11ColliderOBB::GetCenter() const
{
	return m_vCenter;
}

void CDx11ColliderOBB::SetOBBInfo(const float fAxisLen[],
	CDx11Transform * pTransform)
{
	m_vCenter =  pTransform->GetWorldPos();
	m_vRotation = pTransform->GetWorldRot();

	for (int i = 0; i < AXIS_MAX; ++i)
	{
		m_vAxis[i] = pTransform->GetWorldAxis((AXIS)i);
		m_fAxisLen[i] = fAxisLen[i];
		if(m_fRadius < m_fAxisLen[i]) m_fRadius = m_fAxisLen[i];
	}

	m_vPrevPos = m_pTransform->GetWorldPos();
	m_vPrevRot = m_pTransform->GetWorldRot();
}

void CDx11ColliderOBB::SetOBBInfo(CDx11Transform * pTransform,
	float RotX,float RotY, float RotZ, 
	float ScaleX, float ScaleY, float ScaleZ)
{
	m_vCenter = pTransform->GetWorldPos();
	Vec3 Rot = Vec3( RotX, RotY, RotZ);
	m_vRotation = Rot;

	m_fAxisLen[0] = ScaleX;
	m_fAxisLen[1] = ScaleY;
	m_fAxisLen[2] = ScaleZ;

	for (int i = 0; i < AXIS_MAX; ++i)
	{
		m_vAxis[i] = pTransform->GetWorldAxis((AXIS)i);
		if (m_fRadius < m_fAxisLen[i]) m_fRadius = m_fAxisLen[i];
	}

	m_vPrevPos = m_pTransform->GetWorldPos();
	m_vPrevRot = m_pTransform->GetWorldRot();
}

void CDx11ColliderOBB::SetPivot(const Vec3 & vPivot)
{
	m_vPivot = vPivot;
}

bool CDx11ColliderOBB::Init()
{
//#if defined(DEBUG) | defined(_DEBUG)
	m_pMesh = DX11_GET_SINGLE(CDx11ResourcesManager)->FindMesh("CubeMesh");
//#endif
	if (!CDx11Collider::Init())
		return false;

	return true;
}

void CDx11ColliderOBB::Input(float fTime)
{
}

int CDx11ColliderOBB::Update(float fTime)
{
	return 0;
}

int CDx11ColliderOBB::LateUpdate(float fTime)
{
	Vec3	vPos = m_pTransform->GetWorldPos();
	Vec3	vDist = vPos - m_vPrevPos;
	Vec3	vRot = m_pTransform->GetWorldRot();
	Vec3	vAngle = vRot - m_vPrevRot;
	
	m_vCenter += vDist;
	m_vRotation += vAngle;

	m_vPrevPos = vPos;
	m_vPrevRot = vRot;

//#if defined(DEBUG) | defined(_DEBUG)
	Matrix	matScale, matRot, matTranslate;
	XMVECTOR vQuarternion;
	matScale = XMMatrixScaling(m_fAxisLen[0] * 2, m_fAxisLen[1] * 2, m_fAxisLen[2] * 2);
	vQuarternion = XMQuaternionRotationRollPitchYaw(m_vRotation.x, m_vRotation.y, m_vRotation.z);
	matRot = XMMatrixRotationQuaternion(vQuarternion);

	Matrix	matPivot;
	matPivot = XMMatrixTranslation(m_vPivot.x, m_vPivot.y, m_vPivot.z);

	matTranslate = XMMatrixTranslation(m_vCenter.x, m_vCenter.y, m_vCenter.z);
	
	m_matWorld = matScale * matPivot * matRot * matTranslate;

	Vec3	vAxis[AXIS_MAX] = { Vec3X, Vec3Y, Vec3Z };
	for (int i = 0; i < AXIS_MAX; ++i)
	{
		Vec	v = vAxis[i].Convert();
		v = XMVector3TransformNormal(v, matRot);
		m_vAxis[i].Convert(v);
		m_vAxis[i] = m_vAxis[i].Normalize();
	}
//#endif

	return 0;
}

void CDx11ColliderOBB::Render(float fTime)
{
//#if defined(DEBUG) | defined(_DEBUG)
	CDx11GameObject*	pCameraObj = m_pScene->GetMainCamera();
	CDx11Camera*		pCamera = (CDx11Camera*)pCameraObj->FindComponentFromTypeName("CDx11Camera");

	TRANSFORMCBUFFER	tBuffer = {};

	tBuffer.matWorld = m_matWorld;
	tBuffer.matView = pCamera->GetViewMatrix();
	tBuffer.matProj = pCamera->GetProjMatrix();
	tBuffer.matWV = m_matWorld * tBuffer.matView;
	tBuffer.matWVP = tBuffer.matWV * tBuffer.matProj;

	tBuffer.matWorld = XMMatrixTranspose(tBuffer.matWorld);
	tBuffer.matView = XMMatrixTranspose(tBuffer.matView);
	tBuffer.matProj = XMMatrixTranspose(tBuffer.matProj);
	tBuffer.matWV = XMMatrixTranspose(tBuffer.matWV);
	tBuffer.matWVP = XMMatrixTranspose(tBuffer.matWVP);

	SAFE_RELEASE(pCamera);
	SAFE_RELEASE(pCameraObj);

	m_pShader->UpdateConstantBuffer("Transform", &tBuffer, ST_VERTEX);
//#endif
	CDx11Collider::Render(fTime);
}

CDx11ColliderOBB * CDx11ColliderOBB::Clone()
{
	return new CDx11ColliderOBB(*this);
}

bool CDx11ColliderOBB::Collision(CDx11Collider * pColl)
{
	switch (pColl->GetColliderType())
	{
	case CST_OBB:
		return CollisionOBB(m_vCenter, m_vAxis, m_fAxisLen, 
			((CDx11ColliderOBB*)pColl)->m_vCenter,
			((CDx11ColliderOBB*)pColl)->m_vAxis, 
			((CDx11ColliderOBB*)pColl)->m_fAxisLen);
	}
	return false;
}
