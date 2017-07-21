#pragma once
#include "Component\Dx11Script.h"

DX11_USING

class CUser :
	public CDx11Script
{
public:
	CUser();
	~CUser();
	CUser(const CUser & user);

private:
	INT64		m_iSerialNumber;
	list<CUser*> m_UserObj;

private:
	int					m_iHP;
	int					m_iHPMax;
	CDx11GameObject*	m_pHPBar;
	CDx11GameObject*	m_pShadowBar;
	float				m_fGap;
	float				m_fValueAcc;

public:
	INT64 GetSerialNumber()	const
	{
		return m_iSerialNumber;
	}

	void SetSerialNumber(INT64 iSerialNumber)
	{
		m_iSerialNumber = iSerialNumber;
	}

public:
	void AddUserObj(CUser* pObj);
	CUser* FindUserObj(INT64 SerialNumber);
public:
	virtual bool Init();
	virtual void Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CUser* Clone();
	virtual void OnCollisionEnter(class CDx11Collider* pColl, float fTime);
	virtual void OnCollision(class CDx11Collider* pColl, float fTime);
	virtual void OnCollisionExit(class CDx11Collider* pColl, float fTime);
	
public:
	void CreateBar();
	void SubtractHP(int iHP);
	void AddHP(int iHP);
};

