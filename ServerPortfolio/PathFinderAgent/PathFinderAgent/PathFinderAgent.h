#pragma once
#include "stdafx.h"

#include "../../ZoneServer/ServerLibrary/HeaderFiles/ClientSession.h"

#include "packet.h"

class PathFinderAgent : public ClientSession
{
public:
	PathFinderAgent();
	~PathFinderAgent();

	void OnConnect();
	void DisConnect();
	void Reset();

	void OnRecv();
};

