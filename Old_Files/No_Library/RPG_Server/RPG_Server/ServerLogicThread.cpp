#include "ServerLogicThread.h"

#include "ConnectorClass.h"

ServerLogicThread::ServerLogicThread()
{
}

ServerLogicThread::~ServerLogicThread()
{
}

void ServerLogicThread::Init(SessionManager* _sessionManager,
							 FieldManager* _fieldManager)
{
	m_sessionManager = _sessionManager;
	m_fieldManager = _fieldManager;

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

		ParsingConnector();

		Sleep(1);
	}
}

void ServerLogicThread::ParsingUser()
{
	list<Session*>* vSessionList = m_sessionManager->GetItemList();

	User* user;

	//range-based for문
	/*
	for (const auto& element : *vSessionList)
	{
		user = dynamic_cast<User*>(element);

		if (!user->GetIsConnected())
		{
			//존에 있었다면 해당 존에서 먼저 나간 후
			if (user->GetField() != nullptr)
			{
				user->GetField()->ExitUser(user);
			}

			//shutdown 이후 close
			closesocket(user->GetSocket());

			//세션매니저에서 유저를 삭제해줌과 동시에 오브젝트풀에 반환해준다.
			m_sessionManager->DeleteSession(user);
			m_sessionManager->DeleteSessionID(user->GetInfo()->userInfo.userID);

			continue;
		}

		int tempNum = 10;

		while (1)
		{
			Packet* packet = reinterpret_cast<Packet*>(user->GetRecvBuffer()->CanParsing());

			if (packet == nullptr) break;

			//테스트용 패킷 처리
			if (packet->cmd == 12345)
			{
				printf("Test Checking \n");
			}
			else
			{
				switch (static_cast<RecvCommand>(packet->cmd))
				{
					//채팅 패킷
					//바로 해당 존에 SendAll
				case RecvCommand::C2Zone_CHATTING:
					user->GetField()->
						SectorSendAll(user->GetSector()->GetRoundSectorsVec(),
							reinterpret_cast<char*>(packet), packet->size);
					break;
					//HeartBeatChecked 성공 패킷 받을 시
				case RecvCommand::C2Zone_CHECK_ALIVE:
					user->HeartBeatChecked();
					break;
					//회원가입 시도시
				case RecvCommand::C2Zone_REGISTER_ACCOUNT:
					OnPacket_RegisterUser(user, packet);
					break;
					//로그인 시도시
				case RecvCommand::C2Zone_LOGIN:
					OnPacket_LogInUser(user, packet);
					break;
					//접속 시 유저 정보 요청 시
				case RecvCommand::C2Zone_REQUIRE_INFO:
					user->SendInfo();
					break;
					//게임 시작 후 첫 존 입장 시 받는 패킷
					//스폰 위치, 타일 지정, 존에 있었다면 해당 존에서는 Exit처리
					//해당 존에 접속 중인 유저들에게 입장했다고 알려줌
				case RecvCommand::C2Zone_TRY_ENTER_FIELD:
					OnPacket_EnterField(user, packet);
					break;
					//접속 중인 유저들 리스트 보내줌
				case RecvCommand::C2Zone_ENTER_FIELD_SUCCESS:
					OnPacket_EnterFieldSuccess(user);
					break;
					//유저 이동
				case RecvCommand::C2Zone_USER_MOVE:
					OnPacket_UpdateUserPosition(user, packet, false);
					break;
					//유저 이동
				case RecvCommand::C2Zone_USER_MOVE_FINISH:
					OnPacket_UpdateUserPosition(user, packet, true);
					break;
					//공격 실패 시(몬스터가 죽었거나 범위 밖으로 나갔을 때)
				case RecvCommand::C2Zone_USER_ATTACK_FAILED:
					OnPacket_UserAttackFailed(user, packet);
					break;
					//공격 성공 시
				case RecvCommand::C2Zone_USER_ATTACK_MONSTER:
					OnPacket_UserAttack(user, packet);
					break;
					//유저 부활 시
				case RecvCommand::C2Zone_USER_REVIVE:
					OnPacket_UserRevive(user);
					break;
					//필드 이동
				case RecvCommand::C2Zone_ENTER_FIELD:
					printf("[ Try To Enter Field ]\n");
					OnPacket_EnterField(user, packet);
					break;
					//필드 이동
				case RecvCommand::C2Zone_ENTER_VILLAGE:
					printf("[ Try To Enter Village ]\n");
					OnPacket_EnterField(user, packet);
					break;
					//접속 끊을 시
				case RecvCommand::C2Zone_EXIT_USER:
					OnPacket_UpdateUser(user, packet);
					user->Disconnect();
					break;
					//HeartBeatCheck 6회 후 유저 업데이트 요청 시
				case RecvCommand::C2Zone_UPDATE_INFO:
					OnPacket_UpdateUser(user, packet);
					break;
				case RecvCommand::C2Zone_ENTER_TEST_USER:
					OnPacket_EnterTestUser(user, packet);
					break;
				}
			}

			tempNum--;

			if (tempNum <= 0)
			{
				tempNum = 10;

				break;
			}
		}
	}
	*/

	//for_each
	/*list<Session*>::iterator iterBegin = vSessionList->begin();
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
						//HeartBeatChecked 성공 패킷 받을 시
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
	*/

	list<Session*>::iterator iterEnd = vSessionList->end();

	for (list<Session*>::iterator iter = vSessionList->begin(); iter != iterEnd;)
	{
		user = dynamic_cast<User*>(*iter);
		++iter;

		if (!user->GetIsConnected())
		{
			//존에 있었다면 해당 존에서 먼저 나간 후
			if (user->GetField() != nullptr)
			{
				user->GetField()->ExitUser(user);
			}

			//shutdown 이후 close
			closesocket(user->GetSocket());

			//세션매니저에서 유저를 삭제해줌과 동시에 오브젝트풀에 반환해준다.
			m_sessionManager->DeleteSession(user);
			m_sessionManager->DeleteSessionID(user->GetInfo()->userInfo.userID);

			continue;
		}

		int tempNum = 10;

		while (1)
		{
			Packet* packet = reinterpret_cast<Packet*>(user->GetRecvBuffer()->CanParsing());

			if (packet == nullptr) break;

			//테스트용 패킷 처리
			if (packet->cmd == 12345)
			{
				printf("Test Checking \n");
			}
			else
			{
				switch (static_cast<RecvCommand>(packet->cmd))
				{
					//채팅 패킷
					//바로 해당 존에 SendAll
				case RecvCommand::C2Zone_CHATTING:
					user->GetField()->
						SectorSendAll(user->GetSector()->GetRoundSectorsVec(),
							reinterpret_cast<char*>(packet), packet->size);
					break;
					//HeartBeatChecked 성공 패킷 받을 시
				case RecvCommand::C2Zone_CHECK_ALIVE:
					user->HeartBeatChecked();
					break;
					//회원가입 시도시
				case RecvCommand::C2Zone_REGISTER_ACCOUNT:
					OnPacket_RegisterUser(user, packet);
					break;
					//로그인 시도시
				case RecvCommand::C2Zone_LOGIN:
					OnPacket_LogInUser(user, packet);
					break;
					//접속 시 유저 정보 요청 시
				case RecvCommand::C2Zone_REQUIRE_INFO:
					user->RequestUserInfo();
					break;
					//게임 시작 후 첫 존 입장 시 받는 패킷
					//스폰 위치, 타일 지정, 존에 있었다면 해당 존에서는 Exit처리
					//해당 존에 접속 중인 유저들에게 입장했다고 알려줌
				case RecvCommand::C2Zone_TRY_ENTER_FIELD:
					OnPacket_EnterField(user, packet);
					break;
					//접속 중인 유저들 리스트 보내줌
				case RecvCommand::C2Zone_ENTER_FIELD_SUCCESS:
					OnPacket_EnterFieldSuccess(user);
					break;
					//유저 이동
				case RecvCommand::C2Zone_USER_MOVE:
					OnPacket_UpdateUserPosition(user, packet, false);
					break;
					//유저 이동
				case RecvCommand::C2Zone_USER_MOVE_FINISH:
					OnPacket_UpdateUserPosition(user, packet, true);
					break;
					//공격 실패 시(몬스터가 죽었거나 범위 밖으로 나갔을 때)
				case RecvCommand::C2Zone_USER_ATTACK_FAILED:
					OnPacket_UserAttackFailed(user, packet);
					break;
					//공격 성공 시
				case RecvCommand::C2Zone_USER_ATTACK_MONSTER:
					OnPacket_UserAttack(user, packet);
					break;
					//유저 부활 시
				case RecvCommand::C2Zone_USER_REVIVE:
					OnPacket_UserRevive(user);
					break;
					//필드 이동
				case RecvCommand::C2Zone_ENTER_FIELD:
					printf("[ Try To Enter Field ]\n");
					OnPacket_EnterField(user, packet);
					break;
					//필드 이동
				case RecvCommand::C2Zone_ENTER_VILLAGE:
					printf("[ Try To Enter Village ]\n");
					OnPacket_EnterField(user, packet);
					break;
					//접속 끊을 시
				case RecvCommand::C2Zone_EXIT_USER:
					OnPacket_UpdateUser(user, packet);
					user->Disconnect();
					break;
					//HeartBeatCheck 6회 후 유저 업데이트 요청 시
				case RecvCommand::C2Zone_UPDATE_INFO:
					OnPacket_UpdateUser(user, packet);
					break;
				case RecvCommand::C2Zone_ENTER_TEST_USER:
					OnPacket_EnterTestUser(user, packet);
					break;
				}
			}

			tempNum--;

			if (tempNum <= 0)
			{
				tempNum = 10;

				break;
			}
		}
	}
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
		case SendCommand::Zone2C_MONSTER_ATTACK:
			OnPacket_MonsterAttack(monster, packet);
			break;
		default:
			/*printf("=======================================\n");
			printf("Monster %d : ", monster->GetInfo().index);*/
			monster->GetField()->
				SectorSendAll(monster->GetSector()->GetRoundSectorsVec(), reinterpret_cast<char*>(packet), packet->size);
			break;
		}
	}
}

