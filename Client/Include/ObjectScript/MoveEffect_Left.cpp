#include "MoveEffect_Left.h"
#include "GameObject/Dx11GameObject.h"
#include "Component/Dx11Transform.h"
#include "Scene/Dx11Layer.h"
#include "Scene/Dx11Scene.h"



CMoveEffect_Left::CMoveEffect_Left()
{
}


CMoveEffect_Left::~CMoveEffect_Left()
{
}

CMoveEffect_Left::CMoveEffect_Left(const CMoveEffect_Left & MO)
{
}

bool CMoveEffect_Left::Init()
{
	return true;
}

void CMoveEffect_Left::Input(float fTime)
{
}

int CMoveEffect_Left::Update(float fTime)
{

	CDx11GameObject*	pObj = m_pScene->FindGameObjectFromTag(m_Name);
	if (pObj) {
		CDx11Transform*		pObjTr = pObj->GetTransform();
		Vec3	vPos = pObjTr->GetWorldPos();
		m_pTransform->SetWorldPos(vPos.x + 11.f, vPos.y - 3.75f, vPos.z - 1.f);
		SAFE_RELEASE(pObjTr);
	}
	SAFE_RELEASE(pObj);

	return 0;
}

int CMoveEffect_Left::LateUpdate(float fTime)
{

	return 0;
}

void CMoveEffect_Left::Render(float fTime)
{
}

CMoveEffect_Left * CMoveEffect_Left::Clone()
{
	return nullptr;
}
