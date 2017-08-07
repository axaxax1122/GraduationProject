#include "Item.h"
#include "Component/Dx11Transform.h"
#include "GameObject/Dx11GameObject.h"
#include "Laser.h"
#include "Component/Dx11Collider.h"

CItem::CItem() :
	m_iHP(0)
{

}

CItem::CItem(const CItem & item) :
	CDx11Script(item)
{
}


CItem::~CItem()
{
}

bool CItem::Init()
{
	m_iHP = 100;
	return true;
}

void CItem::Input(float fTime)
{
}

int CItem::Update(float fTime)
{
	return 0;
}

int CItem::LateUpdate(float fTime)
{

	return 0;
}

void CItem::Render(float fTime)
{
}

CItem * CItem::Clone()
{
	return new CItem(*this);
}

void CItem::OnCollisionEnter(CDx11Collider * pColl, float fTime)
{
	//if ((pColl->GetTag() == "MyLaserColl") ||
	//	(pColl->GetTag() == "UserLaserColl"))
	//{
	//	CDx11GameObject*	pObj = pColl->GetGameObject();
	//	CDx11Transform*		pTr = pObj->GetTransform();
	//	pColl->SetColor(Red);
	//	Vec3	vPos = pTr->GetWorldPos();
	//	CLaser*	pLaser = (CLaser*)pObj->FindComponentFromType(CT_SCRIPT);
	//	pLaser->DeathMotion();
	//	pObj->Death();
	//	SAFE_RELEASE(pLaser);
	//	SAFE_RELEASE(pTr);
	//	SAFE_RELEASE(pObj);

	//}
}

void CItem::OnCollision(CDx11Collider * pColl, float fTime)
{
}

void CItem::OnCollisionExit(CDx11Collider * pColl, float fTime)
{
}