void ServerLogicThread::ParsingConnector()
{
	//if (!Connector->GetIsConnected())
	//{
	//	//재접속
	//	Connector->Connect();
	//}

	int tempNum = 10;

	while (1)
	{
		Packet* packet = reinterpret_cast<Packet*>(CONNECTOR->GetRecvBuffer()->CanParsing());

		if (packet == nullptr) break;

		switch (static_cast<RecvCommand>(packet->cmd))
		{
		case RecvCommand::DB2Zone_LOGIN_FAILED_INVALID_ID:
			OnPacket_LogInFailed(packet);
			break;
		case RecvCommand::DB2Zone_LOGIN_FAILED_WRONG_PASSWORD:
			OnPacket_LogInFailed(packet);
			break;
		case RecvCommand::DB2Zone_LOGIN_SUCCESS:
			OnPacket_LogInSuccess(packet);
			break;
		case RecvCommand::DB2Zone_REGISTER_FAILED:
			OnPacket_RegisterFailed(packet);
			break;
		case RecvCommand::DB2Zone_REGISTER_SUCCESS:
			OnPacket_RegisterSuccess(packet);
			break;
		case RecvCommand::DB2Zone_GET_USER_DATA_FAILED:
			OnPacket_GetUserInfoFailed(packet);
			break;
		case RecvCommand::DB2Zone_GET_USER_DATA_SUCCESS:
			OnPacket_GetUserInfoSuccess(packet);
			break;
		case RecvCommand::DB2Zone_MONSTERS_DATA:
			CONNECTOR->GetMonstersData(packet);
			m_fieldManager->InitMonsterThread();
			break;
		case RecvCommand::DB2Zone_UPDATE_USER_FAILED:
			OnPacket_UpdateUserFailed(packet);
			break;
		case RecvCommand::DB2Zone_UPDATE_USER_SUCCESS:
			OnPacket_UpdateUserSuccess(packet);
			break;
		}

		tempNum--;

		if (tempNum <= 0)
		{
			tempNum = 10;

			break;
		}
	}
}

