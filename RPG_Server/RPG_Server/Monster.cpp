#include "Monster.h"
#include "User.h"
#include "Zone.h"
#include "SectorManager.h"
#include "Sector.h"

Monster::~Monster()
{
}

void Monster::Init(MonsterInfo& _info, MonsterData& _data)
{
	m_info = _info;
	m_data = _data;
	m_info.hp = m_data.hp;

	m_spawnPosition.x = m_info.position.x;
	m_spawnPosition.y = m_info.position.y;

	m_sendBuffer = new SendBuffer();
	m_sendBuffer->Init(1000);

	m_currentTime = m_maxTime = 0.0f;

	Spawn();
}

void Monster::Reset()
{
	m_sendBuffer->Reset();
}

void Monster::Update()
{
	m_currentTime += 1.0f / 10.0f;

	FSM();

	if (m_isAttack)
	{
		if (m_info.state != STATE::ATTACK)
		{
			m_info.state = STATE::ATTACK;
			m_isAttack = false;
		}
	}

	if (m_isDeath)
	{
		if (m_info.state != STATE::DEATH)
		{
			m_info.state = STATE::DEATH;
			m_currentTime = 0.0f;
		}
	}

	//printf("%d : position : %f, %f \n", m_info.index, m_info.position.x, m_info.position.y);
}

void Monster::Spawn()
{
	m_target = nullptr;

	m_info.hp.currentValue = m_info.hp.maxValue;

	m_info.position = m_spawnPosition;
	m_homeTile = m_zoneTilesData->GetTile(
		static_cast<int>(m_info.position.x),
		static_cast<int>(m_info.position.y));
	m_nowTile = m_homeTile;

	UpdateSector();

	m_info.state = STATE::SPAWN;

	m_isDeath = false;
	m_isAttack = false;

	MonsterInfoPacket* monsterInfoPacket = 
		reinterpret_cast<MonsterInfoPacket*>(m_sendBuffer->
			GetBuffer(sizeof(MonsterInfoPacket)));
	monsterInfoPacket->info = m_info;
	monsterInfoPacket->Init(SendCommand::MONSTER_INFO, sizeof(MonsterInfoPacket));

	m_sendBuffer->Write(monsterInfoPacket->size);
	packetQueue.AddItem(PacketQueuePair(this, monsterInfoPacket));
}

void Monster::Move()
{
	VECTOR2 tempVec2 = VECTOR2(m_targetPosition - m_info.position);
	float magnitude = tempVec2.Magnitude();

	if (magnitude <= m_data.moveSpeed * (1.0f / 30.0f) || magnitude == 0.0f)
		m_info.position = m_targetPosition;

	//두 직선, 벡터의 관계(사이각,회전각) 구하기
	//https://darkpgmr.tistory.com/121
	//VECTOR2(m_targetPosition - m_info.position) / magnitude 이 부분

	//https://www.youtube.com/watch?v=WxWJorOVIj8
	//https://www.youtube.com/watch?v=-gokuNjpyNg

	m_info.position = m_info.position + tempVec2 / magnitude * m_data.moveSpeed * (1.0f / 30.0f);
	//각도 * 이동속도 * 시간
}

void Monster::Attack()
{
	float magnitude = VECTOR2(m_info.position - m_target->GetInfo()->unitInfo.position).Magnitude();

	//머니까 따라감
	if (magnitude > m_data.attackDistance * m_data.attackDistance)
	{
		m_targetTile = m_target->GetTile();

		if (m_targetTile == m_nowTile) return;

		PathFindStart(m_targetTile);
		m_info.state = STATE::MOVE;

		return;
	}
	if (m_target->GetInfo()->unitInfo.zoneNum != m_zone->GetZoneNum() 
		|| m_target->GetIsDeath())
	{
		m_targetTile = m_homeTile;
		PathFindStart(m_targetTile);
		m_info.state = STATE::RETURN;

		return;
	}

	if (m_currentTime >= m_data.attackDelay)
	{
		MonsterAttackPacket* monsterAttackPacket =
			reinterpret_cast<MonsterAttackPacket*>(m_sendBuffer->
				GetBuffer(sizeof(MonsterAttackPacket)));

		monsterAttackPacket->monsterIndex = m_info.index;
		monsterAttackPacket->damage = m_data.attackDamage;
		monsterAttackPacket->Init(SendCommand::MONSTER_ATTACK, sizeof(MonsterAttackPacket));

		m_sendBuffer->Write(monsterAttackPacket->size);
		packetQueue.AddItem(PacketQueuePair(this, monsterAttackPacket));

		m_currentTime = 0.0f;
	}
}

