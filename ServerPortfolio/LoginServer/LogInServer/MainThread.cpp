#include "MainThread.h"

#include "HeartBeatThread.h"

#include "ZoneConnector.h"
#include "ZoneServerManager.h"

MainThread::MainThread()
{
}

MainThread::~MainThread()
{
	for (int i = MAX_EVENT; i > 0; --i)
	{
		if (m_hEvent[i - 1]) { CloseHandle(m_hEvent[i - 1]); m_hEvent[i - 1] = 0; }
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

	m_num = 0;

	processFunc[0] = &MainThread::ConnectUser;
	processFunc[1] = &MainThread::DisConnectUser;
	processFunc[2] = &MainThread::AddToHashMap;
	processFunc[3] = &MainThread::ProcessDBConnectorPacket;
	processFunc[4] = &MainThread::ProcessZoneServerPacket;
	processFunc[5] = &MainThread::ProcessHeartBeat;

	return true;
}

void MainThread::SetManagers(LogInSessionManager* _logInSessionManager, HeartBeatThread* _heartBeatThread)
{
	m_logInSessionManager = _logInSessionManager;
	m_heartBeatThread = _heartBeatThread;

	m_zoneServerManager = new ZoneServerManager();

	m_packetHandler = new PacketHandler(*_logInSessionManager, *m_zoneServerManager);

	Thread<MainThread>::Start(this);

	MYDEBUG("[ MainThread Start ]\n");
}

void MainThread::LoopRun()
{
	while (1)
	{
		int num = WaitForMultipleObjects(MAX_EVENT, m_hEvent, FALSE, INFINITE);
		//printf("hEvent : %d \n", num);

		(this->*processFunc[num])();
	}
}

void MainThread::ConnectUser()
{
	//유저가 Connect된 경우
	m_connectQueue.Swap();

	std::queue<SOCKET>& connectQueue = m_connectQueue.GetSecondaryQueue();

	size_t size = connectQueue.size();

	for (int i = 0; i < size; i++)
	{
		LogInSession* tempSession = m_logInSessionManager->PopSession();

		if (tempSession == nullptr)
		{
			//예외처리
			MYDEBUG("[ Session is nullptr ] \n");
			FILELOG("Session is nullptr");

			// + LOG 남기기
			//접속 정원됐으니까 패킷 하나 보내주자. 혹은 시간 지나면 ?
			continue;
		}

		tempSession->SetSocket(connectQueue.front());

		tempSession->OnConnect();

		m_logInSessionManager->AddTempSession(tempSession);

		//MYDEBUG("tempSessionList Size : %zd \n", m_logInSessionManager->GetSessionList()->GetItemList().size());

		connectQueue.pop();
	}
}

void MainThread::DisConnectUser()
{
	//유저가 Disconnect된 경우
	m_disconnectQueue.Swap();

	std::queue<LogInSession*>& disconnectQueue = m_disconnectQueue.GetSecondaryQueue();

	size_t size = disconnectQueue.size();

	for (int i = 0; i < size; i++)
	{
		LogInSession* tempSession = disconnectQueue.front();

		if (tempSession == nullptr)
		{
			disconnectQueue.pop();

			continue;
		}

		if (tempSession->m_isInHashMap)
		{
			//세션매니저에서 유저를 삭제해줌과 동시에 오브젝트풀에 반환해준다.
			m_logInSessionManager->ReturnSession(tempSession);
			//MYDEBUG("sessionHashmapSize : %zd \n", m_logInSessionManager->GetSessionHashMap()->GetItemHashMap().size());
		}
		else
		{
			m_logInSessionManager->DeleteTempSession(tempSession, true);
			//MYDEBUG("tempSessionList Size : %zd \n", m_logInSessionManager->GetSessionList()->GetItemList().size());
		}

		disconnectQueue.pop();
	}
}

void MainThread::AddToHashMap()
{
	m_hashMapQueue.Swap();

	std::queue<LogInSession*>& hashMapQueue = m_hashMapQueue.GetSecondaryQueue();

	size_t size = hashMapQueue.size();

	for (int i = 0; i < size; i++)
	{
		LogInSession* tempSession = hashMapQueue.front();

		m_logInSessionManager->DeleteTempSession(tempSession, false);
		m_logInSessionManager->AddSession(tempSession);
		tempSession->m_isInHashMap = true;

		//MYDEBUG("tempSessionList Size : %zd \n", m_logInSessionManager->GetSessionList()->GetItemList().size());
		MYDEBUG("sessionHashmapSize : %zd \n", m_logInSessionManager->GetSessionHashMap()->GetItemHashMap().size());

		hashMapQueue.pop();
	}
}

void MainThread::ProcessDBConnectorPacket()
{
	m_dbPacketQueue.Swap();

	std::queue<Packet*>& dbPacketQueue = m_dbPacketQueue.GetSecondaryQueue();

	size_t size = dbPacketQueue.size();

	for (int i = 0; i < size; i++)
	{
		Packet* packet = dbPacketQueue.front();

		m_packetHandler->HandleDBConnectorPacket(packet);

		dbPacketQueue.pop();
	}
}

void MainThread::ProcessZoneServerPacket()
{
	m_zoneServerPacketQueue.Swap();

	std::queue<PacketQueuePair_ZoneNum>& zoneServerPacketQueue = m_zoneServerPacketQueue.GetSecondaryQueue();

	size_t size = zoneServerPacketQueue.size();

	for (int i = 0; i < size; i++)
	{
		const PacketQueuePair_ZoneNum& packetQueue = zoneServerPacketQueue.front();
		Packet* packet = packetQueue.packet;
		int zoneNum = packetQueue.m_zoneNum;

		m_packetHandler->HandleZoneServerPacket(zoneNum, packet);

		zoneServerPacketQueue.pop();
	}
}

void MainThread::ProcessHeartBeat()
{
	m_heartBeatThread->HeartBeat();
}

void MainThread::ConnectWithZoneServer(SOCKET _socket)
{
	ZoneConnector* zoneConnector = new ZoneConnector();

	zoneConnector->SetSocket(_socket);
	zoneConnector->SetZoneNum(m_num);

	zoneConnector->OnConnect();

	m_zoneServerManager->GetZoneConnectorVec()->emplace_back(zoneConnector);

	MYDEBUG("[ %d번 Zone 연결됨 ]\n", 
		zoneConnector->GetZoneNum());

	m_num += 1;
}

void MainThread::AddToConnectQueue(SOCKET _socket)
{
	m_connectQueue.GetPrimaryQueue().emplace(_socket);

	SetEvent(m_hEvent[EVENT_CONNECT]);
}

void MainThread::AddToDisConnectQueue(LogInSession* _session)
{
	m_disconnectQueue.GetPrimaryQueue().emplace(_session);

	SetEvent(m_hEvent[EVENT_DISCONNECT]);
}

void MainThread::AddToHashMapQueue(LogInSession* _session)
{
	m_hashMapQueue.GetPrimaryQueue().emplace(_session);

	SetEvent(m_hEvent[EVENT_STOREUSER]);
}

void MainThread::AddToDBConnectorPacketQueue(Packet* _packet)
{
	m_dbPacketQueue.GetPrimaryQueue().emplace(_packet);

	SetEvent(m_hEvent[EVENT_DBCONNECTOR]);
}

void MainThread::AddToZoneServerPacketQueue(const PacketQueuePair_ZoneNum& _packetQueue)
{
	m_zoneServerPacketQueue.AddObject(_packetQueue);

	SetEvent(m_hEvent[EVENT_ZONESERVER]);
}

void MainThread::HearBeatCheck()
{
	SetEvent(m_hEvent[EVENT_HEARTBEAT]);
}