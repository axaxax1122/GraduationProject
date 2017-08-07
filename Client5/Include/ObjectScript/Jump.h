#pragma once
#include "Component\Dx11Script.h"

DX11_USING

class CJump :
	public CDx11Script
{
public:
	CJump();
	~CJump();
	CJump(const CJump& jump);
public:
	float _jumpPower, _gravity;
	bool _isJump;
	
public:
	virtual bool Init();
	virtual void Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CJump* Clone();
	virtual void OnCollisionEnter(class CDx11Collider* pColl, float fTime);
	virtual void OnCollision(class CDx11Collider* pColl, float fTime);
	virtual void OnCollisionExit(class CDx11Collider* pColl, float fTime);
};

