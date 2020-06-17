#pragma once
#include <time.h>

#include "../ServerLibrary/HeaderFiles/OnlyHeaders/SingletonBase.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/Thread.h"
#include "../ServerLibrary/HeaderFiles/OnlyHeaders/DoubleQueue.h"

#include "User.h"
#include "Monster.h"
#include "packet.h"
#include "SessionManager.h"
#include "FieldManager.h"

//=====================================================

//ServerLogicThread

//해당 서버의 모든 유저들의 패킷처리를 관리하는 단일 클래스
//몬스터 패킷또한 이 곳을 거친다.

//=====================================================

class ServerLogicThread : public Thread<ServerLogicThread>,
						  public SingletonBase<ServerLogicThread>
{
private:
	SessionManager*							m_sessionManager;
	FieldManager*							m_fieldManager;

	DoubleQueue<PacketQueuePair_Monster>	m_monsterPacketDoubleQueue;
	DoubleQueue<PacketQueuePair_User>		m_userPacketDoubleQueue;
	DoubleQueue<Packet*>*					m_dbConnectorPacketDoubleQueue;

	HANDLE									m_hEvent[3];

	//테스트 시간 체크용
	time_t start, end;

public:
	ServerLogicThread();
	~ServerLogicThread();

	void Init();
	void GetManagers(SessionManager* _sessionManager,
		FieldManager* _fieldManager);

	void LoopRun();

	void ParsingUser();
	void ParsingMonster();
	void ParsingConnector();

	void OnPacket_EnterField(User* _user, Packet* _packet);
	void OnPacket_EnterFieldSuccess(User* _user);
	void OnPacket_UpdateUserPosition(User* _user, Packet* _packet, bool _isFinish);
	void OnPacket_UserAttackFailed(User* _user, Packet* _packet);
	void OnPacket_UserAttack(User* _user, Packet* _packet);
	void OnPacket_UserRevive(User* _user);

	void OnPacket_MonsterAttack(Monster* _monster, Packet* _packet);

	void OnPacket_RegisterUser(User* _user, Packet* _packet);
	void OnPacket_LogInUser(User* _user, Packet* _packet);
	void OnPacket_UpdateUser(User* _user, Packet* _packet);

	//DB agent====================================================
	void OnPacket_LogInSuccess(Packet* _packet);
	void OnPacket_LogInFailed(Packet* _packet);

	void OnPacket_RegisterSuccess(Packet* _packet);
	void OnPacket_RegisterFailed(Packet* _packet);

	void OnPacket_GetUserInfoSuccess(Packet* _packet);
	void OnPacket_GetUserInfoFailed(Packet* _packet);

	void OnPacket_UpdateUserSuccess(Packet* _packet);
	void OnPacket_UpdateUserFailed(Packet* _packet);

	Session* FindUser(SOCKET _socket);
	//============================================================

	void OnPacket_EnterTestUser(User* _user, Packet* _packet);

	DoubleQueue<PacketQueuePair_Monster>* GetMonsterDoubleQueue() { return &m_monsterPacketDoubleQueue; }
	DoubleQueue<PacketQueuePair_User>* GetSessionDoubleQueue() { return &m_userPacketDoubleQueue; }

	HANDLE GetEventHandle(int num) { return m_hEvent[num]; }
};