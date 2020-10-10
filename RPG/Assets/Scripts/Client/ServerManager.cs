using System.Collections;
using System.Collections.Generic;
using System.Text;
using UnityEngine;

public class ServerManager : Singleton<ServerManager>
{
    public int userID;
    bool isConnect;
    public string ip;
    public ushort logInServerPort;
    public ushort firstFieldNum;
    public ushort myZone;
    public ushort zoneServerPort;

    private Session session = new Session();
    private Session logInSession = new Session();

    public MapManager mapManager;

    public bool isLogInConnect;
    public bool isRegisterConnect;
    public bool isStartConnect;

    private float heartbeatTime = 0.0f;

    // Start is called before the first frame update
    void Start()
    {
        isLogInConnect = false;
        isRegisterConnect = false;
        isStartConnect = false;

        Init();
    }

    // Update is called once per frame
    void Update()
    {
        session.Update();

        logInSession.Update();

        if (!isStartConnect) return;

        heartbeatTime += Time.deltaTime;

        //2초마다 실행
        if (heartbeatTime >= 2.0f)
        {
            HeartBeat();

            heartbeatTime = 0.0f;
        }

        //if(isChangeZone)
        //{
        //    time += Time.deltaTime;

        //    if (time >= 5.0f)
        //    {
        //        time = 0.0f;
        //        isChangeZone = false;
        //    }
        //}
    }

    public void Init()
    {
        logInSession.Init();
        logInSession.isLoginSession = true;
    }

    public void ZoneServerConnect(int _num)
    {
        if (GameManager.Instance.titleUI != null)
        {
            GameManager.Instance.titleUI.CloseZoneSelectWindow();
            GameManager.Instance.titleUI = null;
        }

        myZone = (ushort)_num;
        session.Init();

        switch (_num)
        {
            case 0:             
                session.Connect(ip, zoneServerPort);
                break;
            case 1:
                session.Connect(ip, zoneServerPort + 1);
                break;
            case 2:
                session.Connect(ip, zoneServerPort + 2);
                break;
            case 3:
                session.Connect(ip, zoneServerPort + 3);
                break;
            case 4:
                session.Connect(ip, zoneServerPort + 4);
                break;
            case 5:
                session.Connect(ip, zoneServerPort + 5);
                break;
            case 6:
                session.Connect(ip, zoneServerPort + 6);
                break;
            case 7:
                session.Connect(ip, zoneServerPort + 7);
                break;
            case 8:
                session.Connect(ip, zoneServerPort + 8);
                break;
            case 9:
                session.Connect(ip, zoneServerPort + 9);
                break;
        }
    }

    public void ZoneChange(int _num)
    {
        if (myZone == _num)
        {

        }
        else
        {
            SessionInfoPacket updateInfoPacket = new SessionInfoPacket();
            updateInfoPacket.SetCmd(SendCommand.C2Zone_UPDATE_INFO);
            updateInfoPacket.info.userInfo = PlayerManager.instance.userInfo;
            updateInfoPacket.info.unitInfo = PlayerManager.instance.unitInfo;

            SendData_ZoneServer(updateInfoPacket.GetBytes());

            ChangeZonePacket changeZonePacket = new ChangeZonePacket();
            changeZonePacket.SetCmd(SendCommand.C2Login_CHANGE_ZONE);
            changeZonePacket.zoneNum = _num;

            SendData_LogInServer(changeZonePacket.GetBytes());
        }
    }

    public void zoneSessionDisConnect()
    {
        session.ShutdownSocket();
        session.mySocket.Close();
    }

    public void logInServerConnect()
    {
        logInSession.Connect(ip, logInServerPort);
    }

    public void SendData_ZoneServer(byte[] _buffer)
    {
        if (!session.isConnect)
        {
            Disconnect();
            return;
        }

        session.SendData(_buffer);
    }

    public void SendData_LogInServer(byte[] _buffer)
    {
        if (!logInSession.isConnect)
        {
            Disconnect();
            return;
        }

        logInSession.SendData(_buffer);
    }

    public void SendData_ZoneServer(byte[] _buffer, int _size)
    {
        if (!session.isConnect)
        {
            Disconnect();
            return;
        }

        session.SendData(_buffer, _size);
    }

    public void SendData_LogInServer(byte[] _buffer, int _size)
    {
        if (!logInSession.isConnect)
        {
            Disconnect();
            return;
        }

        logInSession.SendData(_buffer, _size);
    }

    public void Disconnect()
    {
        Application.Quit();
    }

    public bool GetIsConnect()
    {
        return session.isConnect;
    }

    public void LogIn()
    {
        isLogInConnect = true;

        logInServerConnect();
    }

    public void Register()
    {
        isRegisterConnect = true;

        logInServerConnect();
    }

