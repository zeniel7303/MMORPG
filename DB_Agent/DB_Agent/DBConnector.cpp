#include "DBConnector.h"

#include "DB_Agent.h"

DBConnector::~DBConnector()
{
}

void DBConnector::Init(int _num)
{
	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	hThread = (HANDLE)_beginthreadex(NULL, 0, &ThreadFunc, (void*)this, 0, &threadID);

	m_sendBuffer = new SendBuffer();
	m_sendBuffer->Init(30000);

	m_state = READY;
	m_num = _num + 1;

	m_isConnect = false;
	m_packet = nullptr;

	mysql_init(&m_connect);
}

bool DBConnector::Connect(const char* host,
	const char* username,
	const char* password,
	const char* database,
	const unsigned int port,
	const char* unix_socket,
	const unsigned int client_flag)
{
	if (&connect == NULL)
	{
		printf("%d Error : %s \n", mysql_errno(&m_connect), mysql_error(&m_connect));
		exit(1);
	}

	if (mysql_real_connect(&m_connect, host, username, password, database, port, unix_socket, client_flag) == NULL)
	{
		m_isConnect = false;

		printf("[ DB Connect Failed ]\n");

		return false;
	}

	m_isConnect = true;

	return true;
}

void DBConnector::DisConnect()
{
	if (&connect != NULL)
	{
		mysql_close(&m_connect);

		m_isConnect = false;
	}
}

bool DBConnector::SelectDB(const char* DBname)
{
	return mysql_select_db(&m_connect, DBname);
}

MYSQL_RES* DBConnector::Query(const char* query)
{
	if (!m_isConnect || &m_connect == NULL)
	{
		printf("%d Error : %s \n", mysql_errno(&m_connect), mysql_error(&m_connect));
		//exit(1);
	}

	if (mysql_query(&m_connect, query))
	{
		printf("%d Error : %s \n", mysql_errno(&m_connect), mysql_error(&m_connect));
		//exit(1);
	}

	m_result = mysql_store_result(&m_connect);

	return m_result;
}

void DBConnector::Login(LogInPacket_DBAgent* _packet)
{
	MYSQL_ROW mysql_row;

	char str1[256];
	sprintf(str1, "select *from accounttable where ID = '%s'", _packet->id);

	m_result = this->Query(str1);

	//해당 id 존재하지 않음
	if ((mysql_row = mysql_fetch_row(m_result)) == NULL)
	{
		PacketWithSocket* logInFailedPacket =
			reinterpret_cast<PacketWithSocket*>(m_sendBuffer->
				GetBuffer(sizeof(PacketWithSocket)));
		logInFailedPacket->Init(SendCommand::DB2Zone_LOGIN_FAILED_INVALID_ID, sizeof(PacketWithSocket));
		logInFailedPacket->socket = _packet->socket;

		m_packetQueue.AddItem(logInFailedPacket);
	}
	//해당 id 존재함
	else
	{
		int result = strcmp(mysql_row[1], _packet->password);

		//비밀번호 일치
		if (result == 0)
		{
			LogInSuccessPacket* logInSuccessPacket =
			reinterpret_cast<LogInSuccessPacket*>(m_sendBuffer->
				GetBuffer(sizeof(LogInSuccessPacket)));
			logInSuccessPacket->Init(SendCommand::DB2Zone_LOGIN_SUCCESS, sizeof(LogInSuccessPacket));
			logInSuccessPacket->socket = _packet->socket;
			logInSuccessPacket->userIndex = atoi(mysql_row[2]);

			m_packetQueue.AddItem(logInSuccessPacket);
		}
		//비밀번호 불일치
		else
		{
			PacketWithSocket* logInFailedPacket =
				reinterpret_cast<PacketWithSocket*>(m_sendBuffer->
					GetBuffer(sizeof(PacketWithSocket)));
			logInFailedPacket->Init(SendCommand::DB2Zone_LOGIN_FAILED_WRONG_PASSWORD, sizeof(PacketWithSocket));
			logInFailedPacket->socket = _packet->socket;

			m_packetQueue.AddItem(logInFailedPacket);
		}
	}
}

