using System.Collections;
using System.Collections.Generic;
using System.Text;
using UnityEngine;

public class ServerManager : Singleton<ServerManager>
{
    bool isConnect;
    public string ip;
    public ushort port;

    private Session session = new Session();

    public MapManager mapManager;

    public bool isLogInConnect;
    public bool isRegisterConnect;

    // Start is called before the first frame update
    void Start()
    {
        isLogInConnect = false;
        isRegisterConnect = false;

        Init();
    }

    // Update is called once per frame
    void Update()
    {
        session.Update();
    }

    public void Init()
    {
        session.Init();
    }

    public void Connect()
    {
        session.Connect(ip, port);
    }

    public void SendData(byte[] _buffer)
    {
        if (!session.isConnect)
        {
            Disconnect();
            return;
        }

        session.SendData(_buffer);
    }

    public void SendData(byte[] _buffer, int _size)
    {
        if (!session.isConnect)
        {
            Disconnect();
            return;
        }
        session.SendData(_buffer, _size);
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

        session.Connect(ip, port);
    }

    public void Register()
    {
        isRegisterConnect = true;

        session.Connect(ip, port);
    }

    public void SendExitUser()
    {
        SessionInfoPacket updateInfoPacket = new SessionInfoPacket();
        updateInfoPacket.SetCmd(SendCommand.C2Zone_EXIT_USER);
        updateInfoPacket.info.userInfo = PlayerManager.instance.userInfo;
        updateInfoPacket.info.unitInfo = PlayerManager.instance.unitInfo;

        SendData(updateInfoPacket.GetBytes());

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
        }
    }

    public void GetUserListInRange(UserListPacket_Light _packet)
    {
        for (int i = 0; i < _packet.userNum; i++)
        {
            if (_packet.info[i].userID == session.userID) continue;

            if (mapManager == null) return;

            mapManager.otherPlayersDic[_packet.info[i].userID].isVisible = true;
        }
    }

    public void GetUserListInvisible(UserListPacket_Light _packet)
    {
        for (int i = 0; i < _packet.userNum; i++)
        {
            if (_packet.info[i].userID == session.userID) continue;

            if (mapManager == null) return;

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

            mapManager.otherPlayersDic[_packet.info[i].userID].isVisible = true;

            Vector3 tempPosi = new Vector3(
                        _packet.info[i].position.position.x, 0.5f,
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
        }
    }

    public void GetMonsterListInvisible(MonsterInfoListPacket _packet)
    {
        if (mapManager == null) return;

        for (int i = 0; i < _packet.monsterNum; i++)
        {
            mapManager.monsterDic[_packet.info[i].index].isVisible = false;
        }
    }

    public void GetMonsterListVisible(MonsterInfoListPacket _packet)
    {
        if (mapManager == null) return;

        for (int i = 0; i < _packet.monsterNum; i++)
        {
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
}
