#include "Dx11Transform.h"
#include "../GameObject/Dx11GameObject.h"

DX11_USING

CDx11Transform::CDx11Transform()
{
	m_eComType = CT_TRANSFORM;
	SetTypeName<CDx11Transform>();
	m_bStatic = false;
	m_bUpdate = true;
	m_iParentFlag = TPF_ALL;
	m_eRotFlag = RF_XYZ;
	m_vQuarternion = DirectX::XMQuaternionRotationRollPitchYaw(0.f, 0.f, 0.f);
}

CDx11Transform::CDx11Transform(const CDx11Transform & transform)
{
	*this = transform;
	m_iRefCount = 1;
}

CDx11Transform::~CDx11Transform()
{
}

bool CDx11Transform::Init()
{
	// Local 초기화
	m_vLocalScale = Vec3One;
	m_vLocalMove = Vec3Zero;
	m_vLocalRot = Vec3Zero;
	m_vLocalPos = Vec3Zero;
	m_vLocalAxis[AXIS_X] = Vec3X;
	m_vLocalAxis[AXIS_Y] = Vec3Y;
	m_vLocalAxis[AXIS_Z] = Vec3Z;

	m_matLocalScale = XMMatrixIdentity();
	m_matLocalRotX = XMMatrixIdentity();
	m_matLocalRotY = XMMatrixIdentity();
	m_matLocalRotZ = XMMatrixIdentity();
	m_matLocalRot = XMMatrixIdentity();
	m_matLocalTranslate = XMMatrixIdentity();
	m_matLocal = XMMatrixIdentity();

	// World 초기화
	m_vWorldScale = Vec3One;
	m_vWorldRot = Vec3Zero;
	m_vWorldPos = Vec3Zero;
	m_vWorldFinalScale = Vec3One;
	m_vWorldFinalPos = Vec3Zero;
	m_vWorldFinalRot = Vec3Zero;
	m_vWorldAxis[AXIS_X] = Vec3X;
	m_vWorldAxis[AXIS_Y] = Vec3Y;
	m_vWorldAxis[AXIS_Z] = Vec3Z;
	m_vWorldView = Vec3Y;

	m_matWorldScale = XMMatrixIdentity();
	m_matWorldRotX = XMMatrixIdentity();
	m_matWorldRotY = XMMatrixIdentity();
	m_matWorldRotZ = XMMatrixIdentity();
	m_matWorldRot = XMMatrixIdentity();
	m_matWorldTranslate = XMMatrixIdentity();
	m_matWorld = XMMatrixIdentity();
	return true;
}

int CDx11Transform::Update(float fTime)
{
	// 정적인 객체이면 바로 리턴
	if (m_bStatic)
		return 0;
	return 0;
}

int CDx11Transform::LateUpdate(float fTime)
{
	// 최종적인 행렬과의 연산은 LateUpdate에서 수행

	// 정적인 객체이면 바로 리턴
	// 업데이트 안해도 되도 바로 리턴

	if (m_bStatic)
		return 0;

	else if (!m_bUpdate)
		return 0;

	m_matLocal = m_matLocalScale * m_matLocalRot * m_matLocalTranslate;
	m_matWorld = m_matWorldScale * m_matWorldRot * m_matWorldTranslate;

	return 0;
}

void CDx11Transform::Render(float fTime)
{
	m_bUpdate = false;
}

CDx11Transform * CDx11Transform::Clone()
{
	return new CDx11Transform(*this);
}

Vec3 CDx11Transform::GetLocalScale()
{
	return m_vLocalScale;
}

Vec3 CDx11Transform::GetLocalRot()
{
	return m_vLocalRot;
}

float CDx11Transform::GetLocalRotX()
{
	return m_vLocalRot.x;
}

float CDx11Transform::GetLocalRotY()
{
	return m_vLocalRot.y;
}

float CDx11Transform::GetLocalRotZ()
{
	return m_vLocalRot.z;
}

