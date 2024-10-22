#include "App.h"

App::App()
{
}


App::~App()
{
}

void App::Init(int _num)
{
	int temp = _num * 480;

	// Winsock Start - winsock.dll �ε�
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
	for (int i = temp; i < temp + 480; i++)
	{
		dummy = new Dummy(i, 1, &testFieldTilesData);

		dummyVec.push_back(dummy);
	}
}

void App::Begin(IpEndPoint& _ipEndPoint)
{
	//ThreadClass<App>::Start(this);

	Dummy* dummy = nullptr;

	for (int i = 0; i < dummyVec.size(); i++)
	{
		dummy = dummyVec[i];
		dummy->Connect(_ipEndPoint);
		iocp.AddSocket(dummy->GetSocket(), (unsigned long long)dummy);

		Sleep(10);

		dummy->OnConnect(dummy->GetSocket());

		dummy->m_loginDummy->SetDummyNum(dummy->GetDummyNum());
		dummy->m_loginDummy->Connect();
		Sleep(10);
		dummy->m_loginDummy->OnConnect(dummy->m_loginDummy->GetSocket());
	}

	ThreadClass<App>::Start(this);

	//Sleep(3000);
}

void App::LoopRun()
{
	Dummy* dummy = nullptr;

	while (1)
	{
		for (const auto& element : dummyVec)
		{
			dummy = element;

			dummy->FSM();
			//dummy->Test();
		}

		Sleep(1000 / 10);
		//Sleep(1000);
	}
}