#include "Sector.h"

Sector::Sector(int _num)
{
	m_sectorNum = _num;
}

Sector::~Sector()
{
	m_roundSectorsVec.clear();
	m_roundSectorsVec.resize(0);
}