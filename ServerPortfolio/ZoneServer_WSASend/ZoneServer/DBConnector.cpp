#include "DBConnector.h"

DBConnector::DBConnector()
{
}

DBConnector::~DBConnector()
{
	DisConnect();
}

bool DBConnector::Connect(const char* _ip, const unsigned short _portNum, IOCPClass* _iocpClass)
{
	TRYCATCH(m_ipEndPoint = IpEndPoint(_ip, _portNum));

	// 1. 소켓생성
	m_socket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_socket == INVALID_SOCKET)
	{
		MYDEBUG("[ Error - Invalid socket ]\n");
		FILELOG("[ Error - Invalid socket ]");

		return false;
	}

	// 2. 연결요청
	if (connect(m_socket, (SOCKADDR*)&m_ipEndPoint, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		MYDEBUG("[ Error - Fail to connect ]\n");
		FILELOG("[ Error - Fail to connect ]");
		// 4. 소켓종료
		closesocket(m_socket);
		// Winsock End
		WSACleanup();

		return false;
	}
	else
	{
		MYDEBUG("[ DBAgent Connecting Success ]\n");

		m_IOCPClass = _iocpClass;
		m_IOCPClass->Associate(m_socket, (unsigned long long)this);

		return true;
	}
}

void DBConnector::OnConnect()
{
	ClientSession::OnConnect();

	m_start = std::chrono::high_resolution_clock::now();

	Sleep(1);

	Packet* requestMonsterDataPacket = new Packet();
	requestMonsterDataPacket->Init(SendCommand::Zone2DB_REQUEST_MONSTER_DATA, 
		sizeof(Packet));

	SharedPointer<char> tempPtr = 
		SharedPointer<char>(reinterpret_cast<char*>(requestMonsterDataPacket));
	AddToSendQueue(tempPtr);
}

void DBConnector::DisConnect()
{
	ClientSession::DisConnect();

	int errorNum = WSAGetLastError();
	if (errorNum != 0)
	{
		MYDEBUG("===== [ close socket : %ld, %d Error - DBConnector ] ===== \n", m_socket, errorNum);
	}
	else
	{
		MYDEBUG("===== [ close socket : %ld - DBConnector ] ===== \n", m_socket);
	}

	shutdown(m_socket, SD_BOTH);
	//shutdown 이후 close
	closesocket(m_socket);

	//Connect("211.221.147.29", 30003, m_IOCPClass);
	//OnConnect();
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

		MainThread::getSingleton()->AddToDBConnectorPacketQueue(packet);

		tempNum--;

		if (tempNum <= 0)
		{
			tempNum = 20;

			break;
		}
	}
}

void DBConnector::HeartBeat()
{
	auto m_end = std::chrono::high_resolution_clock::now();
	auto m_durationSec = std::chrono::duration_cast<std::chrono::seconds>(m_end - m_start);

	if (m_durationSec.count() > 30)
	{
		MYDEBUG("[ DB와 연결 끊김 ] \n");

		DisConnect();

		Connect("211.221.147.29", 30003, m_IOCPClass);
	}

	Packet* heartBeatPacket = new Packet();
	heartBeatPacket->Init(SendCommand::Zone2DB_HEARTBEAT, sizeof(Packet));

	MYDEBUG("[ DB HeartBeat ] \n");
	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(heartBeatPacket));
	AddToSendQueue(tempPtr);
}