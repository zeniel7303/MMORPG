#include "User.h"
#include "Field.h"
#include "Sector.h"

User::User()
{
}

User::~User()
{
}

void User::Init()
{
	Session::Init();

	m_field = nullptr;
	m_startCheckingHeartBeat = false;
	m_isCheckingHeartBeat = false;
	m_isTestClient = false;

	m_sector = nullptr;
}

void User::OnConnect(SOCKET _socket)
{
	Session::OnConnect(_socket);

	IsConnectedPacket* isConnectedPacket =
		reinterpret_cast<IsConnectedPacket*>(GetSendBuffer()->
			GetBuffer(sizeof(IsConnectedPacket)));

	isConnectedPacket->Init(SendCommand::Zone2C_ISCONNECTED, sizeof(IsConnectedPacket));
	isConnectedPacket->isConnected = m_isConnected;

	Session::Send(isConnectedPacket);

	//printf("[ isConnectedPacket 전송 완료 ]\n");
}

void User::Disconnect()
{
	Session::Disconnect();

	//UpdateInfo();

	Reset();
}

void User::Reset()
{
	Session::Reset();

	m_field = nullptr;
	m_startCheckingHeartBeat = false;
	m_isCheckingHeartBeat = false;
	m_isTestClient = false;

	m_sector = nullptr;
	m_basicInfo.unitInfo.fieldNum = 0;
}

void User::HeartBeatChecked()
{
	SetIsChecking(false);

	if (m_heartBeatCheckedCount >= 3)
	{
		if (!m_isTestClient)
		{
			Packet* UpdateInfoPacket = reinterpret_cast<Packet*>(Session::GetSendBuffer()->
				GetBuffer(sizeof(Packet)));
			UpdateInfoPacket->Init(SendCommand::Zone2C_UPDATE_INFO, sizeof(Packet));

			Session::Send(UpdateInfoPacket);
		}

		m_heartBeatCheckedCount = 0;

		//printf("[ HeartBeat Checking & Update Success : %d ]\n", m_heartBeatCheckedCount);
	}
	else
	{
		m_heartBeatCheckedCount++;

		//printf("[ HeartBeat Checking Success : %d ]\n", m_heartBeatCheckedCount);
	}
}

void User::UpdateInfo()
{
	printf("[ %d user - INFO UPDATE TO DATABASE ] \n", m_basicInfo.userInfo.userID);

	MYSQLCLASS->UpdateUserInfo(m_basicInfo.userInfo.userID, m_basicInfo.unitInfo.level,
		m_basicInfo.unitInfo.hp.currentValue, m_basicInfo.unitInfo.hp.maxValue,
		m_basicInfo.unitInfo.mp.currentValue, m_basicInfo.unitInfo.mp.maxValue,
		m_basicInfo.unitInfo.exp.currentValue, m_basicInfo.unitInfo.exp.maxValue,
		m_basicInfo.unitInfo.atk, m_basicInfo.unitInfo.def);
}

void User::Death()
{
	m_basicInfo.unitInfo.state = STATE::DEATH;

	//죽었다고 패킷 전송해줘야함.
	UserNumPacket* userNumPacket =
		reinterpret_cast<UserNumPacket*>(Session::GetSendBuffer()->
			GetBuffer(sizeof(UserNumPacket)));

	userNumPacket->userIndex = m_basicInfo.userInfo.userID;
	userNumPacket->Init(SendCommand::Zone2C_USER_DEATH, sizeof(UserNumPacket));

	m_field->SectorSendAll(m_sector->GetRoundSectorsVec(), reinterpret_cast<char*>(userNumPacket), userNumPacket->size);
}

void User::Respawn(VECTOR2 _spawnPosition)
{
	m_basicInfo.unitInfo.state = STATE::IDLE;

	m_basicInfo.unitInfo.hp.currentValue = m_basicInfo.unitInfo.hp.maxValue;
	m_basicInfo.unitInfo.mp.currentValue = m_basicInfo.unitInfo.mp.maxValue;

	Unit::SetUnitPosition(_spawnPosition.x, _spawnPosition.y);
	m_tile = m_fieldTilesData->GetTile(
		static_cast<int>(m_basicInfo.unitInfo.position.x),
		static_cast<int>(m_basicInfo.unitInfo.position.y));

	SessionInfoPacket* sessionInfoPacket =
		reinterpret_cast<SessionInfoPacket*>(Session::GetSendBuffer()->
			GetBuffer(sizeof(SessionInfoPacket)));
	sessionInfoPacket->Init(SendCommand::Zone2C_USER_REVIVE, sizeof(SessionInfoPacket));

	sessionInfoPacket->info.userInfo = m_basicInfo.userInfo;
	sessionInfoPacket->info.unitInfo = m_basicInfo.unitInfo = m_unitInfo;

	Session::GetSendBuffer()->Write(sessionInfoPacket->size);

	m_field->SectorSendAll(m_sector->GetRoundSectorsVec(), reinterpret_cast<char*>(sessionInfoPacket), sessionInfoPacket->size);
}

