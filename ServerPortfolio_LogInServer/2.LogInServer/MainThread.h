#pragma once
#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/Thread.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/DoubleQueue.h"

#include "LogInSession.h"
#include "LogInSessionManager.h"

#include "PacketHandler.h"

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
		EVENT_CONNECT,
		EVENT_DISCONNECT,
		EVENT_DBCONNECTOR,
		MAX_EVENT
	};

	struct PacketQueuePair
	{
		class LogInSession* session;
		Packet* packet;

		PacketQueuePair(LogInSession* _session, Packet* _packet)
		{
			session = _session;
			packet = _packet;
		}
	};

private:
	MainThread();

	DoubleQueue<PacketQueuePair>			m_recvQueue;
	DoubleQueue<SOCKET>						m_connectQueue;
	DoubleQueue<LogInSession*>				m_disconnectQueue;
	DoubleQueue<Packet*>					m_dbPacketQueue;

	HANDLE									m_hEvent[MAX_EVENT];

	LogInSessionManager*					m_logInSessionManager;

	PacketHandler*							m_packetHandler;

public:	
	~MainThread();

	bool Init();
	void SetManagers(LogInSessionManager* _logInSessionManager);

	void LoopRun();

	using Process = void (MainThread::*)();
	Process processFunc[MAX_EVENT];

	void ProcessPacket();
	void ConnectUser();
	void DisConnectUser();
	void ProcessDBConnectorPacket();

	void AddToUserPacketQueue(const PacketQueuePair& _packetQueuePair);
	void AddToConnectQueue(SOCKET _socket);
	void AddToDisConnectQueue(LogInSession* _session);
	void AddToDBConnectorPacketQueue(Packet* _packet);
};