void DBConnector::Register(RegisterPacket_DBAgent* _packet)
{
	MYSQL_ROW mysql_row;

	char str1[256];
	sprintf(str1, "select *from accounttable where ID = '%s'", _packet->id);

	m_result = this->Query(str1);

	//해당 id가 존재하지 않음 - 회원가입 가능
	if ((mysql_row = mysql_fetch_row(m_result)) == NULL)
	{
		m_result = this->Query("select *from accounttable");

		if (m_result)
		{
			rowNum = mysql_num_rows(m_result);
			fieldNum = mysql_num_fields(m_result);
			row = mysql_fetch_row(m_result);
			field = mysql_fetch_fields(m_result); //pfield[iFieldNum].name;
		}
		else
		{
			if (mysql_field_count(&m_connect) == 0)
			{
				rowNum = mysql_affected_rows(&m_connect);
			}
		}

		char str2[256];
		sprintf(str2, "INSERT INTO `accounttable` (`ID`, `PASSWORD`, `idNum`) VALUES('%s', '%s', '%d')",
			_packet->id, _packet->password, (int)(rowNum + 1));

		this->Query(str2);

		char str3[1024];
		sprintf(str3, "INSERT INTO `infotable` (`userID`, `userName`, `level`, `curHp`, `maxHp`, `curMp`, `maxMp`, `curExp`, `maxExp`, `atk`, `def`) \
			VALUES ('%d', '%s', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d')",
			(int)(rowNum + 1), _packet->id, 1, 100, 100, 100, 100, 0, 100, 20, 0);

		this->Query(str3);

		PacketWithSocket* RegisterSuccessPacket =
			reinterpret_cast<PacketWithSocket*>(m_sendBuffer->
				GetBuffer(sizeof(PacketWithSocket)));
		RegisterSuccessPacket->Init(SendCommand::DB2Zone_REGISTER_SUCCESS, sizeof(PacketWithSocket));
		RegisterSuccessPacket->socket = _packet->socket;

		m_packetQueue.AddItem(RegisterSuccessPacket);
	}
	//해당 id가 존재함 - 회원가입 불가능
	else
	{
		PacketWithSocket* RegisterFailedPacket =
			reinterpret_cast<PacketWithSocket*>(m_sendBuffer->
				GetBuffer(sizeof(PacketWithSocket)));
		RegisterFailedPacket->Init(SendCommand::DB2Zone_REGISTER_FAILED, sizeof(PacketWithSocket));
		RegisterFailedPacket->socket = _packet->socket;

		m_packetQueue.AddItem(RegisterFailedPacket);
	}
}

void DBConnector::GetUserInfo(RequireUserInfoPacket_DBAgent* _packet)
{
	MYSQL_ROW mysql_row;

	char str1[256];
	sprintf(str1, "select *from infotable where userID = %d", _packet->userIndex);

	m_result = this->Query(str1);

	//중복이 아님
	if ((mysql_row = mysql_fetch_row(m_result)) == NULL)
	{
		//실패
		PacketWithSocket* GetSessionInfoFailedPacket =
			reinterpret_cast<PacketWithSocket*>(m_sendBuffer->
				GetBuffer(sizeof(PacketWithSocket)));
		GetSessionInfoFailedPacket->Init(SendCommand::DB2Zone_GET_USER_DATA_FAILED, sizeof(PacketWithSocket));
		GetSessionInfoFailedPacket->socket = _packet->socket;

		m_packetQueue.AddItem(GetSessionInfoFailedPacket);
	}
	else
	{
		//성공
		GetSessionInfoPacket* sessionInfoPacket =
			reinterpret_cast<GetSessionInfoPacket*>(m_sendBuffer->
				GetBuffer(sizeof(GetSessionInfoPacket)));
		sessionInfoPacket->Init(SendCommand::DB2Zone_GET_USER_DATA_SUCCESS, sizeof(GetSessionInfoPacket));
		sessionInfoPacket->socket = _packet->socket;

		sprintf(sessionInfoPacket->info.userInfo.userName, "%s", mysql_row[1]);
		sessionInfoPacket->info.unitInfo.level = atoi(mysql_row[2]);
		sessionInfoPacket->info.unitInfo.hp.currentValue = atoi(mysql_row[3]);
		sessionInfoPacket->info.unitInfo.hp.maxValue = atoi(mysql_row[4]);
		sessionInfoPacket->info.unitInfo.mp.currentValue = atoi(mysql_row[5]);
		sessionInfoPacket->info.unitInfo.mp.maxValue = atoi(mysql_row[6]);
		sessionInfoPacket->info.unitInfo.exp.currentValue = atoi(mysql_row[7]);
		sessionInfoPacket->info.unitInfo.exp.maxValue = atoi(mysql_row[8]);
		sessionInfoPacket->info.unitInfo.atk = atoi(mysql_row[9]);
		sessionInfoPacket->info.unitInfo.def = atoi(mysql_row[10]);

		m_packetQueue.AddItem(sessionInfoPacket);
	}
}

void DBConnector::UpdateUser(UpdateUserPacket* _packet)
{
	MYSQL_ROW mysql_row;

	char str1[256];
	sprintf(str1, "select *from infotable where userID = %d", _packet->userIndex);

	m_result = this->Query(str1);

	//중복이 아님
	if ((mysql_row = mysql_fetch_row(m_result)) == NULL)
	{
		//실패
		PacketWithSocket* updateUserFailedPacket =
			reinterpret_cast<PacketWithSocket*>(m_sendBuffer->
				GetBuffer(sizeof(PacketWithSocket)));
		updateUserFailedPacket->Init(SendCommand::DB2Zone_UPDATE_USER_FAILED, sizeof(PacketWithSocket));
		updateUserFailedPacket->socket = _packet->socket;

		m_packetQueue.AddItem(updateUserFailedPacket);
	}
	else
	{
		//성공
		char str2[1024];
		sprintf(str2, "UPDATE infotable SET level = '%d', curHp = '%d', maxHp = '%d', curMp = '%d', maxMp = '%d', \
			curExp = '%d', maxExp = '%d', atk = '%d', def = '%d' WHERE userID = '%d'",
			_packet->unitInfo.level, 
			_packet->unitInfo.hp.currentValue, _packet->unitInfo.hp.maxValue,
			_packet->unitInfo.mp.currentValue, _packet->unitInfo.mp.maxValue,
			_packet->unitInfo.exp.currentValue, _packet->unitInfo.exp.maxValue, 
			_packet->unitInfo.atk, _packet->unitInfo.def, _packet->userIndex);

		m_result = this->Query(str2);

		PacketWithSocket* updateUserSuccessPacket =
			reinterpret_cast<PacketWithSocket*>(m_sendBuffer->
				GetBuffer(sizeof(PacketWithSocket)));
		updateUserSuccessPacket->Init(SendCommand::DB2Zone_UPDATE_USER_SUCCESS, sizeof(PacketWithSocket));
		updateUserSuccessPacket->socket = _packet->socket;

		m_packetQueue.AddItem(updateUserSuccessPacket);
	}
}

