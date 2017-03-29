#include "Monster.h"
#include "Component/Dx11Collider.h"
#include "GameObject/Dx11GameObject.h"
#include "Scene/Dx11Scene.h"
#include "Scene/Dx11Layer.h"
CMonster::CMonster()
{
}

CMonster::CMonster(const CMonster & monster)	:
	CDx11Script(monster)
{
}

CMonster::~CMonster()
{
}

bool CMonster::Init()
{
	CDx11Layer*	pLayer = m_pScene->FindLayer("DefaultLayer");
	if (!pLayer)
		return false;

	return true;
}

void CMonster::Input(float fTime)
{
}

int CMonster::Update(float fTime)
{
	return 0;
}

int CMonster::LateUpdate(float fTime)
{
	return 0;
}

void CMonster::Render(float fTime)
{
}

CMonster * CMonster::Clone()
{
	return new CMonster(*this);
}

void CMonster::OnCollisionEnter(CDx11Collider * pColl, float fTime)
{
	//if (pColl->GetTag() == "BulletColl")
	//{
	//	CDx11GameObject*	pObj = pColl->GetGameObject();
	//	pObj->Death();//총알 삭제
	//	SAFE_RELEASE(pObj);

	//	//자기자신 삭제.
	//	//m_pGameObject->Death();
	//}
}

void CMonster::OnCollision(CDx11Collider * pColl, float fTime)
{
}

void CMonster::OnCollisionExit(CDx11Collider * pColl, float fTime)
{
	/*if (pColl->GetTag() == "BulletColl")
	{
		CDx11GameObject*	pObj = pColl->GetGameObject();

		pObj->Death();

		SAFE_RELEASE(pObj);

		m_pGameObject->Death();
	}*/
}
