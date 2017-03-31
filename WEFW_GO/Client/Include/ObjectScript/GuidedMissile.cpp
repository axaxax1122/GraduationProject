#include "GuidedMissile.h"
#include "GameObject/Dx11GameObject.h"
#include "Component/Dx11Transform.h"
#include "Scene/Dx11Scene.h"
#include "Scene/Dx11Layer.h"
#include "Player.h"
#include "Component\Dx11Collider.h"

CGuidedMissile::CGuidedMissile()	:
	m_fSpeed(0),
	m_fDist(0),
	m_vRot(Vec3Zero),
	m_pMonster(NULL)
{
}

CGuidedMissile::CGuidedMissile(const CGuidedMissile & missile)	:
	CDx11Script(missile)
{
	*this = missile;
	m_iRefCount = 1;

	if (m_pMonster)
		m_pMonster->AddRef();
}

CGuidedMissile::~CGuidedMissile()
{
	SAFE_RELEASE(m_pMonster);
}


void CGuidedMissile::SetRotationAxis(const Vec3& vRot)
{
	m_vRot = vRot;
}


void CGuidedMissile::SetDirection(const Vec3 & vDir)
{
	m_vDir = vDir;
}

void CGuidedMissile::SetTarget(CDx11GameObject * pTarget)
{
	m_pMonster = pTarget;
}

void CGuidedMissile::DeathMotion()
{
	// 이펙트 생성
	CDx11GameObject*	pEffectObj = CDx11GameObject::Create("SmallEffect", OT_CLONE);
	CDx11Transform*		pEffectTr = pEffectObj->GetTransform();
	Vec3	vPos = m_pTransform->GetWorldPos();

	pEffectTr->SetWorldPos(vPos);
	m_pLayer->AddObject(pEffectObj);

	SAFE_RELEASE(pEffectTr);
	SAFE_RELEASE(pEffectObj);
}

bool CGuidedMissile::Init()
{
	m_fSpeed = 1.2f;
	m_fDist = 15.f;
	m_vDir = Vec3Y;
	m_vRot = Vec3Zero;
	return true;
}

void CGuidedMissile::Input(float fTime)
{
}

int CGuidedMissile::Update(float fTime)
{
	//CDx11GameObject*	pMonsterObj = FindTarget();
	//if (!pMonsterObj) 
	//	return 0;
	if (!m_pMonster) 
	{
		m_pMonster = FindTarget();
		if (!m_pMonster)
			return 0;
	}
	CDx11Transform*		pMonsterTr = m_pMonster->GetTransform();
	if (m_fDist <= 14.f)
	{
		m_vDir = pMonsterTr->GetWorldPos() - m_pTransform->GetWorldPos();

		SetDirection(m_vDir.Normalize());

		// m_vDir과 미사일의 Y축을 내적
		float fDot = m_vDir.Dot(m_pTransform->GetWorldAxis(AXIS_Y));
		float fAngle = acos(fDot);

		// 미사일의 Y축과 m_vDir을 외적해서 회전 축을 구한다.
		Vec3 vRotation = m_pTransform->GetWorldAxis(AXIS_Y).Cross(m_vDir);
		vRotation = vRotation.Normalize();
		m_pTransform->RotationWorld(fAngle, fTime * 6, vRotation);
		m_pTransform->SetWorldView(m_pTransform->GetWorldAxis(AXIS_Y));

	}
	
	float fLength = (pMonsterTr->GetWorldPos() - m_pTransform->GetWorldPos()).Length();
	if (fLength > 0.3f)
	{
		m_pTransform->MoveWorld(m_fSpeed, fTime);
		m_fDist -= (m_fSpeed * fTime);
	}
	if(m_fSpeed < 10.f )
		m_fSpeed += 0.01f;

	if (m_fDist <= 0.f || fLength < 0.3f)
		m_pGameObject->Death();
	

	SAFE_RELEASE(pMonsterTr);
//	SAFE_RELEASE(pMonsterObj);

	return 0;
}

int CGuidedMissile::LateUpdate(float fTime)
{
	return 0;
}

void CGuidedMissile::Render(float fTime)
{
}

CGuidedMissile * CGuidedMissile::Clone()
{
	return new CGuidedMissile(*this);
}


CDx11GameObject * CGuidedMissile::FindTarget()
{
	//	모든 몬스터의 리스트를 포인터로 받아온다.
	//	각 몬스터와의 거리를 측정한 후 제일 가까운 몬스터를 대상으로 지정함.

	list<CDx11GameObject*>* pMonsterList = m_pScene->FindGameObjectsFromTag("MonsterPrototype");
	list<CDx11GameObject*>::iterator	iter;
	list<CDx11GameObject*>::iterator	iterEnd = pMonsterList->end();

	CDx11GameObject*	pMonster = NULL;
	float fDistance = 100.f;

	for (iter = pMonsterList->begin(); iter != iterEnd; ++iter)
	{
		CDx11Transform* pMonsterTr = (*iter)->GetTransform();

		Vec3 vMonsterPos = pMonsterTr->GetWorldPos();
		Vec3 vDist = vMonsterPos - m_pTransform->GetWorldPos();
		if (fDistance > vDist.Length())
		{
			fDistance = vDist.Length();
			pMonster = (*iter);
		}
		SAFE_RELEASE(pMonsterTr);
	}

	if (!pMonster)
		return NULL;

	pMonster->AddRef();
	return pMonster;
}

