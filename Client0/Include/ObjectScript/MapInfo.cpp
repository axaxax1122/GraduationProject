 #include "MapInfo.h"
#include <fstream>//파일입출력
#include <iostream>
#include "GameObject/Dx11GameObject.h"
#include "Component/Dx11Transform.h"
#include "Component/Dx11Renderer.h"
#include "Scene/Dx11Scene.h"
#include "Scene/Dx11Layer.h"
#include "Resources/Dx11Mesh.h"
#include "Resources/Dx11ResourcesManager.h"
#include "Component/Dx11ColliderOBB.h"
#include "Component/Dx11Transform.h"
#include "MapObject.h"
#include "Rendering/Dx11RenderManager.h"
#include "Scene/QuadTree.h"
#include "Scene/Dx11SceneManager.h"
#include "MoveEffect_Left.h"
#include "MoveEffect_Right.h"
#include "MoveObject_LEFT.h"
#include "MoveObject_RIGHT.h"

CMapInfo::CMapInfo()
{
}
CMapInfo::CMapInfo(const CMapInfo & map) :
	CDx11Script(map)
{
}

CMapInfo::~CMapInfo()
{
}

vector<FBXINFO> *CMapInfo::GetInfo()
{
	return &m_vInfo;
}

void CMapInfo::Road() {
	//----------------------------1. 파일 입출력------------------------------
	static char filename[50] = "Road.txt";
	ifstream inFile;
	inFile.open(filename, ios::in);

	if (inFile.is_open() == false) {//실패했을경우
		cout << "데이터 열기 실패(파일없음)" << endl << endl << endl;
	}
	else {
		m_vInfo.clear();
		while (!inFile.eof()) {
			inFile >> info.name >> info.PosX >> info.PosY >> info.PosZ >>
				info.RotX >> info.RotY >> info.RotZ >>
				info.ScaleX >> info.ScaleY >> info.ScaleZ
				>> info.MeshName;
			m_vInfo.push_back(info);
			FBXINFO	TempFbxInfo;
			TempFbxInfo.PosX = info.PosX; TempFbxInfo.PosY = info.PosY; TempFbxInfo.PosZ = info.PosZ;
			TempFbxInfo.RotX = info.RotX; TempFbxInfo.RotY = info.RotY; TempFbxInfo.RotZ = info.RotZ;
			TempFbxInfo.ScaleX = info.ScaleX; TempFbxInfo.ScaleY = info.ScaleY; TempFbxInfo.ScaleZ = info.ScaleZ;
			m_vInfoPos.insert(make_pair(info.MeshName, TempFbxInfo));
		}
	}
	inFile.close();

	//-----------------------2. 맵 게임오브젝트 생성 -------------------------
	static int i = 0;
	vector<FBXINFO>::iterator	iter;
	vector<FBXINFO>::iterator	iterEnd = GetInfo()->end();
	for (iter = GetInfo()->begin(); iter != iterEnd; ++iter)
	{
		string name = "map\\" + iter->MeshName;
		name += ".FBX";
		// string -> wchar_t	
		const char* all = name.c_str();
		int len = 1 + strlen(all);
		wchar_t* t_name = new wchar_t[len];
		if (NULL == t_name) throw std::bad_alloc();
		mbstowcs(t_name, all, len);

		//--------------------------- FBX 메쉬 로드-----------------------------------
		CDx11Mesh* pFbxMesh = DX11_GET_SINGLE(CDx11ResourcesManager)->LoadMesh(iter->MeshName, t_name);
		delete[] t_name;
		SAFE_RELEASE(pFbxMesh);
		//-------------------------------- 맵 FBX ------------------------------------	
		CDx11GameObject* pMap = CDx11GameObject::Create(iter->name);

		//컴포넌트 추가
		CDx11Renderer* pRenderer = pMap->AddComponent<CDx11Renderer>("MapRenderer");
		pRenderer->SetMesh(iter->MeshName);
		pRenderer->SetShader(DEFAULT_BUMP_SHADER);
		SAFE_RELEASE(pRenderer);

		//컴포넌트 추가
		CDx11Transform* pMapTr = pMap->GetTransform();
		unordered_map<string, FBXINFO>::iterator iterOneZumPos = m_vInfoPos.find(iter->MeshName);

		pMapTr->SetWorldScale(Vec3((iter->ScaleX / iterOneZumPos->second.ScaleX),
			(iter->ScaleY / iterOneZumPos->second.ScaleY),
			(iter->ScaleZ / iterOneZumPos->second.ScaleZ)));

		pMapTr->SetWorldRot(Vec3(-((iterOneZumPos->second.RotX - (iter->RotX))* DX11_PI / 180.f),
			-((iterOneZumPos->second.RotY - (iter->RotY))* DX11_PI / 180.f),
			-((iterOneZumPos->second.RotZ - (iter->RotZ))* DX11_PI / 180.f)));

		pMapTr->SetWorldPos(Vec3(iter->PosX, iter->PosY, iter->PosZ));

		//컴포넌트 추가
		CMapObject* pMapObject = pMap->AddComponent<CMapObject>("ObjectCollsionHandling");
		if ((-1<iter->RotX) && (iter->RotX< 1) && (-1<iter->RotY) && (iter->RotY < 1) && (-1<iter->RotZ) && (iter->RotZ<1)) {//x0 y0 z 0
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ)));
		}
		if ((265<iter->RotX) && (iter->RotX< 275) && (265<iter->RotY) && (iter->RotY < 275) && (265<iter->RotZ) && (iter->RotZ<275)) {//x270 y270 z 270
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ)));
		}
		else if ((85<iter->RotX) && (iter->RotX < 95) && (85<iter->RotY) && (iter->RotY < 95)) {//x 90 y90			
			pMapObject->SetSize((Vec3(iter->ScaleY, iter->ScaleZ, iter->ScaleX)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275) && (85<iter->RotY) && (iter->RotY < 95)) {//x 270 y 90			
			pMapObject->SetSize((Vec3(iter->ScaleY, iter->ScaleZ, iter->ScaleX)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275) && (175<iter->RotY) && (iter->RotY < 185)) {//x 270 y 180			
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleZ, iter->ScaleY)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275) && (265<iter->RotY) && (iter->RotY < 275)) {//x 270 y 270			
			pMapObject->SetSize((Vec3(iter->ScaleY, iter->ScaleZ, iter->ScaleX)));
		}
		else if ((85<iter->RotX) && (iter->RotX < 95)) {//x 90	
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleZ, iter->ScaleY)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275)) {//x 270		
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleZ, iter->ScaleY)));
		}
		else if ((85<iter->RotY) && (iter->RotY < 95)) {//y 90	
			pMapObject->SetSize((Vec3(iter->ScaleZ, iter->ScaleY, iter->ScaleX)));
		}
		else if ((175<iter->RotY) && (iter->RotY < 185)) {//y 180	
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ)));
		}
		else if ((265<iter->RotY) && (iter->RotY < 275)) {//y 270	
			pMapObject->SetSize((Vec3(iter->ScaleZ, iter->ScaleY, iter->ScaleX)));
		}
		SAFE_RELEASE(pMapObject);

		//컴포넌트 추가
		CDx11ColliderOBB* pOBB = pMap->AddComponent<CDx11ColliderOBB>("MapColl");
		pOBB->SetOBBInfo(pMapTr,
			iter->RotX* DX11_PI / 180.f, iter->RotY* DX11_PI / 180.f, iter->RotZ* DX11_PI / 180.f
			, iter->ScaleX / 2.f, iter->ScaleY / 2.f, iter->ScaleZ / 2.f);
		SAFE_RELEASE(pOBB);
		SAFE_RELEASE(pMapTr);

		//레이어에 추가	
		m_pLayer->AddObject(pMap);

		QuadTree*	pQuadTree = DX11_GET_SINGLE(CDx11SceneManager)->GetQuadTree();
		pQuadTree->AddStaticObj(pMap);
		SAFE_RELEASE(pQuadTree);

		SAFE_RELEASE(pMap);
	}
}

