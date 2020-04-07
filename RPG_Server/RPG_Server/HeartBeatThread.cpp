#include "HeartBeatThread.h"



HeartBeatThread::HeartBeatThread()
{
}


HeartBeatThread::~HeartBeatThread()
{
}

void HeartBeatThread::Init(SessionManager* _sessionManager)
{
	m_sessionManager = _sessionManager;

	ThreadClass<HeartBeatThread>::Start(this);

	printf("[ HeartBeatThread Init Success ]\n");
}
	 
void HeartBeatThread::LoopRun()
{
	Packet* checkAlivePacket = new Packet();
	checkAlivePacket->Init(SendCommand::CHECK_ALIVE, sizeof(Packet));

	while (1)
	{
		HeartBeat(checkAlivePacket);

		Sleep(5000);
	}
}

void HeartBeatThread::HeartBeat(Packet* _packet)
{
	list<Session*>* vSessionList = m_sessionManager->GetItemList();
	list<Session*>::iterator iterEnd = vSessionList->end();

	User* user;

	for (list<Session*>::iterator iter = vSessionList->begin(); iter != iterEnd;)
	{
		user = reinterpret_cast<User*>(*iter);
		++iter;

		if (!user->GetIsChecking() && user->GetStartCheckingHeartBeat())
		{
			user->SetIsChecking(true);
			user->Send(reinterpret_cast<char*>(_packet), _packet->size);
		}
		else if(user->GetIsChecking())
		{
			user->SetIsConnected(false);

			printf("[ HeartBeat Checking Failed ]\n");
		}
	}
}