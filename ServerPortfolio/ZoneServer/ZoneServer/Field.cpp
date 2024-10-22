#include "Field.h"

Field::Field(int _num, VECTOR2* _spawnPosition, const char* _name)
{
	m_failed = false;

	m_fieldNum = _num;
	m_spawnPosition = *_spawnPosition;

	TRYCATCH_CONSTRUCTOR(m_fieldTilesData = new FieldTilesData(), m_failed);
	if (!m_fieldTilesData->GetMap(_name) || m_failed)
	{
		m_failed = true;

		return;
	}
	TRYCATCH_CONSTRUCTOR(m_sectorManager = new SectorManager(), m_failed);
	if (m_failed) return;
	TRYCATCH_CONSTRUCTOR(m_monsterManager = new MonsterManager(this, m_fieldTilesData, m_sectorManager), m_failed);
	if (m_failed) return;

	m_leaveSectorsVec.resize(9);
	m_enterSectorsVec.resize(9);
}

Field::~Field()
{
	if (m_fieldTilesData != nullptr) delete m_fieldTilesData; 
	if (m_monsterManager != nullptr) delete m_monsterManager;
	if (m_sectorManager != nullptr) delete m_sectorManager;

	m_leaveSectorsVec.clear();
	m_leaveSectorsVec.resize(0);

	m_enterSectorsVec.clear();
	m_enterSectorsVec.resize(0);
}

void Field::InitMonsterThread()
{
	if (m_monsterManager->CreateMonsters())
	{
		
	}
}

void Field::FieldSendAll(SharedPointer<char>& sharedptr)
{
	for (const auto& element : m_userList.GetItemList())
	{
		element->AddToSendQueue(sharedptr);
	}
}

void Field::SectorSendAll(const std::vector<Sector*>& _sectorsVec, 
	SharedPointer<char>& sharedptr)
{
	const std::vector<Sector*>& tempVector = _sectorsVec;

	for (const auto& element_1 : tempVector)
	{
		if (element_1 == nullptr) continue;

		const std::list<User*> tempList = element_1->GetUserList()->GetItemList();

		if (tempList.empty()) continue;

		for (const auto& element_2 : tempList)
		{
			if (!element_2->GetIsGetUserList()) continue;

			element_2->AddToSendQueue(sharedptr);
		}
	}
}

void Field::SendUserList(User* _user)
{
	UserListPacket* userListPacket = new UserListPacket();

	userListPacket->userNum = 0;

	int i = 0;

	for (const auto& element : m_userList.GetItemList())
	{
		userListPacket->info[i].userInfo = element->GetInfo()->userInfo;
		userListPacket->info[i].position = element->GetInfo()->unitInfo.position;

		i++;
		userListPacket->userNum++;
	}

	userListPacket->size = (sizeof(Info_PacketUse) * userListPacket->userNum)
							+ sizeof(WORD) + sizeof(Packet);
	userListPacket->Init(SendCommand::Zone2C_USER_LIST, userListPacket->size);

	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(userListPacket));
	_user->AddToSendQueue(tempPtr);

	SendUserList_InRange(_user);

	MYDEBUG("[ USER LIST 전송 완료 ]\n");

	_user->SetIsGetUserList(true);

	m_monsterManager->SendMonsterList(_user);
}

void Field::SendUserList_InRange(User* _user)
{
	int tempNum = 0;

	UserListPacket_Light* userListPacket_InRange = new UserListPacket_Light();

	userListPacket_InRange->userNum = 0;

	const std::vector<Sector*> tempVec = _user->GetSector()->GetRoundSectorsVec();

	for (const auto& tempSector : tempVec)
	{
		const std::list<User*> tempList = tempSector->GetUserList()->GetItemList();

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

	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(userListPacket_InRange));
	_user->AddToSendQueue(tempPtr);
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

			_user->EnterField(this, m_fieldNum, &tempVec);
		}
		else
		{
			_user->EnterField(this, m_fieldNum, &m_spawnPosition);
		}
	}
	else
	{
		_user->EnterField(this, m_fieldNum, &m_spawnPosition);
	}

	m_userList.AddItem(_user);

	SendEnterUserInfo(_user);

	UpdateUserSector(_user);

	MYDEBUG("[%d Field : User Insert - %d (접속자 수  : %d) ]\n",
		m_fieldNum, _user->GetInfo()->userInfo.userID, (int)m_userList.GetItemList().size());
}

