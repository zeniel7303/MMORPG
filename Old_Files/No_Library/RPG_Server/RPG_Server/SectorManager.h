#pragma once

#include "Sector.h"
#include "FieldTilesData.h"
#include "CriticalSectionClass.h"

#define SIZE 10

class SectorManager
{
private:
	FieldTilesData m_fieldtilesData;

	std::map<WORD, Sector*> m_sectorMap;

	CriticalSectionClass m_lock;

public:
	SectorManager();
	~SectorManager();

	void Init(FieldTilesData* _data);
	Sector* GetSector(int _x, int _y);
	Sector* GetSector(int _num);
	//void GetNeighborSectors(Sector* _sectors[], Sector* _sector);
	void GetNeighborSectors(std::vector<Sector*>* _sectorsVec, Sector* _sector);

	int GetSize() { return SIZE; }
};

