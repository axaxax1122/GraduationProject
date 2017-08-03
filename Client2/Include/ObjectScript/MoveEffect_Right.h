#pragma once
#include "Component\Dx11Script.h"

DX11_USING

class CMoveEffect_Right :
	public CDx11Script
{
public:
	CMoveEffect_Right();
	~CMoveEffect_Right();
	CMoveEffect_Right(const CMoveEffect_Right& MO);
private:
	float m_fDist;
	float m_fSpeed;
	string m_Name;
	string	m_strTypeName;
public:
	void SetName(string name) {
		m_Name = name;
	}
	virtual bool Init();
	virtual void Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CMoveEffect_Right* Clone();
	template <typename T>
	void SetTypeName()
	{
		m_strTypeName = typeid(T).name() + 12;
	}
};

