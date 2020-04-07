#pragma once
#include <stdio.h>
#include <time.h>

#pragma warning(disable:4996)

#include "ThreadClass.h"
#include "RingBuffer.h"
#include "CircBuf.h"

using namespace std;

enum class SendCommand : WORD
{
	TEST = 1234
};

#pragma pack(push, 1)
struct Packet
{
	WORD size;
	WORD cmd;
};

struct ChattingPacket : Packet
{
	int userIndex = 10000;

	char id[15];
	char chattingBuffer[30];
};

struct LogInPacket : Packet
{
	char id[15];
	char password[15];
};

struct ZoneNumPacket : Packet
{
	WORD zoneNum;
};

#pragma pack(pop)

class Client
{
private:
	SOCKET listenSocket;
	SOCKADDR_IN serverAddr;

	//=============================================

	char buffer[1000];

	Packet* packet;
	ChattingPacket* chattingPacket;
	LogInPacket* logInPacket;
	ZoneNumPacket* zoneNumPacket_1;
	ZoneNumPacket* zoneNumPacket_2;

	bool isConnected;

	float currentTime;
	float intervalTime;

public:
	Client();
	~Client();

	void Init(int _port);
	bool Connect(int _num);
	void Send(char* _data, DWORD _bytes);
	void Recv(char* _data, DWORD _bytes);
	void Task(int _num);

	bool GetIsConnected() { return isConnected; }
	ZoneNumPacket* GetPacket1() { return zoneNumPacket_1; }
	ZoneNumPacket* GetPacket2() { return zoneNumPacket_2; }
	char* GetBuffer() { return buffer; }
};