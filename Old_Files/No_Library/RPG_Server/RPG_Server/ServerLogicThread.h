#pragma once
#include <time.h>

#include "User.h"
#include "Monster.h"
#include "packet.h"
#include "singletonBase.h"
#include "ThreadClass.h"
#include "SharedQueue.h"

#include "SessionManager.h"
#include "FieldManager.h"

//=====================================================

//ServerLogicThread

//�ش� ������ ��� �������� ��Ŷó���� �����ϴ� ���� Ŭ����
//���� ��Ŷ���� �� ���� ��ģ��.

//=====================================================

class ServerLogicThread : public ThreadClass<ServerLogicThread>,
	public singletonBase<ServerLogicThread>
{
private:
	SessionManager*					m_sessionManager;
	FieldManager*					m_fieldManager;

	SharedQueue<PacketQueuePair>	m_monsterPacketQueue;

	//�׽�Ʈ �ð� üũ��
	time_t start, end;

public:
	ServerLogicThread();
	~ServerLogicThread();

	void Init(SessionManager* _sessionManager,
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

	SharedQueue<PacketQueuePair>* GetSharedQueue() { return &m_monsterPacketQueue; }
};