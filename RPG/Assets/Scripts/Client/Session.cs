using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

using System.Net.Sockets;
using System.Net;
using System.Threading;

using System;
using System.Text;
using System.Runtime.InteropServices;

//C# 비동기 클라이언트 소켓 예제
//https://windowshyun.tistory.com/16

public class Session : MonoBehaviour
{
    public int userID;

    public Socket mySocket;

    private byte[] recvBuffer = new byte[65535];
    private byte[] packetBuffer = new byte[65535];

    private Packet packet;
    private int readPoint;

    public bool isConnect;

    //http://www.csharpstudy.com/Threads/lock.aspx
    private object lockObject = new object();

    //ManualResetEvent 클래스
    //http://www.csharpstudy.com/Threads/manualresetevent.aspx

    //C# _ 멀티스레드,ManualResetEvent,동기화 를 시켜보자
    //https://itnhappy.tistory.com/24

    //https://kuimoani.tistory.com/entry/C-%EC%8A%A4%EB%A0%88%EB%93%9C-%EB%8F%99%EA%B8%B0%ED%99%94%EB%A5%BC-%EC%9C%84%ED%95%9C-AutoResetEvent%EC%99%80-ManualResetEvent

    private static ManualResetEvent manualEvent = new ManualResetEvent(false);

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    public void Update()
    {
        int tempNum = 20;

        lock(lockObject)
        {
            while (true)
            {
                if (readPoint >= 2)
                {
                    //https://docs.microsoft.com/ko-kr/dotnet/csharp/programming-guide/unsafe-code-pointers/fixed-size-buffers
                    /*unsafe
                    {
                        fixed (byte* fixedBytes = packetBuffer)
                        {
                            Marshal.PtrToStructure((IntPtr)fixedBytes, tempPacket);

                            Debug.Log(tempPacket);
                        }
                    }*/

                    Parsing(packetBuffer, ref packet);

                    //https://stackoverflow.com/questions/31637497/how-to-handle-socket-exception
                    if (packet.GetSize() <= readPoint)
                    {
                        try
                        {
                            ExecuteCommand(packet, packetBuffer);

                            Buffer.BlockCopy(packetBuffer, packet.GetSize(),
                                packetBuffer, 0,
                                readPoint - packet.GetSize());

                            readPoint -= packet.GetSize();

                            //Debug.Log(readPoint);
                        }
                        catch (SocketException socketException)
                        {
                            Debug.Log("SocketException Error : " + socketException.ToString());
                        }
                    }
                }

                tempNum--;

                if (tempNum <= 0)
                {
                    return;
                }
            }
        }
    }

    public void Init()
    {
        mySocket = new Socket(
            AddressFamily.InterNetwork,
            SocketType.Stream,
            ProtocolType.Tcp);

        isConnect = false;

        userID = 0;
    }

    public bool Connect(string _ip, int _port)
    {
        try
        {
            mySocket.BeginConnect(_ip, _port, new AsyncCallback(ConnectCallback), mySocket);

            //http://www.devpia.com/MAEUL/Contents/Detail.aspx?BoardID=17&MAEULNo=8&no=164425&ref=164425
            manualEvent.WaitOne();

            Recv();
        }
        catch(Exception e)
        {
            Console.WriteLine(e.ToString());
        }

        return true;
    }

    protected static void ConnectCallback(IAsyncResult ar)
    {
        Socket socket = (Socket)ar.AsyncState;

        try
        {
            socket.EndConnect(ar);
            manualEvent.Set();
        }
        catch (Exception e)
        {
            socket.Close();
            manualEvent.Set();

            Console.WriteLine(e.ToString());
        }
    }

    public void Recv()
    {
        packet = new Packet();

        mySocket.BeginReceive(recvBuffer, 0,
            recvBuffer.Length, SocketFlags.None,
            new AsyncCallback(RecvCallback), mySocket);

        readPoint = 0;
    }

