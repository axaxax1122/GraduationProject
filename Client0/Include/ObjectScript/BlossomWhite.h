#pragma once
#include "Component\Dx11Script.h"

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

};

