#pragma once
#include "Component\Dx11Script.h"

DX11_USING
class CMinimapMove :
	public CDx11Script
{
public:
	CMinimapMove();
	~CMinimapMove();
	CMinimapMove(const CMinimapMove& MO);

	
public:
	virtual bool Init();
	virtual void Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CMinimapMove* Clone();
	template <typename T>
	void SetTypeName()
	{
		m_strTypeName = typeid(T).name() + 12;
	}

};

