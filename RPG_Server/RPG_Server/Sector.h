#pragma once
#include "Tile.h"
#include "User.h"
#include "Monster.h"

class Sector
{
private:
	std::list<User*> userList;
	std::list<Monster*> monsterList;

	int m_sectorNum;

	Tile** m_tiles;

public:
	Sector(Tile** _tiles, int _num);
	~Sector();

	void Init();

	int GetSectorNum() { return m_sectorNum; }
};