
#include "UserSession.h"
#include "ZEROStream.h"
#include "../Client.h"
#include "NetworkManager.h"

CNetworkManager* CNetworkManager::m_pInst = NULL;

CNetworkManager::CNetworkManager()	:
	m_pEventSelect(NULL),
	m_hSocket(NULL),
	m_pGameSession(NULL)
{
	WSADATA		wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
}

CNetworkManager::~CNetworkManager()
{
	m_pGameSession->End();
	SAFE_DELETE(m_pGameSession);

	delete	m_pEventSelect;
	closesocket(m_hSocket);
	WSACleanup();
}

CInGameNetwork * CNetworkManager::GetInGameNetwork()
{
	return m_pEventSelect;
}

void CNetworkManager::AddLoginObj(CDx11GameObject * pObj)
{
	m_LoginObj.push_back(pObj);
}


SOCKET CNetworkManager::GetSocket() const
{
	return m_hSocket;
}

volatile bool CNetworkManager::Init(char * pIP, short sPort)
{
	m_pGameSession = new CUserSession;
	m_pGameSession->Begin();
	m_pGameSession->TcpBind();

	m_pEventSelect = new CInGameNetwork;
	m_pEventSelect->Begin(m_pGameSession->GetSocket());
	
//	const char opt_val = TRUE;
//	setsockopt(m_pGameSession->GetSocket(), IPPROTO_TCP, TCP_NODELAY, &opt_val, sizeof(opt_val));
	
	m_pEventSelect->SetSession(m_pGameSession);
	m_pGameSession->Connect(pIP, sPort);

	return true;
}

bool CNetworkManager::Login(const char * pID, const char * pPass)
{
	BYTE	strPacket[PACKET_SIZE] = {};

	CZEROStreamSP	stream;

	stream->SetBuffer(strPacket);

	stream->WriteInt32(strlen(pID));
	stream->WriteBytes((BYTE*)pID, strlen(pID));
	stream->WriteInt32(strlen(pPass));
	stream->WriteBytes((BYTE*)pPass, strlen(pPass));

	m_pGameSession->WritePacket(PP_LOGIN, strPacket, stream->GetLength());
	m_pGameSession->WriteComplete();
	return true;
}

bool CNetworkManager::InitSucc()
{
	BYTE	strPacket[PACKET_SIZE] = {};
	CZEROStreamSP	stream;
	stream->SetBuffer(strPacket);
	//
	m_pGameSession->WritePacket(PP_CHARACTER_INIT_SUCC, strPacket, stream->GetLength());
	m_pGameSession->WriteComplete();
	return true;
}

bool CNetworkManager::MovePlayer(const Vec3 &vPos)
{
	BYTE	strPacket[PACKET_SIZE] = {};
	CZEROStreamSP	stream;
	stream->SetBuffer(strPacket);
	stream->WriteFloat(vPos.x);
	stream->WriteFloat(vPos.y);
	stream->WriteFloat(vPos.z);
	m_pGameSession->WritePacket(PP_CHARACTER_MOVE, strPacket, stream->GetLength());
	m_pGameSession->WriteComplete();
	return true;
}

bool CNetworkManager::RotatePlayer(const Vec3 & vRot)
{
	BYTE	strPacket[PACKET_SIZE] = {};
	CZEROStreamSP	stream;
	stream->SetBuffer(strPacket);
	stream->WriteFloat(vRot.x);
	stream->WriteFloat(vRot.y);
	stream->WriteFloat(vRot.z);
	m_pGameSession->WritePacket(PP_CHARACTER_ROTATION, strPacket, stream->GetLength());
	m_pGameSession->WriteComplete();
	return true;
}

bool CNetworkManager::JumpPlayer(const Vec3 & vPos)
{
	BYTE	strPacket[PACKET_SIZE] = {};
	CZEROStreamSP	stream;
	stream->SetBuffer(strPacket);
	stream->WriteFloat(vPos.x);
	stream->WriteFloat(vPos.y);
	stream->WriteFloat(vPos.z);
	m_pGameSession->WritePacket(PP_JUMP_PLAYER, strPacket, stream->GetLength());
	m_pGameSession->WriteComplete();
	return true;

}

bool CNetworkManager::AnimState(int iStateFlag)
{
	BYTE   strPacket[PACKET_SIZE] = {};
	CZEROStreamSP   stream;
	stream->SetBuffer(strPacket);
	stream->WriteInt32(iStateFlag);
	m_pGameSession->WritePacket(PP_PLAYER_ANIMSTATE, strPacket, stream->GetLength());
	m_pGameSession->WriteComplete();
	return true;

}

bool CNetworkManager::ShootPlayer(const Vec3 & vPos, const Vec3 & vScale, const Vec3 & vDir)
{
	BYTE   strPacket[PACKET_SIZE] = {};
	CZEROStreamSP   stream;
	stream->SetBuffer(strPacket);
	stream->WriteFloat(vPos.x);
	stream->WriteFloat(vPos.y);
	stream->WriteFloat(vPos.z);

	stream->WriteFloat(vScale.x);
	stream->WriteFloat(vScale.y);
	stream->WriteFloat(vScale.z);

	stream->WriteFloat(vDir.x);
	stream->WriteFloat(vDir.y);
	stream->WriteFloat(vDir.z);

	m_pGameSession->WritePacket(PP_PLAYER_SHOOT, strPacket, stream->GetLength());
	m_pGameSession->WriteComplete();
	return true;
}

bool CNetworkManager::HeatPlayer()
{
	BYTE   strPacket[PACKET_SIZE] = {};
	CZEROStreamSP   stream;
	stream->SetBuffer(strPacket);
	m_pGameSession->WritePacket(PP_PLAYER_HEAT, strPacket, stream->GetLength());
	m_pGameSession->WriteComplete();
	return true;
}

bool CNetworkManager::TimeStart()
{
	BYTE	strPacket[PACKET_SIZE] = {};
	CZEROStreamSP	stream;
	stream->SetBuffer(strPacket);
	m_pGameSession->WritePacket(PP_TIME_START, strPacket, stream->GetLength());
	m_pGameSession->WriteComplete();
	return true;
}

//bool CNetworkManager::GameEnter()
//{
//	BYTE	strPacket[PACKET_SIZE] = {};
//
//	CZEROStreamSP	stream;
//
//	stream->SetBuffer(strPacket);
//
//	m_pGameSession->WritePacket(PP_GAMEENTER, strPacket, stream->GetLength());
//
//	return true;
//}


