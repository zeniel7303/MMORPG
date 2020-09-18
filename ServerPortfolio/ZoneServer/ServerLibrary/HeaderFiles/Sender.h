#pragma once
#pragma warning(disable:4005)

#include <iostream>
#include <winsock2.h>

#include "../HeaderFiles/SendBuffer.h"

class Sender
{
private:
	SendBuffer*				m_sendBuffer;

	WSABUF					m_wsaBuf;
	DWORD					m_sendBytes;

public:
	Sender();
	~Sender();

	void Init();
	void Reset();
	void Write();
	void Send(SOCKET _socket, char* _data, DWORD _bytes);
	void ReSend();

	SendBuffer* GetSendBuffer() { return m_sendBuffer; }
};

