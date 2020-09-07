#include "PathFinderAgent.h"

PathFinderAgent::PathFinderAgent()
{
}

PathFinderAgent::~PathFinderAgent()
{
	DisConnect();
}

bool PathFinderAgent::Connect(const char* _ip, const unsigned short _portNum, IOCPClass* _iocpClass)
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
		MYDEBUG("[ PathFindAget Connecting Success ]\n");

		m_IOCPClass = _iocpClass;
		m_IOCPClass->Associate(m_socket, (unsigned long long)this);

		return true;
	}
}

void PathFinderAgent::OnConnect()
{
	ClientSession::OnConnect();

	//BOOL bVal = TRUE;
	//::setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (char *)&bVal, sizeof(BOOL));
}

void PathFinderAgent::DisConnect()
{
	ClientSession::DisConnect();

	int errorNum = WSAGetLastError();
	if (errorNum != 0)
	{
		MYDEBUG("[ %d Error ]\n", errorNum);
	}

	//MYDEBUG("===== [ close socket : %d ] ===== \n", m_socket);

	shutdown(m_socket, SD_BOTH);
	//shutdown 捞饶 close
	closesocket(m_socket);

	Connect("211.221.147.29", 30009, m_IOCPClass);
	OnConnect();
}

void PathFinderAgent::Reset()
{
	ClientSession::Reset();
}

void PathFinderAgent::OnRecv()
{
	/*int errorNum = WSAGetLastError();
	if (errorNum != 0)
	{
		MYDEBUG("[ 1. %d Error ]\n", errorNum);
	}*/

	MYDEBUG("writable size : %d \n", m_receiver->GetRingBuffer()->GetWriteableSize());

	int tempNum = 20;

	while (1)
	{
		Packet* packet = reinterpret_cast<Packet*>(m_receiver->GetPacket());

		if (packet == nullptr) break;

		MainThread::getSingleton()->AddToPathFindAgentPacketQueue(packet);

		tempNum--;

		if (tempNum <= 0)
		{
			tempNum = 20;

			break;
		}
	}
}

void PathFinderAgent::SendPathFindPacket(int _index, int _fieldNum, unsigned short _state,
	int _nowTileX, int _nowTileY, int _targetTileX, int _targetTileY)
{
	PathFindPacket* pathFindPacket =
		reinterpret_cast<PathFindPacket*>(m_sendBuffer->GetBuffer(sizeof(PathFindPacket)));
	pathFindPacket->Init(SendCommand::Zone2Path_PATHFIND, sizeof(PathFindPacket));

	pathFindPacket->monsterNum = _index;
	pathFindPacket->fieldNum = _fieldNum;
	pathFindPacket->state = _state;

	pathFindPacket->nowPosition.x = _nowTileX;
	pathFindPacket->nowPosition.y = _nowTileY;
	pathFindPacket->targetPosition.x = _targetTileX;
	pathFindPacket->targetPosition.y = _targetTileY;

	m_sendBuffer->Write(pathFindPacket->size);

	Send(reinterpret_cast<char*>(pathFindPacket), pathFindPacket->size);
}