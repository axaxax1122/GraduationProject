#pragma once
#include "ZEROEventSelect.h"
#include "../NetWork.h"
#include "../SceneScript/MainScene.h"

class CInGameNetwork :
	public CZEROEventSelect
{
public:
	CInGameNetwork();
	~CInGameNetwork();

private:
	class CUserSession* m_pSession;
	CMainScene* m_pMainScene;

public:
	void SetSession(class CUserSession* pSession);
	void SetMainScene(CMainScene* pMainScene);

public:
	//virtual VOID	OnIoWrote(VOID);
	virtual VOID	OnIoRead(VOID);
	virtual VOID	OnIoConnected(VOID);
	virtual VOID	OnIoDisconnected(VOID);

private:
	void CreateUser(BYTE* pPacket, DWORD dwLength);
};

