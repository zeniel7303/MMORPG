using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.IO;

[System.Serializable]
public struct MonsterSpawnEditor
{
    public ushort monsterIndex;
    public Color gridColor;
}

public class TilesGroup : MonoBehaviour
{
    public float width = 1.0f;
    public float height = 1.0f;

    public int mapSizeX = 100;
    public int mapSizeY = 100;

    [HideInInspector]
    public List<Tile> mapList;

    private Tile hoverGrid;
    public Tile HoverGrid
    {
        get { return hoverGrid; }
        set
        {
            if (hoverGrid != value)
            {
                hoverGrid = value;
            }
        }
    }
    [HideInInspector]
    public bool isReleaseBackDraw = false;
    [HideInInspector]
    public bool isReleaseDraw = false;

    [HideInInspector]
    public bool isTerrain, isBlock, isMonsterSpawn;
    
    [HideInInspector]
    public BoxCollider boxCollider;

    public float linePos = 0.05f;

    public bool isShowGizmo = false;

    [HideInInspector]
    public int brushSize = 0;

    [HideInInspector]
    public int selectSpawnIndex;

    [HideInInspector]
    public List<MonsterSpawnEditor> spawnList = new List<MonsterSpawnEditor>();

    public void Init()
    {
        boxCollider = this.GetComponent<BoxCollider>();

        if (boxCollider == null)
        {
            boxCollider = this.gameObject.AddComponent<BoxCollider>();
            ReleaseBoxCollider();
        }
    }
    
    public void SetHoverGrid(float _x, float _z)
    {
        if (isReleaseBackDraw)
        {
            _x -= this.transform.position.x;
            _z -= this.transform.position.z;

            int curX = (int)(_x / height);
            int curY = (int)(_z / width);

            HoverGrid = mapList[curY * mapSizeX + curX];

            isReleaseDraw = true;
        }
    }

    public void SaveToBinary(string _path)
    {
        using (
            BinaryWriter writer = new BinaryWriter(File.Open(_path, FileMode.Create)))
        {
            writer.Write(mapSizeX);
            writer.Write(mapSizeY);

            for (int i = 0; i < mapList.Count; i++)
            {
                writer.Write((ushort)mapList[i].tileType);
                writer.Write((ushort)mapList[i].spawnMonsterIndex);
            }
        }
    }

    public void LoadToBinary(string _path)
    {
        BinaryReader reader = new BinaryReader(File.OpenRead(_path));

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

        spawnList.Clear();
        for (int i = 0; i < monsterIndexList.Count; i++)
        {
            MonsterSpawnEditor spawnData = new MonsterSpawnEditor();
            spawnData.gridColor =
            new Color(
                UnityEngine.Random.Range(0.0f, 1.0f),
                UnityEngine.Random.Range(0.0f, 1.0f),
                UnityEngine.Random.Range(0.0f, 1.0f)
                );
            spawnData.monsterIndex = monsterIndexList[i];
            spawnList.Add(spawnData);
        }

        width = 1.0f;
        height = 1.0f;

        ReleaseBoxCollider();

        isReleaseBackDraw = true;
    }

    public void CreateGrid()
    {
        mapList = new List<Tile>();

        for (int y = 0; y < mapSizeY; y++)
        {
            for (int x = 0; x < mapSizeX; x++)
            {
                Tile currentTile = new Tile((ushort)x, (ushort)y,
                    this.transform.position.x + (x * width),
                    this.transform.position.z + (y * height));
                mapList.Add(currentTile);
            }
        }
        
        ReleaseBoxCollider();

        isReleaseBackDraw = true;
    }

    private void ReleaseBoxCollider()
    {
        boxCollider.center = new Vector3(
            (width * mapSizeX) / 2.0f,
            0.0f,
            (height * mapSizeY) / 2.0f);

        boxCollider.size = new Vector3(width * mapSizeX, 0.0f, height * mapSizeY);
    }

    //계속 Event가 오지 않음
    //카메라의 값이 변할때만 호출
    //그래서 그때그때 값이 안변하는거같지만 변해있다! 단지 화면 출력만 그렇게 안할뿐.
    private void OnDrawGizmos()
    {
        if (!isShowGizmo)
            return;

        if (isReleaseBackDraw)
        {
            BackgroundDraw();
        }

        if (isReleaseDraw &&
            (isBlock || isTerrain))
        {
            SelectGridDraw();
        }
    }
    

