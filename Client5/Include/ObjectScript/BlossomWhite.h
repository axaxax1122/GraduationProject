#pragma once
#include "Component\Dx11Script.h"
#include "Core/Dx11Input.h"

DX11_USING

class CBlossomWhite :
	public CDx11Script
{
public:
	CBlossomWhite();
	~CBlossomWhite();
	CBlossomWhite(const CBlossomWhite& BW);
private:
	float m_fDist;
	float m_fSpeed;
	string	m_strTypeName;
	Vec3 m_vSize;
	Vec3 m_vSizeTemp;
	int m_random_size;
	int m_random_x;
	int m_random_y;
	CDx11Mouse*	pMouse;

	enum DIRECTION
	{
		DIR_FRONT,
		DIR_BACK,
		DIR_LEFT,
		DIR_RIGHT,
		DIR_FRONT_LEFT,
		DIR_FRONT_RIGHT,
		DIR_BACK_LEFT,
		DIR_BACK_RIGHT
	};
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
	virtual CBlossomWhite* Clone();
	
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

