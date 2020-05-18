using System.Collections;
using System.Collections.Generic;
using UnityEngine;

//where T : MonoBehaviour 있어야함.
public class ObjectPool<T> : MonoBehaviour where T : MonoBehaviour
{
    public List<T> itemList;

    public void Init(int _size, T _prefabs, Transform _parent)
    {
        itemList = new List<T>();

        for(int i = 0; i < _size; i++)
        {
            T poolObject = (T)Instantiate(_prefabs);
            poolObject.gameObject.transform.SetParent(_parent);
            poolObject.gameObject.transform.localPosition = Vector3.zero;
            poolObject.gameObject.SetActive(false);
            itemList.Add(poolObject);
        }
    }

    public T GetObject()
    {
        T poolObject = itemList[0];
        poolObject.gameObject.SetActive(true);
        itemList.RemoveAt(0);

        return poolObject;
    }

    public void ReturnObject(T _object)
    {
        _object.gameObject.SetActive(false);
        itemList.Add(_object);
    }
}
