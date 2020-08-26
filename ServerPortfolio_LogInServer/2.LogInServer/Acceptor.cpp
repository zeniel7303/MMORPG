#include "Acceptor.h"

#include "MainThread.h"

Acceptor::Acceptor(const char* _ip, const unsigned short _port, HANDLE _handle, int _num)
	:AcceptorSession(_ip, _port, _handle, _num)
{
	
}

Acceptor::~Acceptor()
{
}

void Acceptor::OnAccept()
{
	MainThread::getSingleton()->AddToConnectQueue(m_socket);
}