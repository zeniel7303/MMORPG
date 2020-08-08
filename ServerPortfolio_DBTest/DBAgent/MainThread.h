#pragma once
#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/Thread.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/DoubleQueue.h"

#include "DBAgent.h"
#include "DBConnectorManager.h"
#include "DBConnector.h"

#include "packet.h"

class MainThread : public Thread<MainThread>
{
public:
	static MainThread* getSingleton()
	{
		static MainThread singleton;

		return &singleton;
	}

	enum eHandle : int
	{
		EVENT_RECV,
		EVENT_SEND,
		EVENT_CONNECT,
		EVENT_DISCONNECT,
		MAX_EVENT
	};

private:
	struct PacketQueuePair
	{
		class DBAgent* agent;
		Packet* packet;

		PacketQueuePair(DBAgent* _agent, Packet* _packet)
		{
			agent = _agent;
			packet = _packet;
		}
	};

	DoubleQueue<SOCKET>				m_connectQueue;
	DoubleQueue<Packet*>			m_recvQueue;
	DoubleQueue<Packet*>			m_sendQueue;

	DBAgent*						m_dbAgent;
	DBConnectorManager*				m_dbConnectorManager;

	HANDLE							m_hEvent[MAX_EVENT];

	MainThread();
public:	
	~MainThread();

	bool Init();
	void SetManagers();

	void LoopRun();

	void ConnectUser();
	void ProcessRecv();
	void ProcessSend();

	void AddToConnectQueue(SOCKET _socket);
	void AddToRecvQueue(Packet* _packet);
	void AddToSendQueue(Packet* _packet);
};