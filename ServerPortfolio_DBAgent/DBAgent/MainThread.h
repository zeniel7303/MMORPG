#pragma once
#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/Thread.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/DoubleQueue.h"

#include "DBAgent.h"
#include "DBAgentManager.h"
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
	DoubleQueue<DBAgent*>			m_disconnectQueue;
	DoubleQueue<PacketQueuePair>			m_recvQueue;
	DoubleQueue<PacketQueuePair>			m_sendQueue;

	DBAgent*						m_dbAgent;
	DBConnectorManager*				m_dbConnectorManager;
	DBAgentManager*					m_dbAgentManager;

	HANDLE							m_hEvent[MAX_EVENT];

	MainThread();
public:	
	~MainThread();

	bool Init();
	void SetManagers(DBAgentManager* _dbAgentManager);

	void LoopRun();

	void ConnectUser();
	void DisConnectUser();
	void ProcessRecv();
	void ProcessSend();

	void AddToConnectQueue(SOCKET _socket);
	void AddToDisConnectQueue(DBAgent* _session);
	void AddToRecvQueue(const PacketQueuePair& _packetQueuePair);
	void AddToSendQueue(const PacketQueuePair& _packetQueuePair);
};