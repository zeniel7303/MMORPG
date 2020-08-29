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

	ZoneNumPacket* zoneNumPacket =
		reinterpret_cast<ZoneNumPacket*>(m_sendBuffer->
			GetBuffer(sizeof(ZoneNumPacket)));
	zoneNumPacket->Init(SendCommand::LogIn2Zone_ZONENUM, sizeof(ZoneNumPacket));
	zoneNumPacket->zoneNum = m_num;
	Send(reinterpret_cast<char*>(zoneNumPacket), zoneNumPacket->size);
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

		MainThread::getSingleton()->AddToZoneServerPacketQueue({ m_num, packet });

		tempNum--;

		if (tempNum <= 0)
		{
			tempNum = 20;

			break;
		}
	}
}

void ZoneConnector::HeartBeat()
{
	MYDEBUG("[ %d ZoneServer HeartBeat ]\n", m_num);

	Packet* alivePacket =
		reinterpret_cast<Packet*>(m_sendBuffer->GetBuffer(sizeof(Packet)));
	alivePacket->Init(SendCommand::Login2Zone_ALIVE, sizeof(Packet));

	Send(reinterpret_cast<char*>(alivePacket), alivePacket->size);
}

void ZoneConnector::AuthenticationSuccess(AuthenticationPacket* _packet)
{
	LogInSuccessPacket* authenticationSuccessPacket = reinterpret_cast<LogInSuccessPacket*>(m_sendBuffer->
		GetBuffer(sizeof(LogInSuccessPacket)));
	authenticationSuccessPacket->Init(SendCommand::Login2Zone_AUTHENTICATION_SUCCESS, sizeof(LogInSuccessPacket));
	authenticationSuccessPacket->userIndex = _packet->userIndex;
	authenticationSuccessPacket->socket = _packet->socket;
	//m_sendBuffer->Write(LogInSuccess->size);
	MYDEBUG("[ authentication Success ] \n");

	Send(reinterpret_cast<char*>(authenticationSuccessPacket), authenticationSuccessPacket->size);
}

void ZoneConnector::AuthenticationFailed(AuthenticationPacket* _packet)
{
	AuthenticationFailedPacket* authenticationFailedPacket = reinterpret_cast<AuthenticationFailedPacket*>(m_sendBuffer->
		GetBuffer(sizeof(AuthenticationFailedPacket)));
	authenticationFailedPacket->Init(SendCommand::Login2Zone_AUTHENTICATION_FAILED, sizeof(AuthenticationFailedPacket));
	authenticationFailedPacket->userIndex = _packet->userIndex;
	authenticationFailedPacket->socket = _packet->socket;

	MYDEBUG("[ authentication Failed ] \n");

	Send(reinterpret_cast<char*>(authenticationFailedPacket), authenticationFailedPacket->size);
}