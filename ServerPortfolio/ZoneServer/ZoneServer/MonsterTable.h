#pragma once
#define _WINSOCKAPI_

#include "packet.h"

#include "MainThread.h"

class MonsterTable
{
public:
	static MonsterTable* getSingleton()
	{
		static MonsterTable singleton;

		return &singleton;
	}
private:
	std::vector<MonsterData>	m_monsterDataVec;

	MonsterTable();
public:
	~MonsterTable();

	void GetMonstersData(Packet* _packet);

	MonsterData* GetMonsterData(int _num) { return &m_monsterDataVec[_num]; }
};