    void SelectGridDraw()
    {
        Vector3 pos = new Vector3(0.0f, this.transform.position.y, 0.0f);

        if (isBlock || isTerrain)
            Gizmos.color = Color.blue;

        if (hoverGrid == null) return;

        if (brushSize == 0)
        {
            float posX = hoverGrid.x * width;
            float posY = hoverGrid.y * height;
            
            Gizmos.DrawLine(
                  new Vector3(
                      posX,
                      0.0f,
                      posY
                      ) + pos,

                  new Vector3(
                      posX + width,
                      0.0f,
                      posY
                      ) + pos
                      );

            Gizmos.DrawLine(pos +
                new Vector3(
                    posX + width,
                    0.0f,
                    posY), pos +
                new Vector3(
                    posX + width,
                    0.0f,
                    posY + width));

            Gizmos.DrawLine(pos +
                new Vector3(
                    posX,
                    0.0f,
                    posY), pos +
                new Vector3(
                    posX,
                    0.0f,
                    posY + width));

            Gizmos.DrawLine(pos +
                new Vector3(
                    posX,
                    0.0f,
                    posY + width),
               pos +
                new Vector3(
                    posX + width,
                    0.0f,
                    posY + width));
        }
        else
        {
            for (int y = -brushSize; y <= brushSize; y++)
            {
                for (int x = -brushSize; x <= brushSize; x++)
                {
                    int curX = hoverGrid.x + x;
                    int curY = hoverGrid.y + y;

                    if (curX >= 0 && curX < mapSizeX &&
                        curY >= 0 && curY < mapSizeY)
                    {
                        Tile currentTile = mapList[curY * mapSizeX + curX];
                        
                        float posX = currentTile.x * width;
                        float posY = currentTile.y * height;

                        Gizmos.DrawLine(
                              new Vector3(
                                  posX,
                                  0.0f,
                                  posY
                                  ) + pos,

                              new Vector3(
                                  posX + width,
                                  0.0f,
                                  posY
                                  ) + pos
                                  );

                        Gizmos.DrawLine(pos +
                            new Vector3(
                                posX + width,
                                0.0f,
                                posY), pos +
                            new Vector3(
                                posX + width,
                                0.0f,
                                posY + width));

                        Gizmos.DrawLine(pos +
                            new Vector3(
                                posX,
                                0.0f,
                                posY), pos +
                            new Vector3(
                                posX,
                                0.0f,
                                posY + width));

                        Gizmos.DrawLine(pos +
                            new Vector3(
                                posX,
                                0.0f,
                                posY + width),
                           pos +
                            new Vector3(
                                posX + width,
                                0.0f,
                                posY + width));
                    }
                }
            }
        }
    }

    void BackgroundDraw()
    {
        Vector3 pos = new Vector3(0.0f, this.transform.position.y, 0.0f);

        for (int i = mapList.Count - 1; i >= 0; i--)
        {
            Tile currentTile = mapList[i];

            if (currentTile.tileType == TileType.NON)
            {
                if (currentTile.spawnMonsterIndex == 0)
                    Gizmos.color = Color.white;
                else
                {
                    try
                    {
                        int monsterSpawnIndex = spawnList.FindIndex(0, item => item.monsterIndex == currentTile.spawnMonsterIndex);
                        Gizmos.color = spawnList[monsterSpawnIndex].gridColor;
                    }
                    catch
                    {
                        currentTile.spawnMonsterIndex = 0;
                        Gizmos.color = Color.white;
                    }
                }

                float posX = currentTile.x * width;
                float posY = currentTile.y * height;

                Gizmos.DrawLine(
                    new Vector3(
                        posX + linePos,
                        0.0f,
                        posY + linePos
                        ) + pos,

                    new Vector3(
                        posX + width - linePos,
                        0.0f,
                        posY + linePos
                        ) + pos
                        );

                Gizmos.DrawLine(pos +
                    new Vector3(
                        posX + width - linePos,
                        0.0f,
                        posY + linePos), pos +
                    new Vector3(
                        posX + width - linePos,
                        0.0f,
                        posY + height - linePos));

                Gizmos.DrawLine(pos +
                    new Vector3(
                        posX + linePos,
                        0.0f,
                        posY + linePos), pos +
                    new Vector3(
                        posX + linePos,
                        0.0f,
                        posY + height - linePos));

                Gizmos.DrawLine(pos +
                    new Vector3(
                        posX + linePos,
                        0.0f,
                        posY + height - linePos),
                   pos +
                    new Vector3(
                        posX + width - linePos,
                        0.0f,
                        posY + height - linePos));
            }
        }
    }
}