void CMapInfo::Wall()
{
	//----------------------------1. 파일 입출력------------------------------
	static char filename[50] = "Well.txt";
	ifstream inFile;
	inFile.open(filename, ios::in);

	if (inFile.is_open() == false) {//실패했을경우
		cout << "데이터 열기 실패(파일없음)" << endl << endl << endl;
	}
	else {
		m_vInfo.clear();
		while (!inFile.eof()) {
			inFile >> info.name >>
				info.PosX >> info.PosY >> info.PosZ >>
				info.RotX >> info.RotY >> info.RotZ >>
				info.ScaleX >> info.ScaleY >> info.ScaleZ
				>> info.MeshName;
			m_vInfo.push_back(info);
			FBXINFO	TempFbxInfo;
			TempFbxInfo.PosX = info.PosX; TempFbxInfo.PosY = info.PosY; TempFbxInfo.PosZ = info.PosZ;
			TempFbxInfo.RotX = info.RotX; TempFbxInfo.RotY = info.RotY; TempFbxInfo.RotZ = info.RotZ;
			TempFbxInfo.ScaleX = info.ScaleX; TempFbxInfo.ScaleY = info.ScaleY; TempFbxInfo.ScaleZ = info.ScaleZ;
			m_vInfoPos.insert(make_pair(info.MeshName, TempFbxInfo));
		}
	}
	inFile.close();

	//-----------------------2. 맵 게임오브젝트 생성 -------------------------
	static int i = 0;
	vector<FBXINFO>::iterator	iter;
	vector<FBXINFO>::iterator	iterEnd = GetInfo()->end();
	for (iter = GetInfo()->begin(); iter != iterEnd; ++iter)
	{
		string name = "map\\" + iter->MeshName;
		name += ".FBX";
		// string -> wchar_t	
		const char* all = name.c_str();
		int len = 1 + strlen(all);
		wchar_t* t_name = new wchar_t[len];
		if (NULL == t_name) throw std::bad_alloc();
		mbstowcs(t_name, all, len);

		//--------------------------- FBX 메쉬 로드-----------------------------------
		CDx11Mesh* pFbxMesh = DX11_GET_SINGLE(CDx11ResourcesManager)->LoadMesh(iter->MeshName, t_name);
		delete[] t_name;
		SAFE_RELEASE(pFbxMesh);

		//-------------------------------- 맵 FBX ------------------------------------	
		CDx11GameObject* pMap = CDx11GameObject::Create(iter->name);

		//컴포넌트 추가
		CDx11Renderer* pRenderer = pMap->AddComponent<CDx11Renderer>("MapRenderer");
		pRenderer->SetMesh(iter->MeshName);
		pRenderer->SetShader(DEFAULT_BUMP_SHADER);
		SAFE_RELEASE(pRenderer);

		//컴포넌트 추가
		CDx11Transform* pMapTr = pMap->GetTransform();
		//스케일을 iter->Scale/iterOneZumPos 해서 비율 구해서 해야할듯.
		unordered_map<string, FBXINFO>::iterator iterOneZumPos = m_vInfoPos.find(iter->MeshName);

		pMapTr->SetWorldScale(Vec3((iter->ScaleX / iterOneZumPos->second.ScaleX),
			(iter->ScaleY / iterOneZumPos->second.ScaleY),
			(iter->ScaleZ / iterOneZumPos->second.ScaleZ)));

		pMapTr->SetWorldRot(Vec3(-((iterOneZumPos->second.RotX - (iter->RotX))* DX11_PI / 180.f),
			-((iterOneZumPos->second.RotY - (iter->RotY))* DX11_PI / 180.f),
			-((iterOneZumPos->second.RotZ - (iter->RotZ))* DX11_PI / 180.f)));

		pMapTr->SetWorldPos(Vec3(iter->PosX, iter->PosY, iter->PosZ));

		//컴포넌트 추가
		CMapObject* pMapObject = pMap->AddComponent<CMapObject>("ObjectCollsionHandling");
		if ((-1<iter->RotX) && (iter->RotX< 1) && (-1<iter->RotY) && (iter->RotY < 1) && (-1<iter->RotZ) && (iter->RotZ<1)) {//x0 y0 z 0
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ)));
		}
		if ((265<iter->RotX) && (iter->RotX< 275) && (265<iter->RotY) && (iter->RotY < 275) && (265<iter->RotZ) && (iter->RotZ<275)) {//x270 y270 z 270
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ)));
		}
		else if ((85<iter->RotX) && (iter->RotX < 95) && (85<iter->RotY) && (iter->RotY < 95)) {//x 90 y90			
			pMapObject->SetSize((Vec3(iter->ScaleY, iter->ScaleZ, iter->ScaleX)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275) && (85<iter->RotY) && (iter->RotY < 95)) {//x 270 y 90			
			pMapObject->SetSize((Vec3(iter->ScaleY, iter->ScaleZ, iter->ScaleX)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275) && (175<iter->RotY) && (iter->RotY < 185)) {//x 270 y 180			
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleZ, iter->ScaleY)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275) && (265<iter->RotY) && (iter->RotY < 275)) {//x 270 y 270			
			pMapObject->SetSize((Vec3(iter->ScaleY, iter->ScaleZ, iter->ScaleX)));
		}
		else if ((85<iter->RotX) && (iter->RotX < 95)) {//x 90	
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleZ, iter->ScaleY)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275)) {//x 270		
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleZ, iter->ScaleY)));
		}
		else if ((85<iter->RotY) && (iter->RotY < 95)) {//y 90	
			pMapObject->SetSize((Vec3(iter->ScaleZ, iter->ScaleY, iter->ScaleX)));
		}
		else if ((175<iter->RotY) && (iter->RotY < 185)) {//y 180	
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ)));
		}
		else if ((265<iter->RotY) && (iter->RotY < 275)) {//y 270	
			pMapObject->SetSize((Vec3(iter->ScaleZ, iter->ScaleY, iter->ScaleX)));
		}
		SAFE_RELEASE(pMapObject);

		//컴포넌트 추가
		CDx11ColliderOBB* pOBB = pMap->AddComponent<CDx11ColliderOBB>("MapColl");
		pOBB->SetOBBInfo(pMapTr,
			iter->RotX* DX11_PI / 180.f, iter->RotY* DX11_PI / 180.f, iter->RotZ* DX11_PI / 180.f
			, iter->ScaleX / 2.f, iter->ScaleY / 2.f, iter->ScaleZ / 2.f);
		SAFE_RELEASE(pOBB);
		SAFE_RELEASE(pMapTr);

		//레이어에 추가	
		m_pLayer->AddObject(pMap);

		QuadTree*	pQuadTree = DX11_GET_SINGLE(CDx11SceneManager)->GetQuadTree();
		pQuadTree->AddStaticObj(pMap);
		SAFE_RELEASE(pQuadTree);

		SAFE_RELEASE(pMap);
	}
}

