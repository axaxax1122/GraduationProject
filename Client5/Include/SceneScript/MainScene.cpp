#include "MainScene.h"
#include "GameObject/Dx11GameObject.h"
#include "Component/Dx11Transform.h"
#include "Component/Dx11Renderer.h"
#include "Scene/Dx11Scene.h"
#include "Scene/Dx11Layer.h"
#include "../ObjectScript/Player.h"
#include "../ObjectScript/Bullet.h"
#include "../ObjectScript/Monster.h"
#include "Core/Dx11Scheduler.h"
#include "Resources/Dx11ResourcesManager.h"
#include "Resources/Dx11Texture.h"
#include "Component/Dx11Material.h"
#include "../ObjectScript/GuidedMissile.h"
#include "Component/Dx11ColliderSphere.h"
#include "../ObjectScript/Laser.h"
#include "Core/Dx11CollisionManager.h"
#include "Component/Dx11ColliderRect.h"
#include "Component/Dx11ColliderAABB.h"
#include "Component/Dx11ColliderOBB.h"
#include "Component/Dx11UIBar.h"
#include "Resources/Dx11Mesh.h"
#include "Component/Dx11Camera.h"
#include "Component/Dx11Camera3rd.h"
#include "Component/Dx11BillBoard.h"
#include "Component/Dx11Terrain.h"
#include "Component/Dx11Animation2D.h"
#include "Component/Dx11UIButton.h"
#include "../ObjectScript/MapInfo.h"
#include "../ObjectScript/Snow.h"
#include "Component/Dx11SnowUI.h"
#include "../ObjectScript/Jump.h"
#include "../ObjectScript/MapObject.h"
#include "../ObjectScript/PlayerCollision.h"
#include "Scene/Dx11SceneManager.h"
#include "Component/Dx11UIBackground.h"
#include "Rendering/Dx11RenderManager.h"
#include "../ObjectScript/Star.h"
#include "../ObjectScript/BlossomWhite.h"
#include "../ObjectScript/MoveEffect_Right.h"
#include "../ObjectScript/MoveEffect_Left.h"
#include "Component/Dx11FSM.h"
#include "Core/Dx11Device.h"
#include "Component/Dx11ColliderRay.h"
#include "../Network/ZEROStream.h"
#include "../ObjectScript/Item.h"
#include "../Network/NetworkManager.h"
#include "../ClientCore.h"
#include "Core/Dx11Input.h"
#include "../ObjectScript/MinimapMove.h"
#include "../ObjectScript/User.h"
#include<ctime>	
#include<cstdlib>
#include "Core/Dx11SoundManager.h"

extern bool moveFlag;

CMainScene::CMainScene() :
	m_iMonsterCount(0),
	m_pMyPlayer(NULL),
	m_pUserPlayer(NULL),
	m_pUserTr(NULL),
	m_pLeftCarTr(NULL),
	m_pRightCarTr(NULL),
	m_pLeftCarTr2(NULL),
	m_pRightCarTr2(NULL),
	m_bTextFlag(false),
	m_Count(0)
{
	uHandle = CreateEvent(NULL, false, false, 0);
	random = 0;
	srand(time(NULL));
}

CMainScene::~CMainScene()
{
	SAFE_RELEASE(m_pMyPlayer);
	SAFE_RELEASE(m_pUserPlayer);
	SAFE_RELEASE(m_pUserTr);
	SAFE_RELEASE(m_pLeftCarTr);
	SAFE_RELEASE(m_pRightCarTr);
	SAFE_RELEASE(m_pLeftCarTr2);
	SAFE_RELEASE(m_pRightCarTr2);
	DX11_GET_SINGLE(Dx11SoundManager)->ReleaseSound();
}

int CMainScene::GetMonsterCount() const
{
	return m_iMonsterCount;
}

bool CMainScene::Init()
{
	DX11_GET_SINGLE(Dx11SoundManager)->InitSound();
	DX11_GET_SINGLE(Dx11SoundManager)->PlayBGM2();

	if (!GET_SINGLE(CNetworkManager)->Init(const_cast<char*>(CClientCore::GetInst()->GetIP()),
		CClientCore::GetInst()->GetPort()))
		return false;

	// 환경맵 설정
	m_pScene->CreateSky("SkyTex", L"Skybox\\SkyNoon.dds");

	// 디폴트 레이어
	CDx11Layer*	pLayer = m_pScene->FindLayer("DefaultLayer");
	if (!pLayer)
		return false;

	// 높이맵 생성
	CDx11GameObject*	pTerrainObj = CDx11GameObject::Create("Terrain");
	CDx11Terrain*		pTerrain = pTerrainObj->AddComponent<CDx11Terrain>("Terrain");
	CDx11Transform*		pTerrainTr = pTerrainObj->GetTransform();
	pTerrainTr->SetWorldPos(-65.f, 0.f, -65.f);
	SAFE_RELEASE(pTerrainTr);
	pTerrain->CreataTerrain("TerrainMesh", 129, 129, "TerrainDiffuse",
		L"Terrain\\RM_Ground_stone_D.png");
	pLayer->AddObject(pTerrainObj);
	SAFE_RELEASE(pTerrainObj);

	// 조명 생성
	CDx11GameObject*	pLightObj = m_pScene->CreateLight(LT_DIR, "Light");
	CDx11Transform*		pLightTr = pLightObj->GetTransform();
	pLightTr->RotationWorld(90.f*DX11_PI / 180.f, 1.f, AXIS_X);
	SAFE_RELEASE(pLightTr);
	SAFE_RELEASE(pLightObj);


	//맵FBX MeshInfo
	CDx11GameObject* pMapInfo = CDx11GameObject::Create("MapInfoObject");
	pLayer->AddObject(pMapInfo);//레이어에 추가(씬확인 때문에 먼저해줌)
	CMapInfo*	pMapInfoScript = pMapInfo->AddComponent<CMapInfo>("MapInfo");
	SAFE_RELEASE(pMapInfoScript);
	SAFE_RELEASE(pMapInfo);

	InitMapEffect();
	// 오브젝트 생성
	InitBullet();
	InitEffect();
	InitMiniMap();
	WaitText();
	SAFE_RELEASE(pTerrain);



	// Aim UI
	CDx11Layer*	pUILayer = m_pScene->FindLayer("UILayer");

	CDx11GameObject*	pAimObj = CDx11GameObject::Create("AimObj");
	CDx11Transform*		pTransform = pAimObj->GetTransform();
	RESOLUTION_TYPE eRS = DX11_GET_SINGLE(CDx11Device)->GetResolutionType();


	pTransform->SetWorldPos(g_tRS[eRS].iWidth / 2 - 5.f, g_tRS[eRS].iHeight / 2 + 10.f, 0.f);
	pTransform->SetWorldScale(150.f, 100.f, 1.f);
	SAFE_RELEASE(pTransform);

	CDx11Renderer*	pRenderer = pAimObj->AddComponent<CDx11Renderer>("AimRenderer");
	pRenderer->SetMesh("RectCenterUIMesh");
	pRenderer->SetShader(UI_SHADER);
	pRenderer->SetRenderState(DX11_RS_DEPTH_DISABLE);
	pRenderer->SetRenderState(DX11_RS_ALPHABLEND);
	pRenderer->CreateMaterial("CrossHair", L"UI\\CrossHair3.png");
	SAFE_RELEASE(pRenderer);

	CDx11UIBackground*	pUIBack = pAimObj->AddComponent<CDx11UIBackground>("AimBack");
	SAFE_RELEASE(pUIBack);

	CDx11ColliderRay*	pRay = pAimObj->AddComponent<CDx11ColliderRay>("AimColl");
	SAFE_RELEASE(pRay);

	pUILayer->AddObject(pAimObj);

	SAFE_RELEASE(pAimObj);

	SAFE_RELEASE(pUILayer);


	// 쿼드트리 렌더링 오브젝트 생성
	QuadTree* pQuadTree = DX11_GET_SINGLE(CDx11SceneManager)->GetQuadTree();
	CreateQuadTreeRender(pQuadTree);
	SAFE_RELEASE(pQuadTree);



	// 접속 처리한다.
	if (!GET_SINGLE(CNetworkManager)->Login(GET_SINGLE(CClientCore)->GetID(),
		GET_SINGLE(CClientCore)->GetPass()))
		return false;

	// 마우스 컨트롤
	ShowCursor(false);
	DX11_GET_SINGLE(CDx11Input)->SetControl(true);


	// 차 이동 트랜스폼
	CDx11GameObject *LeftCar1 = pLayer->FindGameObjectFromTag("CC_ME_R4(2)");
	CDx11GameObject *LeftCar2 = pLayer->FindGameObjectFromTag("HotDogTruck(1)");
	CDx11GameObject *RightCar1 = pLayer->FindGameObjectFromTag("jeep");
	CDx11GameObject *RightCar2 = pLayer->FindGameObjectFromTag("HotDogTruck(2)");

	if (LeftCar1) m_pLeftCarTr = LeftCar1->GetTransform();
	if (LeftCar2) m_pLeftCarTr2 = LeftCar2->GetTransform();
	if (RightCar1) m_pRightCarTr = RightCar1->GetTransform();
	if (RightCar2) m_pRightCarTr2 = RightCar2->GetTransform();

	if (LeftCar1)  SAFE_RELEASE(LeftCar1);
	if (LeftCar2)  SAFE_RELEASE(LeftCar2);
	if (RightCar1) SAFE_RELEASE(RightCar1);
	if (RightCar2) SAFE_RELEASE(RightCar2);

	//벛꽃 



	SAFE_RELEASE(pLayer);
	return true;
}

