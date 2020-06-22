#pragma once
#include <Windows.h>

#include "SendBuffer.h"

class Sender
{
private:
	SendBuffer*				m_sendBuffer;

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

