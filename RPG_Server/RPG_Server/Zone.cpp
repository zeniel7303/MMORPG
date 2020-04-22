#include "Zone.h"

Zone::Zone()
{
}

Zone::~Zone()
{
}

void Zone::Init(int _num, VECTOR2 _spawnPosition)
{
	m_zoneNum = _num;
	m_spawnPosition = _spawnPosition;

	m_sendBuffer = new SendBuffer();
	m_sendBuffer->Init(10000);

	printf("[ %d Zone Init ,", _num);
}

void Zone::GetMap(const char* _name)
{
	m_zoneTilesData.GetMap(_name);

	m_sectorManager.Init(&m_zoneTilesData);
	
	m_monsterLogicThread.Init(this, &m_zoneTilesData, &m_sectorManager);
	m_monsterLogicThread.ThreadClass<MonsterLogicThread>::Start(&m_monsterLogicThread);
}

void Zone::ZoneSendAll(char * _buffer, int _size)
{
	for (const auto& element : m_itemList)
	{
		if (!element->GetIsTestClient())
		{
			element->Send(_buffer, _size);
		}
	}
}

void Zone::SectorSendAll(Sector* _sector, Sector** _sectors, char * _buffer, int _size)
{
	m_sectorManager.GetNeighborSectors(_sectors, _sector);

	for (int i = 0; i < 9; i++)
	{
		for (const auto& element : *_sectors[i]->Manager<User>::GetItemList())
		{
			if (!element->GetIsTestClient())
			{
				element->Send(_buffer, _size);
			}
		}
	}

	//일반적인 for문
	/*list<User*>::iterator iterEnd = m_itemList.end();
	User* user;

	list<User*>::iterator iter;
	for (iter = m_itemList.begin(); iter != iterEnd; iter++)
	{
		user = *iter;

		//if (!user->GetIsTestClient())
		{
			user->Send(_buffer, _size);
		}
	}
	*/

	//Range-based for loop
	/*for (const auto& element : m_itemList)
	{
		if (!element->GetIsTestClient())
		{
			element->Send(_buffer, _size);
		}
	}*/

	//for_each
	//https://tt91.tistory.com/11
	/*list<User*>::iterator iterBegin = m_itemList.begin();
	list<User*>::iterator iterEnd = m_itemList.end();

	for_each(iterBegin, iterEnd, [_buffer, _size](User* _user)
	{
		//if (!_user->GetIsTestClient())
		{
			_user->Send(_buffer, _size);
		}
	});
	*/
}

void Zone::SectorSendAll(Sector** _sectors, char * _buffer, int _size)
{
	for (int i = 0; i < 9; i++)
	{
		if (_sectors[i] == nullptr) break;

		if (_sectors[i]->Manager<User>::GetItemList()->size() <= 0) continue;

		for (const auto& element : *_sectors[i]->Manager<User>::GetItemList())
		{
			if (element == nullptr) continue;

			if (!element->GetIsTestClient())
			{
				element->Send(_buffer, _size);
			}
		}
	}
}

void Zone::SendUserList(User* _user)
{
	m_locker.EnterLock();

	UserListPacket* userListPacket =
		reinterpret_cast<UserListPacket*>(_user->GetSendBuffer()->
			GetBuffer(sizeof(UserListPacket)));

	//userListPacket->userNum = m_itemList.size();
	userListPacket->userNum = 0;

	int i = 0;
	list<User*>::iterator iter;
	list<User*>::iterator iterEnd = m_itemList.end();
	User* user;

	for (iter = m_itemList.begin(); iter != iterEnd; iter++)
	{
		user = *iter;

		if (user->GetIsTestClient()) continue;

		userListPacket->info[i].userInfo = user->GetInfo()->userInfo;
		userListPacket->info[i].unitInfo = user->GetInfo()->unitInfo;

		i++;
		userListPacket->userNum++;
	}

	userListPacket->size = (sizeof(BasicInfo) * userListPacket->userNum)
							+ sizeof(WORD) + sizeof(Packet);
	userListPacket->Init(SendCommand::USER_LIST, userListPacket->size);

	_user->GetSendBuffer()->Write(userListPacket->size);
	_user->Send(reinterpret_cast<char*>(userListPacket), userListPacket->size);

	SendUserList_InRange(_user);

	m_locker.LeaveLock();

	printf("[ USER LIST 전송 완료 ]\n");

	m_monsterLogicThread.SendMonsterList(_user);
}

