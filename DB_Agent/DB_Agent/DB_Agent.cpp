#include "DB_Agent.h"

DB_Agent::DB_Agent()
{
}

DB_Agent::~DB_Agent()
{
}

void DB_Agent::Init()
{
	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	hThread[0] = (HANDLE)_beginthreadex(NULL, 0, &ThreadFunc_1, (void*)this, 0, &threadID[0]);
	hThread[1] = (HANDLE)_beginthreadex(NULL, 0, &ThreadFunc_2, (void*)this, 0, &threadID[1]);

	Session::Init();

	m_IOCPClass.Init();
	m_listenClass.Init(&m_IOCPClass, this, 
		"192.168.0.13", 30003);
}

void DB_Agent::GetConnectorManager(DBConnectorManager* _connectorManager)
{
	m_connectorManager = _connectorManager;

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
		SetEvent(hEvent);

		tempNum--;

		if (tempNum <= 0)
		{
			tempNum = 20;

			break;
		}
	}
}

void DB_Agent::Thread_1()
{
	while (1)
	{
		int tempNum = 20;

		WaitForSingleObject(hEvent, INFINITE);

		while (1)
		{
			for (const auto& element : m_connectorManager->GetConnectorList())
			{
				if (element->GetState() == READY)
				{
					Packet* tempPacket = m_recvSharedQueue.GetItem();

					element->SetPacket(tempPacket);
					SetEvent(element->GetEventHandle());
				}

				if (m_recvSharedQueue.GetSize() <= 0) break;
			}

			tempNum--;

			if (tempNum <= 0 || m_recvSharedQueue.GetSize() <= 0)
			{
				tempNum = 20;
				
				//¾²Áö¸¶
				//ResetEvent(hEvent);

				break;
			}
		}
	}
}

void DB_Agent::Thread_2()
{
	while (1)
	{
		if (m_sendSharedQueue.IsEmpty()) continue;

		Packet* packet = m_sendSharedQueue.GetItem();
		Send(packet);

		Sleep(1);
	}
}