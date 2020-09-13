#include "App.h"

App::App(int _num)
{
	int temp = _num * 100;

	iocp = new IOCPClass();

	villageTilesData.GetMap("Village.fmap");
	fieldTilesData.GetMap("Field.fmap");
	testFieldTilesData.GetMap("TestScene.fmap");

	Dummy* dummy = nullptr;
	for (int i = temp; i < temp + 100; i++)
	{
		dummy = new Dummy(i, 0, iocp);
		dummy->SetTilesData(&testFieldTilesData);

		dummyVec.push_back(dummy);
	}
}

App::~App()
{
}

void App::Begin()
{
	Dummy* dummy = nullptr;

	for (int i = 0; i < dummyVec.size(); i++)
	{
		dummy = dummyVec[i];
		dummy->m_loginSession->Connect();
		Sleep(10);
		dummy->m_loginSession->OnConnect();
	
		Sleep(100);
	}

	Thread<App>::Start(this);
}

void App::LoopRun()
{
	Dummy* dummy = nullptr;

	while (1)
	{
		for (const auto& element : dummyVec)
		{
			dummy = element;
			if (!dummy->m_zoneSession->isReady) continue;

			dummy->FSM();
		}

		Sleep(1000 / 10);
	}
}