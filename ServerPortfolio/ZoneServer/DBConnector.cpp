#include "DBConnector.h"

DBConnector::DBConnector()
{
}

DBConnector::~DBConnector()
{
}

void DBConnector::Init(const char* _ip, const unsigned short _portNum, HANDLE _handle)
{
	Session::Init(0, _handle);

	m_ipEndPoint = IpEndPoint(_ip, _portNum);
}

bool DBConnector::Connect()
{
	// 1. 소켓생성
	m_socket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_socket == INVALID_SOCKET)
	{
		printf("Error - Invalid socket\n");

		return false;
	}

	// 2. 연결요청
	if (connect(m_socket, (SOCKADDR*)&m_ipEndPoint, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		printf("Error - Fail to connect\n");
		// 4. 소켓종료
		closesocket(m_socket);
		// Winsock End
		WSACleanup();

		return false;
	}
	else
	{
		printf("[ Connecting Success ]\n");

		return true;
	}
}

void DBConnector::OnConnect()
{
	Session::OnConnect();

	//왜 바로 못보내고 Sleep 걸어야 보내지?
	Sleep(1);
	//BOOL bVal = TRUE;
	//::setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, (char *)&bVal, sizeof(BOOL));

	Packet* requestMonsterDataPacket =
		reinterpret_cast<Packet*>(Session::GetSendBuffer()->
			GetBuffer(sizeof(Packet)));
	requestMonsterDataPacket->Init(SendCommand::Zone2DB_REQUEST_MONSTER_DATA, sizeof(Packet));

	Send(reinterpret_cast<char*>(requestMonsterDataPacket), requestMonsterDataPacket->size);
}

void DBConnector::GetMonstersData(Packet* _packet)
{
	MonstersInfoPacket* monstersInfoPacket = static_cast<MonstersInfoPacket*>(_packet);

	for (int i = 0; i < monstersInfoPacket->count; i++)
	{
		m_monsterDataVec.push_back(monstersInfoPacket->monstersData[i]);
	}
}

void DBConnector::Disconnect()
{
	Session::Disconnect();
}

void DBConnector::Reset()
{
	Session::Reset();
}

void DBConnector::CheckCompletion(ST_OVERLAPPED* _overlapped)
{
	if (_overlapped == &m_overlapped)
	{
		if (m_overlapped.bytes <= 0)
		{
			//printf("[INFO] BYTES <= 0\n");

			//printf("2 \n");
			Disconnect();

			return;
		}

		m_recvBuffer->Write(m_overlapped.bytes);

		Parsing();

		Recv();
	}
}

void DBConnector::Parsing()
{
	int tempNum = 20;

	while (1)
	{
		Packet* packet = reinterpret_cast<Packet*>(GetRecvBuffer()->CanParsing());

		if (packet == nullptr) break;

		m_doubleQueue.AddObject(packet);
		SetEvent(m_hEvent);

		tempNum--;

		if (tempNum <= 0)
		{
			tempNum = 20;

			break;
		}
	}
}