void Zone::SendUserList_InRange(User* _user)
{
	int tempNum = 0;

	m_sectorManager.GetNeighborSectors(_user->GetSectors(), _user->GetSector());

	UserListPacket* userListPacket_InRange =
		reinterpret_cast<UserListPacket*>(_user->GetSendBuffer()->
			GetBuffer(sizeof(UserListPacket)));

	userListPacket_InRange->userNum = 0;

	for (int i = 0; i < 9; i++)
	{
		if (_user->GetSectors()[i] == nullptr) break;

		if (_user->GetSectors()[i]->Manager<User>::GetItemList()->size() <= 0) continue;

		for (const auto& element : *_user->GetSectors()[i]->Manager<User>::GetItemList())
		{
			if (element == nullptr) break;

			if (element->GetIsTestClient()) continue;

			userListPacket_InRange->info[tempNum].userInfo = element->GetInfo()->userInfo;
			userListPacket_InRange->info[tempNum].unitInfo = element->GetInfo()->unitInfo;

			tempNum++;
			userListPacket_InRange->userNum++;
		}
	}

	userListPacket_InRange->size = (sizeof(BasicInfo) * userListPacket_InRange->userNum)
		+ sizeof(WORD) + sizeof(Packet);
	userListPacket_InRange->Init(SendCommand::USER_LIST_IN_RANGE, userListPacket_InRange->size);

	_user->GetSendBuffer()->Write(userListPacket_InRange->size);
	_user->Send(reinterpret_cast<char*>(userListPacket_InRange), userListPacket_InRange->size);
}

//테스트용
void Zone::EnterTestClient(User* _user)
{
	_user->TestClientEnterZone(this, m_zoneNum);

	m_locker.EnterLock();

	AddItem(_user);

	m_locker.LeaveLock();

	printf("[%d Zone : Test Client Insert - %d (", m_zoneNum, _user->GetInfo()->userInfo.userID);
	printf("user count : %d) ]\n", (int)m_itemList.size());
}

void Zone::EnterUser(User* _user)
{
	if (_user->GetZone() != nullptr)
	{
		if (_user->GetZone()->GetZoneNum() == 2)
		{
			VECTOR2 tempVec;
			tempVec.x = 52.0f;
			tempVec.y = 86.0f;

			_user->EnterZone(this, m_zoneNum, tempVec);
		}
		else
		{
			_user->EnterZone(this, m_zoneNum, m_spawnPosition);
		}
	}
	else
	{
		_user->EnterZone(this, m_zoneNum, m_spawnPosition);
	}

	m_locker.EnterLock();

	AddItem(_user);

	UpdateUserSector(_user);

	SendEnterUserInfo(_user);

	m_locker.LeaveLock();

	printf("[%d Zone : User Insert - %d (", m_zoneNum, _user->GetInfo()->userInfo.userID);
	printf("user count : %d) ]\n", (int)m_itemList.size());
}

