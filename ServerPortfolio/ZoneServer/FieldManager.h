#pragma once
#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/Utils.h"

#include "Field.h"

//=====================================================

//FieldManager

//Field들을 관리하기위한 매니저

//=====================================================

class FieldManager
{
private:
	bool				m_failed;

	map<WORD, Field*>	m_fieldMap;
	int					m_fieldCount;

public:
	FieldManager();
	~FieldManager();

	void InitMonsterThread();

	bool IsFailed() { return m_failed; }

	Field* GetField(int _num);

	int GetFieldCount() { return m_fieldCount; }
};