Vec3 CDx11Transform::GetLocalPos()
{
	return m_vLocalPos;
}

Vec3 CDx11Transform::GetLocalView()
{
	return m_vLocalView;
}

const Vec3 * CDx11Transform::GetLocalAxis()
{
	return m_vLocalAxis;
}

Vec3 CDx11Transform::GetLocalAxis(AXIS axis)
{
	return m_vLocalAxis[axis];
}

Matrix CDx11Transform::GetLocalMatrix()
{
	return m_matLocal;
}

void CDx11Transform::SetLocalScale(const Vec3 & vScale)
{
	Vec3	vSc = vScale - m_vLocalScale;
	ScaleLocal(vSc);
}

void CDx11Transform::SetLocalScale(float x, float y, float z)
{
	Vec3	vScale(x, y, z);
	Vec3	vSc = vScale - m_vLocalScale;
	ScaleLocal(vSc);
}

void CDx11Transform::SetLocalRot(const Vec3 & vRot)
{
	Vec3	vRotate = vRot - m_vLocalRot;
	RotateLocal(vRotate);
}

void CDx11Transform::SetLocalRot(float x, float y, float z)
{
	Vec3	vRot(x, y, z);
	Vec3	vRotate = vRot - m_vLocalRot;
	RotateLocal(vRotate);
}

void CDx11Transform::SetLocalRotX(float x)
{
	Vec3	vRot(x, m_vLocalRot.y, m_vLocalRot.z);
	Vec3	vRotate = vRot - m_vLocalRot;
	RotateLocal(vRotate);
}

void CDx11Transform::SetLocalRotY(float y)
{
	Vec3	vRot(m_vLocalRot.x, y, m_vLocalRot.z);
	Vec3	vRotate = vRot - m_vLocalRot;
	RotateLocal(vRotate);
}

void CDx11Transform::SetLocalRotZ(float z)
{
	Vec3	vRot(m_vLocalRot.x, m_vLocalRot.y, z);
	Vec3	vRotate = vRot - m_vLocalRot;
	RotateLocal(vRotate);
}

void CDx11Transform::SetLocalPos(const Vec3 & vPos)
{
	Vec3 vMove = vPos - m_vLocalPos;
	MoveLocal(vMove);
}

void CDx11Transform::SetLocalPos(float x, float y, float z)
{
	Vec3 vPos(x, y, z);
	Vec3 vMove = vPos - m_vLocalPos;
	MoveLocal(vMove);
}

void CDx11Transform::SetLocalView(const Vec3 & vView)
{
	m_vLocalView = vView;
}

void CDx11Transform::SetLocalView(float x, float y, float z)
{
	m_vLocalView.x = x;
	m_vLocalView.y = y;
	m_vLocalView.z = z;
}

void CDx11Transform::ComputeLocalAxis()
{
	for (int i = 0; i < AXIS_MAX; ++i)
	{
		Vec	v = m_vLocalAxis[i].Convert();
		v = XMVector3TransformNormal(v, m_matLocalRot);
		m_vLocalAxis[i].Convert(v);
		m_vLocalAxis[i] = m_vLocalAxis[i].Normalize();
	}
}

void CDx11Transform::MoveLocal(float fSpeed, float fTime, AXIS axis)
{
	Vec3	vMove =  m_vLocalAxis[axis] * fSpeed * fTime;
	MoveLocal(vMove);
}

