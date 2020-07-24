#include "User.h"
#include "Field.h"
#include "Sector.h"

#include "DBConnector.h"
#include "MainThread.h"

User::User()
{
	m_basicInfo.unitInfo.Reset();
	m_basicInfo.userInfo.Reset();

	m_tile = nullptr;
	m_sector = nullptr;
	m_field = nullptr;
	m_fieldTilesData = nullptr;

	m_isGetUserList = false;

	m_heartBeatCheckedCount = 0;
}

User::~User()
{
	DisConnect();
	//Reset();
}

void User::OnConnect()
{
	ClientSession::OnConnect();

	IsConnectedPacket* isConnectedPacket =
		reinterpret_cast<IsConnectedPacket*>(m_sendBuffer->
			GetBuffer(sizeof(IsConnectedPacket)));

	isConnectedPacket->Init(SendCommand::Zone2C_ISCONNECTED, sizeof(IsConnectedPacket));
	isConnectedPacket->isConnected = m_isConnected;

	Send(reinterpret_cast<char*>(isConnectedPacket), isConnectedPacket->size);

	//printf("[ isConnectedPacket 전송 완료 ]\n");
}

void User::DisConnect()
{
	ClientSession::DisConnect();

	int errorNum = WSAGetLastError();
	if (errorNum != 0)
	{
		printf("%d Error \n", errorNum);
	}

	printf("===== [ close socket : %d ] ===== \n", m_socket);

	shutdown(m_socket, SD_BOTH);
	//shutdown 이후 close
	closesocket(m_socket); //순서상 다른 곳에서 해도된다.

	m_heartBeatCheckedCount = 0;

	MainThread::getSingleton()->AddToDisConnectQueue(this);
}

void User::Reset()
{
	ClientSession::Reset();

	m_basicInfo.unitInfo.Reset();
	m_basicInfo.userInfo.Reset();

	m_field = nullptr;

	m_isGetUserList = false;

	m_sector = nullptr;

	m_basicInfo.unitInfo.fieldNum = 0;
}

void User::OnRecv()
{
	int tempNum = 20;

	while (1)
	{
		Packet* packet = reinterpret_cast<Packet*>(m_receiver->GetPacket());

		if (packet == nullptr) break;

		//Heap에다 넣는 것
		//MainThread::PacketQueuePair_User* temp = new MainThread::PacketQueuePair_User(this, packet);
		//MainThread::getSingleton()->AddToUserPacketQueue(temp);
		//상수로 넣는것(상수니까 주소바뀔 수 없다.)
		MainThread::getSingleton()->AddToUserPacketQueue({ this, packet });

		tempNum--;

		if (tempNum <= 0)
		{
			tempNum = 20;

			break;
		}
	}
}

