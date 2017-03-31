#include "Item.h"



CItem::CItem()
{
}

CItem::CItem(const CItem & item)	:
	CDx11Script(item)
{
}


CItem::~CItem()
{
}

bool CItem::Init()
{
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
}

void CItem::OnCollision(CDx11Collider * pColl, float fTime)
{
}

void CItem::OnCollisionExit(CDx11Collider * pColl, float fTime)
{
}
