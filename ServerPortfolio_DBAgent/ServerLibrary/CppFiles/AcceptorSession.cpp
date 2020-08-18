#include "../HeaderFiles/AcceptorSession.h"

AcceptorSession::AcceptorSession(const char* _ip, const unsigned short _port, HANDLE _handle, int _num)
{
	m_failed = false;

	m_tempNum = _num;

	m_overlapped.state = IO_STATE::IO_ACCEPT;

	m_hIOCP = _handle;
	m_ipEndPoint = IpEndPoint(_ip, _port);
	m_listenSocket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_listenSocket == INVALID_SOCKET)
	{
		WSACleanup();
		MYDEBUG("[ Failed socket() ]\n");
		LOG::FileLog("../LogFile.txt", __FILENAME__, __LINE__, "[ Failed socket() ]");

		m_failed = true;
		return;
	}

	//앞에 :: 붙인 이유
	//https://stackoverflow.com/questions/44861571/operator-error
	if (bind(m_listenSocket, (SOCKADDR*)&m_ipEndPoint, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		MYDEBUG("[ Binding Error ]\n");
		LOG::FileLog("../LogFile.txt", __FILENAME__, __LINE__, "[ Binding Error ]");
		closesocket(m_listenSocket);
		WSACleanup();

		m_failed = true;
		return;
	}

	if (listen(m_listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		MYDEBUG("[ Listening Error ]\n");
		LOG::FileLog("../LogFile.txt", __FILENAME__, __LINE__, "[ Listening Error ]");
		closesocket(m_listenSocket);
		WSACleanup();

		m_failed = true;
		return;
	}

	m_locallen = m_remotelen = 0;
	m_locallen = m_remotelen = sizeof(sockaddr_in);

	TRYCATCH_CONSTRUCTOR(m_byteBuffer = new char[256], m_failed);
	if (m_failed) return;

	GenerateOverlappedIO();
	if (m_failed) return;

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
	GetAcceptExSockaddrs(m_byteBuffer,
		0, (sizeof(sockaddr_in) + 16), (sizeof(sockaddr_in) + 16),
		(sockaddr **)&m_plocal, &m_locallen,
		(sockaddr **)&m_premote, &m_remotelen);

	setsockopt(m_socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
		(char *)&m_listenSocket, sizeof(m_listenSocket));

	CreateIoCompletionPort(reinterpret_cast <HANDLE>(m_socket),
		m_hIOCP, (unsigned long long)m_socket, 0);

	//연결 클라의 IP도 보내주자. -> m_premote
	OnAccept();

	GenerateOverlappedIO();
}

void AcceptorSession::DisConnect()
{
	//연결해제
	int errorNum = WSAGetLastError();
	if (errorNum != 0)
	{
		printf("%d Error \n", errorNum);
	}

	printf("===== [ close listenSocket : %d ] ===== \n", m_listenSocket);

	shutdown(m_listenSocket, SD_BOTH);
	//shutdown 이후 close
	closesocket(m_listenSocket);

	printf("===== [ close socket : %d ] ===== \n", m_socket);

	shutdown(m_socket, SD_BOTH);
	//shutdown 이후 close
	closesocket(m_socket);
}

void AcceptorSession::GenerateOverlappedIO()
{
	m_socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (AcceptEx(m_listenSocket, m_socket,
		m_byteBuffer, 0, (sizeof(sockaddr_in) + 16), (sizeof(sockaddr_in) + 16),
		&bytes, &m_overlapped) == FALSE)
	{
		int num = WSAGetLastError();
		if (num != WSA_IO_PENDING)
		{
			printf("[ ACCEPTOR ] - Error NUM : %d \n", num);

			m_failed = true;
		}
	}
}