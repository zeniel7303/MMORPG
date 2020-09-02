#include "App.h"

App::App()
{
}


App::~App()
{
}

void App::Init(int _num)
{
	// Winsock Start - winsock.dll ·Îµå
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
	{
		printf("Error - Can not load 'winsock.dll' file\n");
	}

	iocp.Init();

	villageTilesData.GetMap("Village.fmap");
	fieldTilesData.GetMap("Field.fmap");
	testFieldTilesData.GetMap("TestScene.fmap");

	Dummy* dummy = nullptr;
	for (int i = 0; i < _num; i++)
	{
		dummy = new Dummy(i, 1, &testFieldTilesData);

		dummyVec.push_back(dummy);
	}
}

void App::Begin(IpEndPoint& _ipEndPoint)
{
	Dummy* dummy = nullptr;

	for (int i = 0; i < dummyVec.size(); i++)
	{
		dummy = dummyVec[i];
		dummy->Connect(_ipEndPoint);
		iocp.AddSocket(dummy->GetSocket(), (unsigned long long)dummy);

		Sleep(50);

		dummy->OnConnect(dummy->GetSocket());
	}

	//Sleep(3000);

	ThreadClass<App>::Start(this);
}

void App::LoopRun()
{
	Dummy* dummy = nullptr;

	while (1)
	{
		for (const auto& element : dummyVec)
		{
			dummy = element;

			dummy->RandomTask();
		}

		Sleep(200);
	}
}