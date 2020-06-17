#pragma once
#include <winsock2.h>
#include <MSWSock.h>

#pragma comment(lib, "mswsock.lib")

class Acceptor
{
private:
	SOCKET					m_listenSocket;

	char*					m_byteBuffer;

public:
	Acceptor();
	~Acceptor();

	void Init(SOCKET _listenSocket);
	bool StartAccept(SOCKET _socket, DWORD _bytes, WSAOVERLAPPED _overlapped);
	void GetAccept(SOCKET _socket);
};

