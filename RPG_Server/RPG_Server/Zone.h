#pragma once
#include <time.h>
#include <algorithm>

#include "ManagerFrame.h"
#include "SendBuffer.h"

#include "User.h"
#include "ZoneTilesData.h"
#include "MonsterLogicThread.h"

class Zone : public Manger<User>
{
private:
	WORD m_zoneNum;
	VECTOR2 m_spawnPosition;

	//==================================================

	ZoneTilesData m_zoneTilesData;

	MonsterLogicThread m_monsterLogicThread;

	CriticalSectionClass m_locker;

	//==================================================

	SendBuffer* m_sendBuffer;

	time_t		start, end;

public:
	Zone();
	~Zone();

	void Init(int _num, VECTOR2 _spawnPosition);
	void GetMap(const char* _name);

	void SendAll(char * _buffer, int _size);

	void SendUserList(User* _user);

	void EnterTestClient(User* _user);
	void EnterUser(User* _user);
	void SendEnterUserInfo(User* _user);

	void ExitUser(User* _user);
	void SendExitUserInfo(int _num);

	bool UserAttack(User * _user, int _monsterIndex);

	void RespawnUser(User* _user);

	WORD GetZoneNum() { return m_zoneNum; }

	VECTOR2& GetSpawnPosition() { return m_spawnPosition; }

	ZoneTilesData* GetTilesData() { return &m_zoneTilesData; }
};