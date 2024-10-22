#include "PacketHandler.h"

#include "DBConnector.h"
#include "LogInConnector.h"
#include "PathFinderAgent.h"

#include "MonsterTable.h"

PacketHandler::~PacketHandler()
{
}

void PacketHandler::HandleUserPacket(User* _user, Packet* _packet)
{
	Packet* packet = _packet;
	User* user = _user;

	(this->*userPacketFunc[packet->cmd])(user, packet);

	user->GetReceiver()->GetRingBuffer()->Read(packet->size);
}

void PacketHandler::HandleDBConnectorPacket(Packet* _packet)
{
	Packet* packet = _packet;

	(this->*dbPacketFunc[packet->cmd % 100])(packet);

	DBConnector::getSingleton()->GetReceiver()->GetRingBuffer()->Read(packet->size);
}

void PacketHandler::HandleLogInServerPacket(Packet* _packet)
{
	Packet* packet = _packet;

	(this->*logInServerPacketFunc[packet->cmd % 200])(packet);

	LogInConnector::getSingleton()->GetReceiver()->GetRingBuffer()->Read(packet->size);
}

void PacketHandler::HandlePathFindAgentPacket(Packet* _packet)
{
	Packet* packet = _packet;
	
	(this->*pathFindAgentPacketFunc[packet->cmd % 500])(packet);

	PathFinderAgent::getSingleton()->GetReceiver()->GetRingBuffer()->Read(packet->size);
}

void PacketHandler::OnPacket_AuthenticationUser(User* _user, Packet* _packet)
{
	LogInSuccessPacket* logInSuccessPacket = reinterpret_cast<LogInSuccessPacket*>(_packet);

	_user->GetInfo()->userInfo.userID = logInSuccessPacket->userIndex;

	User* tempUser;
	tempUser = FindUserAtHashMap_socket(logInSuccessPacket->socket);

	if (tempUser != nullptr)
	{
		MYDEBUG("[ %d User Is Duplicated (Abnormal Connect) ]\n",
			tempUser->GetInfo()->userInfo.userID);

		_user->DisConnect();

		return;
	}

	AuthenticationPacket* authenticationPacket = new AuthenticationPacket();
	authenticationPacket->Init(SendCommand::Zone2Login_AUTHENTICATION, sizeof(AuthenticationPacket));
	authenticationPacket->userIndex = logInSuccessPacket->userIndex;
	authenticationPacket->socket = _user->GetSocket();

	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(authenticationPacket));
	LogInConnector::getSingleton()->AddToSendQueue(tempPtr);

	MainThread::getSingleton()->AddToHashMapQueue(_user);
}

void PacketHandler::OnPacket_RequestInfo(User* _user, Packet* _packet)
{
	LogInSuccessPacket* logInSuccessPacket = reinterpret_cast<LogInSuccessPacket*>(_packet);

	_user->RequestUserInfo(logInSuccessPacket->userIndex);
}

void PacketHandler::OnPacket_UpdateUser(User* _user, Packet* _packet)
{
	SessionInfoPacket* sessionInfoPacket = reinterpret_cast<SessionInfoPacket*>(_packet);

	Position tempPosition = _user->GetInfo()->unitInfo.position;

	_user->GetInfo()->unitInfo = sessionInfoPacket->info.unitInfo;
	_user->GetInfo()->userInfo = sessionInfoPacket->info.userInfo;
	_user->GetInfo()->unitInfo.position = tempPosition;

	_user->UpdateInfo();
}

void PacketHandler::OnPacket_EnterField(User* _user, Packet* _packet)
{
	//MYDEBUG("Check \n");

	FieldNumPacket* fieldNumPacket = reinterpret_cast<FieldNumPacket*>(_packet);

	Field* field = m_fieldManager.GetField(fieldNumPacket->fieldNum);
	if (field == nullptr) return;

	Field* prevField = m_fieldManager.GetField(_user->GetInfo()->unitInfo.fieldNum);
	if (prevField != nullptr)
	{
		MYDEBUG("[ Exit User (Prev Field) ]\n");
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

	UserPositionPacket* userPositionPacket_temp = new UserPositionPacket;
	userPositionPacket_temp->userIndex = userPositionPacket->userIndex;
	userPositionPacket_temp->position = userPositionPacket->position;
	userPositionPacket_temp->Init(SendCommand::Zone2C_USER_MOVE, sizeof(UserPositionPacket));
	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(userPositionPacket_temp));

	if (_user->GetInfo()->unitInfo.state == STATE::IDLE)
	{
		_user->SetState(STATE::MOVE);
	}

	Field* field = _user->GetField();

	_user->SetPosition(userPositionPacket->position);

	field->UpdateUserSector(_user);

	field->SectorSendAll(_user->GetSector()->GetRoundSectorsVec(), tempPtr);
}