void Zone::SendEnterUserInfo(User* _user)
{
	SessionInfoPacket* sessionInfoPacket =
		reinterpret_cast<SessionInfoPacket*>(m_sendBuffer->
			GetBuffer(sizeof(SessionInfoPacket)));
	sessionInfoPacket->Init(SendCommand::ENTER_USER_INFO, sizeof(SessionInfoPacket));
	sessionInfoPacket->info.userInfo = _user->GetInfo()->userInfo;
	sessionInfoPacket->info.unitInfo = _user->GetInfo()->unitInfo;
	m_sendBuffer->Write(sessionInfoPacket->size);

	UserNumPacket* userNumPacket_InRange =
		reinterpret_cast<UserNumPacket*>(m_sendBuffer->
			GetBuffer(sizeof(UserNumPacket)));
	userNumPacket_InRange->Init(SendCommand::ENTER_USER_INFO_IN_RANGE, sizeof(UserNumPacket));
	userNumPacket_InRange->userIndex = _user->GetInfo()->userInfo.userID;
	m_sendBuffer->Write(userNumPacket_InRange->size);

	for (const auto& element : m_itemList)
	{
		if (element->GetIsTestClient()) continue;

		if (element->GetInfo()->userInfo.userID ==
			sessionInfoPacket->info.userInfo.userID) continue;

		element->Send(reinterpret_cast<char*>(sessionInfoPacket), sessionInfoPacket->size);
	}

	m_sectorManager.GetNeighborSectors(_user->GetSectors(), _user->GetSector());

	for (int i = 0; i < 5; i++)
	{
		if (_user->GetSectors()[i] == nullptr) break;

		if (_user->GetSectors()[i]->Manager<User>::GetItemList()->size() <= 0) continue;

		for (const auto& element : *_user->GetSectors()[i]->Manager<User>::GetItemList())
		{
			if (element == nullptr) break;

			if (element->GetIsTestClient()) continue;

			if (element->GetInfo()->userInfo.userID ==
				userNumPacket_InRange->userIndex) continue;

			element->Send(reinterpret_cast<char*>(userNumPacket_InRange), userNumPacket_InRange->size);
		}
	}

	/*SessionInfoPacket* sessionInfoPacket =
		reinterpret_cast<SessionInfoPacket*>(m_sendBuffer->
			GetBuffer(sizeof(SessionInfoPacket)));
	sessionInfoPacket->Init(SendCommand::ENTER_USER_INFO_IN_RANGE, sizeof(SessionInfoPacket));
	sessionInfoPacket->info.userInfo = _user->GetInfo()->userInfo;
	sessionInfoPacket->info.unitInfo = _user->GetInfo()->unitInfo;
	m_sendBuffer->Write(sessionInfoPacket->size);*/

	/*list<User*>::iterator iterBegin = m_itemList.begin();
	list<User*>::iterator iterEnd = m_itemList.end();

	for_each(iterBegin, iterEnd, [sessionInfoPacket](User* _user)
	{
		if (_user->GetInfo()->userInfo.userID !=
			sessionInfoPacket->info.userInfo.userID)
		{
			_user->Send(reinterpret_cast<char*>(sessionInfoPacket), sessionInfoPacket->size);
		}
	});*/

	/*
	list<User*>::iterator iter;
	list<User*>::iterator iterEnd = m_itemList.end();
	User* user;

	for (iter = m_itemList.begin(); iter != iterEnd; iter++)
	{
		user = *iter;
		if (user->GetInfo()->userInfo.userID ==
			sessionInfoPacket->info.userInfo.userID) continue;

		user->Send(reinterpret_cast<char*>(sessionInfoPacket), sessionInfoPacket->size);
	}*/
}

void Zone::ExitUser(User* _user)
{
	m_locker.EnterLock();

	DeleteItem(_user);

	_user->GetSector()->Manager<User>::DeleteItem(_user);

	SendExitUserInfo(_user->GetInfo()->userInfo.userID);

	m_locker.LeaveLock();

	printf("[%d Zone] : User Delete - %d (", m_zoneNum, _user->GetInfo()->userInfo.userID);
	printf("user count : %d)\n", (int)m_itemList.size());
}

