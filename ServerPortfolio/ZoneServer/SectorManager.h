#pragma once
#include <map>
#include <algorithm>

#include "Sector.h"
#include "FieldTilesData.h"

#define SIZE 10

class SectorManager
{
private:
	std::map<WORD, Sector*> m_sectorMap;

public:
	SectorManager();
	~SectorManager();

	void Init();
	Sector* GetSector(int _x, int _y);
	Sector* GetSector(int _num);
	//void GetNeighborSectors(Sector* _sectors[], Sector* _sector);
	void GetNeighborSectors(std::vector<Sector*>* _sectorsVec, Sector* _sector);

	int GetSize() { return SIZE; }
};