void User::HeartBeatChecked()
{
	m_start = std::chrono::system_clock::now();

	printf("Check \n");

	if (m_heartBeatCheckedCount >= 3)
	{
		if (!m_isTestClient)
		{
			Packet* UpdateInfoPacket = reinterpret_cast<Packet*>(m_sendBuffer->
				GetBuffer(sizeof(Packet)));
			UpdateInfoPacket->Init(SendCommand::Zone2C_UPDATE_INFO, sizeof(Packet));
			//m_sendBuffer->Write(UpdateInfoPacket->size);

			Send(reinterpret_cast<char*>(UpdateInfoPacket), UpdateInfoPacket->size);
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
	MYDEBUG("[ %d user - INFO UPDATE TO DATABASE START ] \n", m_basicInfo.userInfo.userID);

	UpdateUserPacket* updateUserPacket =
		reinterpret_cast<UpdateUserPacket*>(m_sendBuffer->
			GetBuffer(sizeof(UpdateUserPacket)));
	updateUserPacket->Init(SendCommand::Zone2DB_UPDATE_USER, sizeof(UpdateUserPacket));
	//m_sendBuffer->Write(updateUserPacket->size);
	updateUserPacket->userIndex = m_basicInfo.userInfo.userID;
	updateUserPacket->unitInfo = m_basicInfo.unitInfo;
	updateUserPacket->socket = m_socket;

	DBCONNECTOR->Send(reinterpret_cast<char*>(updateUserPacket), updateUserPacket->size);
}

void User::Death()
{
	m_basicInfo.unitInfo.state = STATE::DEATH;

	//죽었다고 패킷 전송해줘야함.
	UserNumPacket* userNumPacket =
		reinterpret_cast<UserNumPacket*>(m_sendBuffer->
			GetBuffer(sizeof(UserNumPacket)));

	userNumPacket->userIndex = m_basicInfo.userInfo.userID;
	userNumPacket->Init(SendCommand::Zone2C_USER_DEATH, sizeof(UserNumPacket));
	//m_sendBuffer->Write(userNumPacket->size);

	m_field->SectorSendAll(m_sector->GetRoundSectorsVec(), reinterpret_cast<char*>(userNumPacket), userNumPacket->size);
}

void User::Respawn(VECTOR2 _spawnPosition)
{
	m_basicInfo.unitInfo.state = STATE::IDLE;

	m_basicInfo.unitInfo.hp.currentValue = m_basicInfo.unitInfo.hp.maxValue;
	m_basicInfo.unitInfo.mp.currentValue = m_basicInfo.unitInfo.mp.maxValue;

	m_basicInfo.unitInfo.SetUnitPosition(_spawnPosition.x, _spawnPosition.y);
	m_tile = m_fieldTilesData->GetTile(
		static_cast<int>(m_basicInfo.unitInfo.position.x),
		static_cast<int>(m_basicInfo.unitInfo.position.y));

	SessionInfoPacket* sessionInfoPacket =
		reinterpret_cast<SessionInfoPacket*>(m_sendBuffer->
			GetBuffer(sizeof(SessionInfoPacket)));
	sessionInfoPacket->Init(SendCommand::Zone2C_USER_REVIVE, sizeof(SessionInfoPacket));
	//m_sendBuffer->Write(sessionInfoPacket->size);

	sessionInfoPacket->info.userInfo = m_basicInfo.userInfo;
	sessionInfoPacket->info.unitInfo = m_basicInfo.unitInfo;

	m_field->SectorSendAll(m_sector->GetRoundSectorsVec(), reinterpret_cast<char*>(sessionInfoPacket), sessionInfoPacket->size);
}

void User::Hit(int _index, int _damage)
{
	if (m_basicInfo.unitInfo.state == STATE::DEATH) return;

	int damage = _damage - m_basicInfo.unitInfo.def;
	if (damage < 0) damage = 0;

	m_basicInfo.unitInfo.hp.currentValue -= damage;

	UserHitPacket* userHitPacket =
		reinterpret_cast<UserHitPacket*>(m_sendBuffer->
			GetBuffer(sizeof(UserHitPacket)));

	userHitPacket->userIndex = m_basicInfo.userInfo.userID;
	userHitPacket->monsterIndex = _index;
	userHitPacket->damage = damage;
	userHitPacket->Init(SendCommand::Zone2C_USER_HIT, sizeof(UserHitPacket));
	//m_sendBuffer->Write(userHitPacket->size);

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
		reinterpret_cast<UserExpPacket*>(m_sendBuffer->
			GetBuffer(sizeof(UserExpPacket)));

	userExpPacket->exp = _exp;
	userExpPacket->Init(SendCommand::Zone2C_USER_PLUS_EXP, sizeof(UserExpPacket));
	//m_sendBuffer->Write(userExpPacket->size);

	Send(reinterpret_cast<char*>(userExpPacket), userExpPacket->size);

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
		reinterpret_cast<UserNumPacket*>(m_sendBuffer->
			GetBuffer(sizeof(UserNumPacket)));

	userNumPacket->userIndex = m_basicInfo.userInfo.userID;
	userNumPacket->Init(SendCommand::Zone2C_USER_LEVEL_UP, sizeof(UserNumPacket));
	//m_sendBuffer->Write(userNumPacket->size);

	m_field->SectorSendAll(m_sector->GetRoundSectorsVec(), reinterpret_cast<char*>(userNumPacket), userNumPacket->size);

	//다시 레벨업 체크
	if (m_basicInfo.unitInfo.exp.currentValue >= m_basicInfo.unitInfo.exp.maxValue)
	{
		LevelUp();
	}
}

void User::RequestUserInfo()
{
	RequireUserInfoPacket_DBAgent* RequireUserInfoPacket =
		reinterpret_cast<RequireUserInfoPacket_DBAgent*>(m_sendBuffer->
			GetBuffer(sizeof(RequireUserInfoPacket_DBAgent)));
	RequireUserInfoPacket->Init(SendCommand::Zone2DB_REQUEST_USER_DATA, sizeof(RequireUserInfoPacket_DBAgent));
	//m_sendBuffer->Write(RequireUserInfoPacket->size);
	RequireUserInfoPacket->userIndex = m_basicInfo.userInfo.userID;
	RequireUserInfoPacket->socket = m_socket;

	DBCONNECTOR->Send(reinterpret_cast<char*>(RequireUserInfoPacket), RequireUserInfoPacket->size);
}

//기본적인 유저의 정보를 보내준다. DB가 있으면 여기서 불러와서 보내줌.
void User::SendInfo(GetSessionInfoPacket* _packet)
{
	int tempNum = m_basicInfo.userInfo.userID;
	m_basicInfo.userInfo = _packet->info.userInfo;
	m_basicInfo.userInfo.userID = tempNum;
	m_basicInfo.unitInfo = _packet->info.unitInfo;

	SessionInfoPacket* sessionInfoPacket =
		reinterpret_cast<SessionInfoPacket*>(m_sendBuffer->
			GetBuffer(sizeof(SessionInfoPacket)));
	sessionInfoPacket->Init(SendCommand::Zone2C_REGISTER_USER, sizeof(SessionInfoPacket));
	//m_sendBuffer->Write(sessionInfoPacket->size);

	sessionInfoPacket->info.userInfo = m_basicInfo.userInfo;
	sessionInfoPacket->info.unitInfo = m_basicInfo.unitInfo;

	Send(reinterpret_cast<char*>(sessionInfoPacket), sessionInfoPacket->size);

	MYDEBUG("[ REGISTER_USER 전송 완료 ]\n");
}

void User::RequestUserInfoFailed()
{
	MYDEBUG("[ GetUserInfo Failed, Disconnect %d Socket User ]\n", m_socket);

	DisConnect();
}

//받아온 Field의 Num값을 User에 넣어주고 완료했다는 패킷 보냄. 이후 클라에선 씬 전환해줄듯
void User::EnterField(Field *_field, int _fieldNum, VECTOR2 _spawnPosition)
{
	EnterFieldPacket* enterFieldPacket =
		reinterpret_cast<EnterFieldPacket*>(m_sendBuffer->
			GetBuffer(sizeof(EnterFieldPacket)));
	enterFieldPacket->fieldNum = _fieldNum;

	enterFieldPacket->position = _spawnPosition;
	m_basicInfo.unitInfo.position.x = _spawnPosition.x;
	m_basicInfo.unitInfo.position.y = _spawnPosition.y;

	enterFieldPacket->Init(SendCommand::Zone2C_ENTER_FIELD, sizeof(EnterFieldPacket));
	//m_sendBuffer->Write(enterFieldPacket->size);

	m_field = _field;
	m_basicInfo.unitInfo.fieldNum = m_field->GetFieldNum();
	m_fieldTilesData = m_field->GetTilesData();

	m_tile = m_fieldTilesData->GetTile(
		static_cast<int>(m_basicInfo.unitInfo.position.x),
		static_cast<int>(m_basicInfo.unitInfo.position.y));

	Send(reinterpret_cast<char*>(enterFieldPacket), enterFieldPacket->size);

	MYDEBUG("[ ENTER_FIELD 전송 완료 ]\n");
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

void User::LogInUser(LogInPacket* _packet)
{
	LogInPacket_DBAgent* logInPacket_DBAgent =
		reinterpret_cast<LogInPacket_DBAgent*>(m_sendBuffer->
			GetBuffer(sizeof(LogInPacket_DBAgent)));
	logInPacket_DBAgent->Init(SendCommand::Zone2DB_LOGIN, sizeof(LogInPacket_DBAgent));
	//m_sendBuffer->Write(logInPacket_DBAgent->size);
	logInPacket_DBAgent->socket = m_socket;
	strcpy_s(logInPacket_DBAgent->id, _packet->id);
	strcpy_s(logInPacket_DBAgent->password, _packet->password);

	DBCONNECTOR->Send(reinterpret_cast<char*>(logInPacket_DBAgent), logInPacket_DBAgent->size);
}

void User::RegisterUser(RegisterUserPacket* _packet)
{
	RegisterPacket_DBAgent* registerPacket_DBAgent =
		reinterpret_cast<RegisterPacket_DBAgent*>(m_sendBuffer->
			GetBuffer(sizeof(RegisterPacket_DBAgent)));
	registerPacket_DBAgent->Init(SendCommand::Zone2DB_REGISTER, sizeof(RegisterPacket_DBAgent));
	//m_sendBuffer->Write(registerPacket_DBAgent->size);
	registerPacket_DBAgent->socket = m_socket;
	strcpy_s(registerPacket_DBAgent->id, _packet->id);
	strcpy_s(registerPacket_DBAgent->password, _packet->password);

	DBCONNECTOR->Send(reinterpret_cast<char*>(registerPacket_DBAgent), registerPacket_DBAgent->size);
}

void User::LogInDuplicated()
{
	Packet* LogInFailed = reinterpret_cast<Packet*>(m_sendBuffer->
		GetBuffer(sizeof(Packet)));
	LogInFailed->Init(SendCommand::Zone2C_LOGIN_FAILED_DUPLICATED, sizeof(Packet));
	//m_sendBuffer->Write(LogInFailed->size);

	Send(reinterpret_cast<char*>(LogInFailed), LogInFailed->size);
}

void User::LogInSuccess(int _num)
{
	m_basicInfo.userInfo.userID = _num;

	Packet* LogInSuccess = reinterpret_cast<Packet*>(m_sendBuffer->
		GetBuffer(sizeof(Packet)));
	LogInSuccess->Init(SendCommand::Zone2C_LOGIN_SUCCESS, sizeof(Packet));
	//m_sendBuffer->Write(LogInSuccess->size);
	MYDEBUG("[ Login Success ] \n");

	Send(reinterpret_cast<char*>(LogInSuccess), LogInSuccess->size);
}

void User::LogInFailed()
{
	Packet* LogInFailed = reinterpret_cast<Packet*>(m_sendBuffer->
		GetBuffer(sizeof(Packet)));
	LogInFailed->Init(SendCommand::Zone2C_LOGIN_FAILED, sizeof(Packet));
	//m_sendBuffer->Write(LogInFailed->size);
	MYDEBUG("[ Login Failed ] \n");

	Send(reinterpret_cast<char*>(LogInFailed), LogInFailed->size);
}

void User::RegisterSuccess()
{
	Packet* RegisterSuccessPacket = reinterpret_cast<Packet*>(m_sendBuffer->
		GetBuffer(sizeof(Packet)));
	RegisterSuccessPacket->Init(SendCommand::Zone2C_REGISTER_USER_SUCCESS, sizeof(Packet));
	//m_sendBuffer->Write(RegisterSuccessPacket->size);

	Send(reinterpret_cast<char*>(RegisterSuccessPacket), RegisterSuccessPacket->size);
}

void User::RegisterFailed()
{
	Packet* RegisterFailedPacket = reinterpret_cast<Packet*>(m_sendBuffer->
		GetBuffer(sizeof(Packet)));
	RegisterFailedPacket->Init(SendCommand::Zone2C_REGISTER_USER_FAILED, sizeof(Packet));
	//m_sendBuffer->Write(RegisterFailedPacket->size);

	Send(reinterpret_cast<char*>(RegisterFailedPacket), RegisterFailedPacket->size);
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

	m_basicInfo.unitInfo.SetUnitInfo(IDLE, 1,
		100, 100, 100, 100, 100, 0, 20, 0);
	m_basicInfo.unitInfo.SetUnitPosition(0, 0);

	m_basicInfo.unitInfo.position.x = _spawnPosition.x;
	m_basicInfo.unitInfo.position.y = _spawnPosition.y;

	m_tile = m_fieldTilesData->GetTile(
		static_cast<int>(m_basicInfo.unitInfo.position.x),
		static_cast<int>(m_basicInfo.unitInfo.position.y));

	SessionInfoPacket* sessionInfoPacket =
		reinterpret_cast<SessionInfoPacket*>(m_sendBuffer->
			GetBuffer(sizeof(SessionInfoPacket)));
	sessionInfoPacket->Init(SendCommand::Zone2C_REGISTER_TEST_USER, sizeof(SessionInfoPacket));
	//m_sendBuffer->Write(sessionInfoPacket->size);

	sessionInfoPacket->info.userInfo = m_basicInfo.userInfo;
	sessionInfoPacket->info.unitInfo = m_basicInfo.unitInfo;

	Send(reinterpret_cast<char*>(sessionInfoPacket), sessionInfoPacket->size);
}