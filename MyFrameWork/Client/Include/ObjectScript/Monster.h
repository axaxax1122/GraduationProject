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
	Vec3	m_vDir;

public:
	void SetSpeed(float fSpeed);
	void SetDirection(const Vec3& v);

public:
	virtual bool Init();
	virtual void Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CMonster* Clone();
};