void CMainScene::Input(float fTime)
{
}

int CMainScene::Update(float fTime)
{
	// 로그인 유저를 얻어와서 일반 레이어에 추가한다.
	if (!GET_SINGLE(CNetworkManager)->EmptyLoginUser())
	{
		CDx11Layer*	pLayer = m_pScene->FindLayer("DefaultLayer");
		GET_SINGLE(CNetworkManager)->AddLayer(pLayer);

		m_pUserPlayer = pLayer->FindGameObjectFromTag("UserObject");
		m_pUserPlayer->AddRef();
		m_pUserTr = m_pUserPlayer->GetTransform();

		SAFE_RELEASE(pLayer);
	}
	if (moveFlag == true && m_bTextFlag == false) {
		CDx11Layer*   pUILayer = m_pScene->FindLayer("UILayer");
		CDx11GameObject* pText = pUILayer->FindGameObjectFromTag("WaitText");

		if (!pText)
			return 0;

		pText->Death();
		m_bTextFlag = true;
		SAFE_RELEASE(pUILayer);
		SAFE_RELEASE(pText);

	}

	// 충돌체 렌더링
	static bool bCollRender;
	if (GetAsyncKeyState(VK_F1) & 0x8000)
		bCollRender = true;
	else if (bCollRender)
	{
		DX11_GET_SINGLE(CDx11CollisionManager)->ColliderRenderEnable();
		bCollRender = false;
	}

	// 쿼드트리 렌더링
	static bool bQTRender;
	if (GetAsyncKeyState(VK_F2) & 0x8000)
		bQTRender = true;
	else if (bQTRender)
	{
		DX11_GET_SINGLE(CDx11RenderManager)->QuadTreeRenderEnable();
		bQTRender = false;
	}

	//결과창 
	ResultShow();

	return 0;
}

int CMainScene::LateUpdate(float fTime)
{
	WaitForSingleObject(uHandle, INFINITE);

	SetEvent(uHandle);

	static int temp = 0;
	static int temp2 = 0;
	if (temp > 25) {
		if (temp2 == 0) {
			CreateSnow();
			temp2++;
		}
	}
	else {
		temp++;
	}
	return 0;
}

void CMainScene::Render(float fTime)
{
}

CMainScene * CMainScene::Clone()
{
	return new CMainScene(*this);
}

void CMainScene::InitPlayer(BYTE * pPacket, CDx11Terrain * pTerrain)
{

	// 받아온 좌표 읽기.
	CZEROStreamSP	stream;
	stream->SetBuffer(pPacket);

	Vec3	vPos, vScale, vRot;
	DWORD iCount;

	stream->ReadFloat(&vScale.x);
	stream->ReadFloat(&vScale.y);
	stream->ReadFloat(&vScale.z);
	stream->ReadFloat(&vPos.x);
	stream->ReadFloat(&vPos.y);
	stream->ReadFloat(&vPos.z);
	stream->ReadFloat(&vRot.x);
	stream->ReadFloat(&vRot.y);
	stream->ReadFloat(&vRot.z);

	stream->ReadDWORD(&iCount);

	if (!m_pScene)
		return;

	//만들어놨던 디폴트 레이어를 찾는다.
	CDx11Layer*	pLayer = m_pScene->FindLayer("DefaultLayer");
	if (!pLayer)
		return;


	// 플레이어 객체를 생성한다.
	CDx11GameObject*	pPlayer = CDx11GameObject::Create("PlayerObject");
	pLayer->AddObject(pPlayer);

	CDx11Transform*		pTr = pPlayer->GetTransform();
	pTr->SetWorldScale(vScale);
	pTr->SetWorldPos(Vec3(vPos.x, (vPos.y - 1.5f), vPos.z));
	pTr->SetWorldRot(vRot);

	pTr->SetLocalRotY(-DX11_PI);
	pTr->RotationWorld(-DX11_PI, 1.f, AXIS_Y);

	// Finite State Machine
	CDx11FSM*	pFSM = pPlayer->AddComponent<CDx11FSM>("PlayerFSM");
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



	// 렌더러를 생성한다.
	CDx11Renderer*	pRenderer = pPlayer->AddComponent<CDx11Renderer>("PlayerRenderer");
	pRenderer->SetMesh("PlayerMesh");
	pRenderer->SetShader(ANIMATION_SHADER);

	// 노말 텍스처 세팅
	CDx11Material*	pMtrl = pRenderer->GetMaterial();
	pMtrl->SetDiffuseTexture("FutureSoldier_Diffuse", L"MeshTextures\\FutureSoldier_02_albedo_2.TGA", MESH_PATH);
	pMtrl->SetSpecularTexture("FutureSoldier_Specular", L"MeshTextures\\FutureSoldier_02_roughness.TGA", MESH_PATH);
	pMtrl->SetNormalMapTexture("FutureSoldier_Normal", L"MeshTextures\\FutureSoldier_01_normal.TGA", MESH_PATH);

	SAFE_RELEASE(pMtrl);
	SAFE_RELEASE(pRenderer);


	// 플레이어 스크립트를 컴포넌트에 추가한다.
	CPlayer*	pPlayerScript = pPlayer->AddComponent<CPlayer>("PlayerScript");
	if (pTerrain) pPlayerScript->SetTerrain(pTerrain);
	SAFE_RELEASE(pPlayerScript);

	//컴포넌트 추가
	//CJump*	pJumpScript = pPlayer->AddComponent<CJump>("JumpGo");
	//SAFE_RELEASE(pJumpScript);

	//컴포넌트 추가
	CPlayerCollision* pPlayerCollision = pPlayer->AddComponent<CPlayerCollision>("PlayerCollisionGo");
	SAFE_RELEASE(pPlayerCollision);


	// 플레이어 충돌체 설정.
	CDx11ColliderOBB*	pOBB = pPlayer->AddComponent<CDx11ColliderOBB>("PlayerColl");
	float fAxisLen[3];
	fAxisLen[0] = 0.015f*45.f;
	fAxisLen[1] = 0.015f*35.f;
	fAxisLen[2] = 0.015f*45.f;
	pOBB->SetPivot(Vec3(0.f, 1.f, 0.f));
	pOBB->SetOBBInfo(fAxisLen, pTr);
	SAFE_RELEASE(pOBB);

	SAFE_RELEASE(pTr);

	// 플레이어 카메라를 생성한다.
	CDx11GameObject*	pCameraObj = m_pScene->CreateCamera("PlayerCamera", DX11_PI / 3.f, 0.1f, 1000.f);
	CDx11Transform*		pCameraTr = pCameraObj->GetTransform();

	pCameraTr->MoveWorld(vPos);
	CDx11Camera*		pCam = (CDx11Camera*)pCameraObj->FindComponentFromTypeName("CDx11Camera");

	pCam->SetTarget(pPlayer);
	pCam->SetCameraType(CVT_THIRD);
	if (pTerrain) pCam->SetTerrain(pTerrain);
	m_pScene->ChangeCamera("PlayerCamera");

	if (iCount % 2 == 0)
		pCam->RotateLeftRight(DX11_PI);


	GET_SINGLE(CNetworkManager)->InitSucc();

	SAFE_RELEASE(pCam);
	SAFE_RELEASE(pCameraTr);
	SAFE_RELEASE(pCameraObj);


	m_pMyPlayer = pPlayer;
	pPlayer->AddRef();

	SAFE_RELEASE(pPlayer);
	SAFE_RELEASE(pLayer);
	SetEvent(uHandle);
}


