#pragma once
#include <list>

#include "PathFinder.h"

class PathFinderManager
{
private:
	std::list<PathFinder*> m_pathFinderList;

public:
	PathFinderManager();
	~PathFinderManager();

	std::list<PathFinder*> GetPathFinderList() { return m_pathFinderList; }
};