void CMapInfo::Object()
{
	//----------------------------1. 파일 입출력------------------------------
	static char filename[50] = "Object.txt";
	ifstream inFile;
	inFile.open(filename, ios::in);

	if (inFile.is_open() == false) {//실패했을경우
		cout << "데이터 열기 실패(파일없음)" << endl << endl << endl;
	}
	else {
		m_vInfo.clear();
		while (!inFile.eof()) {
			inFile >> info.name >>
				info.PosX >> info.PosY >> info.PosZ >>
				info.RotX >> info.RotY >> info.RotZ >>
				info.ScaleX >> info.ScaleY >> info.ScaleZ
				>> info.MeshName;
			m_vInfo.push_back(info);
			FBXINFO	TempFbxInfo;
			TempFbxInfo.PosX = info.PosX; TempFbxInfo.PosY = info.PosY; TempFbxInfo.PosZ = info.PosZ;
			TempFbxInfo.RotX = info.RotX; TempFbxInfo.RotY = info.RotY; TempFbxInfo.RotZ = info.RotZ;
			TempFbxInfo.ScaleX = info.ScaleX; TempFbxInfo.ScaleY = info.ScaleY; TempFbxInfo.ScaleZ = info.ScaleZ;
			m_vInfoPos.insert(make_pair(info.MeshName, TempFbxInfo));
		}
	}
	inFile.close();

	//-----------------------2. 맵 게임오브젝트 생성 -------------------------
	static int i = 0;
	vector<FBXINFO>::iterator	iter;
	vector<FBXINFO>::iterator	iterEnd = GetInfo()->end();
	for (iter = GetInfo()->begin(); iter != iterEnd; ++iter)
	{
		string name = "map\\" + iter->MeshName;
		name += ".FBX";
		// string -> wchar_t	
		const char* all = name.c_str();
		int len = 1 + strlen(all);
		wchar_t* t_name = new wchar_t[len];
		if (NULL == t_name) throw std::bad_alloc();
		mbstowcs(t_name, all, len);

		//--------------------------- FBX 메쉬 로드-----------------------------------
		CDx11Mesh* pFbxMesh = DX11_GET_SINGLE(CDx11ResourcesManager)->LoadMesh(iter->MeshName, t_name);
		delete[] t_name;
		SAFE_RELEASE(pFbxMesh);

		//-------------------------------- 맵 FBX ------------------------------------	
		CDx11GameObject* pMap = CDx11GameObject::Create(iter->name);

		//컴포넌트 추가
		CDx11Renderer* pRenderer = pMap->AddComponent<CDx11Renderer>("MapRenderer");
		pRenderer->SetMesh(iter->MeshName);
		pRenderer->SetShader(DEFAULT_BUMP_SHADER);
		SAFE_RELEASE(pRenderer);

		//컴포넌트 추가
		CDx11Transform* pMapTr = pMap->GetTransform();
		//스케일을 iter->Scale/iterOneZumPos 해서 비율 구해서 해야할듯.
		unordered_map<string, FBXINFO>::iterator iterOneZumPos = m_vInfoPos.find(iter->MeshName);

		pMapTr->SetWorldScale(Vec3((iter->ScaleX / iterOneZumPos->second.ScaleX),
			(iter->ScaleY / iterOneZumPos->second.ScaleY),
			(iter->ScaleZ / iterOneZumPos->second.ScaleZ)));

		pMapTr->SetWorldRot(Vec3(-((iterOneZumPos->second.RotX - (iter->RotX))* DX11_PI / 180.f),
			-((iterOneZumPos->second.RotY - (iter->RotY))* DX11_PI / 180.f),
			-((iterOneZumPos->second.RotZ - (iter->RotZ))* DX11_PI / 180.f)));

		pMapTr->SetWorldPos(Vec3(iter->PosX, iter->PosY, iter->PosZ));

		//컴포넌트 추가
		CMapObject* pMapObject = pMap->AddComponent<CMapObject>("ObjectCollsionHandling");
		if ((-1<iter->RotX) && (iter->RotX< 1) && (-1<iter->RotY) && (iter->RotY < 1) && (-1<iter->RotZ) && (iter->RotZ<1)) {//x0 y0 z 0
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ)));
		}
		if ((265<iter->RotX) && (iter->RotX< 275) && (265<iter->RotY) && (iter->RotY < 275) && (265<iter->RotZ) && (iter->RotZ<275)) {//x270 y270 z 270
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ)));
		}
		else if ((85<iter->RotX) && (iter->RotX < 95) && (85<iter->RotY) && (iter->RotY < 95)) {//x 90 y90			
			pMapObject->SetSize((Vec3(iter->ScaleY, iter->ScaleZ, iter->ScaleX)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275) && (85<iter->RotY) && (iter->RotY < 95)) {//x 270 y 90			
			pMapObject->SetSize((Vec3(iter->ScaleY, iter->ScaleZ, iter->ScaleX)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275) && (175<iter->RotY) && (iter->RotY < 185)) {//x 270 y 180			
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleZ, iter->ScaleY)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275) && (265<iter->RotY) && (iter->RotY < 275)) {//x 270 y 270			
			pMapObject->SetSize((Vec3(iter->ScaleY, iter->ScaleZ, iter->ScaleX)));
		}
		else if ((85<iter->RotX) && (iter->RotX < 95)) {//x 90	
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleZ, iter->ScaleY)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275)) {//x 270		
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleZ, iter->ScaleY)));
		}
		else if ((85<iter->RotY) && (iter->RotY < 95)) {//y 90	
			pMapObject->SetSize((Vec3(iter->ScaleZ, iter->ScaleY, iter->ScaleX)));
		}
		else if ((175<iter->RotY) && (iter->RotY < 185)) {//y 180	
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ)));
		}
		else if ((265<iter->RotY) && (iter->RotY < 275)) {//y 270	
			pMapObject->SetSize((Vec3(iter->ScaleZ, iter->ScaleY, iter->ScaleX)));
		}
		SAFE_RELEASE(pMapObject);

		//컴포넌트 추가
		CDx11ColliderOBB* pOBB = pMap->AddComponent<CDx11ColliderOBB>("MapColl");
		pOBB->SetOBBInfo(pMapTr,
			iter->RotX* DX11_PI / 180.f, iter->RotY* DX11_PI / 180.f, iter->RotZ* DX11_PI / 180.f
			, iter->ScaleX / 2.f, iter->ScaleY / 2.f, iter->ScaleZ / 2.f);
		SAFE_RELEASE(pOBB);
		SAFE_RELEASE(pMapTr);

		//레이어에 추가	
		m_pLayer->AddObject(pMap);

		QuadTree*	pQuadTree = DX11_GET_SINGLE(CDx11SceneManager)->GetQuadTree();
		pQuadTree->AddStaticObj(pMap);
		SAFE_RELEASE(pQuadTree);

		SAFE_RELEASE(pMap);
	}
}