void PacketHandler::OnPacket_UpdateUserPosition_Finish(User* _user, Packet* _packet)
{
	UserPositionPacket* userPositionPacket = reinterpret_cast<UserPositionPacket*>(_packet);

	UserPositionPacket* userPositionPacket_temp = new UserPositionPacket;
	userPositionPacket_temp->userIndex = userPositionPacket->userIndex;
	userPositionPacket_temp->position = userPositionPacket->position;
	userPositionPacket_temp->Init(SendCommand::Zone2C_USER_MOVE_FINISH, sizeof(UserPositionPacket));
	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(userPositionPacket_temp));

	_user->SetState(STATE::IDLE);

	Field* field = _user->GetField();

	_user->SetPosition(userPositionPacket->position);

	field->UpdateUserSector(_user);

	field->SectorSendAll(_user->GetSector()->GetRoundSectorsVec(), tempPtr);
}

void PacketHandler::OnPacket_UserAttackFailed(User* _user, Packet* _packet)
{
	UserAttackPacket * userAttackPacket = reinterpret_cast<UserAttackPacket*>(_packet);

	UserAttackPacket* userAttackPacket_temp = new UserAttackPacket;
	userAttackPacket_temp->monsterIndex = userAttackPacket->monsterIndex;
	userAttackPacket_temp->position = userAttackPacket->position;
	userAttackPacket_temp->userIndex = userAttackPacket->userIndex;
	userAttackPacket_temp->Init(SendCommand::Zone2C_USER_ATTACK_FAILED, sizeof(UserAttackPacket));
	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(userAttackPacket_temp));

	Field* field = _user->GetField();
	field->SectorSendAll(_user->GetSector()->GetRoundSectorsVec(), tempPtr);
}

void PacketHandler::OnPacket_UserAttack(User* _user, Packet* _packet)
{
	UserAttackPacket * userAttackPacket = reinterpret_cast<UserAttackPacket*>(_packet);

	UserAttackPacket* userAttackPacket_temp = new UserAttackPacket;
	userAttackPacket_temp->monsterIndex = userAttackPacket->monsterIndex;
	userAttackPacket_temp->position = userAttackPacket->position;
	userAttackPacket_temp->userIndex = userAttackPacket->userIndex;
	userAttackPacket_temp->Init(SendCommand::Zone2C_USER_ATTACK_MONSTER, sizeof(UserAttackPacket));
	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(userAttackPacket_temp));

	Field* field = _user->GetField();

	if (field->UserAttack(_user, userAttackPacket->monsterIndex))
	{
		field->SectorSendAll(_user->GetSector()->GetRoundSectorsVec(), tempPtr);
	}
}

void PacketHandler::OnPacket_UserRevive(User* _user, Packet* _packet)
{
	Field* field = _user->GetField();

	field->RespawnUser(_user);
}

void PacketHandler::OnPacket_ExitUser(User* _user, Packet* _packet)
{
	OnPacket_UpdateUser(_user, _packet);

	_user->DisConnect();
}

void PacketHandler::OnPacket_Chatting(User* _user, Packet* _packet)
{
	ChattingPacket* chattingPacket = reinterpret_cast<ChattingPacket*>(_packet);

	ChattingPacket* chattingPacket_temp = new ChattingPacket;
	strcpy(chattingPacket_temp->chattingBuffer, chattingPacket->chattingBuffer);
	strcpy(chattingPacket_temp->id, chattingPacket->id);
	chattingPacket_temp->userIndex = chattingPacket->userIndex;
	chattingPacket_temp->Init(SendCommand::Zone2C_CHATTING, sizeof(ChattingPacket));
	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(chattingPacket_temp));
	//MYDEBUG("%s \n", chattingPacket->id);

	_user->GetField()->
		SectorSendAll(_user->GetSector()->GetRoundSectorsVec(), tempPtr);
}

