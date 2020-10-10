using System.Collections;
using System.Collections.Generic;
using UnityEngine;

//다중 충돌
//https://m.blog.naver.com/PostView.nhn?blogId=his00722&logNo=220176366306&proxyReferer=http%3A%2F%2Fwww.google.co.kr%2Furl%3Fsa%3Dt%26rct%3Dj%26q%3D%26esrc%3Ds%26source%3Dweb%26cd%3D2%26ved%3D2ahUKEwjVsorClsDiAhVRUd4KHXV2ApQQFjABegQIBhAB%26url%3Dhttp%253A%252F%252Fm.blog.naver.com%252Fhis00722%252F220176366306%26usg%3DAOvVaw3XVCXzN4g1lnezGBuFwfkR

[RequireComponent(typeof(Animator))]
[RequireComponent(typeof(CapsuleCollider))]
[RequireComponent(typeof(Rigidbody))]

public class PlayerManager : Unit
{
    public static PlayerManager instance;

    //[HideInInspector]
    public PlayerAnimation playerAnimation;
    //[HideInInspector]
    public PlayerMovement playerMovement;
    //[HideInInspector]
    public PlayerAttackManage playerAttack;

    public UIManager ui;
    public Quest quest;
    public dialogue dialogue;
    public GameObject damagePivot;

    public FloatingChatting floatingChattingPrefab;
    private FloatingChatting floatingChatting;

    //======================================================

    [HideInInspector]
    public UserInfo userInfo;

    [HideInInspector]
    public bool isInTitleOrLoading = true;
    [HideInInspector]
    public bool isTalk = false;
    [HideInInspector]
    public bool isAttack = false;
    [HideInInspector]
    public bool isDeath = false;

    //======================================================

    protected void Awake()
    {
        unitInfo.zoneNum = 0;

        instance = this;
        GameManager.Instance.player = this;

        playerAnimation = this.gameObject.AddComponent<PlayerAnimation>();
        if (playerAnimation == null)
            playerAnimation = this.gameObject.AddComponent<PlayerAnimation>();
        playerAnimation.Init(this);

        playerMovement = this.gameObject.AddComponent<PlayerMovement>();
        if (playerMovement == null)
            playerMovement = this.gameObject.AddComponent<PlayerMovement>();
        playerMovement.myManager = this;

        playerAttack = this.gameObject.AddComponent<PlayerAttackManage>();
        if(playerAttack == null)
            playerAttack = this.gameObject.AddComponent<PlayerAttackManage>();
        playerAttack.Init(this);

        floatingChatting = FloatingChatting.Instantiate(floatingChattingPrefab);
        floatingChatting.transform.SetParent(this.transform);
        floatingChatting.transform.localScale = new Vector3(0.04f, 0.04f, 0.04f);
        floatingChatting.transform.localPosition = new Vector3(0, 1.1f, 0);

        unitInfo.hp.currentValue = 1;
        unitInfo.hp.maxValue = 1;

        playerUI();
    }

    public void Spawn()
    {
        Vector3 vec = new Vector3(unitInfo.position.position.x,
                                  3, unitInfo.position.position.z);

        this.transform.position = vec;

        playerMovement.Init();

        UIManager.Instance._playerInfoUI.Level.text = unitInfo.level.ToString();

        string playerName = userInfo.name.ToString();
        UIManager.Instance._playerInfoUI.PlayerName.text = playerName;
    }

    public void Respawn(UnitInfo _info)
    {
        isDeath = false;

        playerAnimation.AnimationPlay("Revive");

        UIManager.Instance.DeathUI.gameObject.SetActive(false);

        unitInfo.hp = _info.hp;
        unitInfo.mp = _info.mp;

        unitInfo.position = _info.position;

        Spawn();
    }

    public void PlusExp(int _exp)
    {
        GameManager.Instance.chattingUI.SystemChattingInput(
            string.Format("경험치 획득 : {0}.", _exp));

        unitInfo.exp.currentValue += _exp;

        int tempExp = unitInfo.exp.currentValue - unitInfo.exp.maxValue;

        while(tempExp >= 0)
        {
            unitInfo.exp.currentValue = 0;
            unitInfo.exp.currentValue += tempExp;

            tempExp = unitInfo.exp.currentValue - unitInfo.exp.maxValue;
        }
    }

