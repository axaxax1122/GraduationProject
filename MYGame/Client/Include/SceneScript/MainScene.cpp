#include "MainScene.h"
#include "GameObject/Dx11GameObject.h"
#include "Component/Dx11Transform.h"
#include "Component/Dx11Renderer.h"
#include "Scene/Dx11Scene.h"
#include "Scene/Dx11Layer.h"
#include "../ObjectScript/Player.h"
#include "../ObjectScript/Bullet.h"
#include "../ObjectScript/Monster.h"
#include "../ObjectScript/Snow.h"
#include "Core/Dx11Scheduler.h"
#include "Resources/Dx11ResourcesManager.h"
#include "Resources/Dx11Texture.h"
#include "Component/Dx11Material.h"
#include "../ObjectScript/Skill2.h"
#include "Component/Dx11ColliderSphere.h"
#include "Component/Dx11ColliderHexahedron.h"
#include "../ObjectScript/Star.h"
#include "../ObjectScript/SubCamera.h"
#include "Component/Dx11UIBar.h"
#include "Component/Dx11UIBack.h"
#include "Component/Dx11SnowUI.h"
#include "Resources/Dx11Mesh.h"
#include "Component/Dx11BillBoard.h"
#include "Component/Dx11Animation2D.h"
#include "Component/Dx11Camera.h"
#include "Component/Dx11Camera3rd.h"
#include "../ObjectScript/MapInfo.h"

CMainScene::CMainScene()
{
}

CMainScene::~CMainScene()
{
	SAFE_RELEASE(m_pPlayer);
}

