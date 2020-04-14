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

	m_monsterLogicThread.Init(this, &m_zoneTilesData);
	m_monsterLogicThread.ThreadClass<MonsterLogicThread>::Start(&m_monsterLogicThread);
}

//부하가 가장 심한 곳
//세 방법 다 속도는 비슷했다.(그나마 가장 빠른 for_each문 사용) -> 모든 list의 아이템을 체크하는 곳을 for_each문으로 변경
void Zone::SendAll(char * _buffer, int _size)
{
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
		element->Send(_buffer, _size);
	}*/

	//for_each
	//https://tt91.tistory.com/11
	list<User*>::iterator iterBegin = m_itemList.begin();
	list<User*>::iterator iterEnd = m_itemList.end();

	for_each(iterBegin, iterEnd, [_buffer, _size](User* _user)
	{
		//if (!_user->GetIsTestClient())
		{
			_user->Send(_buffer, _size);
		}
	});
}

void Zone::SendUserList(User* _user)
{
	m_locker.EnterLock();

	UserListPacket* userListPacket =
		reinterpret_cast<UserListPacket*>(_user->GetSendBuffer()->
			GetBuffer(sizeof(UserListPacket)));

	//userListPacket->userNum = m_itemList.size();
	userListPacket->userNum = 0;

	list<User*>::iterator iterEnd = m_itemList.end();
	User* user;

	int i = 0;
	list<User*>::iterator iter;
	for (iter = m_itemList.begin(); iter != iterEnd; iter++)
	{
		user = *iter;

		if (user->GetIsTestClient()) break;

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

	m_locker.LeaveLock();

	printf("[ USER LIST 전송 완료 ]\n");

	m_monsterLogicThread.SendMonsterList(_user);
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

	list<User*>::iterator iterBegin = m_itemList.begin();
	list<User*>::iterator iterEnd = m_itemList.end();

	for_each(iterBegin, iterEnd, [sessionInfoPacket](User* _user)
	{
		if (_user->GetInfo()->userInfo.userID !=
			sessionInfoPacket->info.userInfo.userID)
		{
			_user->Send(reinterpret_cast<char*>(sessionInfoPacket), sessionInfoPacket->size);
		}
	});

	/*list<User*>::iterator iterEnd = m_itemList.end();
	User* user;

	list<User*>::iterator iter;
	for (iter = m_itemList.begin(); iter != iterEnd; iter++)
	{
		user = *iter;
		if (user->GetInfo()->userInfo.userID ==
			sessionInfoPacket->info.userInfo.userID) break;

		user->Send(reinterpret_cast<char*>(sessionInfoPacket), sessionInfoPacket->size);
	}*/
}

void Zone::ExitUser(User* _user)
{
	m_locker.EnterLock();

	DeleteItem(_user);

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

	list<User*>::iterator iterBegin = m_itemList.begin();
	list<User*>::iterator iterEnd = m_itemList.end();

	for_each(iterBegin, iterEnd, [userPacket](User* _user)
	{
		_user->Send(reinterpret_cast<char*>(userPacket), userPacket->size);
	});

	/*list<User*>::iterator iterEnd = m_itemList.end();
	User* user;

	list<User*>::iterator iter;
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

	SendAll(reinterpret_cast<char*>(packet), packet->size);

	if (monster->GetIsDeath())
	{
		_user->PlusExp(monster->GetData().dropExp);
	}

	return true;
}

void Zone::RespawnUser(User* _user)
{
	_user->Respawn(m_spawnPosition);
}