#include "MoveObject_RIGHT.h"
#include "GameObject/Dx11GameObject.h"
#include "Component/Dx11Transform.h"
#include "Scene/Dx11Layer.h"
#include "Scene/Dx11Scene.h"
#include "GameObject/Dx11GameObject.h"
#include "Component/Dx11Transform.h"
#include "Component/Dx11Collider.h"

CMoveObject_RIGHT::CMoveObject_RIGHT()
{
}


CMoveObject_RIGHT::~CMoveObject_RIGHT()
{
}

CMoveObject_RIGHT::CMoveObject_RIGHT(const CMoveObject_RIGHT & MO)
{
}

bool CMoveObject_RIGHT::Init()
{
	m_fSpeed = 0.f;
	m_fDist = 20.f;
	return true;
}

void CMoveObject_RIGHT::Input(float fTime)
{
}

int CMoveObject_RIGHT::Update(float fTime)
{
	return 0;
}

int CMoveObject_RIGHT::LateUpdate(float fTime)
{
	//static int count = 0;
	//if (count > 150) {
	//	if (m_pGameObject->GetTag() == ("jeep")) {
	//		m_fSpeed = 20.f;
	//		m_pTransform->MoveWorld(m_fSpeed, fTime* 3.f, AXIS_X);
	//	}
	//	if (m_pGameObject->GetTag() == ("HotDogTruck(2)")) {
	//		m_fSpeed = 20.f;
	//		m_pTransform->MoveWorld(m_fSpeed, fTime* 3.f, AXIS_X);
	//	}
	//	//나가면 처음으로 ㄱㄱ
	//	if (m_pTransform->GetWorldPos().x > 210.f) {
	//		m_pTransform->SetWorldPos(-210.f, m_pTransform->GetWorldPos().y, m_pTransform->GetWorldPos().z);
	//	}
	//}
	//else {
	//	count++;
	//}
	return 0;
}

void CMoveObject_RIGHT::Render(float fTime)
{
}

CMoveObject_RIGHT * CMoveObject_RIGHT::Clone()
{
	return nullptr;
}

void CMoveObject_RIGHT::OnCollisionEnter(CDx11Collider * pColl, float fTime)
{

}

void CMoveObject_RIGHT::OnCollision(CDx11Collider * pColl, float fTime)
{
	//맵오브젝트 와 캐릭터
	if (pColl->GetTag() == "PlayerColl")
	{
		CDx11GameObject *pObject = pColl->GetGameObject();
		CDx11Transform * pTrObj = pObject->GetTransform();
		pTrObj->MoveWorld(-20.f, fTime * 2, m_pTransform->GetWorldAxis(AXIS_Z));
		SAFE_RELEASE(pTrObj);
		SAFE_RELEASE(pObject);
	}
}

void CMoveObject_RIGHT::OnCollisionExit(CDx11Collider * pColl, float fTime)
{
}