void User::Hit(int _index, int _damage)
{
	if (m_basicInfo.unitInfo.state == STATE::DEATH) return;

	int damage = _damage - m_basicInfo.unitInfo.def;
	if (damage < 0) damage = 0;

	m_basicInfo.unitInfo.hp.currentValue -= damage;

	UserHitPacket* userHitPacket =
		reinterpret_cast<UserHitPacket*>(Session::GetSendBuffer()->
			GetBuffer(sizeof(UserHitPacket)));

	userHitPacket->userIndex = m_basicInfo.userInfo.userID;
	userHitPacket->monsterIndex = _index;
	userHitPacket->damage = damage;
	userHitPacket->Init(SendCommand::Zone2C_USER_HIT, sizeof(UserHitPacket));
	m_field->SectorSendAll(m_sector->GetRoundSectorsVec(), reinterpret_cast<char*>(userHitPacket), userHitPacket->size);

	if (m_basicInfo.unitInfo.hp.currentValue <= 0)
	{
		Death();
	}
}

void User::PlusExp(int _exp)
{
	m_basicInfo.unitInfo.exp.currentValue += _exp;

	UserExpPacket* userExpPacket =
		reinterpret_cast<UserExpPacket*>(Session::GetSendBuffer()->
			GetBuffer(sizeof(UserExpPacket)));

	userExpPacket->exp = _exp;
	userExpPacket->Init(SendCommand::Zone2C_USER_PLUS_EXP, sizeof(UserExpPacket));
	Session::GetSendBuffer()->Write(userExpPacket->size);

	Session::Send(reinterpret_cast<char*>(userExpPacket), userExpPacket->size);

	//이후 레벨업 체크해줘야함
	if (m_basicInfo.unitInfo.exp.currentValue >= m_basicInfo.unitInfo.exp.maxValue)
	{
		LevelUp();
	}
}

void User::LevelUp()
{
	int tempExp = m_basicInfo.unitInfo.exp.currentValue - m_basicInfo.unitInfo.exp.maxValue;

	m_basicInfo.unitInfo.level++;
	m_basicInfo.unitInfo.exp.currentValue = tempExp;
	m_basicInfo.unitInfo.hp.maxValue += 50;
	m_basicInfo.unitInfo.hp.currentValue = m_basicInfo.unitInfo.hp.maxValue;
	m_basicInfo.unitInfo.mp.currentValue = m_basicInfo.unitInfo.mp.maxValue;
	m_basicInfo.unitInfo.atk += 5;
	m_basicInfo.unitInfo.def += 2;

	UserNumPacket* userNumPacket =
		reinterpret_cast<UserNumPacket*>(Session::GetSendBuffer()->
			GetBuffer(sizeof(UserNumPacket)));

	userNumPacket->userIndex = m_basicInfo.userInfo.userID;
	userNumPacket->Init(SendCommand::Zone2C_USER_LEVEL_UP, sizeof(UserNumPacket));
	Session::GetSendBuffer()->Write(userNumPacket->size);

	m_field->SectorSendAll(m_sector->GetRoundSectorsVec(), reinterpret_cast<char*>(userNumPacket), userNumPacket->size);

	//다시 레벨업 체크
	if (m_basicInfo.unitInfo.exp.currentValue >= m_basicInfo.unitInfo.exp.maxValue)
	{
		LevelUp();
	}
}

