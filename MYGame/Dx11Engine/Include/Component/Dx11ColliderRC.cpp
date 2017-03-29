#include "Dx11ColliderRC.h"
#include "Dx11Transform.h"

#if defined(_DEBUG) | defined(DEBUG)
#include "../Resources/Dx11Mesh.h"
#include "../Resources/Dx11ResourcesManager.h"
#include "../Rendering/Dx11Shader.h"
#include "../Scene/Dx11Scene.h"
#include "Dx11Camera.h"
#include "../GameObject/Dx11GameObject.h"
#endif

DX11_USING

CDx11ColliderRC::CDx11ColliderRC()
{
	m_eSubType = CST_RECT;
	SetTypeName<CDx11ColliderRC>();
}

CDx11ColliderRC::CDx11ColliderRC(const CDx11ColliderRC & collider)	:
	CDx11Collider(collider)
{
	m_tInfo = collider.m_tInfo;
}

CDx11ColliderRC::~CDx11ColliderRC()
{
}

void CDx11ColliderRC::SetInfo(float l, float t, float r, float b)
{
	m_tInfo.SetInfo(l, t, r, b);
}

RECTINFO CDx11ColliderRC::GetInfo()
{
	return m_tInfo;
}

bool CDx11ColliderRC::Init()
{
	CDx11Collider::Init();
#if defined(_DEBUG)| defined(DEBUG)
	m_pMesh = DX11_GET_SINGLE(CDx11ResourcesManager)->FindMesh(RECT_UI_MESH);
#endif
	return true;
}

void CDx11ColliderRC::Input(float fTime)
{
}

int CDx11ColliderRC::Update(float fTime)
{
	return 0;
}

int CDx11ColliderRC::LateUpdate(float fTime)
{
	Vec3 vPos = m_pTransform->GetWorldPos();
	Vec3 vDist = vPos - m_vPrevPos;

	m_tInfo.Move(vDist);
	m_vPrevPos = vPos;

#if defined(_DEBUG) | defined(DEBUG)
	Matrix matScale, matTrans;
	matScale = XMMatrixScaling(m_tInfo.r - m_tInfo.l, m_tInfo.b - m_tInfo.t,
		1.f);
	matTrans = XMMatrixTranslation(m_tInfo.l, m_tInfo.t, 0.f);
	m_matWorld = matScale * matTrans;
#endif
	return 0;
}

void CDx11ColliderRC::Render(float fTime)
{
#if defined(_DEBUG)|defined(DEBUG)
	CDx11GameObject* pObj = m_pScene->FindCamera("UICamera");

	CDx11Camera* pCamera = (CDx11Camera*)pObj->FindComponentFromType(CT_CAMERA);

	TRANSFORMCBUFFER tBuffer = {};
	tBuffer.matView = pCamera->GetViewMatrix();
	tBuffer.matProj = pCamera->GetProjMatrix();

	SAFE_RELEASE(pCamera);
	SAFE_RELEASE(pObj);

	tBuffer.matWorld = m_matWorld;

	tBuffer.matWV = tBuffer.matWorld * tBuffer.matView;
	tBuffer.matWVP = tBuffer.matWV * tBuffer.matProj;

	tBuffer.matWorld = XMMatrixTranspose(tBuffer.matWorld);
	tBuffer.matView = XMMatrixTranspose(tBuffer.matView);
	tBuffer.matProj = XMMatrixTranspose(tBuffer.matProj);

	tBuffer.matWV = XMMatrixTranspose(tBuffer.matWV);
	tBuffer.matWVP = XMMatrixTranspose(tBuffer.matWVP);

	m_pShader->UpdateConstantBuffer("Transform", &tBuffer, ST_VERTEX);
#endif
	//·»´õ·¯ ¿ªÇÒ - (·»´õ·¯ ÄÄÆ÷³ÍÆ® ºÎÂøÇÒ¼ö¾ø±â¿¡.. )
	CDx11Collider::Render(fTime);
}

CDx11ColliderRC * CDx11ColliderRC::Clone()
{
	return new CDx11ColliderRC(*this);
}

bool CDx11ColliderRC::Collision(CDx11Collider * pColl)
{
	return true;
}
