#include "MoveObject_LEFT.h"
#include "GameObject/Dx11GameObject.h"
#include "Component/Dx11Transform.h"
#include "Scene/Dx11Layer.h"
#include "Scene/Dx11Scene.h"
#include "GameObject/Dx11GameObject.h"
#include "Component/Dx11Transform.h"
#include "Component/Dx11Collider.h"

CMoveObject_LEFT::CMoveObject_LEFT()
{
}
CMoveObject_LEFT::~CMoveObject_LEFT()
{
}
CMoveObject_LEFT::CMoveObject_LEFT(const CMoveObject_LEFT & MO)
{
}

bool CMoveObject_LEFT::Init()
{
	m_fSpeed = 0;
	m_fDist = 20.f;
	return true;
}

void CMoveObject_LEFT::Input(float fTime)
{
}

int CMoveObject_LEFT::Update(float fTime)
{
	return 0;
}

int CMoveObject_LEFT::LateUpdate(float fTime)
{
	//static int count = 0;
	//if (count > 150) {
	//	if (m_pGameObject->GetTag() == ("CC_ME_R4(2)")) {
	//		m_fSpeed = -15.f;
	//		m_pTransform->MoveWorld(m_fSpeed, fTime* 2.f, AXIS_X);
	//	}
	//	if (m_pGameObject->GetTag() == ("HotDogTruck(1)")) {
	//		m_fSpeed = -15.f;
	//		m_pTransform->MoveWorld(m_fSpeed, fTime* 2.f, AXIS_X);
	//	}
	//	//나가면 처음으로 ㄱㄱ
	//	if (m_pTransform->GetWorldPos().x < -200.f) {
	//		m_pTransform->SetWorldPos(210.f, m_pTransform->GetWorldPos().y, m_pTransform->GetWorldPos().z);
	//	}
	//}
	//else {
	//	count++;
	//}
	return 0;
}

void CMoveObject_LEFT::Render(float fTime)
{
}
CMoveObject_LEFT * CMoveObject_LEFT::Clone()
{
	return new CMoveObject_LEFT(*this);
}

void CMoveObject_LEFT::OnCollisionEnter(CDx11Collider * pColl, float fTime)
{

}

void CMoveObject_LEFT::OnCollision(CDx11Collider * pColl, float fTime)
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

void CMoveObject_LEFT::OnCollisionExit(CDx11Collider * pColl, float fTime)
{
}
