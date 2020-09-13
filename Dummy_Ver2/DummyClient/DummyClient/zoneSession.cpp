#include "zoneSession.h"

#include "loginSession.h"

zoneSession::zoneSession(int _num, IOCPClass* _iocp)
{
	tempNum = _num;

	iocp = _iocp;

	isReady = false;
}

zoneSession::~zoneSession()
{
}

bool zoneSession::Connect()
{
	m_ipEndPoint = IpEndPoint("211.221.147.29", m_portNum + 1 + tempNum);
	MYDEBUG("%d \n", m_ipEndPoint.port);

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
		MYDEBUG("[ %d Connecting Success ]\n", m_userIdx);

		iocp->Associate(m_socket, (unsigned long long)this);

		OnConnect();

		return true;
	}
}

void zoneSession::OnConnect()
{
	ClientSession::OnConnect();

	LogInSuccessPacket* loginSuccessPacket =
		reinterpret_cast<LogInSuccessPacket*>(m_sendBuffer->
			GetBuffer(sizeof(LogInSuccessPacket)));
	loginSuccessPacket->Init(SendCommand::C2Zone_REQUIRE_INFO, sizeof(LogInSuccessPacket));
	loginSuccessPacket->userIndex = m_userIdx;

	m_sendBuffer->Write(loginSuccessPacket->size);
	Send(reinterpret_cast<char*>(loginSuccessPacket), loginSuccessPacket->size);

	m_loginSession->SendZoneNumPacket();
}

void zoneSession::DisConnect()
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

void zoneSession::Reset()
{
	ClientSession::Reset();
}

void zoneSession::OnRecv()
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

void zoneSession::PacketHandle(Packet* _packet)
{
	switch (_packet->cmd)
	{
	case (int)RecvCommand::Zone2C_REGISTER_USER:
	{
		SessionInfoPacket* sessionInfoPacket = 
			reinterpret_cast<SessionInfoPacket*>(_packet);

		m_basicInfo.unitInfo = sessionInfoPacket->info.unitInfo;
		m_basicInfo.userInfo = sessionInfoPacket->info.userInfo;
		m_basicInfo.unitInfo.state = STATE::IDLE;

		FieldNumPacket* fieldNumPacket =
			reinterpret_cast<FieldNumPacket*>(m_sendBuffer->
				GetBuffer(sizeof(FieldNumPacket)));
		fieldNumPacket->Init(SendCommand::C2Zone_TRY_ENTER_FIELD, sizeof(FieldNumPacket));
		fieldNumPacket->fieldNum = 3;
		m_sendBuffer->Write(fieldNumPacket->size);
		Send(reinterpret_cast<char*>(fieldNumPacket), fieldNumPacket->size);
	}
		break;
	case (int)RecvCommand::Zone2C_ENTER_FIELD:
	{
		EnterFieldPacket* enterFieldPacket =
			reinterpret_cast<EnterFieldPacket*>(_packet);

		//m_zoneNum = enterFieldPacket->fieldNum;
		m_basicInfo.unitInfo.position.x = enterFieldPacket->position.x;
		m_basicInfo.unitInfo.position.y = enterFieldPacket->position.y;

		Packet* packet =
			reinterpret_cast<Packet*>(m_sendBuffer->
				GetBuffer(sizeof(Packet)));
		packet->Init(SendCommand::C2Zone_ENTER_FIELD_SUCCESS, sizeof(Packet));
		m_sendBuffer->Write(packet->size);
		Send(reinterpret_cast<char*>(packet), packet->size);

		isReady = true;
	}
		break;
	}

	m_receiver->GetRingBuffer()->Read(_packet->size);
}