#include "MonsterManager.h"
#include "Field.h"

#include "MainThread.h"
#include "DBConnector.h"
#include "MonsterTable.h"

MonsterManager::MonsterManager(Field* _field, FieldTilesData* _fieldTilesData,
	SectorManager* _sectorManager)
{
	m_field = _field;
	m_fieldTilesData = _fieldTilesData;
	m_sectorManager = _sectorManager;
}

MonsterManager::~MonsterManager()
{
	for (int i = 0; i < m_monsterVec.size(); i++)
	{
		delete m_monsterVec[i];
	}
	
	m_monsterVec.clear();
}

bool MonsterManager::CreateMonsters()
{
	Tile** tileMap = m_fieldTilesData->GetMap();

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
				*MonsterTable::getSingleton()->GetMonsterData(monsterInfo.monsterType - 10001);

			TRYCATCH(monster = new Monster(m_field, m_fieldTilesData, 
				m_sectorManager, monsterInfo, monsterData));
			if (monster->IsFailed())
			{
				MYDEBUG("[ %d Field - %d Monsters Spawn Failed ] \n", m_field->GetFieldNum(), monsterCount);
				return false;
			}

			m_monsterVec.emplace_back(monster);

			monsterCount++;
		}
	}

	MYDEBUG("[ %d Field - %d Monsters Spawn ] \n", m_field->GetFieldNum(), monsterCount);

	return true;
}

void MonsterManager::Update()
{
	for (auto& element : m_monsterVec)
	{
		element->Update();
	}
}

void MonsterManager::SendMonsterList(User* _user)
{
	MonsterInfoListPacket* monsterInfoListPacket = new MonsterInfoListPacket();

	monsterInfoListPacket->monsterNum = WORD(m_monsterVec.size());

	int i = 0;
	for (const auto& element : m_monsterVec)
	{
		monsterInfoListPacket->info[i] = element->GetInfo();

		i++;
	}

	monsterInfoListPacket->size = (sizeof(MonsterInfo) * monsterInfoListPacket->monsterNum)
									+ sizeof(WORD) + sizeof(Packet);

	monsterInfoListPacket->Init(SendCommand::Zone2C_MONSTER_INFO_LIST, monsterInfoListPacket->size);

	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(monsterInfoListPacket));
	_user->AddToSendQueue(tempPtr);

	SendMonsterList_InRange(_user);

	MYDEBUG("[ MONSTER LIST 전송 완료 ]\n");
}

void MonsterManager::SendMonsterList_InRange(User* _user)
{
	int tempNum = 0;

	MonsterInfoListPacket* monsterInfoListPacket_InRange =
		new MonsterInfoListPacket();

	monsterInfoListPacket_InRange->monsterNum = 0;

	std::vector<Sector*>& tempVec = _user->GetSector()->GetRoundSectorsVec();

	for (const auto& tempSector : tempVec)
	{
		const std::list<Monster*> tempList = tempSector->GetMonsterList()->GetItemList();

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

	SharedPointer<char> tempPtr =
		SharedPointer<char>(reinterpret_cast<char*>(monsterInfoListPacket_InRange));
	_user->AddToSendQueue(tempPtr);
}

Monster* MonsterManager::GetMonster(int _num)
{
	return m_monsterVec[_num];
}