bool CMainScene::Init()
{
	CDx11Layer*	pLayer = m_pScene->FindLayer("DefaultLayer");
	if (!pLayer)
		return false;
	
	//---------------------------�÷��̾�(����)-----------------------------	
	CDx11GameObject* pPlayer = CDx11GameObject::Create("PlayerObject");

	//����
	m_pPlayer = pPlayer;
	m_pPlayer->AddRef();

	//�ؽ�Ʈ �ε�
	//CDx11Texture*	pTex = DX11_GET_SINGLE(CDx11ResourcesManager)->LoadTexture("Sword", L"Sword.png");
	//SAFE_RELEASE(pTex);

	//������Ʈ �߰�
	CDx11Renderer* pRenderer = pPlayer->AddComponent<CDx11Renderer>("PlayerRenderer");
	pRenderer->SetMesh(HEXAGON_TEX_MESH);
	pRenderer->SetShader(FOG_SHADER);
	pRenderer->CreateMaterial("Sword", L"Sword.png");// ������ �����Ѵ�.
	SAFE_RELEASE(pRenderer);

	//������Ʈ �߰�
	CDx11Transform* pPlayerTr = pPlayer->GetTransform();
	pPlayerTr->SetWorldScale(Vec3(1.f, 1.f, 1.f));	
	SAFE_RELEASE(pPlayerTr);

	//������Ʈ�߰�
	Dx11ColliderHexahedron* pHexahedron = pPlayer->AddComponent<Dx11ColliderHexahedron>("BulletColl");
	pHexahedron->SetInfo(Vec3Zero, 1.0f);
	SAFE_RELEASE(pHexahedron);

	////������Ʈ �߰�
	CPlayer*	pPlayerScript = pPlayer->AddComponent<CPlayer>("Player");
	SAFE_RELEASE(pPlayerScript);

	//���̾ �߰�
	pLayer->AddObject(pPlayer);

	//--------------------�÷��̾� �ڽ�(����,����)--------------------------
	CDx11GameObject* pPlayerChild = CDx11GameObject::Create("LeftChild");

	//�ؽ�Ʈ �ε�
	//pTex = DX11_GET_SINGLE(CDx11ResourcesManager)->LoadTexture("Child", L"Child.PNG");
	//SAFE_RELEASE(pTex);

	//������Ʈ �߰�
	pRenderer = pPlayerChild->AddComponent<CDx11Renderer>("PlayerChildRenderer");
	pRenderer->SetMesh(RECT_TEX_MESH);
	pRenderer->SetShader(PIXELCUT_SHADER);
	pRenderer->CreateMaterial("Child", L"Child.PNG");// ������ �����Ѵ�.
	pRenderer->SetRenderState(DX11_RS_ALPHABLEND);		// Alpha Blend
	SAFE_RELEASE(pRenderer);

	//������Ʈ �߰�
	CDx11Transform* pChildTr = pPlayerChild->GetTransform();
	pChildTr->MoveWorld(-2.f, 1.f, AXIS_X);
	pChildTr->SetWorldScale(0.5f, 0.5f, 1.f);
	//pChildTr->DeleteParentFlag(TPF_POS);
	SAFE_RELEASE(pChildTr);

	//�ڽĿ� �߰�(Ư��, ���⼭ �θ��� Ʈ�������� �����Ѵ�.)
	pPlayer->AddChild(pPlayerChild);
	SAFE_RELEASE(pPlayerChild);

	//--------------------------- �÷��̾� �ڽ�2(������) -------------------------
	pPlayerChild = CDx11GameObject::Create("RightChild");

	//�ؽ�Ʈ �ε�
	//pTex = DX11_GET_SINGLE(CDx11ResourcesManager)->LoadTexture("Over", L"Over.PNG");
	//SAFE_RELEASE(pTex);

	//������Ʈ �߰�
	pRenderer = pPlayerChild->AddComponent<CDx11Renderer>("PlayerChildRenderer");
	pRenderer->SetMesh(RECT_TEX_MESH);
	pRenderer->SetShader(DEFAULT_TEX_SHADER);
	pRenderer->CreateMaterial("Over", L"Over.PNG");// ������ �����Ѵ�.
	SAFE_RELEASE(pRenderer);

	//������Ʈ �߰�
	pChildTr = pPlayerChild->GetTransform();
	pChildTr->MoveWorld(1.f, 1.f, AXIS_X);
	pChildTr->SetWorldScale(0.5f, 0.5f, 1.f);
	SAFE_RELEASE(pChildTr);

	//�ڽĿ� �߰�
	pPlayer->AddChild(pPlayerChild);
	SAFE_RELEASE(pPlayerChild);
	SAFE_RELEASE(pPlayer);

	//------------------------- Main Move Camera -------------------------------
	
	CDx11GameObject*	pCamera = m_pScene->GetMainCamera();

	CDx11Camera*	pCam = (CDx11Camera*)pCamera->FindComponentFromTypeName("CDx11Camera");
	pCam->SetTarget(m_pPlayer);
	SAFE_RELEASE(pCam);

	CDx11Camera3rd*	p3rdCam = pCamera->AddComponent<CDx11Camera3rd>();
	SAFE_RELEASE(p3rdCam);
	
	SAFE_RELEASE(pCamera);	

	//----------------------------���� ī�޶�-----------------------------------	
	//CDx11GameObject* pSubCamera = m_pScene->CreateCamera("SubCamera1",
	//	DX11_PI / 3.f, 0.1f, 1000.f);

	////������Ʈ �߰�
	//pRenderer = pSubCamera->AddComponent<CDx11Renderer>("SubCameraRenderer");
	//pRenderer->SetMesh(SPHERE_MESH);
	//pRenderer->SetShader(DEFAULT_SHADER);
	//SAFE_RELEASE(pRenderer);

	////������Ʈ �߰�
	//pChildTr = pSubCamera->GetTransform();
	//pChildTr->MoveWorld(-3.f, 1.f, AXIS_Z);
	//SAFE_RELEASE(pChildTr);

	////�߰�	
	//pPlayer->AddChild(pSubCamera);//�θ� �����ϱ⶧���� �и��±���..
	//SAFE_RELEASE(pSubCamera);


	//---------------------------------�ٴ�-------------------------------------	
	CDx11GameObject*	pFloor = CDx11GameObject::Create("FloorObject");

	//������Ʈ �߰�
	pRenderer = pFloor->AddComponent<CDx11Renderer>("FloorRenderer");
	pRenderer->SetMesh(HEXAGON_TEX_MESH);
	pRenderer->SetShader(DEFAULT_TEX_SHADER);
	pRenderer->CreateMaterial("Earth", L"Earth.jpg");// ������ �����Ѵ�.
	SAFE_RELEASE(pRenderer);

	//������Ʈ �߰�
	CDx11Transform*	pFloorTr = pFloor->GetTransform();
	pFloorTr->MoveWorld(Vec3(0.f, -1.f, 0.f));
	pFloorTr->SetWorldScale(4.f, 0.3f, 4.f);
	SAFE_RELEASE(pFloorTr);

	//���̾ �߰�
	pLayer->AddObject(pFloor);
	SAFE_RELEASE(pFloor);

	//-------------------------- �Ѿ�(������Ÿ��) ------------------------------------
	CDx11GameObject* pBullet = CDx11GameObject::Create("BulletPrototype", OT_PROTOTYPE);

	//������Ʈ �߰�
	pRenderer = pBullet->AddComponent<CDx11Renderer>("BulletRenderer");
	pRenderer->SetMesh(PYRAMID_MESH);
	pRenderer->SetShader(DEFAULT_SHADER);
	SAFE_RELEASE(pRenderer);

	pHexahedron = pBullet->AddComponent<Dx11ColliderHexahedron>("BulletColl");
	pHexahedron->SetInfo(Vec3Zero, 0.5f);
	SAFE_RELEASE(pHexahedron);

	//������Ʈ �߰�
	CBullet*	pBulletSc = (CBullet*)pBullet->AddComponent<CBullet>("BulletScript");
	SAFE_RELEASE(pBulletSc);

	//----------------------------- ȯ��� ---------------------------------------------
	m_pScene->CreateSky("SkyTex", L"DesertSky.dds");

	//-----------------------------���� �Ѿ�----------------------------------------
	CDx11GameObject*	pMonsterBullet = CDx11GameObject::Create("MonsterBulletPrototype", OT_PROTOTYPE);

	//������Ʈ �߰�
	pRenderer = pMonsterBullet->AddComponent<CDx11Renderer>("MonsterBulletRenderer");
	pRenderer->SetMesh(SPHERE_MESH);
	pRenderer->SetShader(DEFAULT_SHADER);
	SAFE_RELEASE(pRenderer);

	//------------------------------�Ѿ�(��ų2)----------------------------------------
	//����(�߽� �� ������Ʈ)
	CDx11GameObject* pSkill2 = CDx11GameObject::Create("Skill2Prototype", OT_PROTOTYPE);

	//������Ʈ �߰�
	CSkill2*	pSkill2Sc = pSkill2->AddComponent<CSkill2>();
	SAFE_RELEASE(pSkill2Sc);

	//��ü(��ü ������Ʈ)
	CDx11GameObject* pSkill2Child = CDx11GameObject::Create("Skill2Child", OT_NORMAL);
	pChildTr = pSkill2Child->GetTransform();
	pChildTr->SetWorldPos(1.f, 0.f, 0.f);
	SAFE_RELEASE(pChildTr);

	//������Ʈ �߰�
	pRenderer = pSkill2Child->AddComponent<CDx11Renderer>("Skill2Renderer");
	pRenderer->SetMesh(TRIANGLE_MESH);
	pRenderer->SetShader(DEFAULT_SHADER);
	SAFE_RELEASE(pRenderer);

	//���ϵ忡 �߰�
	pSkill2->AddChild(pSkill2Child);
	SAFE_RELEASE(pSkill2Child);

	//-----------------------------�Ѿ�(��ų3<����ź>)---------------------------------
	CDx11GameObject*	pSkill3 = CDx11GameObject::Create("Skill3Prototype", OT_PROTOTYPE);

	//������Ʈ �߰�
	pRenderer = pSkill3->AddComponent<CDx11Renderer>("Skill3Renderer");
	pRenderer->SetMesh(TRIANGLE_MESH);
	pRenderer->SetShader(DEFAULT_SHADER);
	SAFE_RELEASE(pRenderer);

	//-------------------------------- ��(dds) ----------------------------------
	//CDx11GameObject* pStar = CDx11GameObject::Create("StarObject");

	////�ؽ��� �ε�
	////pTex = DX11_GET_SINGLE(CDx11ResourcesManager)->LoadTexture("WireFence", L"WireFence.dds");
	////SAFE_RELEASE(pTex);

	////������Ʈ �߰�
	//pRenderer = pStar->AddComponent<CDx11Renderer>("EnemyRenderer");
	//pRenderer->SetMesh(HEXAGON_TEX_MESH);
	//pRenderer->SetShader(PIXELCUT_SHADER);
	//pRenderer->SetRenderState(DX11_RS_NON_FACECULL);
	//pRenderer->SetRenderState(DX11_RS_ALPHABLEND);			//�ȼ� �ڸ���
	//pRenderer->CreateMaterial("WireFence", L"WireFence.dds");// ������ �����Ѵ�.	
	//SAFE_RELEASE(pRenderer);

	////������Ʈ�߰�
	//CStar*	pEnemyScript = pStar->AddComponent<CStar>("Star");
	//SAFE_RELEASE(pEnemyScript);

	////������Ʈ�߰�
	//CDx11Transform* pEnmTr = pStar->GetTransform();
	//pEnmTr->SetWorldPos(-2.f, 1.5f, 0.f);
	//pEnmTr->SetWorldView(Vec3(0, 1, 0));
	////pEnmTr->ScaleWorld(Vec3(0.5f, 0.5f, 0.5f));
	//SAFE_RELEASE(pEnmTr);

	//////���̾ �߰�	
	//pLayer->AddObject(pStar);
	//SAFE_RELEASE(pStar);	

	//////----------------------- ����(������Ÿ��) -----------------------------------
	//CDx11GameObject* pMonster = CDx11GameObject::Create("MonsterPrototype", OT_PROTOTYPE);

	////�ؽ�Ʈ �ε�
	////pTex = DX11_GET_SINGLE(CDx11ResourcesManager)->LoadTexture("Earth", L"Earth.jpg");
	////SAFE_RELEASE(pTex);
	//
	////������Ʈ�߰�
	//pRenderer = pMonster->AddComponent<CDx11Renderer>("MonsterRenderer");
	//pRenderer->SetMesh(HEXAGON_TEX_MESH);
	//pRenderer->SetShader(FOG_SHADER);
	//pRenderer->CreateMaterial("Earth", L"Earth.jpg");// ������ �����Ѵ�.
	//SAFE_RELEASE(pRenderer);

	////������Ʈ�߰�
	//pHexahedron = pMonster->AddComponent<Dx11ColliderHexahedron>("MonsterColl");
	//pHexahedron->SetInfo(Vec3Zero, 0.8f);
	//SAFE_RELEASE(pHexahedron);

	////������Ʈ�߰�
	//CMonster*	pMonsterSc = (CMonster*)pMonster->AddComponent<CMonster>("MonsterBulletScript");
	//SAFE_RELEASE(pMonsterSc);

	//////-------------------------- �� ��� ---------------------------------
	//CDx11GameObject* pSnow = CDx11GameObject::Create("SnowPrototype", OT_PROTOTYPE);
	//CreateSnow();
	//
	////-----------------------------Ÿ�̸� �����췯---------------------------------------
	//// ����
	//DX11_GET_SINGLE(CDx11Scheduler)->CreateSchedule(this, &CMainScene::CreateMonster,
	//	1.f, SO_LOOP);

	//// -------------------------- UI HP Slow Bar -----------------------------------------
	//CDx11GameObject*  pBarObj = CDx11GameObject::Create("HPSlowBar");
	//
	//// �������� �����Ѵ�.
	//pRenderer = pBarObj->AddComponent<CDx11Renderer>("HPBarSlowRenderer");
	//pRenderer->SetMesh(RECT_UI_MESH);
	//pRenderer->SetShader(UI_SHADER);
	//pRenderer->CreateMaterial("WhiteBar", L"WhiteBar.png");
	//pRenderer->SetRenderState(DX11_RS_DEPTH_DISABLE);
	//SAFE_RELEASE(pRenderer);
	//
	//// ������Ʈ �߰�
	//CDx11UIBar*	 pBar = pBarObj->AddComponent<CDx11UIBar>("UISlowBar");
	//pBar->SetBarSize(500.f, 40.f);//������
	//pBar->SetBarPos(400.f, 600.f);//��ġ
	//pBar->SetValueMinMax(0.f, 1000.f);//�ּ��ִ�
	//pBar->SetBarDir(UBD_LEFT);
	//
	//// ��� �߰�
	//pPlayer = m_pScene->FindObjectFromTag("PlayerObject");
	//if (pPlayer)
	//{
	//	pPlayerScript = (CPlayer*)pPlayer->FindComponentFromTag("Player");
	//	pPlayerScript->SetSlowHPBar(pBar);//����..Ŭ���� ����;;
	//	SAFE_RELEASE(pPlayerScript);
	//	SAFE_RELEASE(pPlayer);
	//}
	//
	//SAFE_RELEASE(pBar);
	//
	//// ���̾� �߰�
	//CDx11Layer*	 pUILayer = m_pScene->FindLayer("UILayer");
	//if (!pUILayer)
	//	return false;
	//pUILayer->AddObject(pBarObj);
	//
	//SAFE_RELEASE(pBarObj);
	//SAFE_RELEASE(pUILayer);
	//
	//// --------------------------------- UI HP Bar -------------------------------------
	//pBarObj = CDx11GameObject::Create("HPBar");
	//
	//// �������� �����Ѵ�.
	//pRenderer = pBarObj->AddComponent<CDx11Renderer>("HPBarRenderer");
	//pRenderer->SetMesh(RECT_UI_MESH);
	//pRenderer->SetShader(UI_SHADER);
	//pRenderer->CreateMaterial("BarDefault", L"Bar.png");
	//SAFE_RELEASE(pRenderer);
	//
	//// ������Ʈ �߰�
	//pBar = pBarObj->AddComponent<CDx11UIBar>("UIBar");
	//pBar->SetBarSize(500.f, 40.f);//������
	//pBar->SetBarPos(400.f, 600.f);//��ġ
	//pBar->SetValueMinMax(0.f, 1000.f);//�ּ��ִ�
	//pBar->SetBarDir(UBD_LEFT);
	//
	//// ��� �߰�
	//pPlayer = m_pScene->FindObjectFromTag("PlayerObject");
	//if (pPlayer)
	//{
	//	pPlayerScript = (CPlayer*)pPlayer->FindComponentFromTag("Player");
	//	pPlayerScript->SetHPBar(pBar);//����..Ŭ���� ����;;
	//	SAFE_RELEASE(pPlayerScript);
	//	SAFE_RELEASE(pPlayer);
	//}
	//
	//SAFE_RELEASE(pBar);
	//
	//// ���̾� �߰�
	//pUILayer = m_pScene->FindLayer("UILayer");
	//if (!pUILayer)
	//	return false;
	//pUILayer->AddObject(pBarObj);
	//SAFE_RELEASE(pBarObj);
	//SAFE_RELEASE(pUILayer);

	//------------------------------------  Sample Effect ����------------------------------------
	//CDx11GameObject*	pEffectObj = CDx11GameObject::Create("Effect");

	////�ؽ�Ʈ �ε�
	////pTex = DX11_GET_SINGLE(CDx11ResourcesManager)->LoadTexture("Effect", L"Effect.png");
	////SAFE_RELEASE(pTex);

	//CDx11Transform*	pEffectTr = pEffectObj->GetTransform();
	//pEffectTr->SetWorldScale(2.f, 2.f, 1.f);
	//pEffectTr->SetWorldPos(0.f, 3.f, 5.f);
	//SAFE_RELEASE(pEffectTr);
	//
	//pRenderer = pEffectObj->AddComponent<CDx11Renderer>("EffectRenderer");
	//pRenderer->SetMesh(EffectCenterMesh);
	//pRenderer->SetShader(EffectAtlasShader);
	//pRenderer->CreateMaterial("Effect", L"Effect.png");
	//pRenderer->SetRenderState(DX11_RS_ALPHABLEND, RST_BLEND);
	//SAFE_RELEASE(pRenderer);
	//
	//CDx11BillBoard*	pEffectBill = pEffectObj->AddComponent<CDx11BillBoard>("BillBoard");
	//SAFE_RELEASE(pEffectBill);
	//
	//CDx11Animation2D*	pAnimation2D = pEffectObj->AddComponent<CDx11Animation2D>("Animatino2D");
	//pAnimation2D->SetMaxFrame(5, 4);//5x4 ��� �ؽ���.
	//pAnimation2D->SetAnimation2DType(A2D_ATLAS_ALL);
	//pAnimation2D->SetAnimationLimitTime(0.5f);
	//SAFE_RELEASE(pAnimation2D);
	//
	//pLayer->AddObject(pEffectObj);
	//SAFE_RELEASE(pEffectObj);

	//----------------------------- ��FBX MeshInfo -------------------------------
	CDx11GameObject* pMapInfo = CDx11GameObject::Create("MapInfoObject");
	
	//���̾ �߰�(��Ȯ�� ������ ��������)
	pLayer->AddObject(pMapInfo);

	//������Ʈ �߰�
	CMapInfo*	pMapInfoScript = pMapInfo->AddComponent<CMapInfo>("MapInfo");
	SAFE_RELEASE(pMapInfoScript);	
	
	SAFE_RELEASE(pMapInfo);

	//-----------------------------------------------------------------------------
	SAFE_RELEASE(pLayer);//��

	return true;
}

