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
	enum DIRECTION
	{
		DIR_FRONT,
		DIR_BACK,
		DIR_LEFT,
		DIR_RIGHT,
		DIR_FRONT_LEFT,
		DIR_FRONT_RIGHT,
		DIR_BACK_LEFT,
		DIR_BACK_RIGHT,
		DIR_NOMAL,//초기화시
	};
	float				m_fShotDelay;
	bool				m_bShot;
	float				m_fMissileDelay;
	bool				m_bMissile;
	
private:
	int					m_iHP;
	int					m_iHP_User;
	int					m_iHPMax;
	int					m_iHPMax_User;
	CDx11GameObject*	m_pHPBar;
	CDx11GameObject*	m_pShadowBar;
	CDx11GameObject*	m_pHPBar_User;
	CDx11GameObject*	m_pShadowBar_User;
	float				m_fGap;
	float				m_fValueAcc;
	int					m_iPowerUP;
	DIRECTION			m_eDir;
private:
	CDx11Terrain*		m_pTerrain;

private:
	bool				m_bLeft;
	POINT				m_ptMouse;
	POINT				m_ptOldMouse;

private:
	bool				m_bControl;

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

public:
	void ShootMissile(const Vec3& vView, float fTime);
	void ShootGuidedMissile(const Vec3& vView, float fTime);
	void ShakeCamera();
	void ComputeShotDelay(bool& bShot, float& fShotDelay, float fTime, float fDelay);
	void TraceCameraView(float fTime);
	void TurnToDirection(DIRECTION eDir, float fTime);
	void MoveFront(float fTime);
	void MoveDash(float fTime);
	void RotatePlayer(float fAngle, float fTime, const Vec3& vCross);
	void RotatePlayer(float fAngle, float fTime, const AXIS vAxis);
public:
	DIRECTION GetDIRECTION() {
		return m_eDir;
	}

public:
	void SubtractHP(int iHP);
	void AddHP(int iHP);
	void SubtractHP_User(int iHP);
	void AddHP_User(int iHP);

	void UpdateUserHP();
	int GetHP() {
		return m_iHP;
	}
	void CreateBar();
	void CreatePlayeruUI();
	void CreateUserUI();//임시!
	void SetTerrain(class CDx11Terrain* pTerrain);

private:
	class CDx11GameObject* CreateLaser(class CDx11Transform* pTransform, const Vec3& vDir);
	class CDx11GameObject* CreateGuidedMissile(class CDx11Transform* pTransform, const Vec3& vDir);
};