void CMainScene::InitBullet()
{
	//만들어놨던 디폴트 레이어를 찾는다.
	CDx11Layer*	pLayer = m_pScene->FindLayer("DefaultLayer");
	if (!pLayer)
		return;

	// 불렛 프로토타입을 만든다.
	CDx11GameObject*	pBullet = CDx11GameObject::Create("BulletPrototype", OT_PROTOTYPE);

	// 렌더러를 생성한다.
	CDx11Renderer* pRenderer = pBullet->AddComponent<CDx11Renderer>("BulletRenderer");
	pRenderer->SetMesh("BulletMesh");
	pRenderer->SetShader(DEFAULT_BUMP_SHADER);
	SAFE_RELEASE(pRenderer);

	// 총알 스크립트를 컴포넌트에 추가한다.
	CBullet*	pBulletSc = (CBullet*)pBullet->AddComponent<CBullet>("BulletScript");
	SAFE_RELEASE(pBulletSc);

	// 충돌체를 설정한다.
	CDx11ColliderOBB*	pOBB = pBullet->AddComponent<CDx11ColliderOBB>("BulletColl");
	CDx11Transform*	pTr = pBullet->GetTransform();
	float fAxisLen[3];
	fAxisLen[0] = 0.5f;
	fAxisLen[1] = 1.f;
	fAxisLen[2] = 0.5f;
	pOBB->SetOBBInfo(fAxisLen, pTr);
	SAFE_RELEASE(pTr);
	SAFE_RELEASE(pOBB);



	// 유도 미사일 프로토타입을 만든다.
	CDx11GameObject* pMissile = CDx11GameObject::Create("MissilePrototype", OT_PROTOTYPE);

	// 렌더러를 생성한다.
	pRenderer = pMissile->AddComponent<CDx11Renderer>("MissileRenderer");
	pRenderer->SetMesh("MissileMesh");
	pRenderer->SetShader(DEFAULT_BUMP_SHADER);
	SAFE_RELEASE(pRenderer);

	// 미사일 스크립트를 컴포넌트에 추가한다.
	CGuidedMissile*	pMissileSc = (CGuidedMissile*)pMissile->AddComponent<CGuidedMissile>("MissileScript");
	SAFE_RELEASE(pMissileSc);

	// 충돌체를 설정한다.
	pOBB = pMissile->AddComponent<CDx11ColliderOBB>("MissileColl");
	pTr = pMissile->GetTransform();
	fAxisLen[0] = 0.5f;
	fAxisLen[1] = 1.f;
	fAxisLen[2] = 0.5f;
	pOBB->SetOBBInfo(fAxisLen, pTr);
	SAFE_RELEASE(pTr);
	SAFE_RELEASE(pOBB);


	// 레이저 프로토타입을 만든다.
	CDx11GameObject*	pLaser = CDx11GameObject::Create("LaserPrototype", OT_PROTOTYPE);

	// 렌더러를 생성한다.
	pRenderer = pLaser->AddComponent<CDx11Renderer>("LaserRenderer");
	pRenderer->SetMesh("MissileMesh");
	pRenderer->SetShader(DEFAULT_BUMP_SHADER);
	SAFE_RELEASE(pRenderer);

	// 총알 스크립트를 컴포넌트에 추가한다.
	CLaser*	pLaserSc = (CLaser*)pLaser->AddComponent<CLaser>("LaserScript");
	SAFE_RELEASE(pLaserSc);

	// 충돌체를 설정한다.
	pOBB = pLaser->AddComponent<CDx11ColliderOBB>("LaserCollider");
	pTr = pLaser->GetTransform();
	fAxisLen[0] = 0.5f;
	fAxisLen[1] = 1.f;
	fAxisLen[2] = 0.5f;
	pOBB->SetOBBInfo(fAxisLen, pTr);
	SAFE_RELEASE(pTr);
	SAFE_RELEASE(pOBB);


	SAFE_RELEASE(pLayer);

}

void CMainScene::InitMonster()
{
	//만들어놨던 디폴트 레이어를 찾는다.
	CDx11Layer*	pLayer = m_pScene->FindLayer("DefaultLayer");
	if (!pLayer)
		return;

	// 몬스터 프로토타입을 만든다.
	CDx11GameObject*	pMonster = CDx11GameObject::Create("MonsterPrototype", OT_PROTOTYPE);

	// 렌더러를 생성한다.
	CDx11Renderer* pRenderer = pMonster->AddComponent<CDx11Renderer>("MonsterRenderer");
	pRenderer->SetMesh("MonsterMesh");
	pRenderer->SetShader(DEFAULT_BUMP_SHADER);
	SAFE_RELEASE(pRenderer);

	// 몬스터 스크립트를 컴포넌트에 추가한다.
	CMonster*	pMonsterSc = (CMonster*)pMonster->AddComponent<CMonster>("MonsterScript");
	SAFE_RELEASE(pMonsterSc);

	// 충돌체를 설정한다.
	CDx11ColliderOBB* pOBB = pMonster->AddComponent<CDx11ColliderOBB>("MonsterColl");
	CDx11Transform* pTr = pMonster->GetTransform();
	float fAxisLen[3];
	fAxisLen[0] = 1.5f;
	fAxisLen[1] = 1.5f;
	fAxisLen[2] = 1.5f;
	pOBB->SetPivot(Vec3(0.f, 1.5f, 0.f));
	pOBB->SetOBBInfo(fAxisLen, pTr);
	SAFE_RELEASE(pTr);
	SAFE_RELEASE(pOBB);



	SAFE_RELEASE(pLayer);

	// n초마다 몬스터를 생성하는 스케쥴러를 만든다.
	DX11_GET_SINGLE(CDx11Scheduler)->CreateSchedule(this, &CMainScene::CreateMonster, 5.f, SO_LOOP);
}

void CMainScene::InitEffect()
{
	// 이펙트 생성
	CDx11GameObject*	pEffectObj = CDx11GameObject::Create("Effect", OT_PROTOTYPE);

	CDx11Transform*		pEffectTr = pEffectObj->GetTransform();
	pEffectTr->SetWorldScale(15.f, 15.f, 15.f);
	SAFE_RELEASE(pEffectTr);

	CDx11Renderer*	pRenderer = pEffectObj->AddComponent<CDx11Renderer>("EffectRenderer");
	pRenderer->CreateMaterial("Explosion1", L"Effect\\Explosion\\320x240\\Explosion1.png");
	SAFE_RELEASE(pRenderer);

	CDx11BillBoard* pBillBoard = pEffectObj->AddComponent<CDx11BillBoard>("EffectBillBoard");
	SAFE_RELEASE(pBillBoard);

	CDx11Animation2D* pAnimation2D = pEffectObj->AddComponent<CDx11Animation2D>("EffectAnim2D");
	pAnimation2D->Init();
	pAnimation2D->SetMaxFrame(89, 1);
	pAnimation2D->SetAnimation2DOption(A2DO_ONCE);
	//	pAnimation2D->SetAnimation2DType(A2D_ATLAS_ALL);
	pAnimation2D->SetAnimationLimitTime(1.f);

	for (int i = 1; i <= 89; ++i)
	{
		char	strKey[256] = {};
		sprintf_s(strKey, "Explosion%d", i);
		wchar_t	strPath[MAX_PATH] = {};
		wsprintf(strPath, L"Effect\\Explosion\\320x240\\Explosion%d.png", i);
		pAnimation2D->AddFrameTexture(strKey, strPath);

	}
	SAFE_RELEASE(pAnimation2D);



	CDx11GameObject*	pSmallEffectObj = CDx11GameObject::Create("SmallEffect", OT_PROTOTYPE);
	pEffectTr = pSmallEffectObj->GetTransform();
	pEffectTr->SetWorldScale(5.f, 5.f, 5.f);
	SAFE_RELEASE(pEffectTr);

	pRenderer = pSmallEffectObj->AddComponent<CDx11Renderer>("SmallRenderer");
	pRenderer->CreateMaterial("SmallExplosion1", L"Effect\\Explosion\\64x48\\Explosion1.png");
	SAFE_RELEASE(pRenderer);

	pBillBoard = pSmallEffectObj->AddComponent<CDx11BillBoard>("SmallBillBoard");
	SAFE_RELEASE(pBillBoard);

	pAnimation2D = pSmallEffectObj->AddComponent<CDx11Animation2D>("SmallAnim2D");
	pAnimation2D->Init();
	pAnimation2D->SetMaxFrame(89, 1);
	pAnimation2D->SetAnimation2DOption(A2DO_ONCE);
	pAnimation2D->SetAnimationLimitTime(0.75f);
	for (int i = 1; i <= 89; ++i)
	{
		char	strKey[256] = {};
		sprintf_s(strKey, "Explosion%d", i);
		wchar_t	strPath[MAX_PATH] = {};
		wsprintf(strPath, L"Effect\\Explosion\\64x48\\Explosion%d.png", i);
		pAnimation2D->AddFrameTexture(strKey, strPath);

	}
	SAFE_RELEASE(pAnimation2D);


	//CDx11GameObject*	pSubEffectObj = CDx11GameObject::Create("SubEffect", OT_PROTOTYPE);
	//pEffectTr = pSubEffectObj->GetTransform();
	//pEffectTr->SetWorldScale(0.8f, 0.8f, 0.8f);
	//SAFE_RELEASE(pEffectTr);

	//pRenderer = pSubEffectObj->AddComponent<CDx11Renderer>("SubRenderer");
	//pRenderer->CreateMaterial("SubExplosion", L"Effect\\Explosion\\T_Explosion_SubUV1.png");
	//SAFE_RELEASE(pRenderer);

	//pBillBoard = pSubEffectObj->AddComponent<CDx11BillBoard>("SubBillBoard");
	//SAFE_RELEASE(pBillBoard);

	//pAnimation2D = pSubEffectObj->AddComponent<CDx11Animation2D>("SubAnim2D");
	//pAnimation2D->Init();
	//pAnimation2D->SetMaxFrame(6, 6);
	//pAnimation2D->SetAnimation2DOption(A2DO_ONCE);
	//pAnimation2D->SetAnimation2DType(A2D_ATLAS_ALL);
	//pAnimation2D->SetAnimationLimitTime(0.75f);

	//SAFE_RELEASE(pAnimation2D);


}

