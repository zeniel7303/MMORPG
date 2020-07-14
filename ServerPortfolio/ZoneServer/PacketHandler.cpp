#include "PacketHandler.h"

#include "DBConnector.h"

PacketHandler::~PacketHandler()
{
}

void PacketHandler::HandleUserPacket(User* _user, Packet* _packet)
{
	Packet* packet = _packet;
	User* user = _user;

	switch (static_cast<RecvCommand>(packet->cmd))
	{
		//채팅 패킷
		//바로 해당 존에 SendAll
	case RecvCommand::C2Zone_CHATTING:
		user->GetField()->
			SectorSendAll(_user->GetSector()->GetRoundSectorsVec(),
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
		MYDEBUG("[ Try To Enter Field ]\n");
		OnPacket_EnterField(_user, _packet);
		break;
		//필드 이동
	case RecvCommand::C2Zone_ENTER_VILLAGE:
		MYDEBUG("[ Try To Enter Village ]\n");
		OnPacket_EnterField(user, _packet);
		break;
		//접속 끊을 시
	case RecvCommand::C2Zone_EXIT_USER:
		OnPacket_UpdateUser(user, packet);
		_user->DisConnect();
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

void PacketHandler::HandleMonsterPacket(Monster* _monster, Packet* _packet)
{
	Packet* packet = _packet;
	Monster* monster = _monster;

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

void PacketHandler::HandleDBConnectorPacket(Packet* _packet)
{
	Packet* packet = _packet;

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
		DBCONNECTOR->GetMonstersData(packet);
		m_fieldManager.InitMonsterThread();
		break;
	case RecvCommand::DB2Zone_UPDATE_USER_FAILED:
		OnPacket_UpdateUserFailed(packet);
		break;
	case RecvCommand::DB2Zone_UPDATE_USER_SUCCESS:
		OnPacket_UpdateUserSuccess(packet);
		break;
	}
}

void PacketHandler::OnPacket_EnterField(User* _user, Packet* _packet)
{
	FieldNumPacket* fieldNumPacket = reinterpret_cast<FieldNumPacket*>(_packet);

	Field* field = m_fieldManager.GetField(fieldNumPacket->fieldNum);
	if (field == nullptr) return;

	Field* prevField = m_fieldManager.GetField(_user->GetInfo()->unitInfo.fieldNum);
	if (prevField != nullptr)
	{
		MYDEBUG("[ Exit User (Prev Field) ]");
		prevField->ExitUser(_user);
	}

	field->EnterUser(_user);
}

void PacketHandler::OnPacket_EnterFieldSuccess(User* _user)
{
	_user->StartCheckingHeartBeat();

	Field* field = _user->GetField();

	field->SendUserList(_user);
}

void PacketHandler::OnPacket_UpdateUserPosition(User* _user, Packet* _packet, bool _isFinish)
{
	UserPositionPacket* userPositionPacket = reinterpret_cast<UserPositionPacket*>(_packet);

	if (!_isFinish && _user->GetInfo()->unitInfo.state == STATE::IDLE) _user->SetState(STATE::MOVE);
	else if (_isFinish) _user->SetState(STATE::IDLE);

	Field* field = _user->GetField();

	_user->SetPosition(userPositionPacket->position);

	field->UpdateUserSector(_user);

	field->SectorSendAll(_user->GetSector()->GetRoundSectorsVec(), reinterpret_cast<char*>(_packet), _packet->size);
}

void PacketHandler::OnPacket_UserAttackFailed(User* _user, Packet* _packet)
{
	UserAttackPacket * userAttackPacket = reinterpret_cast<UserAttackPacket*>(_packet);

	Field* field = _user->GetField();
	field->SectorSendAll(_user->GetSector()->GetRoundSectorsVec(), reinterpret_cast<char*>(_packet), _packet->size);
}

void PacketHandler::OnPacket_UserAttack(User* _user, Packet* _packet)
{
	UserAttackPacket * userAttackPacket = reinterpret_cast<UserAttackPacket*>(_packet);

	Field* field = _user->GetField();

	if (field->UserAttack(_user, userAttackPacket->monsterIndex))
	{
		field->SectorSendAll(_user->GetSector()->GetRoundSectorsVec(), reinterpret_cast<char*>(_packet), _packet->size);
	}
}

void PacketHandler::OnPacket_UserRevive(User* _user)
{
	Field* field = _user->GetField();

	field->RespawnUser(_user);
}

void PacketHandler::OnPacket_MonsterAttack(Monster* _monster, Packet* _packet)
{
	MonsterAttackPacket * monsterAttackPacket = reinterpret_cast<MonsterAttackPacket*>(_packet);

	User* user = _monster->GetTarget();
	if (user == nullptr) return;

	user->Hit(monsterAttackPacket->monsterIndex, monsterAttackPacket->damage);
}

void PacketHandler::OnPacket_RegisterUser(User* _user, Packet* _packet)
{
	RegisterUserPacket* registerUserPacket = reinterpret_cast<RegisterUserPacket*>(_packet);

	_user->RegisterUser(registerUserPacket);
}

void PacketHandler::OnPacket_LogInUser(User* _user, Packet* _packet)
{
	LogInPacket* logInPacket = reinterpret_cast<LogInPacket*>(_packet);

	_user->LogInUser(logInPacket);
}

void PacketHandler::OnPacket_UpdateUser(User* _user, Packet* _packet)
{
	SessionInfoPacket* sessionInfoPacket = reinterpret_cast<SessionInfoPacket*>(_packet);

	_user->GetInfo()->unitInfo = sessionInfoPacket->info.unitInfo;
	_user->GetInfo()->userInfo = sessionInfoPacket->info.userInfo;

	_user->UpdateInfo();
}

void PacketHandler::OnPacket_LogInSuccess(Packet* _packet)
{
	LogInSuccessPacket* logInSuccessPacket = reinterpret_cast<LogInSuccessPacket*>(_packet);

	User* tempUser;
	tempUser = dynamic_cast<User*>(FindUser(logInSuccessPacket->socket));

	if (tempUser != nullptr)
	{
		//중복 로그인체크
		if (m_sessionManager.FindSessionID(tempUser->GetInfo()->userInfo.userID))
		{
			//중복 로그인이니 접속하지마라
			tempUser->LogInDuplicated();

			return;
		}

		m_sessionManager.AddSessionID(tempUser->GetInfo()->userInfo.userID);

		tempUser->LogInSuccess(logInSuccessPacket->userIndex);
	}
}

void PacketHandler::OnPacket_LogInFailed(Packet* _packet)
{
	PacketWithSocket* packetWithSocket = reinterpret_cast<PacketWithSocket*>(_packet);

	User* tempUser;
	tempUser = dynamic_cast<User*>(FindUser(packetWithSocket->socket));

	if (tempUser != nullptr)
	{
		tempUser->LogInFailed();
	}
}

void PacketHandler::OnPacket_RegisterSuccess(Packet* _packet)
{
	PacketWithSocket* packetWithSocket = reinterpret_cast<PacketWithSocket*>(_packet);

	User* tempUser;
	tempUser = dynamic_cast<User*>(FindUser(packetWithSocket->socket));

	if (tempUser != nullptr)
	{
		tempUser->RegisterSuccess();
	}
}

void PacketHandler::OnPacket_RegisterFailed(Packet* _packet)
{
	PacketWithSocket* packetWithSocket = reinterpret_cast<PacketWithSocket*>(_packet);

	User* tempUser;
	tempUser = dynamic_cast<User*>(FindUser(packetWithSocket->socket));

	if (tempUser != nullptr)
	{
		tempUser->RegisterFailed();
	}
}

void PacketHandler::OnPacket_GetUserInfoSuccess(Packet* _packet)
{
	GetSessionInfoPacket* getSessionInfoPacket = reinterpret_cast<GetSessionInfoPacket*>(_packet);

	User* tempUser;
	tempUser = dynamic_cast<User*>(FindUser(getSessionInfoPacket->socket));

	if (tempUser != nullptr)
	{
		tempUser->SendInfo(getSessionInfoPacket);
	}
}

void PacketHandler::OnPacket_GetUserInfoFailed(Packet* _packet)
{
	PacketWithSocket* packetWithSocket = reinterpret_cast<PacketWithSocket*>(_packet);

	User* tempUser;
	tempUser = dynamic_cast<User*>(FindUser(packetWithSocket->socket));

	if (tempUser != nullptr)
	{
		tempUser->RequestUserInfoFailed();
	}
}

void PacketHandler::OnPacket_UpdateUserSuccess(Packet* _packet)
{
	PacketWithSocket* packetWithSocket = reinterpret_cast<PacketWithSocket*>(_packet);

	User* tempUser;
	tempUser = dynamic_cast<User*>(FindUser(packetWithSocket->socket));

	if (tempUser != nullptr)
	{
		MYDEBUG("[ %d user - INFO UPDATE TO DATABASE SUCCESS ] \n", tempUser->GetInfo()->userInfo.userID);
	}
}

void PacketHandler::OnPacket_UpdateUserFailed(Packet* _packet)
{
	PacketWithSocket* packetWithSocket = reinterpret_cast<PacketWithSocket*>(_packet);

	User* tempUser;
	tempUser = dynamic_cast<User*>(FindUser(packetWithSocket->socket));

	if (tempUser != nullptr)
	{
		//재시도
		tempUser->UpdateInfo();
	}
}

Session* PacketHandler::FindUser(SOCKET _socket)
{
	const list<Session*> vSessionList = m_sessionManager.GetItemList();

	for (const auto& element : vSessionList)
	{
		if (element->GetSocket() == _socket)
		{
			return element;
		}
	}

	return nullptr;
}

void PacketHandler::OnPacket_EnterTestUser(User* _user, Packet* _packet)
{
	TestClientEnterPacket* testClientEnterPacket = reinterpret_cast<TestClientEnterPacket*>(_packet);

	Field* field = m_fieldManager.GetField(testClientEnterPacket->fieldNum);

	field->EnterTestClient(_user, testClientEnterPacket->userNum);

	//_user->StartCheckingHeartBeat();
}