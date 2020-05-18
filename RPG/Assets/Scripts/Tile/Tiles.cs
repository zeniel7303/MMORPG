using System.Collections;
using System.Collections.Generic;
using System;
using UnityEngine;
using System.IO;
using System.Runtime.InteropServices;

public class Tiles{

    public float width = 1.0f;
    public float height = 1.0f;

    public int mapSizeX = 100;
    public int mapSizeY = 100;
 
    public List<Tile> mapList;

    public void LoadTile(string _filePath)
    {
        BinaryReader reader = new BinaryReader(File.OpenRead(_filePath));

        mapList = new List<Tile>();
        List<ushort> monsterIndexList = new List<ushort>();

        mapSizeX = reader.ReadInt32();
        mapSizeY = reader.ReadInt32();

        for (ushort y = 0; y < mapSizeY; y++)
        {
            for (ushort x = 0; x < mapSizeX; x++)
            {
                TileType currentTileType = (TileType)reader.ReadUInt16();
                ushort currentMonsterIndex = reader.ReadUInt16();

                Tile currentTile = new Tile(x, y, currentTileType);
                currentTile.spawnMonsterIndex = currentMonsterIndex;

                if (currentMonsterIndex != 0 &&
                    !monsterIndexList.Contains(currentMonsterIndex))
                {
                    monsterIndexList.Add(currentMonsterIndex);
                }

                mapList.Add(currentTile);
            }
        }
    }
}