void CDx11Transform::MoveLocal(const Vec3& vMove)
{
//	구했던 차이값을 더한다. (차이만큼 이동)
	m_vLocalPos += vMove;
//	최종 위치 = 로컬 위치 + 월드 위치
	m_vWorldFinalPos = m_vLocalPos + m_vWorldPos;

	ComputeRotationPos();

	m_matWorldTranslate = XMMatrixTranslation(m_vWorldFinalPos.x, m_vWorldFinalPos.y,
		m_vWorldFinalPos.z);

//	자신의 차일드 리스트의 포인터를 받아온다.
	list<CDx11GameObject*>* pChildList = m_pGameObject->GetChildList();
	list<CDx11GameObject*>::iterator	iter;
	list<CDx11GameObject*>::iterator	iterEnd = pChildList->end();

	for (iter = pChildList->begin(); iter != iterEnd; ++iter)
	{
		CDx11Transform* pTr = (*iter)->GetTransform();
	//	부모가 이동했으니 자식의 이동함수를 불러준다.
	//	자식이 부모의 이동에 영향을 받는다면 (즉, TPF_POS가 true라면)
	//	바로 위에 작성한 if문이 실행되어 자식의 위치가 갱신될 것이다.
		pTr->MoveLocal(Vec3Zero);
		SAFE_RELEASE(pTr);
	}

	EnableUpdate();
}

void CDx11Transform::MoveLocal(float fSpeed, float fTime, const Vec3 & v)
{
	Vec3	vMove = v * fSpeed * fTime;
	MoveLocal(vMove);
}

void CDx11Transform::ScaleLocal(const Vec3& vScale)
{
	//	구했던 차이값을 더한다. (차이만큼 스케일)
	m_vLocalScale += vScale;
	//	최종 스케일 = 로컬 스케일 * 월드 스케일
	m_vWorldFinalScale = m_vLocalScale * m_vWorldScale;

	//	부모가 존재하고,
	//	부모의 스케일에 영향을 받을 때
	if (m_iParentFlag & TPF_SCALE)
	{
		CDx11GameObject*	pParent = m_pGameObject->GetParent();
		if (pParent)
		{
			CDx11Transform*	pTr = pParent->GetTransform();
			//	자식의 최종 스케일 *= 부모의 최종 스케일
			m_vWorldFinalScale *= pTr->GetWorldScale();

			SAFE_RELEASE(pTr);
			SAFE_RELEASE(pParent);
		}
	}

	m_matWorldScale = XMMatrixScaling(m_vWorldFinalScale.x, m_vWorldFinalScale.y,
		m_vWorldFinalScale.z);

	//	자신의 차일드 리스트의 포인터를 받아온다.
	list<CDx11GameObject*>* pChildList = m_pGameObject->GetChildList();
	list<CDx11GameObject*>::iterator	iter;
	list<CDx11GameObject*>::iterator	iterEnd = pChildList->end();

	for (iter = pChildList->begin(); iter != iterEnd; ++iter)
	{
		CDx11Transform* pTr = (*iter)->GetTransform();
		pTr->ScaleLocal(Vec3Zero);
		SAFE_RELEASE(pTr);
	}

	EnableUpdate();
}

void CDx11Transform::RotateLocal(const Vec3& vRot)
{
	//	구했던 차이값을 더한다. (차이만큼 회전)
	m_vLocalRot += vRot;

	//	부모가 존재하고,
	XMVECTOR q = XMQuaternionRotationRollPitchYaw(m_vLocalRot.x, m_vLocalRot.y, m_vLocalRot.z);
	m_matLocalRot = XMMatrixRotationQuaternion(q);

//	축과 위치 조정
	ComputeLocalAxis();

	//	자신의 차일드 리스트의 포인터를 받아온다.
	list<CDx11GameObject*>* pChildList = m_pGameObject->GetChildList();
	list<CDx11GameObject*>::iterator	iter;
	list<CDx11GameObject*>::iterator	iterEnd = pChildList->end();

	for (iter = pChildList->begin(); iter != iterEnd; ++iter)
	{
		CDx11Transform* pTr = (*iter)->GetTransform();
		//	부모가 이동했으니 자식의 이동함수를 불러준다.
		//	자식이 부모의 이동에 영향을 받는다면 (즉, TPF_POS가 true라면)
		//	바로 위에 작성한 if문이 실행되어 자식의 위치가 갱신될 것이다.
		pTr->RotateLocal(Vec3Zero);
		SAFE_RELEASE(pTr);
	}

	EnableUpdate();
}