void CMapInfo::Move_Object_LEFT()
{
	//----------------------------1. 파일 입출력------------------------------
	static char filename[50] = "move_object_LEFT.txt";
	ifstream inFile;
	inFile.open(filename, ios::in);

	if (inFile.is_open() == false) {//실패했을경우
		cout << "데이터 열기 실패(파일없음)" << endl << endl << endl;
	}
	else {
		m_vInfo.clear();
		while (!inFile.eof()) {
			inFile >> info.name >>
				info.PosX >> info.PosY >> info.PosZ >>
				info.RotX >> info.RotY >> info.RotZ >>
				info.ScaleX >> info.ScaleY >> info.ScaleZ
				>> info.MeshName;
			m_vInfo.push_back(info);
			FBXINFO	TempFbxInfo;
			TempFbxInfo.PosX = info.PosX; TempFbxInfo.PosY = info.PosY; TempFbxInfo.PosZ = info.PosZ;
			TempFbxInfo.RotX = info.RotX; TempFbxInfo.RotY = info.RotY; TempFbxInfo.RotZ = info.RotZ;
			TempFbxInfo.ScaleX = info.ScaleX; TempFbxInfo.ScaleY = info.ScaleY; TempFbxInfo.ScaleZ = info.ScaleZ;
			m_vInfoPos.insert(make_pair(info.MeshName, TempFbxInfo));
		}
	}
	inFile.close();

	//-----------------------2. 맵 게임오브젝트 생성 -------------------------
	static int i = 0;
	vector<FBXINFO>::iterator	iter;
	vector<FBXINFO>::iterator	iterEnd = GetInfo()->end();
	for (iter = GetInfo()->begin(); iter != iterEnd; ++iter)
	{
		string name = "map\\" + iter->MeshName;
		name += ".FBX";
		// string -> TCHAR	
		const char* all = name.c_str();
		int len = 1 + strlen(all);
		wchar_t* t_name = new wchar_t[len];
		if (NULL == t_name) throw std::bad_alloc();
		mbstowcs(t_name, all, len);

		//--------------------------- FBX 메쉬 로드-----------------------------------
		CDx11Mesh* pFbxMesh = DX11_GET_SINGLE(CDx11ResourcesManager)->LoadMesh(iter->MeshName, t_name);
		delete[] t_name;
		SAFE_RELEASE(pFbxMesh);

		//-------------------------------- 맵 FBX ------------------------------------	
		CDx11GameObject* pMap = CDx11GameObject::Create(iter->name);

		//컴포넌트 추가
		CDx11Renderer* pRenderer = pMap->AddComponent<CDx11Renderer>("CarRenderer");
		pRenderer->SetMesh(iter->MeshName);
		pRenderer->SetShader(DEFAULT_BUMP_SHADER);
		SAFE_RELEASE(pRenderer);

		//컴포넌트 추가
		CDx11Transform* pMapTr = pMap->GetTransform();
		//스케일을 iter->Scale/iterOneZumPos 해서 비율 구해서 해야할듯.
		unordered_map<string, FBXINFO>::iterator iterOneZumPos = m_vInfoPos.find(iter->MeshName);

		pMapTr->SetWorldScale(Vec3((iter->ScaleX / iterOneZumPos->second.ScaleX),
			(iter->ScaleY / iterOneZumPos->second.ScaleY),
			(iter->ScaleZ / iterOneZumPos->second.ScaleZ)));

		pMapTr->SetWorldRot(Vec3(-((iterOneZumPos->second.RotX - (iter->RotX))* DX11_PI / 180.f),
			-((iterOneZumPos->second.RotY - (iter->RotY))* DX11_PI / 180.f),
			-((iterOneZumPos->second.RotZ - (iter->RotZ))* DX11_PI / 180.f)));

		pMapTr->SetWorldPos(Vec3(iter->PosX, iter->PosY, iter->PosZ));

		//컴포넌트 추가
		CMapObject* pMapObject = pMap->AddComponent<CMapObject>("ObjectCollsionHandling");
		if ((-1<iter->RotX) && (iter->RotX< 1) && (-1<iter->RotY) && (iter->RotY < 1) && (-1<iter->RotZ) && (iter->RotZ<1)) {//x0 y0 z 0
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ)));
		}
		else if ((265<iter->RotX) && (iter->RotX< 275) && (265<iter->RotY) && (iter->RotY < 275) && (265<iter->RotZ) && (iter->RotZ<275)) {//x270 y270 z 270
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ)));
		}
		else if ((85<iter->RotX) && (iter->RotX < 95) && (85<iter->RotY) && (iter->RotY < 95)) {//x 90 y90			
			pMapObject->SetSize((Vec3(iter->ScaleY, iter->ScaleZ, iter->ScaleX)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275) && (85<iter->RotY) && (iter->RotY < 95)) {//x 270 y 90			
			pMapObject->SetSize((Vec3(iter->ScaleY, iter->ScaleZ, iter->ScaleX)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275) && (175<iter->RotY) && (iter->RotY < 185)) {//x 270 y 180			
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleZ, iter->ScaleY)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275) && (265<iter->RotY) && (iter->RotY < 275)) {//x 270 y 270			
			pMapObject->SetSize((Vec3(iter->ScaleY, iter->ScaleZ, iter->ScaleX)));
		}
		else if ((85<iter->RotX) && (iter->RotX < 95)) {//x 90	
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleZ, iter->ScaleY)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275)) {//x 270		
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleZ, iter->ScaleY)));
		}
		else if ((85<iter->RotY) && (iter->RotY < 95)) {//y 90	
			pMapObject->SetSize((Vec3(iter->ScaleZ, iter->ScaleY, iter->ScaleX)));
		}
		else if ((175<iter->RotY) && (iter->RotY < 185)) {//y 180	
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ)));
		}
		else if ((265<iter->RotY) && (iter->RotY < 275)) {//y 270	
			pMapObject->SetSize((Vec3(iter->ScaleZ, iter->ScaleY, iter->ScaleX)));
		}
		SAFE_RELEASE(pMapObject);

		//컴포넌트 추가
		CMoveObject_LEFT* pMove = pMap->AddComponent<CMoveObject_LEFT>("MoveObject_LEFT");
		SAFE_RELEASE(pMove);

		//컴포넌트 추가
		CDx11ColliderOBB* pOBB = pMap->AddComponent<CDx11ColliderOBB>("MapColl");
		pOBB->SetOBBInfo(pMapTr,
			iter->RotX* DX11_PI / 180.f, iter->RotY* DX11_PI / 180.f, iter->RotZ* DX11_PI / 180.f
			, iter->ScaleX / 2.f, iter->ScaleY / 2.f, iter->ScaleZ / 2.f);
		SAFE_RELEASE(pOBB);
		SAFE_RELEASE(pMapTr);

		//레이어에 추가	
		m_pLayer->AddObject(pMap);

		SAFE_RELEASE(pMap);
	}
}

