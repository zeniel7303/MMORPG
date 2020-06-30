#include "AcceptorSession.h"

#include "ServerLogicThread.h"

AcceptorSession::AcceptorSession(SOCKET _listenSocket, IpEndPoint* _ipEndPoint, HANDLE _handle, int _num)
{
	m_tempNum = _num;

	m_overlapped.state = IO_STATE::IO_ACCEPT;

	m_hIOCP = _handle;
	m_ipEndPoint = *_ipEndPoint;
	m_listenSocket = _listenSocket;

	m_locallen = m_remotelen = 0;
	m_locallen = m_remotelen = sizeof(sockaddr_in);

	TRYCATCH_CONSTRUCTOR(m_byteBuffer = new char[256], m_failed);
	if (m_failed) return;

	if (!ActuateAcceptEx())
	{
		MYDEBUG("[ %d AcceptEx Error ]\n", m_tempNum + 1);

		m_failed = true;

		return;
	}
	
	MYDEBUG("[ %d Acceptor Start ] \n", m_tempNum + 1);
}

AcceptorSession::~AcceptorSession()
{
	SAFE_DELETE_ARRAY(m_byteBuffer);
}

void AcceptorSession::Reset()
{
	m_listenSocket = 0;
}

void AcceptorSession::HandleOverlappedIO(ST_OVERLAPPED* _overlapped)
{
	switch (_overlapped->state)
	{
	case IO_ACCEPT:
	{
		//MYDEBUG("[ %d Acceptor - Accept ]\n", m_tempNum + 1);

		GetAcceptExSockaddrs(m_byteBuffer,
			0, (sizeof(sockaddr_in) + 16), (sizeof(sockaddr_in) + 16),
			(sockaddr **)&m_plocal, &m_locallen,
			(sockaddr **)&m_premote, &m_remotelen);

		setsockopt(m_socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
			(char *)&m_listenSocket, sizeof(m_listenSocket));

		CreateIoCompletionPort(reinterpret_cast <HANDLE>(m_socket),
			m_hIOCP, (unsigned long long)m_socket, 0);

		//연결 클라의 IP도 보내주자. -> m_premote
		ServerLogicThread::getSingleton()->AddToConnectQueue(m_socket);

		if (!ActuateAcceptEx())
		{
			MYDEBUG("[ AcceptEx Error ]\n");
		}
	}
		break;
	default:
		//예외
		break;
	}
}

void AcceptorSession::DisConnect()
{

}

bool AcceptorSession::ActuateAcceptEx()
{
	m_socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (AcceptEx(m_listenSocket, m_socket,
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