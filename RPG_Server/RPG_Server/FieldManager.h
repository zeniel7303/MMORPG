#pragma once
#include "Field.h"

//=====================================================

//FieldManager

//Field들을 관리하기위한 매니저

//=====================================================

class FieldManager
{
private:
	map<WORD, Field*> m_fieldMap;
	int m_fieldCount;

public:
	FieldManager();
	~FieldManager();

	void Init();
	void InitMonsterThread();

	Field* GetField(int _num);

	int GetFieldCount() { return m_fieldCount; }
};