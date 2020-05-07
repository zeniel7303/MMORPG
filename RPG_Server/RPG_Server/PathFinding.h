#pragma once
#include "stdafx.h"
#include "Tile.h"

class PathFinding
{
public:
	void PathFind(list<VECTOR2>* _pathList, Tile* _startTile, Tile* _finishTile)
	{
		//openList = 주변에 갈 수 있는 길들을 담은 List
		//clostList = 오픈리스트에서 F의 값이 가장 작은 Tile들을 담은 리스트 (갈 수 있는 길)

		//초기화 및 openList, closeList 생성
		_pathList->clear();
		list<Tile*> openList;
		list<Tile*> closeList;

		//시작타일을 초기화하고 열었다고 체크
		_startTile->Reset();
		openList.push_back(_startTile);
		_startTile->SetIsOpened(true);

		_startTile->SetNeighborParent(&openList, _finishTile);

		//시작타일을 OpenList에서 삭제 후 CloseList에 추가 후 닫았다고 체크
		openList.pop_front();
		_startTile->SetIsOpened(false);

		closeList.push_back(_startTile);
		_startTile->SetIsClosed(true);

		Tile* currentTile = nullptr;

		//closeList에 _startTile부터 _finishTile까지의 최단경로를 전부 담을때까지 반복되는 반복문
		while (1)
		{
			//openList를 IsTileChecked 기준으로 정렬
			//https://twpower.github.io/71-use-sort-and-stable_sort-in-cpp
			openList.sort(IsTileChecked);

			currentTile = openList.front();

			if (currentTile == _finishTile)
			{
				closeList.push_back(currentTile);
				currentTile->SetIsClosed(true);

				break;
			}

			if (!currentTile->GetIsClosed())
			{
				currentTile->SetNeighborParent(&openList, _finishTile);

				closeList.push_back(currentTile);
				openList.pop_front();

				currentTile->SetIsOpened(false);
				currentTile->SetIsClosed(true);
			}
		}

		//closeList를 역순으로 _pathList에 전부 담는다.
		//(위에서 currentTile이 _finishTile을 가리키고 있기때문에 그대로 다시 역순으로 _pathList에 담으면된다.)
		while (1)
		{
			_pathList->push_back(VECTOR2(static_cast<float>(currentTile->GetX()), 
				static_cast<float>(currentTile->GetY())));

			currentTile = currentTile->GetParentTile();

			if (currentTile == nullptr) break;
		}

		//역순으로 담겨있으므로 reverse해준다.
		reverse(_pathList->begin(), _pathList->end());
		//_pathList의 가장 앞부분은 _startTile이므로 삭제한다.
		//(현재 서 있는 지점이 _startTile이므로 이동하는데 필요없다.)
		_pathList->pop_front();
		//_pathList.erase(_pathList.begin());

		//지금까지 길찾기에 사용됐던 타일들 초기화한다.
		list<Tile*>::iterator openIterEnd = openList.end();
		list<Tile*>::iterator closeIterEnd = closeList.end();
		Tile* tile;

		for (list<Tile*>::iterator iter = openList.begin(); iter != openIterEnd; iter++)
		{
			tile = *iter;
			tile->SetIsOpened(false);
		}

		for (list<Tile*>::iterator iter = closeList.begin(); iter != closeIterEnd; iter++)
		{
			tile = *iter;
			tile->SetIsClosed(false);
		}
	}

	static bool IsTileChecked(Tile* a, Tile* b)
	{
		return (a->GetFitness() < b->GetFitness());
	}
};