#pragma once
#include "../Dx11Object.h"

DX11_BEGIN

class DX11_DLL CDx11CollisionManager :
	public CDx11Object
{
	DX11_DECLARE_SINGLE(CDx11CollisionManager)

private:
	vector<class CDx11GameObject*>	m_ObjList;
//#if defined(DEBUG) | defined(_DEBUG)
	bool	m_bColliderRender;
//#endif
	vector<class CDx11GameObject*>	m_ParentObjList;
	vector<class CDx11GameObject*>	m_ChildObjList;


public:
	bool Init();
	void AddGameObject(class CDx11GameObject* pObj);
	void Collision(float fTime);
	void SetColliderRender(bool bEnable);
	void ColliderRenderEnable();
	bool GetColliderRender();
	void SetParentList(class QuadTree* pNode);
	void SetChildList(class QuadTree* pNode);

private:
	bool CheckCollision(CDx11GameObject* pObj1, CDx11GameObject* pObj2, float fTime);

};

DX11_END