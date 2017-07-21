#include "Dx11SceneManager.h"
#include "Dx11Scene.h"
#include "../Core/Dx11Device.h"
#include "QuadTree.h"
#include "../Core/Dx11CollisionManager.h"

DX11_USING

DX11_DEFINITION_SINGLE(CDx11SceneManager)

CDx11SceneManager::CDx11SceneManager()	:
	m_pCurScene(NULL),
	m_pNextScene(NULL),
	m_pPrevScene(NULL),
	m_pQuadTree(NULL)
{
	SetTypeName<CDx11SceneManager>();
}

CDx11SceneManager::~CDx11SceneManager()
{
	SAFE_RELEASE(m_pPrevScene);
	SAFE_RELEASE(m_pNextScene);
	SAFE_RELEASE(m_pCurScene);
	SAFE_RELEASE(m_pQuadTree);
}

CDx11Scene * CDx11SceneManager::GetCurrentScene()
{
	m_pCurScene->AddRef();

	return m_pCurScene;
}

QuadTree * CDx11SceneManager::GetQuadTree()
{
	m_pQuadTree->AddRef();
	return m_pQuadTree;
}

bool CDx11SceneManager::Init()
{
	m_pCurScene = CreateScene();

	// 쿼드트리 생성
	m_pQuadTree = new QuadTree;
	m_pQuadTree->SetParent(Vec3(-300.f, 0.f, 300.f), Vec3(300.f, 0.f, 300.f),
		Vec3(-300.f, 0.f, -300.f), Vec3(300.f, 0.f, -300.f));

	return true;
}

void CDx11SceneManager::Input(float fTime)
{
	m_pCurScene->Input(fTime);
}

int CDx11SceneManager::Update(float fTime)
{
	m_pCurScene->Update(fTime);

	return Change();
}

int CDx11SceneManager::LateUpdate(float fTime)
{
	m_pCurScene->LateUpdate(fTime);

	return Change();
}

void CDx11SceneManager::Collision(QuadTree* pNode, float fTime)
{
//	m_pCurScene->Collision(fTime);
	DX11_GET_SINGLE(CDx11CollisionManager)->SetParentList(pNode);
	DX11_GET_SINGLE(CDx11CollisionManager)->Collision(fTime);

	if (pNode->m_pChild[0])
	{
		for (int i = 0; i < 4; ++i)
		{
			Collision(pNode->m_pChild[i], fTime);
		}
	}
}

void CDx11SceneManager::Render(float fTime)
{
	DX11_GET_SINGLE(CDx11Device)->ClearTarget();

	m_pCurScene->Render(fTime);

	DX11_GET_SINGLE(CDx11Device)->Present();
}

CDx11Scene * CDx11SceneManager::CreateScene(const string & strTag)
{
	CDx11Scene*	pScene = new CDx11Scene;

	pScene->SetTag(strTag);

	if (!pScene->Init())
	{
		SAFE_RELEASE(pScene);
		return NULL;
	}

	return pScene;
}


void CDx11SceneManager::ChangeScene(CDx11Scene * pNext)
{
	m_pNextScene = pNext;
}

SCENE_CHANGE CDx11SceneManager::Change()
{
	if (m_pNextScene)
	{
		SAFE_RELEASE(m_pCurScene);
		m_pCurScene = m_pNextScene;
		m_pNextScene = NULL;
		return SC_CHANGE;
	}

	return SC_NONE;
}
