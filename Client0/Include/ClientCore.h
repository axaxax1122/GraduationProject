#pragma once

#include "Client.h"
#include "Network\NetworkManager.h"
#include "Network.h"

class CClientCore
{
	DX11_DECLARE_SINGLE(CClientCore)

private:
	char			m_strID[ID_SIZE];
	char			m_strPass[PASS_SIZE];
	char			m_strIP[16];
	short			m_sPort;

public:
	const char* GetIP()	const;
	short GetPort()	const;
	const char* GetID() const;
	const char* GetPass()	const;

public:
	bool Init(wchar_t* pTitle, wchar_t* pClass, HINSTANCE hInst,
		int iIconID, int iSmallIconID, RESOLUTION_TYPE eRT,
		bool bWindowMode, WNDPROC pProc = NULL);
	int Run();
};