void Zone::SendExitUserInfo(int _num)
{
	UserNumPacket* userPacket =
		reinterpret_cast<UserNumPacket*>(m_sendBuffer->
			GetBuffer(sizeof(UserNumPacket)));
	userPacket->Init(SendCommand::LEAVE_USER_INFO, sizeof(UserNumPacket));
	userPacket->userIndex = _num;
	m_sendBuffer->Write(userPacket->size);

	/*list<User*>::iterator iterBegin = m_itemList.begin();
	list<User*>::iterator iterEnd = m_itemList.end();

	for_each(iterBegin, iterEnd, [userPacket](User* _user)
	{
		_user->Send(reinterpret_cast<char*>(userPacket), userPacket->size);
	});*/

	ZoneSendAll(reinterpret_cast<char*>(userPacket), userPacket->size);

	/*list<User*>::iterator iter;
	list<User*>::iterator iterEnd = m_itemList.end();
	User* user;

	for (iter = m_itemList.begin(); iter != iterEnd; iter++)
	{
		user = *iter;
		user->Send(reinterpret_cast<char*>(userPacket), userPacket->size);
	}*/
}

bool Zone::UserAttack(User * _user, int _monsterIndex)
{
	Monster* monster = m_monsterLogicThread.GetMonster(_monsterIndex);

	if (monster == nullptr) return false;

	Packet* packet = monster->Hit(_user, _user->GetInfo()->unitInfo.atk);
	if (packet == nullptr) return false;

	SectorSendAll(_user->GetSector(), _user->GetSectors(), reinterpret_cast<char*>(packet), packet->size);

	if (monster->GetIsDeath())
	{
		_user->PlusExp(monster->GetData().dropExp);
	}

	return true;
}

void Zone::RespawnUser(User* _user)
{
	_user->Respawn(m_spawnPosition);

	UpdateUserSector(_user);
}

void Zone::UpdateUserSector(User* _user)
{
	Sector* prevSector = _user->GetSector();

	if (prevSector != nullptr)
	{
		printf("[ Exit User (Prev Sector) ] %s User : Now Sector : %d\n", 
			_user->GetInfo()->userInfo.userName, _user->GetSector()->GetSectorNum());
		prevSector->Manager<User>::DeleteItem(_user);

		_user->SetSector(m_sectorManager.
			GetSector(_user->GetInfo()->unitInfo.position.x, _user->GetInfo()->unitInfo.position.y));
		_user->GetSector()->Manager<User>::AddItem(_user);

		m_locker.EnterLock();

		UserNumPacket* userNumPacket_Exit =
			reinterpret_cast<UserNumPacket*>(m_sendBuffer->
				GetBuffer(sizeof(UserNumPacket)));
		userNumPacket_Exit->Init(SendCommand::LEAVE_SECTOR_USER_INFO, sizeof(UserNumPacket));
		userNumPacket_Exit->userIndex = _user->GetInfo()->userInfo.userID;
		m_sendBuffer->Write(userNumPacket_Exit->size);

		UserNumPacket* userNumPacket_Enter =
			reinterpret_cast<UserNumPacket*>(m_sendBuffer->
				GetBuffer(sizeof(UserNumPacket)));
		userNumPacket_Enter->Init(SendCommand::ENTER_SECTOR_USER_INFO, sizeof(UserNumPacket));
		userNumPacket_Enter->userIndex = _user->GetInfo()->userInfo.userID;
		m_sendBuffer->Write(userNumPacket_Enter->size);

		ExitSector(_user, _user->GetSector()->GetSectorNum(), prevSector->GetSectorNum(),
			reinterpret_cast<char*>(userNumPacket_Exit), userNumPacket_Exit->size);

		EnterSector(_user, _user->GetSector()->GetSectorNum(), prevSector->GetSectorNum(),
			reinterpret_cast<char*>(userNumPacket_Enter), userNumPacket_Enter->size);

		m_locker.LeaveLock();
	}
	else
	{
		_user->SetSector(m_sectorManager.
			GetSector(_user->GetInfo()->unitInfo.position.x, 
				_user->GetInfo()->unitInfo.position.y));
		
		_user->GetSector()->Manager<User>::AddItem(_user);
	}
}

