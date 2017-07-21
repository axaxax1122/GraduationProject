#pragma once

#include "Component/Dx11Script.h"
#include "Component/Dx11Terrain.h"
#include "Scene/QuadTree.h"

DX11_USING

class CMainScene :
	public CDx11Script
{
public:
	CMainScene();
	~CMainScene();

private:
	class CDx11GameObject* m_pMyPlayer;
	class CDx11GameObject* m_pUserPlayer;
	class CDx11Transform * m_pUserTr;

	class CDx11Transform* m_pLeftCarTr;
	class CDx11Transform* m_pRightCarTr;
	class CDx11Transform* m_pLeftCarTr2;
	class CDx11Transform* m_pRightCarTr2;

	bool m_bTextFlag;

private:
	int	m_iMonsterCount;
	HANDLE uHandle;

public:
	int GetMonsterCount() const;

public:
	virtual bool Init();
	virtual void Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CMainScene* Clone();

public:
	void InitPlayer(BYTE * pPacket, CDx11Terrain* pTerrain);
	void InitBullet();
	void InitMonster();
	void InitEffect();
	void InitMapEffect();
	int InitMiniMap();
	int CreateSnow();
	int CreateMonster(float fTime);
	void CreateQuadTreeRender(class QuadTree* pNode);

public:
	void MovePlayer(BYTE* pPacket);
	void MoveOtherPlayer(BYTE* pPacket);
	void DieOtherPlayer(BYTE* pPacket);
	void RotateOtherPlayer(BYTE* pPacket);
	void JumpOtherPlayer(BYTE* pPacket);
	void AnimOtherPlayer(BYTE* pPacket);
	void ShootOtherPlayer(BYTE* pPacket);
	void HeatPlayer(BYTE* pPacket);

	void CreateBox(BYTE* pPacket);
	void LeftCarMove(BYTE* pPacket);
	void RightCarMove(BYTE* pPacket);

	void WaitText();

public:
	void LoadResource();
};

