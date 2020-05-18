using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.Runtime.InteropServices;

[Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct UserInfo
{
    public int userID;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 20)]
    public string name;
};