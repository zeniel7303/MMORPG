using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.Text;
using System.Runtime.InteropServices;

public enum RecvCommand
{
    Zone2C_ISCONNECTED = 50,

    Zone2C_REGISTER_USER = 100,

    Zone2C_UPDATE_INFO = 111,

    Zone2C_ENTER_FIELD = 200,
    Zone2C_USER_LIST = 210,
    Zone2C_USER_LIST_IN_RANGE = 220,
    Zone2C_USER_LIST_VISIBLE = 230,
    Zone2C_USER_LIST_INVISIBLE = 240,
    Zone2C_ENTER_USER_INFO = 250,
    Zone2C_ENTER_USER_INFO_IN_RANGE = 260,
    Zone2C_LEAVE_USER_INFO = 270,
    Zone2C_USER_HIT = 280,
    Zone2C_USER_PLUS_EXP = 290,
    Zone2C_USER_LEVEL_UP = 300,
    Zone2C_USER_DEATH = 310,
    Zone2C_USER_REVIVE = 320,
    Zone2C_LEAVE_SECTOR_USER_INFO = 330,
    Zone2C_ENTER_SECTOR_USER_INFO = 340,

    Zone2C_USER_MOVE = 301,
    Zone2C_USER_MOVE_FINISH = 311,
    Zone2C_USER_ATTACK_MONSTER = 321,
    Zone2C_USER_ATTACK_FAILED = 331,

    Zone2C_MONSTER_INFO_LIST = 500,
    Zone2C_MONSTER_INFO_LIST_IN_RANGE = 510,
    Zone2C_MONSTER_INFO_LIST_VISIBLE = 520,
    Zone2C_MONSTER_INFO_LIST_INVISIBLE = 530,
    Zone2C_MONSTER_INFO = 540,

    Zone2C_MONSTER_MOVE = 600,
    Zone2C_MONSTER_ATTACK = 610,
    Zone2C_MONSTER_HIT = 620,
    Zone2C_MONSTER_HIT_FAIL = 630,

    Zone2C_CHATTING = 1001,

    Zone2C_CHECK_ALIVE = 5000,

    Zone2C_REGISTER_USER_SUCCESS = 6000,
    Zone2C_REGISTER_USER_FAILED = 6010,

    Zone2C_LOGIN_SUCCESS = 7100,
    Zone2C_LOGIN_FAILED = 7110,
    Zone2C_LOGIN_FAILED_DUPLICATED = 7120
}

public enum SendCommand
{
    C2Zone_REQUIRE_INFO = 101,
    C2Zone_UPDATE_INFO = 111,

    C2Zone_TRY_ENTER_FIELD = 201,
    C2Zone_ENTER_FIELD_SUCCESS = 211,

    C2Zone_USER_MOVE = 301,
    C2Zone_USER_MOVE_FINISH = 311,
    C2Zone_USER_ATTACK_MONSTER = 321,
    C2Zone_USER_ATTACK_FAILED = 331,
    C2Zone_USER_REVIVE = 341,

    C2Zone_ENTER_FIELD = 401,
    C2Zone_ENTER_VILLAGE = 411,

    C2Zone_CHATTING = 1001,

    C2Zone_EXIT_USER = 2001,

    C2Zone_CHECK_ALIVE = 5000,

    C2Zone_REGISTER_ACCOUNT = 6001,

    C2Zone_LOGIN = 7001
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
    private ushort fieldNum;

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

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 30)]
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
};