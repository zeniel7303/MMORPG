#include "FieldTilesData.h"

FieldTilesData::FieldTilesData(const char * _name)
{
	GetMap(_name);
}

FieldTilesData::~FieldTilesData()
{
	for (int i = 0; i < m_mapSizeY; i++)
	{
		delete[] m_Map[i];
	}

	delete m_Map;
}

bool FieldTilesData::GetMap(const char * _name)
{
	ifstream inputBinary(_name, ios::in | ios::binary);

	inputBinary.read(reinterpret_cast<char*>(&m_mapSizeX), sizeof(int));
	inputBinary.read(reinterpret_cast<char*>(&m_mapSizeY), sizeof(int));

	TRYCATCH(m_Map = new Tile*[m_mapSizeY]);

	for (int y = 0; y < m_mapSizeY; y++)
	{
		TRYCATCH(m_Map[y] = new Tile[m_mapSizeX]);

		for (int x = 0; x < m_mapSizeX; x++)
		{
			TileType tileType;
			unsigned short monsterIndex;

			inputBinary.read(reinterpret_cast<char*>(&tileType), sizeof(TileType));
			inputBinary.read(reinterpret_cast<char*>(&monsterIndex), sizeof(unsigned short));

			m_Map[y][x].SetTile(x, y, tileType, monsterIndex);
		}
	}

	Tile* tile;
	Tile* currentTile;

	//이웃타일들을 미리 저장 Tile의 방위표시는 이것때문.
	for (int y = 0; y < m_mapSizeY; y++)
	{
		for (int x = 0; x < m_mapSizeX; x++)
		{
			tile = &m_Map[y][x];
			currentTile = nullptr;

			if (x < m_mapSizeX - 1)
			{
				currentTile = &m_Map[y][x + 1];
				tile->SetNeighborTiles(E, currentTile);

				if (y < m_mapSizeY - 1)
				{
					currentTile = &m_Map[y + 1][x + 1];
					tile->SetNeighborTiles(NE, currentTile);
				}
				if (y > 0)
				{
					currentTile = &m_Map[y - 1][x + 1];
					tile->SetNeighborTiles(SE, currentTile);
				}
			}
			if (x > 0)
			{
				currentTile = &m_Map[y][x - 1];
				tile->SetNeighborTiles(W, currentTile);

				if (y < m_mapSizeY - 1)
				{
					currentTile = &m_Map[y + 1][x - 1];
					tile->SetNeighborTiles(NW, currentTile);
				}
				if (y > 0)
				{
					currentTile = &m_Map[y - 1][x - 1];
					tile->SetNeighborTiles(SW, currentTile);
				}
			}
			if (y < m_mapSizeY - 1)
			{
				currentTile = &m_Map[y + 1][x];
				tile->SetNeighborTiles(N, currentTile);
			}
			if (y > 0)
			{
				currentTile = &m_Map[y - 1][x];
				tile->SetNeighborTiles(S, currentTile);
			}
		}
	}

	return true;
}

Tile** FieldTilesData::GetMap()
{
	return m_Map;
}

int FieldTilesData::GetMapSizeX()
{
	return m_mapSizeX;
}

int FieldTilesData::GetMapSizeY()
{
	return m_mapSizeY;
}

Tile* FieldTilesData::GetTile(int _x, int _y)
{
	if (_x < 0 || _x >= m_mapSizeX ||
		_y < 0 || _y >= m_mapSizeY)
		return nullptr;

	return &m_Map[_y][_x];
}

/*Tile** FieldTilesData::GetTiles(int _x, int _y)
{
	Tile** tempTiles;

	try
	{
		tempTiles = new Tile*[10];
	}
	catch (const std::bad_alloc& error)
	{
		printf("bad alloc : %s\n", error.what());
		return nullptr;
	}

	for (int y = (_y * 10); y < (_y * 10) + 10; y++)
	{
		try
		{
			tempTiles[y % 10] = new Tile[10];
		}
		catch (const std::bad_alloc& error)
		{
			printf("bad alloc : %s\n", error.what());
			return nullptr;
		}

		for (int x = (_x * 10); x < (_x * 10) + 10; x++)
		{
			tempTiles[y % 10][x % 10] = m_Map[y][x];
		}
	}

	//printf("\n");
	//
	//for (int j = 0; j < 10; j++)
	//{
	//	for (int i = 0; i < 10; i++)
	//	{
	//		printf("(%d,%d) ", tempTiles[j][i].GetX(), tempTiles[j][i].GetY());
	//	}
	//
	//	printf("\n");
	//}

	return tempTiles;
}*/