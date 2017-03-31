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

CMainScene::CMainScene()	:
	m_iMonsterCount(0)
{
}

CMainScene::~CMainScene()
{
}

int CMainScene::GetMonsterCount() const
{
	return m_iMonsterCount;
}

bool CMainScene::Init()
{
	// 텍스처 로딩
//	CDx11Texture*	pTexture = DX11_GET_SINGLE(CDx11ResourcesManager)->LoadTexture("rose", L"rose.jpg");
//	SAFE_RELEASE(pTexture);

	// 매쉬 로딩
	CDx11Mesh*	pMesh = DX11_GET_SINGLE(CDx11ResourcesManager)->LoadMesh("PlayerMesh", L"FutureSoldier.FBX");
	SAFE_RELEASE(pMesh);

	pMesh = DX11_GET_SINGLE(CDx11ResourcesManager)->LoadMesh("MonsterMesh", L"android2.FBX");
	SAFE_RELEASE(pMesh);

	pMesh = DX11_GET_SINGLE(CDx11ResourcesManager)->LoadMesh("MissileMesh", L"bullet\\missile2.FBX");
	SAFE_RELEASE(pMesh);

	pMesh = DX11_GET_SINGLE(CDx11ResourcesManager)->LoadMesh("BulletMesh", L"bullet\\missile.FBX");
	SAFE_RELEASE(pMesh);

	//pMesh = DX11_GET_SINGLE(CDx11ResourcesManager)->LoadMesh("MapMesh", L"65.FBX");
	//SAFE_RELEASE(pMesh);




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




	//맵FBX MeshInfo
	CDx11GameObject* pMapInfo = CDx11GameObject::Create("MapInfoObject");
	pLayer->AddObject(pMapInfo);//레이어에 추가(씬확인 때문에 먼저해줌)
	CMapInfo*	pMapInfoScript = pMapInfo->AddComponent<CMapInfo>("MapInfo");
	SAFE_RELEASE(pMapInfoScript);
	SAFE_RELEASE(pMapInfo);

	//눈
	CreateSnow();

	// 조명 생성
	CDx11GameObject*	pLightObj = m_pScene->CreateLight(LT_DIR, "Light");
	CDx11Transform*		pLightTr = pLightObj->GetTransform();
	pLightTr->RotationWorld(DX11_PI / 3.f, 1.f, AXIS_X);
	SAFE_RELEASE(pLightTr);
	SAFE_RELEASE(pLightObj);


	// 오브젝트 생성
	InitPlayer(pTerrain);
	InitBullet();
	InitMonster();
	InitEffect();
	SAFE_RELEASE(pTerrain);

	// TestUI
	//CDx11Layer*	pUILayer = m_pScene->FindLayer("UILayer");

	//CDx11GameObject*	pUIObj = CDx11GameObject::Create("UIObj");
	//CDx11Transform*		pTransform = pUIObj->GetTransform();
	//pTransform->SetWorldPos(590.f, 500.f, 0.f);
	//pTransform->SetWorldScale(100.f, 50.f, 1.f);
	//SAFE_RELEASE(pTransform);

	//CDx11Renderer* pRenderer = pUIObj->AddComponent<CDx11Renderer>("UIRenderer");
	//pRenderer->CreateMaterial("HPBar", L"Bar.png");
	//SAFE_RELEASE(pRenderer);
	//
	//CDx11UIBar*		pBar = pUIObj->AddComponent<CDx11UIBar>("UIBar");
	//pBar->SetBarSize(Vec2(100.f, 50.f));
	//pBar->SetMinMax(0.f, 10.f);
	//pBar->SetValue(10.f);
	//SAFE_RELEASE(pBar);

	//pUILayer->AddObject(pUIObj);
	//SAFE_RELEASE(pUIObj);

	//SAFE_RELEASE(pUILayer);


	SAFE_RELEASE(pLayer);
	return true;
}

void CMainScene::Input(float fTime)
{
}

