#pragma once

#include "Sector.h"
#include "ZoneTilesData.h"

class SectorManager
{
private:
	ZoneTilesData zonetilesData;

	std::map<WORD, Sector*> m_sectorMap;

public:
	SectorManager();
	~SectorManager();

	void Init(ZoneTilesData* _data);
	Sector* GetSector(int _x, int _y);
	Sector** GetNeighborSectors(Sector* _sectors[], Sector* _sector);
};

