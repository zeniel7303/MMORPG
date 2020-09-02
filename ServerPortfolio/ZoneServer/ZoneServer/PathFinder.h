#pragma once
#include "../../ZoneServer/ServerLibrary/HeaderFiles/FileLog.h"
#include "../../ZoneServer/ServerLibrary/HeaderFiles/OnlyHeaders/Thread.h"
#include "../../ZoneServer/ServerLibrary/HeaderFiles/SendBuffer.h"

#include "packet.h"

#include "PathFinding.h"

#include "Monster.h"

enum PATHFINDER_STATE
{
	PATHFIND_READY,
	PATHFIND_ACTIVE
};

class PathFinder : Thread<PathFinder>
{
public:
	Packet*				m_packet;
	Monster*			m_monster;

	HANDLE				m_hEvent;

private:
	PATHFINDER_STATE	m_state;

	PathFinding			m_pathFind;
	Tile*				m_nowTile;
	Tile*				m_targetTile;

	SendBuffer*			m_sendBuffer;

public:
	PathFinder();
	~PathFinder();

	void LoopRun();

	PATHFINDER_STATE GetState() { return m_state; }
};

