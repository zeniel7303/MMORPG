#include "ServerLogicThread.h"

ServerLogicThread::ServerLogicThread()
{
}

ServerLogicThread::~ServerLogicThread()
{
}

void ServerLogicThread::Init(SessionManager* _sessionManager,
							ZoneManager* _zoneManager)
{
	m_sessionManager = _sessionManager;
	m_zoneManager = _zoneManager;

	ThreadClass<ServerLogicThread>::Start(this);

	printf("[ ServerLogicThread Init Success ]\n");
}

void ServerLogicThread::LoopRun()
{
	while(1)
	{
		//start = clock();

		ParsingUser();

		//end = clock();

		//double result = (double)(end - start);
		//printf("1 : %.3f \n", result / 1000);

		ParsingMonster();

		Sleep(1);
	}
}

void ServerLogicThread::ParsingUser()
{
	list<Session*>* vSessionList = m_sessionManager->GetItemList();

	list<Session*>::iterator iterBegin = vSessionList->begin();
	list<Session*>::iterator iterEnd = vSessionList->end();

	User* user;

	for_each(iterBegin, iterEnd, [=](Session* _session) mutable
	{
		user = dynamic_cast<User*>(_session);

		if (!user->GetIsConnected())
		{
			//존에 있었다면 해당 존에서 먼저 나간 후
			if (user->GetZone() != nullptr)
			{
				user->GetZone()->ExitUser(user);
			}

			//shutdown 이후 close
			closesocket(user->GetSocket());

			//세션매니저에서 유저를 삭제해줌과 동시에 오브젝트풀에 반환해준다.
			m_sessionManager->DeleteSession(user);
			m_sessionManager->DeleteSessionID(user->GetInfo()->userInfo.userID);

			printf("[ 접속자 수 : %d ] \n", m_sessionManager->GetItemList()->size());
		}
		else
		{
			Packet* packet = reinterpret_cast<Packet*>(user->GetRecvBuffer()->CanParsing());

			if (packet != nullptr)
			{
				//테스트용 패킷 처리
				if (packet->cmd == 1234)
				{
					ZoneNumPacket* zoneNumPacket = static_cast<ZoneNumPacket*>(packet);

					Zone* zone = m_zoneManager->GetZone(zoneNumPacket->zoneNum);

					zone->EnterTestClient(user);
				}
				else if (packet->cmd == 12345)
				{
					printf("Test Checking \n");
				}
				else
				{
					//일반적인 서버의 패킷 처리 스위치문
					switch (static_cast<RecvCommand>(packet->cmd))
					{
						//채팅 패킷
						//바로 해당 존에 SendAll
					case RecvCommand::CHATTING:
						user->GetZone()->SendAll(reinterpret_cast<char*>(packet), packet->size);
						break;
						//HeartBeahChecked 성공 패킷 받을 시
					case RecvCommand::CHECK_ALIVE:
						user->SetIsChecking(false);
						user->HeartBeatChecked();
						break;
						//회원가입 시도시
					case RecvCommand::REGISTER_ACCOUNT:
						RegisterUser(user, packet);
						break;
						//로그인 시도시
					case RecvCommand::LOGIN:
						LogInUser(user, packet);
						break;
						//접속 시 유저 정보 요청 시
					case RecvCommand::REQUIRE_INFO:
						user->SendInfo();
						break;
						//게임 시작 후 첫 존 입장 시 받는 패킷
						//스폰 위치, 타일 지정, 존에 있었다면 해당 존에서는 Exit처리
						//해당 존에 접속 중인 유저들에게 입장했다고 알려줌
					case RecvCommand::TRY_ENTER_ZONE:
						EnterZone(user, packet);
						break;
						//접속 중인 유저들 리스트 보내줌
					case RecvCommand::ENTER_ZONE_SUCCESS:
						EnterZoneSuccess(user);
						break;
						//유저 이동
					case RecvCommand::USER_MOVE:
						UpdateUserPosition(user, packet);
						break;
						//유저 이동
					case RecvCommand::USER_MOVE_FINISH:
						UpdateUserPosition(user, packet);
						break;
						//공격 실패 시(몬스터가 죽었거나 범위 밖으로 나갔을 때)
					case RecvCommand::USER_ATTACK_FAILED:
						UserAttackFailed(user, packet);
						break;
						//공격 성공 시
					case RecvCommand::USER_ATTACK_MONSTER:
						UserAttack(user, packet);
						break;
						//유저 부활 시
					case RecvCommand::USER_REVIVE:
						UserRevive(user);
						break;
						//
					case RecvCommand::ENTER_FIELD_ZONE:
						printf("[ Try To Enter Field Zone ]\n");
						EnterZone(user, packet);
						break;
						//
					case RecvCommand::ENTER_VILLAGE_ZONE:
						printf("[ Try To Enter Village Zone ]\n");
						EnterZone(user, packet);
						break;
						//접속 끊을 시
					case RecvCommand::EXIT_USER:
						UpdateUser(user, packet);
						user->Disconnect();
						break;
						//HeartBeatCheck 6회 후 유저 업데이트 요청 시
					case RecvCommand::UPDATE_INFO:
						UpdateUser(user, packet);
						break;
					}
				}
			}
		}
	});

	//일반적인 For문
	/*
	list<Session*>::iterator iterEnd = vSessionList->end();

	User* user;

	for (list<Session*>::iterator iter = vSessionList->begin(); iter != iterEnd;)
	{
		user = dynamic_cast<User*>(*iter);
		++iter;

		if (!user->GetIsConnected())
		{
			//존에 있었다면 해당 존에서 먼저 나간 후
			if (user->GetZone() != nullptr)
			{
				user->GetZone()->ExitUser(user);
			}

			//shutdown 이후 close
			closesocket(user->GetSocket());

			//세션매니저에서 유저를 삭제해줌과 동시에 오브젝트풀에 반환해준다.
			m_sessionManager->DeleteSession(user);
			m_sessionManager->DeleteSessionID(user->GetInfo()->userInfo.userID);

			printf("[ 접속자 수 : %d ] \n", m_sessionManager->GetItemList()->size());

			continue;
		}

		Packet* packet = reinterpret_cast<Packet*>(user->GetRecvBuffer()->CanParsing());

		if (packet == nullptr) continue;
		
		//테스트용 임시
		if (packet->cmd == 1234)
		{
			ZoneNumPacket* zoneNumPacket = static_cast<ZoneNumPacket*>(packet);

			Zone* zone = m_zoneManager->GetZone(zoneNumPacket->zoneNum);

			zone->EnterTestClient(user);
		}
		else
		{
			switch (static_cast<RecvCommand>(packet->cmd))
			{
			case RecvCommand::CHATTING:
				user->GetZone()->SendAll(reinterpret_cast<char*>(packet), packet->size);
				break;
			case RecvCommand::CHECK_ALIVE:
				user->SetIsChecking(false);
				user->HeartBeatChecked();
				break;
			case RecvCommand::REGISTER_ACCOUNT:
				RegisterUser(user, packet);
				break;
			case RecvCommand::LOGIN:
				LogInUser(user, packet);
				break;
			case RecvCommand::REQUIRE_INFO:
				user->SendInfo();
				break;
			case RecvCommand::TRY_ENTER_ZONE:
				EnterZone(user, packet);
				break;
			case RecvCommand::ENTER_ZONE_SUCCESS:
				EnterZoneSuccess(user);
				break;
			case RecvCommand::USER_MOVE:
				UpdateUserPosition(user, packet);
				break;
			case RecvCommand::USER_MOVE_FINISH:
				UpdateUserPosition(user, packet);
				break;
			case RecvCommand::USER_ATTACK_FAILED:
				UserAttackFailed(user, packet);
				break;
			case RecvCommand::USER_ATTACK_MONSTER:
				UserAttack(user, packet);
				break;
			case RecvCommand::USER_REVIVE:
				UserRevive(user);
				break;
			case RecvCommand::ENTER_FIELD_ZONE:
				printf("[ Try To Enter Field Zone ]\n");
				EnterZone(user, packet);
				break;
			case RecvCommand::ENTER_VILLAGE_ZONE:
				printf("[ Try To Enter Village Zone ]\n");
				EnterZone(user, packet);
				break;
			case RecvCommand::EXIT_USER:
				UpdateUser(user, packet);
				user->Disconnect();
				break;
			case RecvCommand::UPDATE_INFO:
				UpdateUser(user, packet);
				break;
			}
		}
	}
	*/
}

