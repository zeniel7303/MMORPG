#include "SectorManager.h"

SectorManager::SectorManager()
{
	for (int j = 0; j < SIZE; j++)
	{
		for (int i = 0; i < SIZE; i++)
		{
			Sector* sector;
			sector = new Sector(i + j * SIZE);

			m_sectorMap.insert(make_pair(i + j * SIZE, sector));
		}
	}

	for (int i = 0; i < SIZE * SIZE; i++)
	{
		GetNeighborSectors(m_sectorMap[i]->GetRoundSectorsVec(), m_sectorMap[i]);
	}
}

SectorManager::~SectorManager()
{
	map<WORD, Sector*>::iterator i;
	for (i = m_sectorMap.begin(); i != m_sectorMap.end(); i++)
	{
		delete i->second;
	}
}

Sector* SectorManager::GetSector(int _x, int _y)
{
	int x = _x / SIZE;
	int y = _y / SIZE;

	std::map<WORD, Sector*>::iterator iter = m_sectorMap.find(x + y * SIZE);
	if (iter == m_sectorMap.end()) return nullptr;

	return iter->second;
}

Sector* SectorManager::GetSector(int _num)
{
	std::map<WORD, Sector*>::iterator iter = m_sectorMap.find(_num);
	if (iter == m_sectorMap.end()) return nullptr;

	return iter->second;
}

void SectorManager::GetNeighborSectors(std::vector<Sector*>& _sectorsVec, Sector* _sector)
{
	int tempNum = _sector->GetSectorNum();

	switch (tempNum)
	{
	case 0:
	{
		_sectorsVec.push_back(m_sectorMap.find(0)->second);
		_sectorsVec.push_back(m_sectorMap.find(1)->second);
		_sectorsVec.push_back(m_sectorMap.find(SIZE)->second);
		_sectorsVec.push_back(m_sectorMap.find(SIZE + 1)->second);
	}
		break;
	case SIZE - 1:
	{
		int temp = SIZE - 1;

		_sectorsVec.push_back(m_sectorMap.find(temp - 1)->second);
		_sectorsVec.push_back(m_sectorMap.find(temp)->second);
		_sectorsVec.push_back(m_sectorMap.find(temp + (SIZE - 1))->second);
		_sectorsVec.push_back(m_sectorMap.find(temp + SIZE)->second);
	}
		break;
	case SIZE * SIZE - SIZE:
	{
		int temp = SIZE * SIZE - SIZE;

		_sectorsVec.push_back(m_sectorMap.find(temp - SIZE)->second);
		_sectorsVec.push_back(m_sectorMap.find(temp - (SIZE - 1))->second);
		_sectorsVec.push_back(m_sectorMap.find(temp)->second);
		_sectorsVec.push_back(m_sectorMap.find(temp + 1)->second);
	}
		break;
	case SIZE * SIZE - 1:
	{
		int temp = SIZE * SIZE - 1;

		_sectorsVec.push_back(m_sectorMap.find(temp - (SIZE + 1))->second);
		_sectorsVec.push_back(m_sectorMap.find(temp - SIZE)->second);
		_sectorsVec.push_back(m_sectorMap.find(temp - 1)->second);
		_sectorsVec.push_back(m_sectorMap.find(temp)->second);
	}
		break;
	default:
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				int temp = tempNum + (-11 + i) + (j * SIZE);

				std::map<WORD, Sector*>::iterator iter
					= m_sectorMap.find(tempNum + (-11 + i) + (j * SIZE));

				if (iter != m_sectorMap.end())
				{
					_sectorsVec.push_back(iter->second);
				}
			}
		}
	}
		break;
	}

	sort(_sectorsVec.begin(), _sectorsVec.end());
}