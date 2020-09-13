#include "DBConnector.h"

DBConnector::~DBConnector()
{
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
		MYDEBUG("[ Connecting Success ]\n");

		return true;
	}
}

void DBConnector::OnConnect()
{
	ClientSession::OnConnect();

	Sleep(1);

	Packet* helloPacket =
		reinterpret_cast<Packet*>(m_sendBuffer->
			GetBuffer(sizeof(Packet)));
	helloPacket->Init(SendCommand::LogIn2DB_HELLO, sizeof(Packet));

	Send(reinterpret_cast<char*>(helloPacket), helloPacket->size);
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

		MainThread::getSingleton()->AddToDBConnectorPacketQueue(packet);

		tempNum--;

		if (tempNum <= 0)
		{
			tempNum = 20;

			break;
		}
	}
}