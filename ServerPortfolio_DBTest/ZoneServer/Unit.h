#pragma once
#include "UnitInfo.h"

class Unit
{
protected:
	UnitInfo		m_unitInfo;

public:
	Unit();
	virtual ~Unit();

	void SetUnitInfo(
		STATE _state,
		int _level,
		int _maxHp,
		int _currentHp,
		int _maxMp,
		int _currentMp,
		int _maxExp,
		int _currentExp,
		int _atk,
		int _def
		);

	void SetUnitPosition(
		float _x,
		float _z);

	UnitInfo GetUnitInfoPacket() { return m_unitInfo; }
};