void CMainScene::InitMapEffect()
{
	//--------------------------------- move_object_RIGHT [jeep]-----------------------------------------------
	CDx11GameObject*   pEffectObj_jeep = CDx11GameObject::Create("Effect_jeep");

	//컴포넌트 추가
	CDx11Transform*   pEffectTr = pEffectObj_jeep->GetTransform();
	pEffectTr->SetWorldScale(10.f, 10.f, 10.f);
	pEffectTr->SetWorldPos(0.f, 3.f, 0.f);
	SAFE_RELEASE(pEffectTr);

	//컴포넌트 추가
	CDx11Renderer*   pRenderer = pEffectObj_jeep->AddComponent<CDx11Renderer>("EffectRenderer");
	pRenderer->CreateMaterial("Effect", L"Effect\\CarEffect\\carEffect.png");
	SAFE_RELEASE(pRenderer);

	//컴포넌트 추가
	CDx11BillBoard*   pEffectBill = pEffectObj_jeep->AddComponent<CDx11BillBoard>("BillBoard");
	SAFE_RELEASE(pEffectBill);

	//컴포넌트 추가
	CDx11Animation2D*   pAnimation2D = pEffectObj_jeep->AddComponent<CDx11Animation2D>("Animatino2D");
	pAnimation2D->SetMaxFrame(5, 4);//5x4 행렬 텍스쳐.
	pAnimation2D->SetAnimation2DType(A2D_ATLAS_ALL);
	pAnimation2D->SetAnimationLimitTime(0.5f);
	SAFE_RELEASE(pAnimation2D);

	//컴포넌트 추가
	CMoveEffect_Right* pEffect = pEffectObj_jeep->AddComponent<CMoveEffect_Right>("CMoveEffect");
	pEffect->SetName("jeep");
	SAFE_RELEASE(pEffect);

	// 디폴트 레이어
	CDx11Layer*   pLayer = m_pScene->FindLayer("DefaultLayer");
	pLayer->AddObject(pEffectObj_jeep);
	SAFE_RELEASE(pEffectObj_jeep);

	//--------------------------------- move_object_RIGHT [HotDogTruck(2)]-----------------------------------------------
	CDx11GameObject*   pEffectObj_HotDogTruck_Right = CDx11GameObject::Create("Effect_HotDogTruck(2)");

	//컴포넌트 추가
	pEffectTr = pEffectObj_HotDogTruck_Right->GetTransform();
	pEffectTr->SetWorldScale(10.f, 10.f, 10.f);
	pEffectTr->SetWorldPos(0.f, 3.f, 0.f);
	SAFE_RELEASE(pEffectTr);

	//컴포넌트 추가
	pRenderer = pEffectObj_HotDogTruck_Right->AddComponent<CDx11Renderer>("EffectRenderer");
	pRenderer->CreateMaterial("CarEffect", L"Effect\\CarEffect\\carEffect.png");
	SAFE_RELEASE(pRenderer);

	//컴포넌트 추가
	pEffectBill = pEffectObj_HotDogTruck_Right->AddComponent<CDx11BillBoard>("BillBoard");
	SAFE_RELEASE(pEffectBill);

	//컴포넌트 추가
	pAnimation2D = pEffectObj_HotDogTruck_Right->AddComponent<CDx11Animation2D>("Animatino2D");
	pAnimation2D->SetMaxFrame(5, 4);//5x4 행렬 텍스쳐.
	pAnimation2D->SetAnimation2DType(A2D_ATLAS_ALL);
	pAnimation2D->SetAnimationLimitTime(0.5f);
	SAFE_RELEASE(pAnimation2D);

	//컴포넌트 추가
	pEffect = pEffectObj_HotDogTruck_Right->AddComponent<CMoveEffect_Right>("CMoveEffect");
	pEffect->SetName("HotDogTruck(2)");
	SAFE_RELEASE(pEffect);

	// 디폴트 레이어
	pLayer->AddObject(pEffectObj_HotDogTruck_Right);
	SAFE_RELEASE(pEffectObj_HotDogTruck_Right);

	//--------------------------------- move_object_LEFT [CC_ME_R4(2)]-----------------------------------------------
	CDx11GameObject*   pEffectObj_CC_ME_R4 = CDx11GameObject::Create("Effect_HotDogTruck(2)");

	//컴포넌트 추가
	pEffectTr = pEffectObj_CC_ME_R4->GetTransform();
	pEffectTr->SetWorldScale(10.f, 10.f, 10.f);
	pEffectTr->SetWorldPos(0.f, 3.f, 0.f);
	SAFE_RELEASE(pEffectTr);

	//컴포넌트 추가
	pRenderer = pEffectObj_CC_ME_R4->AddComponent<CDx11Renderer>("EffectRenderer");
	pRenderer->CreateMaterial("CarEffect", L"Effect\\CarEffect\\carEffect.png");
	SAFE_RELEASE(pRenderer);

	//컴포넌트 추가
	pEffectBill = pEffectObj_CC_ME_R4->AddComponent<CDx11BillBoard>("BillBoard");
	SAFE_RELEASE(pEffectBill);

	//컴포넌트 추가
	pAnimation2D = pEffectObj_CC_ME_R4->AddComponent<CDx11Animation2D>("Animatino2D");
	pAnimation2D->SetMaxFrame(5, 4);//5x4 행렬 텍스쳐.
	pAnimation2D->SetAnimation2DType(A2D_ATLAS_ALL);
	pAnimation2D->SetAnimationLimitTime(0.5f);
	SAFE_RELEASE(pAnimation2D);

	//컴포넌트 추가
	CMoveEffect_Left *pEffect_left = pEffectObj_CC_ME_R4->AddComponent<CMoveEffect_Left>("CMoveEffect");
	pEffect_left->SetName("CC_ME_R4(2)");
	SAFE_RELEASE(pEffect_left);

	// 디폴트 레이어
	pLayer->AddObject(pEffectObj_CC_ME_R4);
	SAFE_RELEASE(pEffectObj_CC_ME_R4);
	//--------------------------------- move_object_LEFT [HotDogTruck]-----------------------------------------------
	CDx11GameObject*   pEffectObj_HotDogTruck_LEFT = CDx11GameObject::Create("HotDogTruck(1)");

	//컴포넌트 추가
	pEffectTr = pEffectObj_HotDogTruck_LEFT->GetTransform();
	pEffectTr->SetWorldScale(10.f, 10.f, 10.f);
	pEffectTr->SetWorldPos(0.f, 3.f, 0.f);
	SAFE_RELEASE(pEffectTr);

	//컴포넌트 추가
	pRenderer = pEffectObj_HotDogTruck_LEFT->AddComponent<CDx11Renderer>("EffectRenderer");
	pRenderer->CreateMaterial("CarEffect", L"Effect\\CarEffect\\carEffect.png");
	SAFE_RELEASE(pRenderer);

	//컴포넌트 추가
	pEffectBill = pEffectObj_HotDogTruck_LEFT->AddComponent<CDx11BillBoard>("BillBoard");
	SAFE_RELEASE(pEffectBill);

	//컴포넌트 추가
	pAnimation2D = pEffectObj_HotDogTruck_LEFT->AddComponent<CDx11Animation2D>("Animatino2D");
	pAnimation2D->SetMaxFrame(5, 4);//5x4 행렬 텍스쳐.
	pAnimation2D->SetAnimation2DType(A2D_ATLAS_ALL);
	pAnimation2D->SetAnimationLimitTime(0.5f);
	SAFE_RELEASE(pAnimation2D);

	//컴포넌트 추가
	pEffect_left = pEffectObj_HotDogTruck_LEFT->AddComponent<CMoveEffect_Left>("CMoveEffect");
	pEffect_left->SetName("HotDogTruck(1)");
	SAFE_RELEASE(pEffect_left);

	// 디폴트 레이어
	pLayer->AddObject(pEffectObj_HotDogTruck_LEFT);
	SAFE_RELEASE(pEffectObj_HotDogTruck_LEFT);



	SAFE_RELEASE(pLayer);
}

