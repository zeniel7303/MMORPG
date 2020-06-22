#include "FieldManager.h"

FieldManager::FieldManager()
{
}


FieldManager::~FieldManager()
{
	map<WORD, Field*>::iterator i;
	for (i = m_fieldMap.begin(); i != m_fieldMap.end(); i++)
	{
		delete i->second;
	}
}

bool FieldManager::Init()
{
	m_fieldCount = 0;

	Field* m_villageField;
	try
	{
		m_villageField = new Field();
	}
	catch (const std::bad_alloc& error)
	{
		printf("bad alloc : %s\n", error.what());
		return false;
	}
	if (!m_villageField->Init(1, VECTOR2(17, 19)))
	{
		return false;
	}
	if (!m_villageField->GetMap("Village.fmap"))
	{
		return false;
	}
	m_fieldMap.insert(make_pair(m_villageField->GetFieldNum(), m_villageField));
	m_fieldCount += 1;

	Field* m_fieldField;
	try
	{
		m_fieldField = new Field();
	}
	catch (const std::bad_alloc& error)
	{
		printf("bad alloc : %s\n", error.what());
		return false;
	}
	if (!m_fieldField->Init(2, VECTOR2(77, 75)))
	{
		return false;
	}
	if (!m_fieldField->GetMap("Field.fmap"))
	{
		return false;
	}
	m_fieldMap.insert(make_pair(m_fieldField->GetFieldNum(), m_fieldField));
	m_fieldCount += 1;

	Field* m_testField;
	try
	{
		m_testField = new Field();
	}
	catch (const std::bad_alloc& error)
	{
		printf("bad alloc : %s\n", error.what());
		return false;
	}
	if (!m_testField->Init(999, VECTOR2(48, 48)))
	{
		return false;
	}
	if (!m_testField->GetMap("TestScene.fmap"))
	{
		return false;
	}
	m_fieldMap.insert(make_pair(m_testField->GetFieldNum(), m_testField));
	m_fieldCount += 1;

	return true;
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