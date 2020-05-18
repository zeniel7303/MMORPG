using System.Collections;
using System.Collections.Generic;
using UnityEngine;

using System;
using System.Text;
using System.Runtime.InteropServices;

public enum STATE : ushort
{
    SPAWN,
    IDLE,
    MOVE,
    RETURN,
    ATTACK,
    HIT,
    DEATH
};

[Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class ZeroToMax
{
    public int currentValue;
    public int maxValue;

    public ZeroToMax()
    {
        currentValue = 0;
        maxValue = 0;
    }
};

[Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct Position
{
    public Vector position;
    public Vector direction;

    public Position(Vector _dir, float _x, float _z)
    {
        position.x = _x;
        position.z = _z;
        direction = _dir;
    }
};

[Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct Vector
{
    public float x;
    public float z;

    public Vector(float _x = 0.0f, float _z = 0.0f)
    {
        x = _x;
        z = _z;
    }
};

[Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct UnitInfo
{
    public ushort zoneNum;

    public int level;
    
    public ZeroToMax hp;
    public ZeroToMax mp;
    public ZeroToMax exp;
    
    public int atk;
    public int def;

    public Position position;

    public STATE state;

    public int GetLevel()
    {
        return level;
    }
}

public class Unit : MonoBehaviour
{
    [HideInInspector]
    public UnitInfo unitInfo;

    public void SetUnitInfo(UnitInfo _unitInfo)
    {
        unitInfo = _unitInfo;
    }
}
