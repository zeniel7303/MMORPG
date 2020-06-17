#include "Acceptor.h"

Acceptor::Acceptor()
{
	
}

Acceptor::~Acceptor()
{
}

void Acceptor::Init(SOCKET _listenSocket)
{
	m_listenSocket = _listenSocket;

	m_byteBuffer = new char[256];
}

bool Acceptor::StartAccept(SOCKET _socket, DWORD _bytes, WSAOVERLAPPED _overlapped)
{
	if (AcceptEx(m_listenSocket, _socket,
		m_byteBuffer, 0, (sizeof(sockaddr_in) + 16), (sizeof(sockaddr_in) + 16),
		&_bytes, &_overlapped) == FALSE)
	{
		int num = WSAGetLastError();
		if (num != WSA_IO_PENDING)
		{
			return false;
		}
	}

	return true;
}

void Acceptor::GetAccept(SOCKET _socket)
{
	int locallen, remotelen;
	sockaddr_in *plocal = 0, *premote = 0;

	locallen = remotelen = sizeof(sockaddr_in);
	GetAcceptExSockaddrs(m_byteBuffer,
		0, (sizeof(sockaddr_in) + 16), (sizeof(sockaddr_in) + 16),
		(sockaddr **)&plocal, &locallen,
		(sockaddr **)&premote, &remotelen);

	setsockopt(_socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
		(char *)&m_listenSocket, sizeof(m_listenSocket));
}