void CDx11Transform::RotationLocal(float fAngle, float fTime, AXIS axis)
{
	Vec3	vRot = Vec3Zero;

	switch (axis)
	{
	case AXIS_X:
		vRot.x = fAngle * fTime;
		break;
	case AXIS_Y:
		vRot.y = fAngle * fTime;
		break;
	case AXIS_Z:
		vRot.z = fAngle * fTime;
		break;
	}

	RotateLocal(vRot);
}

void CDx11Transform::RotationLocal(float fAngle, float fTime, const Vec3 & vRot)
{
	Vec3 vRotate = vRot * fAngle * fTime;
	RotateLocal(vRotate);
}

Vec3 CDx11Transform::GetWorldScale()
{
	return m_vWorldFinalScale;
}

Vec3 CDx11Transform::GetWorldRot()
{
	return m_vWorldFinalRot;
}

float CDx11Transform::GetWorldRotX()
{
	return m_vWorldRot.x;
}

float CDx11Transform::GetWorldRotY()
{
	return m_vWorldRot.y;
}

float CDx11Transform::GetWorldRotZ()
{
	return m_vWorldRot.z;
}

Vec3 CDx11Transform::GetWorldPos()
{
	return m_vWorldFinalPos;
}

Vec3 CDx11Transform::GetWorldView()
{
	return m_vWorldView;
}

const Vec3 * CDx11Transform::GetWorldAxis()
{
	return m_vWorldAxis;
}

Vec3 CDx11Transform::GetWorldAxis(AXIS axis)
{
	return m_vWorldAxis[axis];
}

Matrix CDx11Transform::GetWorldMatrix()
{
	return m_matWorld;
}


Matrix CDx11Transform::GetWorldScaleMatrix()
{
	return m_matWorldScale;
}

Matrix CDx11Transform::GetWorldRotMatrix()
{
	return m_matWorldRot;
}

Matrix CDx11Transform::GetWorldTranslateMatrix()
{
	return m_matWorldTranslate;
}


void CDx11Transform::SetWorldScale(const Vec3 & vScale)
{
	Vec3 vSc = vScale - m_vWorldScale;
	ScaleWorld(vSc);
}

void CDx11Transform::SetWorldScale(float x, float y, float z)
{
	Vec3 vScale(x, y, z);
	Vec3 vSc = vScale - m_vWorldScale;
	ScaleWorld(vSc);
}

void CDx11Transform::SetWorldRot(const Vec3 & vRot)
{
	Vec3 vRotation = vRot - m_vWorldRot;
	RotateWorld(vRotation);
}

void CDx11Transform::SetWorldRot(float x, float y, float z)
{
	Vec3 vRot(x, y, z);
	Vec3 vRotation = vRot - m_vWorldRot;
	if (vRotation.x == 0 && vRotation.y == 0 && vRotation.z == 0)
		return;

	RotateWorld(vRotation);
}

void CDx11Transform::SetWorldRotX(float x)
{
	Vec3 vRot(x, m_vWorldRot.y, m_vWorldRot.z);
	Vec3 vRotation = vRot - m_vWorldRot;
	RotateWorld(vRotation);
}

void CDx11Transform::SetWorldRotY(float y)
{
	Vec3 vRot(m_vWorldRot.x, y, m_vWorldRot.z);
	Vec3 vRotation = vRot - m_vWorldRot;
	RotateWorld(vRotation);
}

void CDx11Transform::SetWorldRotZ(float z)
{
	Vec3 vRot(m_vWorldRot.x, m_vWorldRot.y, z);
	Vec3 vRotation = vRot - m_vWorldRot;
	RotateWorld(vRotation);
}

void CDx11Transform::SetWorldRotMatrix(const Matrix & matRot)
{
	m_matWorldRot = matRot;
	m_matWorld = m_matWorldScale * m_matWorldRot * m_matWorldTranslate;
}

