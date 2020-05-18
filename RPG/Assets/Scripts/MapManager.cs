using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MapManager : MonoBehaviour
{
    //public string tileMap;

    public GameManager gameManagerPrefabs;
    public UIManager uiManagerPrefabs;

    public Dictionary<int, OtherPlayer> otherPlayersDic;
    public Dictionary<int, Monster> monsterDic;

    public List<OtherPlayer> visibleUserList;
    public List<Monster> visibleMonsterList;

    // Start is called before the first frame update
    void Start()
    {
        //https://codingmania.tistory.com/166
        if (GameManager.Instance == null)
        {
            GameManager.Instantiate(gameManagerPrefabs);
        }

        if (UIManager.Instance == null)
        {
            UIManager.Instantiate(uiManagerPrefabs);
        }

        GameManager.Instance.mapManager = this;
        ServerManager.Instance.mapManager = this;

        otherPlayersDic = new Dictionary<int, OtherPlayer>();
        monsterDic = new Dictionary<int, Monster>();

        visibleUserList = new List<OtherPlayer>();
        visibleMonsterList = new List<Monster>();

        Packet packet = new Packet(SendCommand.C2Zone_ENTER_FIELD_SUCCESS);
        ServerManager.Instance.SendData(packet.GetBytes());
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void OtherPlayerSpawn(UnitInfo _unitInfo, UserInfo _userInfo)
    {
        //Debug.Log("OtherPlayerSpawn");

        if (otherPlayersDic.ContainsKey(_userInfo.userID))
        {
            return;
        }

        OtherPlayer player = ObjectPoolManager.Instance.otherPlayers.GetObject();
        player.Init(_unitInfo, _userInfo, PlayerManager.instance.transform);
        otherPlayersDic.Add(player.userInfo.userID, player);
    }

    public void OtherPlayerRelease(int _userIndex, UnitInfo _unitInfo, UserInfo _userInfo)
    {
        if (!otherPlayersDic.ContainsKey(_userIndex)) return;

        otherPlayersDic[_userIndex].Release(_unitInfo, _userInfo);
    }

    public void RemoveOtherPlayer(int _userIndex)
    {
        if (!otherPlayersDic.ContainsKey(_userIndex)) return;

        OtherPlayer player = otherPlayersDic[_userIndex];
        ObjectPoolManager.Instance.otherPlayers.ReturnObject(player);
        otherPlayersDic.Remove(_userIndex);
    }

    public void AddVisiblePlayer(OtherPlayer _player)
    {
        if (!visibleUserList.Contains(_player)) visibleUserList.Add(_player);
    }

    public void RemoveVisiblePlayer(OtherPlayer _player)
    {
        if (visibleUserList.Contains(_player)) visibleUserList.Remove(_player);
    }

    public void AddVisibleMonster(Monster _monster)
    {
        if (!visibleMonsterList.Contains(_monster)) visibleMonsterList.Add(_monster);
    }

    public void RemoveVisibleMonster(Monster _monster)
    {
        if (visibleMonsterList.Contains(_monster)) visibleMonsterList.Remove(_monster);
    }

    public void BeginMoveOtherPlayer(int _userIndex, Vector _position, Vector _direction)
    {
        if (!otherPlayersDic.ContainsKey(_userIndex)) return;
        otherPlayersDic[_userIndex].BeginMove(_position, _direction);
    }

    public void EndMoveOtherPlayer(int _userIndex, Vector _position, Vector _direction)
    {
        if (!otherPlayersDic.ContainsKey(_userIndex)) return;
        otherPlayersDic[_userIndex].EndMove(_position, _direction);
    }

    public void UserAttackFailed(int _userIndex, Vector _direction)
    {
        if (!otherPlayersDic.ContainsKey(_userIndex)) return;

        otherPlayersDic[_userIndex].Attack(_direction);
    }

    public void UserAttackMonster(int _userIndex, int _monsterIndex, Vector _direction)
    {
        if (!otherPlayersDic.ContainsKey(_userIndex)) return;
        if (!monsterDic.ContainsKey(_monsterIndex)) return;

        otherPlayersDic[_userIndex].Attack(_direction);
        otherPlayersDic[_userIndex].TargetLookAt(monsterDic[_monsterIndex].transform.position);
    }

    public void UserHit(int _userIndex, int _monsterIndex, int _damage)
    {
        Monster monster = monsterDic[_monsterIndex];

        if(_userIndex == PlayerManager.instance.userInfo.userID)
        {
            monster.Attack(PlayerManager.instance.transform.position);
            PlayerManager.instance.Hit(_damage);

            return;
        }

        OtherPlayer otherPlayer = otherPlayersDic[_userIndex];
        monster.Attack(otherPlayer.transform.position);
        otherPlayer.Hit(_damage);
    }

    public void UserDeath(int _userIndex)
    {
        otherPlayersDic[_userIndex].Death();
    }

    public void UserLevelUp(int _userIndex)
    {
        //otherPlayersDic[_userIndex].LevelUp();
    }

    public void MonsterSpawn(MonsterInfo _info)
    {
        Monster monster = ObjectPoolManager.Instance.monsters[_info.monsterType].GetObject();
        monster.Init(_info, PlayerManager.instance.transform);

        monsterDic.Add(monster.monsterInfo.index, monster);
    }

    public void MonsterRelease(MonsterInfo _info)
    {
        if(monsterDic.ContainsKey(_info.index))
        {
            monsterDic[_info.index].Release(_info);
        }
        else
        {
            MonsterSpawn(_info);
        }
    }

    public void MonsterMove(int _index, Vector _position)
    {
        if (!monsterDic.ContainsKey(_index)) return;

        monsterDic[_index].Move(_position);
    }

    public void MonsterHit(MonsterHitPacket _packet)
    {
        Monster monster = monsterDic[_packet.monsterIndex];

        monster.Hit(_packet.damage, _packet.userIndex);
    }

    //http://teamcreator.80port.net/webzine/2002_01/p_lecture_01_bh_col.htm

    //https://m.blog.naver.com/PostView.nhn?blogId=gooldare&logNo=221479323579&categoryNo=36&proxyReferer=https%3A%2F%2Fwww.google.com%2F
    public List<Monster> CheckMonsterInRange(float _angle, float _range)
    {
        List<Monster> monsterList = new List<Monster>();

        for (int i = visibleMonsterList.Count - 1; i >= 0; i--)
        {
            Vector3 monsterPos = visibleMonsterList[i].transform.position;

            Vector3 direction = new Vector3(monsterPos.x, 0.0f, monsterPos.z)
                - new Vector3(PlayerManager.instance.transform.position.x, 0.0f, PlayerManager.instance.transform.position.z);

            if (direction.sqrMagnitude > _range * _range) continue;

            Vector3 forwardVector = PlayerManager.instance.transform.forward;
            forwardVector.y = 0.0f;

            if (Vector3.Dot(direction.normalized, forwardVector) >= 
                Mathf.Cos(Mathf.Deg2Rad * (_angle * 0.5f)))
            {
                monsterList.Add(visibleMonsterList[i]);
            }
        }

        return monsterList;
    }

    public void Chatting(int _userIndex, string _chatting)
    {
        if (_userIndex == PlayerManager.instance.userInfo.userID)
        {
            PlayerManager.instance.Chatting(_chatting);

            return;
        }

        if(_userIndex >= 10000)
        {
            return;
        }

        OtherPlayer player = otherPlayersDic[_userIndex];
        player.Chatting(_chatting);
    }

    public void Clear()
    {
        foreach (var val in monsterDic)
        {
            Monster monster = val.Value;
            ObjectPoolManager.Instance.monsters[monster.monsterInfo.monsterType].ReturnObject(monster);
        }

        foreach (var val in otherPlayersDic)
        {
            OtherPlayer player = val.Value;
            ObjectPoolManager.Instance.otherPlayers.ReturnObject(player);
        }

        monsterDic.Clear();
        otherPlayersDic.Clear();

        visibleMonsterList.Clear();
        visibleUserList.Clear();
    }
}
