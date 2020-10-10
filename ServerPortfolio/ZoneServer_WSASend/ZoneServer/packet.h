#pragma once
#include "CommandList.h"
#include "UserInfo.h"
#include "UnitInfo.h"

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

	Info_PacketUser_Light() 
	{
		userID = 0;
		position.x = 0.0f;
		position.y = 0.0f;
	}

	Info_PacketUser_Light(int _id, const Position& _posi)
	{
		userID = _id;
		position = _posi;
	}
};

struct Packet
{
	WORD size;
	WORD cmd;

	void Init(SendCommand _cmd, int _size)
	{
		size = _size;
		cmd = static_cast<USHORT>(_cmd);
	}
};

struct TestPacket : Packet
{
	int count;
	char test[1000];
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

struct ZoneNumPacket : Packet
{
	int zoneNum;
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

struct UserListPacket_Test : Packet
{
	WORD userNum;
	Info_PacketUser_Light* info;

	UserListPacket_Test(int _num)
	{
		info = new Info_PacketUser_Light[_num]();
	}

	~UserListPacket_Test()
	{
		printf("üũ \n");

		SAFE_DELETE_ARRAY(info);
	}
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

struct MonsterInfoListPacket_Test : Packet
{
	WORD monsterNum;

	MonsterInfo* info;

	MonsterInfoListPacket_Test(int _num)
	{
		info = new MonsterInfo[_num]();
	}

	~MonsterInfoListPacket_Test()
	{
		printf("üũ \n");

		SAFE_DELETE_ARRAY(info);
	}
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
	char chattingBuffer[255];
};

struct ChattingPacket_Whisper : Packet
{
	int userIndex;
	char targetId[15];
	char id[15];
	char chattingBuffer[255];
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

struct ChangeZonePacket : Packet
{
	int zoneNum;
	int userIndex;
};

//=================================================

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

struct AuthenticationPacket : Packet
{
	int userIndex;
	SOCKET socket;
};

struct AuthenticationFailedPacket : Packet
{
	int userIndex;
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

//==================================================

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