void CDx11Transform::SetWorldPos(const Vec3 & vPos)
{
	Vec3 vMove = vPos - m_vWorldPos;
	if (vMove.x == 0 && vMove.y == 0 && vMove.z == 0)
		return;

	MoveWorld(vMove);
}

void CDx11Transform::SetWorldPos(float x, float y, float z)
{
	Vec3 vPos(x, y, z);
	Vec3 vMove = vPos - m_vWorldPos;
	MoveWorld(vMove);
}

void CDx11Transform::SetWorldView(const Vec3 & vView)
{
	m_vWorldView = vView;
}

void CDx11Transform::SetWorldView(float x, float y, float z)
{
	m_vWorldView.x = x;
	m_vWorldView.y = y;
	m_vWorldView.z = z;
}

void CDx11Transform::ComputeWorldAxis()
{
	Vec3	vAxis[AXIS_MAX] = { Vec3X, Vec3Y, Vec3Z };

	for (int i = 0; i < AXIS_MAX; ++i)
	{
		Vec	v = vAxis[i].Convert();
		v = XMVector3TransformNormal(v, m_matWorldRot);
		m_vWorldAxis[i].Convert(v);
		m_vWorldAxis[i] = m_vWorldAxis[i].Normalize();
	}
}

void CDx11Transform::MoveWorld(float fSpeed, float fTime, AXIS axis)
{
	Vec3	vMove = m_vWorldAxis[axis] * fSpeed * fTime;
	MoveWorld(vMove);

}

void CDx11Transform::MoveWorld(float fSpeed, float fTime)
{
	Vec3	vMove = m_vWorldView * fSpeed * fTime;
	MoveWorld(vMove);
}

void CDx11Transform::MoveWorld(float fSpeed, float fTime, const Vec3 & v)
{
	Vec3	vMove = v * fSpeed * fTime;
	MoveWorld(vMove);
}

void CDx11Transform::RotationWorld(float fAngle, float fTime, AXIS axis)
{
	Vec3	vRot = Vec3Zero;

	switch (axis)
	{
	case AXIS_X:
		vRot.x = fAngle * fTime;
		break;
	case AXIS_Y:
		vRot.y = fAngle * fTime;
		break;
	case AXIS_Z:
		vRot.z = fAngle * fTime;
		break;
	}

	RotateWorld(vRot);
}

void CDx11Transform::RotationWorld(float fAngle, float fTime, const Vec3 & vRot)
{
	Vec3 vRotate = vRot * fAngle * fTime;
	RotateWorld(vRotate);
}


void CDx11Transform::ScaleWorld(const Vec3 & vScale)
{
	m_vWorldScale += vScale;
	m_vWorldFinalScale = m_vLocalScale * m_vWorldScale;

	if (m_iParentFlag & TPF_SCALE)
	{
		CDx11GameObject*	pParent = m_pGameObject->GetParent();
		if (pParent)
		{
			CDx11Transform*	pTr = pParent->GetTransform();
			m_vWorldFinalScale += pTr->GetWorldScale();
			SAFE_RELEASE(pTr);
			SAFE_RELEASE(pParent);
		}
	}
	
	m_matWorldScale = XMMatrixScaling(m_vWorldFinalScale.x, m_vWorldFinalScale.y,
		m_vWorldFinalScale.z);

	list<CDx11GameObject*>*	pChildList = m_pGameObject->GetChildList();
	list<CDx11GameObject*>::iterator	iter;
	list<CDx11GameObject*>::iterator	iterEnd = pChildList->end();

	for (iter = pChildList->begin(); iter != iterEnd; ++iter)
	{
		CDx11Transform*	pTr = (*iter)->GetTransform();

		pTr->ScaleWorld(Vec3Zero);

		SAFE_RELEASE(pTr);
	}

	EnableUpdate();
}

