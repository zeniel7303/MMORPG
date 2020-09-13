#pragma once
#include "Session.h"

class loginDummy : public Session
{
public:
	loginDummy();
	~loginDummy();

	bool Connect();
	void OnConnect(SOCKET _socket);

	void SetDummyNum(int _num) { dummyNum = _num; }
};

