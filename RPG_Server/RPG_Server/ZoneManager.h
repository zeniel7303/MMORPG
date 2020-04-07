#pragma once
#include "Zone.h"

class ZoneManager
{
private:
	map<WORD, Zone*> m_zoneMap;

public:
	ZoneManager();
	~ZoneManager();

	void Init();

	Zone* GetZone(int _num);
};