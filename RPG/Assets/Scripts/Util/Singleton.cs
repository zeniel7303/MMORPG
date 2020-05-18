using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Singleton<T> : MonoBehaviour where T : MonoBehaviour
{
    protected static T instance;
    public static T Instance
    {
        get
        {
            if (!instance)
            {
                instance = GameObject.FindObjectOfType(typeof(T)) as T;

                if (!instance)
                {
                    GameObject container = new GameObject();
                    container.name = typeof(T).Name;
                    instance = container.AddComponent(typeof(T)) as T;
                }
            }

            return instance;
        }
    }

    protected virtual void Awake()
    {
        if (instance != null && instance != this)
        {
            Destroy(this.gameObject);
            return;
        }
        else
        {
            instance = GameObject.FindObjectOfType(typeof(T)) as T;
        }

        DontDestroyOnLoad(this.gameObject);

    }
}