#pragma once
#include "CommandList.h"

#pragma pack(push, 1)

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

struct IsConnectedPacket : Packet
{
	bool isConnected;
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
	int zoneNum;
};

struct UserNumPacket : Packet
{
	int userIndex;
};

struct ZoneNumPacket : Packet
{
	int zoneNum;
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

struct ChangeZonePacket : Packet
{
	int zoneNum;
};

#pragma pack(pop)