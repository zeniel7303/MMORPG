#include "Field.h"

Field::Field(int _num, VECTOR2 _spawnPosition, const char* _name)
{
	m_failed = false;

	m_fieldNum = _num;
	m_spawnPosition = _spawnPosition;

	TRYCATCH_CONSTRUCTOR(m_fieldTilesData = new FieldTilesData(), m_failed);
	if (!m_fieldTilesData->GetMap(_name) || m_failed)
	{
		m_failed = true;

		return;
	}
	TRYCATCH_CONSTRUCTOR(m_sectorManager = new SectorManager(), m_failed);
	if (m_failed) return;
	TRYCATCH_CONSTRUCTOR(m_monsterLogicThread = new MonsterLogicThread(this, m_fieldTilesData, m_sectorManager), m_failed);
	if (m_failed) return;

	TRYCATCH_CONSTRUCTOR(m_sendBuffer = new SendBuffer(10000), m_failed);
	if (m_failed) return;

	m_leaveSectorsVec.resize(9);
	m_enterSectorsVec.resize(9);
}

Field::~Field()
{
	if (m_fieldTilesData != nullptr) delete m_fieldTilesData; 
	if (m_monsterLogicThread != nullptr) delete m_monsterLogicThread;
	if (m_sectorManager != nullptr) delete m_sectorManager;

	if (m_sendBuffer != nullptr) delete m_sendBuffer;

	m_leaveSectorsVec.clear();
	m_leaveSectorsVec.resize(0);

	m_enterSectorsVec.clear();
	m_enterSectorsVec.resize(0);
}

void Field::InitMonsterThread()
{
	if (m_monsterLogicThread->CreateMonsters())
	{
		m_monsterLogicThread->Thread<MonsterLogicThread>::Start(m_monsterLogicThread);
	}
}

void Field::FieldSendAll(char * _buffer, int _size)
{
	for (const auto& element : m_itemList)
	{
		element->Send(_buffer, _size);
	}
}

void Field::SectorSendAll(std::vector<Sector*>* _sectorsVec, char * _buffer, int _size)
{
	std::vector<Sector*> tempVector = *_sectorsVec;

	for (const auto& element_1 : tempVector)
	{
		if (element_1 == nullptr) continue;

		std::list<User*> tempList = *element_1->Manager_List<User>::GetItemList();

		if (tempList.size() <= 0) continue;

		for (const auto& element_2 : tempList)
		{
			if (!element_2->GetIsGetUserList()) continue;

			element_2->Send(_buffer, _size);
		}
	}
}

void Field::SendUserList(User* _user)
{
	UserListPacket* userListPacket =
		reinterpret_cast<UserListPacket*>(_user->GetSendBuffer()->
			GetBuffer(sizeof(UserListPacket)));

	userListPacket->userNum = 0;

	int i = 0;

	for (const auto& element : m_itemList)
	{
		userListPacket->info[i].userInfo = element->GetInfo()->userInfo;
		userListPacket->info[i].position = element->GetInfo()->unitInfo.position;

		i++;
		userListPacket->userNum++;
	}

	userListPacket->size = (sizeof(Info_PacketUse) * userListPacket->userNum)
							+ sizeof(WORD) + sizeof(Packet);
	userListPacket->Init(SendCommand::Zone2C_USER_LIST, userListPacket->size);

	_user->GetSendBuffer()->Write(userListPacket->size);
	_user->Send(reinterpret_cast<char*>(userListPacket), userListPacket->size);

	SendUserList_InRange(_user);

	MYDEBUG("[ USER LIST 전송 완료 ]\n");

	_user->SetIsGetUserList(true);

	m_monsterLogicThread->SendMonsterList(_user);
}

void Field::SendUserList_InRange(User* _user)
{
	int tempNum = 0;

	UserListPacket_Light* userListPacket_InRange =
		reinterpret_cast<UserListPacket_Light*>(_user->GetSendBuffer()->
			GetBuffer(sizeof(UserListPacket_Light)));

	userListPacket_InRange->userNum = 0;

	std::vector<Sector*> tempVec = *_user->GetSector()->GetRoundSectorsVec();

	for (const auto& tempSector : tempVec)
	{
		std::list<User*> tempList = *tempSector->Manager_List<User>::GetItemList();

		if (tempList.size() <= 0) continue;

		for (const auto& tempUser : tempList)
		{
			userListPacket_InRange->info[tempNum].userID = tempUser->GetInfo()->userInfo.userID;
			userListPacket_InRange->info[tempNum].position = tempUser->GetInfo()->unitInfo.position;

			tempNum++;
			userListPacket_InRange->userNum++;
		}
	}

	userListPacket_InRange->size = (sizeof(Info_PacketUser_Light) * userListPacket_InRange->userNum)
		+ sizeof(WORD) + sizeof(Packet);
	userListPacket_InRange->Init(SendCommand::Zone2C_USER_LIST_IN_RANGE, userListPacket_InRange->size);

	_user->GetSendBuffer()->Write(userListPacket_InRange->size);
	_user->Send(reinterpret_cast<char*>(userListPacket_InRange), userListPacket_InRange->size);
}