void CDx11Transform::MoveWorld(const Vec3 & vMove)
{
	m_vWorldPos += vMove;
	m_vWorldFinalPos = m_vLocalPos + m_vWorldPos;

//	부모가 있을 때는 이 함수에서 TransformCoord 함수를 돌려준다.
//	자식은 자신의 절대적인 위치 FinalPos를 갱신받는다.
//	ComputeRotationPos();
	CDx11GameObject*	pParent = m_pGameObject->GetParent();
	if (pParent)
	{
		CDx11Transform*	pTr = pParent->GetTransform();

		Vec3	vDist = m_vLocalPos + m_vWorldPos;
		Matrix	matParent = pTr->GetWorldTranslateMatrix();
		Vec		v = XMVector3TransformCoord(vDist.Convert(), matParent);
		m_vWorldFinalPos.Convert(v);

		SAFE_RELEASE(pTr);
		SAFE_RELEASE(pParent);
	}

	m_matWorldTranslate = XMMatrixTranslation(m_vWorldFinalPos.x, m_vWorldFinalPos.y,
		m_vWorldFinalPos.z);
	
	

	list<CDx11GameObject*>*	pChildList = m_pGameObject->GetChildList();
	list<CDx11GameObject*>::iterator	iter;
	list<CDx11GameObject*>::iterator	iterEnd = pChildList->end();

	for (iter = pChildList->begin(); iter != iterEnd; ++iter)
	{
		CDx11Transform*	pTr = (*iter)->GetTransform();

		pTr->MoveWorld(Vec3Zero);

		SAFE_RELEASE(pTr);
	}

	EnableUpdate();
}

void CDx11Transform::RotateWorld(const Vec3& vRot)
{
	m_vWorldRot += vRot;
	m_vWorldFinalRot = m_vLocalRot + m_vWorldRot;

	if (m_iParentFlag & TPF_ROT)
	{
		CDx11GameObject*	pParent = m_pGameObject->GetParent();
		if (pParent)
		{
			CDx11Transform*	pTr = pParent->GetTransform();
			m_vWorldFinalRot += pTr->GetWorldRot();	
			SAFE_RELEASE(pTr);
			SAFE_RELEASE(pParent);
		}
	}
	
	m_vQuarternion = XMQuaternionRotationRollPitchYaw(m_vWorldFinalRot.x, m_vWorldFinalRot.y, m_vWorldFinalRot.z);
	m_matWorldRot = XMMatrixRotationQuaternion(m_vQuarternion);
	
//	ComputeLocalAxis();
	ComputeWorldAxis();
	ComputeRotationPos();
	
	// 최종적으로 로컬 회전 행렬을 곱해주어서 
	// 로컬과 월드의 축을 각각 유지시키면서
	// 로컬의 회전을 월드에 포함시켜준다.
	m_matWorldRot = m_matLocalRot * m_matWorldRot;

	list<CDx11GameObject*>*	pChildList = m_pGameObject->GetChildList();
	list<CDx11GameObject*>::iterator	iter;
	list<CDx11GameObject*>::iterator	iterEnd = pChildList->end();

	for (iter = pChildList->begin(); iter != iterEnd; ++iter)
	{
		CDx11Transform*	pTr = (*iter)->GetTransform();

		pTr->RotateWorld(Vec3Zero);

		SAFE_RELEASE(pTr);
	}

	EnableUpdate();
}

void CDx11Transform::ComputeRotationPos()
{
	CDx11GameObject*	pParent = m_pGameObject->GetParent();
	if (pParent) 
	{
		CDx11Transform*	pTr = pParent->GetTransform();

		Vec3	vDist = m_vLocalPos + m_vWorldPos;
		Vec		v;
		
		if (m_eRotFlag != RF_BILLBOARD)
		{
			Matrix	matParent = pTr->GetWorldMatrix();
			v = XMVector3TransformCoord(vDist.Convert(), matParent);
		}
		else
		{
			Matrix	matParent = pTr->GetWorldTranslateMatrix();
			v = XMVector3TransformCoord(vDist.Convert(), matParent);
		}

		m_vWorldFinalPos.Convert(v);

	//	이동 행렬을 갱신한다.
		m_matWorldTranslate = XMMatrixTranslation(m_vWorldFinalPos.x, m_vWorldFinalPos.y, m_vWorldFinalPos.z);

		SAFE_RELEASE(pTr);
		SAFE_RELEASE(pParent);
	}
}

