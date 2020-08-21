#include "DBConnector.h"

DBConnector::DBConnector()
{
}

DBConnector::~DBConnector()
{
	m_monsterDataVec.clear();
	m_monsterDataVec.resize(0);

	DisConnect();
}

bool DBConnector::Connect(const char* _ip, const unsigned short _portNum)
{
	TRYCATCH(m_ipEndPoint = IpEndPoint(_ip, _portNum));

	// 1. 家南积己
	m_socket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_socket == INVALID_SOCKET)
	{
		MYDEBUG("[ Error - Invalid socket ]\n");
		FILELOG("[ Error - Invalid socket ]");

		return false;
	}

	// 2. 楷搬夸没
	if (connect(m_socket, (SOCKADDR*)&m_ipEndPoint, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		MYDEBUG("[ Error - Fail to connect ]\n");
		FILELOG("[ Error - Fail to connect ]");
		// 4. 家南辆丰
		closesocket(m_socket);
		// Winsock End
		WSACleanup();

		return false;
	}
	else
	{
		MYDEBUG("[ DBAgent Connecting Success ]\n");

		return true;
	}
}

void DBConnector::OnConnect()
{
	ClientSession::OnConnect();

	BOOL bVal = TRUE;
	::setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (char *)&bVal, sizeof(BOOL));

	Sleep(1);

	Packet* requestMonsterDataPacket =
		reinterpret_cast<Packet*>(m_sendBuffer->
			GetBuffer(sizeof(Packet)));
	requestMonsterDataPacket->Init(SendCommand::Zone2DB_REQUEST_MONSTER_DATA, sizeof(Packet));

	Send(reinterpret_cast<char*>(requestMonsterDataPacket), requestMonsterDataPacket->size);
}

void DBConnector::GetMonstersData(Packet* _packet)
{
	MonstersInfoPacket* monstersInfoPacket = reinterpret_cast<MonstersInfoPacket*>(_packet);

	for (int i = 0; i < monstersInfoPacket->count; i++)
	{
		m_monsterDataVec.push_back(monstersInfoPacket->monstersData[i]);
	}
}

void DBConnector::DisConnect()
{
	ClientSession::DisConnect();

	int errorNum = WSAGetLastError();
	if (errorNum != 0)
	{
		printf("%d Error \n", errorNum);
	}

	//MYDEBUG("===== [ close socket : %d ] ===== \n", m_socket);

	shutdown(m_socket, SD_BOTH);
	//shutdown 捞饶 close
	closesocket(m_socket);
}

void DBConnector::Reset()
{
	ClientSession::Reset();
}

void DBConnector::OnRecv()
{
	int tempNum = 20;

	while (1)
	{
		Packet* packet = reinterpret_cast<Packet*>(m_receiver->GetPacket());

		if (packet == nullptr) break;

		if (tempNum <= 18)
		{
			printf("check \n");
		}

		MainThread::getSingleton()->AddToDBConnectorPacketQueue(packet);

		tempNum--;

		if (tempNum <= 0)
		{
			tempNum = 20;

			break;
		}
	}
}