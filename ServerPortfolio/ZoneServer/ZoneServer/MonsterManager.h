#pragma once
#include <map>
#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/Utils.h"

#include "Monster.h"
#include "FieldTilesData.h"
#include "SectorManager.h"

//=====================================================

//MonsterManager

//Field�� ��� Monster���� �����ϴ� Class

//=====================================================

class Field;

class MonsterManager
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

	void Update();

	void SendMonsterList(User* _user);
	void SendMonsterList_InRange(User* _user);
	Monster* GetMonster(int _num);
};