#pragma once
#include "../../ZoneServer/ServerLibrary/HeaderFiles/FileLog.h"
#include "../../ZoneServer/ServerLibrary/HeaderFiles/OnlyHeaders/Thread.h"
#include "../../ZoneServer/ServerLibrary/HeaderFiles/OnlyHeaders/DoubleQueue.h"

#include "PathFinderManager.h"

#include "PathFinderAgentManager.h"
#include "PathFinderAgent.h"

#include "FieldManager.h"

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
		EVENT_CONNECT,
		EVENT_DISCONNECT,
		EVENT_RECV,
		MAX_EVENT
	};

private:
	struct PacketQueuePair
	{
		class PathFinderAgent* agent;
		Packet* packet;

		PacketQueuePair(PathFinderAgent* _agent, Packet* _packet)
		{
			agent = _agent;
			packet = _packet;
		}
	};

	DoubleQueue<SOCKET>				m_connectQueue;
	DoubleQueue<PathFinderAgent*>	m_disconnectQueue;
	DoubleQueue<PacketQueuePair>	m_recvQueue;

	PathFinderManager*				m_pathFinderManager;
	PathFinderAgentManager*			m_pathFinderAgentManager;

	FieldManager*					m_fieldManager;

	HANDLE							m_hEvent[MAX_EVENT];

public:
	MainThread();
	~MainThread();

	bool Init();
	void SetManagers(PathFinderAgentManager* _pathFinderAgentManager, 
		FieldManager* _fieldManager);

	void LoopRun();

	using Process = void (MainThread::*)();
	Process processFunc[MAX_EVENT];

	void ConnectUser();
	void DisConnectUser();
	void ProcessRecv();

	void AddToConnectQueue(SOCKET _socket);
	void AddToDisConnectQueue(PathFinderAgent* _agent);
	void AddToRecvQueue(const PacketQueuePair& _packetQueuePair);

	FieldManager* GetFieldManager() { return m_fieldManager; }
};

