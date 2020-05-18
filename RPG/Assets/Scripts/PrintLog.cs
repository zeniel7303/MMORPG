using System.Collections;
using System.Collections.Generic;
using UnityEngine;

//https://dawnarc.com/2019/09/unityprint-log-on-screen/
//https://assetstore.unity.com/packages/tools/gui/screen-logger-49114

public class PrintLog : MonoBehaviour
{
    string m_log;
    Queue m_logQueue = new Queue();

    // Start is called before the first frame update
    void Start()
    {
        //Debug.Log("log1");
    }
    void OnEnable()
    {
        Application.logMessageReceived += HandleLog;
    }

    private void OnDisable()
    {
        Application.logMessageReceived -= HandleLog;
    }

    void HandleLog(string logString, string stackTrace, LogType type)
    {
        m_log = logString;
        string newString = "\n [" + type + "] : " + m_log;
        m_logQueue.Enqueue(newString);
        if (type == LogType.Exception)
        {
            newString = "\n" + stackTrace;
            m_logQueue.Enqueue(newString);
        }
        m_log = string.Empty;
        foreach (string mylog in m_logQueue)
        {
            m_log += mylog;
        }
    }

    private void OnGUI()
    {
        GUILayout.Label(m_log);
    }

    // Update is called once per frame
    void Update()
    {

    }

}
