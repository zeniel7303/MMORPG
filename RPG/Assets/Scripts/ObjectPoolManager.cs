using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public struct Monsters
{
    public ushort monsterType;
    public Monster monsterPrefab;
}

public class ObjectPoolManager : Singleton<ObjectPoolManager>
{
    public ObjectPool<OtherPlayer> otherPlayers;
    public Dictionary<ushort, ObjectPool<Monster>> monsters;

    public OtherPlayer otherPlayerPrefab;
    public Monsters[] monstersPrefabs;

    public int playerObjectPoolCount = 0;
    public int monsterObjectPoolCount = 0;

    private void Awake()
    {
        base.Awake();

        otherPlayers = new ObjectPool<OtherPlayer>();

        GameObject otherPlayersGameObject = new GameObject();
        otherPlayersGameObject.transform.SetParent(this.transform);
        otherPlayersGameObject.name = "otherPlayers";

        otherPlayers.Init(playerObjectPoolCount, otherPlayerPrefab, otherPlayersGameObject.transform);

        monsters = new Dictionary<ushort, ObjectPool<Monster>>();

        for(int i = 0; i < monstersPrefabs.Length; i++)
        {
            GameObject monster = new GameObject();
            monster.transform.SetParent(this.transform);
            monster.name = "Monsters Num : " + i;

            monsters.Add(monstersPrefabs[i].monsterType, new ObjectPool<Monster>());
            monsters[monstersPrefabs[i].monsterType].Init(monsterObjectPoolCount, monstersPrefabs[i].monsterPrefab, monster.transform);
        }
    }
}