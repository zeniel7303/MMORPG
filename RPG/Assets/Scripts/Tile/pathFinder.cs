using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public static class PathFinder
{
    public static List<Tile> mapList;

    public static List<Tile> openList;
    public static List<Tile> closeList;

    public static Tile currentTile, startTile, endTile, tempTile;

    public static int mapSizeX, mapSizeY;
    public static float mapWidth, mapHeight;
        
    public static Tiles tileMap;

    public static void Init(ref TilesGroup _tileMap)
    {
        openList = new List<Tile>();
        closeList = new List<Tile>();
    }

    public static void Init(ref Tiles _tileMap)
    {
        openList = new List<Tile>();
        closeList = new List<Tile>();

        tileMap = _tileMap;

        mapSizeX = tileMap.mapSizeX;
        mapSizeY = tileMap.mapSizeY;

        mapWidth = tileMap.width;
        mapHeight = tileMap.height;

        mapList = new List<Tile>();

        for (int i = 0; i < _tileMap.mapList.Count; i++)
        {
            mapList.Add(new Tile(_tileMap.mapList[i]));

            mapList[i].section = (ushort)((mapList[i].x * mapList[i].y) / 100);
        }
    }

    public static Tile GetTile(Vector3 _pos)
    {
        int x = (int)((_pos.x) / mapWidth);
        int z = (int)((_pos.z) / mapHeight);

        if (x < 0 || x >= mapSizeX || z < 0 || z >= mapSizeY)
            return null;

        return mapList[z * mapSizeX + x];
    }

    public static Tile GetTile(int _x, int _y)
    {
        if (_x < 0 || _x >= mapSizeX || _y < 0 || _y >= mapSizeY)
            return null;

        return mapList[_y * mapSizeX + _x];
    }

    public static Quaternion TargetLookAt(Vector3 _originEuler, Vector3 _to, Vector3 _from)
    {
        return
            Quaternion.Euler(
                _originEuler.x,
                GetDegree(_to, _from),
                _originEuler.z);
    }

    private static float GetDegree(Vector3 _to, Vector3 _from)
    {
        return Mathf.Atan2(_to.x - _from.x, _to.z - _from.z) * 180.0f / Mathf.PI - 180.0f;
    }

    //이제 서버 클라 소통시 좌표를 송수신하므로 타일간 패스파인딩은 쓸 일이 없다.
    /*
    public static void pathFindingStart(Tile _startTile, Tile _endTile)
    {
        openList.Clear();
        closeList.Clear();

        for (int i = 0; i < tileMap.mapList.Count; i++)
        {
            if (tileMap.mapList[i].tileType != TileType.OBJECT)
            {
                tileMap.mapList[i].isOpened = true;
            }
        }

        _pathFindingEnded = false;
        _count = 0;
        _stack = 0;

        pathFinding(startTile, endTile);
    }
    */

    /*
    public static List<Tile> AddOpenList(Tile currentTile)
    {
        if (currentTile == null)
        {
            return _openList;
        }

        int startX = currentTile.x - 1;
        int startY = currentTile.y - 1;

        for(int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                Tile node = _tileMap.mapList[(startY * (int)(_tileMap.mapSizeX / _tileMap.width)) + 
                    startX + j + (i * (int)(_tileMap.mapSizeY / _tileMap.height))];

                if (!node.isOpened) continue;
                if (node.tileType == TileType.OBJECT) continue;
                if (node.x == _startTile.x && node.y == _startTile.y) continue;

                node.parentTile = currentTile;

                //임의의 색출변수
                bool addObj = true;

                if(_openList.Count > 0)
                {
                    for (int k = 0; k < _openList.Count; ++k)
                    {
                        //이미 갈 수 있는 길로 체크됬으면
                        if (_openList[k] == node)
                        {
                            //연산에서 제외
                            addObj = false;
                            break;
                        }
                    }
                }         

                if (!addObj) continue;

                _openList.Add(node);
            }                
        }

        return _openList;
    }

    public static List<Tile> pathFinding(Tile startTile, Tile endTile)
    {
        //시작 전에 할당하고 초기화들 해주자
        List<Tile> finalTile = new List<Tile>();

        if (_closeList.Count > 0)
        {
            for (int i = 0; i < _closeList.Count; i++)
            {
                _closeList[i].Init();
            }
        }

        if (_openList.Count > 0)
        {
            for (int i = 0; i < _openList.Count; i++)
            {
                _openList[i].Init();
            }
        }

        _openList.Clear();
        _closeList.Clear();

        _pathFindingEnded = false;
        _count = 0;
        _stack = 0;

        float tempTotalCost = 5000;
        Tile tempTile = null;

        _startTile = startTile;
        _endTile = endTile;
        _currentTile = startTile;
        _tempTile = null;

        //_time = Random.Range(0f, 0.35f);
        //_currentTime += Time.fixedDeltaTime;

        //if (_currentTime > _time)
        //{

        //}

        //본격적인 길찾기 시작
        while (true)
        {
            tempTotalCost = 5000;
            tempTile = null;

            if (_stack > 200) break;

            for (int i = 0; i < AddOpenList(_currentTile).Count; ++i)
            {
                _openList[i].costToGoal = (Mathf.Abs(_endTile.x - _openList[i].x) + Mathf.Abs(_endTile.y - _openList[i].y)) * 10;

                Vector2Int center1 = new Vector2Int(_openList[i].parentTile.x, _openList[i].parentTile.y);
                Vector2Int center2 = new Vector2Int(_openList[i].x, _openList[i].y);

                float x1 = Vector2Int.Distance(center1, center2);

                if (x1 > _tileMap.width)
                {
                    _openList[i].costFromStart = _openList[i].parentTile.costFromStart + 14;
                }
                else
                {
                    _openList[i].costFromStart = _openList[i].parentTile.costFromStart + 10;
                }

                _openList[i].totalCost = _openList[i].costFromStart + _openList[i].costToGoal;

                //경로 비용은 다 담겼지만, 가장 작은 경로비용을 산출
                if (tempTotalCost > _openList[i].totalCost)
                {
                    tempTotalCost = _openList[i].totalCost;
                    tempTile = _openList[i];
                }

                bool addObj = true;

                if (_openList.Count > 0)
                {
                    for (int j = 0; j < _openList.Count; ++j)
                    {
                        if (_openList[j] == tempTile)
                        {
                            addObj = false;
                            break;
                        }
                    }
                }

                _openList[i].isOpened = false;

                if (!addObj) continue;

                _openList.Add(tempTile);
            }

            if (tempTile == _endTile)
            {
                break;
            }

            if (_openList.Count == 0)
            {
                break;
            }

            //도착했을때, 체크된 최단거리를 클로즈벡터에 담는다
            _closeList.Add(tempTile);

            for (int i = 0; i < _openList.Count; ++i)
            {
                if (_openList[i] == tempTile)
                {
                    _openList.RemoveAt(i);
                    break;
                }
            }

            _currentTile = tempTile;

            _stack += 1;
        }

        if (_stack > 200)
        {
            return null;
        }

        if (_openList.Count == 0)
        {
            return null;
        }

        //도착
        while (_currentTile.parentTile != null)
        {
            //OutputDebugString(L"1\n");
            _currentTile = _currentTile.parentTile;
        }

        _tempTile = tempTile;

        //마지막 타일을 갈 길을 담는 벡터에 담는다.
        finalTile.Add(tempTile);

        Tile temp1 = tempTile.parentTile;

        //마지막 타일부터 부모 노드를 따라 갈 길을 담는 벡터에 담는 함수
        //finish(temp1);
        bool isExisted = false;

        while (true)
        {
            isExisted = false;

            for (int i = 0; i < _closeList.Count; ++i)
            {
                if (_closeList[i] == temp1)
                {
                    finalTile.Add(_closeList[i]);

                    temp1 = _closeList[i].parentTile;

                    isExisted = true;

                    break;
                }
            }

            if (!isExisted) break;
        }

        //역순정렬
        Tile temp2;
        for (int i = 0; i < finalTile.Count / 2; i++)
        {
            temp2 = finalTile[i];
            finalTile[i] = finalTile[(finalTile.Count - 1) - i];
            finalTile[(finalTile.Count - 1) - i] = temp2;
        }

        finalTile = primaryErase(finalTile);

        //for (int i = 0; i < finalTile.Count; i++)
        //{
        //    Vector3Int tmp = new Vector3Int(i, finalTile[i].x, finalTile[i].y);
        //    Debug.Log(tmp);
        //}

        _pathFindingEnded = true;

        return finalTile;
    }
    */

    public static List<Vector2> PathFindStart(Tile _startTile, Tile _endTile, 
        Vector2 _startVec, Vector2 _targetVec)
    {
        closeList.Clear();
        openList.Clear();

        startTile = _startTile;
        endTile = _endTile;

        return PathFinding(_startVec, _targetVec);
    }

    public static List<Vector2> PathFinding(Vector2 _startVec, Vector2 _targetVec)
    {
        if (startTile == null || endTile == null)
            return null;

        List<Vector2> pathList = new List<Vector2>();

        startTile.Init();

        openList.Add(startTile);
        startTile.isOpened = true;

        startTile.SetNeighborParent(mapList, mapSizeX, mapSizeY, ref openList, ref closeList, endTile);

        openList.RemoveAt(0);
        startTile.isOpened = false;

        closeList.Add(startTile);

        startTile.isClosed = true;

        Tile currentTile = null;

        while (true)
        {
            currentTile = (from Tile item in openList
                       orderby item.fitness ascending
                       select item).FirstOrDefault();

            if (currentTile == null)
                return null;

            if (currentTile.Equals(endTile))
            {
                closeList.Add(currentTile);
                currentTile.isClosed = true;

                //부모 역순으로 검색
                break;
            }
            else
            {
                if (!currentTile.isClosed)
                    currentTile.SetNeighborParent(mapList, mapSizeX, mapSizeY,
                        ref openList, ref closeList, endTile);

                closeList.Add(currentTile);
                openList.Remove(currentTile);

                currentTile.isOpened = false;
                currentTile.isClosed = true;
            }
        }

        currentTile = endTile;

        DirectionType prevDir = tileDirectionCheck(currentTile, currentTile.parentGrid);
        DirectionType nextDir = tileDirectionCheck(currentTile, currentTile.parentGrid);

        pathList.Add(_targetVec);
        currentTile = currentTile.parentGrid;

        while (currentTile != null)
        {
            float posX = currentTile.x * mapWidth;
            float posY = currentTile.y * mapHeight;

            pathList.Add(new Vector2(posX + 0.5f, posY + 0.5f));
            currentTile = currentTile.parentGrid;
        }

        //초기화
        if (closeList.Count > 0)
        {
            for (int i = 0; i < closeList.Count; i++)
            {
                closeList[i].isClosed = false;
            }
        }
        if (openList.Count > 0)
        {
            for (int i = 0; i < openList.Count; i++)
            {
                openList[i].isOpened = false;
            }
        }

        pathList.Reverse();

        return pathList;
    }

    private static DirectionType tileDirectionCheck(Tile curTile, Tile nextTile)
    {
        if (curTile.x < nextTile.x)
        {
            if (curTile.y < nextTile.y)
            {
                return DirectionType.NE;
            }
            else if (curTile.y > nextTile.y)
            {
                return DirectionType.SE;
            }
            else
            {
                return DirectionType.E;
            }
        }
        else if (curTile.x > nextTile.x)
        {
            if (curTile.y < nextTile.y)
            {
                return DirectionType.NW;
            }
            else if (curTile.y > nextTile.y)
            {
                return DirectionType.SW;
            }
            else
            {
                return DirectionType.W;
            }
        }
        else if (curTile.y < nextTile.y)
        {
            return DirectionType.N;
        }
        else
        {
            return DirectionType.S;
        }
    }

    private static List<Tile> primaryErase(List<Tile> checkTile)
    {
        DirectionType[] curDir = new DirectionType[checkTile.Count];
        for (int i = 0; i < checkTile.Count - 1; i++)
        {
            curDir[i] = tileDirectionCheck(checkTile[i], checkTile[i + 1]);
            //Debug.Log(curDir[i]);
        }

        int[] tmpInt = new int[checkTile.Count];
        int check = 0;
        for (int i = 1; i < checkTile.Count - 1; i++)
        {
            if (curDir[i - 1] == curDir[i] && curDir[i] == curDir[i + 1])
            {
                tmpInt[check] = i;
                check++;
            }
        }

        for (int i = check - 1; i >= 0; i--)
        {
            int x = tmpInt[i];
            checkTile.RemoveAt(x);
        }

        return checkTile;
    }
}