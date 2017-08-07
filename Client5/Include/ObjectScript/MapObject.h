#pragma once
#include "Component\Dx11Script.h"
DX11_USING
class CMapObject :
	public CDx11Script
{
public:
	CMapObject();
	CMapObject(const CMapObject& map);
	~CMapObject();	

public:
	Vec3 ObjPos;
	Vec3 ObjSize;
	Vec3 ObjRot;
	Vec3 ObjHalfSize;

	Vec3 PlayerPos;
	Vec3 PlayerSize;
	Vec3 PlayerHalfSize;

	float My_front;
	float My_back;
	float My_Right;
	float My_Left;
	float My_up;
	float My_down;

	float Obj_Front;
	float Obj_Back;
	float Obj_Right;
	float Obj_Left;
	float Obj_Up;
	float Obj_Down;

	float m_fNomalX;
	float m_fNomalZ;

	CDx11GameObject *pPlayer;
	class CPlayerCollision *pPlayerColl;
	float m_Count = 0;
public:
	bool left_collision(CDx11GameObject *pPlayer);
	bool right_collision(CDx11GameObject *pPlayer);
	bool front_collision(CDx11GameObject *pPlayer);
	bool back_collision(CDx11GameObject *pPlayer);
	bool Down_collision(CDx11GameObject *pPlayer);
	bool Back_Left_collision(CDx11GameObject *pPlayer);
	bool Back_Right_collision(CDx11GameObject *pPlayer);
	bool Front_Left_collision(CDx11GameObject *pPlayer);
	bool Front_Right_collision(CDx11GameObject *pPlayer);

	void MyLocation(CDx11GameObject *pPlayer);

public:
	void SetSize(Vec3 vSize);

public:
	virtual bool Init();
	virtual void Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CMapObject* Clone();
	virtual void OnCollisionEnter(class CDx11Collider* pColl, float fTime);
	virtual void OnCollision(class CDx11Collider* pColl, float fTime);
	virtual void OnCollisionExit(class CDx11Collider* pColl, float fTime);
};

