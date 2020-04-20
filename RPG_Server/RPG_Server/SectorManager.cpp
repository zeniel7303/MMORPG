#include "SectorManager.h"

SectorManager::SectorManager()
{
}

SectorManager::~SectorManager()
{
}

void SectorManager::Init(ZoneTilesData* _data)
{
	zonetilesData = *_data;

	for (int j = 0; j < 10; j++)
	{
		for (int i = 0; i < 10; i++)
		{
			Sector* sector;
			sector = new Sector(zonetilesData.GetTiles(j, i), i + j * 10);

			m_sectorMap.insert(make_pair(i + j * 10, sector));
		}
	}
}

Sector* SectorManager::GetSector(int _x, int _y)
{
	int x = _x / 10;
	int y = _y / 10;

	map<WORD, Sector*>::iterator iter = m_sectorMap.find(x + y * 10);
	if (iter == m_sectorMap.end()) return nullptr;

	return iter->second;
}

Sector** SectorManager::GetNeighborSectors(Sector* _sectors[], Sector* _sector)
{
	int tempNum = _sector->GetSectorNum();

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			map<WORD, Sector*>::iterator iter 
				= m_sectorMap.find(tempNum + (-11 + i) + (j * 10));

			if (iter != m_sectorMap.end())
			{
				_sectors[i + j * 3] = iter->second;
			}
		}
	}

	return _sectors;
}