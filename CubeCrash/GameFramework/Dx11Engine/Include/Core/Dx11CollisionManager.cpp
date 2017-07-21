#include "Dx11CollisionManager.h"
#include "../Component/Dx11Collider.h"
#include "../GameObject/Dx11GameObject.h"
#include "../Scene/Dx11SceneManager.h"
#include "../Scene/QuadTree.h"

DX11_USING

DX11_DEFINITION_SINGLE(CDx11CollisionManager)

CDx11CollisionManager::CDx11CollisionManager()
{
	// �浹ü ������ �οﺯ��
	m_bColliderRender = false;
}


CDx11CollisionManager::~CDx11CollisionManager()
{
}

bool CDx11CollisionManager::Init()
{
	m_ParentObjList.reserve(10);
	m_ChildObjList.reserve(50);
	return true;
}

void CDx11CollisionManager::AddGameObject(CDx11GameObject * pObj)
{
//	�浹 ������Ʈ�� ������ ���ܽ�Ų��.
	CDx11Component* pColl = pObj->FindComponentFromType(CT_COLLIDER);
	if (!pColl)
		return;

	m_ObjList.push_back(pObj);
	SAFE_RELEASE(pColl);

}

void CDx11CollisionManager::Collision(float fTime)
{
	if (m_ParentObjList.size() + m_ChildObjList.size() < 2)
	{
		m_ParentObjList.clear();
		m_ChildObjList.clear();
		return;
	}

	if (m_ParentObjList.size() >= 2)
	{
		vector<CDx11GameObject*>::iterator	iter1;
		vector<CDx11GameObject*>::iterator	iter1End = m_ParentObjList.end() - 1;

		for (iter1 = m_ParentObjList.begin(); iter1 != iter1End; ++iter1)
		{
			vector<CDx11GameObject*>::iterator	iter2;
			vector<CDx11GameObject*>::iterator	iter2End = m_ParentObjList.end();
			for (iter2 = iter1 + 1; iter2 != iter2End; ++iter2)
			{
				//  �� ������Ʈ�� �˻�
				CheckCollision(*iter1, *iter2, fTime);
			}
		}
	}

	// ���� ���� ������� ��� ������Ʈ���� �浹 ���θ� ��� �˻��Ѵ�.
	vector<CDx11GameObject*>::iterator	iter1;
	vector<CDx11GameObject*>::iterator	iter1End = m_ParentObjList.end();
	vector<CDx11GameObject*>::iterator	iter2;
	vector<CDx11GameObject*>::iterator	iter2End = m_ChildObjList.end();

	for (iter1 = m_ParentObjList.begin(); iter1 != iter1End; ++iter1)
	{
		for (iter2 = m_ChildObjList.begin(); iter2 != iter2End; ++iter2)
		{
			//  �� ������Ʈ�� �˻�
			CheckCollision(*iter1, *iter2, fTime);
		}
	}

	m_ParentObjList.clear();
	m_ChildObjList.clear();
	m_ParentObjList.reserve(10);
	m_ChildObjList.reserve(50);
}

void CDx11CollisionManager::SetColliderRender(bool bEnable)
{
	m_bColliderRender = bEnable;

}

void CDx11CollisionManager::ColliderRenderEnable()
{
	m_bColliderRender = !m_bColliderRender;
}

bool CDx11CollisionManager::GetColliderRender()
{
	return m_bColliderRender;
}

void CDx11CollisionManager::SetParentList(QuadTree * pNode)
{
	m_ParentObjList.clear();
	m_ChildObjList.clear();

	vector<class CDx11GameObject*>::iterator	iter;
	vector<class CDx11GameObject*>::iterator iterEnd = pNode->m_StaticObjList.end();
	for (iter = pNode->m_StaticObjList.begin(); iter != iterEnd; ++iter)
	{
		m_ParentObjList.push_back(*iter);
	}
	iterEnd = pNode->m_DynamicObjList.end();
	for (iter = pNode->m_DynamicObjList.begin(); iter != iterEnd; ++iter)
	{
		m_ParentObjList.push_back(*iter);
	}

	if (pNode->m_pChild[0])
	{
		for (int i = 0; i < 4; ++i)
		{
			SetChildList(pNode->m_pChild[i]);
		}
	}
}

void CDx11CollisionManager::SetChildList(QuadTree * pNode)
{
	vector<class CDx11GameObject*>::iterator	iter;
	vector<class CDx11GameObject*>::iterator iterEnd = pNode->m_StaticObjList.end();
	for (iter = pNode->m_StaticObjList.begin(); iter != iterEnd; ++iter)
	{
		m_ChildObjList.push_back(*iter);
	}
	iterEnd = pNode->m_DynamicObjList.end();
	for (iter = pNode->m_DynamicObjList.begin(); iter != iterEnd; ++iter)
	{
		m_ChildObjList.push_back(*iter);
	}

	if (pNode->m_pChild[0])
	{
		for (int i = 0; i < 4; ++i)
		{
			SetChildList(pNode->m_pChild[i]);
		}
	}
}


bool CDx11CollisionManager::CheckCollision(CDx11GameObject * pObj1, CDx11GameObject * pObj2, float fTime)
{
	// �� ������Ʈ�� ������ �ִ� ��� �浹 ������Ʈ�� �޾ƿ´�.

	list<CDx11Component*>*	pColl1 = pObj1->FindComponentsFromType(CT_COLLIDER);
	list<CDx11Component*>*	pColl2 = pObj2->FindComponentsFromType(CT_COLLIDER);

	list<CDx11Component*>::iterator	iter1;
	list<CDx11Component*>::iterator	iter1End = pColl1->end();
	list<CDx11Component*>::iterator	iter2;
	list<CDx11Component*>::iterator	iter2End = pColl2->end();

	for (iter1 = pColl1->begin(); iter1 != iter1End; ++iter1)
	{
		for (iter2 = pColl2->begin(); iter2 != iter2End; ++iter2)
		{
			CDx11Collider* pC1 = (CDx11Collider*)*iter1;
			CDx11Collider* pC2 = (CDx11Collider*)*iter2;
			
			// �浹�� ���� ���
			if (pC1->Collision(pC2))
			{
				// �浹ü�� ���ٸ� �߰����ְ�,
				// �浹 �����Լ��� ȣ���Ѵ�.
				if (!pC1->CheckCollider(pC2))
				{
					pC1->AddCollider(pC2);
					pC2->AddCollider(pC1);

					pObj1->OnCollisionEnter(pC2, fTime);
					pObj2->OnCollisionEnter(pC1, fTime);
				}
				// �浹ü�� �ִٸ� �浹 �� �Լ��� ȣ���Ѵ�.
				else
				{
					pObj1->OnCollision(pC2, fTime);
					pObj2->OnCollision(pC1, fTime);
				}
			}
			// �浹�� �ȉ��� ���
			else
			{
				// �ִٸ� �����ش�.
				if (pC1->CheckCollider(pC2))
				{
					pC1->EraseCollider(pC2);
					pC2->EraseCollider(pC1);

					pObj1->OnCollisionExit(pC2, fTime);
					pObj2->OnCollisionExit(pC1, fTime);
				}
			}

		}
	}

	return true;
}