void Field::SendEnterUserInfo(User* _user)
{
	SessionInfoPacket* sessionInfoPacket = new SessionInfoPacket();
	sessionInfoPacket->Init(SendCommand::Zone2C_ENTER_USER_INFO, sizeof(SessionInfoPacket));
	sessionInfoPacket->info.userInfo = _user->GetInfo()->userInfo;
	sessionInfoPacket->info.unitInfo = _user->GetInfo()->unitInfo;

	SharedPointer<char> tempPtr1 =
		SharedPointer<char>(reinterpret_cast<char*>(sessionInfoPacket));

	for (const auto& element : m_userList.GetItemList())
	{
		if (element->GetInfo()->userInfo.userID ==
			sessionInfoPacket->info.userInfo.userID) continue;

		element->AddToSendQueue(tempPtr1);
	}

	UserNumPacket* userNumPacket_InRange = new UserNumPacket();
	userNumPacket_InRange->Init(SendCommand::Zone2C_ENTER_USER_INFO_IN_RANGE, sizeof(UserNumPacket));
	userNumPacket_InRange->userIndex = _user->GetInfo()->userInfo.userID;

	SharedPointer<char> tempPtr2 =
		SharedPointer<char>(reinterpret_cast<char*>(userNumPacket_InRange));

	if (_user->GetSector() == nullptr) return;

	const std::vector<Sector*>& tempVec = _user->GetSector()->GetRoundSectorsVec();

	if (tempVec.size() <= 0) return;

	for (const auto& tempSector : tempVec)
	{
		const std::list<User*> tempList = tempSector->GetUserList()->GetItemList();

		if (tempList.size() <= 0) continue;

		for (const auto& tempUser : tempList)
		{
			if (tempUser->GetInfo()->userInfo.userID ==
				userNumPacket_InRange->userIndex) continue;

			tempUser->AddToSendQueue(tempPtr2);
		}
	}
}

void Field::ExitUser(User* _user)
{
	m_userList.DeleteItem(_user);

	_user->GetSector()->GetUserList()->DeleteItem(_user);

	SendExitUserInfo(_user->GetInfo()->userInfo.userID);

	MYDEBUG("[%d Field : User Delete - %d (접속자 수  : %d) ]\n",
		m_fieldNum, _user->GetInfo()->userInfo.userID, (int)m_userList.GetItemList().size());
}

void Field::SendExitUserInfo(int _num)
{
	UserNumPacket* userPacket = new UserNumPacket();
	userPacket->Init(SendCommand::Zone2C_LEAVE_USER_INFO, sizeof(UserNumPacket));
	userPacket->userIndex = _num;

	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(userPacket));
	FieldSendAll(tempPtr);
}

bool Field::UserAttack(User * _user, int _monsterIndex)
{
	Monster* monster = m_monsterManager->GetMonster(_monsterIndex);
	if (monster == nullptr) return false;

	Packet* packet = monster->Hit(_user, _user->GetInfo()->unitInfo.atk);
	if (packet == nullptr) return false;

	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(packet));

	SectorSendAll(_user->GetSector()->GetRoundSectorsVec(), tempPtr);

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

		prevSector->GetUserList()->DeleteItem(_user);

		_user->SetSector(nowSector);
		_user->GetSector()->GetUserList()->AddItem(_user);

		std::vector<Sector*>* prevSectorsNeighbor = &prevSector->GetRoundSectorsVec();
		std::vector<Sector*>* nowSectorsNeighbor = &nowSector->GetRoundSectorsVec();

		auto iter1 = std::set_difference(
			prevSectorsNeighbor->begin(), prevSectorsNeighbor->end(),
			nowSectorsNeighbor->begin(), nowSectorsNeighbor->end(),
			m_leaveSectorsVec.begin());

		m_leaveSectorsVec.resize(iter1 - m_leaveSectorsVec.begin());

		auto iter2 = std::set_difference(
			nowSectorsNeighbor->begin(), nowSectorsNeighbor->end(),
			prevSectorsNeighbor->begin(), prevSectorsNeighbor->end(),
			m_enterSectorsVec.begin());

		m_enterSectorsVec.resize(iter2 - m_enterSectorsVec.begin());

		LeaveSector(_user);

		EnterSector(_user);

		m_leaveSectorsVec.resize(9);
		m_enterSectorsVec.resize(9);

		return;
	}

	_user->SetSector(nowSector);
	_user->GetSector()->GetUserList()->AddItem(_user);

	m_enterSectorsVec.resize((int)nowSector->GetRoundSectorsVec().size());
	std::copy(nowSector->GetRoundSectorsVec().begin(), nowSector->GetRoundSectorsVec().end(),
		m_enterSectorsVec.begin());

	EnterSector(_user);

	m_enterSectorsVec.resize(9);

	return;
}

