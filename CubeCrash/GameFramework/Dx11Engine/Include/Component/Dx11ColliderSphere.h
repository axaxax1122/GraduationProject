#pragma once
#include "Dx11Collider.h"

DX11_BEGIN

class DX11_DLL CDx11ColliderSphere :
	public CDx11Collider
{
public:
	CDx11ColliderSphere();
	CDx11ColliderSphere(const CDx11ColliderSphere& collider);
	~CDx11ColliderSphere();

private:
	Vec3		m_vCenter;
	Vec3		m_vPivot;
	float		m_fRadius;

public:
	Vec3 GetCenter() const;
	float GetRadius() const;

public:
	void SetInfo(const Vec3& vCenter,float fRadius);

public:
	virtual bool Init();
	virtual void Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CDx11ColliderSphere* Clone();

public:
	virtual bool Collision(CDx11Collider* pColl);
};

DX11_END