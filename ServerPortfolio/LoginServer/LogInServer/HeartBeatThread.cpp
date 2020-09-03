#include "HeartBeatThread.h"

HeartBeatThread::HeartBeatThread(LogInSessionManager& _sessionManager, int _cycle)
	: m_sessionManager(_sessionManager)
{
	m_cycle = _cycle * 1000;

	MYDEBUG("[ HeartBeat Init Success ]\n");

	Thread::Start(this);
}

HeartBeatThread::~HeartBeatThread()
{
}

void HeartBeatThread::LoopRun()
{
	while (1)
	{
		Sleep(m_cycle);

		MainThread::getSingleton()->HearBeatCheck();
	}
}

void HeartBeatThread::HeartBeat()
{
	const std::unordered_map<int, LogInSession*>& tempHashMap = m_sessionManager.GetSessionHashMap()->GetItemHashMap();

	LogInSession* session;

	m_end = std::chrono::high_resolution_clock::now();
	auto nowTime = std::chrono::high_resolution_clock::now();

	for (const auto& element : tempHashMap)
	{
		session = element.second;

		m_durationSec = std::chrono::duration_cast<std::chrono::seconds>(m_end - session->GetStartTime());

		if (m_durationSec.count() > 60/* && m_durationSec.count() < 10000.0f*/)
		{
			//연결 끊기
			session->DisConnect();

			MYDEBUG("[ HeartBeat Checking Failed ]\n");
		}
	}

	//for_each
	/*
	list<Session*>::const_iterator iterBegin = vSessionList.begin();
	list<Session*>::const_iterator iterEnd = vSessionList.end();

	for_each(iterBegin, iterEnd, [=](Session* _session) mutable
	{
		user = dynamic_cast<User*>(_session);

		//유저가 채킹중이 아니고, 유저가 채킹에 대한 준비가 되었는가(게임에 확실히 접속했는가)
		//입장에 성공했을 경우 채킹에 준비되었다고 처리했다.
		if (!user->IsChecking() && user->GetStartCheckingHeartBeat())
		{
			user->SetChecking(true);
			user->Send(reinterpret_cast<char*>(_packet), _packet->size);
		}
		//유저가 채킹중이다.(클라이언트 쪽에서 HeartBeat Checking 완료 패킷이 안왔다.)
		else if (user->IsChecking())
		{
			//연결 끊기
			user->SetConnected(false);

			MYDEBUG("[ HeartBeat Checking Failed ]\n");
		}
	});*/

	//일반적인 For문
	/*for (list<Session*>::iterator iter = vSessionList->begin(); iter != iterEnd;)
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
	}*/
}