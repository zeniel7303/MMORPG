#pragma once
#include "../../ZoneServer/ServerLibrary/HeaderFiles/AcceptorSession.h"

class ZoneServerAcceptor : public AcceptorSession
{
public:
	ZoneServerAcceptor(const char* _ip, const unsigned short _port, HANDLE _handle, int _num);
	~ZoneServerAcceptor();

	void OnAccept();
};

