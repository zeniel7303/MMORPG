#include "ZoneManager.h"

ZoneManager::ZoneManager()
{
}


ZoneManager::~ZoneManager()
{
}

void ZoneManager::Init()
{
	Zone* m_villageZone = new Zone();
	m_villageZone->Init(1, VECTOR2(17, 19));
	m_villageZone->GetMap("Village.fmap");
	m_zoneMap.insert(make_pair(m_villageZone->GetZoneNum(), m_villageZone));

	Zone* m_fieldZone = new Zone();
	m_fieldZone->Init(2, VECTOR2(77, 75));
	m_fieldZone->GetMap("Field.fmap");
	m_zoneMap.insert(make_pair(m_fieldZone->GetZoneNum(), m_fieldZone));
}

Zone* ZoneManager::GetZone(int _num)
{
	map<WORD, Zone*>::iterator iter = m_zoneMap.find(_num);
	if (iter == m_zoneMap.end()) return nullptr;

	return iter->second;
}