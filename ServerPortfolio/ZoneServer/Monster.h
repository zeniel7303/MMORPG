#pragma once
#include "stdafx.h"

#include "../ServerLibrary/HeaderFiles/OnlyHeaders/DoubleQueue.h"
#include "../ServerLibrary/HeaderFiles/CriticalSection.h"

#include "packet.h"
#include "UnitInfo.h"
#include "SendBuffer.h"
#include "PathFinding.h"
#include "FieldTilesData.h"

//=====================================================

//Monster

//Monster 객체이다.
//PacketQueuePair는 Singleton으로 선언된 ServerLogicThread와 공유하여 Monster가 보낼 패킷을 저장해두는 용도로 쓴다.
//PathFinding을 상속받아 몬스터의 길찾기에 이용된다.

//=====================================================

class User;
class Field;
class SectorManager;
class Sector;
class ServerLogicThread;

class Monster : public PathFinding
{
private:
	MonsterInfo				m_info;
	MonsterData				m_data;

	VECTOR2					m_spawnPosition;

	Tile*					m_homeTile;
	Tile*					m_nowTile;
	Tile*					m_targetTile;
	VECTOR2					m_targetPosition;

	list<VECTOR2>			m_tileList;

	//========================================

	Field*					m_field;
	FieldTilesData*			m_fieldTilesData;
	SectorManager*			m_sectorManager;

	Sector*					m_sector;

	User*					m_target;

	CRITICAL_SECTION		m_cs;

	//========================================

	SendBuffer*				m_sendBuffer;

	//========================================

	bool					m_isDeath;
	bool					m_isAttack;

	float					m_currentTime;
	float					m_maxTime;

public:
	Monster(Field* _field, FieldTilesData* _fieldTilesData, SectorManager* _sectorManager)
	{
		m_field = _field;
		m_fieldTilesData = _fieldTilesData;
		m_sectorManager = _sectorManager;

		InitializeCriticalSection(&m_cs);
	};

	~Monster();

	void Init(MonsterInfo& _info, MonsterData& _data);
	void Reset();

	void Update();

	void Spawn();
	void Move();

	void Attack();
	Packet* Hit(User* _user, int _damage);

	void FSM();

	void PathFindStart(Tile* _targetTile);
	bool PathMove();

	void UpdateSector();

	MonsterInfo GetInfo() { return m_info; }
	MonsterData GetData() { return m_data; }
	Field* GetField() { return m_field; }
	Sector* GetSector() { return m_sector; }
	User* GetTarget() { return m_target; }

	bool IsDeath() { return m_isDeath; }
};

