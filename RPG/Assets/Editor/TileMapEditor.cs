using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using UnityEditorInternal;

[CustomEditor(typeof(TilesGroup))]
public class TileMapEditor : Editor
{
    private Vector3 mouseHitPos;

    TilesGroup tileMap;

    private bool isDragging;

    private ReorderableList spawnList;

    //인스펙터에서 이 오브젝트를 선택하면 호출된다
    public void OnEnable()
    {
        tileMap = (TilesGroup)target;
        tileMap.Init();
        isDragging = false;

        SceneView.onSceneGUIDelegate = GridUpdate;

        spawnList = new ReorderableList(serializedObject,
            serializedObject.FindProperty("spawnList"),
            true, true, true, true);

        spawnList.drawElementCallback =
            (Rect rect, int index, bool isActive, bool isFocused) =>
            {
                var element = spawnList.serializedProperty.GetArrayElementAtIndex(index);
                rect.y += 2;
                EditorGUI.PropertyField(
                    new Rect(rect.x, rect.y, 60, EditorGUIUtility.singleLineHeight),
                    element.FindPropertyRelative("monsterIndex"), GUIContent.none);

                EditorGUI.PropertyField(
                    new Rect(rect.x + 60 + 10, rect.y, rect.width - 80, EditorGUIUtility.singleLineHeight),
                    element.FindPropertyRelative("gridColor"), GUIContent.none);
            };

        spawnList.drawHeaderCallback = (Rect rect) =>
        {
            EditorGUI.LabelField(rect, "Spawn Mosnter Data");
        };

        spawnList.drawElementBackgroundCallback =
            (Rect rect, int index, bool isActive, bool isFocused) =>
            {
                if (Event.current.type == EventType.Repaint)
                {
                    EditorStyles.miniButton.Draw(rect, false, isActive, isFocused, false);
                }
            };

        spawnList.onAddCallback = (ReorderableList l) =>
        {
            var index = l.serializedProperty.arraySize;
            l.serializedProperty.arraySize++;
            l.index = index;

            var element = l.serializedProperty.GetArrayElementAtIndex(index);
            element.FindPropertyRelative("monsterIndex").intValue = index + 10000 + 1;
            element.FindPropertyRelative("gridColor").colorValue =
            new Color(
                UnityEngine.Random.Range(0.0f, 1.0f),
                UnityEngine.Random.Range(0.0f, 1.0f),
                UnityEngine.Random.Range(0.0f, 1.0f)
                );
        };

        spawnList.onSelectCallback = (ReorderableList l) =>
        {
            var index = l.index;
            tileMap.selectSpawnIndex = index;
        };
    }

    public override void OnInspectorGUI()
    {
        DrawDefaultInspector();

        GUILayout.BeginHorizontal();

        GUILayout.Label(string.Format("Brush Size : {0}", tileMap.brushSize + 1));

        tileMap.brushSize = (int)GUILayout.HorizontalSlider(tileMap.brushSize, 0, 9);

        GUILayout.EndHorizontal();

        GUILayout.BeginHorizontal();

        bool isTerrainCheck = GUILayout.Toggle(tileMap.isTerrain, "Set Tile Terrain");

        if (isTerrainCheck != tileMap.isTerrain)
        {
            tileMap.isTerrain = isTerrainCheck;

            tileMap.isBlock = false;
            tileMap.isMonsterSpawn = false;
        }

        bool isBlockCheck = GUILayout.Toggle(tileMap.isBlock, "Set Tile Block");

        if (isBlockCheck != tileMap.isBlock)
        {
            tileMap.isBlock = isBlockCheck;

            tileMap.isTerrain = false;
            tileMap.isMonsterSpawn = false;
        }

        bool isMonsterSpawn = GUILayout.Toggle(tileMap.isMonsterSpawn, "Set Monster spawn position");

        if (isMonsterSpawn != tileMap.isMonsterSpawn)
        {
            if (isMonsterSpawn)
            {
                tileMap.brushSize = 0;
            }

            tileMap.isMonsterSpawn = isMonsterSpawn;

            tileMap.isTerrain = false;
            tileMap.isBlock = false;
        }

        GUILayout.EndHorizontal();

        serializedObject.Update();
        spawnList.DoLayoutList();
        serializedObject.ApplyModifiedProperties();

        if (GUILayout.Button("CreateTile"))
        {
            tileMap.CreateGrid();
        }

        if (GUILayout.Button("SaveTile"))
        {
            tileMap.SaveToBinary(EditorUtility.SaveFilePanelInProject("저장하기",
                "New Map", "fmap", "gg", "Assets"));
        }

        if (GUILayout.Button("LoadTile"))
        {
            string curPath = EditorUtility.OpenFilePanel("불러오기", "Assets", "fmap");

            tileMap.LoadToBinary(curPath);
        }
    }

