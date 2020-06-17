#pragma once
#include <iostream>
#include <string>
#include <mysql.h>

#pragma comment(lib, "libmySQL.lib")

#pragma warning(disable:4996)

using namespace std;

//=====================================================

//MySQLClass

//MySQL에 접근하는 클래스
//Connect & SelectDB로 rpg DB에 접근한다.
//이후 다른 함수들로 로그인, 회원가입, 유저DB업데이트, 몬스터DB불러오기를 실행한다.

//=====================================================

class dbClass
{
private:
	//CriticalSectionClass	m_lock;

	const char*				m_dataName;

	MYSQL					connect;
	MYSQL_RES*				result;

	MYSQL_ROW				row;
	MYSQL_FIELD*			field;

	//레코드셋의 개수
	my_ulonglong			rowNum;
	//필드의 개수
	unsigned int			fieldNum;

	//결과가 있는 쿼리인가?
	bool					haveResultQuery;
	//연결이 되어 있는가?
	bool					isConnect;

	//미사용
	/*
	unsigned int			fieldNum;			//필드의 개수
	my_ulonglong			currentPos;			//현재 row 의 offset

	bool					haveResultQuery;	//결과가 있는 쿼리인가요?
	bool					isBof;				//레코드셋의 처음인가요?		(begin of file)
	bool					isEof;				//레코드셋의 마지막인가요?	(end of file)
	*/

public:
	dbClass();
	~dbClass();

	void Init();
	bool Connect(const char* host,
		const char* username,
		const char* password,
		const char* database,
		const unsigned int port,
		const char* unix_socket,
		const unsigned int client_flag);
	void DisConnect();

	bool SelectDB(const char* DBname);

	//아이디를 먼저 비교해 해당 id가 있는지 체크 후 비밀번호가 맞다면 해당 행을 불러온다.
	bool OverlappedCheck(const char* id, const char* password, int &_userId);
	//아이디를 먼저 비교해 해당 id가 있는지 체크 후 존재하지 않는다면 받아온 비밀번호와 합쳐 회원가입을 해준다.
	bool InsertData(const char* id, const char* password);

	//해당 유저의 번호로 유저의 정보를 찾아서 불러온다.
	//_userId는 절대 변경되선 안된다. 그래서 const int&로 인자를 불러옴.
	bool GetUserInfo(const int& _userId,
		char* _userName, int& _level, int& _curHp, int& _maxHp,
		int& _curMp, int& _maxMp, int& _curExp, int& _maxExp,
		int& _atk, int& _def);
	//해당 유저의 번호로 유저의 정보를 찾아 그 행에 지금 현재 유저의 정보를 DB에 업데이트한다.
	bool UpdateUserInfo(const int& _userId,
		int _level, int _curHp, int _maxHp,
		int _curMp, int _maxMp, int _curExp, int _maxExp,
		int _atk, int _def);

	//몬스터 타입에 따른 몬스터의 정보를 불러온다.
	bool GetMonsterInfo(WORD& _monsterType, int& _curHp, int& _maxHp,
		float& _attackDelay, int& _attackDamage, float& _attackDistance,
		float& _moveSpeed, int& _patrolRange, float& _patrolDelay,
		float& returnDistance, int& _dropExp);

	void Query(const char* tableName);

	//미사용
	/*
	void First();
	void Last();
	void Next();
	void Prev();
	void Move(int n);
	*/
};

