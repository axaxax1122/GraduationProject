#include "MinimapMove.h"
#include "GameObject/Dx11GameObject.h"
#include "Component/Dx11Transform.h"
#include "Scene/Dx11Layer.h"
#include "Scene/Dx11Scene.h"
#include<iostream>
using namespace std;

CMinimapMove::CMinimapMove()
{
}

CMinimapMove::~CMinimapMove()
{
	//FreeConsole();
}

CMinimapMove::CMinimapMove(const CMinimapMove & MO)
{
}

bool CMinimapMove::Init()
{
	//AllocConsole();
	//freopen("CONOUT$", "wt", stdout);

	return true;
}

void CMinimapMove::Input(float fTime)
{
}

int CMinimapMove::Update(float fTime)
{
	//-------------------------------------- 캐릭터 -------------------------------------------
	if (m_pGameObject->GetTag() == "MinimapPlayer") {
		CDx11GameObject*	pPlayerObj = m_pScene->FindGameObjectFromTag("PlayerObject");
		if (pPlayerObj) {
			CDx11Transform*		pPlayerTr = pPlayerObj->GetTransform();

			float MapPlayersize = 5.f;
			Vec3 PlayerPos = pPlayerTr->GetWorldPos();
			m_pTransform->SetWorldPos(Vec3((PlayerPos.x / 2.f + (1280 - 103) - MapPlayersize), (-PlayerPos.z / 2.f + (103) - MapPlayersize), 0));

			SAFE_RELEASE(pPlayerTr);
		}
		SAFE_RELEASE(pPlayerObj);
	}
	//-------------------------------------- 캐릭터2 -------------------------------------------
	if (m_pGameObject->GetTag() == "MinimapPlayer2") {
		CDx11GameObject*	pPlayerObj = m_pScene->FindGameObjectFromTag("UserObject");
		if (pPlayerObj) {
			CDx11Transform*		pPlayerTr = pPlayerObj->GetTransform();

			float MapPlayersize = 5.f;
			Vec3 PlayerPos = pPlayerTr->GetWorldPos();
			m_pTransform->SetWorldPos(Vec3((PlayerPos.x / 2.f + (1280 - 103) - MapPlayersize), (-PlayerPos.z / 2.f + (103) - MapPlayersize), 0));

			SAFE_RELEASE(pPlayerTr);
		}
		SAFE_RELEASE(pPlayerObj);
	}
	//---------------------------------------- jeep -----------------------------------------
	if (m_pGameObject->GetTag() == "Minimapjeep") {
		CDx11GameObject*	pJeepObj = m_pScene->FindGameObjectFromTag("jeep");
		if (pJeepObj) {
			CDx11Transform*		pJeepObjTr = pJeepObj->GetTransform();

			float MapPlayersize = 5.f;
			Vec3 JeepPos = pJeepObjTr->GetWorldPos();
			m_pTransform->SetWorldPos(Vec3((JeepPos.x / 2.f + (1280 - 103) - MapPlayersize), (-JeepPos.z / 2.f + (103) - MapPlayersize), 0));

			SAFE_RELEASE(pJeepObjTr);
		}
		SAFE_RELEASE(pJeepObj);
	}
	//---------------------------------------- HotDogTruck(2) --------------------------------------
	if (m_pGameObject->GetTag() == "MinimapHotDogTruck(2)") {
		CDx11GameObject*	pJeepObj = m_pScene->FindGameObjectFromTag("HotDogTruck(2)");
		if (pJeepObj) {
			CDx11Transform*		pJeepObjTr = pJeepObj->GetTransform();

			float MapPlayersize = 5.f;
			Vec3 JeepPos = pJeepObjTr->GetWorldPos();
			m_pTransform->SetWorldPos(Vec3((JeepPos.x / 2.f + (1280 - 103) - MapPlayersize), (-JeepPos.z / 2.f + (103) - MapPlayersize), 0));

			SAFE_RELEASE(pJeepObjTr);
		}
		SAFE_RELEASE(pJeepObj);
	}
	//---------------------------------------- CC_ME_R4(2) -----------------------------------------
	if (m_pGameObject->GetTag() == "MinimapCC_ME_R4(2)") {
		CDx11GameObject*	pJeepObj = m_pScene->FindGameObjectFromTag("CC_ME_R4(2)");
		if (pJeepObj) {
			CDx11Transform*		pJeepObjTr = pJeepObj->GetTransform();

			float MapPlayersize = 5.f;
			Vec3 JeepPos = pJeepObjTr->GetWorldPos();
			m_pTransform->SetWorldPos(Vec3((JeepPos.x / 2.f + (1280 - 103) - MapPlayersize), (-JeepPos.z / 2.f + (103) - MapPlayersize), 0));

			SAFE_RELEASE(pJeepObjTr);
		}
		SAFE_RELEASE(pJeepObj);
	}
	//---------------------------------------- HotDogTruck -----------------------------------------
	if (m_pGameObject->GetTag() == "MinimapHotDogTruck") {
		CDx11GameObject*	pJeepObj = m_pScene->FindGameObjectFromTag("HotDogTruck(1)");
		if (pJeepObj) {
			CDx11Transform*		pJeepObjTr = pJeepObj->GetTransform();

			float MapPlayersize = 5.f;
			Vec3 JeepPos = pJeepObjTr->GetWorldPos();
			m_pTransform->SetWorldPos(Vec3((JeepPos.x / 2.f + (1280 - 103) - MapPlayersize), (-JeepPos.z / 2.f + (103) - MapPlayersize), 0));

			SAFE_RELEASE(pJeepObjTr);
		}
		SAFE_RELEASE(pJeepObj);
	}

	return 0;
}

int CMinimapMove::LateUpdate(float fTime)
{
	return 0;
}

void CMinimapMove::Render(float fTime)
{
}

CMinimapMove * CMinimapMove::Clone()
{
	return nullptr;
}
