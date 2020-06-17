#include "Unit.h"

Unit::Unit()
{
	
}

Unit::~Unit()
{
	
}

void Unit::SetUnitInfo(
	STATE _state,
	int _level,
	int _maxHp,
	int _currentHp,
	int _maxMp,
	int _currentMp,
	int _maxExp,
	int _currentExp,
	int _atk,
	int _def)
{
	m_unitInfo.state = _state;
	m_unitInfo.level = _level;
	m_unitInfo.hp.maxValue = _maxHp;
	m_unitInfo.hp.currentValue = _currentHp;
	m_unitInfo.mp.maxValue = _maxMp;
	m_unitInfo.mp.currentValue = _currentMp;
	m_unitInfo.exp.maxValue = _maxExp;
	m_unitInfo.exp.currentValue = _currentExp;
	m_unitInfo.atk = _atk;
	m_unitInfo.def = _def;
}

void Unit::SetUnitPosition(
	float _x,
	float _z)
{
	m_unitInfo.position.x = _x;
	m_unitInfo.position.y = _z;
}