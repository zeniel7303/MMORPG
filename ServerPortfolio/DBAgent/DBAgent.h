#pragma once
#include "../ServerLibrary/HeaderFiles/ClientSession.h"

#include "packet.h"

#pragma comment(lib, "ws2_32.lib")

class DBAgent : public ClientSession
{
private:

public:
	DBAgent();
	~DBAgent();

	void OnConnect();
	void DisConnect();
	void Reset();
	void OnRecv();
};

