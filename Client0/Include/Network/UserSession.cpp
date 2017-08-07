#include "UserSession.h"



CUserSession::CUserSession()
{
}


CUserSession::~CUserSession()
{
}

void CUserSession::SetConnected(BOOL bConnect)
{
	m_bConnect = bConnect;
}

void CUserSession::SetSerialNumber(INT64 iSerialNumber)
{
	m_iSerialNumber = iSerialNumber;
}

INT64 CUserSession::GetSerialNumber() const
{
	return m_iSerialNumber;
}

BOOL CUserSession::Begin(VOID)
{
	m_bConnect = FALSE;

	return CZEROPacketSession::Begin();
}

BOOL CUserSession::End(VOID)
{
	return CZEROPacketSession::End();
}

BOOL CUserSession::Restart(SOCKET hListen)
{
	// 동기화 개체
	CZEROThreadSync Sync;

	// 개체를 종료해줍니다.
	End();

	// 개체를 다시 시작하고 Accept상태로 만들어줍니다.
	// Accept를 하기 위해서는 반드시 hListenSocket 값이 필요합니다.
	// 이 값은 CZEROServerIocp의 멤버인 m_pListen의 GetSocket으로 받아올 수 있습니다.
	return Begin() && Accept(hListen);
}