void ServerLogicThread::ParsingMonster()
{
	while (1)
	{
		if (m_monsterPacketQueue.IsEmpty()) return;

		PacketQueuePair* packetQueuePair = &m_monsterPacketQueue.GetItem();
		Packet* packet = packetQueuePair->packet;
		Monster* monster = packetQueuePair->monster;

		//해당 몬스터가 죽어있다면 패스
		if (monster->GetIsDeath()) continue;

		switch (static_cast<SendCommand>(packet->cmd))
		{
		case SendCommand::MONSTER_ATTACK:
			MonsterAttack(monster, packet);
			break;
		default:
			monster->GetZone()->
				SendAll(reinterpret_cast<char*>(packet), packet->size);
			break;
		}
	}
}

void ServerLogicThread::EnterZone(User* _user, Packet* _packet)
{
	ZoneNumPacket* zoneNumPacket = static_cast<ZoneNumPacket*>(_packet);

	Zone* zone = m_zoneManager->GetZone(zoneNumPacket->zoneNum);
	if (zone == nullptr) return;

	Zone* prevZone = m_zoneManager->GetZone(_user->GetInfo()->unitInfo.zoneNum);
	if (prevZone != nullptr)
	{
		printf("[ Exit User (Prev Zone) ]");
		prevZone->ExitUser(_user);
	}

	zone->EnterUser(_user);
}

