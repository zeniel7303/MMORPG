#include "Connector.h"

Connector::Connector()
{
}

Connector::~Connector()
{
}

void Connector::Init(const char* _ip, const unsigned short _portNum)
{
	Session::Init();

	m_ipEndPoint = IpEndPoint(_ip, _portNum);
}

bool Connector::Connect()
{
	// 1. 家南积己
	m_socket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_socket == INVALID_SOCKET)
	{
		printf("Error - Invalid socket\n");

		return false;
	}

	// 2. 楷搬夸没
	if (connect(m_socket, (SOCKADDR*)&m_ipEndPoint, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
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
		printf("[ Connecting Success ]\n");

		return true;
	}
}

void Connector::OnConnect(SOCKET _socket)
{
	Session::OnConnect(_socket);
}

void Connector::CheckCompletion(ST_OVERLAPPED* _overlapped, DWORD _bytes)
{
	if (_overlapped == &m_recvOverlapped)
	{
		if (_bytes <= 0)
		{
			//printf("[INFO] BYTES <= 0\n");

			//printf("2 \n");
			Disconnect();

			return;
		}

		m_recvBuffer->Write(_bytes);

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
			printf("犁傈价 \n");
			ReSend();
		}
	}
}