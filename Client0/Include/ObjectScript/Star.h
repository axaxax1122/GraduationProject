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
};

