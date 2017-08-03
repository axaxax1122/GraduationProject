#include "PlayerCollision.h"
#include "GameObject/Dx11GameObject.h"
#include "Scene/Dx11Layer.h"
#include "Component/Dx11Transform.h"
#include<iostream>

CPlayerCollision::CPlayerCollision() :
	PS(NoColl),
	m_pObj(NULL),
	PlayerY(0),
	slidingVector(0, 0, 0),
	ObjPos(0, 0, 0),
	PlayerPos(0, 0, 0),
	m_vNomalObj(0, 0, 0),
	m_vPlayerNomal(0, 0, 0),
	One(0, 0, 0),
	Two(0, 0, 0),
	NomalOne(0, 0, 0),
	NomalTwo(0, 0, 0)
{
}
CPlayerCollision::~CPlayerCollision()
{
}

CPlayerCollision::CPlayerCollision(const CPlayerCollision & Coll) :
	CDx11Script(Coll)
{
}

void CPlayerCollision::SetPS(PlayerState ps)
{
	PS = ps;
}

void CPlayerCollision::SetPS(PlayerState ps, CDx11GameObject * pObj)
{
	PS = ps;
	m_pObj = pObj;
}

PlayerState CPlayerCollision::GetPS()
{
	return PS;
}

bool CPlayerCollision::Init()
{
	return true;
}
void CPlayerCollision::Input(float fTime)
{
}

