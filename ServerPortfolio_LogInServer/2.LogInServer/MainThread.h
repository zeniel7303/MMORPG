#pragma once
#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/Thread.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/DoubleQueue.h"

#include "LogInSession.h"
#include "LogInSessionManager.h"

#include "PacketHandler.h"

class ZoneConnector;
class ZoneServerManager;

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

	struct PacketQueuePair_ZoneNum
	{
		int m_zoneNum;
		Packet* packet;

		PacketQueuePair_ZoneNum(int _num, Packet* _packet)
		{
			m_zoneNum = _num;
			packet = _packet;
		}
	};

private:
	MainThread();

	DoubleQueue<SOCKET>							m_connectQueue;
	DoubleQueue<LogInSession*>					m_disconnectQueue;
	DoubleQueue<Packet*>						m_dbPacketQueue;
	DoubleQueue<PacketQueuePair_ZoneNum>		m_zoneServerPacketQueue;
	DoubleQueue<LogInSession*>					m_hashMapQueue;

	HANDLE										m_hEvent[MAX_EVENT];

	LogInSessionManager*						m_logInSessionManager;

	PacketHandler*								m_packetHandler;

	HeartBeatThread*							m_heartBeatThread;

	ZoneServerManager*							m_zoneServerManager;

	int											m_num;

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
	void AddToZoneServerPacketQueue(const PacketQueuePair_ZoneNum& _packetQueue);
	void HearBeatCheck();

	ZoneServerManager* GetZoneServerManager() { return m_zoneServerManager; }
};

