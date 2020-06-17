#pragma once
#include <map>
#include "ThreadClass.h"
#include "Monster.h"
#include "FieldTilesData.h"
#include "SectorManager.h"

using namespace std;

//=====================================================

//MonsterLogicThread

//Field의 모든 Monster의 행동을 관리하는 Thread

//=====================================================

class Field;

class MonsterLogicThread : public ThreadClass<MonsterLogicThread>
{
private:
	Field* m_field;
	FieldTilesData* m_fieldTilesData;
	SectorManager* m_sectorManager;

	//몬스터는 자주 찾아야하기하므로 map으로 만든다.
	map<int, Monster*> m_monsterMap;

public:
	MonsterLogicThread();
	~MonsterLogicThread();

	void Init(Field* _field, FieldTilesData* _fieldTilesData, SectorManager* _sectorManager);

	void LoopRun();

	void SendMonsterList(User* _user);
	void SendMonsterList_InRange(User* _user);
	Monster* GetMonster(int _num);
};

