#pragma once
#include <iostream>
#include <fstream>

#include "Tile.h"

using namespace std;

//=====================================================

//ZoneTilesData

//Zone의 타일 정보를 가지고 있는 클래스

//=====================================================

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