void Field::EnterUser(User* _user)
{
	if (_user->GetField() != nullptr)
	{
		if (_user->GetField()->GetFieldNum() == 2)
		{	
			VECTOR2 tempVec;
			tempVec.x = 52.0f;
			tempVec.y = 86.0f;

			_user->EnterField(this, m_fieldNum, tempVec);
		}
		else
		{
			_user->EnterField(this, m_fieldNum, m_spawnPosition);
		}
	}
	else
	{
		_user->EnterField(this, m_fieldNum, m_spawnPosition);
	}

	AddItem(_user);

	UpdateUserSector(_user);

	SendEnterUserInfo(_user);

	MYDEBUG("[%d Field : User Insert - %d (접속자 수  : %d) ]\n",
		m_fieldNum, _user->GetInfo()->userInfo.userID, (int)m_itemList.size());
}

void Field::SendEnterUserInfo(User* _user)
{
	SessionInfoPacket* sessionInfoPacket =
		reinterpret_cast<SessionInfoPacket*>(m_sendBuffer->
			GetBuffer(sizeof(SessionInfoPacket)));
	sessionInfoPacket->Init(SendCommand::Zone2C_ENTER_USER_INFO, sizeof(SessionInfoPacket));
	sessionInfoPacket->info.userInfo = _user->GetInfo()->userInfo;
	sessionInfoPacket->info.unitInfo = _user->GetInfo()->unitInfo;
	m_sendBuffer->Write(sessionInfoPacket->size);

	for (const auto& element : m_itemList)
	{
		if (element->GetInfo()->userInfo.userID ==
			sessionInfoPacket->info.userInfo.userID) continue;

		element->Send(reinterpret_cast<char*>(sessionInfoPacket), sessionInfoPacket->size);
	}

	UserNumPacket* userNumPacket_InRange =
		reinterpret_cast<UserNumPacket*>(m_sendBuffer->
			GetBuffer(sizeof(UserNumPacket)));
	userNumPacket_InRange->Init(SendCommand::Zone2C_ENTER_USER_INFO_IN_RANGE, sizeof(UserNumPacket));
	userNumPacket_InRange->userIndex = _user->GetInfo()->userInfo.userID;
	m_sendBuffer->Write(userNumPacket_InRange->size);

	std::vector<Sector*> tempVec = *_user->GetSector()->GetRoundSectorsVec();

	for (const auto& tempSector : tempVec)
	{
		std::list<User*> tempList = *tempSector->Manager_List<User>::GetItemList();

		if (tempList.size() <= 0) continue;

		for (const auto& tempUser : tempList)
		{
			if (tempUser->GetInfo()->userInfo.userID ==
				userNumPacket_InRange->userIndex) continue;

			tempUser->Send(reinterpret_cast<char*>(userNumPacket_InRange), userNumPacket_InRange->size);
		}
	}
}

void Field::ExitUser(User* _user)
{
	DeleteItem(_user);

	_user->GetSector()->Manager_List<User>::DeleteItem(_user);

	SendExitUserInfo(_user->GetInfo()->userInfo.userID);

	MYDEBUG("[%d Field : User Delete - %d (접속자 수  : %d) ]\n",
		m_fieldNum, _user->GetInfo()->userInfo.userID, (int)m_itemList.size());
}

void Field::SendExitUserInfo(int _num)
{
	UserNumPacket* userPacket =
		reinterpret_cast<UserNumPacket*>(m_sendBuffer->
			GetBuffer(sizeof(UserNumPacket)));
	userPacket->Init(SendCommand::Zone2C_LEAVE_USER_INFO, sizeof(UserNumPacket));
	userPacket->userIndex = _num;
	m_sendBuffer->Write(userPacket->size);

	FieldSendAll(reinterpret_cast<char*>(userPacket), userPacket->size);
}

