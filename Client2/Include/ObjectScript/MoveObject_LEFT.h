#pragma once
#include "Component\Dx11Script.h"

DX11_USING

class CMoveObject_LEFT :
	public CDx11Script
{
public:
	CMoveObject_LEFT();
	~CMoveObject_LEFT();
	CMoveObject_LEFT(const CMoveObject_LEFT& MO);
private:
	float m_fDist;
	float m_fSpeed;
	string	m_strTypeName;
public:
	virtual bool Init();
	virtual void Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CMoveObject_LEFT* Clone();
	template <typename T>
	void SetTypeName()
	{
		m_strTypeName = typeid(T).name() + 12;
	}
	virtual void OnCollisionEnter(class CDx11Collider* pColl, float fTime);
	virtual void OnCollision(class CDx11Collider* pColl, float fTime);
	virtual void OnCollisionExit(class CDx11Collider* pColl, float fTime);
};

