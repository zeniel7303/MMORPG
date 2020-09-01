#include "PathFinderManager.h"

PathFinderManager::PathFinderManager()
{
	PathFinder* pathFinder;

	for (int i = 0; i < 100; i++)
	{
		pathFinder = new PathFinder();

		m_pathFinderList.push_back(pathFinder);
	}
}

PathFinderManager::~PathFinderManager()
{
}