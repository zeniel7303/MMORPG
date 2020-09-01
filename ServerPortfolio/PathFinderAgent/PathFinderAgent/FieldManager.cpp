#include "FieldManager.h"

FieldManager::FieldManager()
{
	FieldTilesData* villageField;
	villageField = new FieldTilesData("Village.fmap");
	m_fieldDataMap.insert(make_pair(1, villageField));

	FieldTilesData* fieldField;
	fieldField = new FieldTilesData("Field.fmap");
	m_fieldDataMap.insert(make_pair(2, fieldField));

	FieldTilesData* testField;
	testField = new FieldTilesData("TestScene.fmap");
	m_fieldDataMap.insert(make_pair(3, testField));
}

FieldManager::~FieldManager()
{
}

FieldTilesData* FieldManager::GetFieldData(int _num)
{
	std::map<unsigned short, FieldTilesData*>::iterator iter = m_fieldDataMap.find(_num);
	if (iter == m_fieldDataMap.end()) return nullptr;

	return iter->second;
}