    public void HeartBeat()
    {
        //Debug.Log("나 살아있어요.");

        Packet CheckAlivePacket = new Packet(SendCommand.C2Zone_CHECK_ALIVE);

        SendData_LogInServer(CheckAlivePacket.GetBytes());
    }

    public void SendExitUser()
    {
        SessionInfoPacket updateInfoPacket = new SessionInfoPacket();
        updateInfoPacket.SetCmd(SendCommand.C2Zone_EXIT_USER);
        updateInfoPacket.info.userInfo = PlayerManager.instance.userInfo;
        updateInfoPacket.info.unitInfo = PlayerManager.instance.unitInfo;

        SendData_ZoneServer(updateInfoPacket.GetBytes());

        session.mySocket.Disconnect(false);

        GameManager.instance.isNormalExit = true;

        Debug.Log("업데이트할 info 전송 - 정상 종료");

        Disconnect();
    }

    public void GetUserList(UserListPacket _packet)
    {
        for(int i = 0; i < _packet.userNum; i++)
        {
            UnitInfo unitInfo = new UnitInfo();
            unitInfo.position = _packet.info[i].position;
            UserInfo userInfo = new UserInfo();
            userInfo = _packet.info[i].userInfo;

            if(userInfo.userID == session.userID)
            {
                PlayerManager.instance.Spawn();

                continue;
            }

            if (mapManager == null) return;

            mapManager.OtherPlayerSpawn(unitInfo, userInfo);

            mapManager.otherPlayersDic[userInfo.userID].isVisible = false;
        }
    }

    public void GetUserListInRange(UserListPacket_Light _packet)
    {
        for (int i = 0; i < _packet.userNum; i++)
        {
            if (_packet.info[i].userID == session.userID) continue;

            if (mapManager == null) return;

            if (!mapManager.otherPlayersDic.ContainsKey(_packet.info[i].userID)) continue;

            mapManager.otherPlayersDic[_packet.info[i].userID].isVisible = true;
        }
    }

    public void GetUserListInvisible(UserListPacket_Light _packet)
    {
        for (int i = 0; i < _packet.userNum; i++)
        {
            if (_packet.info[i].userID == session.userID) continue;

            if (mapManager == null) return;

            if (!mapManager.otherPlayersDic.ContainsKey(_packet.info[i].userID)) continue;

            mapManager.otherPlayersDic[_packet.info[i].userID].isVisible = false;
            mapManager.EndMoveOtherPlayer(_packet.info[i].userID, 
                _packet.info[i].position.position, _packet.info[i].position.direction);
        }
    }

    public void GetUserListVisible(UserListPacket_Light _packet)
    {
        for (int i = 0; i < _packet.userNum; i++)
        {
            if (_packet.info[i].userID == session.userID) continue;

            if (mapManager == null) return;

            if (!mapManager.otherPlayersDic.ContainsKey(_packet.info[i].userID)) continue;

            mapManager.otherPlayersDic[_packet.info[i].userID].isVisible = true;
            Vector3 tempPosi = new Vector3(
                        _packet.info[i].position.position.x, 3.0f,
                        _packet.info[i].position.position.z);

            mapManager.otherPlayersDic[_packet.info[i].userID].transform.position = tempPosi;
        }
    }

    public void EnterUser(SessionInfoPacket _packet)
    {
        Debug.Log("EnterUser");

        int userID = _packet.info.userInfo.userID;

        if (userID == session.userID) return;

        UnitInfo unitInfo = new UnitInfo();
        unitInfo = _packet.info.unitInfo;
        UserInfo userInfo = new UserInfo();
        userInfo = _packet.info.userInfo;

        if (mapManager == null) return;

        mapManager.OtherPlayerSpawn(unitInfo, userInfo);
    }

    public void ExitUser(SessionInfoPacket _packet)
    {
        int userID = _packet.info.userInfo.userID;

        if (userID == session.userID) return;

        if (mapManager == null) return;

        mapManager.RemoveOtherPlayer(userID);
    }

    public void StartUserMove(UserPositionPacket _packet)
    {
        int userID = _packet.userIndex;

        if (userID == session.userID) return;

        if (mapManager == null) return;
        mapManager.BeginMoveOtherPlayer(userID, _packet.position.position, _packet.position.direction);
    }

    public void EndUserMove(UserPositionPacket _packet)
    {
        int userID = _packet.userIndex;

        if (userID == session.userID) return;

        if (mapManager == null) return;
        mapManager.EndMoveOtherPlayer(userID, _packet.position.position, _packet.position.direction);
    }

    public void UserAttackFailed(UserAttackPacket _packet)
    {
        if (mapManager == null) return;

        if (_packet.userIndex != PlayerManager.instance.userInfo.userID)
        {
            mapManager.UserAttackFailed(_packet.userIndex, _packet.position);
        }
    }

