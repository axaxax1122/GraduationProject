#pragma once
#include "Component\Dx11Script.h"

DX11_USING

class CLoadingScene :
	public CDx11Script
{
public:
	CLoadingScene();
	~CLoadingScene();

private:
	FADE_STATE	m_eState;
	float		m_fFadeAmount;

public:
	virtual bool Init();
	virtual void Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CLoadingScene* Clone();

public:
	void FadeOut(float fTime);
	void FadeIn(float fTime);
};

