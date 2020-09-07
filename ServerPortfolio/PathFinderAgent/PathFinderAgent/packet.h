#pragma once
#include "stdafx.h"
#include "CommandList.h"

#pragma pack(push, 1)

struct Packet
{
	Packet()
	{
		memset(this, 0, sizeof(Packet));
	}

	WORD size;
	WORD cmd;

	void Init(SendCommand _cmd, int _size)
	{
		size = _size;
		cmd = static_cast<USHORT>(_cmd);
	}
};

struct PathFindPacket : Packet
{
	int monsterNum;
	int fieldNum;
	unsigned short state;

	VECTOR2 nowPosition;
	VECTOR2 targetPosition;
};

struct PathFindPacket_Success : Packet
{
	int monsterNum;
	int fieldNum;
	unsigned short state;

	int listCount;
	VECTOR2 positionList[100];
};

struct PathFindPacket_Failed : Packet
{
	int monsterNum;
	int fieldNum;
};

#pragma pack(pop)