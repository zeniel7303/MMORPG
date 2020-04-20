#pragma once
#include "stdafx.h"

#include "Unit.h"
#include "Session.h"

#include "packet.h"

#include "ZoneTilesData.h"
#include "Sector.h"

//=====================================================

//User

//Session과 Unit을 상속받아 쓰는 클래스

//=====================================================

struct INFO
{
	UserInfo	userInfo;
	UnitInfo	unitInfo;
};

class Zone;

class User : public Session, public Unit
{
private:
	INFO				m_basicInfo;

	Sector*				m_sector;

	Tile*				m_tile;

	//===============================================

	Zone*				m_zone;
	ZoneTilesData*		m_zoneTilesData;

	bool				m_startCheckingHeartBeat;
	bool				m_isTestClient;

public:
	User();
	~User();

	virtual void Init();
	virtual void OnConnect(SOCKET _socket);
	virtual void Disconnect();
	virtual void Reset();
	//살아있는지 체크
	virtual void HeartBeatChecked();
	//유저의 정보를 DB에 올리는 함수
	void UpdateInfo();

	//죽음
	void Death();
	//부활 및 첫스폰
	void Respawn(VECTOR2 _spawnPosition);
	//맞음
	void Hit(int _index, int _damage);

	//경험치 획득
	void PlusExp(int _exp);
	//레벨업
	void LevelUp();

	//유저의 정보 보내기
	void SendInfo();
	//테스트용
	void TestClientEnterZone(Zone* _zone, int _zoneNum);
	//Zone 입장 시
	void EnterZone(Zone* _zone, int _zoneNum, VECTOR2 _spawnPosition);
	//User의 포지션 
	void SetPosition(Position& _position);

	bool LogInUser(LogInPacket* _packet);
	void RegisterUser(RegisterUserPacket* _packet);

	//로그인 중복됨(이미 접속중임)->실패처리
	void LogInDuplicated();
	void LogInSuccess();

	Tile* GetTile() { return m_tile; }
	INFO* GetInfo() { return &m_basicInfo; }
	Zone* GetZone() { return m_zone; }
	void SetZone(Zone* _zone) { m_zone = _zone; }

	bool GetIsDeath() { return m_basicInfo.unitInfo.state == STATE::DEATH; }
	
	void StartCheckingHeartBeat() { m_startCheckingHeartBeat = true; }
	bool GetStartCheckingHeartBeat() { return m_startCheckingHeartBeat; }

	bool GetIsTestClient() { return m_isTestClient; }
};												 

