#include "PathFinder.h"

#include "MainThread.h"
//#include "FieldManager.h"

PathFinder::PathFinder()
{
	m_sendBuffer = new SendBuffer(30000);

	m_state = READY;

	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (m_hEvent == NULL)
	{
		int num = WSAGetLastError();

		MYDEBUG("[Event Handle Creating Fail - %d Error]\n", num);
	}

	InitializeCriticalSection(&m_cs);

	Thread::Start(this);
}

PathFinder::~PathFinder()
{
	DeleteCriticalSection(&m_cs);

	if (m_nowTile != nullptr) delete m_nowTile;
	if (m_targetTile != nullptr) delete m_targetTile;
	if (m_packet != nullptr) delete m_packet;
	if (m_pathFinderAgent != nullptr) delete m_pathFinderAgent;
	if(m_sendBuffer != nullptr) delete m_sendBuffer;
}

void PathFinder::LoopRun()
{
	while (1)
	{
		WaitForSingleObject(m_hEvent, INFINITE);

		m_state = ACTIVE;

		PathFindPacket* pathFindPacket = 
			static_cast<PathFindPacket*>(m_packet);
		
		m_nowTile =
		MainThread::getSingleton()->GetFieldManager()->
			GetFieldData(pathFindPacket->fieldNum)->
			GetTile(pathFindPacket->nowPosition.x, 
				pathFindPacket->nowPosition.y);

		m_targetTile =
			MainThread::getSingleton()->GetFieldManager()->
			GetFieldData(pathFindPacket->fieldNum)->
			GetTile(pathFindPacket->targetPosition.x,
				pathFindPacket->targetPosition.y);

		//MYDEBUG("now : %d, %d / target : %d , %d \n", m_nowTile->GetX(), m_nowTile->GetY(), m_targetTile->GetX(), m_targetTile->GetY());

		//잘못된 타일이면 or 찾는 타일이 막힌 타일 or 찾는 타일이 나 자신이면 다시
		if (m_nowTile == nullptr || m_targetTile == nullptr 
			|| m_targetTile->GetType() == TileType::BLOCK
			|| m_targetTile == m_nowTile)
		{
			MYDEBUG("1 Failed \n");

			//실패처리
			PathFindPacket_Failed* pathFindPacket_Failed =
				reinterpret_cast<PathFindPacket_Failed*>(m_sendBuffer->
					GetBuffer(sizeof(PathFindPacket_Failed)));
			pathFindPacket_Failed->Init(SendCommand::Path2Zone_PATHFIND_FAILED,
				sizeof(PathFindPacket_Failed));

			pathFindPacket_Failed->monsterNum = pathFindPacket->monsterNum;
			pathFindPacket_Failed->fieldNum = pathFindPacket->fieldNum;

			//MYDEBUG("size : %d \n", pathFindPacket_Failed->size);

			m_pathFinderAgent->Send(reinterpret_cast<char*>(pathFindPacket_Failed),
				pathFindPacket_Failed->size);

			CSLock csLock(m_cs);

			m_pathFinderAgent->GetReceiver()->GetRingBuffer()->Read(m_packet->size);

			m_state = READY;

			return;
		}

		m_pathFind.PathFind(&m_tileList, m_nowTile, m_targetTile);

		if (m_tileList.size() <= 0)
		{
			MYDEBUG("2 Failed \n");

			//실패처리
			PathFindPacket_Failed* pathFindPacket_Failed =
				reinterpret_cast<PathFindPacket_Failed*>(m_sendBuffer->
					GetBuffer(sizeof(PathFindPacket_Failed)));
			pathFindPacket_Failed->Init(SendCommand::Path2Zone_PATHFIND_FAILED, 
				sizeof(PathFindPacket_Failed));
		
			pathFindPacket_Failed->monsterNum = pathFindPacket->monsterNum;
			pathFindPacket_Failed->fieldNum = pathFindPacket->fieldNum;

			//MYDEBUG("size : %d \n", pathFindPacket_Failed->size);

			m_pathFinderAgent->Send(reinterpret_cast<char*>(pathFindPacket_Failed),
				pathFindPacket_Failed->size);
		}
		else
		{
			//MYDEBUG("%d monster Success \n", pathFindPacket->monsterNum);

			PathFindPacket_Success* pathFindPacket_Success =
				reinterpret_cast<PathFindPacket_Success*>(m_sendBuffer->
					GetBuffer(sizeof(PathFindPacket_Success)));

			pathFindPacket_Success->monsterNum = pathFindPacket->monsterNum;
			pathFindPacket_Success->fieldNum = pathFindPacket->fieldNum;
			pathFindPacket_Success->state = pathFindPacket->state;

			pathFindPacket_Success->listCount = m_tileList.size();
				
			int i = 0;
			for (const auto& element : m_tileList)
			{
				pathFindPacket_Success->positionList[i] = element;

				i++;
				//pathFindPacket_Success->listCount++;
			}

			pathFindPacket_Success->size = (sizeof(VECTOR2) * pathFindPacket_Success->listCount)
				+ sizeof(int) + sizeof(int) + sizeof(unsigned short) + sizeof(int) + sizeof(Packet);
			pathFindPacket_Success->Init(SendCommand::Path2Zone_PATHFIND_SUCCESS, 
				pathFindPacket_Success->size);

			//MYDEBUG("size : %d \n", pathFindPacket_Success->size);

			m_pathFinderAgent->Send(reinterpret_cast<char*>(pathFindPacket_Success),
				pathFindPacket_Success->size);
		}

		CSLock csLock(m_cs);

		m_pathFinderAgent->GetReceiver()->GetRingBuffer()->Read(m_packet->size);

		m_state = READY;
	}
}