void ServerLogicThread::EnterZoneSuccess(User* _user)
{
	_user->StartCheckingHeartBeat();

	Zone* zone = _user->GetZone();

	zone->SendUserList(_user);
}

void ServerLogicThread::UpdateUserPosition(User* _user, Packet* _packet)
{
	UserPositionPacket* userPositionPacket = static_cast<UserPositionPacket*>(_packet);

	Zone* zone = _user->GetZone();

	_user->SetPosition(userPositionPacket->position);
	zone->SendAll(reinterpret_cast<char*>(_packet), _packet->size);
}

void ServerLogicThread::UserAttackFailed(User* _user, Packet* _packet)
{
	UserAttackPacket * userAttackPacket = static_cast<UserAttackPacket*>(_packet);

	Zone* zone = _user->GetZone();
	zone->SendAll(reinterpret_cast<char*>(_packet), _packet->size);
}

void ServerLogicThread::UserAttack(User* _user, Packet* _packet)
{
	UserAttackPacket * userAttackPacket = static_cast<UserAttackPacket*>(_packet);

	Zone* zone = _user->GetZone();

	if (zone->UserAttack(_user, userAttackPacket->monsterIndex))
	{
		zone->SendAll(reinterpret_cast<char*>(_packet), _packet->size);
	}
}

void ServerLogicThread::UserRevive(User* _user)
{
	Zone* zone = _user->GetZone();

	zone->RespawnUser(_user);
}

void ServerLogicThread::MonsterAttack(Monster* _monster, Packet* _packet)
{
	MonsterAttackPacket * monsterAttackPacket = static_cast<MonsterAttackPacket*>(_packet);

	User* user = _monster->GetTarget();
	if (user == nullptr) return;

	user->Hit(monsterAttackPacket->monsterIndex, monsterAttackPacket->damage);
}

void ServerLogicThread::RegisterUser(User* _user, Packet* _packet)
{
	RegisterUserPacket* registerUserPacket = static_cast<RegisterUserPacket*>(_packet);

	_user->RegisterUser(registerUserPacket);
}

void ServerLogicThread::LogInUser(User* _user, Packet* _packet)
{
	LogInPacket* logInPacket = static_cast<LogInPacket*>(_packet);

	if (_user->LogInUser(logInPacket))
	{
		//중복 로그인체크
		if (m_sessionManager->FindSessionID(_user->GetInfo()->userInfo.userID))
		{
			//중복 로그인이니 접속하지마라
			_user->LogInDuplicated();

			return;
		}

		m_sessionManager->AddSessionID(_user->GetInfo()->userInfo.userID);

		_user->LogInSuccess();
	}
}

void ServerLogicThread::UpdateUser(User* _user, Packet* _packet)
{
	SessionInfoPacket* sessionInfoPacket = static_cast<SessionInfoPacket*>(_packet);

	_user->GetInfo()->unitInfo = sessionInfoPacket->info.unitInfo;
	_user->GetInfo()->userInfo = sessionInfoPacket->info.userInfo;

	_user->UpdateInfo();
}