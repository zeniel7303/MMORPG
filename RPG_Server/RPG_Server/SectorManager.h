#pragma once

#include "Sector.h"
#include "ZoneTilesData.h"
#include "CriticalSectionClass.h"

#define SIZE 10

class SectorManager
{
private:
	ZoneTilesData zonetilesData;

	std::map<WORD, Sector*> m_sectorMap;

	CriticalSectionClass m_locker;

public:
	SectorManager();
	~SectorManager();

	void Init(ZoneTilesData* _data);
	Sector* GetSector(int _x, int _y);
	Sector* GetSector(int _num);
	Sector** GetNeighborSectors(Sector* _sectors[], Sector* _sector);

	int GetSize() { return SIZE; }
};

