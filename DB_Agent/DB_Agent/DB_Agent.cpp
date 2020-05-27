#include "DB_Agent.h"

DB_Agent::DB_Agent()
{
}

DB_Agent::~DB_Agent()
{
}

void DB_Agent::Init()
{
	Session::Init();

	m_IOCPClass.Init();
	m_listenClass.Init(&m_IOCPClass, this, 
		"192.168.0.13", 30003);
}

void DB_Agent::GetConnectorManager(DBConnectorManager* _connectorManager)
{
	m_connectorManager = _connectorManager;

	EventClass<DB_Agent>::CreateEventHandle(this);
	ThreadClass<DB_Agent>::Start(this);

	WaitForSingleObject(m_listenClass.GetHandle(), INFINITE);
}

void DB_Agent::Parsing()
{
	int tempNum = 20;

	while (1)
	{
		Packet* packet = reinterpret_cast<Packet*>(GetRecvBuffer()->CanParsing());

		if (packet == nullptr) break;

		m_recvSharedQueue.AddItem(packet);
		SetEvent(EventClass<DB_Agent>::GetEventHandle());

		tempNum--;

		if (tempNum <= 0)
		{
			tempNum = 20;

			break;
		}
	}
}

void DB_Agent::EventFunc()
{
	while (1)
	{
		int tempNum = 20;

		WaitForSingleObject(EventClass<DB_Agent>::GetEventHandle(), INFINITE);

		while (1)
		{
			for (const auto& element : m_connectorManager->GetConnectorList())
			{
				if (element->GetState() == READY)
				{
					Packet* tempPacket = m_recvSharedQueue.GetItem();

					element->SetPacket(tempPacket);
					SetEvent(element->EventClass<DBConnector>::GetEventHandle());
				}

				if (m_recvSharedQueue.GetSize() <= 0) break;
			}

			tempNum--;

			if (tempNum <= 0 || m_recvSharedQueue.GetSize() <= 0)
			{
				tempNum = 20;

				ResetEvent(EventClass<DB_Agent>::GetEventHandle());

				break;
			}
		}
	}
}

void DB_Agent::LoopRun()
{
	while (1)
	{
		if (m_sendSharedQueue.IsEmpty()) continue;

		Packet* packet = m_sendSharedQueue.GetItem();
		Send(packet);

		Sleep(1);
	}
}