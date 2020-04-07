#include "MySQLClass.h"

MySQLClass::MySQLClass()
{
}

MySQLClass::~MySQLClass()
{
}

void MySQLClass::Init()
{
	isConnect = false;

	mysql_init(&connect);
}

bool MySQLClass::Connect(const char *host,
	const char *username,
	const char *password,
	const char *database,
	const unsigned int port,
	const char *unix_socket,
	const unsigned int client_flag)
{
	if (&connect == NULL)
	{
		printf("%d Error : %s \n", mysql_errno(&connect), mysql_error(&connect));
		exit(1);
	}

	if (mysql_real_connect(&connect, host, username, password, database, port, unix_socket, client_flag) == NULL)
	{
		isConnect = false;

		printf("[ DB Connect Failed ]\n");

		return false;
	}

	isConnect = true;

	m_dataName = database;

	return true;
}

void MySQLClass::DisConnect()
{
	if (&connect != NULL)
	{
		mysql_close(&connect);

		isConnect = false;
	}
}

bool MySQLClass::SelectDB(const char* DBname)
{
	return mysql_select_db(&connect, DBname);
}

bool MySQLClass::OverlappedCheck(const char* id, const char* password, int &_userId)
{
	m_lock.EnterLock();

	MYSQL_ROW mysql_row;

	char str1[256];
	sprintf(str1, "select *from accounttable where ID = '%s'", id);

	if (mysql_query(&connect, str1))
	{
		printf("%d Error : %s \n", mysql_errno(&connect), mysql_error(&connect));
		//exit(1);
	}

	result = mysql_store_result(&connect);

	//해당 id 존재하지 않음
	if ((mysql_row = mysql_fetch_row(result)) == NULL)
	{
		return false;
	}
	//해당 id 존재함
	else
	{
		int result = strcmp(mysql_row[1], password);

		//비밀번호 일치
		if (result == 0)
		{
			_userId = atoi(mysql_row[2]);

			m_lock.LeaveLock();

			return true;
		}
		//비밀번호 불일치
		else
		{
			m_lock.LeaveLock();

			return false;
		}
	}
}

bool MySQLClass::InsertData(const char* id, const char* password)
{
	m_lock.EnterLock();

	MYSQL_ROW mysql_row;

	char str1[256];
	sprintf(str1, "select *from accounttable where ID = '%s'", id);

	if (mysql_query(&connect, str1))
	{
		printf("%d Error : %s \n", mysql_errno(&connect), mysql_error(&connect));
		//exit(1);
	}

	result = mysql_store_result(&connect);

	//해당 id가 존재하지 않음 - 회원가입 가능
	if ((mysql_row = mysql_fetch_row(result)) == NULL)
	{
		this->Query("select *from accounttable");

		char str2[256];
		sprintf(str2, "INSERT INTO `accounttable` (`ID`, `PASSWORD`, `idNum`) VALUES('%s', '%s', '%d')", 
			id, password, (int)(rowNum + 1));

		mysql_query(&connect, str2);

		char str3[1024];
		sprintf(str3, "INSERT INTO `infotable` (`userID`, `userName`, `level`, `curHp`, `maxHp`, `curMp`, `maxMp`, `curExp`, `maxExp`, `atk`, `def`) \
			VALUES ('%d', '%s', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d')",
			(int)(rowNum + 1), id, 1, 100, 100, 100, 100, 0, 100, 20, 0);

		mysql_query(&connect, str3);

		m_lock.LeaveLock();

		return true;
	}
	//해당 id가 존재함 - 회원가입 불가능
	else
	{
		m_lock.LeaveLock();

		return false;
	}
}

bool MySQLClass::GetUserInfo(const int& _userId,
	char* _userName, int& _level,
	int& _curHp, int& _maxHp, int& _curMp, int& _maxMp, int& _curExp, int& _maxExp,
	int& _atk, int& _def)
{
	m_lock.EnterLock();

	MYSQL_ROW mysql_row;

	char str1[256];
	sprintf(str1, "select *from infotable where userID = %d", _userId);

	if (mysql_query(&connect, str1))
	{
		printf("%d Error : %s \n", mysql_errno(&connect), mysql_error(&connect));
		//exit(1);
	}

	result = mysql_store_result(&connect);

	//중복이 아님
	if ((mysql_row = mysql_fetch_row(result)) == NULL)
	{
		m_lock.LeaveLock();

		return false;
	}
	else
	{
		sprintf(_userName, "%s", mysql_row[1]);
		_level = atoi(mysql_row[2]);
		_curHp = atoi(mysql_row[3]);
		_maxHp = atoi(mysql_row[4]);
		_curMp = atoi(mysql_row[5]);
		_maxMp = atoi(mysql_row[6]);
		_curExp = atoi(mysql_row[7]);
		_maxExp = atoi(mysql_row[8]);
		_atk = atoi(mysql_row[9]);
		_def = atoi(mysql_row[10]);

		m_lock.LeaveLock();

		return true;
	}
}

