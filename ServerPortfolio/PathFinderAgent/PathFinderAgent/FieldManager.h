#pragma once
#include <map>
#include "../../ZoneServer/ServerLibrary/HeaderFiles/FileLog.h"
#include "../../ZoneServer/ServerLibrary/HeaderFiles/Utils.h"

#include "FieldTilesData.h"

class FieldManager
{
private:
	std::map<unsigned short, 
		FieldTilesData*>	m_fieldDataMap;
	int						m_fieldDataCount;

public:
	FieldManager();
	~FieldManager();

	FieldTilesData* GetFieldData(int _num);
};