bool Field::UserAttack(User * _user, int _monsterIndex)
{
	Monster* monster = m_monsterLogicThread->GetMonster(_monsterIndex);

	if (monster == nullptr) return false;

	Packet* packet = monster->Hit(_user, _user->GetInfo()->unitInfo.atk);
	if (packet == nullptr) return false;

	SectorSendAll(_user->GetSector()->GetRoundSectorsVec(), reinterpret_cast<char*>(packet), packet->size);

	if (monster->IsDeath())
	{
		_user->PlusExp(monster->GetData().dropExp);
	}

	return true;
}

void Field::RespawnUser(User* _user)
{
	_user->Respawn(m_spawnPosition);

	UpdateUserSector(_user);
}

void Field::UpdateUserSector(User* _user)
{
	Sector* nowSector = m_sectorManager->
		GetSector(_user->GetTile()->GetX(), _user->GetTile()->GetY());

	//true == same, false = different
	if (_user->CompareSector(nowSector))
	{
		//true
		return;
	}

	//false
	//다르니까 섹터 변경 및 전 섹터가 유저가 나간 것을 반영
	Sector* prevSector = _user->GetSector();

	if (prevSector != nullptr)
	{
		/*printf("[ Exit User (Prev Sector) ] %s User : Now Sector : %d\n",
			_user->GetInfo()->userInfo.userName, _user->GetSector()->GetSectorNum());*/

		prevSector->Manager_List<User>::DeleteItem(_user);

		_user->SetSector(nowSector);
		_user->GetSector()->Manager_List<User>::AddItem(_user);

		std::vector<Sector*> prevSectorsNeighbor = *prevSector->GetRoundSectorsVec();
		std::vector<Sector*> nowSectorsNeighbor = *nowSector->GetRoundSectorsVec();

		auto iter1 = std::set_difference(
			prevSectorsNeighbor.begin(), prevSectorsNeighbor.end(),
			nowSectorsNeighbor.begin(), nowSectorsNeighbor.end(),
			m_leaveSectorsVec.begin());

		m_leaveSectorsVec.resize(iter1 - m_leaveSectorsVec.begin());

		auto iter2 = std::set_difference(
			nowSectorsNeighbor.begin(), nowSectorsNeighbor.end(),
			prevSectorsNeighbor.begin(), prevSectorsNeighbor.end(),
			m_enterSectorsVec.begin());

		m_enterSectorsVec.resize(iter2 - m_enterSectorsVec.begin());

		LeaveSector(_user);

		EnterSector(_user);

		m_leaveSectorsVec.resize(9);
		m_enterSectorsVec.resize(9);

		return;
	}

	_user->SetSector(nowSector);
	_user->GetSector()->Manager_List<User>::AddItem(_user);

	return;
}

void Field::LeaveSector(User* _user)
{
	UserNumPacket* userNumPacket_Exit =
		reinterpret_cast<UserNumPacket*>(m_sendBuffer->
			GetBuffer(sizeof(UserNumPacket)));
	userNumPacket_Exit->Init(SendCommand::Zone2C_LEAVE_SECTOR_USER_INFO, sizeof(UserNumPacket));
	userNumPacket_Exit->userIndex = _user->GetInfo()->userInfo.userID;
	m_sendBuffer->Write(userNumPacket_Exit->size);

	//20200516
	//유니티 클라이언트에서 이 패킷을 받지 않아서 유령이 생기는 현상이 발생함. = m_leaveSectorsVec 내에 없었다는 의미
	// -> Visible을 안받게 하면되나?
	//해결

	//자신이 있던 섹터 범위 내의 다른 유저들에게 자신이 나갔다고 알려주는 함수
	SectorSendAll(&m_leaveSectorsVec, reinterpret_cast<char*>(userNumPacket_Exit), userNumPacket_Exit->size);

	//내가 있던 섹터 범위 내의 다른 유저들의 리스트를 보내주는 함수(클라에서 안보이게 하기 위해)
	SendInvisibleUserList(_user);
	
	//내가 있던 섹터 범위 내의 몬스터들의 리스트를 보내주는 함수(클라에서 안보이게 하기 위해)
	SendInvisibleMonsterList(_user);
}

