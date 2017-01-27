#pragma once
#include "Component\Dx11Script.h"

DX11_USING

class CPlayer :
	public CDx11Script
{
public:
	CPlayer();
	~CPlayer();

private:
	float m_fUpdateTime;

public:
	virtual bool Init();
	virtual void Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CPlayer* Clone();

private:
	void ShootMissile(const Vec3& vView);
	void ShakeCamera();
	class CDx11GameObject* CreateBullet(class CDx11Transform* pTransform, const Vec3& vDir);
};