void CDx11Transform::SetQuarternion(const XMVECTOR & vQuarternion)
{
	m_vQuarternion = vQuarternion;
}

void CDx11Transform::SetWorldAxis(const Vec3 & vAxis, AXIS axis)
{
	m_vWorldAxis[axis] = vAxis;
}

void CDx11Transform::SetRotationFlag(ROTATION_FLAG eFlag)
{
	m_eRotFlag = eFlag;
}



void CDx11Transform::Static(bool bStatic)
{
	m_bStatic = bStatic;
}

void CDx11Transform::ClearParentFlag()
{
	m_iParentFlag = TPF_NONE;
}

void CDx11Transform::SetParentFlag(int iFlag)
{
	m_iParentFlag = iFlag;
}

void CDx11Transform::AddParentFlag(int iFlag)
{
//	OR연산으로 해당 플래그 추가해줌
	m_iParentFlag |= iFlag;
}

void CDx11Transform::DeleteParentFlag(int iFlag)
{
// XOR연산으로 해당 플래그 제거해줌
	m_iParentFlag ^= iFlag;
}


void CDx11Transform::EnableUpdate()
{
	m_bUpdate = true;

}

bool CDx11Transform::GetStatic()
{
	return m_bStatic;
}

void CDx11Transform::Copy(CDx11Transform * pTransform)
{
//	현재는 불렛이 Copy함수를 호출한다.
//	인자로 미사일을 발사한 객체의 트랜스폼을 받아온다.

//	총알 자신이 속한 씬, 레이어, 게임오브젝트를 저장
	CDx11Scene*	pScene = m_pScene;
	CDx11Layer* pLayer = m_pLayer;
	CDx11GameObject*	pObj = m_pGameObject;

//	얕은 복사
//	플레이어 or 차일드 객체의 트랜스폼의 정보로 복사함
//	*this = *pTransform;

	m_vLocalRot = pTransform->GetLocalRot();
	m_vLocalAxis[AXIS_X] = pTransform->GetLocalAxis(AXIS_X);
	m_vLocalAxis[AXIS_Y] = pTransform->GetLocalAxis(AXIS_Y);
	m_vLocalAxis[AXIS_Z] = pTransform->GetLocalAxis(AXIS_Z);

	m_matLocal = pTransform->GetLocalMatrix();



	m_vWorldScale = pTransform->GetWorldScale();
	m_vWorldFinalScale = pTransform->GetWorldPos();
	m_vWorldRot = pTransform->GetWorldRot();
	m_vWorldFinalRot = pTransform->GetWorldRot();
	m_vWorldPos = pTransform->GetWorldPos();
	m_vWorldFinalPos = pTransform->GetWorldPos();
	m_vWorldMove = Vec3Zero;
	m_vWorldAxis[AXIS_X] = pTransform->GetWorldAxis(AXIS_X);
	m_vWorldAxis[AXIS_Y] = pTransform->GetWorldAxis(AXIS_Y);
	m_vWorldAxis[AXIS_Z] = pTransform->GetWorldAxis(AXIS_Z);
	m_vWorldView = pTransform->GetWorldView();

	m_matWorldScale = pTransform->GetWorldScaleMatrix();
	m_matWorldRot = pTransform->GetWorldRotMatrix();
	m_matWorldTranslate = pTransform->GetWorldTranslateMatrix();
	m_matWorld = pTransform->GetWorldMatrix();


//	저장해놓았던 자신의 정보로 다시 바꿔줌.
	m_pTransform = this;
	m_pGameObject = pObj;
	m_pLayer = pLayer;
	m_pScene = pScene;
}
