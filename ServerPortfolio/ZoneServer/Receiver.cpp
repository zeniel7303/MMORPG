#include "Receiver.h"

Receiver::Receiver()
{
	m_failed = false;

	m_recvBytes = 0;
	m_flag = 0;

	TRYCATCH_CONSTRUCTOR(m_recvBuffer = new RingBuffer(30000, 10000), m_failed);
}

Receiver::~Receiver()
{
	if(m_recvBuffer != nullptr) delete m_recvBuffer;
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

bool Receiver::ASyncRecv(SOCKET _socket, WSABUF& _wsabuf, WSAOVERLAPPED& _overlapped)
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

			return false;
		}
	}

	return true;
}

char* Receiver::GetPacket()
{
	return m_recvBuffer->CanParsing();
}