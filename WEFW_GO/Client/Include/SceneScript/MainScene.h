#pragma once

#include "Component/Dx11Script.h"
#include "Component/Dx11Terrain.h"

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
	int GetMonsterCount() const;

public:
	virtual bool Init();
	virtual void Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CMainScene* Clone();

private:
	void InitPlayer(CDx11Terrain* pTerrain);
	void InitBullet();
	void InitMonster();
	void InitEffect();
	int CreateSnow();
	int CreateMonster(float fTime);
};

