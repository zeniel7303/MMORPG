#include "Session.h"

Session::Session()
{

}

Session::~Session()
{

}

void Session::Init()
{
	m_recvOverlapped.session = this;
	m_sendOverlapped.session = this;

	m_recvBuffer = new RingBuffer();
	m_recvBuffer->Init(65535, 30000);

	m_sendBuffer = new SendBuffer();
	m_sendBuffer->Init(30000);

	m_recvBytes = 0;
	m_flags = 0;
	m_sendBytes = 0;

	m_isConnected = false;
	m_isCheckingHeartBeat = false;
	m_heartBeatCheckedCount = 0;

	userIdx = 0;
}

void Session::OnConnect(SOCKET _socket)
{
	m_socket = _socket;
	m_isConnected = true;

	Recv();
}

void Session::Disconnect()
{
	int errorNum = WSAGetLastError();
	if (errorNum != 0)
	{
		printf("%d Error \n", errorNum);
	}

	m_isConnected = false;
	m_isCheckingHeartBeat = false;

	printf("===== [ close socket : %d ] ===== \n", m_socket);

	shutdown(m_socket, SD_BOTH);
}

void Session::Reset()
{
	m_socket = 0;
	m_recvBytes = 0;
	m_flags = 0;

	m_isConnected = false;

	m_heartBeatCheckedCount = 0;

	m_recvBuffer->Reset();
	m_sendBuffer->Reset();
}

//WSASend
//보내고 무시해라? - 이러면 테스트할떈 동기와 비슷하다.
//HOW?????????????????????

//RECV SEND 체크용
void Session::CheckCompletion(bool _bool, ST_OVERLAPPED* _overlapped, DWORD _bytes)
{
	////비정상적인 종료 or TimeOut
	if (_bool == false)
	{
		printf("[INFO] GetQueuedCompletionStatus Failure\n");

		//printf("1 \n");
		Disconnect();

		return;
	}

	if (_overlapped == &m_recvOverlapped)
	{
		if (_bytes <= 0)
		{
			//printf("[INFO] BYTES <= 0\n");

			//printf("2 \n");
			Disconnect();

			return;
		}

		//printf("Check \n");
		m_recvBuffer->Write(_bytes);

		Parsing();

		Recv();
	}
	else if (_overlapped == &m_sendOverlapped)
	{
		if (_bytes <= 0)
		{
			//printf("[INFO] BYTES <= 0\n");

			//printf("3 \n");
			Disconnect();

			return;
		}

		if (m_sendDataBuffer.len > _bytes)
		{
			printf("재전송 \n");
			ReSend();
		}
	}
}

void Session::Recv()
{
	m_recvDataBuffer.buf = m_recvBuffer->GetWritePoint();
	m_recvDataBuffer.len = m_recvBuffer->GetWriteableSize();

	if (WSARecv(
		m_socket,
		&m_recvDataBuffer,
		1,
		&m_recvBytes,
		&m_flags,
		&m_recvOverlapped,
		NULL)
		== SOCKET_ERROR)
	{
		int num = WSAGetLastError();
		if (num != WSA_IO_PENDING)
		{
			printf("SOCKET %d : RECV FAILURE\n", m_socket);

			//printf("4 \n");
			Disconnect();
		}
	}
}

void Session::Send(Packet* _packet)
{
	m_sendDataBuffer.buf = m_sendBuffer->GetWritePoint();
	m_sendDataBuffer.len = _packet->size;

	if (WSASend(
		m_socket,
		&m_sendDataBuffer,
		1,
		&m_sendBytes,
		0,
		&m_sendOverlapped,
		NULL)
		== SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING)
	{
		int num = WSAGetLastError();
		if (num != WSA_IO_PENDING)
		{
			printf("SOCKET %d : SEND IO PENDING FAILURE %d\n", m_socket, num);

			//printf("5 \n");
			Disconnect();
		}
	}
}

void Session::Send(char* _data, DWORD _bytes)
{
	m_sendDataBuffer.buf = _data;
	m_sendDataBuffer.len = _bytes;

	if (WSASend(
		m_socket,
		&m_sendDataBuffer,
		1,
		&m_sendBytes,
		0,
		&m_sendOverlapped,
		NULL)
		== SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING)
	{
		int num = WSAGetLastError();
		if (num != WSA_IO_PENDING)
		{
			printf("SOCKET %d : SEND IO PENDING FAILURE %d\n", m_socket, num);

			//printf("6 \n");
			Disconnect();
		}
	}
}

void Session::ReSend()
{
	if (WSASend(
		m_socket,
		&m_sendDataBuffer,
		1,
		&m_sendBytes,
		0,
		&m_sendOverlapped,
		NULL)
		== SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		printf("RESEND IO PENDING FAILURE\n");
	}
}

void Session::Parsing()
{
	int tempNum = 0;

	while (1)
	{
		Packet* packet = reinterpret_cast<Packet*>(GetRecvBuffer()->CanParsing());

		if (packet == nullptr) break;

		switch (static_cast<RecvCommand>(packet->cmd))
		{
		/*case RecvCommand::LogIn2C_ISCONNECTED:
		{
			IsConnectedPacket* isConnectedPacket = static_cast<IsConnectedPacket*>(packet);
		
			LogInPacket* loginPacket =
				reinterpret_cast<LogInPacket*>(m_sendBuffer->
					GetBuffer(sizeof(LogInPacket)));
			loginPacket->Init(SendCommand::C2Zone_LOGIN, sizeof(LogInPacket));
			strcpy(loginPacket->id, id);
			strcpy(loginPacket->password, password);
			loginPacket->zoneNum = 0;

			Send(reinterpret_cast<char*>(loginPacket), loginPacket->size);
		}
			break;
		case RecvCommand::Zone2C_LOGIN_SUCCESS:
		{
			LogInSuccessPacket_PortNum* logInSuccessPacket = static_cast<LogInSuccessPacket_PortNum*>(packet);
		
			logInSuccessPacket->userIndex = userIdx;
		}
		break;*/
		case RecvCommand::Zone2C_CHECK_ALIVE:
			Send(reinterpret_cast<char*>(packet), packet->size);
			break;
		case RecvCommand::Zone2C_REGISTER_TEST_USER:
		{
			SessionInfoPacket* sessionInfoPacket = static_cast<SessionInfoPacket*>(packet);

			m_basicInfo.unitInfo = sessionInfoPacket->info.unitInfo;
			m_basicInfo.userInfo = sessionInfoPacket->info.userInfo;
			//printf("REGISTER %d \n", m_basicInfo.userInfo.userID);

			m_basicInfo.unitInfo.state = STATE::IDLE;
		}
		break;
		case RecvCommand::Zone2C_UPDATE_STATE_TEST_USER:
		{
			TestClientStatePacket* testClientStatePacket = static_cast<TestClientStatePacket*>(packet);

			m_basicInfo.unitInfo.state = testClientStatePacket->state;

			m_basicInfo.unitInfo.position.x = testClientStatePacket->vec2.x;
			m_basicInfo.unitInfo.position.y = testClientStatePacket->vec2.y;
		}
		break;
		case RecvCommand::Zone2C_TEST:
		{
			TestPacket* testPacket = static_cast<TestPacket*>(packet);

			//InterlockedExchange((LPLONG)&count, testPacket->count);
			//printf("%d : %d \n", m_basicInfo.userInfo.userID, count);
		}
		break;
		}

		tempNum++;

		if (tempNum > 20)
		{
			tempNum = 0;

			break;
		}
	}
	
}