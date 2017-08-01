
#include "InGameNetwork.h"
#include "NetworkManager.h"
#include "UserSession.h"
#include "ZEROStream.h"
#include "Dx11Engine.h"
#include "Scene/Dx11SceneManager.h"
#include "Scene/Dx11Scene.h"
#include "Scene/Dx11Layer.h"
#include "GameObject/Dx11GameObject.h"
#include "Component/Dx11Transform.h"
#include "Component/Dx11Renderer.h"
#include "Component/Dx11Material.h"
#include "../ObjectScript/User.h"
#include "Component/Dx11ColliderSphere.h"
#include "Resources/Dx11ResourcesManager.h"
#include "../ObjectScript/Jump.h"
#include "../ObjectScript/Player.h"
#include "../ObjectScript/PlayerCollision.h"
#include "Component/Dx11ColliderOBB.h"
#include "Component/Dx11Camera.h"
#include "../ObjectScript/Player.h"
#include "../ObjectScript/User.h"
#include "Component/Dx11FSM.h"

DX11_USING

bool UpdateFlag = false;
extern HANDLE logoEvent;
bool moveFlag = false;
bool objectFlag = false;

CInGameNetwork::CInGameNetwork()	:
	m_pSession(NULL)
{

}

CInGameNetwork::~CInGameNetwork()
{
}

void CInGameNetwork::SetSession(CUserSession * pSession)
{
	m_pSession = pSession;
}

void CInGameNetwork::SetMainScene(CMainScene * pMainScene)
{
	m_pMainScene = pMainScene;
}


VOID CInGameNetwork::OnIoRead(VOID)
{
	if (m_pSession->ReadPacketForEventSelect())
	{
		DWORD	dwHeader = 0;
		BYTE	Packet[PACKET_SIZE] = {};
		DWORD	dwLength;

		if (!dwHeader == PP_LOGIN_SUCC)
			WaitForSingleObject(logoEvent, INFINITE);


		while (m_pSession->GetPacket(dwHeader, Packet, dwLength))
		{
	
			switch (dwHeader)
			{
			case PP_CHARACTER_INIT:
			{

				CDx11GameObject*	pTerrainObj = CDx11GameObject::Create("Terrain");
				CDx11Terrain*		pTerrain = pTerrainObj->AddComponent<CDx11Terrain>("Terrain");
				CDx11Transform*		pTerrainTr = pTerrainObj->GetTransform();
				pTerrainTr->SetWorldPos(-65.f, 0.f, -65.f);
				pTerrain->CreataTerrain("TerrainMesh", 129, 129, "TerrainDiffuse",
					L"Terrain\\RM_Ground_stone_D.png");
				SAFE_RELEASE(pTerrainTr);
				SAFE_RELEASE(pTerrainObj);

				m_pMainScene->InitPlayer(Packet, pTerrain);
				SAFE_RELEASE(pTerrain);
				GET_SINGLE(CNetworkManager)->TimeStart();
				
			}
				break;
			case PP_LOGIN_SUCC:
			{
				INT64		iSerialNumber;
				memcpy(&iSerialNumber, Packet, 8);

				m_pSession->SetSerialNumber(iSerialNumber);
				int a = 10;
			}
				break;
			case PP_DISCONNECT:
				break;
			case PP_USER_LOGIN:
				CreateUser(Packet, dwLength, m_pMainScene);
				//GET_SINGLE(CNetworkManager)->TimeStart();
				break;
			case PP_USER_ALLREADY:
				break;
			case PP_LOGIN_FAILED:
				break;
			case PP_PLAYER_GAMESTART:
				moveFlag = true;
			//	objectFlag = true;
				break;
			case PP_CHARACTER_MOVE_SUCC:
				m_pMainScene->MovePlayer(Packet);
				break;
			case PP_OTHERPLAYER_MOVE:
				m_pMainScene->MoveOtherPlayer(Packet);
				break;
			case PP_OTHERPLAYER_DISCONNNECT:
				m_pMainScene->DieOtherPlayer(Packet);
				break;
			case PP_CHARACTER_ROTATION_SUCC:
				break;
			case PP_OTHERPLAYER_ROTATION:
				m_pMainScene->RotateOtherPlayer(Packet);
				break;
			case PP_OTHERPLAYER_ANIMSTATE:
				m_pMainScene->AnimOtherPlayer(Packet);
				break;
			case PP_BOX_CREATE:
				m_pMainScene->CreateBox(Packet);
				break;
			case PP_OTHERPLAYER_SHOOT:
				m_pMainScene->ShootOtherPlayer(Packet);
				break;
			case PP_LEFTCAR_MOVE:
				m_pMainScene->LeftCarMove(Packet);
				break;
			case PP_RIGHTCAR_MOVE:
				m_pMainScene->RightCarMove(Packet);
				break;
			case PP_OTHERPLAYER_HEAT:
				m_pMainScene->HeatPlayer(Packet);
			}
		}

	}

}

