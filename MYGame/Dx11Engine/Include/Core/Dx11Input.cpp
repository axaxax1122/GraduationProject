#include "Dx11Input.h"
#include "../GameObject/Dx11GameObject.h"
#include "../Component/Dx11Mouse.h"
#include "../Component/Dx11ColliderPoint.h"
#include "Dx11CollisionManager.h"
#include "../Component/Dx11Transform.h"
#include "Dx11Device.h"

DX11_USING

DX11_DEFINITION_SINGLE(CDx11Input)

CDx11Input::CDx11Input() :
	m_pCreateKey(NULL),
	m_pMouseObj(NULL),
	m_pMouse(NULL),
	m_bInput(true)
{
}

CDx11Input::~CDx11Input()
{
	SAFE_RELEASE(m_pMouse);
	SAFE_RELEASE(m_pMouseObj);
	Safe_Delete_Map(m_mapKey);
}

bool CDx11Input::Init(HWND hWnd)
{

	CreateKey("MoveFront", 'W');
	CreateKey('S', "MoveBack");
	CreateKey("RotZFront", 'D');
	CreateKey('A', "RotZBack");
	CreateKey("Fire", VK_SPACE);
	CreateKey("MouseLButton", VK_LBUTTON);
	CreateKey("MouseRButton", VK_RBUTTON);

	// Sample
	CreateKey(VK_CONTROL, '1', "Skill1");

	//------------------------------------- ���콺 ------------------------------------- 
	m_pMouseObj = CDx11GameObject::Create("Mouse");

	//������Ʈ ����
	m_pMouse = m_pMouseObj->AddComponent<CDx11Mouse>("Mouse");//�浹 ó���Ϸ���!
	m_pMouse->SetWindowHandle(hWnd);

	//������Ʈ ����
	//CDx11ColliderPoint*	pColl = m_pMouseObj->AddComponent<CDx11ColliderPoint>("Mouse");
	//SAFE_RELEASE(pColl);

	m_hWnd = hWnd;//�߿�!

	return true;
}

int CDx11Input::Update(float fTime)
{
	unordered_map<string, PKEYINFO>::iterator	iter;
	unordered_map<string, PKEYINFO>::iterator	iterEnd = m_mapKey.end();

	for (iter = m_mapKey.begin(); iter != iterEnd; ++iter)
	{
		UINT	iKey = 0;
		vector<UINT>::iterator	iter1;
		vector<UINT>::iterator	iter1End = iter->second->vecKey.end();

		for (iter1 = iter->second->vecKey.begin(); iter1 != iter1End; ++iter1)
		{
			if (GetAsyncKeyState(*iter1) & 0x8000)
				++iKey;
		}

		if (iKey == iter->second->vecKey.size())
		{
			if (!iter->second->bDown && !iter->second->bPush)
				iter->second->bDown = true;

			else if (iter->second->bDown && !iter->second->bPush)
			{
				iter->second->bPush = true;
				iter->second->bDown = false;
			}
		}

		else
		{
			if (iter->second->bDown || iter->second->bPush)
			{
				iter->second->bUp = true;
				iter->second->bDown = false;
				iter->second->bPush = false;
			}

			else
				iter->second->bUp = false;
		}
		//���콺 ��ư
		if (iter->first == "MouseLButton")
		{
			m_pMouse->SetLButtonInfo(iter->second->bDown, iter->second->bPush,
				iter->second->bUp);
		}

		else if (iter->first == "MouseRButton")
		{
			m_pMouse->SetRButtonInfo(iter->second->bDown, iter->second->bPush,
				iter->second->bUp);
		}
	}

	//--------------------------------------- ���콺 ���� ---------------------------------------

	GetCursorPos(&m_ptMouse); // ���콺 ��ġ�� ���Ѵ�.
							  // ��ũ�� ��ǥ�� Ŭ���̾�Ʈ ��ǥ�� ��ȯ�Ѵ�.
	ScreenToClient(m_hWnd, &m_ptMouse);

	CDx11Transform*	pTransform = m_pMouseObj->GetTransform();
	pTransform->SetWorldPos(m_ptMouse.x, m_ptMouse.y, 0.f);
	SAFE_RELEASE(pTransform);

	m_pMouseObj->Update(fTime);
	m_pMouseObj->LateUpdate(fTime);
	DX11_GET_SINGLE(CDx11CollisionManager)->AddGameObject(m_pMouseObj);//�浹 ����.

	return 0;

	return 0;
}

bool CDx11Input::KeyDown(const string & strKey)
{
	PKEYINFO	pKey = FindKey(strKey);

	if (!pKey)
		return false;

	return pKey->bDown;
}

bool CDx11Input::KeyPush(const string & strKey)
{
	PKEYINFO	pKey = FindKey(strKey);

	if (!pKey)
		return false;

	return pKey->bPush;
}

bool CDx11Input::KeyUp(const string & strKey)
{
	PKEYINFO	pKey = FindKey(strKey);

	if (!pKey)
		return false;

	return pKey->bUp;
}

PKEYINFO CDx11Input::FindKey(const string & strKey)
{
	unordered_map<string, PKEYINFO>::iterator	iter = m_mapKey.find(strKey);

	if (iter == m_mapKey.end())
		return NULL;

	return iter->second;
}


void CDx11Input::Wheel(short sWheel)
{
	m_sWheel = sWheel / 120;
	m_pMouse->SetWheel(m_sWheel);
}

void CDx11Input::ClearInfo()
{
	m_sWheel = 0;
	m_pMouse->ClearInfo();
}

void CDx11Input::SetInput(bool bInput)
{
	m_bInput = bInput;
}

CDx11Mouse* CDx11Input::GetMouse()
{
	m_pMouse->AddRef();
	return m_pMouse;
}

CDx11GameObject * CDx11Input::GetMouseObj()
{
	return m_pMouseObj;
}