void CMainScene::Input(float fTime)
{
}

int CMainScene::Update(float fTime)
{
	static string	str = "SubCamera1";
	static bool		bReturn;

	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		bReturn = true;
	}

	else if (bReturn)
	{
		bReturn = false;
		m_pScene->ChangeCamera(str);

		if (str == "SubCamera1")
			str = "MainCamera";

		else
			str = "SubCamera1";
	}

	return 0;
}

int CMainScene::LateUpdate(float fTime)
{

	/*CDx11Transform*	asdf = m_pScene->FindObjectFromTag("PlayerObject")->GetTransform();
	Vec3 gg11 = asdf->GetWorldPos();

	CDx11Transform*	pSubCameraObjTr = m_pScene->FindCamera("SubCamera1")->GetTransform();
	Vec3 asdf = pSubCameraObjTr->GetWorldPos();

	int a = 0;*/
	return 0;
}

void CMainScene::Render(float fTime)
{
}

CMainScene * CMainScene::Clone()
{
	return new CMainScene(*this);
}

int CMainScene::CreateMonster(float fTime)
{
	CDx11GameObject*	pMonster = CDx11GameObject::Create("MonsterPrototype", OT_CLONE);

	//������Ʈ�߰�
	CDx11Transform*	pTransform = pMonster->GetTransform();

	pTransform->SetWorldPos((rand() % 1001) / 100.f - 5.f, (rand() % 1001) / 100.f - 5.f,
		(rand() % 501) / 100.f);
	SAFE_RELEASE(pTransform);

	CDx11Layer*	pLayer = m_pScene->FindLayer("DefaultLayer");

	if (!pLayer)
		return false;

	//���̾ �߰�
	pLayer->AddObject(pMonster);
	SAFE_RELEASE(pMonster);
	SAFE_RELEASE(pLayer);

	return 0;
}

