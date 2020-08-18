#include "../HeaderFiles/Session.h"

Session::Session()
{
	m_failed = false;

	memset(&m_overlapped, 0, sizeof(struct ST_OVERLAPPED));
	m_overlapped.session = this;
	m_overlapped.state = IO_STATE::IO_NONE;

	m_socket = 0;
}

Session::~Session()
{
}
