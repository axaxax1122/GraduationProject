#pragma once
#include "Dx11UI.h"

DX11_BEGIN

class DX11_DLL CDx11SnowUI :
	public CDx11UI
{
private:
	friend class CDx11GameObject;

public:
	CDx11SnowUI();
	~CDx11SnowUI();
	CDx11SnowUI(const CDx11SnowUI& ui);

private:
	Vec2		m_vBarPos;
	Vec2		m_vBarSize;
public:
	void SetBarPos(float x, float y, float z);
	void SetBarSize(float x, float y);

public:
	virtual bool Init();
	virtual void Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CDx11SnowUI* Clone();
	template <typename T>
	void SetTypeName()
	{
		m_strTypeName = typeid(T).name() + 12;
	}
};

DX11_END