void Zone::ExitSector(User* _user, int _nowNum, int _prevNum, char* _packet, int _size)
{
	int prevSectorNum = _prevNum;
	int mapSize = m_sectorManager.GetSize();

	switch (_nowNum - prevSectorNum)
	{
		//우측 상단 이동
	case 9:
		m_exitSectors[0] = m_sectorManager.
			GetSector(prevSectorNum - mapSize - 1);
		m_exitSectors[1] = m_sectorManager.
			GetSector(prevSectorNum - mapSize);
		m_exitSectors[2] = m_sectorManager.
			GetSector(prevSectorNum - mapSize + 1);
		m_exitSectors[3] = m_sectorManager.
			GetSector(prevSectorNum + 1);
		m_exitSectors[4] = m_sectorManager.
			GetSector(prevSectorNum + mapSize + 1);
		break;
		//우측 이동
	case 10:
		m_exitSectors[0] = m_sectorManager.
			GetSector(prevSectorNum - mapSize - 1);
		m_exitSectors[1] = m_sectorManager.
			GetSector(prevSectorNum - mapSize);
		m_exitSectors[2] = m_sectorManager.
			GetSector(prevSectorNum - mapSize + 1);
		break;
		//우측 하단 이동
	case 11:
		m_exitSectors[0] = m_sectorManager.
			GetSector(prevSectorNum - mapSize - 1);
		m_exitSectors[1] = m_sectorManager.
			GetSector(prevSectorNum - mapSize);
		m_exitSectors[2] = m_sectorManager.
			GetSector(prevSectorNum - mapSize + 1);
		m_exitSectors[3] = m_sectorManager.
			GetSector(prevSectorNum - 1);
		m_exitSectors[4] = m_sectorManager.
			GetSector(prevSectorNum + mapSize - 1);
		break;
		//하단 이동
	case 1:
		m_exitSectors[0] = m_sectorManager.
			GetSector(prevSectorNum - mapSize - 1);
		m_exitSectors[1] = m_sectorManager.
			GetSector(prevSectorNum - 1);
		m_exitSectors[2] = m_sectorManager.
			GetSector(prevSectorNum + mapSize - 1);
		break;
		//좌측 하단 이동
	case -9:
		m_exitSectors[0] = m_sectorManager.
			GetSector(prevSectorNum - mapSize - 1);
		m_exitSectors[1] = m_sectorManager.
			GetSector(prevSectorNum - 1);
		m_exitSectors[2] = m_sectorManager.
			GetSector(prevSectorNum + mapSize - 1);
		m_exitSectors[3] = m_sectorManager.
			GetSector(prevSectorNum + mapSize);
		m_exitSectors[4] = m_sectorManager.
			GetSector(prevSectorNum + mapSize + 1);
		break;
		//좌측 이동
	case -10:
		m_exitSectors[0] = m_sectorManager.
			GetSector(prevSectorNum + mapSize - 1);
		m_exitSectors[1] = m_sectorManager.
			GetSector(prevSectorNum + mapSize);
		m_exitSectors[2] = m_sectorManager.
			GetSector(prevSectorNum + mapSize + 1);
		break;
		//좌측 상단 이동
	case -11:
		m_exitSectors[0] = m_sectorManager.
			GetSector(prevSectorNum - mapSize + 1);
		m_exitSectors[1] = m_sectorManager.
			GetSector(prevSectorNum + 1);
		m_exitSectors[2] = m_sectorManager.
			GetSector(prevSectorNum + mapSize - 1);
		m_exitSectors[3] = m_sectorManager.
			GetSector(prevSectorNum + mapSize);
		m_exitSectors[4] = m_sectorManager.
			GetSector(prevSectorNum + mapSize + 1);
		break;
		//상단 이동
	case -1:
		m_exitSectors[0] = m_sectorManager.
			GetSector(prevSectorNum - mapSize + 1);
		m_exitSectors[1] = m_sectorManager.
			GetSector(prevSectorNum + 1);
		m_exitSectors[2] = m_sectorManager.
			GetSector(prevSectorNum + mapSize + 1);
		break;
	}

	//내가 있던 섹터 범위 내의 다른 유저들의 리스트를 보내주는 함수(클라에서 안보이게 하기 위해)
	SendInvisibleUserList(_user);

	//자신이 있던 섹터 범위 내의 다른 유저들에게 자신이 나갔다고 알려주는 함수
	SectorSendAll(m_exitSectors, _packet, _size);
}

