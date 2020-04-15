#include "MonsterLogicThread.h"
#include "Zone.h"
#include "ServerLogicThread.h"

MonsterLogicThread::MonsterLogicThread()
{

}

MonsterLogicThread::~MonsterLogicThread()
{

}

void MonsterLogicThread::Init(Zone* _zone, ZoneTilesData* _zoneTilesData)
{
	m_zone = _zone;
	m_zoneTilesData = _zoneTilesData;

	Tile** tileMap = _zoneTilesData->GetMap();

	//ServerLogicThread의 SharedQueue을 알아온다.
	SharedQueue<PacketQueuePair>* packetQueue =
		ServerLogicThread::getSingleton()->GetSharedQueue();

	int monsterCount = 0;
	Monster* monster;

	for (int y = 0; y < _zoneTilesData->GetMapSizeY(); y++)
	{
		for (int x = 0; x < _zoneTilesData->GetMapSizeX(); x++)
		{
			WORD spawnIndex = tileMap[y][x].GetSpawnMonsterIndex();

			if (spawnIndex == 0) continue;

			//DB로 불러오기
			MonsterInfo monsterInfo;
			monsterInfo.index = monsterCount;
			monsterInfo.monsterType = spawnIndex;
			monsterInfo.position = 
				VECTOR2(static_cast<float>(x), static_cast<float>(y));

			MonsterData monsterData;
			monster = new Monster(m_zone, m_zoneTilesData, *packetQueue);

			if (!MYSQLCLASS->GetMonsterInfo(monsterInfo.monsterType, 
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
			}

			/*switch (monsterInfo.monsterType)
			{
			case 10001:
				monsterData.monsterType = 1;
				monsterData.hp = ZeroToMax(100, 100);
				monsterData.attackDelay = 2.5f;
				monsterData.attackDamage = 20;
				monsterData.attackDistance = 1.0f;
				monsterData.moveSpeed = 3.0f;
				monsterData.patrolRange = 10.0f;
				monsterData.patrolDelay = 10.0f;
				monsterData.returnDistance = 20.0f;
				monsterData.dropExp = 40;

				monster->Init(monsterInfo, monsterData);
				break;
			case 10002:
				monsterData.monsterType = 2;
				monsterData.hp = ZeroToMax(200, 200);
				monsterData.attackDelay = 2.5f;
				monsterData.attackDamage = 20;
				monsterData.attackDistance = 1.0f;
				monsterData.moveSpeed = 3.0f;
				monsterData.patrolRange = 5.0f;
				monsterData.patrolDelay = 10.0f;
				monsterData.returnDistance = 20.0f;
				monsterData.dropExp = 10;

				monster->Init(monsterInfo, monsterData);
				break;
			case 10003:
				monsterData.monsterType = 3;
				monsterData.hp = ZeroToMax(300, 300);
				monsterData.attackDelay = 2.5f;
				monsterData.attackDamage = 20;
				monsterData.attackDistance = 1.0f;
				monsterData.moveSpeed = 3.0f;
				monsterData.patrolRange = 5.0f;
				monsterData.patrolDelay = 10.0f;
				monsterData.returnDistance = 20.0f;
				monsterData.dropExp = 10;

				monster->Init(monsterInfo, monsterData);
				break;
			}
			*/
			m_monsterMap.insert(make_pair(monster->GetInfo().index, monster));

			monsterCount++;
		}
	}

	printf("%d Monsters Spawn  ]\n", monsterCount);
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

		Sleep(1000 / 30);
	}
}

void MonsterLogicThread::SendMonsterList(User* _user)
{
	MonsterInfoListPacket* monsterInfoListPacket = 
		reinterpret_cast<MonsterInfoListPacket*>(_user->GetSendBuffer()->
			GetBuffer(sizeof(MonsterInfoListPacket)));

	monsterInfoListPacket->monsterNum = m_monsterMap.size();

	map<int, Monster*>::iterator iterEnd = m_monsterMap.end();
	Monster* monster;

	int i = 0;
	map<int, Monster*>::iterator iter;
	for (iter = m_monsterMap.begin(); iter != iterEnd; iter++)
	{
		monster = iter->second;

		monsterInfoListPacket->info[i] = monster->GetInfo();

		i++;
	}

	monsterInfoListPacket->size = (sizeof(MonsterInfo) * monsterInfoListPacket->monsterNum)
									+ sizeof(WORD) + sizeof(Packet);

	monsterInfoListPacket->Init(SendCommand::MONSTER_INFO_LIST, monsterInfoListPacket->size);

	_user->GetSendBuffer()->Write(monsterInfoListPacket->size);
	_user->Send(reinterpret_cast<char*>(monsterInfoListPacket), monsterInfoListPacket->size);

	printf("[ MONSTER LIST 전송 완료 ]\n");
}

Monster* MonsterLogicThread::GetMonster(int _num)
{
	map<int, Monster*>::iterator iter;

	iter = m_monsterMap.find(_num);

	if (iter != m_monsterMap.end()) return iter->second;

	return nullptr;
}