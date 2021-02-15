#include "../HeaderFiles/Sender.h"

Sender::Sender()
{
	m_sendBytes = 0;
	m_flag = 0;
	m_bytes = 0;

	m_isSend = false;

	m_wsaBuf.buf = nullptr;
}

Sender::~Sender()
{
	while (!m_queue.empty())
	{
		m_queue.pop();
	}
}

void Sender::Reset()
{
	m_sendBytes = 0;
	m_flag = 0;
	m_bytes = 0;

	while (!m_queue.empty())
	{
		m_queue.pop();
	}

	m_isSend = false;
}

bool Sender::ASyncSend(SOCKET _socket, WSAOVERLAPPED& _overlapped)
{
	if (m_queue.empty())
	{
		m_isSend = false;

		return true;
	}

	m_isSend = true;

	char* temp = m_queue.front().GetPointer();
	if (m_bytes <= 0)
	{
		m_bytes = *(WORD*)temp;
	}

	m_wsaBuf.buf = m_queue.front().GetPointer();
	m_wsaBuf.len = *(WORD*)m_wsaBuf.buf;

	if (WSASend(
		_socket,
		&m_wsaBuf,
		1,
		&m_sendBytes,
		m_flag,
		&_overlapped,
		NULL)
		== SOCKET_ERROR)
	{
		int num = WSAGetLastError();
		if (num != WSA_IO_PENDING)
		{
			//printf("[ SOCKET %d : Send Fail ] - Error NUM : %d \n", _socket, num);

			m_isSend = false;

			return false;
		}
	}

	return true;
}

void Sender::ReSend()
{

}