#include "ZoneConnector.h"

#include "MainThread.h"

ZoneConnector::ZoneConnector()
{
}

ZoneConnector::~ZoneConnector()
{
}

void ZoneConnector::OnConnect()
{
	ClientSession::OnConnect();

	BOOL bVal = TRUE;
	::setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (char *)&bVal, sizeof(BOOL));

	Sleep(1);

	Packet* helloPacket =
		reinterpret_cast<Packet*>(m_sendBuffer->
			GetBuffer(sizeof(Packet)));
	helloPacket->Init(SendCommand::LogIn2Zone_HELLO, sizeof(Packet));

	Send(reinterpret_cast<char*>(helloPacket), helloPacket->size);
}

void ZoneConnector::DisConnect()
{
	ClientSession::DisConnect();

	int errorNum = WSAGetLastError();
	if (errorNum != 0)
	{
		printf("%d Error \n", errorNum);
	}

	//MYDEBUG("===== [ close socket : %d ] ===== \n", m_socket);

	shutdown(m_socket, SD_BOTH);
	//shutdown ¿Ã»ƒ close
	closesocket(m_socket);
}

void ZoneConnector::Reset()
{
	ClientSession::Reset();
}

void ZoneConnector::OnRecv()
{
	int tempNum = 20;

	while (1)
	{
		Packet* packet = reinterpret_cast<Packet*>(m_receiver->GetPacket());

		if (packet == nullptr) break;

		MainThread::getSingleton()->AddToZoneServerPacketQueue(packet);

		tempNum--;

		if (tempNum <= 0)
		{
			tempNum = 20;

			break;
		}
	}
}