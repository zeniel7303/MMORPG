#pragma once
#include "../../ZoneServer/ServerLibrary/HeaderFiles/AcceptorSession.h"

#pragma comment(lib, "ws2_32.lib")

class Acceptor : public AcceptorSession
{
public:
	Acceptor(const char* _ip, const unsigned short _port, HANDLE _handle, int _num);
	~Acceptor();

	void OnAccept();
};