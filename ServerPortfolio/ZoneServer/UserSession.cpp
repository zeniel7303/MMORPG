#include "UserSession.h"

UserSession::UserSession()
{
	m_failed = false;

	m_overlapped.state = IO_STATE::IO_RECV;

	TRYCATCH_CONSTRUCTOR(m_receiver = new Receiver(), m_failed);
	if (m_receiver->IsFailed() || m_failed)
	{
		m_failed = true; 
		return;
	}
	TRYCATCH_CONSTRUCTOR(m_sendBuffer = new SendBuffer(10000), m_failed);
	if (m_failed) return;

	m_isTestClient = false;

	m_isConnected = false;
}

UserSession::~UserSession()
{
	if(m_receiver != nullptr) delete m_receiver;
	if(m_sendBuffer != nullptr) delete m_sendBuffer;
}

void UserSession::OnConnect()
{
	m_isConnected = true;

	Recv();
}

void UserSession::DisConnect()
{
	m_isConnected = false;
}

void UserSession::Reset()
{
	m_overlapped.Reset(nullptr);
	m_overlapped.state = IO_STATE::IO_RECV;

	m_receiver->Reset();
	m_sendBuffer->Reset();

	m_isTestClient = false;

	m_isConnected = false;
}

void UserSession::Recv()
{
	m_receiver->ASyncRecv(m_socket, m_overlapped.wsaBuffer, m_overlapped);
}

void UserSession::Send(char* _data, DWORD _bytes)
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