bool MySQLClass::UpdateUserInfo(const int& _userId, int _level,
	int _curHp, int _maxHp, int _curMp, int _maxMp, int _curExp, int _maxExp,
	int _atk, int _def)
{
	m_lock.EnterLock();

	MYSQL_ROW mysql_row;

	char str1[256];
	sprintf(str1, "select *from infotable where userID = %d", _userId);

	if (mysql_query(&connect, str1))
	{
		printf("%d Error : %s \n", mysql_errno(&connect), mysql_error(&connect));
		//exit(1);
	}

	result = mysql_store_result(&connect);

	//중복이 아님
	if ((mysql_row = mysql_fetch_row(result)) == NULL)
	{
		m_lock.LeaveLock();

		return false;
	}
	else
	{
		char str2[1024];
		sprintf(str2, "UPDATE infotable SET level = '%d', curHp = '%d', maxHp = '%d', curMp = '%d', maxMp = '%d', \
			curExp = '%d', maxExp = '%d', atk = '%d', def = '%d' WHERE userID = '%d'",
			_level, _curHp, _maxHp, _curMp, _maxMp, _curExp, _maxExp, _atk, _def, _userId);

		mysql_query(&connect, str2);

		m_lock.LeaveLock();

		return true;
	}
}

bool MySQLClass::GetMonsterInfo(WORD& _monsterType, int& _curHp, int& _maxHp,
	float& _attackDelay, int& _attackDamage, float& _attackDistance,
	float& _moveSpeed, int& _patrolRange, float& _patrolDelay,
	float& returnDistance, int& _dropExp)
{
	if (_monsterType == 10001)
	{
		MYSQL_ROW mysql_row;

		char str1[256];
		sprintf(str1, "select *from monstertable where monsterType = %d", _monsterType - 10000);

		if (mysql_query(&connect, str1))
		{
			printf("%d Error : %s \n", mysql_errno(&connect), mysql_error(&connect));
			//exit(1);
		}

		result = mysql_store_result(&connect);

		//중복이 아님
		if ((mysql_row = mysql_fetch_row(result)) == NULL)
		{
			return false;
		}
		else
		{
			_curHp = atoi(mysql_row[1]);
			_maxHp = atoi(mysql_row[1]);
			_attackDelay = (float)atof(mysql_row[2]);
			_attackDamage = atoi(mysql_row[3]);
			_attackDistance = (float)atof(mysql_row[4]);
			_moveSpeed = (float)atof(mysql_row[5]);
			_patrolRange = atoi(mysql_row[6]);
			_patrolDelay = (float)atof(mysql_row[7]);
			returnDistance = (float)atof(mysql_row[8]);
			_dropExp = atoi(mysql_row[9]);

			return true;
		}
	}
	return false;
}

void MySQLClass::Query(const char* tableName)
{
	if (!isConnect || &connect == NULL)
	{
		printf("%d Error : %s \n", mysql_errno(&connect), mysql_error(&connect));
		//exit(1);
	}

	if (mysql_query(&connect, tableName))
	{
		printf("%d Error : %s \n", mysql_errno(&connect), mysql_error(&connect));
		//exit(1);
	}

	result = mysql_store_result(&connect);

	if (result)
	{
		rowNum = mysql_num_rows(result);
		fieldNum = mysql_num_fields(result);
		row = mysql_fetch_row(result);
		field = mysql_fetch_fields(result); //pfield[iFieldNum].name;

		haveResultQuery = true;
	}
	else
	{
		if (mysql_field_count(&connect) == 0)
		{
			rowNum = mysql_affected_rows(&connect);
			haveResultQuery = false;
		}
	}
}

/*
void MySQLClass::First()
{
	isBof = true;
	isEof = false;
	currentPos = 0;
	mysql_data_seek(result, currentPos);
	row = mysql_fetch_row(result);
}

void MySQLClass::Last()
{
	isBof = false;
	isEof = true;
	currentPos = rowNum - 1;
	mysql_data_seek(result, currentPos);
	row = mysql_fetch_row(result);
}

void MySQLClass::Next()
{
	if (!isEof)
	{
		Move(1);

		//앞으로 이동했으면 최소한 시작점은 아닐것이다.
		if (isBof)
		{
			isBof = false;
		}
	}
	else
	{
		cout << "[EXCEPTION] Exception Next(), 다음 레코드셋이 없습니다. ";
		//exit(1);
	}
}

void MySQLClass::Prev()
{
	if (!isBof)
	{
		Move(-1);

		//뒤로 이동했으면 최소한 끝점은 아닐것이다.
		if (isEof)
		{
			isEof = false;
		}
	}
	else
	{
		cout << "Exception Prev(), 이전 레코드셋이 없습니다." << endl;
		cout << "[Error:" << mysql_errno(&connect) << "] " << mysql_error(&connect) << endl;
		//exit(1);
	}
}

void MySQLClass::Move(int n)
{
	my_ulonglong pos = currentPos + n;

	if (pos == -1)
	{
		isBof = true;
		isEof = false;
	}
	else if (pos == rowNum)
	{
		isBof = false;
		isEof = true;
	}
	else if (pos < 0 || pos > rowNum)
	{
		cout << "Exception Prev() or Next() or Move(), 레코드셋범위를 넘습니다" << endl;
		cout << "[Error:" << mysql_errno(&connect) << "] " << mysql_error(&connect) << endl;
		//exit(1);
	}

	currentPos = pos;
	mysql_data_seek(result, currentPos);
	row = mysql_fetch_row(result);
}
*/