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
	//충돌 컴포넌트 있는지 확인!
	CDx11Component*	pColl = pObj->FindComponentFromType(CT_COLLIDER);
	if (!pColl)
		return;

	//충돌 검사할것 푸쉬.
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

	//충돌 확인
	//충돌 리스트에있는것들을 한번씩 검사한다.
	for (iter = m_ObjList.begin(); iter != iterEnd; ++iter)
	{
		vector<CDx11GameObject*>::iterator	iter1;
		vector<CDx11GameObject*>::iterator	iter1End = m_ObjList.end();
		
		for (iter1 = iter + 1; iter1 != iter1End; ++iter1)
		{
			//-충돌 확인-
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
			//CDx11ColliderSphere이 컴포넌트 리스트에 있기때문에
			//CDx11Component -> CDx11Collider로 형변환하여
			//검사한다.
			CDx11Collider*	pColl1 = (CDx11Collider*)*iter1;
			CDx11Collider*	pColl2 = (CDx11Collider*)*iter2;

			//충돌ㅇㅇ
			if (pColl1->Collision(pColl2))
			{
				//충돌중이 아니였을때
				if (!pColl1->CheckCollider(pColl2))
				{
					pColl1->AddCollider(pColl2);
					pColl2->AddCollider(pColl1);

					//들어온 게임오브젝트의 충돌함수 소환
					pObj1->OnCollisionEnter(pColl2, fTime);
					pObj2->OnCollisionEnter(pColl1, fTime);
				}
				//충돌중 이였을때
				else
				{
					pObj1->OnCollision(pColl2, fTime);
					pObj2->OnCollision(pColl1, fTime);
				}
			}			
			// 충돌ㄴㄴ
			else
			{
				//충돌중이 였을때
				if (pColl1->CheckCollider(pColl2))
				{
					//List 삭제
					pColl1->EraseCollider(pColl2);
					pColl2->EraseCollider(pColl1);

					//탈출
					pObj1->OnCollisionExit(pColl2, fTime);
					pObj2->OnCollisionExit(pColl1, fTime);
				}
			}
		}
	}
	return true;
}
