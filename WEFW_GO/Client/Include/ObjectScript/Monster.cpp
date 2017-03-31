#include "Monster.h"
#include "GameObject/Dx11GameObject.h"
#include "Component/Dx11Transform.h"
#include "Player.h"
#include "Scene/Dx11Layer.h"
#include "Scene/Dx11Scene.h"
#include "Component/Dx11Collider.h"
#include "Core/Dx11Debug.h"
#include "Component/Dx11Renderer.h"
#include "Component/Dx11Animation2D.h"
#include "Component/Dx11BillBoard.h"
#include "GuidedMissile.h"


CMonster::CMonster()	:
	m_fSpeed(0),
	m_fDist(0),
	m_vDir(Vec3Y),
	m_fShotDelay(0),
	m_iHP(10)
{
}

CMonster::CMonster(const CMonster & monster)	:
	CDx11Script(monster)
{
	*this = monster;
	m_iRefCount = 1;
	//m_fSpeed = monster.m_fSpeed;
	//m_fShotDelay = monster.m_fShotDelay;
	//m_vDir = monster.m_vDir;
	//m_iHP = monster.m_iHP;
	//m_fDist = monster.m_fDist;
}


CMonster::~CMonster()
{
}

void CMonster::SetSpeed(float fSpeed)
{
	m_fSpeed = fSpeed;
}

void CMonster::SetDirection(const Vec3 & v)
{
	m_vDir = v;
}

void CMonster::AddDistance(float fDist)
{
	m_fDist += fDist;
}

bool CMonster::Init()
{
	m_fSpeed = 1.f;
	m_fDist = 2.f;
	m_iHP = 20;

	return true;
}

void CMonster::Input(float fTime)
{
}

int CMonster::Update(float fTime)
{
	//	���Ͱ� �̵��Ѵ�.
	if (m_fDist > 0)
	{
		m_fDist -= (m_fSpeed * fTime);
		m_pTransform->MoveWorld(-m_fSpeed, fTime);
	}
	
	// ���Ͱ� �÷��̾�� �̻����� ���.
	else
	{

		// �÷��̾� ������Ʈ�� ���´�.
		CDx11GameObject*	pPlayerObj = m_pScene->FindGameObjectFromTag("PlayerObject");
		CDx11Transform*		pTr = pPlayerObj->GetTransform();


		// m_vDir�� ���Ѵ�.
		Vec3 vDir = pTr->GetWorldPos() - m_pTransform->GetWorldPos();
		SetDirection(vDir.Normalize());

		// m_vDir�� ������ Y���� ����
		float fDot = m_vDir.Dot(m_pTransform->GetWorldAxis(AXIS_Y));
		float fAngle = acos(fDot);

		if (fAngle > DX11_PI / 180.f || fAngle < -DX11_PI / 180.f)
		{
			// ������ Y��� m_vDir�� �����ؼ� ȸ�� ���� ���Ѵ�.
			Vec3 vRotation = m_pTransform->GetWorldAxis(AXIS_Y).Cross(m_vDir);
			vRotation = vRotation.Normalize();
			m_pTransform->RotationWorld(fAngle, fTime, vRotation);
		}
		
		m_fShotDelay += fTime;
		if (m_fShotDelay >= 2.5f)
		{
			ShootMissile(m_pTransform->GetWorldAxis(AXIS_Y), fTime);
			m_fShotDelay = 0;
		}

		SAFE_RELEASE(pTr);
		SAFE_RELEASE(pPlayerObj);
	}

	

	return 0;
}

int CMonster::LateUpdate(float fTime)
{
	return 0;
}

void CMonster::Render(float fTime)
{
}

CMonster * CMonster::Clone()
{
	return new CMonster(*this);
}


void CMonster::OnCollisionEnter(CDx11Collider * pColl, float fTime)
{
	if (pColl->GetTag() == "LaserColl")
	{
		string str = "Monster : Laser\n";
		char* pStr = (char*)str.c_str();
		DX11_GET_SINGLE(CDx11Debug)->OutputConsole(pStr);
		
		CDx11GameObject*	pObj = pColl->GetGameObject();
		if (m_iHP > 0)
			--m_iHP;
		else
		{
			DeathMotion();
			m_pGameObject->Death();
		}
		pObj->Death();

		SAFE_RELEASE(pObj);
	}

	if (pColl->GetTag() == "MissileColl")
	{
		string str = "Monster : Missile\n";
		char* pStr = (char*)str.c_str();
		DX11_GET_SINGLE(CDx11Debug)->OutputConsole(pStr);
		
		CDx11GameObject*	pObj = pColl->GetGameObject();
		if (m_iHP > 0)
		{
			m_iHP -= 3;
		}
		else
		{
			DeathMotion();
			m_pGameObject->Death();
		}
		CGuidedMissile*	pMissile = (CGuidedMissile*)pObj->FindComponentFromType(CT_SCRIPT);
		pMissile->DeathMotion();
		SAFE_RELEASE(pMissile);
		
		pObj->Death();
		SAFE_RELEASE(pObj);
	}
}

void CMonster::OnCollision(CDx11Collider * pColl, float fTime)
{
}

void CMonster::OnCollisionExit(CDx11Collider * pColl, float fTime)
{
}


void CMonster::ShootMissile(const Vec3 & vView, float fTime)
{
	//	�Ѿ� ��ü ����
	CDx11GameObject*	pBullet = CreateBullet(m_pTransform, vView);
	SAFE_RELEASE(pBullet);
}

void CMonster::DeathMotion()
{
	// ����Ʈ ����
	CDx11GameObject*	pEffectObj = CDx11GameObject::Create("Effect", OT_CLONE);
	CDx11Transform*		pEffectTr = pEffectObj->GetTransform();
	Vec3	vPos = m_pTransform->GetWorldPos();

	pEffectTr->SetWorldPos(vPos);
	m_pLayer->AddObject(pEffectObj);

	SAFE_RELEASE(pEffectTr);
	SAFE_RELEASE(pEffectObj);
}

CDx11GameObject * CMonster::CreateBullet(CDx11Transform * pTransform, const Vec3 & vDir)
{
	CDx11GameObject*	pBullet = CDx11GameObject::Create("BulletPrototype",
		OT_CLONE);

	CDx11Transform*		pBulletTr = pBullet->GetTransform();
	pBulletTr->Copy(pTransform);
	pBulletTr->MoveWorld(0.5f, 1.f, AXIS_Z);
	pBulletTr->SetWorldScale(0.06f, 0.06f, 0.06f);
	pBulletTr->SetWorldView(vDir);

	m_pLayer->AddObject(pBullet);

	SAFE_RELEASE(pBulletTr);
	SAFE_RELEASE(pBullet);

	return pBullet;
}


