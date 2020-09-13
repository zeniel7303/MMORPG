#include "loginSession.h"

#include "zoneSession.h"

loginSession::loginSession(int _num, IOCPClass* _iocp)
{
	iocp = _iocp;

	char buffer[64];
	itoa(_num, buffer, 10);

	strcat(id, buffer);
	strcat(password, buffer);
}

loginSession::~loginSession()
{
}

bool loginSession::Connect()
{
	m_ipEndPoint = IpEndPoint("211.221.147.29", 30005);

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

		iocp->Associate(m_socket, (unsigned long long)this);

		return true;
	}
}

void loginSession::OnConnect()
{
	ClientSession::OnConnect();

	LogInPacket* logInPacket = 
		reinterpret_cast<LogInPacket*>(m_sendBuffer->
			GetBuffer(sizeof(LogInPacket)));
	logInPacket->Init(SendCommand::C2Zone_LOGIN, sizeof(LogInPacket));
	strcpy(logInPacket->id, id);
	strcpy(logInPacket->password, password);
	logInPacket->zoneNum = 0;

	m_sendBuffer->Write(logInPacket->size);

	Send(reinterpret_cast<char*>(logInPacket), logInPacket->size);
}

void loginSession::DisConnect()
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

void loginSession::Reset()
{
	ClientSession::Reset();
}

void loginSession::OnRecv()
{
	int tempNum = 20;

	while (1)
	{
		Packet* packet = reinterpret_cast<Packet*>(m_receiver->GetPacket());

		if (packet == nullptr) break;

		PacketHandle(packet);

		tempNum--;

		if (tempNum <= 0)
		{
			tempNum = 20;

			break;
		}
	}
}

void loginSession::PacketHandle(Packet* _packet)
{
	switch (_packet->cmd)
	{
	case (int)RecvCommand::Zone2C_LOGIN_SUCCESS:
	{
		LogInSuccessPacket_PortNum* logInSuccessPacket =
			reinterpret_cast<LogInSuccessPacket_PortNum*>(_packet);

		MYDEBUG("LoginSuccess \n");

		m_zoneSession->SetPortNum(logInSuccessPacket->portNum);
		m_zoneSession->SetUserIdx(logInSuccessPacket->userIndex);

		m_zoneSession->Connect();
	}
		break;
	}

	m_receiver->GetRingBuffer()->Read(_packet->size);
}

void loginSession::SendZoneNumPacket()
{
	ZoneNumPacket* zoneNumPacket =
		reinterpret_cast<ZoneNumPacket*>(m_sendBuffer->
			GetBuffer(sizeof(ZoneNumPacket)));
	zoneNumPacket->Init(SendCommand::C2Login_SENDZONENUM, sizeof(ZoneNumPacket));
	zoneNumPacket->zoneNum = 0;

	m_sendBuffer->Write(zoneNumPacket->size);
	Send(reinterpret_cast<char*>(zoneNumPacket), zoneNumPacket->size);
}

void loginSession::SendHeartBeat()
{
	Packet* CheckAlivePacket =
		reinterpret_cast<Packet*>(m_sendBuffer->
			GetBuffer(sizeof(Packet)));
	CheckAlivePacket->Init(SendCommand::C2Zone_CHECK_ALIVE, sizeof(Packet));

	m_sendBuffer->Write(CheckAlivePacket->size);
	Send(reinterpret_cast<char*>(CheckAlivePacket), CheckAlivePacket->size);
}