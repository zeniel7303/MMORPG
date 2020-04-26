#pragma once
#include "UserInfo.h"
#include "UnitInfo.h"
#include "CommandList.h"

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

struct IsConnectedPacket : Packet
{
	bool isConnected;
};

//Zone2Login
//Zone2Client/C

//struct C2ZS_SessionInfoPacket : Packet
//{
//	BasicInfo info;
//};

struct SessionInfoPacket : Packet
{
	BasicInfo info;
};

struct EnterFieldPacket : Packet
{
	unsigned short fieldNum;
	VECTOR2 position;
};

struct FieldNumPacket : Packet
{
	WORD fieldNum;
};

struct UserListPacket : Packet
{
	WORD userNum;

	BasicInfo info[100];
};

struct UserPositionPacket : Packet
{
	int userIndex;
	Position position;
};

struct UserNumPacket : Packet
{
	int userIndex;
};

struct UserAttackPacket : Packet
{
	int userIndex;
	int monsterIndex;
	VECTOR2 position;
};

struct UserHitPacket : Packet
{
	int userIndex;
	int monsterIndex;
	int damage;
};

struct UserExpPacket : Packet
{
	int exp;
};

struct MonsterInfoPacket : Packet
{
	MonsterInfo info;
};

struct MonsterInfoListPacket : Packet
{
	WORD monsterNum;

	MonsterInfo info[100];
};

struct MonsterPositionPacket : Packet
{
	int monsterIndex;
	VECTOR2 position;
};

struct MonsterAttackPacket : Packet
{
	int monsterIndex;
	int damage;
};

struct MonsterHitPacket : Packet
{
	int monsterIndex;
	int userIndex;
	int damage;
};

struct ChattingPacket : Packet
{
	int userIndex;
	char id[15];
	char chattingBuffer[30];
};

struct RegisterUserPacket : Packet
{
	char id[15];
	char password[15];
};

struct LogInPacket : Packet
{
	char id[15];
	char password[15];
};

#pragma pack(pop)