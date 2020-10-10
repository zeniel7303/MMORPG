#include "FieldManager.h"

FieldManager::FieldManager()
{
	m_failed = false;

	m_fieldCount = 0;

	Field* m_villageField;
	TRYCATCH_CONSTRUCTOR(m_villageField = new Field(1, new VECTOR2(17, 19), "Village.fmap"), m_failed);
	if (m_villageField->IsFailed() || m_failed)
	{
		m_failed = true;

		return;
	}	
	MYDEBUG("[ %d Field Init ] \n", m_villageField->GetFieldNum());
	m_fieldMap.insert(make_pair(m_villageField->GetFieldNum(), m_villageField));
	m_fieldCount += 1;

	Field* m_fieldField;
	TRYCATCH_CONSTRUCTOR(m_fieldField = new Field(2, new VECTOR2(77, 75), "Field.fmap"), m_failed);
	if (m_fieldField->IsFailed() || m_failed)
	{
		m_failed = true;

		return;
	}
	MYDEBUG("[ %d Field Init ] \n", m_fieldField->GetFieldNum());
	m_fieldMap.insert(make_pair(m_fieldField->GetFieldNum(), m_fieldField));
	m_fieldCount += 1;

	Field* m_testField;
	TRYCATCH_CONSTRUCTOR(m_testField = new Field(3, new VECTOR2(48, 48), "TestScene.fmap"), m_failed);
	if (m_testField->IsFailed() || m_failed)
	{
		m_failed = true;

		return;
	}
	MYDEBUG("[ %d Field Init ] \n", m_testField->GetFieldNum());
	m_fieldMap.insert(make_pair(m_testField->GetFieldNum(), m_testField));
	m_fieldCount += 1;
}


FieldManager::~FieldManager()
{
	map<WORD, Field*>::iterator i;
	for (i = m_fieldMap.begin(); i != m_fieldMap.end(); i++)
	{
		delete i->second;
	}
}

void FieldManager::InitMonsterThread()
{
	for (const auto& element : m_fieldMap)
	{
		element.second->InitMonsterThread();
	}
}

Field* FieldManager::GetField(int _num)
{
	map<WORD, Field*>::iterator iter = m_fieldMap.find(_num);
	if (iter == m_fieldMap.end()) return nullptr;

	return iter->second;
}