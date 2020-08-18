#include "../HeaderFiles/Sender.h"

Sender::Sender()
{
}

Sender::~Sender()
{
	delete m_sendBuffer;
}

void Sender::Init()
{
	m_sendBuffer = new SendBuffer(30000);

	m_sendBytes = 0;
}

void Sender::Reset()
{
	m_sendBuffer->Reset();

	m_sendBytes = 0;
}

void Sender::Send(SOCKET _socket, char* _data, DWORD _bytes)
{
	send(_socket, _data, _bytes, 0);
}

void Sender::ReSend()
{

}