void Field::SendInvisibleUserList(User* _user)
{
	int tempNum = 0;

	UserListPacket_Light* userListPacket_Invisible =
		reinterpret_cast<UserListPacket_Light*>(_user->GetSendBuffer()->
			GetBuffer(sizeof(UserListPacket_Light)));
	userListPacket_Invisible->userNum = 0;

	for (const auto& tempSector : m_leaveSectorsVec)
	{
		if (tempSector == nullptr) break;

		if (tempSector->Manager_List<User>::GetItemList()->size() <= 0) continue;

		std::list<User*> tempList = *tempSector->Manager_List<User>::GetItemList();

		for (const auto& element : tempList)
		{
			if (element == nullptr) break;

			userListPacket_Invisible->info[tempNum].userID = element->GetInfo()->userInfo.userID;
			userListPacket_Invisible->info[tempNum].position = element->GetInfo()->unitInfo.position;

			tempNum++;
			userListPacket_Invisible->userNum++;
		}
	}

	if (userListPacket_Invisible->userNum <= 0) return;

	userListPacket_Invisible->size = (sizeof(Info_PacketUser_Light) * userListPacket_Invisible->userNum)
		+ sizeof(WORD) + sizeof(Packet);
	userListPacket_Invisible->Init(SendCommand::Zone2C_USER_LIST_INVISIBLE, userListPacket_Invisible->size);

	_user->GetSendBuffer()->Write(userListPacket_Invisible->size);
	_user->Send(reinterpret_cast<char*>(userListPacket_Invisible), userListPacket_Invisible->size);
}

void Field::SendInvisibleMonsterList(User* _user)
{
	int tempNum = 0;

	MonsterInfoListPacket* monsterInfoListPacket_Invisible =
		reinterpret_cast<MonsterInfoListPacket*>(_user->GetSendBuffer()->
			GetBuffer(sizeof(MonsterInfoListPacket)));

	monsterInfoListPacket_Invisible->monsterNum = 0;

	for (const auto& tempSector : m_leaveSectorsVec)
	{
		if (tempSector == nullptr) break;

		if (tempSector->Manager_List<Monster>::GetItemList()->size() <= 0) continue;

		std::list<Monster*> tempList = *tempSector->Manager_List<Monster>::GetItemList();

		for (const auto& element : tempList)
		{
			monsterInfoListPacket_Invisible->info[tempNum] = element->GetInfo();

			tempNum++;
			monsterInfoListPacket_Invisible->monsterNum++;
		}
	}

	//printf("Invisible %d \n", monsterInfoListPacket_Invisible->monsterNum);
	monsterInfoListPacket_Invisible->size = (sizeof(MonsterInfo) * monsterInfoListPacket_Invisible->monsterNum)
		+ sizeof(WORD) + sizeof(Packet);
	monsterInfoListPacket_Invisible->Init(SendCommand::Zone2C_MONSTER_INFO_LIST_INVISIBLE, monsterInfoListPacket_Invisible->size);

	_user->GetSendBuffer()->Write(monsterInfoListPacket_Invisible->size);
	_user->Send(reinterpret_cast<char*>(monsterInfoListPacket_Invisible), monsterInfoListPacket_Invisible->size);
}

void Field::EnterSector(User* _user)
{
	UserPositionPacket* userPositionPacket_Enter = 
		reinterpret_cast<UserPositionPacket*>(m_sendBuffer->
			GetBuffer(sizeof(UserPositionPacket)));
	userPositionPacket_Enter->Init(SendCommand::Zone2C_ENTER_SECTOR_USER_INFO, sizeof(UserPositionPacket));
	userPositionPacket_Enter->userIndex = _user->GetInfo()->userInfo.userID;
	userPositionPacket_Enter->position = _user->GetInfo()->unitInfo.position;
	m_sendBuffer->Write(userPositionPacket_Enter->size);

	//내가 들어온 섹터 범위 내의 다른 유저들에게 자신이 들어왔다고 알려주는 함수
	SectorSendAll(&m_enterSectorsVec, reinterpret_cast<char*>(userPositionPacket_Enter), userPositionPacket_Enter->size);

	//내가 들어온 섹터 범위 내의 다른 유저들의 리스트를 보내주는 함수(클라에서 보이게 하기 위해)
	SendVisibleUserList(_user);

	//내가 들어온 섹터 범위 내의 몬스터들의 리스트를 보내주는 함수(클라에서 보이게 하기 위해)
	SendVisibleMonsterList(_user);
}

