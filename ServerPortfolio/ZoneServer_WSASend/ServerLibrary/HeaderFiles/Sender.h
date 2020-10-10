#pragma once
#pragma warning(disable:4005)

#include <queue>	
#include <iostream>
#include <winsock2.h>

#include "../HeaderFiles/SharedPointer.h"
#include "../HeaderFiles/SpinLock.h"

class Sender
{
public:
	std::queue<SharedPointer<char>>	m_queue;
	WORD					m_bytes;

	bool					m_isSend;

	SpinLock				m_spinLock;

private:
	bool					m_failed;

	WSABUF					m_wsaBuf;
	DWORD					m_sendBytes;
	DWORD					m_flag;

public:
	Sender();
	~Sender();

	void Reset();	
	bool ASyncSend(SOCKET _socket, WSAOVERLAPPED& _overlapped);
	void ReSend();
};