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
#include "Component/Dx11ColliderHexahedron.h"
#include "Component/Dx11Transform.h"

CMapInfo::CMapInfo()
{
}
CMapInfo::CMapInfo(const CMapInfo & map)
{
}
CMapInfo::~CMapInfo()
{	
}
vector<FBXINFO> *CMapInfo::GetInfo()
{
	return &m_vInfo;
}
void CMapInfo::Road()
{
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
			inFile >>info.name >>info.PosX >> info.PosY >> info.PosZ >>
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
		string name = iter->MeshName;
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
		CDx11Renderer* pRenderer = pMap->AddComponent<CDx11Renderer>("PlayerRenderer");
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
		SAFE_RELEASE(pMapTr);

		//컴포넌트 추가
		Dx11ColliderHexahedron* pHexahedron = pMap->AddComponent<Dx11ColliderHexahedron>("MapColl");
		pHexahedron->SetInfo(Vec3(0, 0, 0), Vec3(iter->RotX, iter->RotY, iter->RotZ), Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ));
		SAFE_RELEASE(pHexahedron);

		//레이어에 추가	
		m_pLayer->AddObject(pMap);

		SAFE_RELEASE(pMap);
	}
}

void CMapInfo::Well()
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
		string name = iter->MeshName;
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
		CDx11Renderer* pRenderer = pMap->AddComponent<CDx11Renderer>("PlayerRenderer");
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
		SAFE_RELEASE(pMapTr);

		//컴포넌트 추가
		Dx11ColliderHexahedron* pHexahedron = pMap->AddComponent<Dx11ColliderHexahedron>("MapColl");
		pHexahedron->SetInfo(Vec3(0, 0, 0), Vec3(iter->RotX, iter->RotY, iter->RotZ), Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ));
		SAFE_RELEASE(pHexahedron);


		//레이어에 추가	
		m_pLayer->AddObject(pMap);

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
		string name = iter->MeshName;
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
		CDx11Renderer* pRenderer = pMap->AddComponent<CDx11Renderer>("PlayerRenderer");
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
		SAFE_RELEASE(pMapTr);

		//컴포넌트 추가
		Dx11ColliderHexahedron* pHexahedron = pMap->AddComponent<Dx11ColliderHexahedron>("MapColl");
		pHexahedron->SetInfo(Vec3(0, 0, 0), Vec3(iter->RotX, iter->RotY, iter->RotZ), Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ));
		SAFE_RELEASE(pHexahedron);

		//레이어에 추가	
		m_pLayer->AddObject(pMap);

		SAFE_RELEASE(pMap);
	}	
}

void CMapInfo::One()
{
	//원점을 찾는다.
	CDx11Layer*	pLayer = m_pScene->FindLayer("DefaultLayer");
	CDx11GameObject* Onegum = pLayer->FindObjectFromTag("FloorObject");
	CDx11Transform* OnegumTr = Onegum->GetTransform();
	
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
			inFile >> info.name>> 
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
		// += path
		string name = iter->MeshName;
		name += ".FBX";

		// string -> TCHAR	
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
		CDx11Renderer* pRenderer = pMap->AddComponent<CDx11Renderer>("PlayerRenderer");
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

		pMapTr->SetWorldRot(Vec3(-((iterOneZumPos->second.RotX-(iter->RotX))* DX11_PI / 180.f),
			-((iterOneZumPos->second.RotY - (iter->RotY))* DX11_PI / 180.f),
			-((iterOneZumPos->second.RotZ - (iter->RotZ))* DX11_PI / 180.f)));

		pMapTr->SetWorldPos(Vec3(iter->PosX, iter->PosY, iter->PosZ));
		SAFE_RELEASE(pMapTr);

		//컴포넌트 추가
		Dx11ColliderHexahedron* pHexahedron = pMap->AddComponent<Dx11ColliderHexahedron>("MapColl");
		pHexahedron->SetInfo(Vec3(0, 0, 0), Vec3(iter->RotX, iter->RotY, iter->RotZ), Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ));
		SAFE_RELEASE(pHexahedron);


		//레이어에 추가	
		m_pLayer->AddObject(pMap);
		SAFE_RELEASE(pMap);
	}

	SAFE_RELEASE(OnegumTr);
	SAFE_RELEASE(Onegum);
	SAFE_RELEASE(pLayer);
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
		// += path
		string name = iter->MeshName;
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
		CDx11Renderer* pRenderer = pMap->AddComponent<CDx11Renderer>("PlayerRenderer");
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
		SAFE_RELEASE(pMapTr);

		//컴포넌트 추가
		Dx11ColliderHexahedron* pHexahedron = pMap->AddComponent<Dx11ColliderHexahedron>("MapColl");
		pHexahedron->SetInfo(Vec3(0, 0, 0), Vec3(iter->RotX, iter->RotY, iter->RotZ), Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ));
		SAFE_RELEASE(pHexahedron);


		//레이어에 추가	
		m_pLayer->AddObject(pMap);

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
		// += path
		string name = iter->MeshName;
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
		CDx11Renderer* pRenderer = pMap->AddComponent<CDx11Renderer>("PlayerRenderer");
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
		SAFE_RELEASE(pMapTr);

		//컴포넌트 추가
		Dx11ColliderHexahedron* pHexahedron = pMap->AddComponent<Dx11ColliderHexahedron>("MapColl");
		pHexahedron->SetInfo(Vec3(0, 0, 0), Vec3(iter->RotX, iter->RotY, iter->RotZ), Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ));
		SAFE_RELEASE(pHexahedron);


		//레이어에 추가	
		m_pLayer->AddObject(pMap);

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
		// += path
		string name = iter->MeshName;
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
		CDx11Renderer* pRenderer = pMap->AddComponent<CDx11Renderer>("PlayerRenderer");
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
		SAFE_RELEASE(pMapTr);

		//컴포넌트 추가
		Dx11ColliderHexahedron* pHexahedron = pMap->AddComponent<Dx11ColliderHexahedron>("MapColl");
		pHexahedron->SetInfo(Vec3(0, 0, 0), Vec3(iter->RotX, iter->RotY, iter->RotZ), Vec3(iter->ScaleX, iter->ScaleY, iter->ScaleZ));
		SAFE_RELEASE(pHexahedron);


		//레이어에 추가	
		m_pLayer->AddObject(pMap);

		SAFE_RELEASE(pMap);
	}
}

bool CMapInfo::Init()
{
	Road();
	Object();	
	Well();
	One();
	Two();
	Three();
	Four();

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

void CMapInfo::OnCollisionEnter(CDx11Collider * pColl, float fTime)
{
}

void CMapInfo::OnCollision(CDx11Collider * pColl, float fTime)
{
}

void CMapInfo::OnCollisionExit(CDx11Collider * pColl, float fTime)
{
}