    public void LevelUp()
    {
        GameManager.Instance.chattingUI.SystemChattingInput(string.Format("Level Up!"));

        unitInfo.level += 1;

        //unitInfo.exp.currentValue = 0;
        unitInfo.hp.maxValue += 50;
        unitInfo.hp.currentValue = unitInfo.hp.maxValue;

        unitInfo.atk += 5;
        unitInfo.def += 2;

        UIManager.Instance._playerInfoUI.Level.text = unitInfo.level.ToString();

        UIManager.Instance.LevelUpCanvas.LevelUp();

        SessionInfoPacket updateInfoPacket = new SessionInfoPacket();
        updateInfoPacket.SetCmd(SendCommand.C2Zone_UPDATE_INFO);
        updateInfoPacket.info.userInfo = PlayerManager.instance.userInfo;
        updateInfoPacket.info.unitInfo = PlayerManager.instance.unitInfo;

        ServerManager.Instance.SendData_ZoneServer(updateInfoPacket.GetBytes());
    }

    public void Init()
    {
        isInTitleOrLoading = false;

        this.transform.position.SetY(3.0f);
    }

    // Update is called once per frame
    void Update()
    {
        //int x = (int)this.transform.position.x;
        //int y = (int)this.transform.position.z;

        if (isInTitleOrLoading) return;

        if (unitInfo.state != STATE.ATTACK &&
            unitInfo.state != STATE.MOVE &&
            !playerAttack.isInAttackCoolTime &&
            Input.GetMouseButtonDown(1))
        {
            playerAttack.Attack();
        }

        //플레이어 UI 갱신
        playerUI();

        playerMovement.CharacterMove();

        if(Input.GetKeyDown(KeyCode.P))
        {
            if (UIManager.Instance.StatUI.isActiveAndEnabled)
            {
                UIManager.Instance.StatUI.gameObject.SetActive(false);
            }
            else
            {
                UIManager.Instance.StatUI.gameObject.SetActive(true);
            }
        }

        if (this.transform.position.y <= -10.0f)
        {
            Spawn();
        }
    }

    public void playerUI()
    {
        UIManager.Instance._playerInfoUI.playerHPBar.maxValue = unitInfo.hp.maxValue;
        UIManager.Instance._playerInfoUI.playerMPBar.maxValue = unitInfo.mp.maxValue;
        UIManager.Instance._playerInfoUI.playerEXPBar.maxValue = unitInfo.exp.maxValue;

        UIManager.Instance._playerInfoUI.playerHPBar.value = unitInfo.hp.currentValue;
        UIManager.Instance._playerInfoUI.playerMPBar.value = unitInfo.mp.currentValue;
        UIManager.Instance._playerInfoUI.playerEXPBar.value = unitInfo.exp.currentValue;

        UIManager.Instance._playerInfoUI.playerHPText.text =
               string.Format("{0} / {1}", unitInfo.hp.currentValue, unitInfo.hp.maxValue);

        UIManager.Instance._playerInfoUI.playerMPText.text =
               string.Format("{0} / {1}", unitInfo.mp.currentValue, unitInfo.mp.maxValue);
    }

    public void Hit(int _damage)
    {
        unitInfo.hp.currentValue -= _damage;

        UIManager.Instance.SetFloatingText(this.gameObject, _damage.ToString(), false);
    }

    public void Death()
    {
        if (isDeath) return;

        isDeath = true;

        playerAnimation.AnimationPlay("Death");

        UIManager.Instance.DeathUI.gameObject.SetActive(true);
    }

    public void Chatting(string _chatting)
    {
        floatingChatting.Floating(_chatting, false);
    }

    public void QuestCheck(string name)
    {
        Cursor.lockState = CursorLockMode.None;
        Cursor.visible = true;

        dialogue.m_npcName = name;

        dialogue.OnDialogueOpen(true);

        //퀘스트 버튼 표시 여부
        for (int i = 0; i < quest.lQuest.Count; i++)
        {
            if (quest.lQuest[i].m_QuestisCleared == "No" 
                && quest.lQuest[i].m_QuestStartNPC == name)
            {
                if (this.unitInfo.GetLevel() >= int.Parse(quest.lQuest[i].m_QuestLevel))
                {
                    dialogue.m_questTitle.text = quest.lQuest[i].m_QuestTitle;
                    dialogue.m_questText.text = quest.lQuest[i].m_QuestText;

                    dialogue.m_questButton.SetActive(true);

                    break;
                }
            }
        }
    }

    private void OnCollisionEnter(Collision col)
    {
        if (col.collider.tag == "npc1")
        {
            Debug.Log("npc1 닿았다.");

            QuestCheck("npc1");
        }
        else if (col.collider.tag == "npc2")
        {
            Debug.Log("npc2 닿았다.");

            QuestCheck("npc2");
        }
    }

    public void SetUserInfo(UserInfo _userInfo)
    {
        userInfo = _userInfo;
    }
}
