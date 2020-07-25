#pragma once
#include <winsock2.h>

#include "../ServerLibrary/HeaderFiles/RingBuffer.h"
#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/Utils.h"

class Receiver
{
private:
	bool					m_failed;

	RingBuffer*				m_recvBuffer;

	DWORD					m_recvBytes;
	DWORD					m_flag;

public:
	Receiver();
	~Receiver();

	void Reset();
	void Write(int _size);
	bool ASyncRecv(SOCKET _socket, WSABUF& _wsabuf, WSAOVERLAPPED& _overlapped);
	char* GetPacket();

	bool IsFailed() { return m_failed; }
};

