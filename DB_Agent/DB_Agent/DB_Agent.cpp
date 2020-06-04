#include "DB_Agent.h"

DB_Agent::DB_Agent()
{
}

DB_Agent::~DB_Agent()
{
}

void DB_Agent::Init()
{
	m_hEvent[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hEvent[1] = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hThread[0] = (HANDLE)_beginthreadex(NULL, 0, &ThreadFunc_1, (void*)this, 0, &m_threadID[0]);
	m_hThread[1] = (HANDLE)_beginthreadex(NULL, 0, &ThreadFunc_2, (void*)this, 0, &m_threadID[1]);

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
		SetEvent(m_hEvent[0]);

		tempNum--;

		if (tempNum <= 0)
		{
			tempNum = 20;

			break;
		}
	}
}

//recv부분
void DB_Agent::Thread_1()
{
	while (1)
	{
		int tempNum = 20;

		WaitForSingleObject(m_hEvent[0], INFINITE);

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
				
				//쓰지마
				//ResetEvent(hEvent);

				break;
			}
		}
	}
}

//send 부분
void DB_Agent::Thread_2()
{
	while (1)
	{
		WaitForSingleObject(m_hEvent[1], INFINITE);

		if (m_sendSharedQueue.IsEmpty()) continue;

		Packet* packet = m_sendSharedQueue.GetItem();
		Send(packet);

		Sleep(1);
	}
}

//200604
//현재 Thread 2개 돌려가면서 쓰는건 마이너스. (수업예정)