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

void MainThread::SetManagers()
{
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
		if (m_dbAgent != nullptr) return;

		m_dbAgent = new DBAgent();
		m_dbAgent->SetSocket(connectQueue.front());
		m_dbAgent->OnConnect();

		connectQueue.pop();
	}
}

void MainThread::ProcessRecv()
{
	std::queue<Packet*>& packetQueue = m_recvQueue.GetSecondaryQueue();

	size_t size = packetQueue.size();

	for (int i = 0; i < size; i++)
	{
		Packet* packet = packetQueue.front();

		for (const auto& element : m_dbConnectorManager->GetConnectorList())
		{
			if (element->GetState() == CONNECTOR_STATE::READY)
			{
				element->SetPacket(packet);

				packetQueue.pop();

				break;
			}
		}
	}
}

void MainThread::ProcessSend()
{
	std::queue<Packet*>& packetQueue = m_sendQueue.GetSecondaryQueue();

	size_t size = packetQueue.size();

	for (int i = 0; i < size; i++)
	{
		Packet* packet = packetQueue.front();

		m_dbAgent->Send(reinterpret_cast<char*>(packet), packet->size);

		packetQueue.pop();
	}
}

void MainThread::AddToConnectQueue(SOCKET _socket)
{
	m_connectQueue.AddObject(_socket);

	SetEvent(m_hEvent[EVENT_CONNECT]);
}

void MainThread::AddToRecvQueue(Packet* _packet)
{
	m_recvQueue.AddObject(_packet);

	SetEvent(m_hEvent[EVENT_RECV]);
}

void MainThread::AddToSendQueue(Packet* _packet)
{
	m_sendQueue.AddObject(_packet);

	SetEvent(m_hEvent[EVENT_SEND]);
}