int CMainScene::InitMiniMap()
{
	//----------------------- 미니맵 ---------------------------------
	CDx11GameObject* pMinimap = CDx11GameObject::Create("Minimap");

	//컴포넌트추가
	CDx11Renderer*	 pRenderer = pMinimap->AddComponent<CDx11Renderer>("MinimapRenderer");
	pRenderer->SetMesh(RECT_UI_MESH);
	pRenderer->SetShader(UI_SHADER);
	pRenderer->SetRenderState(DX11_RS_ALPHABLEND);
	pRenderer->CreateMaterial("Minimap", L"Effect\\minimap5.png");// 재질을 설정한다.
	SAFE_RELEASE(pRenderer);

	//컴포넌트추가
	float mapSize = 200.f;
	float mapHalfSize = 100.f;
	CDx11SnowUI* pSnowUISc = (CDx11SnowUI*)pMinimap->AddComponent<CDx11SnowUI>("MinimapUIScript");
	pSnowUISc->SetBarPos((1280 - 103) - mapHalfSize, (103) - mapHalfSize, 2);
	pSnowUISc->SetBarSize(mapSize, mapSize);
	SAFE_RELEASE(pSnowUISc);

	//레이어에 추가
	CDx11Layer*	pLayer = m_pScene->FindLayer("UILayer");

	if (!pLayer)
		return false;
	pLayer->AddObject(pMinimap);
	SAFE_RELEASE(pMinimap);

	//---------------------------- 미니 플레이어 ---------------------------
	CDx11GameObject* pMinimapPlayer = CDx11GameObject::Create("MinimapPlayer");

	//컴포넌트추가
	pRenderer = pMinimapPlayer->AddComponent<CDx11Renderer>("MinimapPlayerRenderer");
	pRenderer->SetMesh(RECT_UI_MESH);
	pRenderer->SetShader(UI_SHADER);
	pRenderer->SetRenderState(DX11_RS_ALPHABLEND);
	pRenderer->CreateMaterial("MinimapPlayer", L"Effect\\MinimapPlayer.png");// 재질을 설정한다.
	SAFE_RELEASE(pRenderer);

	//컴포넌트추가
	mapSize = 10.f;
	mapHalfSize = 5.f;
	pSnowUISc = (CDx11SnowUI*)pMinimapPlayer->AddComponent<CDx11SnowUI>("MinimapPlayerUIScript");
	pSnowUISc->SetBarPos((1280 - 103) - mapHalfSize, (103) - mapHalfSize, 1);
	pSnowUISc->SetBarSize(mapSize, mapSize);
	SAFE_RELEASE(pSnowUISc);

	//컴포넌트추가
	CMinimapMove *pMp = pMinimapPlayer->AddComponent<CMinimapMove>("MinimapScript");
	SAFE_RELEASE(pMp);

	//레이어에 추가
	pLayer->AddObject(pMinimapPlayer);
	SAFE_RELEASE(pMinimapPlayer);
	//---------------------------- 미니 플레이어2 ---------------------------
	CDx11GameObject* pMinimapPlayer2 = CDx11GameObject::Create("MinimapPlayer2");

	//컴포넌트추가
	pRenderer = pMinimapPlayer2->AddComponent<CDx11Renderer>("MinimapPlayerRenderer");
	pRenderer->SetMesh(RECT_UI_MESH);
	pRenderer->SetShader(UI_SHADER);
	pRenderer->SetRenderState(DX11_RS_ALPHABLEND);
	pRenderer->CreateMaterial("MinimapPlayer", L"Effect\\MinimapPlayer.png");// 재질을 설정한다.
	SAFE_RELEASE(pRenderer);

	//컴포넌트추가
	mapSize = 10.f;
	mapHalfSize = 5.f;
	pSnowUISc = (CDx11SnowUI*)pMinimapPlayer2->AddComponent<CDx11SnowUI>("MinimapPlayerUIScript");
	pSnowUISc->SetBarPos((1280 - 103) - mapHalfSize, (103) - mapHalfSize, 1);
	pSnowUISc->SetBarSize(mapSize, mapSize);
	SAFE_RELEASE(pSnowUISc);

	//컴포넌트추가
	pMp = pMinimapPlayer2->AddComponent<CMinimapMove>("MinimapScript");
	SAFE_RELEASE(pMp);

	//레이어에 추가
	pLayer->AddObject(pMinimapPlayer2);
	SAFE_RELEASE(pMinimapPlayer2);
	//---------------------- move_object_RIGHT [jeep]---------------------------
	CDx11GameObject* pjeep = CDx11GameObject::Create("Minimapjeep");

	//컴포넌트추가
	pRenderer = pjeep->AddComponent<CDx11Renderer>("jeepRenderer");
	pRenderer->SetMesh(RECT_UI_MESH);
	pRenderer->SetShader(UI_SHADER);
	pRenderer->SetRenderState(DX11_RS_ALPHABLEND);
	pRenderer->CreateMaterial("Minimapjeep", L"Effect\\MoveObject_Green.png");// 재질을 설정한다.
	SAFE_RELEASE(pRenderer);

	//컴포넌트추가
	mapSize = 10.f;
	mapHalfSize = 5.f;
	pSnowUISc = (CDx11SnowUI*)pjeep->AddComponent<CDx11SnowUI>("jeepUIScript");
	pSnowUISc->SetBarPos((1280 - 103) - mapHalfSize, (103) - mapHalfSize, 1);
	pSnowUISc->SetBarSize(mapSize, mapSize);
	SAFE_RELEASE(pSnowUISc);

	//컴포넌트추가
	pMp = pjeep->AddComponent<CMinimapMove>("MinimapScript");
	SAFE_RELEASE(pMp);

	//레이어에 추가
	pLayer->AddObject(pjeep);
	SAFE_RELEASE(pjeep);
	//-------------------- move_object_RIGHT [HotDogTruck(2)]-------------------	
	CDx11GameObject* pHotDogTruck2 = CDx11GameObject::Create("MinimapHotDogTruck(2)");

	//컴포넌트추가
	pRenderer = pHotDogTruck2->AddComponent<CDx11Renderer>("HotDogTruck(2)Renderer");
	pRenderer->SetMesh(RECT_UI_MESH);
	pRenderer->SetShader(UI_SHADER);
	pRenderer->SetRenderState(DX11_RS_ALPHABLEND);
	pRenderer->CreateMaterial("MinimapHotDogTruck(2)", L"Effect\\MoveObject.png");
	SAFE_RELEASE(pRenderer);

	//컴포넌트추가
	mapSize = 10.f;
	mapHalfSize = 5.f;
	pSnowUISc = (CDx11SnowUI*)pHotDogTruck2->AddComponent<CDx11SnowUI>("HotDogTruck(2)UIScript");
	pSnowUISc->SetBarPos((1280 - 103) - mapHalfSize, (103) - mapHalfSize, 1);
	pSnowUISc->SetBarSize(mapSize, mapSize);
	SAFE_RELEASE(pSnowUISc);

	//컴포넌트추가
	pMp = pHotDogTruck2->AddComponent<CMinimapMove>("HotDogTruck(2)Script");
	SAFE_RELEASE(pMp);

	//레이어에 추가
	pLayer->AddObject(pHotDogTruck2);
	SAFE_RELEASE(pHotDogTruck2);
	//-------------------- move_object_LEFT [CC_ME_R4(2)]-----------------------
	CDx11GameObject* pCC_ME_R42 = CDx11GameObject::Create("MinimapCC_ME_R4(2)");

	//컴포넌트추가
	pRenderer = pCC_ME_R42->AddComponent<CDx11Renderer>("HotDogTruck(2)Renderer");
	pRenderer->SetMesh(RECT_UI_MESH);
	pRenderer->SetShader(UI_SHADER);
	pRenderer->SetRenderState(DX11_RS_ALPHABLEND);
	pRenderer->CreateMaterial("MinimapCC_ME_R4(2)", L"Effect\\MoveObject_yellow.png");
	SAFE_RELEASE(pRenderer);

	//컴포넌트추가
	mapSize = 10.f;
	mapHalfSize = 5.f;
	pSnowUISc = (CDx11SnowUI*)pCC_ME_R42->AddComponent<CDx11SnowUI>("CC_ME_R4(2)UIScript");
	pSnowUISc->SetBarPos((1280 - 103) - mapHalfSize, (103) - mapHalfSize, 1);
	pSnowUISc->SetBarSize(mapSize, mapSize);
	SAFE_RELEASE(pSnowUISc);

	//컴포넌트추가
	pMp = pCC_ME_R42->AddComponent<CMinimapMove>("CC_ME_R4(2)Script");
	SAFE_RELEASE(pMp);

	//레이어에 추가
	pLayer->AddObject(pCC_ME_R42);
	SAFE_RELEASE(pCC_ME_R42);
	//--------------------- move_object_LEFT [HotDogTruck]----------------------
	CDx11GameObject* pHotDogTruck = CDx11GameObject::Create("MinimapHotDogTruck");

	//컴포넌트추가
	pRenderer = pHotDogTruck->AddComponent<CDx11Renderer>("HotDogTruckRenderer");
	pRenderer->SetMesh(RECT_UI_MESH);
	pRenderer->SetShader(UI_SHADER);
	pRenderer->SetRenderState(DX11_RS_ALPHABLEND);
	pRenderer->CreateMaterial("MinimapHotDogTruck", L"Effect\\MoveObject.png");
	SAFE_RELEASE(pRenderer);

	//컴포넌트추가
	mapSize = 10.f;
	mapHalfSize = 5.f;
	pSnowUISc = (CDx11SnowUI*)pHotDogTruck->AddComponent<CDx11SnowUI>("HotDogTruckUIScript");
	pSnowUISc->SetBarPos((1280 - 103) - mapHalfSize, (103) - mapHalfSize, 1);
	pSnowUISc->SetBarSize(mapSize, mapSize);
	SAFE_RELEASE(pSnowUISc);

	//컴포넌트추가
	pMp = pHotDogTruck->AddComponent<CMinimapMove>("HotDogTruckScript");
	SAFE_RELEASE(pMp);

	//레이어에 추가
	pLayer->AddObject(pHotDogTruck);
	SAFE_RELEASE(pHotDogTruck);

	//-----------------------------------------------------------------------------
	SAFE_RELEASE(pLayer);

	return 0;
}

