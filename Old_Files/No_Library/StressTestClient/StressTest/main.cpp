#include "Client.h"
#include "ThreadClass.h"

#include <winsock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define NUM 500

class App : public ThreadClass<App>
{
private:
	WSADATA WSAData;

	Client client[NUM];

public:
	void Init()
	{
		if (WSAStartup(MAKEWORD(2, 0), &WSAData) != 0)
		{
			printf("Error - Can not load 'winsock.dll' file\n");
		}

		for (int i = 0; i < NUM; i++)
		{
			client[i].Init(30002);

			if (client[i].Connect(i))
			{
				printf("[ %d번 클라 연결 ]\n", i + 1);
			}

			Sleep(50);
		}

		ThreadClass::Start(this);
	}

	void LoopRun()
	{
		for (int i = 0; i < NUM; i++)
		{
			client[i].Send((char*)client[i].GetPacket1(), client[i].GetPacket1()->size);
		}

		/*for (int i = 0; i < NUM / 2; i++)
		{
			client[i].Send((char*)client[i].GetPacket1(), client[i].GetPacket1()->size);
		}

		for (int i = NUM / 2; i < NUM; i++)
		{
			client[i].Send((char*)client[i].GetPacket2(), client[i].GetPacket2()->size);
		}*/

		Sleep(5000);

		while (1)
		{
			for (int i = 0; i < NUM; i++)
			{
				client[i].Task(i);

				//client[i].Send((char*)client[i].GetPacket(), client[i].GetPacket()->size);
			}

			Sleep(200/30);
		}
	}
};

App app;

void main()
{
	app.Init();

	WaitForSingleObject(app.GetHandle(), INFINITE);

	/*
	for (int i = 0; i < NUM; i++)
	{
		client[i].Init(30002);

		if (client[i].Connect(i))
		{
			printf("[ %d번 클라 연결 ]\n", i + 1);
		}

		client[i].Start(&client[i]);

		Sleep(100);
	}

	//while (1)
	{
		for (int i = 0; i < NUM; i++)
		{
			//if (client[i].GetIsConnected())
			{
				//client[i].Start(&client[i]);

				//client[i].Send((char*)client[i].GetPacket(), client[i].GetPacket()->size);
				//printf("%d \n", i);
			}

			//Sleep(50);
		}
	}

	while (1)
	{

	}
	*/
}