#pragma once
#include <vector>

class ZoneConnector;

class ZoneServerManager
{
private:
	std::vector<ZoneConnector*> m_zoneConnectorVec;

public:
	ZoneServerManager();
	~ZoneServerManager();

	ZoneConnector* GetZoneConnector(int _num);

	std::vector<ZoneConnector*>* GetZoneConnectorVec() { return &m_zoneConnectorVec; }
};

