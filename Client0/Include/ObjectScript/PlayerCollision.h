#pragma once
#include "Component\Dx11Script.h"
DX11_USING

enum PlayerState {
	NoColl,
	LeftColl,
	RightColl,
	FrontColl,
	BackColl,
	DownColl,
	UpColl,
	Eege_Back_LeftColl,
	Eege_Back_RightColl,
	Eege_Front_LeftColl,
	Eege_Front_RightColl

};

class CPlayerCollision :
	public CDx11Script
{
public:
	CPlayerCollision();
	~CPlayerCollision();
	CPlayerCollision(const CPlayerCollision& Coll);

public:
	PlayerState PS;
	CDx11GameObject *m_pObj;

	float PlayerY;
	Vec3 ObjPos;
	Vec3 ObjSize;
	Vec3 ObjHalfSize;

	Vec3 PlayerPos;
	Vec3 PlayerSize;
	Vec3 PlayerHalfSize;

	Vec3 PlayerNomal;
	Vec3 slidingVector;
	Vec3 m_vNomalObj;
	Vec3 m_vPlayerNomal;
	Vec3 One;
	Vec3 Two;
	Vec3 NomalOne;
	Vec3 NomalTwo;
public:
	void SetPS(PlayerState ps);
	void SetPS(PlayerState ps, CDx11GameObject *pObj);
	PlayerState GetPS();
public:
	virtual bool Init();
	virtual void Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CPlayerCollision* Clone();
	virtual void OnCollisionEnter(class CDx11Collider* pColl, float fTime);
	virtual void OnCollision(class CDx11Collider* pColl, float fTime);
	virtual void OnCollisionExit(class CDx11Collider* pColl, float fTime);
};

