using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using LitJson;

public class PlayerStat
{
    public string Name;
    public int Hp;
    public int Mp;
    public int Atk;
    public int Def;

    public PlayerStat(string name, int hp, int mp, int atk, int def)
    {
        Name = name;
        Hp = hp;
        Mp = mp;
        Atk = atk;
        Def = def;
    }
}

public class Data : MonoBehaviour
{
    public static Data instance;

    public void Awake()
    {
        if (instance == null)
        {
            Data.instance = this;
        }
    }

    public List<PlayerStat> lPlayerStats = new List<PlayerStat>();

    void Start()
    {
        //Debug.Log("스타트");
        //lPlayerStats.Add(  PlayerStat("a", 100, 100, 10, 10));
        //Debug.Log("추가");
        //Save();
        //Debug.Log("세이브");
    }

    void Save()
    {
        JsonData PlayerJson = JsonMapper.ToJson(lPlayerStats);

        File.WriteAllText(Application.dataPath + "/Resources/PlayerStat.json", PlayerJson.ToString());
    }

    public void Load()
    {
        string JsonString = File.ReadAllText(Application.dataPath + "/Resources/PlayerStat.json");

        JsonData playerData = JsonMapper.ToObject(JsonString);
    }
}
