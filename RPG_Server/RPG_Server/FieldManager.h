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

public:
	FieldManager();
	~FieldManager();

	void Init();

	Field* GetField(int _num);
};