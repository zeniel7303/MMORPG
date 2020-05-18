using System.Collections;
using System.Collections.Generic;
using UnityEngine;

using System.Runtime.InteropServices;

public enum TileType : ushort
{
    NON, BLOCK
}

public enum DirectionType
{
    NW, N, NE, E, SE, S, SW, W
}

public static class DirectionOpposite
{
    public static DirectionType Opposite(this DirectionType _direction)
    {
        return (int)_direction < 4 ? (_direction + 4) : (_direction - 4);
    }
}

[System.Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct TileInfo
{
    //배열의 x, y값
    public ushort x, y;

    public bool isTileBlock;

    //타일의 맵 넘버
    public ushort tileZone;
}

[System.Serializable]
[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class Tile
{
    [SerializeField]
    public ushort x, y;

    [SerializeField]
    public TileType tileType;

    [SerializeField]
    public ushort tileZoneIndex;

    [SerializeField]
    public int goal;
    [SerializeField]
    public int heuristic;
    [SerializeField]
    public int fitness;

    [SerializeField]
    public bool isClosed;
    [SerializeField]
    public bool isOpened;

    [SerializeField]
    public Tile parentGrid;

    [SerializeField]
    public ushort section;

    [SerializeField]
    public ushort spawnMonsterIndex;

    public Tile(ushort _x, ushort _y, float _posX, float _posY)
    {
        x = _x;
        y = _y;

        tileZoneIndex = 0;

        tileType = TileType.NON;
        spawnMonsterIndex = 0;
    }

    public Tile(ushort _x, ushort _y, TileType _type)
    {
        x = _x;
        y = _y;
        tileType = _type;
        spawnMonsterIndex = 0;
    }

    public Tile(TileInfo _info)
    {
        x = _info.x;
        y = _info.y;

        if (_info.isTileBlock)
            tileType = TileType.BLOCK;
        else
            tileType = TileType.NON;

        tileZoneIndex = _info.tileZone;
    }

    public Tile(Tile _tile)
    {
        x = _tile.x;
        y = _tile.y;

        tileType = _tile.tileType;
        tileZoneIndex = _tile.tileZoneIndex;
        spawnMonsterIndex = _tile.spawnMonsterIndex;
    }

    public void Init()
    {
        goal = 0;
        heuristic = 0;
        fitness = 0;

        parentGrid = null;

        isOpened = false;
        isClosed = false;
    }

    public void SetParent(Tile _tile,
        ref List<Tile> _openList, ref List<Tile> _closeList,
        Tile _finishGrid)
    {
        if (!_tile.isClosed && _tile.tileType != TileType.BLOCK)
        {
            _tile.parentGrid = this;
            _tile.PathScoring(this, _finishGrid);

            if (!_tile.isOpened)
            {
                _openList.Add(_tile);
                _tile.isOpened = true;
            }
        }
    }

    public void SetNeighborParent(List<Tile> _mapList,
        int _mapSizeX, int _mapSizeY,
        ref List<Tile> _openList, ref List<Tile> _closeList,
        Tile _finishGrid)
    {
        Tile currentTile = null;

        if (x < _mapSizeX - 1)
        {
            currentTile = _mapList[y * _mapSizeX + x + 1];

            SetParent(currentTile, ref _openList, ref _closeList, _finishGrid);

            if (y < _mapSizeY - 1)
            {
                currentTile = _mapList[(y + 1) * _mapSizeX + (x + 1)];

                SetParent(currentTile, ref _openList, ref _closeList, _finishGrid);
            }
            if (y > 0)
            {
                currentTile = _mapList[(y - 1) * _mapSizeX + (x + 1)];

                SetParent(currentTile, ref _openList, ref _closeList, _finishGrid);
            }

        }
        if (x > 0)
        {
            currentTile = _mapList[(y) * _mapSizeX + (x - 1)];

            SetParent(currentTile, ref _openList, ref _closeList, _finishGrid);

            if (y < _mapSizeY - 1)
            {
                currentTile = _mapList[(y + 1) * _mapSizeX + (x - 1)];

                SetParent(currentTile, ref _openList, ref _closeList, _finishGrid);
            }
            if (y > 0)
            {
                currentTile = _mapList[(y - 1) * _mapSizeX + (x - 1)];

                SetParent(currentTile, ref _openList, ref _closeList, _finishGrid);
            }
        }

        if (y < _mapSizeY - 1)
        {
            currentTile = _mapList[(y + 1) * _mapSizeX + (x)];

            SetParent(currentTile, ref _openList, ref _closeList, _finishGrid);
        }
        if (y > 0)
        {
            currentTile = _mapList[(y - 1) * _mapSizeX + (x)];

            SetParent(currentTile, ref _openList, ref _closeList, _finishGrid);
        }
    }

    //public bool GridLineIntersection(Vector2 _start, Vector2 _finish)
    //{
    //    bool isIntersection = false;

    //    float width = PathFinder.mapWidth;
    //    float height = PathFinder.mapHeight;

    //    float posX = x * width;
    //    float posY = y * height;

    //    isIntersection = PathFinder.(
    //        _start, _finish,
    //        new Vector2(posX, posY),
    //        new Vector2(posX + width, posY));

    //    if (isIntersection)
    //        return isIntersection;

    //    isIntersection = PathFinder.LineIntersection(
    //        _start, _finish,
    //        new Vector2(posX, posY),
    //        new Vector2(posX, posY + height));

    //    if (isIntersection)
    //        return isIntersection;

    //    isIntersection = PathFinder.LineIntersection(
    //        _start, _finish,
    //        new Vector2(posX + width, posY),
    //        new Vector2(posX + width, posY + height));

    //    if (isIntersection)
    //        return isIntersection;

    //    isIntersection = PathFinder.LineIntersection(
    //        _start, _finish,
    //        new Vector2(posX, posY + height),
    //        new Vector2(posX + width, posY + height));

    //    if (isIntersection)
    //        return isIntersection;

    //    return isIntersection;
    //}

    public void PathScoring(Tile _nowGrid, Tile _finishGrid)
    {
        SetGoal(_nowGrid);
        SetHeuristic(_finishGrid);
        SetFitness();
    }

    public void SetGoal(Tile _nowGrid)
    {
        goal =  (x == _nowGrid.x || y == _nowGrid.y ? 10 : 14);
    }

    public void SetHeuristic(Tile _finishGrid)
    {
        heuristic = (Mathf.Abs(x - _finishGrid.x) +
            Mathf.Abs(y - _finishGrid.y))  *10;
    }

    public void SetFitness()
    {
        fitness = goal + heuristic;
    }
}
