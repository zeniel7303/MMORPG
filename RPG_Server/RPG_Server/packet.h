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

struct Info_PacketUse
{
	UserInfo userInfo;
	Position position;
};

struct Info_PacketUser_Light
{
	int userID;
	Position position;
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
	Info_PacketUse info[500];
};

struct UserListPacket_Light : Packet
{
	WORD userNum;
	Info_PacketUser_Light info[500];
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

struct TestClientEnterPacket : Packet
{
	int userNum;
	int fieldNum;
};

struct TestClientStatePacket : Packet
{
	STATE state;
	VECTOR2 vec2;
};

struct TestClientMovePacket : Packet
{
	int userIndex;
	WORD tileCount;
	Position position[20];
};

#pragma pack(pop)