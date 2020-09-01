using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.Text;
using System.Runtime.InteropServices;

public enum SendCommand
{
    C2Zone_TRY_ENTER_FIELD = 0, //첫 시작때만 호출
    C2Zone_ENTER_FIELD_SUCCESS = 1,
    C2Zone_ENTER_FIELD = 2,
    C2Zone_ENTER_VILLAGE = 3,
    C2Zone_USER_MOVE = 4,
    C2Zone_USER_MOVE_FINISH = 5,
    C2Zone_USER_ATTACK_FAILED = 6,
    C2Zone_USER_ATTACK_MONSTER = 7,
    C2Zone_USER_REVIVE = 8,
    C2Zone_REGISTER_ACCOUNT = 9,
    C2Zone_LOGIN = 10,
    C2Zone_REQUIRE_INFO = 11,
    C2Zone_UPDATE_INFO = 12,
    C2Zone_EXIT_USER = 13,
    C2Zone_CHATTING = 14,
    C2Zone_CHECK_ALIVE = 15,
    C2Zone_CHATTING_WHISPER = 17,
    C2Login_CHANGE_ZONE = 18,
    C2Zone_ENTER_TESTFIELD = 19,
    C2Login_SENDZONENUM = 20
}

public enum RecvCommand
{
    Zone2C_USER_MOVE = 4,
    Zone2C_USER_MOVE_FINISH = 5,
    Zone2C_USER_ATTACK_FAILED = 6,
    Zone2C_USER_ATTACK_MONSTER = 7,
    LogIn2C_UPDATE_INFO = 12,
    Zone2C_CHATTING = 14,
    Zone2C_CHATTING_WHISPER = 17,
    Zone2C_CHANGE_ZONE = 18,

    Zone2C_CHATTING_WHISPER_FAIL = 500,

    LogIn2C_ISCONNECTED = 1000,
    Zone2C_ISCONNECTED = 1001,

    Zone2C_REGISTER_USER = 1002,

    Zone2C_ENTER_FIELD = 1004,
    Zone2C_USER_LIST = 1005,
    Zone2C_USER_LIST_IN_RANGE = 1006,
    Zone2C_USER_LIST_VISIBLE = 1007,
    Zone2C_USER_LIST_INVISIBLE = 1008,
    Zone2C_ENTER_USER_INFO = 1009,
    Zone2C_ENTER_USER_INFO_IN_RANGE = 1010,
    Zone2C_LEAVE_USER_INFO = 1011,
    Zone2C_USER_HIT = 1012,
    Zone2C_USER_PLUS_EXP = 1013,
    Zone2C_USER_LEVEL_UP = 1014,
    Zone2C_USER_DEATH = 1015,
    Zone2C_USER_REVIVE = 1016,
    Zone2C_LEAVE_SECTOR_USER_INFO = 1017,
    Zone2C_ENTER_SECTOR_USER_INFO = 1018,

    Zone2C_MONSTER_INFO_LIST = 1019,
    Zone2C_MONSTER_INFO_LIST_IN_RANGE = 1020,
    Zone2C_MONSTER_INFO_LIST_VISIBLE = 1021,
    Zone2C_MONSTER_INFO_LIST_INVISIBLE = 1022,
    Zone2C_MONSTER_INFO = 1023,

    Zone2C_MONSTER_MOVE = 1024,
    Zone2C_MONSTER_ATTACK = 1025,
    Zone2C_MONSTER_HIT = 1026,
    Zone2C_MONSTER_HIT_FAIL = 1027,

    Zone2C_CHECK_ALIVE = 1028,

    Zone2C_REGISTER_USER_SUCCESS = 1029,
    Zone2C_REGISTER_USER_FAILED = 1030,

    Zone2C_LOGIN_SUCCESS = 1031,
    Zone2C_LOGIN_FAILED = 1032,
    Zone2C_LOGIN_FAILED_DUPLICATED = 1033
}

public struct BasicInfo
{
    public UnitInfo unitInfo;
    public UserInfo userInfo;
};

public struct Info_PacketUse
{
    public UserInfo userInfo;
    public Position position;
};

public struct Info_PacketUser_Light
{
    public int userID;
    public Position position;
};

[Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class Packet
{
    private ushort size;
    private ushort cmd;

    public Packet()
    {
        size = (ushort)Marshal.SizeOf(this);
    }

    public Packet(SendCommand _cmd)
    {
        size = (ushort)Marshal.SizeOf(this);
        cmd = (ushort)_cmd;
    }

    public void SetCmd(SendCommand _cmd)
    {
        cmd = (ushort)_cmd;
    }

    public ushort GetCmd()
    {
        return cmd;
    }

    public ushort GetSize()
    {
        return size;
    }

    public byte[] GetBytes()
    {
        byte[] tempBuffer = new byte[size];

        unsafe
        {
            fixed (byte* fixedBytes = tempBuffer)
            {
                Marshal.StructureToPtr(this, (IntPtr)fixedBytes, false);
            }
        }

        return tempBuffer;
    }
}

[Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class IsConnectedPacket : Packet
{
    public bool isConnected;
};

[Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class SessionInfoPacket : Packet
{
    public BasicInfo info;
}

[Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class EnterFieldPacket : Packet
{
    public ushort fieldNum;
    public Vector position;
};

[Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class ZoneNumPacket : Packet
{
    public int zoneNum;
};

[Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class UserNumPacket : Packet
{
    public int userIndex;
};

[Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class SectorPacket : Packet
{
    public int userIndex;
    public bool isEnter;
};

[Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class FieldNumPacket : Packet
{
    public ushort fieldNum;

    public FieldNumPacket() { }

    public FieldNumPacket(SendCommand _cmd, ushort _num)
    {
        this.SetCmd(_cmd);
        fieldNum = _num;
    }

    public ushort GetZoneNum()
    {
        return fieldNum;
    }
}

[Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class UserListPacket : Packet
{
    public ushort userNum;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 500)]
    public Info_PacketUse[] info;
};

[Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class UserListPacket_Light : Packet
{
    public ushort userNum;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 500)]
    public Info_PacketUser_Light[] info;
};

[Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class UserPositionPacket : Packet
{
    public int userIndex;
    public Position position;

    public UserPositionPacket() { }

    public UserPositionPacket(SendCommand _cmd, int _index, Position _position)
    {
        this.SetCmd(_cmd);
        userIndex = _index;
        position = _position;
    }
}

[Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class UserAttackPacket : Packet
{
    public int userIndex;
    public int monsterIndex;
    public Vector position;

    public UserAttackPacket() { }

    public UserAttackPacket(SendCommand _cmd, int _userIndex, int _monsterIndex, Vector _position)
    {
        this.SetCmd(_cmd);
        userIndex = _userIndex;
        monsterIndex = _monsterIndex;
        position = _position;
    }
}

[Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class UserHitPacket : Packet
{
    public int userIndex;
    public int monsterIndex;
    public int damage;
};

[Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class UserExpPacket : Packet
{
    public int exp;
};

[Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class MonsterInfoPacket : Packet
{
    public MonsterInfo info;
};

[Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class MonsterInfoListPacket : Packet
{
    public ushort monsterNum;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 100)]
    public MonsterInfo[] info;
};

[Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class MonsterPositionPacket : Packet
{
    public int monsterIndex;
    public Vector position;
};

[Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class MonsterAttackPacket : Packet
{
    public int monsterIndex;
    public int damage;
};

[Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class MonsterHitPacket : Packet
{
    public int monsterIndex;
    public int userIndex;
    public int damage;
};

[Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)] 
public class ChattingPacket : Packet
{
    public int userIndex;

    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 15)]
    public string id;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 255)]
    public byte[] chatting;

    public ChattingPacket() { }

    public ChattingPacket(int _userIndex, string _id, string _chat)
    {
        this.SetCmd(SendCommand.C2Zone_CHATTING);
        userIndex = _userIndex;
        id = _id;
        chatting = Encoding.UTF8.GetBytes(_chat);
    }
}

[Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class ChattingPacket_Whisper : Packet
{
    public int userIndex;

    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 15)]
    public string targetId;

    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 15)]
    public string id;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 255)]
    public byte[] chatting;

    public ChattingPacket_Whisper() { }

    public ChattingPacket_Whisper(int _userIndex, string _targetId, string _id, string _chat)
    {
        this.SetCmd(SendCommand.C2Zone_CHATTING_WHISPER);
        userIndex = _userIndex;
        targetId = _targetId;
        id = _id;
        chatting = Encoding.UTF8.GetBytes(_chat);
    }
}

[Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class ChangeZonePacket : Packet
{
    public int zoneNum;
};

[Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class RegisterUserPacket : Packet
{
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 15)]
    public string id;

    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 15)]
    public string password;
};

[Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class LogInPacket : Packet
{
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 15)]
    public string id;

    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 15)]
    public string password;

    public int zoneNum;
};

[Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class LogInSuccessPacket : Packet
{
    public int userIndex;
    public int socket;
};

[Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class LogInSuccessPacket_PortNum : Packet
{
    public int userIndex;
    public int portNum;
    public int socket; 
};