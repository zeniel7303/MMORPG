#pragma once
#include <iostream>
#include <fstream>

#include "Tile.h"

using namespace std;

class ZoneTilesData
{
private:
	int m_mapSizeX;
	int m_mapSizeY;

	Tile** m_Map;

public:
	ZoneTilesData();
	~ZoneTilesData();

	void GetMap(const char * _name);
	Tile** GetMap();
	int GetMapSizeX();
	int GetMapSizeY();
	Tile* GetTile(int _x, int _y);
};