VOID CInGameNetwork::OnIoConnected(VOID)
{
}

VOID CInGameNetwork::OnIoDisconnected(VOID)
{
}

void CInGameNetwork::CreateUser(BYTE * pPacket, DWORD dwLength, CMainScene* pMainScene)
{
	CZEROStreamSP	stream;

	stream->SetBuffer(pPacket);
	INT64		iSerialNumber;

	stream->ReadInt64(&iSerialNumber);

	Vec3	vPos, vScale, vRot;

	stream->ReadFloat(&vScale.x);
	stream->ReadFloat(&vScale.y);
	stream->ReadFloat(&vScale.z);

	stream->ReadFloat(&vRot.x);
	stream->ReadFloat(&vRot.y);
	stream->ReadFloat(&vRot.z);

	stream->ReadFloat(&vPos.x);
	stream->ReadFloat(&vPos.y);
	stream->ReadFloat(&vPos.z);

	// 유저 객체를 생성해준다.
	CDx11GameObject*	pUser = CDx11GameObject::Create("UserObject");

	CDx11Transform*	pUserTr = pUser->GetTransform();

	pUserTr->SetWorldScale(vScale);
	pUserTr->SetWorldRot(vRot);
	pUserTr->SetWorldPos(Vec3(vPos.x,(vPos.y-1.5f), vPos.z));

	// 렌더러를 생성한다.
	CDx11Renderer*	pRenderer = pUser->AddComponent<CDx11Renderer>("UserRenderer");
	pRenderer->SetMesh("PlayerMesh");
	pRenderer->SetShader(ANIMATION_SHADER);

	// 노말 텍스처 세팅
	CDx11Material*	pMtrl = pRenderer->GetMaterial();
	pMtrl->SetNormalMapTexture("FutureSoldier_01_normal.TGA", L"FutureSoldier_01_normal.TGA", MESH_PATH);

	SAFE_RELEASE(pMtrl);
	SAFE_RELEASE(pRenderer);

	// Finite State Machine
	CDx11FSM*	pFSM = pUser->AddComponent<CDx11FSM>("UserFSM");
	pFSM->AddState("IDLE1", 9.f, 9.967f);
	pFSM->AddState("IDLE2", 10.f, 11.967f);
	pFSM->AddState("RUN", 0.f, 0.967f);
	pFSM->AddState("SPRINT", 4.f, 4.967f);
	pFSM->AddState("JUMP", 14.f, 14.967f);
	pFSM->AddState("LYING", 21.367f, 22.367f);
	pFSM->AddState("SHOOT", 12.f, 12.967f);
	pFSM->AddState("WALK_SHOOT", 5.f, 5.967f);
	pFSM->AddState("JUMP_SHOOT", 13.f, 13.967f);
	pFSM->AddState("LYING_SHOOT", 23.f, 24.f);
	pFSM->AddState("ROLL_FRONT", 18.f, 18.667f);
	pFSM->AddState("ROLL_BACK", 19.f, 19.667f);
	pFSM->AddState("ROLL_LEFT", 19.667f, 20.334f);
	pFSM->AddState("ROLL_RIGHT", 20.667f, 21.334f);
	pFSM->AddState("CRAWLING", 24.333f, 25.f);
	pFSM->AddState("RELOAD", 29.333f, 32.3f);
	pFSM->AddState("DAMAGE", 32.333f, 32.6f);
	pFSM->AddState("DEATH1", 32.667f, 33.3f);
	pFSM->AddState("DEATH2", 33.333f, 34.167f);
	pFSM->SetCurrentState("IDLE1");
	SAFE_RELEASE(pFSM);


	CUser*	pUserScript = pUser->AddComponent<CUser>("User");
	pUserScript->SetSecene(pMainScene);
	pUserScript->SetSerialNumber(iSerialNumber);
	pUserScript->AddUserObj(pUserScript);
	SAFE_RELEASE(pUserScript);


	// 플레이어 충돌체 설정.
	CDx11ColliderOBB*	pOBB = pUser->AddComponent<CDx11ColliderOBB>("UserColl");
	float fAxisLen[3];
	fAxisLen[0] = 0.015f*110.f;
	fAxisLen[1] = 0.015f*110.f;
	fAxisLen[2] = 0.015f*110.f;
	pOBB->SetPivot(Vec3(0.f, 1.f, 0.f));
	pOBB->SetOBBInfo(fAxisLen, pUserTr);
	SAFE_RELEASE(pOBB);

	SAFE_RELEASE(pUserTr);

	GET_SINGLE(CNetworkManager)->AddLoginObj(pUser);
	UpdateFlag = true;

	//SAFE_RELEASE(pUser);
}
