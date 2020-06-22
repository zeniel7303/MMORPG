#include "Receiver.h"

Receiver::Receiver()
{
}

Receiver::~Receiver()
{
	delete m_recvBuffer;
}

void Receiver::Init()
{
	m_recvBuffer = new RingBuffer();
	m_recvBuffer->Init(65535, 30000);

	m_recvBytes = 0;
	m_flag = 0;
}

void Receiver::Reset()
{
	m_recvBuffer->Reset();

	m_recvBytes = 0;
	m_flag = 0;
}

void Receiver::Write(int _size)
{
	m_recvBuffer->Write(_size);
}

void Receiver::ASyncRecv(SOCKET _socket, WSABUF& _wsabuf, WSAOVERLAPPED& _overlapped)
{
	_wsabuf.buf = m_recvBuffer->GetWritePoint();
	_wsabuf.len = m_recvBuffer->GetWriteableSize();

	if (WSARecv(
		_socket,
		&_wsabuf,
		1,
		&m_recvBytes,
		&m_flag,
		&_overlapped,
		NULL)
		== SOCKET_ERROR)
	{
		int num = WSAGetLastError();
		if (num != WSA_IO_PENDING)
		{
			printf("[ SOCKET %d : Recv Fail ] - Error NUM : %d \n", _socket, num);
		}
	}
}

char* Receiver::GetPacket()
{
	return m_recvBuffer->CanParsing();
}