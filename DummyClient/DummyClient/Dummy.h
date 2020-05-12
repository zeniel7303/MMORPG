#pragma once
#include "Session.h"
#include "Tile.h"
#include "PathFinding.h"
#include "FieldTilesData.h"

class Dummy : public Session
{
private:
	float currentTime;
	float intervalTime;

	int patrolRange;

	Tile* tile;

	PathFinding pathFind;
	list<VECTOR2> tileList;
	VECTOR2 m_targetPosition;
	FieldTilesData tilesData;

public:
	Dummy(int _num, int _fieldNum, FieldTilesData* _data);
	~Dummy();

	bool Connect(IpEndPoint& _ipEndPoint);
	void OnConnect(SOCKET _socket);
	void RandomTask();
	void FSM();
	void Move();
	bool PathMove();
};

