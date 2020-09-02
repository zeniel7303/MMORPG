#include "PathFinder.h"

#include "MainThread.h"
//#include "FieldManager.h"

PathFinder::PathFinder()
{
	m_sendBuffer = new SendBuffer(30000);

	m_state = PATHFIND_READY;

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

		m_state = PATHFIND_ACTIVE;

		PathFindPacket* pathFindPacket =
			static_cast<PathFindPacket*>(m_packet);

		m_pathFind.PathFind(m_monster->GetTileList(), pathFindPacket->nowTile, pathFindPacket->targetTile);

		if (m_monster->GetTileList()->size() <= 0)
		{
			m_monster->PathFindFailed();
		}
		else
		{
			m_monster->PathFindSuccess(pathFindPacket->state);
		}

		m_state = PATHFIND_READY;
	}
}