//여러 유저가 동시에 공격할 경우 동기화 문제 발생
//처리해야함.
Packet* Monster::Hit(User* _user, int _damage)
{
	m_locker.EnterLock();

	if (m_info.state == STATE::DEATH) return nullptr;

	//대부분 알피지는 이럴때 공격이 안됐음
	if (m_info.state == STATE::SPAWN || m_info.state == STATE::RETURN)
	{
		MonsterHitPacket* monsterHitPacket = 
			reinterpret_cast<MonsterHitPacket*>(m_sendBuffer->
			GetBuffer(sizeof(MonsterHitPacket)));
		monsterHitPacket->monsterIndex = m_info.index;
		monsterHitPacket->userIndex = _user->GetInfo()->userInfo.userID;
		monsterHitPacket->damage = 0;
		monsterHitPacket->Init(SendCommand::MONSTER_HIT_FAIL, sizeof(MonsterHitPacket));

		m_sendBuffer->Write(monsterHitPacket->size);
		return monsterHitPacket;

		m_locker.LeaveLock();
	}

	if (m_target == nullptr)
	{
		m_isAttack = true;
		m_target = _user;
	}

	m_info.hp.currentValue -= _damage;

	MonsterHitPacket* monsterHitPacket =
		reinterpret_cast<MonsterHitPacket*>(m_sendBuffer->
			GetBuffer(sizeof(MonsterHitPacket)));

	monsterHitPacket->monsterIndex = m_info.index;
	monsterHitPacket->userIndex = _user->GetInfo()->userInfo.userID;
	monsterHitPacket->damage = _damage;
	monsterHitPacket->Init(SendCommand::MONSTER_HIT, sizeof(MonsterHitPacket));

	m_sendBuffer->Write(monsterHitPacket->size);

	if (m_info.hp.currentValue <= 0)
	{
		m_isDeath = true;
	}

	m_locker.LeaveLock();

	return monsterHitPacket;
}

