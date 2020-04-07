#pragma once
#include <map>
#include "ThreadClass.h"
#include "Monster.h"
#include "ZoneTilesData.h"

using namespace std;

//=====================================================

//MonsterLogicThread

//

//=====================================================

class Zone;

class MonsterLogicThread : public ThreadClass<MonsterLogicThread>
{
private:
	Zone* m_zone;
	ZoneTilesData* m_zoneTilesData;

	//몬스터는 자주 찾아야하기하므로 map으로 만든다.
	map<int, Monster*> m_monsterMap;

public:
	MonsterLogicThread();
	~MonsterLogicThread();

	void Init(Zone* _zone, ZoneTilesData* _zoneTilesData);

	void LoopRun();

	void SendMonsterList(User* _user);
	Monster* GetMonster(int _num);
};

