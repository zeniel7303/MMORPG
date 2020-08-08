#pragma once
#include "CommandList.h"
#include "UnitInfo.h"
#include "UserInfo.h"

#pragma pack(push, 1)

struct BasicInfo
{
	UnitInfo unitInfo;
	UserInfo userInfo;
};

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

struct PacketWithSocket : Packet
{
	SOCKET socket;
};

struct LogInPacket_DBAgent : Packet
{
	char id[15];
	char password[15];
	SOCKET socket;
};

struct LogInSuccessPacket : Packet
{
	int userIndex;
	SOCKET socket;
};

struct RegisterPacket_DBAgent : Packet
{
	char id[15];
	char password[15];
	SOCKET socket;
};

struct RequireUserInfoPacket_DBAgent : Packet
{
	int userIndex;
	SOCKET socket;
};

struct GetSessionInfoPacket : Packet
{
	BasicInfo info;
	SOCKET socket;
};

struct MonstersInfoPacket : Packet
{
	WORD count;
	MonsterData monstersData[500];
};

struct UpdateUserPacket : Packet
{
	int userIndex;
	UnitInfo unitInfo;
	SOCKET socket;
};

#pragma pack(pop)