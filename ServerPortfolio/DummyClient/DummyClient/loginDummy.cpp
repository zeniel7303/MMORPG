#include "loginDummy.h"

loginDummy::loginDummy()
{
	Init();
}

loginDummy::~loginDummy()
{
}

bool loginDummy::Connect()
{
	IpEndPoint ipEndPoint;
	ipEndPoint = IpEndPoint("211.221.147.29", 30005);

	// 1. 家南积己
	m_socket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_socket == INVALID_SOCKET)
	{
		printf("Error - Invalid socket\n");

		return false;
	}

	// 2. 楷搬夸没
	if (connect(m_socket, (SOCKADDR*)&ipEndPoint, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		printf("Error - Fail to connect\n");
		// 4. 家南辆丰
		closesocket(m_socket);
		// Winsock End
		WSACleanup();

		return false;
	}
	else
	{
		printf("%d Dummy Client Server Connected\n", dummyNum);

		return true;
	}
}

void loginDummy::OnConnect(SOCKET _socket)
{
	Session::OnConnect(_socket);

	TestClientEnterPacket* testClientEnterPacket =
		reinterpret_cast<TestClientEnterPacket*>(GetSendBuffer()->
			GetBuffer(sizeof(TestClientEnterPacket)));
	testClientEnterPacket->Init(SendCommand::C2Zone_ENTER_TEST_USER, sizeof(TestClientEnterPacket));
	testClientEnterPacket->userNum = 60000 + dummyNum;
	testClientEnterPacket->fieldNum = 3;
	m_sendBuffer->Write(testClientEnterPacket->size);

	Send(reinterpret_cast<char*>(testClientEnterPacket), testClientEnterPacket->size);
}