#include "Acceptor.h"

Acceptor::Acceptor()
{
	
}

Acceptor::~Acceptor()
{
	SAFE_DELETE_ARRAY(m_byteBuffer);
}

void Acceptor::Init(Session* _session)
{
	m_overlapped.Reset(_session);

	m_byteBuffer = new char[256];

	m_locallen = m_remotelen = sizeof(sockaddr_in);
	m_plocal = nullptr;
	m_premote = nullptr;
}

void Acceptor::SetListenSocket(SOCKET _listenSocket)
{
	m_listenSocket = _listenSocket;
}

void Acceptor::Reset()
{
	m_overlapped.Reset(nullptr);

	m_listenSocket = 0;

	SAFE_DELETE_ARRAY(m_byteBuffer);

	m_locallen = m_remotelen = sizeof(sockaddr_in);
	m_plocal = nullptr;
	m_premote = nullptr;
}

bool Acceptor::Start(SOCKET _socket)
{
	if (AcceptEx(m_listenSocket, _socket,
		m_byteBuffer, 0, (sizeof(sockaddr_in) + 16), (sizeof(sockaddr_in) + 16),
		&m_overlapped.bytes, &m_overlapped) == FALSE)
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
	GetAcceptExSockaddrs(m_byteBuffer,
		0, (sizeof(sockaddr_in) + 16), (sizeof(sockaddr_in) + 16),
		(sockaddr **)&m_plocal, &m_locallen,
		(sockaddr **)&m_premote, &m_remotelen);

	setsockopt(_socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
		(char *)&m_listenSocket, sizeof(m_listenSocket));
}