    void GridUpdate(SceneView sceneview)
    {
        if (!tileMap.isShowGizmo)
            return;

        Event e = Event.current;

        Vector3 temp = new Vector3(e.mousePosition.x, -e.mousePosition.y + Camera.current.pixelHeight);

        Ray ray = Camera.current.ScreenPointToRay(temp);

        RaycastHit hit;

        if (Physics.Raycast(ray, out hit))//, 30.0f))
        {
            if (hit.collider.gameObject == tileMap.gameObject)
                tileMap.SetHoverGrid(hit.point.x, hit.point.z);
        }

        if (e.type == EventType.KeyDown)
        {
            isDragging = true;
        }
        else if (e.type == EventType.KeyUp)
        {
            isDragging = false;
        }

        if(tileMap.isMonsterSpawn &&
            e.button == 0 && e.type == EventType.MouseDown)
        {
            if (tileMap.HoverGrid != null &&
                tileMap.HoverGrid.tileType == TileType.NON)
            {
                tileMap.HoverGrid.spawnMonsterIndex =
                    tileMap.spawnList[tileMap.selectSpawnIndex].monsterIndex;

                isDragging = false;
            }
        }

        //if ((e.type == EventType.MouseDrag || e.type == EventType.MouseDown) && e.button == 0 &&
        //    tileMap.isMonsterSpawn)
        //{
        //    if (tileMap.HoverGrid != null &&
        //        tileMap.HoverGrid.tileType == TileType.NON)
        //    {
        //        tileMap.HoverGrid.spawnMonsterIndex =
        //            tileMap.spawnList[tileMap.selectSpawnIndex].monsterIndex;

        //        isDragging = false;
        //    }
        //}

        if (isDragging)
        {
            if (tileMap.isBlock || tileMap.isTerrain)
            {
                if (tileMap.brushSize == 0)
                {
                    if (tileMap.HoverGrid != null)
                    {
                        if (tileMap.isTerrain)
                        {
                            tileMap.HoverGrid.tileType = TileType.NON;
                        }
                        else if (tileMap.isBlock)
                        {
                            tileMap.HoverGrid.tileType = TileType.BLOCK;
                        }

                        tileMap.HoverGrid.spawnMonsterIndex = 0;
                    }
                }
                else
                {
                    for (int y = -tileMap.brushSize; y <= tileMap.brushSize; y++)
                    {
                        for (int x = -tileMap.brushSize; x <= tileMap.brushSize; x++)
                        {
                            int curX = tileMap.HoverGrid.x + x;
                            int curY = tileMap.HoverGrid.y + y;

                            if (curX >= 0 && curX < tileMap.mapSizeX &&
                                curY >= 0 && curY < tileMap.mapSizeY)
                            {
                                Tile currentTile = tileMap.mapList[curY * tileMap.mapSizeX + curX];

                                if (currentTile != null)
                                {
                                    if (tileMap.isTerrain)
                                    {
                                        currentTile.tileType = TileType.NON;
                                    }
                                    else if (tileMap.isBlock)
                                    {
                                        currentTile.tileType = TileType.BLOCK;
                                    }

                                    tileMap.HoverGrid.spawnMonsterIndex = 0;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}