void Field::SendVisibleUserList(User* _user)
{
	int tempNum = 0;

	UserListPacket_Light* userListPacket_Visible =
		reinterpret_cast<UserListPacket_Light*>(_user->GetSendBuffer()->
			GetBuffer(sizeof(UserListPacket_Light)));
	userListPacket_Visible->userNum = 0;

	for (const auto& tempSector : m_enterSectorsVec)
	{
		if (tempSector == nullptr) break;

		if (tempSector->Manager_List<User>::GetItemList()->size() <= 0) continue;

		std::list<User*> tempList = *tempSector->Manager_List<User>::GetItemList();

		for (const auto& element : tempList)
		{
			if (element == nullptr) break;

			//해당 Sector내의 유저가 이동중일 경우 검증 필요?
			//(유령이 생길 가능성이 가장 큰 곳이라 생각됨)
			/*if (element->GetInfo()->unitInfo.state == STATE::MOVE)
			{
				//어떻게 교차검증?
				for (const auto& roundSector : *_user->GetSector()->GetRoundSectorsVec())
				{
					if (roundSector->Manager_List<User>::FindItem(element))
					{
						userListPacket_Visible->info[tempNum].userID = element->GetInfo()->userInfo.userID;
						userListPacket_Visible->info[tempNum].position = element->GetInfo()->unitInfo.position;

						tempNum++;
						userListPacket_Visible->userNum++;

						break;
					}
				}
			}
			else
			{
				userListPacket_Visible->info[tempNum].userID = element->GetInfo()->userInfo.userID;
				userListPacket_Visible->info[tempNum].position = element->GetInfo()->unitInfo.position;

				tempNum++;
				userListPacket_Visible->userNum++;
			}*/

			userListPacket_Visible->info[tempNum].userID = element->GetInfo()->userInfo.userID;
			userListPacket_Visible->info[tempNum].position = element->GetInfo()->unitInfo.position;

			tempNum++;
			userListPacket_Visible->userNum++;

			/*printf("send %d user, posi : %f, %f \n", element->GetInfo()->userInfo.userID,
				element->GetInfo()->unitInfo.position.x, element->GetInfo()->unitInfo.position.y);*/
		}
	}

	if (userListPacket_Visible->userNum <= 0) return;

	userListPacket_Visible->size = (sizeof(Info_PacketUser_Light) * userListPacket_Visible->userNum)
		+ sizeof(WORD) + sizeof(Packet);
	userListPacket_Visible->Init(SendCommand::Zone2C_USER_LIST_VISIBLE, userListPacket_Visible->size);

	_user->GetSendBuffer()->Write(userListPacket_Visible->size);
	_user->Send(reinterpret_cast<char*>(userListPacket_Visible), userListPacket_Visible->size);
}

void Field::SendVisibleMonsterList(User* _user)
{
	int tempNum = 0;

	MonsterInfoListPacket* monsterInfoListPacket_Visible =
		reinterpret_cast<MonsterInfoListPacket*>(_user->GetSendBuffer()->
			GetBuffer(sizeof(MonsterInfoListPacket)));

	monsterInfoListPacket_Visible->monsterNum = 0;

	for (const auto& tempSector : m_enterSectorsVec)
	{
		if (tempSector == nullptr) break;

		if (tempSector->Manager_List<Monster>::GetItemList()->size() <= 0) continue;

		std::list<Monster*> tempList = *tempSector->Manager_List<Monster>::GetItemList();

		for (const auto& element : tempList)
		{
			monsterInfoListPacket_Visible->info[tempNum] = element->GetInfo();

			tempNum++;
			monsterInfoListPacket_Visible->monsterNum++;
		}
	}

	//printf("Visible %d \n", monsterInfoListPacket_Visible->monsterNum);
	monsterInfoListPacket_Visible->size = (sizeof(MonsterInfo) * monsterInfoListPacket_Visible->monsterNum)
		+ sizeof(WORD) + sizeof(Packet);
	monsterInfoListPacket_Visible->Init(SendCommand::Zone2C_MONSTER_INFO_LIST_VISIBLE, monsterInfoListPacket_Visible->size);

	_user->GetSendBuffer()->Write(monsterInfoListPacket_Visible->size);
	_user->Send(reinterpret_cast<char*>(monsterInfoListPacket_Visible), monsterInfoListPacket_Visible->size);
}

//테스트용
void Field::EnterTestClient(User* _user, int _num)
{
	_user->TestClientEnterField(this, m_fieldNum, _num, m_spawnPosition);

	AddItem(_user);

	UpdateUserSector(_user);

	SendEnterUserInfo(_user);

	MYDEBUG("[%d Field : Test Client Insert - %d (접속자 수  : %d) ]\n",
		m_fieldNum, _user->GetInfo()->userInfo.userID, (int)m_itemList.size());
}