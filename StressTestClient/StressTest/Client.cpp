#include "Client.h"

Client::Client()
{

}

Client::~Client()
{
	
}

void Client::Init(int _port)
{
	listenSocket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listenSocket == INVALID_SOCKET)
	{
		printf("Error - Invalid socket\n");
	}

	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(_port);
	serverAddr.sin_addr.S_un.S_addr = inet_addr("192.168.0.13");
	//inet_pton(AF_INET, "192.168.0.13", &serverAddr.sin_addr);
	//inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr); //로컬

	isConnected = false;

	currentTime = 0.0f;
	intervalTime = 0.0f;
}

bool Client::Connect(int _num)
{
	if (connect(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		printf("Error - Fail to connect\n");
		// 4. 소켓종료
		closesocket(listenSocket);
		// Winsock End
		WSACleanup();

		return false;
	}
	else
	{
		packet = new TestPacket();
		packet->size = sizeof(TestPacket);
		packet->cmd = static_cast<USHORT>(SendCommand::TEST2);

		chattingPacket = new ChattingPacket();
		chattingPacket->size = sizeof(ChattingPacket);
		chattingPacket->cmd = 1001u;

		itoa(_num, chattingPacket->id, 10);
		strcat(chattingPacket->id, " Test\0");
		chattingPacket->userIndex += _num;
		strcpy(chattingPacket->chattingBuffer, "TestChatting\0");

		zoneNumPacket_1 = new ZoneNumPacket();
		zoneNumPacket_1->size = sizeof(ZoneNumPacket);
		zoneNumPacket_1->cmd = 1234;
		zoneNumPacket_1->zoneNum = 1;

		zoneNumPacket_2 = new ZoneNumPacket();
		zoneNumPacket_2->size = sizeof(ZoneNumPacket);
		zoneNumPacket_2->cmd = 1234;
		zoneNumPacket_2->zoneNum = 2;

		isConnected = true;

		//logInPacket = new LogInPacket();
		//logInPacket->size = sizeof(LogInPacket);
		//logInPacket->cmd = 7001u;
		//strcpy(logInPacket->id, "zeniel");
		//strcpy(logInPacket->password, "1029");

		////Recv(buffer, 1000);
		//
		//Sleep(100);

		//Send((char*)chattingPacket, chattingPacket->size);

		return true;
	}
}

void Client::Send(char* _data, DWORD _bytes)
{
	send(listenSocket, _data, _bytes, 0);
}

void Client::Recv(char* _data, DWORD _bytes)
{
	recv(listenSocket, _data, _bytes, 0);
}

void Client::Task(int _num)
{
	currentTime += 1.0f / 30.0f;

	if (intervalTime == 0.0f)
	{
		intervalTime = (float)((rand() % 5) + 1);
	}

	if (currentTime < 1.0f) return;
	//if (currentTime < intervalTime) return;

	//send(listenSocket, (char*)chattingPacket, chattingPacket->size, 0);
	send(listenSocket, (char*)packet, packet->size, 0);
	printf("%d \n", _num);

	currentTime = 0.0f;
	intervalTime = 0.0f;
}

//서버 DB - MYSQL
// - 유저 기준 -
//ID, PASSWORD
//================================================
// - 캐릭터 기준 -
//스테이터스
//정보(끈 위치 저장, 장착 상태)
//퀘스트 저장 상태

//ODBC ???? 뭘까 이게
//이거 말고
//oleDB써라