#pragma once
#include "ManagerFrame.h"

#include "User.h"
#include "Monster.h"

class Sector : public Manager<User> ,public Manager<Monster>
{
private:
	int m_sectorNum;

	//Sector* m_roundSectors[9];
	std::vector<Sector*> m_roundSectorsVec;

public:
	Sector(int _num);
	~Sector();

	int GetSectorNum() { return m_sectorNum; }
	std::vector<Sector*>* GetRoundSectorsVec() { return &m_roundSectorsVec; }
};