void PacketHandler::OnPacket_EnterTestUser(User* _user, Packet* _packet)
{
	TestClientEnterPacket* testClientEnterPacket = reinterpret_cast<TestClientEnterPacket*>(_packet);

	Field* field = m_fieldManager.GetField(testClientEnterPacket->fieldNum);
	if (field == nullptr) return;

	field->EnterTestClient(_user, testClientEnterPacket->userNum);
}

void PacketHandler::OnPacket_Chatting_Whisper(User* _user, Packet* _packet)
{
	ChattingPacket_Whisper* whisperChattingPacket = reinterpret_cast<ChattingPacket_Whisper*>(_packet);

	ChattingPacket_Whisper* whisperChattingPacket_temp = new ChattingPacket_Whisper;
	strcpy(whisperChattingPacket_temp->chattingBuffer, whisperChattingPacket->chattingBuffer);
	strcpy(whisperChattingPacket_temp->id, whisperChattingPacket->id);
	strcpy(whisperChattingPacket_temp->targetId, whisperChattingPacket->targetId);
	whisperChattingPacket_temp->userIndex = whisperChattingPacket->userIndex;
	whisperChattingPacket_temp->Init(SendCommand::Zone2C_CHATTING_WHISPER, sizeof(ChattingPacket_Whisper));
	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(whisperChattingPacket_temp));

	//How to search by value in a Map
	//https://thispointer.com/how-to-search-by-value-in-a-map-c/
	//다른 좋은 방법 없나?
	const std::unordered_map<int, User*> tempHashMap = m_userManager.GetUserHashMap()->GetItemHashMap();
	User* tempUser;

	auto iter = tempHashMap.begin();

	while (iter != tempHashMap.end())
	{
		tempUser = iter->second;

		if (strcmp(tempUser->GetInfo()->userInfo.userName, whisperChattingPacket->targetId) == 0)
		{
			_user->AddToSendQueue(tempPtr);
			tempUser->AddToSendQueue(tempPtr);

			return;
		}

		iter++;
	}

	Packet* whisperFailpacket = new Packet();
	whisperFailpacket->Init(SendCommand::Zone2C_CHATTING_WHISPER_FAIL, sizeof(Packet));

	SharedPointer<char> tempPtr_2 =
		SharedPointer<char>(reinterpret_cast<char*>(whisperFailpacket));
	_user->AddToSendQueue(tempPtr_2);
}

void PacketHandler::OnPacket_GetUserInfoSuccess(Packet* _packet)
{
	GetSessionInfoPacket* getSessionInfoPacket = reinterpret_cast<GetSessionInfoPacket*>(_packet);

	User* tempUser;
	tempUser = FindUserAtHashMap_socket(getSessionInfoPacket->socket);

	if (tempUser != nullptr)
	{
		tempUser->SendInfo(getSessionInfoPacket);
	}
}

void PacketHandler::OnPacket_GetUserInfoFailed(Packet* _packet)
{
	PacketWithSocket* packetWithSocket = reinterpret_cast<PacketWithSocket*>(_packet);

	User* tempUser;
	tempUser = FindUserAtHashMap_socket(packetWithSocket->socket);

	if (tempUser != nullptr)
	{
		tempUser->RequestUserInfoFailed();
	}
}

void PacketHandler::OnPacket_UpdateUserSuccess(Packet* _packet)
{
	PacketWithSocket* packetWithSocket = reinterpret_cast<PacketWithSocket*>(_packet);

	User* tempUser;
	tempUser = FindUserAtHashMap_socket(packetWithSocket->socket);

	if (tempUser != nullptr)
	{
		MYDEBUG("[ %d user - INFO UPDATE TO DATABASE SUCCESS ] \n", tempUser->GetInfo()->userInfo.userID);
	}
}

void PacketHandler::OnPacket_UpdateUserFailed(Packet* _packet)
{
	PacketWithSocket* packetWithSocket = reinterpret_cast<PacketWithSocket*>(_packet);

	User* tempUser;
	tempUser = FindUserAtHashMap_socket(packetWithSocket->socket);

	if (tempUser != nullptr)
	{
		//재시도
		tempUser->UpdateInfo();
	}
}

void PacketHandler::OnPacket_GetMonstersData(Packet* _packet)
{
	MonsterTable::getSingleton()->GetMonstersData(_packet);

	m_fieldManager.InitMonsterThread();
}

