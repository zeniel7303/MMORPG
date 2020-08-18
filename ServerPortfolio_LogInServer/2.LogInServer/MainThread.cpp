#include "MainThread.h"

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

	processFunc[0] = &MainThread::ProcessPacket;
	processFunc[1] = &MainThread::ConnectUser;
	processFunc[2] = &MainThread::DisConnectUser;
	processFunc[3] = &MainThread::ProcessDBConnectorPacket;

	return true;
}

void MainThread::SetManagers(LogInSessionManager* _logInSessionManager)
{
	m_logInSessionManager = _logInSessionManager;

	m_packetHandler = new PacketHandler(*_logInSessionManager);

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

void MainThread::ProcessPacket()
{
	m_recvQueue.Swap();

	std::queue<PacketQueuePair>& recvQueue = m_recvQueue.GetSecondaryQueue();

	size_t size = recvQueue.size();

	for (int i = 0; i < size; i++)
	{
		const PacketQueuePair& packetQueuePair = recvQueue.front();
		Packet* packet = packetQueuePair.packet;
		LogInSession* session = packetQueuePair.session;

		//아이템같은거 줍고 끊기면?
		//m_isActived true인지 false인지만 체크하고 Log에 남기기.
		if (!session->IsConnected())
		{
			//MYDEBUG("Check - DisConnected\n");
			LOG::FileLog("../LogFile.txt", __FILENAME__, __LINE__,
				"%d user disconnected but packet is existed");

			continue;
		}

		m_packetHandler->HandlePacket(session, packet);

		recvQueue.pop();
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

		m_logInSessionManager->AddSession(tempSession);

		MYDEBUG("SessionList Size : %zd \n", m_logInSessionManager->GetSessionList()->GetItemList().size());

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

		//세션매니저에서 유저를 삭제해줌과 동시에 오브젝트풀에 반환해준다.
		m_logInSessionManager->ReturnSession(tempSession);
		MYDEBUG("SessionList Size : %zd \n", m_logInSessionManager->GetSessionList()->GetItemList().size());

		disconnectQueue.pop();
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

void MainThread::AddToUserPacketQueue(const PacketQueuePair& _packetQueuePair)
{
	m_recvQueue.AddObject(_packetQueuePair);

	SetEvent(m_hEvent[EVENT_RECV]);
}

void MainThread::AddToConnectQueue(SOCKET _socket)
{
	m_connectQueue.AddObject(_socket);

	SetEvent(m_hEvent[EVENT_CONNECT]);
}

void MainThread::AddToDisConnectQueue(LogInSession* _session)
{
	m_disconnectQueue.AddObject(_session);

	SetEvent(m_hEvent[EVENT_DISCONNECT]);
}

void MainThread::AddToDBConnectorPacketQueue(Packet* _packet)
{
	m_dbPacketQueue.AddObject(_packet);

	SetEvent(m_hEvent[EVENT_DBCONNECTOR]);
}