void ServerLogicThread::OnPacket_EnterField(User* _user, Packet* _packet)
{
	FieldNumPacket* fieldNumPacket = static_cast<FieldNumPacket*>(_packet);

	Field* field = m_fieldManager->GetField(fieldNumPacket->fieldNum);
	if (field == nullptr) return;

	Field* prevField = m_fieldManager->GetField(_user->GetInfo()->unitInfo.fieldNum);
	if (prevField != nullptr)
	{
		printf("[ Exit User (Prev Field) ]");
		prevField->ExitUser(_user);
	}

	field->EnterUser(_user);
}

void ServerLogicThread::OnPacket_EnterFieldSuccess(User* _user)
{
	_user->StartCheckingHeartBeat();

	Field* field = _user->GetField();

	field->SendUserList(_user);
}

void ServerLogicThread::OnPacket_UpdateUserPosition(User* _user, Packet* _packet, bool _isFinish)
{
	UserPositionPacket* userPositionPacket = static_cast<UserPositionPacket*>(_packet);

	if (!_isFinish && _user->GetInfo()->unitInfo.state == STATE::IDLE) _user->SetState(STATE::MOVE);
	else if(_isFinish) _user->SetState(STATE::IDLE);

	Field* field = _user->GetField();

	_user->SetPosition(userPositionPacket->position);

	field->UpdateUserSector(_user);

	field->SectorSendAll(_user->GetSector()->GetRoundSectorsVec(), reinterpret_cast<char*>(_packet), _packet->size);
}