void Zone::SendInvisibleUserList(User* _user)
{
	int tempNum = 0;

	UserListPacket* userListPacket =
		reinterpret_cast<UserListPacket*>(_user->GetSendBuffer()->
			GetBuffer(sizeof(UserListPacket)));

	userListPacket->userNum = 0;

	for (int i = 0; i < 9; i++)
	{
		if (m_exitSectors[i] == nullptr) break;

		if (m_exitSectors[i]->Manager<User>::GetItemList()->size() <= 0) continue;

		for (const auto& element : *m_exitSectors[i]->Manager<User>::GetItemList())
		{
			if (element == nullptr) break;

			if (element->GetIsTestClient()) continue;

			userListPacket->info[tempNum].userInfo = element->GetInfo()->userInfo;
			userListPacket->info[tempNum].unitInfo = element->GetInfo()->unitInfo;

			tempNum++;
			userListPacket->userNum++;
		}
	}

	userListPacket->size = (sizeof(BasicInfo) * userListPacket->userNum)
		+ sizeof(WORD) + sizeof(Packet);
	userListPacket->Init(SendCommand::USER_LIST_INVISIBLE, userListPacket->size);

	_user->GetSendBuffer()->Write(userListPacket->size);
	_user->Send(reinterpret_cast<char*>(userListPacket), userListPacket->size);
}

