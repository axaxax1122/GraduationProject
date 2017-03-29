#pragma once
#include "Dx11Collider.h"

DX11_BEGIN

class DX11_DLL CDx11ColliderRC :
	public CDx11Collider
{
private:
	friend class CDx11GameObject;

private:
	CDx11ColliderRC();
	CDx11ColliderRC(const CDx11ColliderRC& collider);
	~CDx11ColliderRC();

private:
	RECTINFO		m_tInfo;

public:
	void SetInfo(float l, float t, float r, float b);
	RECTINFO GetInfo();

public:
	virtual bool Init();
	virtual void Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CDx11ColliderRC* Clone();

public:
	virtual bool Collision(CDx11Collider* pColl);
};

DX11_END
