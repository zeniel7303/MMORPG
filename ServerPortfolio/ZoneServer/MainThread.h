#pragma once
#include <time.h>

#include "../ServerLibrary/HeaderFiles/FileLog.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/Thread.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/DoubleQueue.h"

#include "User.h"
#include "Monster.h"
#include "packet.h"
#include "UserManager.h"
#include "FieldManager.h"

#include "PacketHandler.h"

#include "ThreadSchedular.h"

//=====================================================

//MainThread

//해당 서버의 모든 유저들의 패킷처리를 관리하는 단일 클래스
//몬스터 패킷또한 이 곳을 거친다.

//Singleton을 상속하다가 따로 빼둔 이유
//다중 상속을 사용한 실수를 방지하기 위함.

//=====================================================

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
		EVENT_RECV,
		EVENT_CONNECT,
		EVENT_DISCONNECT,
		EVENT_MONSTER,
		EVENT_DBCONNECTOR,
		EVENT_LOGINSERVER,
		EVENT_STOREUSER,
		EVENT_DB_HEARTBEAT,
		EVENT_LOGIN_HEARTBEAT,
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
	UserManager*							m_userManager;
	FieldManager*							m_fieldManager;
	/*std::shared_ptr<UserManager>			m_userManager;
	std::shared_ptr<FieldManager>			m_fieldManager;*/

	PacketHandler*							m_packetHandler;
	HeartBeatThread*						m_heartBeatThread;

	ThreadSchedular*						m_threadSchedular;

	DoubleQueue<PacketQueuePair_User>		m_userPacketQueue;
	DoubleQueue<SOCKET>						m_connectQueue;
	DoubleQueue<User*>						m_disconnectQueue;
	DoubleQueue<PacketQueuePair_Monster>	m_monsterPacketQueue;
	DoubleQueue<Packet*>					m_dbPacketQueue;
	DoubleQueue<Packet*>					m_logInServerPacketQueue;
	DoubleQueue<User*>						m_hashMapQueue;

	HANDLE									m_hEvent[MAX_EVENT];

	MainThread();

public:
	~MainThread();

	bool Init();
	void SetManagers(UserManager* _userManager,
		FieldManager* _fieldManager);
	//완전한 자원이동을 위해 rvalue reference로 받는다.
	/*void SetManagers(std::unique_ptr<UserManager>&& _userManager,
		std::unique_ptr<FieldManager>&& _fieldManager);*/

	void LoopRun();

	using Process = void (MainThread::*)();
	Process processFunc[MAX_EVENT];

	void ProcessUserPacket();
	void ConnectUser();
	void DisConnectUser();
	void ProcessMonsterPacket();
	void ProcessDBConnectorPacket();
	void ProcessLogInServerPacket();
	void AddToHashMap();
	void HeartBeat_DBAgent();
	void HeartBeat_LoginServer();

	void AddToUserPacketQueue(const PacketQueuePair_User& _userPacketQueuePair);
	void AddToConnectQueue(SOCKET _socket);
	void AddToDisConnectQueue(User* _user);
	void AddToMonsterPacketQueue(const PacketQueuePair_Monster& _monsterPacketQueuePair);
	void AddToDBConnectorPacketQueue(Packet* _packet);
	void AddToLogInServerPacketQueue(Packet* _packet);
	void AddToHashMapQueue(User* _user);
};