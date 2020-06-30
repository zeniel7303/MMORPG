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

bool DBConnector::Init(const char* _ip, const unsigned short _portNum)
{
	TRYCATCH(m_ipEndPoint = IpEndPoint(_ip, _portNum));

	return true;
}

bool DBConnector::Connect()
{
	// 1. 소켓생성
	m_socket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_socket == INVALID_SOCKET)
	{
		MYDEBUG("[ Error - Invalid socket ]\n");
		LOG::FileLog("../LogFile.txt", __FILENAME__, __LINE__, "[ Error - Invalid socket ]");

		return false;
	}

	// 2. 연결요청
	if (connect(m_socket, (SOCKADDR*)&m_ipEndPoint, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		MYDEBUG("[ Error - Fail to connect ]\n");
		LOG::FileLog("../LogFile.txt", __FILENAME__, __LINE__, "[ Error - Fail to connect ]");
		// 4. 소켓종료
		closesocket(m_socket);
		// Winsock End
		WSACleanup();

		return false;
	}
	else
	{
		MYDEBUG("[ Connecting Success ]\n");

		return true;
	}
}

void DBConnector::OnConnect()
{
	UserSession::OnConnect();

	//왜 바로 못보내고 Sleep 걸어야 보내지?
	Sleep(1);
	//BOOL bVal = TRUE;
	//::setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, (char *)&bVal, sizeof(BOOL));

	Packet* requestMonsterDataPacket =
		reinterpret_cast<Packet*>(m_sendBuffer->
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

void DBConnector::DisConnect()
{
	UserSession::DisConnect();

	int errorNum = WSAGetLastError();
	if (errorNum != 0)
	{
		printf("%d Error \n", errorNum);
	}

	printf("===== [ close socket : %d ] ===== \n", m_socket);

	shutdown(m_socket, SD_BOTH);
	//shutdown 이후 close
	closesocket(m_socket);
}

void DBConnector::Reset()
{
	UserSession::Reset();
}

void DBConnector::HandleOverlappedIO(ST_OVERLAPPED* _overlapped)
{
	if (_overlapped == &m_overlapped)
	{
		if (m_overlapped.bytes <= 0)
		{
			//printf("[INFO] BYTES <= 0\n");

			//printf("2 \n");
			DisConnect();

			return;
		}

		m_receiver->Write(m_overlapped.bytes);

		Parsing();

		Recv();
	}
}

void DBConnector::Parsing()
{
	int tempNum = 20;

	while (1)
	{
		Packet* packet = reinterpret_cast<Packet*>(m_receiver->GetPacket());

		if (packet == nullptr) break;

		ServerLogicThread::getSingleton()->AddToDBConnectorPacketQueue(packet);

		tempNum--;

		if (tempNum <= 0)
		{
			tempNum = 20;

			break;
		}
	}
}