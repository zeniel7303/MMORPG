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

	Thread::Start(this);
}

PathFinder::~PathFinder()
{
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

		m_pathFind.PathFind(&m_tileList, m_nowTile, m_targetTile);

		if (m_tileList.size() <= 0)
		{
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
			}

			pathFindPacket_Success->size = (sizeof(VECTOR2) * pathFindPacket_Success->listCount)
				+ sizeof(int) + sizeof(int) + sizeof(int) + sizeof(unsigned short) + sizeof(Packet);
			pathFindPacket_Success->Init(SendCommand::Path2Zone_PATHFIND_SUCCESS, 
				pathFindPacket_Success->size);

			//MYDEBUG("size : %d \n", pathFindPacket_Success->size);

			m_pathFinderAgent->Send(reinterpret_cast<char*>(pathFindPacket_Success),
				pathFindPacket_Success->size);
		}

		m_state = READY;
	}
}