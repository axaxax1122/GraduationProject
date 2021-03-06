#pragma once
#include "../Dx11Object.h"

DX11_BEGIN

class DX11_DLL CDx11SceneManager :
	public CDx11Object
{
	DX11_DECLARE_SINGLE(CDx11SceneManager)

private:
	class CDx11Scene*		m_pCurScene;
	class CDx11Scene*		m_pNextScene;
	class CDx11Scene*		m_pPrevScene;
	class QuadTree*			m_pQuadTree;

public:
	CDx11Scene*	GetCurrentScene();
	QuadTree*	GetQuadTree();

public:
	bool Init();
	void Input(float fTime);
	int Update(float fTime);
	int LateUpdate(float fTime);
	void Collision(QuadTree*,float fTime);
	void Render(float fTime);
	class CDx11Scene* CreateScene(const string& strTag = "Scene");
	void ChangeScene(class CDx11Scene* pNext);

private:
	SCENE_CHANGE Change();
};

DX11_END