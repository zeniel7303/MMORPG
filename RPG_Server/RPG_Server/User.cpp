#include "User.h"
#include "Zone.h"
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

	m_zone = nullptr;
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

	isConnectedPacket->Init(SendCommand::ISCONNECTED, sizeof(IsConnectedPacket));
	isConnectedPacket->isConnected = m_isConnected;

	Session::Send(isConnectedPacket);

	printf("[ isConnectedPacket 전송 완료 ]\n");
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

	m_zone = nullptr;
	m_startCheckingHeartBeat = false;
	m_isCheckingHeartBeat = false;
	m_isTestClient = false;

	m_sector = nullptr;
	m_basicInfo.unitInfo.zoneNum = 0;
}

void User::HeartBeatChecked()
{
	if (m_heartBeatCheckedCount >= 6)
	{
		Packet* UpdateInfoPacket = reinterpret_cast<Packet*>(Session::GetSendBuffer()->
			GetBuffer(sizeof(Packet)));
		UpdateInfoPacket->Init(SendCommand::UPDATE_INFO, sizeof(Packet));

		Session::Send(UpdateInfoPacket);

		m_heartBeatCheckedCount = 0;

		printf("[ HeartBeat Checking & Update Success : %d ]\n", m_heartBeatCheckedCount);
	}
	else
	{
		m_heartBeatCheckedCount++;

		printf("[ HeartBeat Checking Success : %d ]\n", m_heartBeatCheckedCount);
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
	userNumPacket->Init(SendCommand::USER_DEATH, sizeof(UserNumPacket));

	m_zone->SectorSendAll(m_sector, m_sectors, reinterpret_cast<char*>(userNumPacket), userNumPacket->size);
}

void User::Respawn(VECTOR2 _spawnPosition)
{
	m_basicInfo.unitInfo.state = STATE::IDLE;

	m_basicInfo.unitInfo.hp.currentValue = m_basicInfo.unitInfo.hp.maxValue;
	m_basicInfo.unitInfo.mp.currentValue = m_basicInfo.unitInfo.mp.maxValue;

	Unit::SetUnitPosition(_spawnPosition.x, _spawnPosition.y);
	m_tile = m_zoneTilesData->GetTile(
		static_cast<int>(m_basicInfo.unitInfo.position.x),
		static_cast<int>(m_basicInfo.unitInfo.position.y));

	SessionInfoPacket* sessionInfoPacket =
		reinterpret_cast<SessionInfoPacket*>(Session::GetSendBuffer()->
			GetBuffer(sizeof(SessionInfoPacket)));
	sessionInfoPacket->Init(SendCommand::USER_REVIVE, sizeof(SessionInfoPacket));

	sessionInfoPacket->info.userInfo = m_basicInfo.userInfo;
	sessionInfoPacket->info.unitInfo = m_basicInfo.unitInfo = m_unitInfo;

	Session::GetSendBuffer()->Write(sessionInfoPacket->size);

	m_zone->SectorSendAll(m_sector, m_sectors, reinterpret_cast<char*>(sessionInfoPacket), sessionInfoPacket->size);
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
	userHitPacket->Init(SendCommand::USER_HIT, sizeof(UserHitPacket));
	m_zone->SectorSendAll(m_sector, m_sectors, reinterpret_cast<char*>(userHitPacket), userHitPacket->size);

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
	userExpPacket->Init(SendCommand::USER_PLUS_EXP, sizeof(UserExpPacket));
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
	userNumPacket->Init(SendCommand::USER_LEVEL_UP, sizeof(UserNumPacket));
	Session::GetSendBuffer()->Write(userNumPacket->size);

	m_zone->SectorSendAll(m_sector, m_sectors, reinterpret_cast<char*>(userNumPacket), userNumPacket->size);

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
	sessionInfoPacket->Init(SendCommand::REGISTER_USER, sizeof(SessionInfoPacket));
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

	/*m_basicInfo.userInfo.userID = m_index;
	strcpy_s(m_basicInfo.userInfo.userName, "TestPlayer");

	m_basicInfo.unitInfo.zoneNum = 0;
	Unit::SetUnitInfo(IDLE, 1,
		100, 100, 100, 100, 100, 0, 20, 0);
	Unit::SetUnitPosition(0, 0);*/

	sessionInfoPacket->info.userInfo = m_basicInfo.userInfo;
	sessionInfoPacket->info.unitInfo = m_unitInfo = m_basicInfo.unitInfo;

	//======================================================

	Session::Send(reinterpret_cast<char*>(sessionInfoPacket), sessionInfoPacket->size);

	printf("[ REGISTER_USER 전송 완료 ]\n");
}

//테스트용
void User::TestClientEnterZone(Zone* _zone, int _zoneNum)
{
	m_zone = _zone;
	m_basicInfo.unitInfo.zoneNum = m_zone->GetZoneNum();

	m_isTestClient = true;
}

//받아온 Zone의 Num값을 User에 넣어주고 완료했다는 패킷 보냄. 이후 클라에선 씬 전환해줄듯
void User::EnterZone(Zone* _zone, int _zoneNum, VECTOR2 _spawnPosition)
{
	EnterZonePacket* enterZonePacket =
		reinterpret_cast<EnterZonePacket*>(Session::GetSendBuffer()->
			GetBuffer(sizeof(EnterZonePacket)));
	enterZonePacket->zoneNum = _zoneNum;

	enterZonePacket->position = _spawnPosition;
	m_basicInfo.unitInfo.position.x = _spawnPosition.x;
	m_basicInfo.unitInfo.position.y = _spawnPosition.y;

	enterZonePacket->Init(SendCommand::ENTER_ZONE, sizeof(EnterZonePacket));
	Session::GetSendBuffer()->Write(enterZonePacket->size);

	m_zone = _zone;
	m_basicInfo.unitInfo.zoneNum = m_zone->GetZoneNum();
	m_zoneTilesData = m_zone->GetTilesData();

	m_tile = m_zoneTilesData->GetTile(
		static_cast<int>(m_basicInfo.unitInfo.position.x),
		static_cast<int>(m_basicInfo.unitInfo.position.y));

	Session::Send(reinterpret_cast<char*>(enterZonePacket), enterZonePacket->size);

	printf("[ ENTER_ZONE 전송 완료 ]\n");
}

void User::SetPosition(Position& _position)
{
	float distance = VECTOR2(m_basicInfo.unitInfo.position - _position).Magnitude();

	/*printf("[%d User : SetPosition (%f, %f) - dir : %f ]\n",
		m_basicInfo.userInfo.userID, _position.x, _position.y, distance);*/

	m_basicInfo.unitInfo.position = _position;
	m_tile = m_zoneTilesData->GetTile(
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
		LogInFailed->Init(SendCommand::LOGIN_FAILED, sizeof(Packet));
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
		RegisterSuccessPacket->Init(SendCommand::REGISTER_USER_SUCCESS, sizeof(Packet));

		Session::Send(RegisterSuccessPacket);
	}
	//false -> 중복이 있어서 회원가입 실패
	else
	{
		Packet* RegisterFailedPacket = reinterpret_cast<Packet*>(Session::GetSendBuffer()->
			GetBuffer(sizeof(Packet)));
		RegisterFailedPacket->Init(SendCommand::REGISTER_USER_FAILED, sizeof(Packet));

		Session::Send(RegisterFailedPacket);
	}
}

void User::LogInDuplicated()
{
	Packet* LogInFailed = reinterpret_cast<Packet*>(Session::GetSendBuffer()->
		GetBuffer(sizeof(Packet)));
	LogInFailed->Init(SendCommand::LOGIN_FAILED_DUPLICATED, sizeof(Packet));

	Session::Send(LogInFailed);
}

void User::LogInSuccess()
{
	Packet* LogInSuccess = reinterpret_cast<Packet*>(Session::GetSendBuffer()->
		GetBuffer(sizeof(Packet)));
	LogInSuccess->Init(SendCommand::LOGIN_SUCCESS, sizeof(Packet));
	printf("[ Login Success ] \n");

	Session::Send(LogInSuccess);
}