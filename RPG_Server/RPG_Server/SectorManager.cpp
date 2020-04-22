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

	for (int j = 0; j < SIZE; j++)
	{
		for (int i = 0; i < SIZE; i++)
		{
			Sector* sector;
			sector = new Sector(zonetilesData.GetTiles(i, j), i + j * SIZE);

			m_sectorMap.insert(make_pair(i + j * SIZE, sector));
		}
	}
}

Sector* SectorManager::GetSector(int _x, int _y)
{
	m_locker.EnterLock();

	int x = _x / SIZE;
	int y = _y / SIZE;

	map<WORD, Sector*>::iterator iter = m_sectorMap.find(x + y * SIZE);
	if (iter == m_sectorMap.end()) return nullptr;

	m_locker.LeaveLock();

	return iter->second;
}

Sector* SectorManager::GetSector(int _num)
{
	m_locker.EnterLock();

	map<WORD, Sector*>::iterator iter = m_sectorMap.find(_num);
	if (iter == m_sectorMap.end()) return nullptr;

	m_locker.LeaveLock();

	return iter->second;
}

Sector** SectorManager::GetNeighborSectors(Sector* _sectors[], Sector* _sector)
{
	m_locker.EnterLock();

	int tempNum = _sector->GetSectorNum();

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			map<WORD, Sector*>::iterator iter 
				= m_sectorMap.find(tempNum + (-11 + i) + (j * SIZE));

			if (iter != m_sectorMap.end())
			{
				_sectors[i + j * 3] = iter->second;
			}
		}
	}

	/*for (int i = 0; i < 9; i++)
	{
		printf("%d ", _sectors[i]->GetSectorNum());
	}

	printf("\n=======================================\n");*/

	m_locker.LeaveLock();

	return _sectors;
}