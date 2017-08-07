#pragma once
#include "Component\Dx11Script.h"

DX11_USING

class CSnow :
	public CDx11Script
{
public:
	CSnow();
	~CSnow();
	CSnow(const CSnow& snow);
private:
	float m_fDist;
	float m_fSpeed;
	string	m_strTypeName;
	Vec3 m_vSize;
	Vec3 m_vSizeTemp;
	int m_random_size;
	int m_random_x;
	int m_random_y;
	HWND		m_hWnd;
	POINT		m_ptPos;
	POINT		m_ptPrevPos;
	Vec2		m_ptGap;
	POINT pt;
	RECT rect;
	float temp;
public:
	virtual bool Init();
	virtual void Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CSnow* Clone();
	virtual void OnCollisionEnter(class CDx11Collider* pColl, float fTime);
	virtual void OnCollision(class CDx11Collider* pColl, float fTime);
	virtual void OnCollisionExit(class CDx11Collider* pColl, float fTime);
	template <typename T>
	void SetTypeName()
	{
		m_strTypeName = typeid(T).name() + 12;
	}
public:
	void ChraterMove(float fTime);
	void Exception();
	void Mouse(float fTime);
};

