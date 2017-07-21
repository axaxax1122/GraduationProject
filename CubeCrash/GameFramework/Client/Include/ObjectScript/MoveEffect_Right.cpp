#include "MoveEffect_Right.h"
#include "GameObject/Dx11GameObject.h"
#include "Component/Dx11Transform.h"
#include "Scene/Dx11Layer.h"
#include "Scene/Dx11Scene.h"


CMoveEffect_Right::CMoveEffect_Right()
{
}


CMoveEffect_Right::~CMoveEffect_Right()
{
}

CMoveEffect_Right::CMoveEffect_Right(const CMoveEffect_Right & MO)
{
}

bool CMoveEffect_Right::Init()
{


	return true;
}

void CMoveEffect_Right::Input(float fTime)
{
}

int CMoveEffect_Right::Update(float fTime)
{

	CDx11GameObject*	pObj = m_pScene->FindGameObjectFromTag(m_Name);
	if (pObj) {
		CDx11Transform*		pObjTr = pObj->GetTransform();
		Vec3	vPos = pObjTr->GetWorldPos();
		m_pTransform->SetWorldPos(vPos.x - 10.f, vPos.y - 4.f, vPos.z - 1.f);
		SAFE_RELEASE(pObjTr);
	}
	SAFE_RELEASE(pObj);

	return 0;
}

int CMoveEffect_Right::LateUpdate(float fTime)
{

	return 0;
}

void CMoveEffect_Right::Render(float fTime)
{
}

CMoveEffect_Right * CMoveEffect_Right::Clone()
{
	return nullptr;
}
