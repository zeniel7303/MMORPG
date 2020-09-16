#pragma once
#include "../../ZoneServer/ServerLibrary/HeaderFiles/FileLog.h"
#include "../../ZoneServer/ServerLibrary/HeaderFiles/OnlyHeaders/Thread.h"
#include "../../ZoneServer/ServerLibrary/HeaderFiles/SendBuffer.h"

#include "packet.h"

#include "PathFinding.h"

#include "PathFinderAgent.h"

enum STATE
{
	READY,
	ACTIVE
};

class PathFinder : Thread<PathFinder>
{
public:
	Packet*				m_packet;
	PathFinderAgent*	m_pathFinderAgent;

	Tile*				m_nowTile;
	Tile*				m_targetTile;

	HANDLE				m_hEvent;

private:
	STATE				m_state;

	PathFinding			m_pathFind;
	
	std::list<VECTOR2>	m_tileList;

	SendBuffer*			m_sendBuffer;

public:
	PathFinder();
	~PathFinder();

	void LoopRun();

	STATE GetState() { return m_state; }
};

