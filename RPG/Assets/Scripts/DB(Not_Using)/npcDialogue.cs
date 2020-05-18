using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using LitJson;

public class npc_dialogue_Component
{
    public string Name;
    ///public List<string> dialogue;
    public string Dial1;
    public string Dial2;

    public npc_dialogue_Component(string name, string dial1, string dial2)
    {
        Name = name;
        Dial1 = dial1;
        Dial2 = dial2;
    }
}

public class npcDialogue : MonoBehaviour
{
    public static npcDialogue instance;

    public List<npc_dialogue_Component> lNdc = new List<npc_dialogue_Component>();

    public Dictionary<string, List<npc_dialogue_Component>> dNdc = new Dictionary<string, List<npc_dialogue_Component>>();

    public void Awake()
    {
        if (instance == null)
        {
            npcDialogue.instance = this;
        }
    }

    void Start()
    {
        //lNdc.Add(new npc_dialogue_Component("npc1", "안녕 ", "반가워"));
        //lNdc.Add(new npc_dialogue_Component("npc2", "하이 ", "나이스 투 미츄"));

        //Save();

        Load();
    }

    void Save()
    {
        JsonData jNpc_dialogue = JsonMapper.ToJson(lNdc);

        File.WriteAllText(Application.dataPath + "/Resources/npc_dialogue.json", jNpc_dialogue.ToString());
    }

    public void Load()
    {
        string JsonString = File.ReadAllText(Application.dataPath + "/Resources/npc_dialogue.json");

        JsonData npcDialogue = JsonMapper.ToObject(JsonString);

        for (int i = 0; i < npcDialogue.Count; i++)
        {
            lNdc.Add(new npc_dialogue_Component(npcDialogue[i]["Name"].ToString(),
                 npcDialogue[i]["Dial1"].ToString(),
                 npcDialogue[i]["Dial2"].ToString()));

            //dNdc.Add("npc1", lNdc);
        }

        //Debug.Log(lNdc[0].Name);
        //Debug.Log(lNdc[0].Dial1);
        //Debug.Log(lNdc[0].Dial2);

        //Debug.Log(lNdc[1].Name);
        //Debug.Log(lNdc[1].Dial1);
        //Debug.Log(lNdc[1].Dial2);

        //Debug.Log(dNdc["npc2"][1].Name);
        //Debug.Log(dNdc["npc2"][1].Dial1);
        //Debug.Log(dNdc["npc2"][1].Dial2);

        //Debug.Log(npcDialogue[0]["Name"].ToString());
        //Debug.Log(npcDialogue[0]["Dial1"].ToString());
        //Debug.Log(npcDialogue[0]["Dial2"].ToString());
    }
}
