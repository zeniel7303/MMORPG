#include "PacketHandler.h"

#include "DBConnector.h"

PacketHandler::~PacketHandler()
{
}

void PacketHandler::HandleUserPacket(User* _user, Packet* _packet)
{
	Packet* packet = _packet;
	User* user = _user;

	(this->*userPacketFunc[packet->cmd])(user, packet);
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
		monster->GetField()->
			SectorSendAll(monster->GetSector()->GetRoundSectorsVec(), reinterpret_cast<char*>(packet), packet->size);
		break;
	}
}

void PacketHandler::HandleDBConnectorPacket(Packet* _packet)
{
	Packet* packet = _packet;

	(this->*dbPacketFunc[packet->cmd % 100])(packet);
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

void PacketHandler::OnPacket_EnterFieldSuccess(User* _user, Packet* _packet)
{
	Field* field = _user->GetField();

	field->SendUserList(_user);
}

void PacketHandler::OnPacket_UpdateUserPosition(User* _user, Packet* _packet)
{
	UserPositionPacket* userPositionPacket = reinterpret_cast<UserPositionPacket*>(_packet);

	if (_user->GetInfo()->unitInfo.state == STATE::IDLE)
	{
		_user->SetState(STATE::MOVE);
	}

	Field* field = _user->GetField();

	_user->SetPosition(userPositionPacket->position);

	field->UpdateUserSector(_user);

	field->SectorSendAll(_user->GetSector()->GetRoundSectorsVec(), reinterpret_cast<char*>(_packet), _packet->size);
}

void PacketHandler::OnPacket_UpdateUserPosition_Finish(User* _user, Packet* _packet)
{
	UserPositionPacket* userPositionPacket = reinterpret_cast<UserPositionPacket*>(_packet);

	_user->SetState(STATE::IDLE);

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

void PacketHandler::OnPacket_UserRevive(User* _user, Packet* _packet)
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

void PacketHandler::OnPacket_RequireInfo(User* _user, Packet* _packet)
{
	_user->RequestUserInfo();
}

void PacketHandler::OnPacket_UpdateUser(User* _user, Packet* _packet)
{
	SessionInfoPacket* sessionInfoPacket = reinterpret_cast<SessionInfoPacket*>(_packet);

	_user->GetInfo()->unitInfo = sessionInfoPacket->info.unitInfo;
	_user->GetInfo()->userInfo = sessionInfoPacket->info.userInfo;

	_user->UpdateInfo();
}

void PacketHandler::OnPacket_ExitUser(User* _user, Packet* _packet)
{
	OnPacket_UpdateUser(_user, _packet);

	_user->DisConnect();
}

void PacketHandler::OnPacket_Chatting(User* _user, Packet* _packet)
{
	_user->GetField()->
		SectorSendAll(_user->GetSector()->GetRoundSectorsVec(),
			reinterpret_cast<char*>(_packet), _packet->size);
}

void PacketHandler::OnPacket_HeartBeat(User* _user, Packet* _packet)
{
	_user->HeartBeatChecked();
}

void PacketHandler::OnPacket_LogInSuccess(Packet* _packet)
{
	LogInSuccessPacket* logInSuccessPacket = reinterpret_cast<LogInSuccessPacket*>(_packet);

	User* tempUser;
	tempUser = dynamic_cast<User*>(FindUser(logInSuccessPacket->socket));

	if (tempUser != nullptr)
	{
		//중복 로그인체크(나중에 DB에서 프로시져를 이용해서 하자)
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

void PacketHandler::OnPakcet_GetMonstersData(Packet* _packet)
{
	DBCONNECTOR->GetMonstersData(_packet);
	m_fieldManager.InitMonsterThread();
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
}