void CMainScene::CreateQuadTreeRender(QuadTree * pNode)
{
	Vec3 vCenter = pNode->GetCenter();
	float fRadius = pNode->GetRadius();
	if (fRadius < 10.f)
	{
		CDx11Layer*			pLayer = m_pScene->FindLayer("UILayer");
		CDx11GameObject*	pObj = pNode->GetQTRender();

		pLayer->AddObject(pObj);

		SAFE_RELEASE(pObj);
		SAFE_RELEASE(pLayer);
	}

	if (pNode->GetChild(0))
	{
		for (int i = 0; i < 4; ++i)
		{
			CreateQuadTreeRender(pNode->GetChild(i));
		}
	}

}

int CMainScene::CreateSnow()
{
	//눈(벚꽃)
	for (int i = 0; i < 3; i++) {
		CDx11GameObject* pSnow = CDx11GameObject::Create("BlossomClone");

		//컴포넌트추가
		CDx11Renderer*	 pRenderer = pSnow->AddComponent<CDx11Renderer>("BlossomRenderer");
		pRenderer->SetMesh(RECT_UI_MESH);
		pRenderer->SetShader(UI_SHADER);
		pRenderer->SetRenderState(DX11_RS_ALPHABLEND);
		pRenderer->CreateMaterial("Blossom", L"Effect\\Blossom.png");// 재질을 설정한다.
		SAFE_RELEASE(pRenderer);

		//컴포넌트추가
		CDx11SnowUI* pSnowUISc = (CDx11SnowUI*)pSnow->AddComponent<CDx11SnowUI>("BlossomUIScript");
		pSnowUISc->SetBarPos((rand() % 1250 + 50), (rand() % 700 + 1), 0);
		random = (rand() % 3 + 1);
		pSnowUISc->SetBarSize(random, random);
		SAFE_RELEASE(pSnowUISc);

		//컴포넌트추가
		CSnow*	pSnowSc = (CSnow*)pSnow->AddComponent<CSnow>("SnowScript");
		SAFE_RELEASE(pSnowSc);

		//레이어에 추가
		CDx11Layer*	pLayer = m_pScene->FindLayer("UILayer");

		if (!pLayer)
			return false;
		pLayer->AddObject(pSnow);

		SAFE_RELEASE(pSnow);
		SAFE_RELEASE(pLayer);
	}

	////사이즈 조절한 눈(벚꽃)
	for (int i = 0; i < 35; i++) {
		CDx11GameObject* pSnow = CDx11GameObject::Create("BlossomClone");

		//컴포넌트추가
		CDx11Renderer*	 pRenderer = pSnow->AddComponent<CDx11Renderer>("BlossomRenderer");
		pRenderer->SetMesh(RECT_UI_MESH);
		pRenderer->SetShader(UI_SHADER);
		pRenderer->SetRenderState(DX11_RS_ALPHABLEND);
		pRenderer->CreateMaterial("Blossom", L"Effect\\Blossom.png");// 재질을 설정한다.
		SAFE_RELEASE(pRenderer);

		//컴포넌트추가
		CDx11SnowUI* pSnowUISc = (CDx11SnowUI*)pSnow->AddComponent<CDx11SnowUI>("BlossomUIScript");
		pSnowUISc->SetBarPos((rand() % 1250 + 50), (rand() % 700 + 1), 0);
		random = (rand() % 15 + 3);
		pSnowUISc->SetBarSize(random, random);
		SAFE_RELEASE(pSnowUISc);

		//컴포넌트추가
		CBlossomWhite*	pSnowSc = (CBlossomWhite*)pSnow->AddComponent<CBlossomWhite>("SnowScript");
		SAFE_RELEASE(pSnowSc);

		//레이어에 추가
		CDx11Layer*	pLayer = m_pScene->FindLayer("UILayer");

		if (!pLayer)
			return false;
		pLayer->AddObject(pSnow);

		SAFE_RELEASE(pSnow);
		SAFE_RELEASE(pLayer);
	}
	//사이즈 조절한 눈2(벚꽃)
	for (int i = 0; i < 5; i++) {
		CDx11GameObject* pSnow = CDx11GameObject::Create("BlossomClone");

		//컴포넌트추가
		CDx11Renderer*	 pRenderer = pSnow->AddComponent<CDx11Renderer>("BlossomRenderer");
		pRenderer->SetMesh(RECT_UI_MESH);
		pRenderer->SetShader(UI_SHADER);
		pRenderer->SetRenderState(DX11_RS_ALPHABLEND);
		pRenderer->CreateMaterial("Blossom", L"Effect\\Blossom.png");// 재질을 설정한다.
		SAFE_RELEASE(pRenderer);

		//컴포넌트추가
		CDx11SnowUI* pSnowUISc = (CDx11SnowUI*)pSnow->AddComponent<CDx11SnowUI>("BlossomUIScript");
		pSnowUISc->SetBarPos((rand() % 1250 + 50), (rand() % 700 + 1), 0);
		random = (rand() % 14 + 4);
		pSnowUISc->SetBarSize(random, random);
		SAFE_RELEASE(pSnowUISc);

		//컴포넌트추가
		CSnow*	pSnowSc = (CSnow*)pSnow->AddComponent<CSnow>("SnowScript");
		SAFE_RELEASE(pSnowSc);

		//레이어에 추가
		CDx11Layer*	pLayer = m_pScene->FindLayer("UILayer");

		if (!pLayer)
			return false;
		pLayer->AddObject(pSnow);

		SAFE_RELEASE(pSnow);
		SAFE_RELEASE(pLayer);
	}
	//사이즈 조절한 스타
	for (int i = 0; i < 4; i++) {
		CDx11GameObject* pSnow = CDx11GameObject::Create("Blossom_WhiteClone");

		//컴포넌트추가
		CDx11Renderer*	 pRenderer = pSnow->AddComponent<CDx11Renderer>("Blossom_WhiteRenderer");
		pRenderer->SetMesh(RECT_UI_MESH);
		pRenderer->SetShader(UI_SHADER);
		pRenderer->SetRenderState(DX11_RS_ALPHABLEND);
		pRenderer->CreateMaterial("Blossom_White", L"Effect\\Blossom_White.png");// 재질을 설정한다.
		SAFE_RELEASE(pRenderer);

		//컴포넌트추가
		CDx11SnowUI* pSnowUISc = (CDx11SnowUI*)pSnow->AddComponent<CDx11SnowUI>("Blossom_WhiteUIScript");
		pSnowUISc->SetBarPos((rand() % 1250 + 50), (rand() % 700 + 1), 0);
		random = (rand() % 5 + 1);
		pSnowUISc->SetBarSize(random, random);
		SAFE_RELEASE(pSnowUISc);

		//컴포넌트추가
		CStar*	pSnowSc = (CStar*)pSnow->AddComponent<CStar>("Blossom_WhiteScript");
		SAFE_RELEASE(pSnowSc);

		//레이어에 추가
		CDx11Layer*	pLayer = m_pScene->FindLayer("UILayer");

		if (!pLayer)
			return false;
		pLayer->AddObject(pSnow);

		SAFE_RELEASE(pSnow);
		SAFE_RELEASE(pLayer);
	}

	//화이트 벛꽃
	for (int i = 0; i < 15; i++) {
		CDx11GameObject* pSnow = CDx11GameObject::Create("Blossom_WhiteClone");

		//컴포넌트추가
		CDx11Renderer*	 pRenderer = pSnow->AddComponent<CDx11Renderer>("Blossom_WhiteRenderer");
		pRenderer->SetMesh(RECT_UI_MESH);
		pRenderer->SetShader(UI_SHADER);
		pRenderer->SetRenderState(DX11_RS_ALPHABLEND);
		pRenderer->CreateMaterial("star", L"Effect\\Blossom_White.png");// 재질을 설정한다.
		SAFE_RELEASE(pRenderer);

		//컴포넌트추가
		CDx11SnowUI* pSnowUISc = (CDx11SnowUI*)pSnow->AddComponent<CDx11SnowUI>("Blossom_WhiteUIScript");
		pSnowUISc->SetBarPos((rand() % 1250 + 50), (rand() % 700 + 1), 0);
		random = (rand() % 16 + 6);
		pSnowUISc->SetBarSize(random, random);
		SAFE_RELEASE(pSnowUISc);

		//컴포넌트추가
		CBlossomWhite*	pSnowSc = (CBlossomWhite*)pSnow->AddComponent<CBlossomWhite>("Blossom_WhiteScript");
		SAFE_RELEASE(pSnowSc);

		//레이어에 추가
		CDx11Layer*	pLayer = m_pScene->FindLayer("UILayer");

		if (!pLayer)
			return false;
		pLayer->AddObject(pSnow);

		SAFE_RELEASE(pSnow);
		SAFE_RELEASE(pLayer);
	}
	return 0;
}