    public void UserAttackMonster(UserAttackPacket _packet)
    {
        if (mapManager == null) return;

        if (_packet.userIndex != PlayerManager.instance.userInfo.userID)
        {
            mapManager.UserAttackMonster(_packet.userIndex, _packet.monsterIndex, _packet.position);
        }
    }

    public void UserHit(UserHitPacket _packet)
    {
        if (mapManager == null) return;

        mapManager.UserHit(_packet.userIndex, _packet.monsterIndex, _packet.damage);
    }

    public void UserDeath(UserNumPacket _packet)
    {
        if (mapManager == null) return;

        if(_packet.userIndex == PlayerManager.instance.userInfo.userID)
        {
            //UI하나 띄워주고
            PlayerManager.instance.Death();
            return;
        }

        mapManager.UserDeath(_packet.userIndex);
    }

    public void UserRespawn(SessionInfoPacket _packet)
    {
        if (mapManager == null) return;

        if (_packet.info.userInfo.userID == PlayerManager.instance.userInfo.userID)
        {
            PlayerManager.instance.Respawn(_packet.info.unitInfo);
            return;
        }

        mapManager.OtherPlayerRelease(_packet.info.userInfo.userID,
            _packet.info.unitInfo, _packet.info.userInfo);
    }

    public void UserLevelUp(UserNumPacket _packet)
    {
        if (mapManager == null) return;

        if(_packet.userIndex == PlayerManager.instance.userInfo.userID)
        {
            PlayerManager.instance.LevelUp();
        }

        mapManager.UserLevelUp(_packet.userIndex);
    }

    public void GetMonsterList(MonsterInfoListPacket _packet)
    {
        if (mapManager == null) return;

        for(int i = 0; i < _packet.monsterNum; i++)
        {
            mapManager.MonsterRelease(_packet.info[i]);
        }
    }

    public void GetMonsterListInRange(MonsterInfoListPacket _packet)
    {
        if (mapManager == null) return;

        for (int i = 0; i < _packet.monsterNum; i++)
        {
            mapManager.monsterDic[_packet.info[i].index].isVisible = true;

            Vector3 tempPosi = new Vector3(
                        _packet.info[i].position.x, 0.5f,
                        _packet.info[i].position.z);

            mapManager.monsterDic[_packet.info[i].index].transform.position = tempPosi;
        }
    }

    public void GetMonsterListInvisible(MonsterInfoListPacket _packet)
    {
        if (mapManager == null) return;

        for (int i = 0; i < _packet.monsterNum; i++)
        {
            if (!mapManager.monsterDic.ContainsKey(_packet.info[i].index)) continue;

            mapManager.monsterDic[_packet.info[i].index].isVisible = false;
        }
    }

    public void GetMonsterListVisible(MonsterInfoListPacket _packet)
    {
        if (mapManager == null) return;

        for (int i = 0; i < _packet.monsterNum; i++)
        {
            if (!mapManager.monsterDic.ContainsKey(_packet.info[i].index)) continue;

            Vector3 tempPosi = new Vector3(
                        _packet.info[i].position.x, 0.5f,
                        _packet.info[i].position.z);

            mapManager.monsterDic[_packet.info[i].index].transform.position = tempPosi;

            mapManager.monsterDic[_packet.info[i].index].isVisible = true;
        }
    }

    public void GetMonster(MonsterInfoPacket _packet)
    {
        if (mapManager == null) return;

        mapManager.MonsterRelease(_packet.info);
    }

    public void MonsterMove(MonsterPositionPacket _packet)
    {
        mapManager.MonsterMove(_packet.monsterIndex, _packet.position);
    }

    public void MonsterHit(MonsterHitPacket _packet)
    {
        if (mapManager == null) return;

        mapManager.MonsterHit(_packet);
    }

    public void Chatting(ChattingPacket _chattingPacket)
    {
        int userIndex = _chattingPacket.userIndex;
        string id = _chattingPacket.id;
        string chatting = Encoding.Default.GetString(_chattingPacket.chatting);

        GameManager.Instance.chattingUI.
            ChattingInput(userIndex, id, chatting);

        mapManager.Chatting(userIndex, chatting);
    }

    public void Chatting_Whisper(ChattingPacket_Whisper _chattingPacket_Whisper)
    {
        int userIndex = _chattingPacket_Whisper.userIndex;
        string targetId = _chattingPacket_Whisper.targetId;
        string id = _chattingPacket_Whisper.id;
        string chatting = Encoding.Default.GetString(_chattingPacket_Whisper.chatting);

        GameManager.Instance.chattingUI.
            ChattingInput_Whisper(userIndex, targetId, id, chatting);
    }

    public void RemoveAll()
    {
        foreach(var pair in mapManager.otherPlayersDic)
        {

        }
    }
}
