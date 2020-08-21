#pragma once
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/IpEndPoint.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/DoubleQueue.h"
#include "../ServerLibrary/HeaderFiles/ClientSession.h"

#include "packet.h"

#include "MainThread.h"

class ZoneConnector : public ClientSession
{
public:
	ZoneConnector();
	~ZoneConnector();

	void OnConnect();
	void DisConnect();
	void Reset();

	void OnRecv();
};