int CPlayerCollision::Update(float fTime)
{
	//-----------플레이어 높이 구하기--------------
	if (m_pObj != NULL)
	{
		CDx11Transform*	pTr = m_pObj->GetTransform();
		ObjPos = pTr->GetWorldPos();
		ObjSize = pTr->GetWorldScale();
		ObjHalfSize = ObjSize / 2.f;
		SAFE_RELEASE(pTr);

		PlayerPos = m_pTransform->GetWorldPos();
		PlayerSize = m_pTransform->GetWorldScale();
		PlayerHalfSize = (PlayerSize / 2) * 25.f;
		PlayerY = ObjPos.y + (ObjHalfSize.y) + (PlayerHalfSize.y);//크기에 따라 점프만 바꿔주면된다.
	}

	//----------------슬라이딩 벡터-----------------
	if (PS == BackColl) {
		m_vPlayerNomal = m_pTransform->GetWorldAxis(AXIS_Z).Normalize();// 플레이어 방향의 법선벡터 구하기	

		One = Vec3(ObjPos.x + ObjHalfSize.x, ObjPos.y, ObjPos.z);// 면 법선벡터 구하기
		Two = Vec3(ObjPos.x, ObjPos.y - ObjHalfSize.y, ObjPos.z);

		NomalOne = (One - ObjPos).Normalize();
		NomalTwo = (Two - ObjPos).Normalize();
		m_vNomalObj = NomalOne.Cross(NomalTwo);

		slidingVector = m_vPlayerNomal - m_vNomalObj*(m_vPlayerNomal.Dot(m_vNomalObj));
	}
	else if (PS == FrontColl) {
		m_vPlayerNomal = m_pTransform->GetWorldAxis(AXIS_Z).Normalize();// 플레이어 방향의 법선벡터 구하기	

		One = Vec3(ObjPos.x + ObjHalfSize.x, ObjPos.y, ObjPos.z);// 면 법선벡터 구하기
		Two = Vec3(ObjPos.x, ObjPos.y - ObjHalfSize.y, ObjPos.z);

		NomalOne = (One - ObjPos).Normalize();
		NomalTwo = (Two - ObjPos).Normalize();
		m_vNomalObj = NomalTwo.Cross(NomalOne);

		slidingVector = m_vPlayerNomal - m_vNomalObj*(m_vPlayerNomal.Dot(m_vNomalObj));
	}
	else if (PS == LeftColl) {
		m_vPlayerNomal = m_pTransform->GetWorldAxis(AXIS_Z).Normalize();// 플레이어 방향의 법선벡터 구하기	

		One = Vec3(ObjPos.x, ObjPos.y, ObjPos.z + ObjHalfSize.z);// 면 법선벡터 구하기
		Two = Vec3(ObjPos.x, ObjPos.y - ObjHalfSize.y, ObjPos.z);

		NomalOne = (One - ObjPos).Normalize();
		NomalTwo = (Two - ObjPos).Normalize();
		m_vNomalObj = NomalTwo.Cross(NomalOne);

		slidingVector = m_vPlayerNomal - m_vNomalObj*(m_vPlayerNomal.Dot(m_vNomalObj));
	}
	else if (PS == RightColl){
		m_vPlayerNomal = m_pTransform->GetWorldAxis(AXIS_Z).Normalize();// 플레이어 방향의 법선벡터 구하기	

		One = Vec3(ObjPos.x, ObjPos.y, ObjPos.z + ObjHalfSize.z);// 면 법선벡터 구하기
		Two = Vec3(ObjPos.x, ObjPos.y - ObjHalfSize.y, ObjPos.z);

		NomalOne = (One - ObjPos).Normalize();
		NomalTwo = (Two - ObjPos).Normalize();
		m_vNomalObj = NomalOne.Cross(NomalTwo);

		slidingVector = m_vPlayerNomal - m_vNomalObj*(m_vPlayerNomal.Dot(m_vNomalObj));
	}

	//충돌 처리
	if (m_pGameObject->GetStateFlag() == STATE_ROLL) {
		switch (PS)
		{
		case NoColl:

			break;
		case LeftColl:
			m_pTransform->MoveWorld(15.f, fTime, Vec3(0, 0, slidingVector.z));
			m_pTransform->MoveWorld(-30.f, fTime, m_pTransform->GetWorldAxis(AXIS_Z));
			break;
		case RightColl:

			m_pTransform->MoveWorld(15.f, fTime, Vec3(0, 0, slidingVector.z));
			m_pTransform->MoveWorld(-30.f, fTime, m_pTransform->GetWorldAxis(AXIS_Z));
			break;
		case FrontColl:
			m_pTransform->MoveWorld(15.f, fTime, Vec3(slidingVector.x, 0, 0));
			m_pTransform->MoveWorld(-30.f, fTime, m_pTransform->GetWorldAxis(AXIS_Z));
			break;
		case BackColl:
			m_pTransform->MoveWorld(15.f, fTime, Vec3(slidingVector.x, 0, 0));
			m_pTransform->MoveWorld(-30.f, fTime, m_pTransform->GetWorldAxis(AXIS_Z));

			break;
		case DownColl:
			m_pTransform->SetWorldPos(Vec3(PlayerPos.x, PlayerY, PlayerPos.z));
			break;
		case Eege_Back_LeftColl:
			m_pTransform->MoveWorld(Vec3(0, 0, -0.7));
			m_pTransform->MoveWorld(Vec3(-0.7, 0, 0));
			break;
		case Eege_Back_RightColl:
			m_pTransform->MoveWorld(Vec3(0, 0, -0.7));
			m_pTransform->MoveWorld(Vec3(0.7, 0, 0));
			break;
		case Eege_Front_LeftColl:
			m_pTransform->MoveWorld(Vec3(0, 0, 0.7));
			m_pTransform->MoveWorld(Vec3(-0.7, 0, 0));
			break;
		case Eege_Front_RightColl:
			m_pTransform->MoveWorld(Vec3(0, 0, 0.7));
			m_pTransform->MoveWorld(Vec3(0.7, 0, 0));
			break;
		default:
			break;
		}
	}
	else {
		switch (PS)
		{
		case NoColl:

			break;
		case LeftColl:
			m_pTransform->MoveWorld(15.f, fTime, Vec3(0, 0, slidingVector.z));
			m_pTransform->MoveWorld(-15.f, fTime, m_pTransform->GetWorldAxis(AXIS_Z));
			break;
		case RightColl:
			m_pTransform->MoveWorld(15.f, fTime, Vec3(0, 0, slidingVector.z));
			m_pTransform->MoveWorld(-15.f, fTime, m_pTransform->GetWorldAxis(AXIS_Z));
			break;
		case FrontColl:
			m_pTransform->MoveWorld(15.f, fTime, Vec3(slidingVector.x, 0, 0));
			m_pTransform->MoveWorld(-15.f, fTime, m_pTransform->GetWorldAxis(AXIS_Z));
			break;
		case BackColl:
			m_pTransform->MoveWorld(15.f, fTime, Vec3(slidingVector.x, 0, 0));
			m_pTransform->MoveWorld(-15.f, fTime, m_pTransform->GetWorldAxis(AXIS_Z));
			break;
		case DownColl:
			m_pTransform->SetWorldPos(Vec3(PlayerPos.x, PlayerY, PlayerPos.z));
			break;
		case Eege_Back_LeftColl:
			m_pTransform->MoveWorld(Vec3(0, 0, -0.325));
			m_pTransform->MoveWorld(Vec3(-0.325, 0, 0));
			break;
		case Eege_Back_RightColl:
			m_pTransform->MoveWorld(Vec3(0, 0, -0.325));
			m_pTransform->MoveWorld(Vec3(0.325, 0, 0));
			break;
		case Eege_Front_LeftColl:
			m_pTransform->MoveWorld(Vec3(0, 0, 0.325));
			m_pTransform->MoveWorld(Vec3(-0.325, 0, 0));
			break;
		case Eege_Front_RightColl:
			m_pTransform->MoveWorld(Vec3(0, 0, 0.325));
			m_pTransform->MoveWorld(Vec3(0.325, 0, 0));
			break;
		default:
			break;
		}
	}

	//중요!
	m_pObj = NULL;
	return 0;
}

int CPlayerCollision::LateUpdate(float fTime)
{
	return 0;
}

void CPlayerCollision::Render(float fTime)
{
}

CPlayerCollision * CPlayerCollision::Clone()
{
	return nullptr;
}

void CPlayerCollision::OnCollisionEnter(CDx11Collider * pColl, float fTime)
{
}

void CPlayerCollision::OnCollision(CDx11Collider * pColl, float fTime)
{
}

void CPlayerCollision::OnCollisionExit(CDx11Collider * pColl, float fTime)
{
}