void ServerLogicThread::OnPacket_UserAttackFailed(User* _user, Packet* _packet)
{
	UserAttackPacket * userAttackPacket = static_cast<UserAttackPacket*>(_packet);

	Field* field = _user->GetField();
	field->SectorSendAll( _user->GetSector()->GetRoundSectorsVec(), reinterpret_cast<char*>(_packet), _packet->size);
}

void ServerLogicThread::OnPacket_UserAttack(User* _user, Packet* _packet)
{
	UserAttackPacket * userAttackPacket = static_cast<UserAttackPacket*>(_packet);

	Field* field = _user->GetField();

	if (field->UserAttack(_user, userAttackPacket->monsterIndex))
	{
		field->SectorSendAll(_user->GetSector()->GetRoundSectorsVec(), reinterpret_cast<char*>(_packet), _packet->size);
	}
}

void ServerLogicThread::OnPacket_UserRevive(User* _user)
{
	Field* field = _user->GetField();

	field->RespawnUser(_user);
}
 
void ServerLogicThread::OnPacket_MonsterAttack(Monster* _monster, Packet* _packet)
{
	MonsterAttackPacket * monsterAttackPacket = static_cast<MonsterAttackPacket*>(_packet);

	User* user = _monster->GetTarget();
	if (user == nullptr) return;

	user->Hit(monsterAttackPacket->monsterIndex, monsterAttackPacket->damage);
}

void ServerLogicThread::OnPacket_RegisterUser(User* _user, Packet* _packet)
{
	RegisterUserPacket* registerUserPacket = static_cast<RegisterUserPacket*>(_packet);

	_user->RegisterUser(registerUserPacket);
}

void ServerLogicThread::OnPacket_LogInUser(User* _user, Packet* _packet)
{
	LogInPacket* logInPacket = static_cast<LogInPacket*>(_packet);

	_user->LogInUser(logInPacket);

	/*if (_user->LogInUser(logInPacket))
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
	}*/
}

void ServerLogicThread::OnPacket_UpdateUser(User* _user, Packet* _packet)
{
	SessionInfoPacket* sessionInfoPacket = static_cast<SessionInfoPacket*>(_packet);

	_user->GetInfo()->unitInfo = sessionInfoPacket->info.unitInfo;
	_user->GetInfo()->userInfo = sessionInfoPacket->info.userInfo;

	_user->UpdateInfo();
}

