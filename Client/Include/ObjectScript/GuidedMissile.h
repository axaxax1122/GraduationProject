#pragma once
#include "Component\Dx11Script.h"


DX11_USING

class CGuidedMissile :
	public CDx11Script
{
public:
	CGuidedMissile();
	CGuidedMissile(const CGuidedMissile& missile);
	~CGuidedMissile();

private:
	float	m_fSpeed;
	float	m_fDist;
	Vec3	m_vDir;
	Vec3	m_vRot;

private:
	class CDx11GameObject*	m_pMonster;

public:
	void SetRotationAxis(const Vec3& vRot);
	void SetDirection(const Vec3& vDir);
	void SetTarget(CDx11GameObject* pTarget);
	void DeathMotion();

public:
	virtual bool Init();
	virtual void Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CGuidedMissile* Clone();

public:
	class CDx11GameObject* FindTarget();

};

