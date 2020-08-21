#pragma once
#include "../ServerLibrary/HeaderFiles/AcceptorSession.h"

class Acceptor : public AcceptorSession
{
private:
	bool	m_isFirstAccept;

public:
	Acceptor(const char* _ip, const unsigned short _port, HANDLE _handle, int _num);
	~Acceptor();

	void OnAccept();
};
