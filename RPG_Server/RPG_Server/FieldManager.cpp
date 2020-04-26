#include "FieldManager.h"

FieldManager::FieldManager()
{
}


FieldManager::~FieldManager()
{
}

void FieldManager::Init()
{
	Field* m_villageField = new Field();
	m_villageField->Init(1, VECTOR2(17, 19));
	m_villageField->GetMap("Village.fmap");
	m_fieldMap.insert(make_pair(m_villageField->GetFieldNum(), m_villageField));

	Field* m_fieldField = new Field();
	m_fieldField->Init(2, VECTOR2(77, 75));
	m_fieldField->GetMap("Field.fmap");
	m_fieldMap.insert(make_pair(m_fieldField->GetFieldNum(), m_fieldField));

	Field* m_testField = new Field();
	m_testField->Init(999, VECTOR2(48, 48));
	m_testField->GetMap("TestScene.fmap");
	m_fieldMap.insert(make_pair(m_testField->GetFieldNum(), m_testField));
}

Field* FieldManager::GetField(int _num)
{
	map<WORD, Field*>::iterator iter = m_fieldMap.find(_num);
	if (iter == m_fieldMap.end()) return nullptr;

	return iter->second;
}