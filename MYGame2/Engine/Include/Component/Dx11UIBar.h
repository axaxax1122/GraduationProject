#pragma once
#include "Dx11UI.h"

DX11_BEGIN

class DX11_DLL CDx11UIBar :
	public CDx11UI
{
private:
	friend class CDx11GameObject;

private:
	CDx11UIBar();
	CDx11UIBar(const CDx11UIBar& ui);
	~CDx11UIBar();

private:
	Vec2		m_vBarSize;
	Vec2		m_vBarPos;
	float		m_fMin;
	float		m_fMax;
	float		m_fValue;
	UIBAR_DIR	m_eBarDir;

public:
	void SetBarDir(UIBAR_DIR eDir);
	void SetBarSize(const Vec2& vSize);
	void SetBarSize(float x, float y);
	void SetBarPos(const Vec2& vPos);
	void SetBarPos(float x, float y);
	void SetValueMinMax(float fMin, float fMax);
	void SetValue(float fValue);
	void AddValue(float fValue);

public:
	virtual bool Init();
	virtual void Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CDx11UIBar* Clone();
};

DX11_END
