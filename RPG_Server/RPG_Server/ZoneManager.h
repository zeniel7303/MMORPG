#pragma once
#include "Zone.h"

//=====================================================

//ZoneManager

//Zone들을 관리하기위한 매니저

//=====================================================

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