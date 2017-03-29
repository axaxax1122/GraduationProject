#include "Dx11CollisionManager.h"
#include "../Component/Dx11Collider.h"
#include "../GameObject/Dx11GameObject.h"

DX11_USING
DX11_DEFINITION_SINGLE(CDx11CollisionManager)

CDx11CollisionManager::CDx11CollisionManager()
{
}

CDx11CollisionManager::~CDx11CollisionManager()
{
}

bool CDx11CollisionManager::Init()
{
	m_ObjList.reserve(500);

	return true;
}

void CDx11CollisionManager::AddGameObject(CDx11GameObject * pObj)
{
	//�浹 ������Ʈ �ִ��� Ȯ��!
	CDx11Component*	pColl = pObj->FindComponentFromType(CT_COLLIDER);
	if (!pColl)
		return;

	//�浹 �˻��Ұ� Ǫ��.
	m_ObjList.push_back(pObj);

	SAFE_RELEASE(pColl);
}

void CDx11CollisionManager::Collision(float fTime)
{
	if (m_ObjList.size() < 2)
	{
		m_ObjList.clear();
		return;
	}
	vector<CDx11GameObject*>::iterator	iter;
	vector<CDx11GameObject*>::iterator	iterEnd = m_ObjList.end() - 1;

	//�浹 Ȯ��
	//�浹 ����Ʈ���ִ°͵��� �ѹ��� �˻��Ѵ�.
	for (iter = m_ObjList.begin(); iter != iterEnd; ++iter)
	{
		vector<CDx11GameObject*>::iterator	iter1;
		vector<CDx11GameObject*>::iterator	iter1End = m_ObjList.end();
		
		for (iter1 = iter + 1; iter1 != iter1End; ++iter1)
		{
			//-�浹 Ȯ��-
			Collision(*iter, *iter1, fTime);
		}
	}

	m_ObjList.clear();
	m_ObjList.reserve(500);
}

bool CDx11CollisionManager::Collision(CDx11GameObject * pObj1, 
	CDx11GameObject * pObj2, float fTime)
{

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
			//CDx11ColliderSphere�� ������Ʈ ����Ʈ�� �ֱ⶧����
			//CDx11Component -> CDx11Collider�� ����ȯ�Ͽ�
			//�˻��Ѵ�.
			CDx11Collider*	pColl1 = (CDx11Collider*)*iter1;
			CDx11Collider*	pColl2 = (CDx11Collider*)*iter2;

			//�浹����
			if (pColl1->Collision(pColl2))
			{
				//�浹���� �ƴϿ�����
				if (!pColl1->CheckCollider(pColl2))
				{
					pColl1->AddCollider(pColl2);
					pColl2->AddCollider(pColl1);

					//���� ���ӿ�����Ʈ�� �浹�Լ� ��ȯ
					pObj1->OnCollisionEnter(pColl2, fTime);
					pObj2->OnCollisionEnter(pColl1, fTime);
				}
				//�浹�� �̿�����
				else
				{
					pObj1->OnCollision(pColl2, fTime);
					pObj2->OnCollision(pColl1, fTime);
				}
			}			
			// �浹����
			else
			{
				//�浹���� ������
				if (pColl1->CheckCollider(pColl2))
				{
					//List ����
					pColl1->EraseCollider(pColl2);
					pColl2->EraseCollider(pColl1);

					//Ż��
					pObj1->OnCollisionExit(pColl2, fTime);
					pObj2->OnCollisionExit(pColl1, fTime);
				}
			}
		}
	}
	return true;
}
