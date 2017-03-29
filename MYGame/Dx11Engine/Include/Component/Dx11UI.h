#pragma once
#include "Dx11Component.h"

DX11_BEGIN

class DX11_DLL CDx11UI :
	public CDx11Component
{
protected:
	CDx11UI();
	CDx11UI(const CDx11UI& ui);
	virtual ~CDx11UI();

public:
	virtual bool Init();
	virtual void Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CDx11UI* Clone() = 0;
	virtual void OnCollisionEnter(class CDx11Collider* pColl, float fTime);
	virtual void OnCollision(class CDx11Collider* pColl, float fTime);
	virtual void OnCollisionExit(class CDx11Collider* pColl, float fTime);
};

DX11_END
