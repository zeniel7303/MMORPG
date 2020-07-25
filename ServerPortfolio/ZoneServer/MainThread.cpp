#include "MainThread.h"

MainThread::MainThread()
{
}

MainThread::~MainThread()
{
	for (int i = MAX_EVENT; i > 0 ; --i)
	{
		if (m_hEvent[i-1]) {	CloseHandle(m_hEvent[i-1]); m_hEvent[i-1] = 0; 	}
	}
}

bool MainThread::Init()
{
	for (int i = 0; i < MAX_EVENT; ++i)
	{
		//실패확률?
		m_hEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);

		if (m_hEvent[i] == NULL)
		{
			int num = WSAGetLastError();

			MYDEBUG("[Event Handle Creating Fail - %d Error]\n", num);

			return false;
		}
	}

	return true;
}

void MainThread::SetManagers(SessionManager* _sessionManager,
	FieldManager* _fieldManager)
{
	m_sessionManager = _sessionManager;
	m_fieldManager = _fieldManager;

	m_packetHandler = new PacketHandler(*_sessionManager, *_fieldManager);

	Thread<MainThread>::Start(this);

	MYDEBUG("[ MainThread Start ]\n");
}

void MainThread::LoopRun()
{
	while(1)
	{
		int num = WaitForMultipleObjects(MAX_EVENT, m_hEvent, FALSE, INFINITE);
		//printf("hEvent : %d \n", num);

		switch (num)
		{
		case EVENT_RECV:
		{
			m_userPacketQueue.Swap();

			ProcessUserPacket();
		}	
			break;
		case EVENT_DBCONNECTOR:
		{
			m_dbPacketQueue.Swap();

			ProcessDBConnectorPacket();
		}
			break;
		case EVENT_MONSTER:
		{
			m_monsterPacketQueue.Swap();

			ProcessMonsterPacket();
		}
			break;
		case EVENT_CONNECT:
		{
			//유저가 Connect된 경우
			m_connectQueue.Swap();

			ConnectUser();
		}			
			break;
		case EVENT_DISCONNECT:
		{
			//유저가 Disconnect된 경우
			m_disconnectQueue.Swap();

			DisConnectUser();
		}	
			break;
		}
	}
}

void MainThread::ProcessUserPacket()
{
	std::queue<PacketQueuePair_User>& userPacketQueue = m_userPacketQueue.GetSecondaryQueue();

	size_t size = userPacketQueue.size();

	for (int i = 0; i < size; i++)
	{
		const PacketQueuePair_User& PacketQueuePair_User = userPacketQueue.front();
		Packet* packet = PacketQueuePair_User.packet;
		User* user = PacketQueuePair_User.user;

		//아이템같은거 줍고 끊기면?
		//m_isActived true인지 false인지만 체크하고 Log에 남기기.
		if (!user->IsConnected())
		{
			//MYDEBUG("Check - DisConnected\n");
			FILELOG("%d user disconnected but packet is existed", user->GetInfo()->userInfo.userID);

			continue;
		}

		//테스트용 패킷 처리
		if (packet->cmd == 12345)
		{
			MYDEBUG("Test Checking \n");
		}
		else
		{
			m_packetHandler->HandleUserPacket(user, packet);
		}

		userPacketQueue.pop();
	}
}

void MainThread::ProcessMonsterPacket()
{
	std::queue<PacketQueuePair_Monster>& monsterPacketQueue = m_monsterPacketQueue.GetSecondaryQueue();

	size_t size = monsterPacketQueue.size();

	for(int i = 0; i < size; i++)
	{
		const PacketQueuePair_Monster& PacketQueuePair_Monster = monsterPacketQueue.front();
		Packet* packet = PacketQueuePair_Monster.packet;
		Monster* monster = PacketQueuePair_Monster.monster;

		//해당 몬스터가 죽어있다면 패스
		if (monster->IsDeath()) continue;

		m_packetHandler->HandleMonsterPacket(monster, packet);

		monsterPacketQueue.pop();
	}
}

void MainThread::ProcessDBConnectorPacket()
{
	//if (!Connector->GetIsConnected())
	//{
	//	//재접속
	//	Connector->Connect();
	//}

	std::queue<Packet*>& dbPacketQueue = m_dbPacketQueue.GetSecondaryQueue();

	size_t size = dbPacketQueue.size();

	for (int i = 0; i < size; i++)
	{
		Packet* packet = dbPacketQueue.front();

		m_packetHandler->HandleDBConnectorPacket(packet);

		dbPacketQueue.pop();
	}
}

