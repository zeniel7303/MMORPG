#pragma once
#include <map>
#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/Utils.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/Thread.h"

#include "Monster.h"
#include "FieldTilesData.h"
#include "SectorManager.h"

//=====================================================

//MonsterLogicThread

//Field의 모든 Monster의 행동을 관리하는 Thread

//=====================================================

class Field;

class MonsterLogicThread : public Thread<MonsterLogicThread>
{
private:
	Field*					m_field;
	FieldTilesData*			m_fieldTilesData;
	SectorManager*			m_sectorManager;

	std::vector<Monster*>	m_monsterVec;

public:
	MonsterLogicThread(Field* _field, FieldTilesData* _fieldTilesData,
		SectorManager* _sectorManager);
	~MonsterLogicThread();

	bool CreateMonsters();

	void LoopRun();

	void SendMonsterList(User* _user);
	void SendMonsterList_InRange(User* _user);
	Monster* GetMonster(int _num);
};