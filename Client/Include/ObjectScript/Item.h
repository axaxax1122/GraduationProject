#pragma once
#include "Component\Dx11Script.h"

DX11_USING

class CItem :
	public CDx11Script
{
public:
	CItem();
	CItem(const CItem& item);
	~CItem();

private:
	int		m_iHP;

public:
	virtual bool Init();
	virtual void Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CItem* Clone();
	virtual void OnCollisionEnter(class CDx11Collider* pColl, float fTime);
	virtual void OnCollision(class CDx11Collider* pColl, float fTime);
	virtual void OnCollisionExit(class CDx11Collider* pColl, float fTime);
};

