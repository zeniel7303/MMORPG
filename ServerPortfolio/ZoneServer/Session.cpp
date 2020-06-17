#include "Session.h"

Session::Session()
{

}

Session::~Session()
{

}

void Session::Init(SOCKET _listenSocket, HANDLE _handle)
{
	m_recvBytes = 0;
	m_flags = 0;
	m_sendBytes = 0;

	m_overlapped.hEvent = 0;
	m_overlapped.Internal = 0;
	m_overlapped.InternalHigh = 0;
	m_overlapped.Offset = 0;
	m_overlapped.OffsetHigh = 0;
	m_overlapped.Pointer = 0;

	m_overlapped.session = this;

	m_recvBuffer = new RingBuffer();
	m_recvBuffer->Init(65535, 30000);

	m_sendBuffer = new SendBuffer();
	m_sendBuffer->Init(30000);

	if (_listenSocket != 0)
	{
		m_acceptor = new Acceptor();
		m_acceptor->Init(_listenSocket);
	}

	m_hEvent = _handle;

	m_index = 0;

	m_isConnected = false;

	m_isCheckingHeartBeat = false;
	m_startCheckingHeartBeat = false;
	m_heartBeatCheckedCount = 0;
}

void Session::OnConnect()
{
	m_overlapped.state = IO_STATE::IO_RECV;

	m_isConnected = true;

	Recv();
}

void Session::Disconnect()
{
	m_overlapped.state = IO_STATE::IO_ACCEPT;

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

	m_isCheckingHeartBeat = false;
	m_startCheckingHeartBeat = false;
	m_heartBeatCheckedCount = 0;
}

void Session::Reset()
{
	m_overlapped.hEvent = 0;
	m_overlapped.Internal = 0;
	m_overlapped.InternalHigh = 0;
	m_overlapped.Offset = 0;
	m_overlapped.OffsetHigh = 0;
	m_overlapped.Pointer = 0;

	m_recvBuffer->Reset();
	m_sendBuffer->Reset();

	m_socket = 0;
	m_recvBytes = 0;
	m_flags = 0;

	m_index = 0;
}

void Session::StartAccept()
{
	m_overlapped.state = IO_STATE::IO_ACCEPT;

	m_socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (!m_acceptor->StartAccept(m_socket, m_overlapped.bytes, m_overlapped))
	{
		//실패시 예외처리
	}
}

void Session::AssociateIOCP(HANDLE _handle)
{
	CreateIoCompletionPort(reinterpret_cast <HANDLE>(m_socket),
		_handle, (unsigned long long)m_socket, 0);
}

void Session::Recv()
{
	m_overlapped.wsaBuffer.buf = m_recvBuffer->GetWritePoint();
	m_overlapped.wsaBuffer.len = m_recvBuffer->GetWriteableSize();

	if (WSARecv(
		m_socket,
		&m_overlapped.wsaBuffer,
		1,
		&m_recvBytes,
		&m_flags,
		&m_overlapped,
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

void Session::Send(char* _data, DWORD _bytes)
{
	//m_sendDataBuffer.buf = _data;
	//m_sendDataBuffer.len = _bytes;

	//if (WSASend(
	//	m_socket,
	//	&m_sendDataBuffer,
	//	1,
	//	&m_sendBytes,
	//	0,
	//	&m_sendOverlapped,
	//	NULL)
	//	== SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING)
	//{
	//	int num = WSAGetLastError();
	//	if (num != WSA_IO_PENDING)
	//	{
	//		printf("SOCKET %d : SEND IO PENDING FAILURE %d\n", m_socket, num);

	//		//printf("6 \n");
	//		Disconnect();
	//	}
	//}

	/*Packet* tempPacket = reinterpret_cast<Packet*>(_data);
	if (tempPacket->size >= 10000)
	{
		printf("cmd : %d \n", tempPacket->cmd);
	}*/

	send(m_socket, _data, _bytes, 0);

	/*if (*(u_short*)(_data + 2) == 10010)
	{
		printf("send command  :  %d \n", *(u_short*)(_data + 2));
	}*/
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