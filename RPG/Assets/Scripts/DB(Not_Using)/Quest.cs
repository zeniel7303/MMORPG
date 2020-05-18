using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using LitJson;

public class questInfo
{
    public string m_QuestisCleared;
    public string m_QuestIndex;
    public string m_QuestTitle;
    public string m_QuestText;
    public string m_QuestLevel;
    public string m_QuestCategory;
    public string m_QuestStartNPC;
    public string m_QuestEndNPC;
    public string m_QuestReward_EXP;
    public string m_QuestReward_Gold;

    public questInfo(string _isCleared, string _index, string _title, string _text, string _level, string _category,
                        string _startNpc, string _endNpc, string _rewardExp, string _rewardGold)
    {
        m_QuestisCleared = _isCleared;
        m_QuestIndex = _index;
        m_QuestTitle = _title;
        m_QuestText = _text;
        m_QuestLevel = _level;
        m_QuestCategory = _category;
        m_QuestStartNPC = _startNpc;
        m_QuestEndNPC = _endNpc;
        m_QuestReward_EXP = _rewardExp;
        m_QuestReward_Gold = _rewardGold;
    }
}

public class Quest : MonoBehaviour
{
    public List<questInfo> lQuest = new List<questInfo>();

    public static Quest instance;

    public void Awake()
    {
        if (instance == null)
        {
            Quest.instance = this;
        }
    }

    // Start is called before the first frame update
    void Start()
    {
        Load();
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void Save()
    {
        JsonData jQuestData = JsonMapper.ToJson(lQuest);

        File.WriteAllText(Application.dataPath + "/Resources/QuestDB.json", jQuestData.ToString());
    }

    public void Load()
    {
        string JsonString = File.ReadAllText(Application.dataPath + "/Resources/QuestDB.json");

        JsonData questData = JsonMapper.ToObject(JsonString);

        for (int i = 0; i < questData.Count; i++)
        {
            lQuest.Add(new questInfo(
                 questData[i]["QuestIsCleared"].ToString(),
                 questData[i]["QuestIndex"].ToString(),
                 questData[i]["QuestTitle"].ToString(),
                 questData[i]["QuestText"].ToString(),
                 questData[i]["QuestLevel"].ToString(),
                 questData[i]["QuestCategory"].ToString(),
                 questData[i]["QuestStartNPC"].ToString(),
                 questData[i]["QuestEndNPC"].ToString(),
                 questData[i]["QuestReward_EXP"].ToString(),
                 questData[i]["QuestReward_Gold"].ToString()));
        }
    }
}