//기본적인 유저의 정보를 보내준다. DB가 있으면 여기서 불러와서 보내줌.
void User::SendInfo()
{
	SessionInfoPacket* sessionInfoPacket =
		reinterpret_cast<SessionInfoPacket*>(Session::GetSendBuffer()->
			GetBuffer(sizeof(SessionInfoPacket)));
	sessionInfoPacket->Init(SendCommand::Zone2C_REGISTER_USER, sizeof(SessionInfoPacket));
	Session::GetSendBuffer()->Write(sessionInfoPacket->size);

	//======================================================

	if (!MYSQLCLASS->GetUserInfo(m_basicInfo.userInfo.userID,
		m_basicInfo.userInfo.userName, m_basicInfo.unitInfo.level,
		m_basicInfo.unitInfo.hp.currentValue, m_basicInfo.unitInfo.hp.maxValue,
		m_basicInfo.unitInfo.mp.currentValue, m_basicInfo.unitInfo.mp.maxValue,
		m_basicInfo.unitInfo.exp.currentValue, m_basicInfo.unitInfo.exp.maxValue,
		m_basicInfo.unitInfo.atk, m_basicInfo.unitInfo.def))
	{
		printf("[ GetUserInfo Failed, Disconnect %d Socket User ]\n", m_socket);
		Disconnect();
	}

	sessionInfoPacket->info.userInfo = m_basicInfo.userInfo;
	sessionInfoPacket->info.unitInfo = m_unitInfo = m_basicInfo.unitInfo;

	//======================================================

	Session::Send(reinterpret_cast<char*>(sessionInfoPacket), sessionInfoPacket->size);

	printf("[ REGISTER_USER 전송 완료 ]\n");
}

//받아온 Field의 Num값을 User에 넣어주고 완료했다는 패킷 보냄. 이후 클라에선 씬 전환해줄듯
void User::EnterField(Field *_field, int _fieldNum, VECTOR2 _spawnPosition)
{
	EnterFieldPacket* enterFieldPacket =
		reinterpret_cast<EnterFieldPacket*>(Session::GetSendBuffer()->
			GetBuffer(sizeof(EnterFieldPacket)));
	enterFieldPacket->fieldNum = _fieldNum;

	enterFieldPacket->position = _spawnPosition;
	m_basicInfo.unitInfo.position.x = _spawnPosition.x;
	m_basicInfo.unitInfo.position.y = _spawnPosition.y;

	enterFieldPacket->Init(SendCommand::Zone2C_ENTER_FIELD, sizeof(EnterFieldPacket));
	Session::GetSendBuffer()->Write(enterFieldPacket->size);

	m_field = _field;
	m_basicInfo.unitInfo.fieldNum = m_field->GetFieldNum();
	m_fieldTilesData = m_field->GetTilesData();

	m_tile = m_fieldTilesData->GetTile(
		static_cast<int>(m_basicInfo.unitInfo.position.x),
		static_cast<int>(m_basicInfo.unitInfo.position.y));

	Session::Send(reinterpret_cast<char*>(enterFieldPacket), enterFieldPacket->size);

	printf("[ ENTER_FIELD 전송 완료 ]\n");
}

void User::SetPosition(Position& _position)
{
	float distance = VECTOR2(m_basicInfo.unitInfo.position - _position).Magnitude();

	/*printf("[%d User : SetPosition (%f, %f) - dir : %f ]\n",
		m_basicInfo.userInfo.userID, _position.x, _position.y, distance);*/

	m_basicInfo.unitInfo.position = _position;
	m_tile = m_fieldTilesData->GetTile(
		static_cast<int>(m_basicInfo.unitInfo.position.x),
		static_cast<int>(m_basicInfo.unitInfo.position.y));
}

bool User::LogInUser(LogInPacket* _packet)
{
	//해당 id 존재함 && password 일치함
	if (MYSQLCLASS->OverlappedCheck(_packet->id, _packet->password, m_basicInfo.userInfo.userID))
	{
		return true;
	}
	//해당 id 존재하지 않음 or password 일치하지 않음
	else
	{
		Packet* LogInFailed = reinterpret_cast<Packet*>(Session::GetSendBuffer()->
			GetBuffer(sizeof(Packet)));
		LogInFailed->Init(SendCommand::Zone2C_LOGIN_FAILED, sizeof(Packet));
		printf("[ Login Failed ] \n");

		Session::Send(LogInFailed);

		return false;
	}
}

void User::RegisterUser(RegisterUserPacket* _packet)
{
	//true -> 중복이 없어서 회원가입 성공
	if (MYSQLCLASS->InsertData(_packet->id, _packet->password))
	{
		Packet* RegisterSuccessPacket = reinterpret_cast<Packet*>(Session::GetSendBuffer()->
			GetBuffer(sizeof(Packet)));
		RegisterSuccessPacket->Init(SendCommand::Zone2C_REGISTER_USER_SUCCESS, sizeof(Packet));

		Session::Send(RegisterSuccessPacket);
	}
	//false -> 중복이 있어서 회원가입 실패
	else
	{
		Packet* RegisterFailedPacket = reinterpret_cast<Packet*>(Session::GetSendBuffer()->
			GetBuffer(sizeof(Packet)));
		RegisterFailedPacket->Init(SendCommand::Zone2C_REGISTER_USER_FAILED, sizeof(Packet));

		Session::Send(RegisterFailedPacket);
	}
}