int CMainScene::CreateMonster(float fTime)
{
	CDx11GameObject*	pMonster = CDx11GameObject::Create("MonsterPrototype", OT_CLONE);
	CDx11Transform*		pTransform = pMonster->GetTransform();
	//	CDx11Transform*		pMonsterTr = pMonster->GetTransform();

	pTransform->SetWorldScale(0.025f, 0.025f, 0.025f);
	//	pTransform->SetLocalRotY(-DX11_PI / 2.f);
	//	SAFE_RELEASE(pMonsterTr);

	CDx11GameObject*	pPlayerObj = m_pScene->FindGameObjectFromTag("PlayerObject");
	CDx11Transform*		pPlayerTr = pPlayerObj->GetTransform();

	Vec3	vPos = pPlayerTr->GetWorldPos();
	pTransform->SetWorldPos(vPos.x + (rand() % 801) / 20.f - 20.f, vPos.y + 5.f,
		vPos.z + (rand() % 801) / 20.f - 20.f);

	SAFE_RELEASE(pPlayerTr);
	SAFE_RELEASE(pPlayerObj);
	SAFE_RELEASE(pTransform);

	CDx11Layer*	pLayer = m_pScene->FindLayer("DefaultLayer");
	if (!pLayer)
		return false;
	pLayer->AddObject(pMonster);
	++m_iMonsterCount;

	SAFE_RELEASE(pMonster);
	SAFE_RELEASE(pLayer);

	return 0;
}


void CMainScene::MovePlayer(BYTE * pPacket)
{
	CDx11Transform * pTr = m_pMyPlayer->GetTransform();
	CZEROStreamSP	stream;
	Vec3		vPos;

	stream->SetBuffer(pPacket);
	stream->ReadFloat(&vPos.x);
	stream->ReadFloat(&vPos.y);
	stream->ReadFloat(&vPos.z);

	pTr->SetWorldPos(vPos);
	SAFE_RELEASE(pTr);
}

void CMainScene::MoveOtherPlayer(BYTE * pPacket)
{
	if (!m_pUserPlayer)
		return;

	CZEROStreamSP	stream;
	Vec3		vPos;

	stream->SetBuffer(pPacket);
	stream->ReadFloat(&vPos.x);
	stream->ReadFloat(&vPos.y);
	stream->ReadFloat(&vPos.z);
	/*
	if (vPos.x < -200 || vPos.x > 200 || vPos.z < -200 || vPos.z > 200)
	return;*/

	m_pUserTr->SetWorldPos(vPos);
	//SAFE_RELEASE(pTr);
}

void CMainScene::DieOtherPlayer(BYTE * pPacket)
{
	CZEROStreamSP	stream;
	INT64	diePlayer;

	stream->SetBuffer(pPacket);
	stream->ReadInt64(&diePlayer);

	if (!m_pUserPlayer)
		return;

	m_pUserPlayer->Death();
}

void CMainScene::RotateOtherPlayer(BYTE * pPacket)
{
	if (!m_pUserPlayer)
		return;

	CZEROStreamSP	stream;
	Vec3		vRot;

	stream->SetBuffer(pPacket);
	stream->ReadFloat(&vRot.x);
	stream->ReadFloat(&vRot.y);
	stream->ReadFloat(&vRot.z);

	m_pUserTr->SetWorldRot(vRot.x, vRot.y + DX11_PI, vRot.z);
}

void CMainScene::JumpOtherPlayer(BYTE * pPacket)
{
	if (!m_pUserPlayer)
		return;

	CZEROStreamSP	stream;
	Vec3		vPos;

	stream->SetBuffer(pPacket);
	stream->ReadFloat(&vPos.x);
	stream->ReadFloat(&vPos.y);
	stream->ReadFloat(&vPos.z);

	m_pUserTr->SetWorldPos(vPos);
}

void CMainScene::AnimOtherPlayer(BYTE * pPacket)
{
	if (!m_pUserPlayer)
		return;

	CZEROStreamSP   stream;
	int iState;

	stream->SetBuffer(pPacket);
	stream->ReadInt32(&iState);
	m_pUserPlayer->SetStateFlag(iState);
}

void CMainScene::ShootOtherPlayer(BYTE * pPacket)
{
	CDx11Layer*   pLayer = m_pScene->FindLayer("DefaultLayer");
	if (!pLayer)
		return;

	CZEROStreamSP   stream;

	stream->SetBuffer(pPacket);

	Vec3      vPos;
	Vec3      vScale;
	Vec3      vDir;

	stream->ReadFloat(&vPos.x);
	stream->ReadFloat(&vPos.y);
	stream->ReadFloat(&vPos.z);

	stream->ReadFloat(&vScale.x);
	stream->ReadFloat(&vScale.y);
	stream->ReadFloat(&vScale.z);

	stream->ReadFloat(&vDir.x);
	stream->ReadFloat(&vDir.y);
	stream->ReadFloat(&vDir.z);


	CDx11GameObject*	pLaser = CDx11GameObject::Create("LaserObject");
	CDx11Transform*		pLaserTr = pLaser->GetTransform();
	pLaserTr->Copy(m_pUserTr);
	pLaserTr->SetWorldScale(vScale);
	pLaserTr->SetWorldPos(vPos);
	pLaserTr->SetWorldView(vDir);

	// 렌더러를 생성한다.
	CDx11Renderer* pRenderer = pLaser->AddComponent<CDx11Renderer>("LaserRenderer");
	pRenderer->SetMesh("MissileMesh");
	pRenderer->SetShader(DEFAULT_BUMP_SHADER);
	SAFE_RELEASE(pRenderer);

	// 총알 스크립트를 컴포넌트에 추가한다.
	CLaser*	pLaserSc = (CLaser*)pLaser->AddComponent<CLaser>("LaserScript");
	//SAFE_RELEASE(pLaserSc);

	// 충돌체를 설정한다.
	CDx11ColliderOBB* pOBB = pLaser->AddComponent<CDx11ColliderOBB>("UserLaserColl");
	float fAxisLen[3];
	fAxisLen[0] = 0.5f;
	fAxisLen[1] = 1.f;
	fAxisLen[2] = 0.5f;
	pOBB->SetOBBInfo(fAxisLen, pLaserTr);
	//	SAFE_RELEASE(pLaserTr);
	//	SAFE_RELEASE(pOBB);
	pLayer->AddObject(pLaser);

	SAFE_RELEASE(pLayer);

}

void CMainScene::HeatPlayer(BYTE * pPacket)
{
	if (!m_pMyPlayer)
		return;

	CDx11Layer*	pLayer = m_pScene->FindLayer("DefaultLayer");

	if (!pLayer)
		return;

	CPlayer* pPlayer = (CPlayer*)m_pMyPlayer->FindComponentFromTag("PlayerScript");
	CLaser*	pLaser = (CLaser*)m_pUserPlayer->FindComponentFromType(CT_SCRIPT);

	pLaser->DeathMotion();
	//pLaser->Death();
	pPlayer->SubtractHP(1);
	pPlayer->ShakeCamera();

	SAFE_RELEASE(pLaser);
	SAFE_RELEASE(pPlayer);
	SAFE_RELEASE(pLayer);

}

