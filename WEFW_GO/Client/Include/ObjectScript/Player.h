#pragma once
#include "Component\Dx11Script.h"
#include "Component\Dx11Terrain.h"

DX11_USING

class CPlayer :
	public CDx11Script
{
public:
	CPlayer();
	~CPlayer();

private:
	float				m_fShotDelay;
	bool				m_bShot;
	float				m_fMissileDelay;
	bool				m_bMissile;
	
private:
	int					m_iHP;
	int					m_iHPMax;
	CDx11GameObject*	m_pHPBar;
	CDx11GameObject*	m_pShadowBar;
	float				m_fGap;
	float				m_fValueAcc;

private:
	CDx11Terrain*		m_pTerrain;

private:
	int					m_iPowerUP;

private:
	bool				m_bLeft;
	POINT				m_ptMouse;
	POINT				m_ptOldMouse;

public:
	virtual bool Init();
	virtual void Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CPlayer* Clone();
	virtual void OnCollisionEnter(class CDx11Collider* pColl, float fTime);
	virtual void OnCollision(class CDx11Collider* pColl, float fTime);
	virtual void OnCollisionExit(class CDx11Collider* pColl, float fTime);

private:
	void ShootMissile(const Vec3& vView, float fTime);
	void ShootGuidedMissile(const Vec3& vView, float fTime);
	void ShakeCamera();
	void ComputeShotDelay(bool& bShot, float& fShotDelay, float fTime, float fDelay);
	void TraceCameraView(float fTime);

public:
	void SubtractHP(int iHP);
	void AddHP(int iHP);
	void CreateBar();
	void SetTerrain(class CDx11Terrain* pTerrain);

private:
	class CDx11GameObject* CreateLaser(class CDx11Transform* pTransform, const Vec3& vDir);
	class CDx11GameObject* CreateGuidedMissile(class CDx11Transform* pTransform, const Vec3& vDir);
};