int CMainScene::Update(float fTime)
{
	static string	str = "MainCamera";
	static bool		bReturn;

	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		bReturn = true;
	}
	else if (bReturn)
	{
		bReturn = false;
		m_pScene->ChangeCamera(str);

		if (str == "PlayerCamera")
			str = "MainCamera";

		else
			str = "PlayerCamera";
	}


	static bool bCollRender;

	if (GetAsyncKeyState(VK_F1) & 0x8000)
		bCollRender = true;
	else if (bCollRender)
	{
		DX11_GET_SINGLE(CDx11CollisionManager)->ColliderRenderEnable();
		bCollRender = false;
	}

	return 0;
}

int CMainScene::LateUpdate(float fTime)
{
	return 0;
}

void CMainScene::Render(float fTime)
{
}

CMainScene * CMainScene::Clone()
{
	return new CMainScene(*this);
}

void CMainScene::InitPlayer(CDx11Terrain* pTerrain)
{
	//만들어놨던 디폴트 레이어를 찾는다.
	CDx11Layer*	pLayer = m_pScene->FindLayer("DefaultLayer");
	if (!pLayer)
		return;

	// 플레이어 객체를 생성한다.
	CDx11GameObject*	pPlayer = CDx11GameObject::Create("PlayerObject");
	pLayer->AddObject(pPlayer);

	CDx11Transform*		pTr = pPlayer->GetTransform();
//	pTr->SetWorldScale(0.03f, 0.03f, 0.03f);
	pTr->SetWorldScale(0.01f, 0.01f, 0.01f);
	pTr->SetLocalRotY(-DX11_PI);
//	pTr->SetLocalRotX(-DX11_PI / 2.f);
	

	// 렌더러를 생성한다.
	CDx11Renderer*	pRenderer = pPlayer->AddComponent<CDx11Renderer>("PlayerRenderer");
	pRenderer->SetMesh("PlayerMesh");
	pRenderer->SetShader(DEFAULT_BUMP_SHADER);
	SAFE_RELEASE(pRenderer);

	// 플레이어 스크립트를 컴포넌트에 추가한다.
	CPlayer*	pPlayerScript = pPlayer->AddComponent<CPlayer>("PlayerScript");
	if(pTerrain) pPlayerScript->SetTerrain(pTerrain);
	SAFE_RELEASE(pPlayerScript);


	// 플레이어 충돌체 설정.
	CDx11ColliderOBB*	pOBB = pPlayer->AddComponent<CDx11ColliderOBB>("PlayerColl");
	float fAxisLen[3];
	fAxisLen[0] = 0.4f;
	fAxisLen[1] = 0.4f;
	fAxisLen[2] = 0.4f;
	pOBB->SetPivot(Vec3(0.f, 0.4f, 0.f));
	pOBB->SetOBBInfo(fAxisLen, pTr);
	
	SAFE_RELEASE(pOBB);

	CDx11ColliderSphere*	pSphere = pPlayer->AddComponent<CDx11ColliderSphere>("PlayerCollSphere");
	pSphere->SetInfo(pTr->GetWorldPos() , 1.f);
	SAFE_RELEASE(pSphere);

	SAFE_RELEASE(pTr);

	



	// 플레이어 카메라를 생성한다.
	CDx11GameObject*	pCameraObj = m_pScene->CreateCamera("PlayerCamera", DX11_PI / 3.f, 0.1f, 1000.f);
	CDx11Transform*		pCameraTr = pCameraObj->GetTransform();
	CDx11Camera*		pCam = (CDx11Camera*)pCameraObj->FindComponentFromTypeName("CDx11Camera");

	pCam->SetTarget(pPlayer);
	pCam->SetCameraType(CVT_THIRD);
	if (pTerrain) pCam->SetTerrain(pTerrain);
	m_pScene->ChangeCamera("PlayerCamera");

	SAFE_RELEASE(pCam);
	SAFE_RELEASE(pCameraTr);
	SAFE_RELEASE(pCameraObj);
	

	SAFE_RELEASE(pPlayer);
	SAFE_RELEASE(pLayer);

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
	fAxisLen[0] = 0.2f;
	fAxisLen[1] = 0.2f;
	fAxisLen[2] = 0.2f;
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
	fAxisLen[0] = 0.1f;
	fAxisLen[1] = 0.2f;
	fAxisLen[2] = 0.1f;
	pOBB->SetOBBInfo(fAxisLen, pTr);
	SAFE_RELEASE(pTr);
	SAFE_RELEASE(pOBB);


	// 레이저 프로토타입을 만든다.
	CDx11GameObject*	pLaser = CDx11GameObject::Create("LaserPrototype", OT_PROTOTYPE);

	// 렌더러를 생성한다.
	pRenderer = pLaser->AddComponent<CDx11Renderer>("LaserRenderer");
	pRenderer->SetMesh(PYRAMID_MESH);
	pRenderer->SetShader(DEFAULT_SHADER);
	SAFE_RELEASE(pRenderer);

	// 총알 스크립트를 컴포넌트에 추가한다.
	CLaser*	pLaserSc = (CLaser*)pLaser->AddComponent<CLaser>("LaserScript");
	SAFE_RELEASE(pLaserSc);

	// 충돌체를 설정한다.
	pOBB = pLaser->AddComponent<CDx11ColliderOBB>("LaserColl");
	pTr = pLaser->GetTransform();
	fAxisLen[0] = 0.1f;
	fAxisLen[1] = 0.2f;
	fAxisLen[2] = 0.1f;
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
	fAxisLen[0] = 0.3f;
	fAxisLen[1] = 0.3f;
	fAxisLen[2] = 0.3f;
	pOBB->SetPivot(Vec3(0.f, 0.3f, 0.f));
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
	pEffectTr->SetWorldScale(3.f, 3.f, 3.f);
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
		TCHAR	strPath[MAX_PATH] = {};
		wsprintf(strPath, L"Effect\\Explosion\\320x240\\Explosion%d.png", i);
		pAnimation2D->AddFrameTexture(strKey, strPath);

	}
	SAFE_RELEASE(pAnimation2D);



	CDx11GameObject*	pSmallEffectObj = CDx11GameObject::Create("SmallEffect", OT_PROTOTYPE);
	pEffectTr = pSmallEffectObj->GetTransform();
	pEffectTr->SetWorldScale(1.f, 1.f, 1.f);
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
		TCHAR	strPath[MAX_PATH] = {};
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