int CMainScene::CreateSnow()
{
	for (int i = 0; i < 100; i++) {
		CDx11GameObject* pSnow = CDx11GameObject::Create("SnowClone", OT_CLONE);

		//������Ʈ�߰�
		CDx11Renderer*	 pRenderer = pSnow->AddComponent<CDx11Renderer>("SnowRenderer");
		pRenderer->SetMesh(RECT_UI_MESH);
		pRenderer->SetShader(UI_SHADER);
		pRenderer->SetRenderState(DX11_RS_ALPHABLEND);
		pRenderer->CreateMaterial("snow", L"snow.png");// ������ �����Ѵ�.
		SAFE_RELEASE(pRenderer);

		//������Ʈ�߰�
		CDx11SnowUI*	pSnowUISc = (CDx11SnowUI*)pSnow->AddComponent<CDx11SnowUI>("SnowUIScript");
		pSnowUISc->SetBarPos((rand() % 1501 + 1), (rand() % 1001 + 1), 0);
		pSnowUISc->SetBarSize(10.f, 10.f);
		SAFE_RELEASE(pSnowUISc);

		//������Ʈ�߰�
		CSnow*	pSnowSc = (CSnow*)pSnow->AddComponent<CSnow>("SnowScript");
		SAFE_RELEASE(pSnowSc);

		//���̾ �߰�
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

int CMainScene::CreateFBXmeshs()
{
	////�� ��������������
	//CMapInfo *pMapInfo = (CMapInfo*)m_pScene->FindObjectFromTag("MapInfoObject")->FindComponentFromTag("MapInfo");
	//	
	////�� ���ӿ�����Ʈ ����
	//vector<FBXINFO>::iterator	iter;
	//vector<FBXINFO>::iterator	iterEnd = pMapInfo->GetInfo()->end();

	//
	//static int i = 0;

	//for (iter = pMapInfo->GetInfo()->begin(); iter != iterEnd; ++iter)
	//{		
	//	string name = iter->name;
	//	name += ".FBX";
	//	// string -> TCHAR	
	//	const char* all = name.c_str();
	//	int len = 1 + strlen(all);
	//	wchar_t* t_name = new wchar_t[len];
	//	if (NULL == t_name) throw std::bad_alloc();
	//	mbstowcs(t_name, all, len);

	//	//--------------------------- FBX �޽� �ε�-----------------------------------
	//	CDx11Mesh* pFbxMesh = DX11_GET_SINGLE(CDx11ResourcesManager)->LoadMesh(FBX_MESH, t_name);
	//	SAFE_RELEASE(pFbxMesh);

	//	//-------------------------------- �� FBX ------------------------------------	
	//	CDx11GameObject* pMap = CDx11GameObject::Create(iter->name);

	//	//������Ʈ �߰�
	//	CDx11Renderer* pRenderer = pMap->AddComponent<CDx11Renderer>("PlayerRenderer");
	//	pRenderer->SetMesh(FBX_MESH);
	//	pRenderer->SetShader(DEFAULT_BUMP_SHADER);
	//	SAFE_RELEASE(pRenderer);

	//	//������Ʈ �߰�
	//	CDx11Transform* pPlayerTr = pMap->GetTransform();
	//	pPlayerTr->SetWorldScale(Vec3(1.f, 1.f, 1.f));
	//	//pPlayerTr->SetWorldRot(-90.f, 0.f, 0.f);
	//	pPlayerTr->SetWorldPos(0, 0.f, 0.f);
	//	SAFE_RELEASE(pPlayerTr);

	//	Dx11ColliderHexahedron* pHexahedron = pMap->AddComponent<Dx11ColliderHexahedron>("MapColl");
	//	pHexahedron->SetInfo(Vec3(iter->PosX,iter->PosY,iter->PosZ), 
	//		Vec3(iter->RotX, iter->RotY, iter->RotZ)
	//		,Vec3( iter->ScaleX, iter->ScaleY,iter->ScaleZ));
	//	SAFE_RELEASE(pHexahedron);


	//	//���̾ �߰�
	//	CDx11Layer*	pLayer = m_pScene->FindLayer("DefaultLayer");
	//	if (!pLayer)
	//		return false;
	//	pLayer->AddObject(pMap);
	//	SAFE_RELEASE(pMap);
	//}

	return 0;
}
