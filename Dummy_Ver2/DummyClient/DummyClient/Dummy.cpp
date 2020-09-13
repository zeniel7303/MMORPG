#include "Dummy.h"

Dummy::Dummy(int _num1, int _num2, IOCPClass* _iocp)
{
	m_loginSession = new loginSession(_num1 + 1, _iocp);
	m_zoneSession = new zoneSession(_num2, _iocp);

	m_loginSession->SetZoneSession(m_zoneSession);
	m_zoneSession->SetLoginSession(m_loginSession);

	patrolRange = /*(int)((rand() % 5) + 1);*/7;

	currentTime = 0.0f;
	intervalTime = (float)(rand() % 20) + 1;
	heartBeatTime = 0.0f;
}

Dummy::~Dummy()
{
}

void Dummy::FSM()
{
	heartBeatTime += 1.0f / 10.0f;

	if (heartBeatTime >= 2.0f)
	{
		m_loginSession->SendHeartBeat();

		heartBeatTime = 0.0f;
	}

	switch (m_zoneSession->m_basicInfo.unitInfo.state)
	{
	case STATE::IDLE:
	{
		//MYDEBUG("IDLE %.2f\n", currentTime);

		currentTime += 1.0f / 10.0f;

		if (currentTime < intervalTime) return;

		tile = tilesData.GetTile(m_zoneSession->m_basicInfo.unitInfo.position.x,
			m_zoneSession->m_basicInfo.unitInfo.position.y);

		int randX = (rand() % ((patrolRange * 2) + 1)) - patrolRange;
		int randY = (rand() % ((patrolRange * 2) + 1)) - patrolRange;

		m_targetPosition = { m_zoneSession->m_basicInfo.unitInfo.position.x + randX,
			m_zoneSession->m_basicInfo.unitInfo.position.y + randY };

		Tile* tempTile = tilesData.GetTile(m_targetPosition.x, m_targetPosition.y);

		//잘못된 타일이면 다시 or 찾는 타일이 막힌 타일 or 찾는 타일이 나 자신이면 다시
		if (tempTile == nullptr || tempTile->GetType() == TileType::BLOCK
			|| tempTile == tile)
		{
			currentTime = 0.0f;

			return;
		}

		pathFinding.PathFind(&tileList, tile, tempTile);

		if (tileList.size() > 0)
		{
			m_targetPosition = tileList.front();

			tileList.pop_front();

			m_zoneSession->m_basicInfo.unitInfo.state = STATE::MOVE;
		}

		currentTime = 0.0f;

		intervalTime = (float)(rand() % 20) + 1;
	}
	break;
	case STATE::MOVE:
	{
		//MYDEBUG("MOVE \n");

		Move();

		if (PathMove()) return;

		m_zoneSession->m_basicInfo.unitInfo.state = STATE::IDLE;
	}
	break;
	}
}

void Dummy::Move()
{
	VECTOR2 temp1;
	VECTOR2 temp2;
	temp2.x = m_zoneSession->m_basicInfo.unitInfo.position.x;
	temp2.y = m_zoneSession->m_basicInfo.unitInfo.position.y;
	//============================================================================
	VECTOR2 tempVec2 = VECTOR2(m_targetPosition - m_zoneSession->m_basicInfo.unitInfo.position);
	float magnitude = tempVec2.Magnitude();

	if (magnitude <= 3.5f * (1.0f / 30.0f) || magnitude == 0.0f)
	{
		temp1 = m_targetPosition;

		m_zoneSession->m_basicInfo.unitInfo.position.x = temp1.x;
		m_zoneSession->m_basicInfo.unitInfo.position.y = temp1.y;
	}

	//두 직선, 벡터의 관계(사이각,회전각) 구하기
	//https://darkpgmr.tistory.com/121
	//VECTOR2(m_targetPosition - m_info.position) / magnitude 이 부분

	//https://www.youtube.com/watch?v=WxWJorOVIj8
	//https://www.youtube.com/watch?v=-gokuNjpyNg

	temp2 = temp2 + tempVec2 / magnitude * 3.5f * (1.0f / 10.0f);
	//각도 * 이동속도 * 시간

	m_zoneSession->m_basicInfo.unitInfo.position.x = temp2.x;
	m_zoneSession->m_basicInfo.unitInfo.position.y = temp2.y;
}

bool Dummy::PathMove()
{
	VECTOR2 temp;
	temp.x = m_zoneSession->m_basicInfo.unitInfo.position.x;
	temp.y = m_zoneSession->m_basicInfo.unitInfo.position.y;

	float distance = VECTOR2(temp - m_targetPosition).SqrMagnitude();

	//목표 좌표에 도착
	if (distance <= 0.1f)
	{
		m_zoneSession->m_basicInfo.unitInfo.position.x = m_targetPosition.x;
		m_zoneSession->m_basicInfo.unitInfo.position.y = m_targetPosition.y;
		tile = tilesData.GetTile(
			static_cast<int>(m_zoneSession->m_basicInfo.unitInfo.position.x),
			static_cast<int>(m_zoneSession->m_basicInfo.unitInfo.position.y));

		UserPositionPacket* userPositionPacket =
			reinterpret_cast<UserPositionPacket*>(m_zoneSession->GetSendBuffer()->
				GetBuffer(sizeof(UserPositionPacket)));
		userPositionPacket->userIndex = m_zoneSession->m_basicInfo.userInfo.userID;
		userPositionPacket->position = m_zoneSession->m_basicInfo.unitInfo.position;

		userPositionPacket->Init(SendCommand::C2Zone_USER_MOVE, sizeof(UserPositionPacket));
		m_zoneSession->GetSendBuffer()->Write(userPositionPacket->size);

		m_zoneSession->Send(reinterpret_cast<char*>(userPositionPacket), userPositionPacket->size);

		/*printf("[ %d test user : now position ( %f, %f ) ]\n", m_zoneSession->m_basicInfo.userInfo.userID,
			m_zoneSession->m_basicInfo.unitInfo.position.x, 
			m_zoneSession->m_basicInfo.unitInfo.position.y);*/

				//남은 타일리스트가 있다면
		if (tileList.size() > 0)
		{
			m_targetPosition = tileList.front();
			tileList.pop_front();
		}
		//남은 타일리스트가 없다면
		else
		{
			UserPositionPacket* userPositionPacket =
				reinterpret_cast<UserPositionPacket*>(m_zoneSession->GetSendBuffer()->
					GetBuffer(sizeof(UserPositionPacket)));
			userPositionPacket->userIndex = m_zoneSession->m_basicInfo.userInfo.userID;
			userPositionPacket->position = m_zoneSession->m_basicInfo.unitInfo.position;

			userPositionPacket->Init(SendCommand::C2Zone_USER_MOVE_FINISH, sizeof(UserPositionPacket));
			m_zoneSession->GetSendBuffer()->Write(userPositionPacket->size);

			m_zoneSession->Send(reinterpret_cast<char*>(userPositionPacket), userPositionPacket->size);

			return false;
		}
	}

	return true;
}