void ServerLogicThread::OnPacket_LogInSuccess(Packet* _packet)
{
	LogInSuccessPacket* logInSuccessPacket = static_cast<LogInSuccessPacket*>(_packet);
	
	User* tempUser;
	tempUser = dynamic_cast<User*>(FindUser(logInSuccessPacket->socket));

	if (tempUser != nullptr)
	{
		//중복 로그인체크
		if (m_sessionManager->FindSessionID(tempUser->GetInfo()->userInfo.userID))
		{
			//중복 로그인이니 접속하지마라
			tempUser->LogInDuplicated();

			return;
		}

		m_sessionManager->AddSessionID(tempUser->GetInfo()->userInfo.userID);

		tempUser->LogInSuccess(logInSuccessPacket->userIndex);
	}
}

void ServerLogicThread::OnPacket_LogInFailed(Packet* _packet)
{
	PacketWithSocket* packetWithSocket = static_cast<PacketWithSocket*>(_packet);

	User* tempUser;
	tempUser = dynamic_cast<User*>(FindUser(packetWithSocket->socket));

	if (tempUser != nullptr)
	{
		tempUser->LogInFailed();
	}
}

void ServerLogicThread::OnPacket_RegisterSuccess(Packet* _packet)
{
	PacketWithSocket* packetWithSocket = static_cast<PacketWithSocket*>(_packet);

	User* tempUser;
	tempUser = dynamic_cast<User*>(FindUser(packetWithSocket->socket));

	if (tempUser != nullptr)
	{
		tempUser->RegisterSuccess();
	}
}

void ServerLogicThread::OnPacket_RegisterFailed(Packet* _packet)
{
	PacketWithSocket* packetWithSocket = static_cast<PacketWithSocket*>(_packet);

	User* tempUser;
	tempUser = dynamic_cast<User*>(FindUser(packetWithSocket->socket));

	if (tempUser != nullptr)
	{
		tempUser->RegisterFailed();
	}
}

void ServerLogicThread::OnPacket_GetUserInfoSuccess(Packet* _packet)
{
	GetSessionInfoPacket* getSessionInfoPacket = static_cast<GetSessionInfoPacket*>(_packet);

	User* tempUser;
	tempUser = dynamic_cast<User*>(FindUser(getSessionInfoPacket->socket));

	if (tempUser != nullptr)
	{
		tempUser->SendInfo(getSessionInfoPacket);
	}
}

void ServerLogicThread::OnPacket_GetUserInfoFailed(Packet* _packet)
{
	PacketWithSocket* packetWithSocket = static_cast<PacketWithSocket*>(_packet);

	User* tempUser;
	tempUser = dynamic_cast<User*>(FindUser(packetWithSocket->socket));

	if (tempUser != nullptr)
	{
		tempUser->RequestUserInfoFailed();
	}
}

void ServerLogicThread::OnPacket_UpdateUserSuccess(Packet* _packet)
{
	PacketWithSocket* packetWithSocket = static_cast<PacketWithSocket*>(_packet);

	User* tempUser;
	tempUser = dynamic_cast<User*>(FindUser(packetWithSocket->socket));

	if (tempUser != nullptr)
	{
		printf("[ %d user - INFO UPDATE TO DATABASE SUCCESS ] \n", tempUser->GetInfo()->userInfo.userID);
	}
}

void ServerLogicThread::OnPacket_UpdateUserFailed(Packet* _packet)
{
	PacketWithSocket* packetWithSocket = static_cast<PacketWithSocket*>(_packet);

	User* tempUser;
	tempUser = dynamic_cast<User*>(FindUser(packetWithSocket->socket));

	if (tempUser != nullptr)
	{
		//재시도
		tempUser->UpdateInfo();
	}
}

Session* ServerLogicThread::FindUser(SOCKET _socket)
{
	list<Session*>* vSessionList = m_sessionManager->GetItemList();

	for (const auto& element : *vSessionList)
	{
		if (element->GetSocket() == _socket)
		{
			return element;
		}
	}

	return nullptr;
}

void ServerLogicThread::OnPacket_EnterTestUser(User* _user, Packet* _packet)
{
	TestClientEnterPacket* testClientEnterPacket = static_cast<TestClientEnterPacket*>(_packet);

	Field* field = m_fieldManager->GetField(testClientEnterPacket->fieldNum);

	field->EnterTestClient(_user, testClientEnterPacket->userNum);

	//_user->StartCheckingHeartBeat();
}