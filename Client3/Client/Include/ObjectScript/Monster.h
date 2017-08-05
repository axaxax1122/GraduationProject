#pragma once
#include "Component\Dx11Script.h"

DX11_USING

class CMonster :
	public CDx11Script
{
public:
	CMonster();
	CMonster(const CMonster& monster);
	~CMonster();

private:
	float	m_fSpeed;
	float	m_fDist;
	float	m_fShotDelay;
	Vec3	m_vDir;
	int		m_iHP;

public:
	void SetSpeed(float fSpeed);
	void SetDirection(const Vec3& v);
	void AddDistance(float fDist);

public:
	virtual bool Init();
	virtual void Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CMonster* Clone();
	virtual void OnCollisionEnter(class CDx11Collider* pColl, float fTime);
	virtual void OnCollision(class CDx11Collider* pColl, float fTime);
	virtual void OnCollisionExit(class CDx11Collider* pColl, float fTime);

public:
	void ShootMissile(const Vec3& vView, float fTime);
	void DeathMotion();
	class CDx11GameObject* CreateBullet(class CDx11Transform* pTransform, const Vec3& vDir);
};

