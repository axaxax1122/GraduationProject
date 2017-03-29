#pragma once
#include "Component\Dx11Script.h"
#include "Component/Dx11UIBar.h"
DX11_USING

class CPlayer :
	public CDx11Script
{
public:
	CPlayer();
	~CPlayer();

private:
	float		m_fSolWSpeed;
	float		m_fSolWLimit;
	float		m_fSolWAcc;
	bool		m_bSolWEnable;
	float		m_fFireTime;
	bool		m_bFire;
	bool		GoBullet;
	float		count;
	Vec3		OriginalPos;
	Vec3		PlayerPos;
	Vec3		PlayerRot;
	Vec3		New;	
	bool		m_bUIHpBarDown;
	bool		m_bUIHpBarDown2;
	bool		m_bUIHpBarUP;
	bool		m_bUIHpBarUP2;

	float		m_fcount;
	float		m_fcount2;
	float		m_fcount3;
	float		m_fcount4;
	float		m_fDamage;
	float		m_fDamage2;
	bool		m_btemp;
	bool		m_btemp2;
	float		m_fHPUP;
	float		m_fHPUP2;
	float		m_fHPUP3;
	float		m_fHPUP4;
	int			m_iHP;
	int			m_iHP2;
	int			m_iHP3;
	int			m_iHP4;
	int			m_iHPMax;
	CDx11UIBar*	m_pHPBar;
	CDx11UIBar*	m_pHPSlowBar;

public:
	void SetHPBar(CDx11UIBar* pBar);
	void SetSlowHPBar(CDx11UIBar* pBar);
	void Damage(float iDamage);
	void SlowDamage(float iDamage);
	void SleepGo();
	void SleepAddGo();
	void AddHP(float iHP);
	void AddSleepHP(float iHP);

public:
	virtual bool Init();
	virtual void Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CPlayer* Clone();

private:
	void Fire();
	void SolW(float fTime);
	void Skill2();
	class CDx11GameObject* CreateBullet(class CDx11Transform* pTransform);
};

