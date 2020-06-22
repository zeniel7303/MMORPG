#include "Session.h"

Session::Session()
{
	m_acceptor = new Acceptor();
	m_acceptor->Init(this);
	m_receiver = new Receiver();
	m_receiver->Init();
	m_sendBuffer = new SendBuffer();
	m_sendBuffer->Init(30000);
}

Session::~Session()
{
	delete m_acceptor;
	delete m_receiver;
	delete m_sendBuffer;
}

void Session::Init(SOCKET _listenSocket)
{
	m_acceptor->SetListenSocket(_listenSocket);

	m_index = 0;

	m_isConnected = false;
}

void Session::OnConnect()
{
	m_acceptor->SetOverlappedState(Acceptor::IO_STATE::IO_RECV);

	m_isConnected = true;

	Recv();
}

void Session::Disconnect()
{
	m_acceptor->SetOverlappedState(Acceptor::IO_STATE::IO_ACCEPT);

	int errorNum = WSAGetLastError();
	if (errorNum != 0)
	{
		printf("%d Error \n", errorNum);
	}

	printf("===== [ close socket : %d ] ===== \n", m_socket);

	shutdown(m_socket, SD_BOTH);
	//shutdown 이후 close
	closesocket(m_socket);

	m_isConnected = false;
}

void Session::Reset()
{
	m_acceptor->Reset();
	m_receiver->Reset();
	m_sendBuffer->Reset();

	m_socket = 0;

	m_index = 0;
}

void Session::StartAccept()
{
	m_acceptor->SetOverlappedState(Acceptor::IO_STATE::IO_ACCEPT);

	m_socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (!m_acceptor->Start(m_socket))
	{
		//실패시 예외처리
		printf("Accept Fail \n");
	}
}

void Session::AssociateIOCP(HANDLE _handle)
{
	m_acceptor->GetAccept(m_socket);

	CreateIoCompletionPort(reinterpret_cast <HANDLE>(m_socket),
		_handle, (unsigned long long)m_socket, 0);
}

void Session::Recv()
{
	m_receiver->ASyncRecv(m_socket, MYOVERLAPPED->wsaBuffer, *MYOVERLAPPED);
}

void Session::Send(char* _data, DWORD _bytes)
{
	/*m_sendDataBuffer.buf = _data;
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
	}*/

	send(m_socket, _data, _bytes, 0);
}

void Session::ReSend()
{
	/*if (WSASend(
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
	}*/
}