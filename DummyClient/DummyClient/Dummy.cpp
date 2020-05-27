#include "Dummy.h"

Dummy::Dummy(int _num, int _fieldNum, FieldTilesData* _data)
{
	dummyNum = _num;
	fieldNum = _fieldNum;
	tilesData = *_data;

	patrolRange = /*(int)((rand() % 5) + 1);*/7;

	Init();

	intervalTime = (float)(rand() % 10) + 1;
}

Dummy::~Dummy()
{
}

bool Dummy::Connect(IpEndPoint& _ipEndPoint)
{
	// 1. 소켓생성
	m_socket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_socket == INVALID_SOCKET)
	{
		printf("Error - Invalid socket\n");

		return false;
	}

	// 2. 연결요청
	if (connect(m_socket, (SOCKADDR*)&_ipEndPoint, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		printf("Error - Fail to connect\n");
		// 4. 소켓종료
		closesocket(m_socket);
		// Winsock End
		WSACleanup();

		return false;
	}
	else
	{
		printf("%d Dummy Client Server Connected\n", dummyNum);

		return true;
	}
}

void Dummy::OnConnect(SOCKET _socket)
{
	Session::OnConnect(_socket);

	if (fieldNum == 1)
	{
		TestClientEnterPacket* testClientEnterPacket =
			reinterpret_cast<TestClientEnterPacket*>(GetSendBuffer()->
				GetBuffer(sizeof(TestClientEnterPacket)));
		testClientEnterPacket->Init(SendCommand::C2Zone_ENTER_TEST_USER, sizeof(TestClientEnterPacket));
		testClientEnterPacket->userNum = 60000 + dummyNum;
		testClientEnterPacket->fieldNum = 999;
		m_sendBuffer->Write(testClientEnterPacket->size);

		Send(reinterpret_cast<char*>(testClientEnterPacket), testClientEnterPacket->size);
	}
	else if (fieldNum == 2)
	{
		TestClientEnterPacket* testClientEnterPacket =
			reinterpret_cast<TestClientEnterPacket*>(GetSendBuffer()->
				GetBuffer(sizeof(TestClientEnterPacket)));
		testClientEnterPacket->Init(SendCommand::C2Zone_ENTER_TEST_USER, sizeof(TestClientEnterPacket));
		testClientEnterPacket->userNum = 60000 + dummyNum;
		testClientEnterPacket->fieldNum = 2;
		m_sendBuffer->Write(testClientEnterPacket->size);

		Send(reinterpret_cast<char*>(testClientEnterPacket), testClientEnterPacket->size);
	}
}

void Dummy::RandomTask()
{
	currentTime += 1.0f / 10.0f;

	if (intervalTime == 0.0f)
	{
		intervalTime = (float)((rand() % 10) + 1);
	}

	if (currentTime < intervalTime) return;

	//==========================================

	TestClientMovePacket* movePacket =
		reinterpret_cast<TestClientMovePacket*>(GetSendBuffer()->
			GetBuffer(sizeof(TestClientMovePacket)));;

	movePacket->userIndex = m_basicInfo.userInfo.userID;

	//==========================================

	currentTime = 0.0f;
	intervalTime = 0.0f;
}

void Dummy::FSM()
{
	switch (m_basicInfo.unitInfo.state)
	{
	case STATE::IDLE:
	{
		currentTime += 1.0f / 10.0f;

		if (currentTime < intervalTime) return;

		tile = tilesData.GetTile(m_basicInfo.unitInfo.position.x, 
			m_basicInfo.unitInfo.position.y);

		int randX = (rand() % ((patrolRange * 2) + 1)) - patrolRange;
		int randY = (rand() % ((patrolRange * 2) + 1)) - patrolRange;

		m_targetPosition = { m_basicInfo.unitInfo.position.x + randX,
			m_basicInfo.unitInfo.position.y + randY };

		Tile* tempTile = tilesData.GetTile(m_targetPosition.x, m_targetPosition.y);

		//잘못된 타일이면 다시 or 찾는 타일이 막힌 타일 or 찾는 타일이 나 자신이면 다시
		if (tempTile == nullptr || tempTile->GetType() == TileType::BLOCK
			|| tempTile == tile)
		{
			currentTime = 0.0f;

			return;
		}

		pathFind.PathFind(&tileList, tile, tempTile);

		if (tileList.size() > 0)
		{
			m_targetPosition = tileList.front();

			tileList.pop_front();

			m_basicInfo.unitInfo.state = STATE::MOVE;

			//VECTOR2 destinationPosition;
			//if (tileList.size() > 0)
			//{
			//	destinationPosition = tileList.back();
			//}
			//else
			//{
			//	destinationPosition = m_targetPosition;
			//}

			//printf("[ %d test user : destination position ( %f, %f ) ]\n", m_basicInfo.userInfo.userID,
			//	destinationPosition.x, destinationPosition.y);
		}

		/*if (tileList.size() > 0)
		{
			TestClientMovePacket* movePacket =
				reinterpret_cast<TestClientMovePacket*>(GetSendBuffer()->
					GetBuffer(sizeof(TestClientMovePacket)));;
			movePacket->userIndex = m_basicInfo.userInfo.userID;
			movePacket->tileCount = 0;

			int tempNum = 0;
			for (const auto& element : tileList)
			{
				movePacket->position[tempNum].x = element.x;
				movePacket->position[tempNum].y = element.y;

				movePacket->tileCount++;
				tempNum++;
			}

			movePacket->size = (sizeof(Position) * movePacket->tileCount)
				+ sizeof(WORD) + sizeof(Packet);
			movePacket->Init(SendCommand::C2Zone_MOVE_TEST_USER, movePacket->size);
			m_sendBuffer->Write(movePacket->size);
			Send(reinterpret_cast<char*>(movePacket), movePacket->size);

			m_basicInfo.unitInfo.state = STATE::MOVE;

			//printf("[ %d user : target ( %f, %f ) ]\n", m_basicInfo.userInfo.userID,
			//	movePacket->position[tempNum - 1].x, movePacket->position[tempNum - 1].y);
		}*/

		currentTime = 0.0f;

		intervalTime = (float)(rand() % 10) + 1;
	}
		break;
	case STATE::MOVE:
	{
		Move();

		if (PathMove()) return;

		m_basicInfo.unitInfo.state = STATE::IDLE;
	}
		break;
	}
}

