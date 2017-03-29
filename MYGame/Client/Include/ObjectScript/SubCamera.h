#pragma once
#include "Component\Dx11Script.h"

DX11_USING

class CSubCamera :
	public CDx11Script
{
public:
	CSubCamera();
	~CSubCamera();

	CSubCamera(const CSubCamera& star);

public:
	virtual bool Init();
	virtual void Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CSubCamera* Clone();
	virtual void OnCollisionEnter(class CDx11Collider* pColl, float fTime);
	virtual void OnCollision(class CDx11Collider* pColl, float fTime);
	virtual void OnCollisionExit(class CDx11Collider* pColl, float fTime);

private:
	CDx11GameObject*	pSubCamera1Obj;
};


