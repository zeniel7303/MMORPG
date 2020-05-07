#include "Dummy.h"

Dummy::Dummy(int _num, int _fieldNum, FieldTilesData* _data)
{
	dummyNum = _num;
	fieldNum = _fieldNum;
	tilesData = *_data;

	patrolRange = /*(int)((rand() % 5) + 1);*/5;

	Init();
}

Dummy::~Dummy()
{
}

bool Dummy::Connect(IpEndPoint& _ipEndPoint)
{
	// 1. 家南积己
	m_socket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_socket == INVALID_SOCKET)
	{
		printf("Error - Invalid socket\n");

		return false;
	}

	// 2. 楷搬夸没
	if (connect(m_socket, (SOCKADDR*)&_ipEndPoint, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		printf("Error - Fail to connect\n");
		// 4. 家南辆丰
		closesocket(m_socket);
		// Winsock End
		WSACleanup();

		return false;
	}
	else
	{
		printf("Server Connected\n");

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

		if (currentTime < 5.0f) return;

		tile = tilesData.GetTile(m_basicInfo.unitInfo.position.x, 
			m_basicInfo.unitInfo.position.y);

		int randX = (rand() % patrolRange * 2) - patrolRange;
		int randY = (rand() % patrolRange * 2) - patrolRange;

		targetPosition = { m_basicInfo.unitInfo.position.x + randX,
			m_basicInfo.unitInfo.position.y + randY };

		Tile* tempTile = tilesData.GetTile(targetPosition.x, targetPosition.y);

		if (tempTile == nullptr)
		{
			currentTime = 0.0f;

			return;
		}

		pathFind.PathFind(&tileList, tile, tempTile);

		if (tileList.size() > 0)
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

			printf("[ %d test user : target position ( %f, %f ) ]\n", m_basicInfo.userInfo.userID,
				movePacket->position[tempNum - 1].x, movePacket->position[tempNum - 1].y);
		}

		currentTime = 0.0f;
	}
		break;
	}
}