#include "../HeaderFiles/Session.h"

Session::Session()
{
	m_failed = false;

	memset(&m_recvOverlapped, 0, sizeof(struct ST_OVERLAPPED));
	m_recvOverlapped.session = this;
	m_recvOverlapped.state = IO_STATE::IO_RECV;

	memset(&m_sendOverlapped, 0, sizeof(struct ST_OVERLAPPED));
	m_sendOverlapped.session = this;
	m_sendOverlapped.state = IO_STATE::IO_SEND;

	m_socket = 0;
}

Session::~Session()
{
}
