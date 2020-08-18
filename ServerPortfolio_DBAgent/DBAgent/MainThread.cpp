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

	m_dbAgent = nullptr;

	TRYCATCH(m_dbConnectorManager = new DBConnectorManager());

	return true;
}

void MainThread::SetManagers(DBAgentManager* _dbAgentManager)
{
	m_dbAgentManager = _dbAgentManager;

	Thread<MainThread>::Start(this);

	MYDEBUG("[ MainThread Start ]\n");
}

void MainThread::LoopRun()
{
	while (1)
	{
		int num = WaitForMultipleObjects(MAX_EVENT, m_hEvent, FALSE, INFINITE);

		switch (num)
		{
		case EVENT_RECV:
		{
			m_recvQueue.Swap();

			ProcessRecv();
		}
			break;
		case EVENT_SEND:
		{
			m_sendQueue.Swap();

			ProcessSend();
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

void MainThread::ConnectUser()
{
	std::queue<SOCKET>& connectQueue = m_connectQueue.GetSecondaryQueue();

	size_t size = connectQueue.size();

	for (int i = 0; i < size; i++)
	{
		DBAgent* tempDBAgent = m_dbAgentManager->PopAgent();

		if (tempDBAgent == nullptr)
		{
			//예외처리
			MYDEBUG("[ dbAgent is nullptr ] \n");
			FILELOG("dbAgent is nullptr");

			// + LOG 남기기
			//접속 정원됐으니까 패킷 하나 보내주자. 혹은 시간 지나면 ?
			continue;
		}

		tempDBAgent->SetSocket(connectQueue.front());

		tempDBAgent->OnConnect();

		m_dbAgentManager->AddAgent(tempDBAgent);
		MYDEBUG("SessionList Size : %zd \n", m_dbAgentManager->GetAgentList()->GetItemList().size());

		connectQueue.pop();
	}
}

void MainThread::DisConnectUser()
{
	std::queue<DBAgent*>& disconnectQueue = m_disconnectQueue.GetSecondaryQueue();

	size_t size = disconnectQueue.size();

	for (int i = 0; i < size; i++)
	{
		DBAgent* tempDBAgent = disconnectQueue.front();

		//세션매니저에서 유저를 삭제해줌과 동시에 오브젝트풀에 반환해준다.
		m_dbAgentManager->ReturnAgent(tempDBAgent);
		MYDEBUG("SessionList Size : %zd \n", m_dbAgentManager->GetAgentList()->GetItemList().size());

		disconnectQueue.pop();
	}
}

void MainThread::ProcessRecv()
{
	std::queue<PacketQueuePair>& recvQueue = m_recvQueue.GetSecondaryQueue();

	size_t size = recvQueue.size();

	for (int i = 0; i < size; i++)
	{
		const PacketQueuePair& packetQueuePair = recvQueue.front();
		Packet* packet = packetQueuePair.packet;
		DBAgent* dbAgent = packetQueuePair.agent;

		if (packet->cmd == static_cast<WORD>(RecvCommand::LogIn2DB_HELLO))
		{
			MYDEBUG("[ LogInServer Connected ]\n");

			recvQueue.pop();

			break;
		}

		for (const auto& element : m_dbConnectorManager->GetConnectorList())
		{
			if (element->GetState() == CONNECTOR_STATE::READY)
			{
				element->SetDBAgent(dbAgent);
				element->SetPacket(packet);

				recvQueue.pop();

				break;
			}
		}
	}
}

void MainThread::ProcessSend()
{
	std::queue<PacketQueuePair>& sendQueue = m_sendQueue.GetSecondaryQueue();

	size_t size = sendQueue.size();

	for (int i = 0; i < size; i++)
	{
		const PacketQueuePair& packetQueuePair = sendQueue.front();
		Packet* packet = packetQueuePair.packet;
		DBAgent* dbAgent = packetQueuePair.agent;

		dbAgent->Send(reinterpret_cast<char*>(packet), packet->size);

		sendQueue.pop();
	}
}

void MainThread::AddToConnectQueue(SOCKET _socket)
{
	m_connectQueue.AddObject(_socket);

	SetEvent(m_hEvent[EVENT_CONNECT]);
}

void MainThread::AddToDisConnectQueue(DBAgent* _session)
{
	m_disconnectQueue.AddObject(_session);

	SetEvent(m_hEvent[EVENT_DISCONNECT]);
}

void MainThread::AddToRecvQueue(const PacketQueuePair& _packetQueuePair)
{
	m_recvQueue.AddObject(_packetQueuePair);

	SetEvent(m_hEvent[EVENT_RECV]);
}

void MainThread::AddToSendQueue(const PacketQueuePair& _packetQueuePair)
{
	m_sendQueue.AddObject(_packetQueuePair);

	SetEvent(m_hEvent[EVENT_SEND]);
}