void Zone::EnterSector(User* _user, int _nowNum, int _prevNum, char* _packet, int _size)
{
	int nowSectorNum = _nowNum;
	int mapSize = m_sectorManager.GetSize();

	switch (nowSectorNum - _prevNum)
	{
		//우측 상단 이동
	case 9:
		m_enterSectors[0] = m_sectorManager.
			GetSector(nowSectorNum - mapSize - 1);
		m_enterSectors[1] = m_sectorManager.
			GetSector(nowSectorNum - 1);
		m_enterSectors[2] = m_sectorManager.
			GetSector(nowSectorNum + mapSize - 1);
		m_enterSectors[3] = m_sectorManager.
			GetSector(nowSectorNum - mapSize);
		m_enterSectors[4] = m_sectorManager.
			GetSector(nowSectorNum - mapSize + 1);
		break;
		//우측 이동
	case 10:
		m_enterSectors[0] = m_sectorManager.
			GetSector(nowSectorNum + mapSize - 1);
		m_enterSectors[1] = m_sectorManager.
			GetSector(nowSectorNum + mapSize);
		m_enterSectors[2] = m_sectorManager.
			GetSector(nowSectorNum + mapSize + 1);
		break;
		//우측 하단 이동
	case 11:
		m_enterSectors[0] = m_sectorManager.
			GetSector(nowSectorNum - mapSize + 1);
		m_enterSectors[1] = m_sectorManager.
			GetSector(nowSectorNum + 1);
		m_enterSectors[2] = m_sectorManager.
			GetSector(nowSectorNum + mapSize - 1);
		m_enterSectors[3] = m_sectorManager.
			GetSector(nowSectorNum + mapSize);
		m_enterSectors[4] = m_sectorManager.
			GetSector(nowSectorNum + mapSize + 1);
		break;
		//하단 이동
	case 1:
		m_enterSectors[0] = m_sectorManager.
			GetSector(nowSectorNum - mapSize + 1);
		m_enterSectors[1] = m_sectorManager.
			GetSector(nowSectorNum + 1);
		m_enterSectors[2] = m_sectorManager.
			GetSector(nowSectorNum + mapSize + 1);
		break;
		//좌측 하단 이동
	case -9:
		m_enterSectors[0] = m_sectorManager.
			GetSector(nowSectorNum - mapSize - 1);
		m_enterSectors[1] = m_sectorManager.
			GetSector(nowSectorNum - mapSize);
		m_enterSectors[2] = m_sectorManager.
			GetSector(nowSectorNum - mapSize + 1);
		m_enterSectors[3] = m_sectorManager.
			GetSector(nowSectorNum + 1);
		m_enterSectors[4] = m_sectorManager.
			GetSector(nowSectorNum + mapSize + 1);
		break;
		//좌측 이동
	case -10:
		m_enterSectors[0] = m_sectorManager.
			GetSector(nowSectorNum - mapSize - 1);
		m_enterSectors[1] = m_sectorManager.
			GetSector(nowSectorNum - mapSize);
		m_enterSectors[2] = m_sectorManager.
			GetSector(nowSectorNum - mapSize + 1);
		break;
		//좌측 상단 이동
	case -11:
		m_enterSectors[0] = m_sectorManager.
			GetSector(nowSectorNum - mapSize - 1);
		m_enterSectors[1] = m_sectorManager.
			GetSector(nowSectorNum - mapSize);
		m_enterSectors[2] = m_sectorManager.
			GetSector(nowSectorNum - mapSize + 1);
		m_enterSectors[3] = m_sectorManager.
			GetSector(nowSectorNum - 1);
		m_enterSectors[4] = m_sectorManager.
			GetSector(nowSectorNum + mapSize - 1);
		break;
		//상단 이동
	case -1:
		m_enterSectors[0] = m_sectorManager.
			GetSector(nowSectorNum - mapSize - 1);
		m_enterSectors[1] = m_sectorManager.
			GetSector(nowSectorNum - 1);
		m_enterSectors[2] = m_sectorManager.
			GetSector(nowSectorNum + mapSize - 1);
		break;
	}

	//내가 들어온 섹터 범위 내의 다른 유저들의 리스트를 보내주는 함수(클라에서 보이게 하기 위해)
	SendVisibleUserList(_user);

	//내가 들어온 섹터 범위 내의 다른 유저들에게 자신이 들어왔다고 알려주는 함수
	SectorSendAll(m_enterSectors, _packet, _size);
}

void Zone::SendVisibleUserList(User* _user)
{
	int tempNum = 0;

	UserListPacket* userListPacket =
		reinterpret_cast<UserListPacket*>(_user->GetSendBuffer()->
			GetBuffer(sizeof(UserListPacket)));

	userListPacket->userNum = 0;

	for (int i = 0; i < 9; i++)
	{
		if (m_enterSectors[i] == nullptr) break;

		if (m_enterSectors[i]->Manager<User>::GetItemList()->size() <= 0) continue;

		for (const auto& element : *m_enterSectors[i]->Manager<User>::GetItemList())
		{
			if (element == nullptr) break;

			if (element->GetIsTestClient()) continue;

			userListPacket->info[tempNum].userInfo = element->GetInfo()->userInfo;
			userListPacket->info[tempNum].unitInfo = element->GetInfo()->unitInfo;

			tempNum++;
			userListPacket->userNum++;
		}
	}

	userListPacket->size = (sizeof(BasicInfo) * userListPacket->userNum)
		+ sizeof(WORD) + sizeof(Packet);
	userListPacket->Init(SendCommand::USER_LIST_VISIBLE, userListPacket->size);

	_user->GetSendBuffer()->Write(userListPacket->size);
	_user->Send(reinterpret_cast<char*>(userListPacket), userListPacket->size);
}