void MainThread::ConnectUser()
{
	std::queue<SOCKET>& connectQueue = m_connectQueue.GetSecondaryQueue();

	size_t size = connectQueue.size();

	for (int i = 0; i < size; i++)
	{
		User* tempUser = dynamic_cast<User*>(m_sessionManager->PopSession());

		if (tempUser == nullptr)
		{
			//예외처리
			MYDEBUG("[ UserSession is nullptr ] \n");
			FILELOG("UserSession is nullptr");

			// + LOG 남기기
			//접속 정원됐으니까 패킷 하나 보내주자. 혹은 시간 지나면 ?
			continue;
		}

		tempUser->SetSocket(connectQueue.front());

		m_sessionManager->AddSessionList(tempUser);
		tempUser->OnConnect();

		connectQueue.pop();
	}

	/*int size = m_connectQueue.GetSecondaryQueueSize();

	for (int i = 0; i < size; i++)
	{
		User* tempUser = dynamic_cast<User*>(m_sessionManager->PopSession());

		if (tempUser == nullptr)
		{
			//예외처리
			MYDEBUG("[ UserSession is nullptr ] \n");
			FILELOG("UserSession is nullptr");

			// + LOG 남기기
			//접속 정원됐으니까 패킷 하나 보내주자. 혹은 시간 지나면 ?
			continue;
		}

		tempUser->SetSocket(m_connectQueue.PopObject());

		m_sessionManager->AddSessionList(tempUser);
		tempUser->OnConnect();
	}*/
}

void MainThread::DisConnectUser()
{
	std::queue<Session*>& disconnectQueue = m_disconnectQueue.GetSecondaryQueue();

	size_t size = disconnectQueue.size();

	for (int i = 0; i < size; i++)
	{
		User* tempUser = dynamic_cast<User*>(disconnectQueue.front());

		//존에 있었다면 해당 존에서 먼저 나간 후
		if (tempUser->GetField() != nullptr)
		{
			tempUser->GetField()->ExitUser(tempUser);
		}

		tempUser->Reset();

		//세션매니저에서 유저를 삭제해줌과 동시에 오브젝트풀에 반환해준다.
		m_sessionManager->ReturnSessionList(tempUser);
		m_sessionManager->DeleteSessionID(tempUser->GetInfo()->userInfo.userID);

		disconnectQueue.pop();
	}

	/*int size = m_disconnectQueue.GetSecondaryQueueSize();

	for (int i = 0; i < size; i++)
	{
		User* tempUser = dynamic_cast<User*>(m_disconnectQueue.PopObject());

		//존에 있었다면 해당 존에서 먼저 나간 후
		if (tempUser->GetField() != nullptr)
		{
			tempUser->GetField()->ExitUser(tempUser);
		}

		tempUser->Reset();

		//세션매니저에서 유저를 삭제해줌과 동시에 오브젝트풀에 반환해준다.
		m_sessionManager->ReturnSessionList(tempUser);
		m_sessionManager->DeleteSessionID(tempUser->GetInfo()->userInfo.userID);
	}*/
}

void MainThread::AddToUserPacketQueue(const PacketQueuePair_User& _userPacketQueuePair)
{
	m_userPacketQueue.AddObject(_userPacketQueuePair);

	SetEvent(m_hEvent[EVENT_RECV]);
}

void MainThread::AddToMonsterPacketQueue(const PacketQueuePair_Monster& _monsterPacketQueuePair)
{
	m_monsterPacketQueue.AddObject(_monsterPacketQueuePair);

	SetEvent(m_hEvent[EVENT_MONSTER]);
}

void MainThread::AddToDBConnectorPacketQueue(Packet* _packet)
{
	m_dbPacketQueue.AddObject(_packet);

	SetEvent(m_hEvent[EVENT_DBCONNECTOR]);
}

void MainThread::AddToConnectQueue(SOCKET _socket)
{
	m_connectQueue.AddObject(_socket);

	SetEvent(m_hEvent[EVENT_CONNECT]);
}

void MainThread::AddToDisConnectQueue(Session* _session)
{
	m_disconnectQueue.AddObject(_session);

	SetEvent(m_hEvent[EVENT_DISCONNECT]);
}