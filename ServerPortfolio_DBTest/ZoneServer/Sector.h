#pragma once
#include <vector>
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/ManagerFrame_List.h"

#include "User.h"
#include "Monster.h"

class Sector
{
private:
	int m_sectorNum;

	std::vector<Sector*> m_roundSectorsVec;

	Manager_List<User> m_userList;
	Manager_List<Monster> m_monsterList;

public:
	Sector(int _num);
	~Sector();

	int GetSectorNum() { return m_sectorNum; }
	std::vector<Sector*>& GetRoundSectorsVec() { return m_roundSectorsVec; }
	Manager_List<User>* GetUserList() { return &m_userList; }
	Manager_List<Monster>* GetMonsterList() { return &m_monsterList; }
};