#pragma once
#include "../../ZoneServer/ServerLibrary/HeaderFiles/IOCPClass.h"
#include "../../ZoneServer/ServerLibrary/HeaderFiles/ClientSession.h"

#include "Tile.h"
#include "PathFinding.h"
#include "FieldTilesData.h"

#include "loginSession.h"
#include "zoneSession.h"

class Dummy
{
public:
	loginSession* m_loginSession;
	zoneSession* m_zoneSession;

private:
	float currentTime;
	float intervalTime;
	float heartBeatTime;

	int patrolRange;

	Tile* tile;

	PathFinding pathFinding;

	list<VECTOR2> tileList;
	VECTOR2 m_targetPosition;
	FieldTilesData tilesData;

public:
	Dummy(int _num1, int _num2, IOCPClass* _iocp);
	~Dummy();

	void FSM();
	void Move();
	bool PathMove();

	void SetTilesData(FieldTilesData* _data) { tilesData = *_data; }
};