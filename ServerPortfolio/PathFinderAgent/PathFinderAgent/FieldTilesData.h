#pragma once
#include <iostream>
#include <fstream>

#include "../../ZoneServer/ServerLibrary/HeaderFiles/FileLog.h"
#include "../../ZoneServer/ServerLibrary/HeaderFiles/Utils.h"

#include "Tile.h"

using namespace std;

class FieldTilesData
{
private:
	int m_mapSizeX;
	int m_mapSizeY;

	Tile** m_Map;

public:
	FieldTilesData(const char * _name);
	~FieldTilesData();

	bool GetMap(const char * _name);
	Tile** GetMap();
	int GetMapSizeX();
	int GetMapSizeY();
	Tile* GetTile(int _x, int _y);
	//Tile** GetTiles(int _x, int _y);
};