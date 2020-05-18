using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.IO;
using System.Linq;

[Serializable]
public class PlayerInfo
{
    public string name;
    public int lives;
    public float strength;
}

public static class JsonHelper
{
    public static T[] FromJson<T>(string json)
    {
        Wrapper<T> wrapper = JsonUtility.FromJson<Wrapper<T>>(json);
        return wrapper.items;
    }

    public static string ToJson<T>(T[] array)
    {
        Wrapper<T> wrapper = new Wrapper<T>();
        wrapper.items = array;
        return JsonUtility.ToJson(wrapper);
    }

    public static string ToJson<T>(T[] array, bool prettyPrint)
    {
        Wrapper<T> wrapper = new Wrapper<T>();
        wrapper.items = array;
        return JsonUtility.ToJson(wrapper, prettyPrint);
    }
    
    public static string ToJson<T>(List<T> list, bool prettyPrint)
    {
        ListWrapper<T> wrapper = new ListWrapper<T>();
        wrapper.items = list.ToList();
        return JsonUtility.ToJson(wrapper, prettyPrint);
    }

    [Serializable]
    private class Wrapper<T>
    {
        public T[] items;
    }

    [Serializable]
    private class ListWrapper<T>
    {
        public List<T> items;
    }

}

public class Json : MonoBehaviour
{
    //// Start is called before the first frame update
    //void Start()
    //{
    //    Save();
    //    Load();
    //}

    //// Update is called once per frame
    //void Update()
    //{

    //}

    //void Save()
    //{
    //    PlayerInfo[] playerInfo = new PlayerInfo[2];

    //    playerInfo[0] = new PlayerInfo();
    //    playerInfo[0].name = "a";
    //    playerInfo[0].lives = 5;
    //    playerInfo[0].strength = 25.0f;

    //    playerInfo[1] = new PlayerInfo();
    //    playerInfo[1].name = "b";
    //    playerInfo[1].lives = 3;
    //    playerInfo[1].strength = 30.2f;

    //    string toJson = JsonHelper.ToJson(playerInfo, prettyPrint: true);
    //    File.WriteAllText(Application.dataPath + "/Resources/data.json", toJson);
    //}

    //void Load()
    //{
    //    string jsonString = File.ReadAllText(Application.dataPath + "/Resources/data.json");
    //    PlayerInfo[] data = JsonHelper.FromJson<PlayerInfo>(jsonString);

    //    foreach (PlayerInfo playerInfo in data)
    //    {
    //        Debug.Log(playerInfo.name);
    //        Debug.Log(playerInfo.lives);
    //        Debug.Log(playerInfo.strength);
    //    }
    //}
}
