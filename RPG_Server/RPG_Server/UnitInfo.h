#pragma once
#include "stdafx.h"

#pragma pack(push, 1)

enum STATE : unsigned short
{
	SPAWN,
	IDLE,
	MOVE,
	ATTACK,
	HIT,
	DEATH,
	//몬스터용
	PATROL,
	//몬스터용
	RETURN
};

struct ZeroToMax
{
	int currentValue;
	int maxValue;

	ZeroToMax(int _current = 0, int _max = 0)
	{
		currentValue = _current;
		maxValue = _max;
	}
};

struct Position : VECTOR2
{
	VECTOR2 direction;

	void Init(VECTOR2& _vector)
	{
		direction = _vector;
	}
};

struct UnitInfo
{
	WORD fieldNum;

	int level;

	ZeroToMax hp;
	ZeroToMax mp;
	ZeroToMax exp;

	int atk;
	int def;

	Position position;

	STATE state = SPAWN;
	
	void SetState(STATE _state)
	{
		state = _state;
	}

	void SetHp(int _current, int _max)
	{
		hp.currentValue = _current;
		hp.maxValue = _max;
	}

	void SetMp(int _current, int _max)
	{
		mp.currentValue = _current;
		mp.maxValue = _max;
	}

	void SetExp(int _current, int _max)
	{
		exp.currentValue = _current;
		exp.maxValue = _max;
	}
};

struct MonsterInfo
{
	int index;
	WORD monsterType;

	STATE state;
	ZeroToMax hp;
	VECTOR2 position;
};

struct MonsterData
{
	WORD monsterType;

	ZeroToMax hp;

	float attackDelay; //공격 딜레이
	int attackDamage; //공격 데미지
	float attackDistance; //공격 사거리

	float returnDistance; //귀환 거리

	float moveSpeed; //움직임 속도
	int patrolRange; //정찰 범위
	float patrolDelay; //정찰 딜레이

	int dropExp;
};

#pragma pack(pop)