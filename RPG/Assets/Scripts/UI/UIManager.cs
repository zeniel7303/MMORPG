using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class UIManager : Singleton<UIManager>
{
    public PlayerInfoUI _playerInfoUI;
    public dialogue _dialogue;
    public Inventory _inven;
    public GameObject DamagePrint;
    public Canvas DeathUI;
    public Canvas EscapeUI;
    public Canvas StatUI;
    public Canvas ZoneSelectUI;
    public Text zoneNumText;

    public LevelUpUI LevelUpCanvas;

    public GameObject PrefabFloatingTXT;

    public void Start()
    {
        DeathUI.gameObject.SetActive(false);
        EscapeUI.gameObject.SetActive(false);
        ZoneSelectUI.gameObject.SetActive(false);
        StatUI.gameObject.SetActive(false);
    }

    public void Update()
    {
        zoneNumText.text = "현재 채널 : " + (ServerManager.Instance.myZone + 1).ToString();
    }

    public void Revive()
    {
        Packet UserRevivePacket = new Packet(SendCommand.C2Zone_USER_REVIVE);
        ServerManager.instance.SendData_ZoneServer(UserRevivePacket.GetBytes());
    }

    public void SetFloatingText(GameObject vr, string damage, bool isEnemy) //몬스터의 위치, 총알의 공격력을 받아옴
    {
        if (PrefabFloatingTXT == null)
        {
            Debug.Log("FloatingText Null");
        }

        GameObject TXT = Instantiate(PrefabFloatingTXT);

        TXT.transform.position = vr.transform.position;

        TXT.transform.SetParent(DamagePrint.gameObject.transform);
        TXT.gameObject.transform.GetComponent<FloatingText>().print(damage, isEnemy);
    }

}
