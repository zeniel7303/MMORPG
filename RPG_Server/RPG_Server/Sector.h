#pragma once
#include "ManagerFrame.h"

#include "Tile.h"
#include "User.h"
#include "Monster.h"

class Sector : public Manager<User> ,public Manager<Monster>
{
private:
	std::list<User*> userList;
	std::list<Monster*> monsterList;

	int m_sectorNum;

	Tile** m_tiles;

public:
	Sector(Tile** _tiles, int _num);
	~Sector();

	bool SectorRangeCheck(Tile* _tile);

	int GetSectorNum() { return m_sectorNum; }
};