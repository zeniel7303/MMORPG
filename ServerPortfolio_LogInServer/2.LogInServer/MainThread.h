#pragma once
#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/Thread.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/DoubleQueue.h"

#include "LogInSession.h"
#include "LogInSessionManager.h"

#include "PacketHandler.h"

class ZoneConnector;

class HeartBeatThread;

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
		EVENT_STOREUSER,
		EVENT_DBCONNECTOR,
		EVENT_ZONESERVER,
		EVENT_HEARTBEAT,
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

	DoubleQueue<SOCKET>				m_connectQueue;
	DoubleQueue<LogInSession*>		m_disconnectQueue;
	DoubleQueue<Packet*>			m_dbPacketQueue;
	DoubleQueue<Packet*>			m_zoneServerPacketQueue;
	DoubleQueue<LogInSession*>		m_hashMapQueue;

	HANDLE							m_hEvent[MAX_EVENT];

	LogInSessionManager*			m_logInSessionManager;

	PacketHandler*					m_packetHandler;

	HeartBeatThread*				m_heartBeatThread;

	ZoneConnector*					m_zoneConnector;

public:	
	~MainThread();

	bool Init();
	void SetManagers(LogInSessionManager* _logInSessionManager, HeartBeatThread* _heartBeatThread);

	void LoopRun();

	using Process = void (MainThread::*)();
	Process processFunc[MAX_EVENT];

	void ConnectUser();
	void DisConnectUser();
	void AddToHashMap();
	void ProcessDBConnectorPacket();
	void ProcessZoneServerPacket();
	void ProcessHeartBeat();

	void ConnectWithZoneServer(SOCKET _socket);

	void AddToConnectQueue(SOCKET _socket);
	void AddToDisConnectQueue(LogInSession* _session);
	void AddToHashMapQueue(LogInSession* _session);
	void AddToDBConnectorPacketQueue(Packet* _packet);
	void AddToZoneServerPacketQueue(Packet* _packet);
	void HearBeatCheck();

	ZoneConnector* GetZoneConnector() { return m_zoneConnector; }
};

