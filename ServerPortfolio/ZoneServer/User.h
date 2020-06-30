	#pragma once
#include "stdafx.h"
#include"../ServerLibrary/HeaderFiles/OnlyHeaders/DoubleQueue.h"

#include "UnitInfo.h"
#include "UserSession.h"
#include "PathFinding.h"
#include "FieldTilesData.h"

#include "packet.h"

//=====================================================

//User

//Session�� Unit�� ��ӹ޾� ���� Ŭ����

//=====================================================

struct INFO
{
	UserInfo	userInfo;
	UnitInfo	unitInfo;
};

class Field;
class Sector;

class User : public UserSession
{
private:
	INFO				m_basicInfo;

	Tile*				m_tile;

	Sector*				m_sector;

	Field*				m_field;
	FieldTilesData*		m_fieldTilesData;

	//===============================================
	bool				m_isGetUserList;

	bool				m_isCheckingHeartBeat;
	bool				m_startCheckingHeartBeat;
	int					m_heartBeatCheckedCount;

public:
	User();
	~User();

	void OnConnect();
	void DisConnect();
	void Reset();

	void HandleOverlappedIO(ST_OVERLAPPED* _overlapped);
	void Parsing();

	//����ִ��� üũ
	void HeartBeatChecked();

	//������ ������ DB�� �ø��� �Լ�
	void UpdateInfo();

	//����
	void Death();
	//��Ȱ �� ù����
	void Respawn(VECTOR2 _spawnPosition);
	//����
	void Hit(int _index, int _damage);

	//����ġ ȹ��
	void PlusExp(int _exp);
	//������
	void LevelUp();

	//������ ���� �ޱ��û(DB������)
	void RequestUserInfo();
	//������ ���� �ޱ� ���� -> ���� ���� ����(Ŭ�󿡰�)
	void SendInfo(GetSessionInfoPacket* _packet);
	//������ ���� �ޱ� ����
	void RequestUserInfoFailed();
	//Field ���� ��
	void EnterField(Field *_field, int _fieldNum, VECTOR2 _spawnPosition);
	//User�� ������(�̵��� ȣ���)
	void SetPosition(Position& _position);

	void LogInUser(LogInPacket* _packet);
	void RegisterUser(RegisterUserPacket* _packet);

	//�α��� �ߺ���(�̹� ��������)->����ó��
	void LogInDuplicated();
	void LogInSuccess(int _num);
	void LogInFailed();

	void RegisterSuccess();
	void RegisterFailed();

	bool CompareSector(Sector* _sector);

	//�׽�Ʈ��
	void TestClientEnterField(Field* _Field, int _fieldNum, int _dummyNum, VECTOR2 _spawnPosition);

	Tile* GetTile() { return m_tile; }
	INFO* GetInfo() { return &m_basicInfo; }

	Field* GetField() { return m_field; }
	void SetField (Field* _field) { m_field = _field; }

	Sector* GetSector() { return m_sector; }
	void SetSector(Sector* _sector) { m_sector = _sector; }

	bool IsDeath() { return m_basicInfo.unitInfo.state == STATE::DEATH; }

	void SetState(STATE _state) { m_basicInfo.unitInfo.state = _state; }

	bool GetIsGetUserList() { return m_isGetUserList; }
	void SetIsGetUserList(bool _bool) { m_isGetUserList = _bool; }

	bool IsChecking() { return m_isCheckingHeartBeat; }
	void SetChecking(bool _bool) { m_isCheckingHeartBeat = _bool; }

	bool GetStartCheckingHeartBeat() { return m_startCheckingHeartBeat; }
	void StartCheckingHeartBeat() { m_startCheckingHeartBeat = true; }
};												 