void CMapInfo::Move_Object_RIGHT()
{
	//----------------------------1. 파일 입출력------------------------------
	static char filename[50] = "move_object_RIGHT.txt";
	ifstream inFile;
	inFile.open(filename, ios::in);

	if (inFile.is_open() == false) {//실패했을경우
		cout << "데이터 열기 실패(파일없음)" << endl << endl << endl;
	}
	else {
		m_vInfo.clear();
		while (!inFile.eof()) {
			inFile >> info.name >>
				info.PosX >> info.PosY >> info.PosZ >>
				info.RotX >> info.RotY >> info.RotZ >>
				info.ScaleX >> info.ScaleY >> info.ScaleZ
				>> info.MeshName;
			m_vInfo.push_back(info);
			FBXINFO	TempFbxInfo;
			TempFbxInfo.PosX = info.PosX; TempFbxInfo.PosY = info.PosY; TempFbxInfo.PosZ = info.PosZ;
			TempFbxInfo.RotX = info.RotX; TempFbxInfo.RotY = info.RotY; TempFbxInfo.RotZ = info.RotZ;
			TempFbxInfo.ScaleX = info.ScaleX; TempFbxInfo.ScaleY = info.ScaleY; TempFbxInfo.ScaleZ = info.ScaleZ;
			m_vInfoPos.insert(make_pair(info.MeshName, TempFbxInfo));
		}
	}
	inFile.close();

	//-----------------------2. 맵 게임오브젝트 생성 -------------------------
	static int i = 0;
	vector<FBXINFO>::iterator	iter;
	vector<FBXINFO>::iterator	iterEnd = GetInfo()->end();
	for (iter = GetInfo()->begin(); iter != iterEnd; ++iter)
	{
		string name = "map\\" + iter->MeshName;
		name += ".FBX";
		// string -> TCHAR	
		const char* all = name.c_str();
		int len = 1 + strlen(all);
		wchar_t* t_name = new wchar_t[len];
		if (NULL == t_name) throw std::bad_alloc();
		mbstowcs(t_name, all, len);

		//--------------------------- FBX 메쉬 로드-----------------------------------
		CDx11Mesh* pFbxMesh = DX11_GET_SINGLE(CDx11ResourcesManager)->LoadMesh(iter->MeshName, t_name);
		delete[] t_name;
		SAFE_RELEASE(pFbxMesh);

		//-------------------------------- 맵 FBX ------------------------------------	
		CDx11GameObject* pMap = CDx11GameObject::Create(iter->name);

		//컴포넌트 추가
		CDx11Renderer* pRenderer = pMap->AddComponent<CDx11Renderer>("CarRenderer");
		pRenderer->SetMesh(iter->MeshName);
		pRenderer->SetShader(DEFAULT_BUMP_SHADER);
		SAFE_RELEASE(pRenderer);

		//컴포넌트 추가
		CDx11Transform* pMapTr = pMap->GetTransform();
		//스케일을 iter->Scale/iterOneZumPos 해서 비율 구해서 해야할듯.
		unordered_map<string, FBXINFO>::iterator iterOneZumPos = m_vInfoPos.find(iter->MeshName);

		pMapTr->SetWorldScale(Vec3((iter->ScaleX / iterOneZumPos->second.ScaleX),
			(iter->ScaleY / iterOneZumPos->second.ScaleY),
			(iter->ScaleZ / iterOneZumPos->second.ScaleZ)));

		pMapTr->SetWorldRot(Vec3(-((iterOneZumPos->second.RotX - (iter->RotX))* DX11_PI / 180.f),
			-((iterOneZumPos->second.RotY - (iter->RotY))* DX11_PI / 180.f),
			-((iterOneZumPos->second.RotZ - (iter->RotZ))* DX11_PI / 180.f)));

		pMapTr->SetWorldPos(Vec3(iter->PosX, iter->PosY, iter->PosZ));

		//컴포넌트 추가
		CMapObject* pMapObject = pMap->AddComponent<CMapObject>("ObjectCollsionHandling");
		if ((-1<iter->RotX) && (iter->RotX< 1) && (-1<iter->RotY) && (iter->RotY < 1) && (-1<iter->RotZ) && (iter->RotZ<1)) {//x0 y0 z 0
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ)));
		}
		else if ((265<iter->RotX) && (iter->RotX< 275) && (265<iter->RotY) && (iter->RotY < 275) && (265<iter->RotZ) && (iter->RotZ<275)) {//x270 y270 z 270
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ)));
		}
		else if ((85<iter->RotX) && (iter->RotX < 95) && (85<iter->RotY) && (iter->RotY < 95)) {//x 90 y90			
			pMapObject->SetSize((Vec3(iter->ScaleY, iter->ScaleZ, iter->ScaleX)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275) && (85<iter->RotY) && (iter->RotY < 95)) {//x 270 y 90			
			pMapObject->SetSize((Vec3(iter->ScaleY, iter->ScaleZ, iter->ScaleX)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275) && (175<iter->RotY) && (iter->RotY < 185)) {//x 270 y 180			
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleZ, iter->ScaleY)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275) && (265<iter->RotY) && (iter->RotY < 275)) {//x 270 y 270			
			pMapObject->SetSize((Vec3(iter->ScaleY, iter->ScaleZ, iter->ScaleX)));
		}
		else if ((85<iter->RotX) && (iter->RotX < 95)) {//x 90	
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleZ, iter->ScaleY)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275)) {//x 270		
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleZ, iter->ScaleY)));
		}
		else if ((85<iter->RotY) && (iter->RotY < 95)) {//y 90	
			pMapObject->SetSize((Vec3(iter->ScaleZ, iter->ScaleY, iter->ScaleX)));
		}
		else if ((175<iter->RotY) && (iter->RotY < 185)) {//y 180	
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ)));
		}
		else if ((265<iter->RotY) && (iter->RotY < 275)) {//y 270	
			pMapObject->SetSize((Vec3(iter->ScaleZ, iter->ScaleY, iter->ScaleX)));
		}
		SAFE_RELEASE(pMapObject);

		//컴포넌트 추가
		CMoveObject_RIGHT* pMove = pMap->AddComponent<CMoveObject_RIGHT>("MoveObject_RIGHT");
		SAFE_RELEASE(pMove);

		//컴포넌트 추가
		CDx11ColliderOBB* pOBB = pMap->AddComponent<CDx11ColliderOBB>("MapColl");
		pOBB->SetOBBInfo(pMapTr,
			iter->RotX* DX11_PI / 180.f, iter->RotY* DX11_PI / 180.f, iter->RotZ* DX11_PI / 180.f
			, iter->ScaleX / 2.f, iter->ScaleY / 2.f, iter->ScaleZ / 2.f);
		SAFE_RELEASE(pOBB);
		SAFE_RELEASE(pMapTr);

		//레이어에 추가	
		m_pLayer->AddObject(pMap);

		SAFE_RELEASE(pMap);
	}
}

void CMapInfo::One()
{
	//----------------------------1. 파일 입출력------------------------------
	static char filename[50] = "1.txt";
	ifstream inFile;
	inFile.open(filename, ios::in);

	if (inFile.is_open() == false) {//실패했을경우
		cout << "데이터 열기 실패(파일없음)" << endl << endl << endl;
	}
	else {
		m_vInfo.clear();
		while (!inFile.eof()) {
			inFile >> info.name >>
				info.PosX >> info.PosY >> info.PosZ >>
				info.RotX >> info.RotY >> info.RotZ >>
				info.ScaleX >> info.ScaleY >> info.ScaleZ
				>> info.MeshName;
			m_vInfo.push_back(info);
			FBXINFO	TempFbxInfo;
			TempFbxInfo.PosX = info.PosX; TempFbxInfo.PosY = info.PosY; TempFbxInfo.PosZ = info.PosZ;
			TempFbxInfo.RotX = info.RotX; TempFbxInfo.RotY = info.RotY; TempFbxInfo.RotZ = info.RotZ;
			TempFbxInfo.ScaleX = info.ScaleX; TempFbxInfo.ScaleY = info.ScaleY; TempFbxInfo.ScaleZ = info.ScaleZ;
			m_vInfoPos.insert(make_pair(info.MeshName, TempFbxInfo));
		}
	}
	inFile.close();

	//-----------------------2. 맵 게임오브젝트 생성 -------------------------
	static int i = 0;
	vector<FBXINFO>::iterator	iter;
	vector<FBXINFO>::iterator	iterEnd = GetInfo()->end();
	for (iter = GetInfo()->begin(); iter != iterEnd; ++iter)
	{
		string name = "map\\" + iter->MeshName;
		name += ".FBX";
		// string -> wchar_t	
		const char* all = name.c_str();
		int len = 1 + strlen(all);
		wchar_t* t_name = new wchar_t[len];
		if (NULL == t_name) throw std::bad_alloc();
		mbstowcs(t_name, all, len);

		//--------------------------- FBX 메쉬+텍,샘 로드-----------------------------------
		//LoadMesh ( 키값 ,파일위치)
		CDx11Mesh* pFbxMesh = DX11_GET_SINGLE(CDx11ResourcesManager)->LoadMesh(iter->MeshName, t_name);
		delete[] t_name;
		SAFE_RELEASE(pFbxMesh);

		//-------------------------------- 맵 FBX ------------------------------------	
		CDx11GameObject* pMap = CDx11GameObject::Create(iter->name);

		//컴포넌트 추가		
		CDx11Renderer* pRenderer = pMap->AddComponent<CDx11Renderer>("MapRenderer");
		pRenderer->SetMesh(iter->MeshName);
		pRenderer->SetShader(DEFAULT_BUMP_SHADER);
		SAFE_RELEASE(pRenderer);

		//컴포넌트 추가
		CDx11Transform* pMapTr = pMap->GetTransform();
		//스케일을 iter->Scale/iterOneZumPos 해서 비율 구해서 해야할듯.
		unordered_map<string, FBXINFO>::iterator iterOneZumPos = m_vInfoPos.find(iter->MeshName);

		pMapTr->SetWorldScale(Vec3((iter->ScaleX / iterOneZumPos->second.ScaleX),
			(iter->ScaleY / iterOneZumPos->second.ScaleY),
			(iter->ScaleZ / iterOneZumPos->second.ScaleZ)));

		pMapTr->SetWorldRot(Vec3(-((iterOneZumPos->second.RotX - (iter->RotX))* DX11_PI / 180.f),
			-((iterOneZumPos->second.RotY - (iter->RotY))* DX11_PI / 180.f),
			-((iterOneZumPos->second.RotZ - (iter->RotZ))* DX11_PI / 180.f)));

		pMapTr->SetWorldPos(Vec3(iter->PosX, iter->PosY, iter->PosZ));

		//컴포넌트 추가
		CMapObject* pMapObject = pMap->AddComponent<CMapObject>("ObjectCollsionHandling");
		if ((-1<iter->RotX) && (iter->RotX< 1) && (-1<iter->RotY) && (iter->RotY < 1) && (-1<iter->RotZ) && (iter->RotZ<1)) {//x0 y0 z 0
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ)));
		}
		if ((265<iter->RotX) && (iter->RotX< 275) && (265<iter->RotY) && (iter->RotY < 275) && (265<iter->RotZ) && (iter->RotZ<275)) {//x270 y270 z 270
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ)));
		}
		else if ((85<iter->RotX) && (iter->RotX < 95) && (85<iter->RotY) && (iter->RotY < 95)) {//x 90 y90			
			pMapObject->SetSize((Vec3(iter->ScaleY, iter->ScaleZ, iter->ScaleX)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275) && (85<iter->RotY) && (iter->RotY < 95)) {//x 270 y 90			
			pMapObject->SetSize((Vec3(iter->ScaleY, iter->ScaleZ, iter->ScaleX)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275) && (175<iter->RotY) && (iter->RotY < 185)) {//x 270 y 180			
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleZ, iter->ScaleY)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275) && (265<iter->RotY) && (iter->RotY < 275)) {//x 270 y 270			
			pMapObject->SetSize((Vec3(iter->ScaleY, iter->ScaleZ, iter->ScaleX)));
		}
		else if ((85<iter->RotX) && (iter->RotX < 95)) {//x 90	
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleZ, iter->ScaleY)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275)) {//x 270		
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleZ, iter->ScaleY)));
		}
		else if ((85<iter->RotY) && (iter->RotY < 95)) {//y 90	
			pMapObject->SetSize((Vec3(iter->ScaleZ, iter->ScaleY, iter->ScaleX)));
		}
		else if ((175<iter->RotY) && (iter->RotY < 185)) {//y 180	
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ)));
		}
		else if ((265<iter->RotY) && (iter->RotY < 275)) {//y 270	
			pMapObject->SetSize((Vec3(iter->ScaleZ, iter->ScaleY, iter->ScaleX)));
		}
		SAFE_RELEASE(pMapObject);

		//컴포넌트 추가
		CDx11ColliderOBB* pOBB = pMap->AddComponent<CDx11ColliderOBB>("MapColl");
		pOBB->SetOBBInfo(pMapTr,
			iter->RotX* DX11_PI / 180.f, iter->RotY* DX11_PI / 180.f, iter->RotZ* DX11_PI / 180.f
			, iter->ScaleX / 2.f, iter->ScaleY / 2.f, iter->ScaleZ / 2.f);
		SAFE_RELEASE(pOBB);
		SAFE_RELEASE(pMapTr);

		//레이어에 추가	
		m_pLayer->AddObject(pMap);

		QuadTree*	pQuadTree = DX11_GET_SINGLE(CDx11SceneManager)->GetQuadTree();
		pQuadTree->AddStaticObj(pMap);
		SAFE_RELEASE(pQuadTree);

		SAFE_RELEASE(pMap);
	}
}

