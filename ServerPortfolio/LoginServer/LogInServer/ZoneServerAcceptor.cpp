#include "ZoneServerAcceptor.h"

#include "MainThread.h"

ZoneServerAcceptor::ZoneServerAcceptor(const char* _ip, const unsigned short _port, HANDLE _handle, int _num)
	:AcceptorSession(_ip, _port, _handle, _num)
{
}

ZoneServerAcceptor::~ZoneServerAcceptor()
{
}

void ZoneServerAcceptor::OnAccept()
{
	MainThread::getSingleton()->ConnectWithZoneServer(m_socket);
}