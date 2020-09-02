#pragma once
#include <map>
#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/Utils.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/Thread.h"

#include "Monster.h"
#include "FieldTilesData.h"
#include "SectorManager.h"

//=====================================================

//MonsterManager

//Field의 모든 Monster들을 관리하는 Class

//=====================================================

class Field;

class MonsterManager : public Thread<MonsterManager>
{
private:
	Field*					m_field;
	FieldTilesData*			m_fieldTilesData;
	SectorManager*			m_sectorManager;

	std::vector<Monster*>	m_monsterVec;

public:
	MonsterManager(Field* _field, FieldTilesData* _fieldTilesData,
		SectorManager* _sectorManager);
	~MonsterManager();

	bool CreateMonsters();

	void LoopRun();

	void SendMonsterList(User* _user);
	void SendMonsterList_InRange(User* _user);
	Monster* GetMonster(int _num);
};