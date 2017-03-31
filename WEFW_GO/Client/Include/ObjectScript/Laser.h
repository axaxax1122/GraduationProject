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

public:
	void SetSpeed(float fSpeed);
	void SetDistance(float fDist);

public:
	virtual bool Init();
	virtual void Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CLaser* Clone();
};

