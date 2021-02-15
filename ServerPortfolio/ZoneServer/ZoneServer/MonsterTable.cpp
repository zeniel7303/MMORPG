#include "MonsterTable.h"

MonsterTable::MonsterTable()
{
}

MonsterTable::~MonsterTable()
{
	m_monsterDataVec.clear();
	m_monsterDataVec.resize(0);
}

void MonsterTable::GetMonstersData(Packet* _packet)
{
	MonstersInfoPacket* monstersInfoPacket = reinterpret_cast<MonstersInfoPacket*>(_packet);

	m_monsterDataVec.reserve(monstersInfoPacket->count);

	for (int i = 0; i < monstersInfoPacket->count; i++)
	{
		m_monsterDataVec.emplace_back(monstersInfoPacket->monstersData[i]);
	}
}