void User::LogInDuplicated()
{
	Packet* LogInFailed = reinterpret_cast<Packet*>(Session::GetSendBuffer()->
		GetBuffer(sizeof(Packet)));
	LogInFailed->Init(SendCommand::Zone2C_LOGIN_FAILED_DUPLICATED, sizeof(Packet));

	Session::Send(LogInFailed);
}

void User::LogInSuccess()
{
	Packet* LogInSuccess = reinterpret_cast<Packet*>(Session::GetSendBuffer()->
		GetBuffer(sizeof(Packet)));
	LogInSuccess->Init(SendCommand::Zone2C_LOGIN_SUCCESS, sizeof(Packet));
	printf("[ Login Success ] \n");

	Session::Send(LogInSuccess);
}

bool User::CompareSector(Sector* _sector)
{
	if (m_sector == nullptr) return false;

	if (m_sector == _sector)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//테스트용
void User::TestClientEnterField(Field* _field, int _fieldNum, int _dummyNum, VECTOR2 _spawnPosition)
{
	m_isTestClient = true;

	m_field = _field;
	m_basicInfo.unitInfo.fieldNum = m_field->GetFieldNum();
	m_fieldTilesData = m_field->GetTilesData();

	m_basicInfo.userInfo.userID = _dummyNum;
	strcpy_s(m_basicInfo.userInfo.userName, "TestPlayer");

	Unit::SetUnitInfo(IDLE, 1,
		100, 100, 100, 100, 100, 0, 20, 0);
	Unit::SetUnitPosition(0, 0);

	m_basicInfo.unitInfo.position.x = _spawnPosition.x;
	m_basicInfo.unitInfo.position.y = _spawnPosition.y;

	m_tile = m_fieldTilesData->GetTile(
		static_cast<int>(m_basicInfo.unitInfo.position.x),
		static_cast<int>(m_basicInfo.unitInfo.position.y));

	SessionInfoPacket* sessionInfoPacket =
		reinterpret_cast<SessionInfoPacket*>(Session::GetSendBuffer()->
			GetBuffer(sizeof(SessionInfoPacket)));
	sessionInfoPacket->Init(SendCommand::Zone2C_REGISTER_TEST_USER, sizeof(SessionInfoPacket));
	Session::GetSendBuffer()->Write(sessionInfoPacket->size);

	sessionInfoPacket->info.userInfo = m_basicInfo.userInfo;
	sessionInfoPacket->info.unitInfo = m_unitInfo = m_basicInfo.unitInfo;

	Session::Send(reinterpret_cast<char*>(sessionInfoPacket), sessionInfoPacket->size);
}

/*void User::TestPathFind(list<VECTOR2>* _list)
{
	//메모리 릭 의심
	//길찾기를 부분은 클라이언트 부분으로.
	m_pathFinding.PathFind(&m_tileList, m_tile, _tile);

	//20.05.04
	//erase(begin())하면 m_targetPosition의 정보가 같이 사라졌다.
	//현재는 iter부분 제외하고 단순히 front 및 pop_front를 사용하면 해결됨.
	m_targetPosition = m_tileList.front();
	m_tileList.pop_front();

	m_tileList = *_list;

	m_targetPosition = m_tileList.front();
	m_tileList.pop_front();

	m_basicInfo.unitInfo.state = STATE::MOVE;
}

void User::TestMove()
{
	//printf("[ 1 ( %f, %f ) ]\n",  m_basicInfo.unitInfo.position.x, m_basicInfo.unitInfo.position.y);

	VECTOR2 temp1;
	VECTOR2 temp2;
	temp2.x = m_basicInfo.unitInfo.position.x;
	temp2.y = m_basicInfo.unitInfo.position.y;
	//============================================================================
	VECTOR2 tempVec2 = VECTOR2(m_targetPosition - m_basicInfo.unitInfo.position);
	float magnitude = tempVec2.Magnitude();

	if (magnitude <= 3.5f * (1.0f / 30.0f) || magnitude == 0.0f)
	{
		temp1 = m_targetPosition;

		m_basicInfo.unitInfo.position.x = temp1.x;
		m_basicInfo.unitInfo.position.y = temp1.y;
	}
		
	//두 직선, 벡터의 관계(사이각,회전각) 구하기
	//https://darkpgmr.tistory.com/121
	//VECTOR2(m_targetPosition - m_info.position) / magnitude 이 부분

	//https://www.youtube.com/watch?v=WxWJorOVIj8
	//https://www.youtube.com/watch?v=-gokuNjpyNg

	temp2 = temp2 + tempVec2 / magnitude * 3.5f * (1.0f / 10.0f);
	//각도 * 이동속도 * 시간

	m_basicInfo.unitInfo.position.x = temp2.x;
	m_basicInfo.unitInfo.position.y = temp2.y;

	//printf("[ 2 ( %f, %f ) ]\n", m_basicInfo.unitInfo.position.x, m_basicInfo.unitInfo.position.y);
}

bool User::PathMove()
{
	VECTOR2 temp;
	temp.x = m_basicInfo.unitInfo.position.x;
	temp.y = m_basicInfo.unitInfo.position.y;

	float distance = VECTOR2(temp - m_targetPosition).SqrMagnitude();

	//목표 좌표에 도착
	if (distance <= 0.1f)
	{
		//printf("1 \n");

		m_basicInfo.unitInfo.position.x = m_targetPosition.x;
		m_basicInfo.unitInfo.position.y = m_targetPosition.y;
		m_tile = m_fieldTilesData->GetTile(
			static_cast<int>(m_basicInfo.unitInfo.position.x),
			static_cast<int>(m_basicInfo.unitInfo.position.y));

		m_field->UpdateUserSector(this);

		UserPositionPacket* userPositionPacket =
			reinterpret_cast<UserPositionPacket*>(m_sendBuffer->
				GetBuffer(sizeof(UserPositionPacket)));
		userPositionPacket->userIndex = m_basicInfo.userInfo.userID;
		userPositionPacket->position = m_basicInfo.unitInfo.position;

		userPositionPacket->Init(SendCommand::Zone2C_USER_MOVE, sizeof(UserPositionPacket));
		Session::GetSendBuffer()->Write(userPositionPacket->size);

		m_field->SectorSendAll(m_sector->GetRoundSectorsVec(),
			reinterpret_cast<char*>(userPositionPacket), userPositionPacket->size);

		//printf("[ 3 ( %f, %f ) ]\n", m_basicInfo.unitInfo.position.x, m_basicInfo.unitInfo.position.y);

		/*printf("[ %d test user : now position ( %f, %f ) ]\n", m_basicInfo.userInfo.userID,
			m_basicInfo.unitInfo.position.x, m_basicInfo.unitInfo.position.y);*/

		//남은 타일리스트가 있다면
		if (m_tileList.size() > 0)
		{
			//printf("2 \n");

			m_targetPosition = m_tileList.front();
			m_tileList.pop_front();

			/*MonsterPositionPacket* monsterPositionPacket =
				reinterpret_cast<MonsterPositionPacket*>(m_sendBuffer->
					GetBuffer(sizeof(MonsterPositionPacket)));

			monsterPositionPacket->monsterIndex = m_info.index;
			monsterPositionPacket->position = m_info.position;
			monsterPositionPacket->Init(SendCommand::Zone2C_MONSTER_MOVE, sizeof(MonsterPositionPacket));

			m_sendBuffer->Write(monsterPositionPacket->size);
			packetQueue.AddItem(PacketQueuePair(this, monsterPositionPacket));*/
		}
		//남은 타일리스트가 없다면
		else
		{
			//printf("3 \n");

			TestClientStatePacket* testClientStatePacket =
				reinterpret_cast<TestClientStatePacket*>(m_sendBuffer->
					GetBuffer(sizeof(TestClientStatePacket)));
			testClientStatePacket->state = STATE::IDLE;
			testClientStatePacket->vec2 =
			{ m_basicInfo.unitInfo.position.x, m_basicInfo.unitInfo.position.y };
			testClientStatePacket->Init(SendCommand::Zone2C_UPDATE_STATE_TEST_USER, sizeof(TestClientStatePacket));

			Session::GetSendBuffer()->Write(testClientStatePacket->size);

			Session::Send(reinterpret_cast<char*>(testClientStatePacket), testClientStatePacket->size);

			/*printf("[ %d test user : now position ( %f, %f ) ]\n", m_basicInfo.userInfo.userID,
				m_basicInfo.unitInfo.position.x, m_basicInfo.unitInfo.position.y);*/

			return false;
		}
	}

	//printf("4 \n");

	return true;
}*/