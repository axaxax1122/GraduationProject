#pragma once
#include "Dx11Component.h"

DX11_BEGIN

class DX11_DLL CDx11Camera :
	public CDx11Component
{
public:
	CDx11Camera();
	CDx11Camera(const CDx11Camera& camera);
	~CDx11Camera();
private:
	enum PROJECTION_TYPE
	{
		PT_PERSPECTIVE,
		PT_ORTHOGONAL
	};

private:
	Matrix		m_matView;
	Matrix		m_matProj;
	Vec3		m_vPrevPos;
	float		m_fAngle;
	float		m_fNear;
	float		m_fFar;
	PROJECTION_TYPE	m_eProjType;
	CDx11GameObject*	m_pTarget;

public:
	Matrix GetViewMatrix();
	Matrix GetProjMatrix();
	CDx11GameObject* GetTarget()	const;

public:
	void SetTarget(CDx11GameObject* pTarget);
	void SetProjection(float fAngle, float fNear, float fFar);
	void SetOrthoProjection(float n, float f);
	void SetAngle(float fAngle);
	void SetNear(float fNear);
	void SetDistance(float fDist);
	void TargetTrace();

public:
	virtual bool Init();
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CDx11Camera* Clone();
};

DX11_END