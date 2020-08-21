#include "Acceptor.h"

#include "MainThread.h"

Acceptor::Acceptor(const char* _ip, const unsigned short _port, HANDLE _handle, int _num)
	:AcceptorSession(_ip, _port, _handle, _num)
{
	m_isFirstAccept = true;
}

Acceptor::~Acceptor()
{
}

void Acceptor::OnAccept()
{
	if (m_isFirstAccept)
	{
		MainThread::getSingleton()->ConnectWithZoneServer(m_socket);

		m_isFirstAccept = false;

		return;
	}

	MainThread::getSingleton()->AddToConnectQueue(m_socket);
}