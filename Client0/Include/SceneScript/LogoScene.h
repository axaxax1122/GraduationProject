#pragma once
#include "Component\Dx11Script.h"
#include "Component\Dx11UIButton.h"

DX11_USING

class CLogoScene :
	public CDx11Script
{
public:
	CLogoScene();
	~CLogoScene();

private:
	FADE_STATE	m_eState;
	float		m_fFadeAmount;

	enum GAME_FLAG
	{
		FLAG_NONE,
		FLAG_START,
		FLAG_EXIT
	};

	GAME_FLAG	m_eFlag;

public:
	virtual bool Init();
	virtual void Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CLogoScene* Clone();

public:
	int StartCallBack(CDx11UIButton* pBtn, float fTime);
	int ExitCallBack(CDx11UIButton* pBtn, float fTime);
	void FadeOut(float fTime);
	void FadeIn(float fTime);
	void GameStart();
	void GameExit();
};