void CMapInfo::Two()
{
	//----------------------------1. 파일 입출력------------------------------
	static char filename[50] = "2.txt";
	ifstream inFile;
	inFile.open(filename, ios::in);

	if (inFile.is_open() == false) {//실패했을경우
		cout << "데이터 열기 실패(파일없음)" << endl << endl << endl;
	}
	else {
		m_vInfo.clear();
		while (!inFile.eof()) {
			inFile >> info.name >>
				info.PosX >> info.PosY >> info.PosZ >>
				info.RotX >> info.RotY >> info.RotZ >>
				info.ScaleX >> info.ScaleY >> info.ScaleZ
				>> info.MeshName;
			m_vInfo.push_back(info);
			FBXINFO	TempFbxInfo;
			TempFbxInfo.PosX = info.PosX; TempFbxInfo.PosY = info.PosY; TempFbxInfo.PosZ = info.PosZ;
			TempFbxInfo.RotX = info.RotX; TempFbxInfo.RotY = info.RotY; TempFbxInfo.RotZ = info.RotZ;
			TempFbxInfo.ScaleX = info.ScaleX; TempFbxInfo.ScaleY = info.ScaleY; TempFbxInfo.ScaleZ = info.ScaleZ;
			m_vInfoPos.insert(make_pair(info.MeshName, TempFbxInfo));
		}
	}
	inFile.close();

	//-----------------------2. 맵 게임오브젝트 생성 -------------------------
	static int i = 0;
	vector<FBXINFO>::iterator	iter;
	vector<FBXINFO>::iterator	iterEnd = GetInfo()->end();
	for (iter = GetInfo()->begin(); iter != iterEnd; ++iter)
	{
		string name = "map\\" + iter->MeshName;
		name += ".FBX";
		// string -> wchar_t	
		const char* all = name.c_str();
		int len = 1 + strlen(all);
		wchar_t* t_name = new wchar_t[len];
		if (NULL == t_name) throw std::bad_alloc();
		mbstowcs(t_name, all, len);

		//--------------------------- FBX 메쉬 로드-----------------------------------
		CDx11Mesh* pFbxMesh = DX11_GET_SINGLE(CDx11ResourcesManager)->LoadMesh(iter->MeshName, t_name);
		delete[] t_name;
		SAFE_RELEASE(pFbxMesh);

		//-------------------------------- 맵 FBX ------------------------------------	
		CDx11GameObject* pMap = CDx11GameObject::Create(iter->name);

		//컴포넌트 추가
		CDx11Renderer* pRenderer = pMap->AddComponent<CDx11Renderer>("MapRenderer");
		pRenderer->SetMesh(iter->MeshName);
		pRenderer->SetShader(DEFAULT_BUMP_SHADER);
		SAFE_RELEASE(pRenderer);

		//컴포넌트 추가
		CDx11Transform* pMapTr = pMap->GetTransform();
		//스케일을 iter->Scale/iterOneZumPos 해서 비율 구해서 해야할듯.
		unordered_map<string, FBXINFO>::iterator iterOneZumPos = m_vInfoPos.find(iter->MeshName);

		pMapTr->SetWorldScale(Vec3((iter->ScaleX / iterOneZumPos->second.ScaleX),
			(iter->ScaleY / iterOneZumPos->second.ScaleY),
			(iter->ScaleZ / iterOneZumPos->second.ScaleZ)));

		pMapTr->SetWorldRot(Vec3(-((iterOneZumPos->second.RotX - (iter->RotX))* DX11_PI / 180.f),
			-((iterOneZumPos->second.RotY - (iter->RotY))* DX11_PI / 180.f),
			-((iterOneZumPos->second.RotZ - (iter->RotZ))* DX11_PI / 180.f)));

		pMapTr->SetWorldPos(Vec3(iter->PosX, iter->PosY, iter->PosZ));

		//컴포넌트 추가
		CMapObject* pMapObject = pMap->AddComponent<CMapObject>("ObjectCollsionHandling");
		if ((-1<iter->RotX) && (iter->RotX< 1) && (-1<iter->RotY) && (iter->RotY < 1) && (-1<iter->RotZ) && (iter->RotZ<1)) {//x0 y0 z 0
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ)));
		}
		if ((265<iter->RotX) && (iter->RotX< 275) && (265<iter->RotY) && (iter->RotY < 275) && (265<iter->RotZ) && (iter->RotZ<275)) {//x270 y270 z 270
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ)));
		}
		else if ((85<iter->RotX) && (iter->RotX < 95) && (85<iter->RotY) && (iter->RotY < 95)) {//x 90 y90			
			pMapObject->SetSize((Vec3(iter->ScaleY, iter->ScaleZ, iter->ScaleX)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275) && (85<iter->RotY) && (iter->RotY < 95)) {//x 270 y 90			
			pMapObject->SetSize((Vec3(iter->ScaleY, iter->ScaleZ, iter->ScaleX)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275) && (175<iter->RotY) && (iter->RotY < 185)) {//x 270 y 180			
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleZ, iter->ScaleY)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275) && (265<iter->RotY) && (iter->RotY < 275)) {//x 270 y 270			
			pMapObject->SetSize((Vec3(iter->ScaleY, iter->ScaleZ, iter->ScaleX)));
		}
		else if ((85<iter->RotX) && (iter->RotX < 95)) {//x 90	
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleZ, iter->ScaleY)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275)) {//x 270		
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleZ, iter->ScaleY)));
		}
		else if ((85<iter->RotY) && (iter->RotY < 95)) {//y 90	
			pMapObject->SetSize((Vec3(iter->ScaleZ, iter->ScaleY, iter->ScaleX)));
		}
		else if ((175<iter->RotY) && (iter->RotY < 185)) {//y 180	
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ)));
		}
		else if ((265<iter->RotY) && (iter->RotY < 275)) {//y 270	
			pMapObject->SetSize((Vec3(iter->ScaleZ, iter->ScaleY, iter->ScaleX)));
		}
		SAFE_RELEASE(pMapObject);

		//컴포넌트 추가
		CDx11ColliderOBB* pOBB = pMap->AddComponent<CDx11ColliderOBB>("MapColl");
		pOBB->SetOBBInfo(pMapTr,
			iter->RotX* DX11_PI / 180.f, iter->RotY* DX11_PI / 180.f, iter->RotZ* DX11_PI / 180.f
			, iter->ScaleX / 2.f, iter->ScaleY / 2.f, iter->ScaleZ / 2.f);
		SAFE_RELEASE(pOBB);
		SAFE_RELEASE(pMapTr);

		//레이어에 추가	
		m_pLayer->AddObject(pMap);

		QuadTree*	pQuadTree = DX11_GET_SINGLE(CDx11SceneManager)->GetQuadTree();
		pQuadTree->AddStaticObj(pMap);
		SAFE_RELEASE(pQuadTree);

		SAFE_RELEASE(pMap);
	}
}

