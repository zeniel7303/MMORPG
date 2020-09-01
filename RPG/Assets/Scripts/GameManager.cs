using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class GameManager : Singleton<GameManager>
{
    public PlayerManager player;
    public TilesGroup tileMap;

    public MapManager mapManager;

    public Chatting chattingUI;

    public TitleUI titleUI;

    public string logInId;
    public string logInPassword;

    public string registerId;
    public string registerPassword;

    public bool isInGame;
    public bool isNormalExit;

    // Start is called before the first frame update
    void Start()
    {
        logInId.Replace("\0", string.Empty);
        logInPassword.Replace("\0", string.Empty);
        registerId.Replace("\0", string.Empty);
        registerPassword.Replace("\0", string.Empty);

        isInGame = false;
        isNormalExit = false;
    }

    // Update is called once per frame
    void Update()
    {
        if(isInGame)
        {
            if((Input.GetKeyDown(KeyCode.Escape)))
            {
                if(UIManager.Instance.EscapeUI.isActiveAndEnabled)
                {
                    UIManager.Instance.EscapeUI.gameObject.SetActive(false);
                }
                else
                {
                    UIManager.Instance.EscapeUI.gameObject.SetActive(true);
                }
            }
        }

        if (mapManager == null) return;
    }

    void SetLogInId(string _string)
    {
        logInId = _string;
    }

    void SetLogInPassword(string _string)
    {
        logInPassword = _string;
    }

    void SetRegisterId(string _string)
    {
        registerId = _string;
    }

    void SetRegisterPassword(string _string)
    {
        registerPassword = _string;
    }

    private void OnApplicationQuit()
    {
        if (!isInGame) return;

        if (isNormalExit) return;

        SessionInfoPacket updateInfoPacket = new SessionInfoPacket();
        updateInfoPacket.SetCmd(SendCommand.C2Zone_UPDATE_INFO);
        updateInfoPacket.info.userInfo = PlayerManager.instance.userInfo;
        updateInfoPacket.info.unitInfo = PlayerManager.instance.unitInfo;

        ServerManager.Instance.SendData_ZoneServer(updateInfoPacket.GetBytes());

        Debug.Log("업데이트할 info 전송 - 비정상 종료");
    }

    //private void Awake()
    //{
    //    //PathFinder.Init(ref tileMap);

    //    //Cursor.lockState = CursorLockMode.Locked;//마우스 커서 고정
    //    //Cursor.visible = false;//마우스 커서 보이기
    //}
}
