#include "ListenClass.h"

ListenClass::ListenClass()
{

}

ListenClass::~ListenClass()
{
	closesocket(m_listenSocket);
	WSACleanup();
}

bool ListenClass::Init(IOCPClass* _IOCPClass, SessionManager* _sessionManager, const char* _ip, const unsigned short _portNum)
{
	m_ipEndPoint = IpEndPoint(_ip, _portNum);

	//m_IOCPClass = _IOCPClass;
	m_sessionManager = _sessionManager;
	m_portNum = _portNum;

	/*m_listenSocket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_listenSocket == INVALID_SOCKET)
	{
		WSACleanup();
		printf("[ Failed socket() ]\n");
		return false;
	}*/

	m_listenSocket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	memset(&m_servAddr, 0, sizeof(m_servAddr));
	m_servAddr.sin_family = AF_INET;
	m_servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	m_servAddr.sin_port = htons(m_portNum);

	//bind 앞에 :: 붙인 이유
	//https://stackoverflow.com/questions/44861571/operator-error
	if (::bind(m_listenSocket, (SOCKADDR*)&m_ipEndPoint, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		printf("[ Binding Error ]\n");
		closesocket(m_listenSocket);
		WSACleanup();

		return false;
	}

	if (listen(m_listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		printf("[ Listening Error ]\n");
		closesocket(m_listenSocket);
		WSACleanup();

		return false;
	}

	//Thread<ListenClass>::Start(this);

	printf("[ ListenClass Start ] \n");

	return true;
}