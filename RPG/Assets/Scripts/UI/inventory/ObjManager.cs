using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ObjManager : MonoBehaviour
{
    // 싱글톤
    static ObjManager st;
    public static ObjManager Call() { return st; }
    void Awake() { st = this; }
    // 게임종료 후 메모리 날려버림.
    void OnDestroy()
    {
        MemoryDelete();
        st = null;
    }

    public GameObject[] Origin;         // 프리팹 원본.
    public List<GameObject> Manager;    // 생성된 객체들을 저장할 리스트.
    public Inventory IV;

    void Start()
    {
        
    }

    // 오브젝트를 받아 생성. (생성한 원본 오브젝트, 생성할 갯수, 생성할 객체의 이름)
    public void SetObject(GameObject _Obj, int _Count, string _Name)
    {
        for (int i = 0; i < _Count; i++)
        {
            GameObject obj = Instantiate(_Obj) as GameObject;
            obj.transform.name = _Name;                     // 이름을 정한다.
            obj.transform.localPosition = Vector3.zero;     // 위치를 정한다.
            obj.SetActive(false);                           // 객체를 비활성화.
            obj.transform.parent = transform;               // 매니저 객체의 자식으로.
            Manager.Add(obj);                               // 리스트에 저장.
        }
    }

    // 필요한 오브젝트를 찾아 반환.
    public GameObject GetObject(string _Name)
    {
        if (Manager == null)
            return null;

        int Count = Manager.Count;
        for (int i = 0; i < Count; i++)
        {
            // 이름이 같지 않으면. 
            if (_Name != Manager[i].name)
                continue;

            //GameObject Obj = Manager[i];

            //// 활성화가 되어있다면.
            //if (Obj.active == true)
            //{
            //    // 리스트의 마지막까지 돌았지만 모든 객체가 사용중이라면.
            //    if (i == Count - 1)
            //    {
            //        // 총알을 새롭게 생성.
            //        //SetObject(Obj, 1, "Bullet");
            //        return Manager[i + 1];
            //    }
            //    continue;
            //}
            //return Manager[i];
        }
        return null;
    }

    // 메모리 삭제.
    public void MemoryDelete()
    {
        if (Manager == null)
            return;

        int Count = Manager.Count;

        for (int i = 0; i < Count; i++)
        {
            GameObject obj = Manager[i];
            GameObject.Destroy(obj);
        }
        Manager = null;
    }
}