void DBConnector::GetMonsterInfo()
{
	MYSQL_ROW mysql_row;

	std::vector<MonsterData> monsterDataVec;

	for (int i = 0; i < 1000; i++)
	{
		char str1[256];
		sprintf(str1, "select *from monstertable where monsterType = %d", i + 1);

		m_result = this->Query(str1);

		if ((mysql_row = mysql_fetch_row(m_result)) == NULL)
		{
			break;
		}
		else
		{
			MonsterData tempData;

			tempData.monsterType = i + 10001;
			tempData.hp.currentValue = atoi(mysql_row[1]);
			tempData.hp.maxValue = atoi(mysql_row[1]);
			tempData.attackDelay = (float)atof(mysql_row[2]);
			tempData.attackDamage = atoi(mysql_row[3]);
			tempData.attackDistance = (float)atof(mysql_row[4]);
			tempData.moveSpeed = (float)atof(mysql_row[5]);
			tempData.patrolRange = atoi(mysql_row[6]);
			tempData.patrolDelay = (float)atof(mysql_row[7]);
			tempData.returnDistance = (float)atof(mysql_row[8]);
			tempData.dropExp = atoi(mysql_row[9]);

			monsterDataVec.push_back(tempData);
		}
	}

	if (monsterDataVec.size() <= 0) return;

	MonstersInfoPacket* monstersInfoPacket =
		reinterpret_cast<MonstersInfoPacket*>(m_sendBuffer->
			GetBuffer(sizeof(GetSessionInfoPacket)));

	monstersInfoPacket->count = monsterDataVec.size();

	for (int i = 0; i < monsterDataVec.size(); i++)
	{
		monstersInfoPacket->monstersData[i] = monsterDataVec[i];
	}

	monstersInfoPacket->size = (sizeof(MonsterData) * monsterDataVec.size())
		+ sizeof(WORD) + sizeof(Packet);
	monstersInfoPacket->Init(SendCommand::DB2Zone_MONSTERS_DATA, monstersInfoPacket->size);

	m_packetQueue.AddItem(monstersInfoPacket);
}

bool DBConnector::Test()
{
	MYSQL_ROW mysql_row;

	char str1[256];
	sprintf(str1, "select *from infotable where userID = %d", m_num);

	m_result = this->Query(str1);

	//중복이 아님
	if ((mysql_row = mysql_fetch_row(m_result)) == NULL)
	{
		return false;
	}
	else
	{
		printf("%d connector - %s \n", m_num, mysql_row[1]);

		return true;
	}
}

void DBConnector::Thread()
{
	while (1)
	{
		WaitForSingleObject(hEvent, INFINITE);

		m_state = ACTIVE;

		switch (static_cast<RecvCommand>(m_packet->cmd))
		{
		case RecvCommand::Zone2DB_LOGIN:
		{
			LogInPacket_DBAgent* logInPacket = static_cast<LogInPacket_DBAgent*>(m_packet);
			Login(logInPacket);
			printf("LogIn \n");
		}			
			break;
		case RecvCommand::Zone2DB_REGISTER:
		{
			RegisterPacket_DBAgent* RegisterPacket = static_cast<RegisterPacket_DBAgent*>(m_packet);
			Register(RegisterPacket);
			printf("Register \n");
		}
			break;
		case RecvCommand::Zone2DB_REQUEST_USER_DATA:
		{
			RequireUserInfoPacket_DBAgent* packet = static_cast<RequireUserInfoPacket_DBAgent*>(m_packet);
			GetUserInfo(packet);
			printf("GetUserInfo \n");
		}		
			break;
		case RecvCommand::Zone2DB_REQUEST_MONSTER_DATA:
		{
			GetMonsterInfo();
			printf("GetMonsterInfo \n");
		}
			break;
		case RecvCommand::Zone2DB_UPDATE_USER:
		{
			UpdateUserPacket* packet = static_cast<UpdateUserPacket*>(m_packet);
			UpdateUser(packet);

			printf("Update User \n");
		}
			break;
		}

		m_packet = nullptr;
		m_state = READY;

		//ResetEvent(EventClass<DBConnector>::GetEventHandle());
	}
}