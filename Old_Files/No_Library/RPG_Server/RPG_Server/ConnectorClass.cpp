#include "ConnectorClass.h"

ConnectorClass::ConnectorClass()
{
}

ConnectorClass::~ConnectorClass()
{
}

void ConnectorClass::Init()
{
	Session::Init();

	m_ipEndPoint = IpEndPoint("211.221.147.29", 30003);
}

bool ConnectorClass::Connect()
{
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
	{
		printf("Error - Can not load 'winsock.dll' file\n");
	}

	// 1. 家南积己
	m_socket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_socket == INVALID_SOCKET)
	{
		printf("Error - Invalid socket\n");

		return false;
	}

	// 2. 楷搬夸没
	if (connect(m_socket, (SOCKADDR*)&m_ipEndPoint, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		printf("Error - Fail to connect\n");
		// 4. 家南辆丰
		closesocket(m_socket);
		// Winsock End
		WSACleanup();

		return false;
	}
	else
	{
		printf("[ Connecting with DBAgent Success ]\n");

		return true;
	}
}

void ConnectorClass::OnConnect(SOCKET _socket)
{
	Session::OnConnect(_socket);

	Packet* requestMonsterDataPacket =
		reinterpret_cast<Packet*>(Session::GetSendBuffer()->
			GetBuffer(sizeof(Packet)));
	requestMonsterDataPacket->Init(SendCommand::Zone2DB_REQUEST_MONSTER_DATA, sizeof(Packet));

	Send(requestMonsterDataPacket);
}

void ConnectorClass::GetMonstersData(Packet* _packet)
{
	MonstersInfoPacket* monstersInfoPacket = static_cast<MonstersInfoPacket*>(_packet);

	for (int i = 0; i < monstersInfoPacket->count; i++)
	{
		monsterDataVec.push_back(monstersInfoPacket->monstersData[i]);
	}
}