void CMapInfo::Three()
{
	//----------------------------1. 파일 입출력------------------------------
	static char filename[50] = "3.txt";
	ifstream inFile;
	inFile.open(filename, ios::in);

	if (inFile.is_open() == false) {//실패했을경우
		cout << "데이터 열기 실패(파일없음)" << endl << endl << endl;
	}
	else {
		m_vInfo.clear();
		while (!inFile.eof()) {
			inFile >> info.name >>
				info.PosX >> info.PosY >> info.PosZ >>
				info.RotX >> info.RotY >> info.RotZ >>
				info.ScaleX >> info.ScaleY >> info.ScaleZ
				>> info.MeshName;
			m_vInfo.push_back(info);
			FBXINFO	TempFbxInfo;
			TempFbxInfo.PosX = info.PosX; TempFbxInfo.PosY = info.PosY; TempFbxInfo.PosZ = info.PosZ;
			TempFbxInfo.RotX = info.RotX; TempFbxInfo.RotY = info.RotY; TempFbxInfo.RotZ = info.RotZ;
			TempFbxInfo.ScaleX = info.ScaleX; TempFbxInfo.ScaleY = info.ScaleY; TempFbxInfo.ScaleZ = info.ScaleZ;
			m_vInfoPos.insert(make_pair(info.MeshName, TempFbxInfo));
		}
	}
	inFile.close();

	//-----------------------2. 맵 게임오브젝트 생성 -------------------------
	static int i = 0;
	vector<FBXINFO>::iterator	iter;
	vector<FBXINFO>::iterator	iterEnd = GetInfo()->end();
	for (iter = GetInfo()->begin(); iter != iterEnd; ++iter)
	{
		string name = "map\\" + iter->MeshName;
		name += ".FBX";
		// string -> wchar_t	
		const char* all = name.c_str();
		int len = 1 + strlen(all);
		wchar_t* t_name = new wchar_t[len];
		if (NULL == t_name) throw std::bad_alloc();
		mbstowcs(t_name, all, len);

		//--------------------------- FBX 메쉬 로드-----------------------------------
		CDx11Mesh* pFbxMesh = DX11_GET_SINGLE(CDx11ResourcesManager)->LoadMesh(iter->MeshName, t_name);
		delete[] t_name;
		SAFE_RELEASE(pFbxMesh);

		//-------------------------------- 맵 FBX ------------------------------------	
		CDx11GameObject* pMap = CDx11GameObject::Create(iter->name);

		//컴포넌트 추가
		CDx11Renderer* pRenderer = pMap->AddComponent<CDx11Renderer>("MapRenderer");
		pRenderer->SetMesh(iter->MeshName);
		pRenderer->SetShader(DEFAULT_BUMP_SHADER);
		SAFE_RELEASE(pRenderer);

		//컴포넌트 추가
		CDx11Transform* pMapTr = pMap->GetTransform();
		//스케일을 iter->Scale/iterOneZumPos 해서 비율 구해서 해야할듯.
		unordered_map<string, FBXINFO>::iterator iterOneZumPos = m_vInfoPos.find(iter->MeshName);

		pMapTr->SetWorldScale(Vec3((iter->ScaleX / iterOneZumPos->second.ScaleX),
			(iter->ScaleY / iterOneZumPos->second.ScaleY),
			(iter->ScaleZ / iterOneZumPos->second.ScaleZ)));

		pMapTr->SetWorldRot(Vec3(-((iterOneZumPos->second.RotX - (iter->RotX))* DX11_PI / 180.f),
			-((iterOneZumPos->second.RotY - (iter->RotY))* DX11_PI / 180.f),
			-((iterOneZumPos->second.RotZ - (iter->RotZ))* DX11_PI / 180.f)));

		pMapTr->SetWorldPos(Vec3(iter->PosX, iter->PosY, iter->PosZ));

		//컴포넌트 추가
		CMapObject* pMapObject = pMap->AddComponent<CMapObject>("ObjectCollsionHandling");
		if ((-1<iter->RotX) && (iter->RotX< 1) && (-1<iter->RotY) && (iter->RotY < 1) && (-1<iter->RotZ) && (iter->RotZ<1)) {//x0 y0 z 0
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ)));
		}
		if ((265<iter->RotX) && (iter->RotX< 275) && (265<iter->RotY) && (iter->RotY < 275) && (265<iter->RotZ) && (iter->RotZ<275)) {//x270 y270 z 270
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ)));
		}
		else if ((85<iter->RotX) && (iter->RotX < 95) && (85<iter->RotY) && (iter->RotY < 95)) {//x 90 y90			
			pMapObject->SetSize((Vec3(iter->ScaleY, iter->ScaleZ, iter->ScaleX)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275) && (85<iter->RotY) && (iter->RotY < 95)) {//x 270 y 90			
			pMapObject->SetSize((Vec3(iter->ScaleY, iter->ScaleZ, iter->ScaleX)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275) && (175<iter->RotY) && (iter->RotY < 185)) {//x 270 y 180			
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleZ, iter->ScaleY)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275) && (265<iter->RotY) && (iter->RotY < 275)) {//x 270 y 270			
			pMapObject->SetSize((Vec3(iter->ScaleY, iter->ScaleZ, iter->ScaleX)));
		}
		else if ((85<iter->RotX) && (iter->RotX < 95)) {//x 90	
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleZ, iter->ScaleY)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275)) {//x 270		
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleZ, iter->ScaleY)));
		}
		else if ((85<iter->RotY) && (iter->RotY < 95)) {//y 90	
			pMapObject->SetSize((Vec3(iter->ScaleZ, iter->ScaleY, iter->ScaleX)));
		}
		else if ((175<iter->RotY) && (iter->RotY < 185)) {//y 180	
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ)));
		}
		else if ((265<iter->RotY) && (iter->RotY < 275)) {//y 270	
			pMapObject->SetSize((Vec3(iter->ScaleZ, iter->ScaleY, iter->ScaleX)));
		}
		SAFE_RELEASE(pMapObject);

		//컴포넌트 추가
		CDx11ColliderOBB* pOBB = pMap->AddComponent<CDx11ColliderOBB>("MapColl");
		pOBB->SetOBBInfo(pMapTr,
			iter->RotX* DX11_PI / 180.f, iter->RotY* DX11_PI / 180.f, iter->RotZ* DX11_PI / 180.f
			, iter->ScaleX / 2.f, iter->ScaleY / 2.f, iter->ScaleZ / 2.f);
		SAFE_RELEASE(pOBB);
		SAFE_RELEASE(pMapTr);

		//레이어에 추가	
		m_pLayer->AddObject(pMap);

		QuadTree*	pQuadTree = DX11_GET_SINGLE(CDx11SceneManager)->GetQuadTree();
		pQuadTree->AddStaticObj(pMap);
		SAFE_RELEASE(pQuadTree);

		SAFE_RELEASE(pMap);
	}
}

