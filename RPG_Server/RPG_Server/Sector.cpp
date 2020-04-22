#include "Sector.h"

Sector::Sector(Tile** _tiles, int _num)
{
	m_tiles = _tiles;
	m_sectorNum = _num;
}

Sector::~Sector()
{
}

bool Sector::SectorRangeCheck(Tile* _tile)
{
	bool check = false;

	for (int x = 0; x < 10; x++)
	{
		for (int y = 0; y < 10; y++)
		{
			if (m_tiles[x][y].GetX() == _tile->GetX() &&
				m_tiles[x][y].GetY() == _tile->GetY())
			{
				check = true;

				break;
			}
		}

		if (check) break;
	}

	return check;
}