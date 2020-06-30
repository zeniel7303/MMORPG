#pragma once
#include <iostream>
#include <fstream>

#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/Utils.h"


#include "Tile.h"

using namespace std;

//=====================================================

//FieldTilesData

//Field의 타일 정보를 가지고 있는 클래스

//=====================================================

class FieldTilesData
{
private:
	int m_mapSizeX;
	int m_mapSizeY;

	Tile** m_Map;

public:
	FieldTilesData();
	~FieldTilesData();

	bool GetMap(const char * _name);
	Tile** GetMap();
	int GetMapSizeX();
	int GetMapSizeY();
	Tile* GetTile(int _x, int _y);
	//Tile** GetTiles(int _x, int _y);
};