void CMapInfo::Four()
{
	//----------------------------1. 파일 입출력------------------------------
	static char filename[50] = "4.txt";
	ifstream inFile;
	inFile.open(filename, ios::in);

	if (inFile.is_open() == false) {//실패했을경우
		cout << "데이터 열기 실패(파일없음)" << endl << endl << endl;
	}
	else {
		m_vInfo.clear();
		while (!inFile.eof()) {
			inFile >> info.name >>
				info.PosX >> info.PosY >> info.PosZ >>
				info.RotX >> info.RotY >> info.RotZ >>
				info.ScaleX >> info.ScaleY >> info.ScaleZ
				>> info.MeshName;
			m_vInfo.push_back(info);
			FBXINFO	TempFbxInfo;
			TempFbxInfo.PosX = info.PosX; TempFbxInfo.PosY = info.PosY; TempFbxInfo.PosZ = info.PosZ;
			TempFbxInfo.RotX = info.RotX; TempFbxInfo.RotY = info.RotY; TempFbxInfo.RotZ = info.RotZ;
			TempFbxInfo.ScaleX = info.ScaleX; TempFbxInfo.ScaleY = info.ScaleY; TempFbxInfo.ScaleZ = info.ScaleZ;
			m_vInfoPos.insert(make_pair(info.MeshName, TempFbxInfo));
		}
	}
	inFile.close();

	//-----------------------2. 맵 게임오브젝트 생성 -------------------------
	static int i = 0;
	vector<FBXINFO>::iterator	iter;
	vector<FBXINFO>::iterator	iterEnd = GetInfo()->end();
	for (iter = GetInfo()->begin(); iter != iterEnd; ++iter)
	{
		string name = "map\\" + iter->MeshName;
		name += ".FBX";
		// string -> wchar_t	
		const char* all = name.c_str();
		int len = 1 + strlen(all);
		wchar_t* t_name = new wchar_t[len];
		if (NULL == t_name) throw std::bad_alloc();
		mbstowcs(t_name, all, len);

		//--------------------------- FBX 메쉬 로드-----------------------------------
		CDx11Mesh* pFbxMesh = DX11_GET_SINGLE(CDx11ResourcesManager)->LoadMesh(iter->MeshName, t_name);
		delete[] t_name;
		SAFE_RELEASE(pFbxMesh);

		//-------------------------------- 맵 FBX ------------------------------------	
		CDx11GameObject* pMap = CDx11GameObject::Create(iter->name);

		//컴포넌트 추가
		CDx11Renderer* pRenderer = pMap->AddComponent<CDx11Renderer>("MapRenderer");
		pRenderer->SetMesh(iter->MeshName);
		pRenderer->SetShader(DEFAULT_BUMP_SHADER);
		SAFE_RELEASE(pRenderer);

		//컴포넌트 추가
		CDx11Transform* pMapTr = pMap->GetTransform();
		//스케일을 iter->Scale/iterOneZumPos 해서 비율 구해서 해야할듯.
		unordered_map<string, FBXINFO>::iterator iterOneZumPos = m_vInfoPos.find(iter->MeshName);

		pMapTr->SetWorldScale(Vec3((iter->ScaleX / iterOneZumPos->second.ScaleX),
			(iter->ScaleY / iterOneZumPos->second.ScaleY),
			(iter->ScaleZ / iterOneZumPos->second.ScaleZ)));

		pMapTr->SetWorldRot(Vec3(-((iterOneZumPos->second.RotX - (iter->RotX))* DX11_PI / 180.f),
			-((iterOneZumPos->second.RotY - (iter->RotY))* DX11_PI / 180.f),
			-((iterOneZumPos->second.RotZ - (iter->RotZ))* DX11_PI / 180.f)));

		pMapTr->SetWorldPos(Vec3(iter->PosX, iter->PosY, iter->PosZ));

		//컴포넌트 추가
		CMapObject* pMapObject = pMap->AddComponent<CMapObject>("ObjectCollsionHandling");
		if ((-1<iter->RotX) && (iter->RotX< 1) && (-1<iter->RotY) && (iter->RotY < 1) && (-1<iter->RotZ) && (iter->RotZ<1)) {//x0 y0 z 0
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ)));
		}
		if ((265<iter->RotX) && (iter->RotX< 275) && (265<iter->RotY) && (iter->RotY < 275) && (265<iter->RotZ) && (iter->RotZ<275)) {//x270 y270 z 270
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ)));
		}
		else if ((85<iter->RotX) && (iter->RotX < 95) && (85<iter->RotY) && (iter->RotY < 95)) {//x 90 y90			
			pMapObject->SetSize((Vec3(iter->ScaleY, iter->ScaleZ, iter->ScaleX)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275) && (85<iter->RotY) && (iter->RotY < 95)) {//x 270 y 90			
			pMapObject->SetSize((Vec3(iter->ScaleY, iter->ScaleZ, iter->ScaleX)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275) && (175<iter->RotY) && (iter->RotY < 185)) {//x 270 y 180			
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleZ, iter->ScaleY)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275) && (265<iter->RotY) && (iter->RotY < 275)) {//x 270 y 270			
			pMapObject->SetSize((Vec3(iter->ScaleY, iter->ScaleZ, iter->ScaleX)));
		}
		else if ((85<iter->RotX) && (iter->RotX < 95)) {//x 90	
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleZ, iter->ScaleY)));
		}
		else if ((265<iter->RotX) && (iter->RotX < 275)) {//x 270		
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleZ, iter->ScaleY)));
		}
		else if ((85<iter->RotY) && (iter->RotY < 95)) {//y 90	
			pMapObject->SetSize((Vec3(iter->ScaleZ, iter->ScaleY, iter->ScaleX)));
		}
		else if ((175<iter->RotY) && (iter->RotY < 185)) {//y 180	
			pMapObject->SetSize((Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ)));
		}
		else if ((265<iter->RotY) && (iter->RotY < 275)) {//y 270	
			pMapObject->SetSize((Vec3(iter->ScaleZ, iter->ScaleY, iter->ScaleX)));
		}
		SAFE_RELEASE(pMapObject);

		//컴포넌트 추가
		CDx11ColliderOBB* pOBB = pMap->AddComponent<CDx11ColliderOBB>("MapColl");
		pOBB->SetOBBInfo(pMapTr,
			iter->RotX* DX11_PI / 180.f, iter->RotY* DX11_PI / 180.f, iter->RotZ* DX11_PI / 180.f
			, iter->ScaleX / 2.f, iter->ScaleY / 2.f, iter->ScaleZ / 2.f);
		SAFE_RELEASE(pOBB);
		SAFE_RELEASE(pMapTr);

		//레이어에 추가	
		m_pLayer->AddObject(pMap);

		QuadTree*	pQuadTree = DX11_GET_SINGLE(CDx11SceneManager)->GetQuadTree();
		pQuadTree->AddStaticObj(pMap);
		SAFE_RELEASE(pQuadTree);

		SAFE_RELEASE(pMap);
	}
}



bool CMapInfo::Init()
{
	Road();
	Move_Object_LEFT();
	Move_Object_RIGHT();
	//Object();
	//Wall();
	//One();
	//Two();
	//Three();
	//Four();

	return true;
}

void CMapInfo::Input(float fTime)
{
}

int CMapInfo::Update(float fTime)
{
	return 0;
}

int CMapInfo::LateUpdate(float fTime)
{

	return 0;
}

void CMapInfo::Render(float fTime)
{
}

CMapInfo * CMapInfo::Clone()
{
	return nullptr;
}




