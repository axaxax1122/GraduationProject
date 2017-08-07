#pragma once
#include "Component\Dx11Script.h"

DX11_USING

class CLaser :
	public CDx11Script
{
public:
	CLaser();
	CLaser(const CLaser& laser);
	~CLaser();

private:
	float		m_fSpeed;
	float		m_fDist;
	Vec3		m_vDir;

public:
	void SetSpeed(float fSpeed);
	void SetDistance(float fDist);
	void SetDirection(const Vec3& vDir);
	void DeathMotion();

public:
	virtual bool Init();
	virtual void Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CLaser* Clone();

public:
	virtual void OnCollisionEnter(class CDx11Collider* pColl, float fTime);
	virtual void OnCollision(class CDx11Collider* pColl, float fTime);
	virtual void OnCollisionExit(class CDx11Collider* pColl, float fTime);
};