void Field::LeaveSector(User* _user)
{
	UserNumPacket* userNumPacket_Exit = new UserNumPacket();
	userNumPacket_Exit->Init(SendCommand::Zone2C_LEAVE_SECTOR_USER_INFO, sizeof(UserNumPacket));
	userNumPacket_Exit->userIndex = _user->GetInfo()->userInfo.userID;

	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(userNumPacket_Exit));

	//자신이 있던 섹터 범위 내의 다른 유저들에게 자신이 나갔다고 알려주는 함수
	SectorSendAll(m_leaveSectorsVec, tempPtr);

	//내가 있던 섹터 범위 내의 다른 유저들의 리스트를 보내주는 함수(클라에서 안보이게 하기 위해)
	SendInvisibleUserList(_user);
	
	//내가 있던 섹터 범위 내의 몬스터들의 리스트를 보내주는 함수(클라에서 안보이게 하기 위해)
	SendInvisibleMonsterList(_user);
}

void Field::SendInvisibleUserList(User* _user)
{
	int tempNum = 0;

	UserListPacket_Light* userListPacket_Invisible = new UserListPacket_Light();
	userListPacket_Invisible->userNum = 0;

	for (const auto& tempSector : m_leaveSectorsVec)
	{
		if (tempSector == nullptr) break;

		if (tempSector->GetUserList()->GetItemList().size() <= 0) continue;

		const std::list<User*> tempList = tempSector->GetUserList()->GetItemList();

		for (const auto& element : tempList)
		{
			if (element == nullptr) break;

			userListPacket_Invisible->info[tempNum].userID = element->GetInfo()->userInfo.userID;
			userListPacket_Invisible->info[tempNum].position = element->GetInfo()->unitInfo.position;

			tempNum++;
			userListPacket_Invisible->userNum++;
		}
	}

	if (userListPacket_Invisible->userNum <= 0)
	{
		delete userListPacket_Invisible;

		return;
	}

	userListPacket_Invisible->size = (sizeof(Info_PacketUser_Light) * userListPacket_Invisible->userNum)
		+ sizeof(WORD) + sizeof(Packet);
	userListPacket_Invisible->Init(SendCommand::Zone2C_USER_LIST_INVISIBLE, userListPacket_Invisible->size);

	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(userListPacket_Invisible));
	_user->AddToSendQueue(tempPtr);
}

void Field::SendInvisibleMonsterList(User* _user)
{
	int tempNum = 0;

	MonsterInfoListPacket* monsterInfoListPacket_Invisible =
		new MonsterInfoListPacket();

	monsterInfoListPacket_Invisible->monsterNum = 0;

	for (const auto& tempSector : m_leaveSectorsVec)
	{
		if (tempSector == nullptr) break;

		if (tempSector->GetMonsterList()->GetItemList().size() <= 0) continue;

		const std::list<Monster*> tempList = tempSector->GetMonsterList()->GetItemList();

		for (const auto& element : tempList)
		{
			monsterInfoListPacket_Invisible->info[tempNum] = element->GetInfo();

			tempNum++;
			monsterInfoListPacket_Invisible->monsterNum++;
		}
	}

	monsterInfoListPacket_Invisible->size = (sizeof(MonsterInfo) * monsterInfoListPacket_Invisible->monsterNum)
		+ sizeof(WORD) + sizeof(Packet);
	monsterInfoListPacket_Invisible->Init(SendCommand::Zone2C_MONSTER_INFO_LIST_INVISIBLE, monsterInfoListPacket_Invisible->size);

	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(monsterInfoListPacket_Invisible));
	_user->AddToSendQueue(tempPtr);
}

