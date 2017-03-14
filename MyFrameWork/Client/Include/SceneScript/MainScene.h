#pragma once

#include "Component/Dx11Script.h"

DX11_USING

class CMainScene :
	public CDx11Script
{
public:
	CMainScene();
	~CMainScene();

private:
	int	m_iMonsterCount;

public:
	int GetMonsterCount();

public:
	virtual bool Init();
	virtual void Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CMainScene* Clone();

public:
	int CreateMonster(float fTime);
};

