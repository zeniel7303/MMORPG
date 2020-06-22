#pragma once
#include <winsock2.h>

#include "../ServerLibrary/HeaderFiles/RingBuffer.h"

class Receiver
{
private:
	RingBuffer*				m_recvBuffer;

	DWORD					m_recvBytes;
	DWORD					m_flag;

public:
	Receiver();
	~Receiver();

	void Init();
	void Reset();
	void Write(int _size);
	void ASyncRecv(SOCKET _socket, WSABUF& _wsabuf, WSAOVERLAPPED& _overlapped);
	char* GetPacket();
};