void CMainScene::CreateBox(BYTE * pPacket)
{
	CZEROStreamSP	stream;

	stream->SetBuffer(pPacket);
	Vec3		vPos;
	Vec3		vScale;
	stream->ReadFloat(&vPos.x);
	stream->ReadFloat(&vPos.y);
	stream->ReadFloat(&vPos.z);

	stream->ReadFloat(&vScale.x);
	stream->ReadFloat(&vScale.y);
	stream->ReadFloat(&vScale.z);

	CDx11Layer*	pLayer = m_pScene->FindLayer("DefaultLayer");
	if (!pLayer)
		return;

	CDx11GameObject*	pBox = CDx11GameObject::Create("BoxPrototype");


	CDx11Renderer*	pRenderer = pBox->AddComponent<CDx11Renderer>("BoxRenderer");
	pRenderer->SetMesh("BoxMesh");
	pRenderer->SetShader(DEFAULT_BUMP_SHADER);
	SAFE_RELEASE(pRenderer);

	CDx11Transform *pTr = pBox->GetTransform();
	pTr->SetWorldScale(vScale);
	pTr->SetWorldPos(vPos);
	//pTr->SetWorldRot(0.f, 0.f, DX11_PI / 6.f);
	pTr->SetLocalRot(0.f, 0.f, DX11_PI / 6.f);
	SAFE_RELEASE(pTr);

	CItem*		pBoxSc = (CItem*)pBox->AddComponent<CItem>("BoxScript");
	SAFE_RELEASE(pBoxSc);

	CDx11ColliderOBB* pOBB = pBox->AddComponent<CDx11ColliderOBB>("BoxColl");
	pTr = pBox->GetTransform();
	float fAxisLen[3];
	fAxisLen[0] = 2.0f;
	fAxisLen[1] = 2.0f;
	fAxisLen[2] = 2.0f;
	//	pOBB->SetPivot(Vec3(-1.2f, 0.0f, 12.f));
	pOBB->SetOBBInfo(fAxisLen, pTr);
	SAFE_RELEASE(pTr);
	SAFE_RELEASE(pOBB);

	pLayer->AddObject(pBox);

	SAFE_RELEASE(pBox);
	SAFE_RELEASE(pLayer);
}

void CMainScene::LeftCarMove(BYTE * pPacket)
{
	CZEROStreamSP   stream;

	stream->SetBuffer(pPacket);
	Vec3      vPos;
	Vec3      vPos2;

	stream->ReadFloat(&vPos.x);
	stream->ReadFloat(&vPos.y);
	stream->ReadFloat(&vPos.z);

	stream->ReadFloat(&vPos2.x);
	stream->ReadFloat(&vPos2.y);
	stream->ReadFloat(&vPos2.z);

	m_pLeftCarTr->SetWorldPos(vPos.x, vPos.y - 1.6f, vPos.z + 10);
	m_pLeftCarTr2->SetWorldPos(vPos2.x - 100.f, vPos2.y + 6.f, vPos2.z + 10);

}

void CMainScene::RightCarMove(BYTE * pPacket)
{
	CZEROStreamSP   stream;

	stream->SetBuffer(pPacket);
	Vec3      vPos;
	Vec3      vPos2;

	stream->ReadFloat(&vPos.x);
	stream->ReadFloat(&vPos.y);
	stream->ReadFloat(&vPos.z);

	stream->ReadFloat(&vPos2.x);
	stream->ReadFloat(&vPos2.y);
	stream->ReadFloat(&vPos2.z);

	m_pRightCarTr->SetWorldPos(vPos.x, vPos.y + 0.75f, vPos.z - 10);
	m_pRightCarTr2->SetWorldPos(vPos2.x + 200.f, vPos2.y + 6.f, vPos2.z - 7);
}

void CMainScene::WaitText()
{
	CDx11GameObject* pText = CDx11GameObject::Create("WaitText");

	//컴포넌트추가
	CDx11Renderer*    pRenderer = pText->AddComponent<CDx11Renderer>("pTextRenderer");
	pRenderer->SetMesh(RECT_UI_MESH);
	pRenderer->SetShader(UI_SHADER);
	pRenderer->SetRenderState(DX11_RS_ALPHABLEND);
	pRenderer->CreateMaterial("WaitText", L"Effect\\Wait.png");// 재질을 설정한다.
	SAFE_RELEASE(pRenderer);


	//컴포넌트추가
	float mapSize = 300.f;
	float mapHalfSize = 200.f;
	CDx11SnowUI* pSnowUISc = (CDx11SnowUI*)pText->AddComponent<CDx11SnowUI>("WaitUIScript");

	RESOLUTION_TYPE eRS = DX11_GET_SINGLE(CDx11Device)->GetResolutionType();
	pSnowUISc->SetBarPos(g_tRS[eRS].iWidth / 2 - 150.f, -80.f, 0.f);
	pSnowUISc->SetBarSize(mapSize, mapSize);
	SAFE_RELEASE(pSnowUISc);

	//레이어에 추가
	CDx11Layer*   pLayer = m_pScene->FindLayer("UILayer");

	if (!pLayer)
		return;
	pLayer->AddObject(pText);
	SAFE_RELEASE(pText);
	SAFE_RELEASE(pLayer);
}

void CMainScene::ResultShow()
{
	CDx11GameObject*	pPlayerObj = m_pScene->FindGameObjectFromTag("PlayerObject");
	CDx11GameObject*	pUserObj = m_pScene->FindGameObjectFromTag("UserObject");

	if (pPlayerObj) {
		CPlayer*	pPlayerComponent = (CPlayer*)pPlayerObj->FindComponentFromTag("PlayerScript");
		if (pPlayerComponent) {
			if (pPlayerComponent->GetHP() <= 0) {

				if (m_Count == 0) {
					m_Count++;
					DX11_GET_SINGLE(Dx11SoundManager)->PlayResult();
				}

				CDx11Layer*	pLayer = m_pScene->FindLayer("UILayer");
				if (!pLayer)
					return;

				CDx11GameObject* pResult = CDx11GameObject::Create("Result");

				//컴포넌트추가
				CDx11Renderer*	 pRenderer = pResult->AddComponent<CDx11Renderer>("ResultRenderer");
				pRenderer->SetMesh(RECT_UI_MESH);
				pRenderer->SetShader(UI_SHADER);
				pRenderer->SetRenderState(DX11_RS_DEPTH_DISABLE);
				pRenderer->CreateMaterial("DEFECT", L"UI\\DEFECT.png");// 재질을 설정한다.
				SAFE_RELEASE(pRenderer);

				//컴포넌트추가1
				RESOLUTION_TYPE eRS = DX11_GET_SINGLE(CDx11Device)->GetResolutionType();
				CDx11SnowUI* pSnowUISc = (CDx11SnowUI*)pResult->AddComponent<CDx11SnowUI>("MinimapUIScript");
				pSnowUISc->SetBarSize(g_tRS[eRS].iWidth, g_tRS[eRS].iHeight);
				pSnowUISc->SetBarPos(0, 0, 0);
				SAFE_RELEASE(pSnowUISc);

				pLayer->AddObject(pResult);
				SAFE_RELEASE(pResult);
				SAFE_RELEASE(pLayer);

			}
		}
		SAFE_RELEASE(pPlayerComponent);
	}
	if (pUserObj) {
		CUser*	pUserComponent = (CUser*)pUserObj->FindComponentFromTag("User");
		if (pUserComponent) {
			if (pUserComponent->GetHP() <= 0) {

				if (m_Count == 0) {
					m_Count++;
					DX11_GET_SINGLE(Dx11SoundManager)->PlayResult();
				}

				CDx11Layer*	pLayer = m_pScene->FindLayer("UILayer");
				if (!pLayer)
					return;

				CDx11GameObject* pResult = CDx11GameObject::Create("Result");

				//컴포넌트추가
				CDx11Renderer*	 pRenderer = pResult->AddComponent<CDx11Renderer>("ResultRenderer");
				pRenderer->SetMesh(RECT_UI_MESH);
				pRenderer->SetShader(UI_SHADER);
				pRenderer->SetRenderState(DX11_RS_DEPTH_DISABLE);
				pRenderer->CreateMaterial("win", L"UI\\win.png");// 재질을 설정한다.
				SAFE_RELEASE(pRenderer);

				//컴포넌트추가1
				RESOLUTION_TYPE eRS = DX11_GET_SINGLE(CDx11Device)->GetResolutionType();
				CDx11SnowUI* pSnowUISc = (CDx11SnowUI*)pResult->AddComponent<CDx11SnowUI>("MinimapUIScript");
				pSnowUISc->SetBarSize(g_tRS[eRS].iWidth, g_tRS[eRS].iHeight);
				pSnowUISc->SetBarPos(0, 0, 0);
				SAFE_RELEASE(pSnowUISc);

				pLayer->AddObject(pResult);
				SAFE_RELEASE(pResult);
				SAFE_RELEASE(pLayer);

			}
		}
		SAFE_RELEASE(pUserComponent);
	}

	SAFE_RELEASE(pPlayerObj);
	SAFE_RELEASE(pUserObj);

}

void CMainScene::LoadResource()
{

	// 매쉬 로딩	
	CDx11Mesh*	 pMesh = DX11_GET_SINGLE(CDx11ResourcesManager)->LoadMesh("BoxMesh", L"Cube1.FBX");
	SAFE_RELEASE(pMesh);

	pMesh = DX11_GET_SINGLE(CDx11ResourcesManager)->LoadMesh("MissileMesh", L"bullet\\missile2.FBX");
	SAFE_RELEASE(pMesh);

	pMesh = DX11_GET_SINGLE(CDx11ResourcesManager)->LoadMesh("BulletMesh", L"bullet\\missile.FBX");
	SAFE_RELEASE(pMesh);

	pMesh = DX11_GET_SINGLE(CDx11ResourcesManager)->LoadMesh("PlayerMesh", L"FutureSoldier_RED.FBX");
	SAFE_RELEASE(pMesh);

}
