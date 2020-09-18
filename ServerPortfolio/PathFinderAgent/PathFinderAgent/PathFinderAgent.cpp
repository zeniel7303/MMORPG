#include "PathFinderAgent.h"

#include "MainThread.h"

PathFinderAgent::PathFinderAgent()
{
}

PathFinderAgent::~PathFinderAgent()
{
	DisConnect();
}

void PathFinderAgent::OnConnect()
{
	ClientSession::OnConnect();

	MYDEBUG("%d socket \n", m_socket);
}

void PathFinderAgent::DisConnect()
{
	ClientSession::DisConnect();

	int errorNum = WSAGetLastError();
	if (errorNum != 0)
	{
		printf("%d Error \n", errorNum);
	}

	printf("===== [ close socket : %d ] ===== \n", m_socket);

	shutdown(m_socket, SD_BOTH);
	//shutdown 이후 close
	closesocket(m_socket); //순서상 다른 곳에서 해도된다.

	MainThread::getSingleton()->AddToDisConnectQueue(this);
}

void PathFinderAgent::Reset()
{
	ClientSession::Reset();
}

void PathFinderAgent::OnRecv()
{
	int tempNum = 20;

	while (1)
	{
		Packet* packet = reinterpret_cast<Packet*>(m_receiver->GetPacket());

		if (packet == nullptr) break;

		MainThread::getSingleton()->AddToRecvQueue({ this, packet });

		tempNum--;

		if (tempNum <= 0)
		{
			tempNum = 20;

			break;
		}
	}
}