void Dummy::Move()
{
	VECTOR2 temp1;
	VECTOR2 temp2;
	temp2.x = m_basicInfo.unitInfo.position.x;
	temp2.y = m_basicInfo.unitInfo.position.y;
	//============================================================================
	VECTOR2 tempVec2 = VECTOR2(m_targetPosition - m_basicInfo.unitInfo.position);
	float magnitude = tempVec2.Magnitude();

	if (magnitude <= 3.5f * (1.0f / 30.0f) || magnitude == 0.0f)
	{
		temp1 = m_targetPosition;

		m_basicInfo.unitInfo.position.x = temp1.x;
		m_basicInfo.unitInfo.position.y = temp1.y;
	}

	//두 직선, 벡터의 관계(사이각,회전각) 구하기
	//https://darkpgmr.tistory.com/121
	//VECTOR2(m_targetPosition - m_info.position) / magnitude 이 부분

	//https://www.youtube.com/watch?v=WxWJorOVIj8
	//https://www.youtube.com/watch?v=-gokuNjpyNg

	temp2 = temp2 + tempVec2 / magnitude * 3.5f * (1.0f / 10.0f);
	//각도 * 이동속도 * 시간

	m_basicInfo.unitInfo.position.x = temp2.x;
	m_basicInfo.unitInfo.position.y = temp2.y;
}

bool Dummy::PathMove()
{
	VECTOR2 temp;
	temp.x = m_basicInfo.unitInfo.position.x;
	temp.y = m_basicInfo.unitInfo.position.y;

	float distance = VECTOR2(temp - m_targetPosition).SqrMagnitude();

	//목표 좌표에 도착
	if (distance <= 0.1f)
	{
		m_basicInfo.unitInfo.position.x = m_targetPosition.x;
		m_basicInfo.unitInfo.position.y = m_targetPosition.y;
		tile = tilesData.GetTile(
			static_cast<int>(m_basicInfo.unitInfo.position.x),
			static_cast<int>(m_basicInfo.unitInfo.position.y));

		UserPositionPacket* userPositionPacket =
			reinterpret_cast<UserPositionPacket*>(m_sendBuffer->
				GetBuffer(sizeof(UserPositionPacket)));
		userPositionPacket->userIndex = m_basicInfo.userInfo.userID;
		userPositionPacket->position = m_basicInfo.unitInfo.position;

		userPositionPacket->Init(SendCommand::C2Zone_USER_MOVE, sizeof(UserPositionPacket));
		Session::GetSendBuffer()->Write(userPositionPacket->size);

		Send(reinterpret_cast<char*>(userPositionPacket), userPositionPacket->size);

		/*printf("[ %d test user : now position ( %f, %f ) ]\n", m_basicInfo.userInfo.userID,
				m_basicInfo.unitInfo.position.x, m_basicInfo.unitInfo.position.y);*/

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
				reinterpret_cast<UserPositionPacket*>(m_sendBuffer->
					GetBuffer(sizeof(UserPositionPacket)));
			userPositionPacket->userIndex = m_basicInfo.userInfo.userID;
			userPositionPacket->position = m_basicInfo.unitInfo.position;

			userPositionPacket->Init(SendCommand::C2Zone_USER_MOVE_FINISH, sizeof(UserPositionPacket));
			Session::GetSendBuffer()->Write(userPositionPacket->size);

			Send(reinterpret_cast<char*>(userPositionPacket), userPositionPacket->size);

			return false;
		}
	}

	return true;
}