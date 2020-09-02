#include "MainThread.h"

MainThread::MainThread()
{
}

MainThread::~MainThread()
{
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

	TRYCATCH(m_pathFinderManager = new PathFinderManager());

	processFunc[0] = &MainThread::ConnectUser;
	processFunc[1] = &MainThread::DisConnectUser;
	processFunc[2] = &MainThread::ProcessRecv;

	return true;
}

void MainThread::SetManagers(PathFinderAgentManager* _pathFinderAgentManager,
	FieldManager* _fieldManager)
{
	m_pathFinderAgentManager = _pathFinderAgentManager;
	m_fieldManager = _fieldManager;

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
	m_connectQueue.Swap();

	std::queue<SOCKET>& connectQueue = m_connectQueue.GetSecondaryQueue();

	size_t size = connectQueue.size();

	for (int i = 0; i < size; i++)
	{
		PathFinderAgent* tempAgent = m_pathFinderAgentManager->PopAgent();

		if (tempAgent == nullptr)
		{
			//예외처리
			MYDEBUG("[ agent is nullptr ] \n");
			FILELOG("agent is nullptr");

			// + LOG 남기기
			//접속 정원됐으니까 패킷 하나 보내주자. 혹은 시간 지나면 ?
			continue;
		}

		tempAgent->SetSocket(connectQueue.front());

		tempAgent->OnConnect();

		m_pathFinderAgentManager->AddAgent(tempAgent);
		MYDEBUG("AgentList Size : %zd \n", m_pathFinderAgentManager->GetAgentList()->GetItemList().size());

		connectQueue.pop();
	}
}

void MainThread::DisConnectUser()
{
	m_disconnectQueue.Swap();

	std::queue<PathFinderAgent*>& disconnectQueue = m_disconnectQueue.GetSecondaryQueue();

	size_t size = disconnectQueue.size();

	for (int i = 0; i < size; i++)
	{
		PathFinderAgent* tempAgent = disconnectQueue.front();

		//세션매니저에서 유저를 삭제해줌과 동시에 오브젝트풀에 반환해준다.
		m_pathFinderAgentManager->ReturnAgent(tempAgent);
		MYDEBUG("SessionList Size : %zd \n", m_pathFinderAgentManager->GetAgentList()->GetItemList().size());

		disconnectQueue.pop();
	}
}

void MainThread::ProcessRecv()
{
	m_recvQueue.Swap();

	std::queue<PacketQueuePair>& recvQueue = m_recvQueue.GetSecondaryQueue();
	//MYDEBUG("primary size : %d \n", m_recvQueue.GetPrimaryQueueSize());
	//MYDEBUG("secondary size : %d \n", m_recvQueue.GetSecondaryQueueSize());

	size_t size = recvQueue.size();

	for (int i = 0; i < size; i++)
	{
		const PacketQueuePair& packetQueuePair = recvQueue.front();
		Packet* packet = packetQueuePair.packet;
		PathFinderAgent* agent = packetQueuePair.agent;

		for (const auto& element : m_pathFinderManager->GetPathFinderList())
		{
			if (element->GetState() == STATE::READY)
			{
				//element->m_packet = nullptr;
				//element->m_pathFinderAgent = nullptr;

				element->m_packet = packet;
				element->m_pathFinderAgent = agent;
				SetEvent(element->m_hEvent);

				recvQueue.pop();

				break;
			}
		}
	}
}

void MainThread::AddToConnectQueue(SOCKET _socket)
{
	m_connectQueue.AddObject(_socket);

	SetEvent(m_hEvent[EVENT_CONNECT]);
}

void MainThread::AddToDisConnectQueue(PathFinderAgent* _agent)
{
	m_disconnectQueue.AddObject(_agent);

	SetEvent(m_hEvent[EVENT_DISCONNECT]);
}

void MainThread::AddToRecvQueue(const PacketQueuePair& _packetQueuePair)
{
	m_recvQueue.AddObject(_packetQueuePair);

	SetEvent(m_hEvent[EVENT_RECV]);
}