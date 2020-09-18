#pragma once
#pragma warning(disable:4005)
#pragma warning(disable:4477)

#include <winsock2.h>

#include "../HeaderFiles/RingBuffer.h"
#include "../HeaderFiles/FileLog.h"
#include "../HeaderFiles/Utils.h"

class Receiver
{
private:
	bool					m_failed;

	RingBuffer*				m_recvBuffer;

	WSABUF					m_wsaBuf;
	DWORD					m_recvBytes;
	DWORD					m_flag;

public:
	Receiver();
	~Receiver();

	void Reset();
	void Write(int _size);
	bool ASyncRecv(SOCKET _socket, WSAOVERLAPPED& _overlapped);
	char* GetPacket();

	bool IsFailed() { return m_failed; }
	RingBuffer* GetRingBuffer() { return m_recvBuffer; }
};