    private void RecvCallback(IAsyncResult ar)
    {
        try
        {
            int recvSize = mySocket.EndReceive(ar);

            lock (lockObject)
            {
                Buffer.BlockCopy(recvBuffer, 0, packetBuffer, readPoint, recvSize);
                readPoint += recvSize;
            }

            mySocket.BeginReceive(recvBuffer, 0,
                recvBuffer.Length, SocketFlags.None,
                new AsyncCallback(RecvCallback), mySocket);
        }
        catch (Exception e)
        {
            Console.WriteLine(e.ToString());

            isConnect = false;
        }   
    }

    public void SendData(byte[] _buffer)
    {
        mySocket.Send(_buffer);
    }

    public void SendData(byte[] _buffer, int _size)
    {
        mySocket.Send(_buffer, _size, 0);
    }

    public void Parsing<T>(byte[] _buffer, ref T packet)
    {
        unsafe
        {
            fixed (byte* fixedBytes = _buffer)
            {
                Marshal.PtrToStructure((IntPtr)fixedBytes, packet);
            }
        }
    }

    public void ExecuteCommand(Packet _packet, byte[] _buffer)
    {
        switch ((RecvCommand)_packet.GetCmd())
        {
            case RecvCommand.Zone2C_CHATTING:
                {
                    ChattingPacket chattingPacket = new ChattingPacket();
                    Parsing(_buffer, ref chattingPacket);

                    ServerManager.Instance.Chatting(chattingPacket);
                }
                break;
            case RecvCommand.Zone2C_CHECK_ALIVE:
                {
                    Debug.Log("나 살아있어요.");

                    Packet CheckAlivePacket = new Packet(SendCommand.C2Zone_CHECK_ALIVE);

                    SendData(CheckAlivePacket.GetBytes());
                }
                break;
            case RecvCommand.Zone2C_ISCONNECTED:
                {
                    IsConnectedPacket isConnectedPacket = new IsConnectedPacket();
                    Parsing(_buffer, ref isConnectedPacket);

                    Debug.Log("IsConnected 수신 완료");

                    if (isConnectedPacket.isConnected)
                    {
                        if(ServerManager.Instance.isLogInConnect)
                        {
                            ServerManager.Instance.isLogInConnect = false;

                            LogInPacket logInPacket = new LogInPacket();
                            logInPacket.SetCmd(SendCommand.C2Zone_LOGIN);
                            logInPacket.id = GameManager.Instance.logInId;
                            logInPacket.password = GameManager.Instance.logInPassword;

                            SendData(logInPacket.GetBytes());
                        }
                        else if (ServerManager.Instance.isRegisterConnect)
                        {
                            ServerManager.Instance.isRegisterConnect = false;

                            RegisterUserPacket registerUserPacket = new RegisterUserPacket();
                            registerUserPacket.SetCmd(SendCommand.C2Zone_REGISTER_ACCOUNT);
                            registerUserPacket.id = GameManager.Instance.registerId;
                            registerUserPacket.password = GameManager.Instance.registerPassword;

                            SendData(registerUserPacket.GetBytes());
                        }
                    }
                }
                break;
            case RecvCommand.Zone2C_REGISTER_USER_SUCCESS:
                {
                    GameManager.Instance.titleUI.registerSuccessWindow.SetActive(true);

                    Packet exitPacket = new Packet(SendCommand.C2Zone_EXIT_USER);

                    SendData(exitPacket.GetBytes());

                    mySocket.Disconnect(false);
                    this.Init();
                }
                break;
            case RecvCommand.Zone2C_REGISTER_USER_FAILED:
                {
                    GameManager.Instance.titleUI.registerFailedWindow.SetActive(true);

                    Packet exitPacket = new Packet(SendCommand.C2Zone_EXIT_USER);

                    SendData(exitPacket.GetBytes());

                    mySocket.Disconnect(false);
                    this.Init();
                }
                break;
            case RecvCommand.Zone2C_LOGIN_SUCCESS:
                {
                    Packet requireInfoPacket = new Packet(SendCommand.C2Zone_REQUIRE_INFO);

                    SendData(requireInfoPacket.GetBytes());
                }
                break;
            case RecvCommand.Zone2C_LOGIN_FAILED:
                {
                    GameManager.Instance.titleUI.loginFailedWindow.SetActive(true);

                    Packet exitPacket = new Packet(SendCommand.C2Zone_EXIT_USER);

                    SendData(exitPacket.GetBytes());

                    mySocket.Disconnect(false);
                    this.Init();
                }
                break;
            case RecvCommand.Zone2C_LOGIN_FAILED_DUPLICATED:
                {
                    GameManager.Instance.titleUI.loginDuplicatedWindow.SetActive(true);

                    Packet exitPacket = new Packet(SendCommand.C2Zone_EXIT_USER);

                    SendData(exitPacket.GetBytes());

                    mySocket.Disconnect(false);
                    this.Init();
                }
                break;
            case RecvCommand.Zone2C_REGISTER_USER:
                {
                    SessionInfoPacket sessionInfoPacket = new SessionInfoPacket();
                    Parsing(_buffer, ref sessionInfoPacket);

                    userID = sessionInfoPacket.info.userInfo.userID;
                    Debug.Log(userID);

                    PlayerManager.instance.SetUserInfo(sessionInfoPacket.info.userInfo);
                    PlayerManager.instance.SetUnitInfo(sessionInfoPacket.info.unitInfo);

                    FieldNumPacket fieldNumPacket = new FieldNumPacket(SendCommand.C2Zone_TRY_ENTER_FIELD, 1);
                    fieldNumPacket.fieldNum = 999;

                    SendData(fieldNumPacket.GetBytes());
                }              
                break;
            case RecvCommand.Zone2C_ENTER_FIELD:
                {
                    Debug.Log("Enter Field");

                    EnterFieldPacket enterFieldPacket = new EnterFieldPacket();
                    Parsing(_buffer, ref enterFieldPacket);

                    if(PlayerManager.instance.unitInfo.zoneNum != 0)
                    {
                        if(ServerManager.Instance.mapManager != null)

                        ServerManager.Instance.mapManager.Clear();
                    }

                    Debug.Log(enterFieldPacket.position);

                    PlayerManager.instance.unitInfo.zoneNum = enterFieldPacket.fieldNum;
                    PlayerManager.instance.unitInfo.position.position = enterFieldPacket.position;

                    Debug.Log(PlayerManager.instance.unitInfo.position.position);

                    switch (PlayerManager.instance.unitInfo.zoneNum)
                    {
                        case 1:
                            SceneManager.LoadScene("Village");
                            break;
                        case 2:
                            SceneManager.LoadScene("Field");
                            break;
                        case 999:
                            SceneManager.LoadScene("TestScene");
                            break;
                    }

                    GameManager.Instance.isInGame = true;

                    PlayerManager.instance.Init();

                    isConnect = true;
                }
                break;
            case RecvCommand.Zone2C_USER_LIST:
                {
                    Debug.Log("Get User List");

                    UserListPacket userListPacket = new UserListPacket();
                    Parsing(_buffer, ref userListPacket);

                    ServerManager.Instance.GetUserList(userListPacket);
                }
                break;
            case RecvCommand.Zone2C_USER_LIST_IN_RANGE:
                {
                    Debug.Log("Get User List In Range");

                    UserListPacket_Light userListPacket = new UserListPacket_Light();
                    Parsing(_buffer, ref userListPacket);

                    ServerManager.Instance.GetUserListInRange(userListPacket);
                }
                break;
            case RecvCommand.Zone2C_ENTER_USER_INFO:
                {
                    SessionInfoPacket sessionInfoPacket = new SessionInfoPacket();
                    Parsing(_buffer, ref sessionInfoPacket);

                    ServerManager.Instance.EnterUser(sessionInfoPacket);
                }
                break;
            case RecvCommand.Zone2C_ENTER_USER_INFO_IN_RANGE:
                {
                    UserNumPacket userNumPacket = new UserNumPacket();
                    Parsing(_buffer, ref userNumPacket);

                    ServerManager.Instance.mapManager.
                        otherPlayersDic[userNumPacket.userIndex].isVisible = true;
                }
                break;
            case RecvCommand.Zone2C_LEAVE_USER_INFO:
                {
                    SessionInfoPacket sessionInfoPacket = new SessionInfoPacket();
                    Parsing(_buffer, ref sessionInfoPacket);

                    ServerManager.Instance.ExitUser(sessionInfoPacket);
                }
                break;
            case RecvCommand.Zone2C_USER_MOVE:
                {
                    UserPositionPacket userPositionPacket = new UserPositionPacket();
                    Parsing(_buffer, ref userPositionPacket);

                    Debug.Log("MOVE");

                    ServerManager.Instance.StartUserMove(userPositionPacket);
                }
                break;
            case RecvCommand.Zone2C_USER_MOVE_FINISH:
                {
                    UserPositionPacket userPositionPacket = new UserPositionPacket();
                    Parsing(_buffer, ref userPositionPacket);

                    Debug.Log("FINISH");

                    ServerManager.Instance.EndUserMove(userPositionPacket);
                }                
                break;
            case RecvCommand.Zone2C_USER_LIST_INVISIBLE:
                {
                    UserListPacket_Light userListPacket = new UserListPacket_Light();
                    Parsing(_buffer, ref userListPacket);

                    Debug.Log("Invisible List : " + userListPacket.userNum);

                    ServerManager.Instance.GetUserListInvisible(userListPacket);
                }
                break;
            case RecvCommand.Zone2C_LEAVE_SECTOR_USER_INFO:
                {
                    UserNumPacket userNumPacket = new UserNumPacket();
                    Parsing(_buffer, ref userNumPacket);

                    if (userNumPacket.userIndex == PlayerManager.instance.userInfo.userID) break;

                    Debug.Log("Leave Sector UserNum : " + userNumPacket.userIndex);

                    ServerManager.Instance.mapManager.
                        otherPlayersDic[userNumPacket.userIndex].isVisible = false;
                }
                break;
            case RecvCommand.Zone2C_USER_LIST_VISIBLE:
                {
                    UserListPacket_Light userListPacket = new UserListPacket_Light();
                    Parsing(_buffer, ref userListPacket);

                    Debug.Log("Visible List : " + userListPacket.userNum);

                    ServerManager.Instance.GetUserListVisible(userListPacket);
                }
                break;
            case RecvCommand.Zone2C_ENTER_SECTOR_USER_INFO:
                {
                    UserPositionPacket userPositionPacket = new UserPositionPacket();
                    Parsing(_buffer, ref userPositionPacket);

                    if (userPositionPacket.userIndex == PlayerManager.instance.userInfo.userID) break;

                    ServerManager.Instance.mapManager.
                        otherPlayersDic[userPositionPacket.userIndex].isVisible = true;

                    Vector3 tempPosi = new Vector3(
                        userPositionPacket.position.position.x, 0.5f, userPositionPacket.position.position.z);
                    ServerManager.Instance.mapManager.
                        otherPlayersDic[userPositionPacket.userIndex].transform.position = tempPosi;
                }
                break;
            case RecvCommand.Zone2C_USER_ATTACK_FAILED:
                {
                    UserAttackPacket userAttackPacket = new UserAttackPacket();
                    Parsing(_buffer, ref userAttackPacket);

                    ServerManager.Instance.UserAttackFailed(userAttackPacket);
                }
                break;
            case RecvCommand.Zone2C_USER_ATTACK_MONSTER:
                {
                    UserAttackPacket userAttackPacket = new UserAttackPacket();
                    Parsing(_buffer, ref userAttackPacket);

                    ServerManager.Instance.UserAttackMonster(userAttackPacket);
                }
                break;
            case RecvCommand.Zone2C_USER_HIT:
                {
                    UserHitPacket userHitPacket = new UserHitPacket();
                    Parsing(_buffer, ref userHitPacket);

                    ServerManager.Instance.UserHit(userHitPacket);
                }
                break;
            case RecvCommand.Zone2C_USER_PLUS_EXP:
                {
                    UserExpPacket userExpPacket = new UserExpPacket();
                    Parsing(_buffer, ref userExpPacket);

                    PlayerManager.instance.PlusExp(userExpPacket.exp);
                }
                break;
            case RecvCommand.Zone2C_USER_LEVEL_UP:
                {
                    UserNumPacket userNumPacket = new UserNumPacket();
                    Parsing(_buffer, ref userNumPacket);

                    ServerManager.Instance.UserLevelUp(userNumPacket);
                }
                break;
            case RecvCommand.Zone2C_USER_DEATH:
                {
                    UserNumPacket userNumPacket = new UserNumPacket();
                    Parsing(_buffer, ref userNumPacket);

                    ServerManager.Instance.UserDeath(userNumPacket);
                }
                break;
            case RecvCommand.Zone2C_USER_REVIVE:
                {
                    SessionInfoPacket sessionInfoPacket = new SessionInfoPacket();
                    Parsing(_buffer, ref sessionInfoPacket);

                    ServerManager.Instance.UserRespawn(sessionInfoPacket);
                }
                break;
            case RecvCommand.Zone2C_MONSTER_INFO:
                {
                    MonsterInfoPacket monsterInfoPacket = new MonsterInfoPacket();
                    Parsing(_buffer, ref monsterInfoPacket);

                    ServerManager.Instance.GetMonster(monsterInfoPacket);
                }
                break;
            case RecvCommand.Zone2C_MONSTER_INFO_LIST:
                {
                    Debug.Log("Get Monster List");

                    MonsterInfoListPacket monsterListPacket = new MonsterInfoListPacket();
                    Parsing(_buffer, ref monsterListPacket);

                    ServerManager.Instance.GetMonsterList(monsterListPacket);
                }
                break;
            case RecvCommand.Zone2C_MONSTER_INFO_LIST_IN_RANGE:
                {
                    Debug.Log("Get Monster List In Range");

                    MonsterInfoListPacket monsterListPacket = new MonsterInfoListPacket();
                    Parsing(_buffer, ref monsterListPacket);

                    ServerManager.Instance.GetMonsterListInRange(monsterListPacket);
                }
                break;
            case RecvCommand.Zone2C_MONSTER_INFO_LIST_INVISIBLE:
                {
                    MonsterInfoListPacket monsterListPacket = new MonsterInfoListPacket();
                    Parsing(_buffer, ref monsterListPacket);

                    Debug.Log("Invisible List : " + monsterListPacket.monsterNum);

                    ServerManager.Instance.GetMonsterListInvisible(monsterListPacket);
                }
                break;
            case RecvCommand.Zone2C_MONSTER_INFO_LIST_VISIBLE:
                {
                    MonsterInfoListPacket monsterListPacket = new MonsterInfoListPacket();
                    Parsing(_buffer, ref monsterListPacket);

                    Debug.Log("Visible List : " + monsterListPacket.monsterNum);

                    ServerManager.Instance.GetMonsterListVisible(monsterListPacket);
                }
                break;
            case RecvCommand.Zone2C_MONSTER_MOVE:
                {
                    MonsterPositionPacket monsterPositionPacket = new MonsterPositionPacket();
                    Parsing(_buffer, ref monsterPositionPacket);

                    ServerManager.Instance.MonsterMove(monsterPositionPacket);
                }
                break;
            case RecvCommand.Zone2C_MONSTER_HIT:
                {
                    MonsterHitPacket monsterHitPacket = new MonsterHitPacket();
                    Parsing(_buffer, ref monsterHitPacket);

                    ServerManager.Instance.MonsterHit(monsterHitPacket);
                }
                break;
            case RecvCommand.Zone2C_MONSTER_HIT_FAIL:
                {
                    MonsterHitPacket monsterHitPacket = new MonsterHitPacket();
                    Parsing(_buffer, ref monsterHitPacket);

                    ServerManager.Instance.MonsterHit(monsterHitPacket);
                }
                break;
            case RecvCommand.Zone2C_UPDATE_INFO:
                {
                    SessionInfoPacket updateInfoPacket = new SessionInfoPacket();
                    updateInfoPacket.SetCmd(SendCommand.C2Zone_UPDATE_INFO);
                    updateInfoPacket.info.userInfo = PlayerManager.instance.userInfo;
                    updateInfoPacket.info.unitInfo = PlayerManager.instance.unitInfo;

                    SendData(updateInfoPacket.GetBytes());

                    Debug.Log("업데이트할 info 전송 - HeartBeat");
                }
                break;
        }
    }
}
