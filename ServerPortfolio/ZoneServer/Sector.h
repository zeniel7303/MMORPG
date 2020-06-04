#pragma once
#include <vector>
#include "../ServerLibrary/HeaderFiles/SubHeaders/ManagerFrame_List.h"

#include "User.h"
#include "Monster.h"

class Sector : public Manager_List<User> ,public Manager_List<Monster>
{
private:
	int m_sectorNum;

	std::vector<Sector*> m_roundSectorsVec;

public:
	Sector(int _num);
	~Sector();

	int GetSectorNum() { return m_sectorNum; }
	std::vector<Sector*>* GetRoundSectorsVec() { return &m_roundSectorsVec; }
};