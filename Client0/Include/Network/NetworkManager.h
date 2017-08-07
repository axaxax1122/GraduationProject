#pragma once

//#include "../Network.h"
#include "InGameNetwork.h"
#include "ZEROCircularQueue.h"
#include "GameObject/Dx11GameObject.h"
#include "Scene/Dx11Layer.h"
#include "ZEROMultiThreadSync.h"

DX11_USING

class CNetworkManager
{
public:
	CNetworkManager();
	~CNetworkManager();

private:
	static CNetworkManager*	m_pInst;

public:
	static CNetworkManager* GetInst()
	{
		if (!m_pInst)
			m_pInst = new CNetworkManager;
		return m_pInst;
	}

	static void DestroyInst()
	{
		if (m_pInst)
		{
			delete	m_pInst;
			m_pInst = NULL;
		}
	}

private:
	class CUserSession* m_pGameSession;
	SOCKET				m_hSocket;
	SOCKADDR_IN	m_tAddr;
	CInGameNetwork*	m_pEventSelect;
	CZEROCircularQueue	m_InGameQueue;
	list<CDx11GameObject*> m_LoginObj;

public:
	CInGameNetwork* GetInGameNetwork();

public:
	void AddLoginObj(CDx11GameObject* pObj);
	bool EmptyLoginUser()
	{
		return m_LoginObj.empty();
	}
	void AddLayer(CDx11Layer* pLayer)
	{
		list<CDx11GameObject*>::iterator iter;
		
		for (iter = m_LoginObj.begin(); iter != m_LoginObj.end(); ++iter)
		{
			pLayer->AddObject((*iter));
			SAFE_RELEASE((*iter));
		}

		m_LoginObj.clear();
	}

public:
	SOCKET GetSocket()	const;

public:
	volatile bool Init(char* pIP, short sPort);

	bool Login(const char* pID, const char* pPass);
	bool InitSucc();
	bool MovePlayer(const Vec3 &vPos);
	bool RotatePlayer(const Vec3 &vRot);
	bool JumpPlayer(const Vec3 &vPos);
	bool AnimState(int iStateFlag);
	bool ShootPlayer(const Vec3 &vPos, const Vec3 &vScale, const Vec3 &vDir);
	bool HeatPlayer();
	bool TimeStart();
};

