#pragma once
#include <time.h>

#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/Thread.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/DoubleQueue.h"

#include "User.h"
#include "Monster.h"
#include "packet.h"
#include "SessionManager.h"
#include "FieldManager.h"

#include "PacketHandler.h"

//=====================================================

//MainThread

//해당 서버의 모든 유저들의 패킷처리를 관리하는 단일 클래스
//몬스터 패킷또한 이 곳을 거친다.

//Singleton을 상속하다가 따로 빼둔 이유
//다중 상속을 사용한 실수를 방지하기 위함.

//=====================================================
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
		EVENT_MONSTER,
		EVENT_DBCONNECTOR,
		MAX_EVENT
	};

	struct PacketQueuePair_User
	{
		class User* user;
		Packet* packet;

		PacketQueuePair_User(User* _user, Packet* _packet)
		{
			user = _user;
			packet = _packet;
		}
	};

	struct PacketQueuePair_Monster
	{
		class Monster* monster;
		Packet* packet;

		PacketQueuePair_Monster(Monster* _monster, Packet* _packet)
		{
			monster = _monster;
			packet = _packet;
		}
	};
		
private:
	SessionManager*							m_sessionManager;
	FieldManager*							m_fieldManager;

	PacketHandler*							m_packetHandler;

	DoubleQueue<PacketQueuePair_User>		m_userPacketQueue;
	DoubleQueue<PacketQueuePair_Monster>	m_monsterPacketQueue;
	DoubleQueue<Packet*>					m_dbPacketQueue;
	DoubleQueue<SOCKET>						m_connectQueue;
	DoubleQueue<Session*>					m_disconnectQueue;

	HANDLE									m_hEvent[MAX_EVENT];

	MainThread();

public:
	~MainThread();

	bool Init();
	void SetManagers(SessionManager* _sessionManager,
		FieldManager* _fieldManager);

	void LoopRun();

	void ProcessUserPacket();
	void ProcessMonsterPacket();
	void ProcessDBConnectorPacket();
	void ConnectUser();
	void DisConnectUser();

	void AddToUserPacketQueue(const PacketQueuePair_User& _userPacketQueuePair);
	void AddToMonsterPacketQueue(const PacketQueuePair_Monster& _monsterPacketQueuePair);
	void AddToDBConnectorPacketQueue(Packet* _packet);
	void AddToConnectQueue(SOCKET _socket);
	void AddToDisConnectQueue(Session* _session);
};