#include "ZoneServerManager.h"

#include "ZoneConnector.h"

ZoneServerManager::ZoneServerManager()
{
}

ZoneServerManager::~ZoneServerManager()
{
}

ZoneConnector* ZoneServerManager::GetZoneConnector(int _num)
{ 
	/*if (_num > m_zoneConnectorVec.size())
	{
		return nullptr;
	}*/

	return m_zoneConnectorVec[_num]; 
}