void PacketHandler::OnPacket_DBAgentAlive(Packet* _packet)
{
	//MYDEBUG("[ DB Alive ] \n");
	DBConnector::getSingleton()->SetStartTime();
}

void PacketHandler::OnPacket_HelloFromLogInServer(Packet* _packet)
{
	MYDEBUG("[ Connecting With LogInServer Success ]\n");
}

void PacketHandler::OnPacket_DisConnectUser(Packet* _packet)
{
	UserNumPacket* userNumPacket = reinterpret_cast<UserNumPacket*>(_packet);

	const std::unordered_map<int, User*> tempHashMap = m_userManager.GetUserHashMap()->GetItemHashMap();

	auto iter = tempHashMap.find(userNumPacket->userIndex);
	if (iter == tempHashMap.end())
	{
		return;
	}

	//User* tempUser = iter->second;
	//tempUser->DisConnect();
}

void PacketHandler::OnPacket_LoginServerAlive(Packet* _packet)
{
	//MYDEBUG("[ LoginServer Alive ] \n");
	LogInConnector::getSingleton()->SetStartTime();
}

void PacketHandler::OnPacket_AuthenticationSuccess(Packet* _packet)
{
	LogInSuccessPacket* logInSuccessPacket = reinterpret_cast<LogInSuccessPacket*>(_packet);

	User* tempUser;
	tempUser = FindUserAtHashMap_socket(logInSuccessPacket->socket);

	if (tempUser != nullptr)
	{
		MYDEBUG("[ %d User Authentication Success ]\n", 
			tempUser->GetInfo()->userInfo.userID);

		OnPacket_RequestInfo(tempUser, _packet);
	}
}

void PacketHandler::OnPacket_AuthenticationFailed(Packet* _packet)
{
	AuthenticationFailedPacket* authenticationFailedPacket = reinterpret_cast<AuthenticationFailedPacket*>(_packet);

	User* tempUser;
	tempUser = FindUserAtHashMap_socket(authenticationFailedPacket->socket);

	if (tempUser != nullptr)
	{
		tempUser->DisConnect();
	}
}

void PacketHandler::OnPacket_ChangeZone(Packet* _packet)
{
	ChangeZonePacket* changeZonePacket = reinterpret_cast<ChangeZonePacket*>(_packet);

	const std::unordered_map<int, User*> tempHashMap = m_userManager.GetUserHashMap()->GetItemHashMap();

	auto iter = tempHashMap.find(changeZonePacket->userIndex);
	if (iter == tempHashMap.end())
	{
		return;
	}

	User* tempUser = iter->second;
	tempUser->DisConnect_ChangeZone(changeZonePacket->zoneNum);
}

void PacketHandler::OnPacket_PathFindSuccess(Packet* _packet)
{
	PathFindPacket_Success* pathFindSuccessPacket = static_cast<PathFindPacket_Success*>(_packet);

	Field* field = m_fieldManager.GetField(pathFindSuccessPacket->fieldNum);

	Monster* monster = field->GetMonsterManager()->GetMonster(pathFindSuccessPacket->monsterNum);

	monster->PathFindSuccess(pathFindSuccessPacket);
}

void PacketHandler::OnPacket_PathFindFailed(Packet* _packet)
{
	PathFindPacket_Failed* pathFindFailedPacket = static_cast<PathFindPacket_Failed*>(_packet);

	Field* field = m_fieldManager.GetField(pathFindFailedPacket->fieldNum);

	Monster* monster = field->GetMonsterManager()->GetMonster(pathFindFailedPacket->monsterNum);

	monster->PathFindFailed();
}

void PacketHandler::SendHeartBeat_DBAgent()
{
	DBConnector::getSingleton()->HeartBeat();
}

void PacketHandler::SendHeartBeat_LoginServer()
{
	LogInConnector::getSingleton()->HeartBeat();
}

User* PacketHandler::FindUserAtHashMap_socket(SOCKET _socket)
{
	const std::unordered_map<int, User*> tempHashMap = m_userManager.GetUserHashMap()->GetItemHashMap();

	for (const auto& element : tempHashMap)
	{
		if (element.second->GetSocket() == _socket)
		{
			return element.second;
		}
	}

	return nullptr;
}