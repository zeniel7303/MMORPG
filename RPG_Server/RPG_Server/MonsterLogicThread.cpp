#include "MonsterLogicThread.h"
#include "Field.h"
#include "ServerLogicThread.h"

#include "ConnectorClass.h"

MonsterLogicThread::MonsterLogicThread()
{

}

MonsterLogicThread::~MonsterLogicThread()
{
	m_monsterMap.clear();
}

void MonsterLogicThread::Init(Field* _field, FieldTilesData* _fieldTilesData, SectorManager* _sectorManager)
{
	m_field = _field;
	m_fieldTilesData = _fieldTilesData;
	m_sectorManager = _sectorManager;

	Tile** tileMap = m_fieldTilesData->GetMap();

	//ServerLogicThread의 SharedQueue을 알아온다.
	SharedQueue<PacketQueuePair>* packetQueue =
		ServerLogicThread::getSingleton()->GetSharedQueue();

	int monsterCount = 0;
	Monster* monster;

	for (int y = 0; y < m_fieldTilesData->GetMapSizeY(); y++)
	{
		for (int x = 0; x < m_fieldTilesData->GetMapSizeX(); x++)
		{
			WORD spawnIndex = tileMap[y][x].GetSpawnMonsterIndex();

			if (spawnIndex == 0) continue;

			//DB로 불러오기
			MonsterInfo monsterInfo;
			monsterInfo.index = monsterCount;
			monsterInfo.monsterType = spawnIndex;
			monsterInfo.position = 
				VECTOR2(static_cast<float>(x), static_cast<float>(y));

			MonsterData monsterData = 
				*CONNECTOR->GetMonsterData(monsterInfo.monsterType - 10001);;
			monster = new Monster(m_field, m_fieldTilesData, 
				m_sectorManager, *packetQueue);

			monster->Init(monsterInfo, monsterData);

			/*if (!MYSQLCLASS->GetMonsterInfo(monsterInfo.monsterType, 
				monsterData.hp.currentValue, monsterData.hp.maxValue,
				monsterData.attackDelay, monsterData.attackDamage, monsterData.attackDistance,
				monsterData.moveSpeed, monsterData.patrolRange, monsterData.patrolDelay,
				monsterData.returnDistance, monsterData.dropExp))
			{
				printf("Monster GetData Error !  ]\n");
			}
			else
			{
				monster->Init(monsterInfo, monsterData);
			}*/

			m_monsterMap.insert(make_pair(monster->GetInfo().index, monster));

			monsterCount++;
		}
	}

	printf("[ %d Field - %d Monsters Spawn ] \n", m_field->GetFieldNum(), monsterCount);
}

void MonsterLogicThread::LoopRun()
{
	map<int, Monster*>::iterator iterEnd = m_monsterMap.end();

	Monster* monster;

	while (1)
	{
		for (map<int, Monster*>::iterator iter = m_monsterMap.begin(); iter != iterEnd; iter++)
		{
			monster = iter->second;
			monster->Update();
		}

		Sleep(1000 / 10);
	}
}

void MonsterLogicThread::SendMonsterList(User* _user)
{
	MonsterInfoListPacket* monsterInfoListPacket = 
		reinterpret_cast<MonsterInfoListPacket*>(_user->GetSendBuffer()->
			GetBuffer(sizeof(MonsterInfoListPacket)));

	monsterInfoListPacket->monsterNum = m_monsterMap.size();

	int i = 0;
	Monster* monster;

	for (const auto& element : m_monsterMap)
	{
		monster = element.second;

		monsterInfoListPacket->info[i] = monster->GetInfo();

		i++;
	}

	monsterInfoListPacket->size = (sizeof(MonsterInfo) * monsterInfoListPacket->monsterNum)
									+ sizeof(WORD) + sizeof(Packet);

	monsterInfoListPacket->Init(SendCommand::Zone2C_MONSTER_INFO_LIST, monsterInfoListPacket->size);

	_user->GetSendBuffer()->Write(monsterInfoListPacket->size);
	_user->Send(reinterpret_cast<char*>(monsterInfoListPacket), monsterInfoListPacket->size);

	SendMonsterList_InRange(_user);

	printf("[ MONSTER LIST 전송 완료 ]\n");
}

void MonsterLogicThread::SendMonsterList_InRange(User* _user)
{
	int tempNum = 0;

	MonsterInfoListPacket* monsterInfoListPacket_InRange =
		reinterpret_cast<MonsterInfoListPacket*>(_user->GetSendBuffer()->
			GetBuffer(sizeof(MonsterInfoListPacket)));

	monsterInfoListPacket_InRange->monsterNum = 0;

	std::vector<Sector*> tempVec = *_user->GetSector()->GetRoundSectorsVec();

	for (const auto& tempSector : tempVec)
	{
		std::list<Monster*> tempList = *tempSector->Manager_List<Monster>::GetItemList();

		if (tempList.size() <= 0) continue;

		for (const auto& element : tempList)
		{
			monsterInfoListPacket_InRange->info[tempNum] = element->GetInfo();

			tempNum++;
			monsterInfoListPacket_InRange->monsterNum++;
		}
	}

	monsterInfoListPacket_InRange->size = (sizeof(MonsterInfo) * monsterInfoListPacket_InRange->monsterNum)
		+ sizeof(WORD) + sizeof(Packet);
	monsterInfoListPacket_InRange->Init(SendCommand::Zone2C_MONSTER_INFO_LIST_IN_RANGE, monsterInfoListPacket_InRange->size);

	_user->GetSendBuffer()->Write(monsterInfoListPacket_InRange->size);
	_user->Send(reinterpret_cast<char*>(monsterInfoListPacket_InRange), monsterInfoListPacket_InRange->size);
}

Monster* MonsterLogicThread::GetMonster(int _num)
{
	map<int, Monster*>::iterator iter;

	iter = m_monsterMap.find(_num);

	if (iter != m_monsterMap.end()) return iter->second;

	return nullptr;
}