void Field::EnterSector(User* _user)
{
	UserPositionPacket* userPositionPacket_Enter = new UserPositionPacket();
	userPositionPacket_Enter->Init(SendCommand::Zone2C_ENTER_SECTOR_USER_INFO, sizeof(UserPositionPacket));
	userPositionPacket_Enter->userIndex = _user->GetInfo()->userInfo.userID;
	userPositionPacket_Enter->position = _user->GetInfo()->unitInfo.position;

	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(userPositionPacket_Enter));

	//내가 들어온 섹터 범위 내의 다른 유저들에게 자신이 들어왔다고 알려주는 함수
	SectorSendAll(m_enterSectorsVec, tempPtr);

	//내가 들어온 섹터 범위 내의 다른 유저들의 리스트를 보내주는 함수(클라에서 보이게 하기 위해)
	SendVisibleUserList(_user);

	//내가 들어온 섹터 범위 내의 몬스터들의 리스트를 보내주는 함수(클라에서 보이게 하기 위해)
	SendVisibleMonsterList(_user);
}

void Field::SendVisibleUserList(User* _user)
{
	int tempNum = 0;

	UserListPacket_Light* userListPacket_Visible = new UserListPacket_Light();
	userListPacket_Visible->userNum = 0;

	for (const auto& tempSector : m_enterSectorsVec)
	{
		if (tempSector == nullptr) break;

		if (tempSector->GetUserList()->GetItemList().size() <= 0) continue;

		const std::list<User*> tempList = tempSector->GetUserList()->GetItemList();

		for (const auto& element : tempList)
		{
			if (element == nullptr) break;

			//printf("posi : %f, %f \n", 
			//	element->GetInfo()->unitInfo.position.x, 
			//	element->GetInfo()->unitInfo.position.y);
			
			userListPacket_Visible->info[tempNum].userID = element->GetInfo()->userInfo.userID;
			userListPacket_Visible->info[tempNum].position = element->GetInfo()->unitInfo.position;

			tempNum++;
			userListPacket_Visible->userNum++;

			//printf("send %d user, posi : %f, %f \n", element->GetInfo()->userInfo.userID,
			//	element->GetInfo()->unitInfo.position.x, element->GetInfo()->unitInfo.position.y);
		}
	}

	if (userListPacket_Visible->userNum <= 0)
	{
		delete userListPacket_Visible;

		return;
	}

	userListPacket_Visible->size = (sizeof(Info_PacketUser_Light) * userListPacket_Visible->userNum)
		+ sizeof(WORD) + sizeof(Packet);
	userListPacket_Visible->Init(SendCommand::Zone2C_USER_LIST_VISIBLE, userListPacket_Visible->size);

	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(userListPacket_Visible));
	_user->AddToSendQueue(tempPtr);
}

void Field::SendVisibleMonsterList(User* _user)
{
	int tempNum = 0;

	MonsterInfoListPacket* monsterInfoListPacket_Visible = new MonsterInfoListPacket();

	monsterInfoListPacket_Visible->monsterNum = 0;

	for (const auto& tempSector : m_enterSectorsVec)
	{
		if (tempSector == nullptr) break;

		if (tempSector->GetMonsterList()->GetItemList().size() <= 0) continue;

		const std::list<Monster*> tempList = tempSector->GetMonsterList()->GetItemList();

		for (const auto& element : tempList)
		{
			monsterInfoListPacket_Visible->info[tempNum] = element->GetInfo();

			tempNum++;
			monsterInfoListPacket_Visible->monsterNum++;
		}
	}

	monsterInfoListPacket_Visible->size = (sizeof(MonsterInfo) * monsterInfoListPacket_Visible->monsterNum)
		+ sizeof(WORD) + sizeof(Packet);
	monsterInfoListPacket_Visible->Init(SendCommand::Zone2C_MONSTER_INFO_LIST_VISIBLE, monsterInfoListPacket_Visible->size);

	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(monsterInfoListPacket_Visible));
	_user->AddToSendQueue(tempPtr);
}

//테스트용
void Field::EnterTestClient(User* _user, int _num)
{
	_user->TestClientEnterField(this, m_fieldNum, _num, &m_spawnPosition);

	m_userList.AddItem(_user);

	UpdateUserSector(_user);

	SendEnterUserInfo(_user);

	MYDEBUG("[%d Field : Test Client Insert - %d (접속자 수  : %d) ]\n",
		m_fieldNum, _user->GetInfo()->userInfo.userID, (int)m_userList.GetItemList().size());
}