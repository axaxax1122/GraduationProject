#pragma once
#include "Dx11Component.h"

DX11_BEGIN

class DX11_DLL CDx11Transform :
	public CDx11Component
{
private:
	friend class CDx11GameObject;

private:
	CDx11Transform();
	CDx11Transform(const CDx11Transform& transform);
	~CDx11Transform();

public:
	virtual bool Init();
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CDx11Transform* Clone();

private:
	XMVECTOR		m_vQuarternion;
	ROTATION_FLAG	m_eRotFlag;

// Local //
private:	
	Vec3		m_vLocalScale;				//크기
	Vec3		m_vLocalRot;				//자전
	Vec3		m_vLocalPos;				//위치
	Vec3		m_vLocalMove;
	Vec3		m_vLocalAxis[AXIS_MAX];		//축
	Vec3		m_vLocalView;

	Matrix		m_matLocalScale;
	Matrix		m_matLocalRotX;
	Matrix		m_matLocalRotY;
	Matrix		m_matLocalRotZ;
	Matrix		m_matLocalRot;
	Matrix		m_matLocalTranslate;
	Matrix		m_matLocal;					

public:
	Vec3 GetLocalScale();
	Vec3 GetLocalRot();
	float GetLocalRotX();
	float GetLocalRotY();
	float GetLocalRotZ();
	Vec3 GetLocalPos();
	Vec3 GetLocalView();
	const Vec3* GetLocalAxis();
	Vec3 GetLocalAxis(AXIS axis);
	Matrix GetLocalMatrix();

public:
	void SetLocalScale(const Vec3& vScale);
	void SetLocalScale(float x, float y, float z);
	void SetLocalRot(const Vec3& vRot);
	void SetLocalRot(float x, float y, float z);
	void SetLocalRotX(float x);
	void SetLocalRotY(float y);
	void SetLocalRotZ(float z);
	void SetLocalPos(const Vec3& vPos);
	void SetLocalPos(float x, float y, float z);
	void SetLocalView(const Vec3& vView);
	void SetLocalView(float x, float y, float z);

public:
	void ComputeLocalAxis();
	void MoveLocal(float fSpeed, float fTime, AXIS axis);
	void MoveLocal(const Vec3& vMove);
	void MoveLocal(float fSpeed, float fTime, const Vec3& v);
	void ScaleLocal(const Vec3& vScale);
	void RotateLocal(const Vec3& vRot);
	void RotationLocal(float fAngle, float fTime, AXIS axis);
	void RotationLocal(float fAngle, float fTime, const Vec3& vRot);

// World //
private:	
	Vec3		m_vWorldScale;
	Vec3		m_vWorldFinalScale;
	Vec3		m_vWorldRot;
	Vec3		m_vWorldFinalRot;
	Vec3		m_vWorldPos;
	Vec3		m_vWorldFinalPos;
	Vec3		m_vWorldMove;
	Vec3		m_vWorldAxis[AXIS_MAX];
	Vec3		m_vWorldView;

	Matrix		m_matWorldScale;
	Matrix		m_matWorldRotX;
	Matrix		m_matWorldRotY;
	Matrix		m_matWorldRotZ;
	Matrix		m_matWorldRot;
	Matrix		m_matWorldTranslate;
	Matrix		m_matWorld;

public:
	Vec3 GetWorldScale();
	Vec3 GetWorldRot();
	float GetWorldRotX();
	float GetWorldRotY();
	float GetWorldRotZ();
	Vec3 GetWorldPos();
	Vec3 GetWorldView();
	const Vec3* GetWorldAxis();
	Vec3 GetWorldAxis(AXIS axis);
	Matrix GetWorldMatrix();
	Matrix GetWorldScaleMatrix();
	Matrix GetWorldRotMatrix();
	Matrix GetWorldTranslateMatrix();

public:
	void SetWorldScale(const Vec3& vScale);
	void SetWorldScale(float x, float y, float z);
	void SetWorldRot(const Vec3& vRot);
	void SetWorldRot(float x, float y, float z);
	void SetWorldRotX(float x);
	void SetWorldRotY(float y);
	void SetWorldRotZ(float z);
	void SetWorldRotMatrix(const Matrix& matRot);
	void SetWorldPos(const Vec3& vPos);
	void SetWorldPos(float x, float y, float z);
	void SetWorldView(const Vec3& vView);
	void SetWorldView(float x, float y, float z);
	void SetWorldAxis(const Vec3& vAxis, AXIS axis);

public:
	void ComputeWorldAxis();
	void ComputeRotationPos();
	void SetQuarternion(const XMVECTOR& vQuarternion);

public:
	void MoveWorld(float fSpeed, float fTime, AXIS axis);
	void MoveWorld(float fSpeed, float fTime);
	void MoveWorld(float fSpeed, float fTime, const Vec3& v);
	void RotationWorld(float fAngle, float fTime, AXIS axis);
	void RotationWorld(float fAngle, float fTime, const Vec3& vRot);
	void ScaleWorld(const Vec3& vScale);
	void MoveWorld(const Vec3& vMove);
	void RotateWorld(const Vec3& vRot);
	
private:
	bool	m_bStatic;		//정적 객체 여부
	bool	m_bUpdate;		//업데이트 여부
	int		m_iParentFlag;	//부모행렬 연산 플래그

public:
	void Static(bool bStatic);
	void ClearParentFlag();
	void SetParentFlag(int iFlag);
	void AddParentFlag(int iFlag);
	void DeleteParentFlag(int iFlag);
	void SetRotationFlag(ROTATION_FLAG eFlag);
	void EnableUpdate();

public:
	bool GetStatic();

public:
	void Copy(CDx11Transform* pTransform);
};

DX11_END