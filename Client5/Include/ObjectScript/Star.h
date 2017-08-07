#pragma once
#include "Component\Dx11Script.h"

DX11_USING
class CStar :
	public CDx11Script
{
public:
	CStar();
	~CStar();
	CStar(const CStar& star);

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
public:
	virtual bool Init();
	virtual void Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CStar* Clone();

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

