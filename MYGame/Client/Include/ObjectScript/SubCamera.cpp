#include "SubCamera.h"
#include "GameObject/Dx11GameObject.h"
#include "Component/Dx11Transform.h"
#include "Scene/Dx11Scene.h"
#include "Component/Dx11Renderer.h"

CSubCamera::CSubCamera()	:
	pSubCamera1Obj(NULL)
{
}


CSubCamera::~CSubCamera()
{
}

CSubCamera::CSubCamera(const CSubCamera & star)
{
}

bool CSubCamera::Init()
{

	
	

	//SAFE_RELEASE(pSubCameraObj);
	return true;
}

void CSubCamera::Input(float fTime)
{
}

int CSubCamera::Update(float fTime)
{

	//if (!pSubCamera1Obj) {
	//	pSubCamera1Obj = m_pScene->CreateCamera("SubCamera1", DX11_PI / 3.f, 0.1f, 1000.f);
	//}
	////컴포넌트 추가
	//
	//int a = 0;

	//SAFE_RELEASE(pSubCamera1Obj);

	//CDx11Transform*	pSubCameraObjTr = pSubCameraObj->GetTransform();
	//int a = 0;

	//SAFE_RELEASE(pSubCameraObj);

	
	/*CDx11Transform*	pPlayerTr = m_pScene->FindObjectFromTag("PlayerObject")->GetTransform();
	Vec3 pPlayerPos = m_pScene->FindObjectFromTag("PlayerObject")->GetTransform()->GetWorldPos();
	Vec3 pPlayerRot = m_pScene->FindObjectFromTag("PlayerObject")->GetTransform()->GetWorldRot();
	
	
	Vec3 m_vWorldAxisZ = m_pScene->FindObjectFromTag("PlayerObject")->GetTransform()->GetWorldAxis(AXIS_Z);
	
	m_pTransform->SetTransform(pPlayerTr);
	
	
	m_pTransform->SetWorldRot(m_vWorldAxisZ.x, m_vWorldAxisZ.y, m_vWorldAxisZ.z-7.f);*/
	
	/*CDx11Transform*	pBulletTr = pSkill2->GetTransform();
	
	pBulletTr->Copy(m_pTransform);
	pBulletTr->SetWorldScale(0.5f, 0.5f, 0.5f);*/
	

	return 0;
}

int CSubCamera::LateUpdate(float fTime)
{
	return 0;
}

void CSubCamera::Render(float fTime)
{
}

CSubCamera * CSubCamera::Clone()
{
	return nullptr;
}

void CSubCamera::OnCollisionEnter(CDx11Collider * pColl, float fTime)
{
}

void CSubCamera::OnCollision(CDx11Collider * pColl, float fTime)
{
}

void CSubCamera::OnCollisionExit(CDx11Collider * pColl, float fTime)
{
}