void Monster::FSM()
{
	switch (m_info.state)
	{
		//기본 상태
		case STATE::IDLE:
		{
			if (m_currentTime < m_data.patrolDelay) break;

			m_targetTile = m_nowTile;

			while (m_targetTile == m_nowTile)
			{
				int randX = (rand() % m_data.patrolRange * 2) - m_data.patrolRange;
				int randY = (rand() % m_data.patrolRange * 2) - m_data.patrolRange;

				m_targetTile = m_zoneTilesData->
					GetTile(m_homeTile->GetX() + randX, m_homeTile->GetY() + randY);
			}

			PathFindStart(m_targetTile);
			m_info.state = STATE::PATROL;
			break;
		}
		//정찰
		case STATE::PATROL:
		{
			Move();
			//이동중이라면 계속 PATROL 고정시켜줌
			if (PathMove()) break;
			//이동이 끝났으니 IDLE로
			m_info.state = STATE::IDLE;
			m_currentTime = 0.0f;
			m_targetTile = nullptr;
			break;
		}
		//이동(공격시 멀 경우에 사용됨)
		case STATE::MOVE:
		{
			Move();

			float magnitude_1 = VECTOR2(m_info.position - m_target->GetInfo()->unitInfo.position).Magnitude();

			//사정거리 내면 공격
			if (magnitude_1 <= m_data.attackDistance * m_data.attackDistance)
			{
				m_info.state = STATE::ATTACK;
				break;
			}

			float magnitude_2 = VECTOR2(m_info.position - m_spawnPosition).Magnitude();

			//귀환거리보다 멀어질경우 귀환
			if (magnitude_2 > m_data.returnDistance * m_data.returnDistance)
			{
				m_targetTile = m_homeTile;
				PathFindStart(m_targetTile);
				m_info.state = STATE::RETURN;

				break;
			}

			//타겟 유저가 움직여서 타겟타일이 바뀌었을 경우 추적
			if (m_targetTile != m_target->GetTile())
			{
				m_targetTile = m_target->GetTile();
				PathFindStart(m_targetTile);

				break;
			}

			//이동중이 아니다 == 도착했다. -> 공격
			if (!PathMove())
			{
				m_info.state = STATE::ATTACK;
			}

			break;
		}
		//원래 타일로 돌아가기
		case STATE::RETURN:
		{
			Move();

			//이동 끝남
			if (!PathMove())
			{
				//체력 다시 원상태로
				Spawn();

				m_currentTime = 0.0f;
			}
			break;
		}
		//공격
		case STATE::ATTACK:
		{
			Attack();
			break;
		}
		//사망 - 5초 후 Spawn
		case STATE::DEATH:
		{
			if (m_currentTime >= 5.0f)
			{
				Spawn();

				m_currentTime = 0.0f;
			}

			break;
		}
		//생성 - 첫 생성 & 부활 둘 다 이용됨
		case STATE::SPAWN:
		{
			if (m_currentTime >= 1.0f)
			{
				m_currentTime = 0.0f;
				m_info.state = STATE::IDLE;
			}
			break;
		}
	}
}

void Monster::PathFindStart(Tile* _targetTile)
{
	PathFinding::PathFind(m_tileList, m_nowTile, _targetTile);

	if (m_tileList.size() == 0) return;

	m_targetPosition = *m_tileList.begin();
	m_tileList.erase(m_tileList.begin());
	m_currentTime = 0;
}

bool Monster::PathMove()
{
	float distance = VECTOR2(m_info.position - m_targetPosition).SqrMagnitude();

	//목표 좌표에 도착
	if (distance <= 0.1f)
	{
		m_info.position = m_targetPosition;
		m_nowTile = m_zoneTilesData->GetTile(
			static_cast<int>(m_info.position.x),
			static_cast<int>(m_info.position.y));

		//남은 타일리스트가 있다면
		if (m_tileList.size() > 0)
		{
			m_targetPosition = *m_tileList.begin();
			m_tileList.erase(m_tileList.begin());

			//현재 섹터 범위 체크
			if (!m_sector->SectorRangeCheck(m_nowTile))
			{
				UpdateSector();
			}

			MonsterPositionPacket* monsterPositionPacket =
				reinterpret_cast<MonsterPositionPacket*>(m_sendBuffer->
					GetBuffer(sizeof(MonsterPositionPacket)));

			monsterPositionPacket->monsterIndex = m_info.index;
			monsterPositionPacket->position = m_info.position;
			monsterPositionPacket->Init(SendCommand::MONSTER_MOVE, sizeof(MonsterPositionPacket));
			
			m_sendBuffer->Write(monsterPositionPacket->size);
			packetQueue.AddItem(PacketQueuePair(this, monsterPositionPacket));
		}
		//남은 타일리스트가 없다면
		else
		{
			return false;
		}
	}
	return true;
}

void Monster::UpdateSector()
{
	Sector* prevSector = m_sector;
	if (prevSector != nullptr)
	{
		//printf("[ Exit User (Prev Sector) ]");
		prevSector->Manager<Monster>::DeleteItem(this);
	}

	m_sector = m_sectorManager->
		GetSector(m_info.position.x, m_info.position.y);

	m_sector->Manager<Monster>::AddItem(this);
}