int CMainScene::CreateSnow()
{
	//프로토 타입
	CDx11GameObject* pSnow = CDx11GameObject::Create("SnowPrototype", OT_PROTOTYPE);


	//눈 클론 100개 
	for (int i = 0; i < 100; i++) {
		CDx11GameObject* pSnow = CDx11GameObject::Create("SnowClone", OT_CLONE);

		//컴포넌트추가
		CDx11Renderer*	 pRenderer = pSnow->AddComponent<CDx11Renderer>("SnowRenderer");
		pRenderer->SetMesh(RECT_UI_MESH);
		pRenderer->SetShader(UI_SHADER);
		pRenderer->SetRenderState(DX11_RS_ALPHABLEND);
		pRenderer->CreateMaterial("snow", L"Effect\\snow.png");// 재질을 설정한다.
		SAFE_RELEASE(pRenderer);

		//컴포넌트추가
		CDx11SnowUI*	pSnowUISc = (CDx11SnowUI*)pSnow->AddComponent<CDx11SnowUI>("SnowUIScript");
		pSnowUISc->SetBarPos((rand() % 1501 + 1), (rand() % 1001 + 1), 0);
		pSnowUISc->SetBarSize(10.f, 10.f);
		SAFE_RELEASE(pSnowUISc);

		//컴포넌트추가
		CSnow*	pSnowSc = (CSnow*)pSnow->AddComponent<CSnow>("SnowScript");
		SAFE_RELEASE(pSnowSc);

		//레이어에 추가
		CDx11Layer*	pLayer = m_pScene->FindLayer("UILayer");
		pLayer;
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

	pTransform->SetWorldScale(0.005f, 0.005f, 0.005f);
//	pTransform->SetLocalRotY(-DX11_PI / 2.f);
//	SAFE_RELEASE(pMonsterTr);
	
	CDx11GameObject*	pPlayerObj = m_pScene->FindGameObjectFromTag("PlayerObject");
	CDx11Transform*		pPlayerTr = pPlayerObj->GetTransform();

	Vec3	vPos = pPlayerTr->GetWorldPos();
	pTransform->SetWorldPos(vPos.x + (rand() % 801) / 100.f - 4.f, vPos.y + (rand() % 201) / 100.f + 3.f,
		vPos.z + (rand() % 801) / 100.f - 4.f);

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
