using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class FloatingChatting : MonoBehaviour
{
    private Text text;

    private bool floating;
    private float time;

    private bool isPlayerManager;

    private void Awake()
    {
        text = this.GetComponentInChildren<Text>();
        this.gameObject.SetActive(false);

        isPlayerManager = true;
    }

    // Update is called once per frame
    void Update()
    {
        if (floating)
        {
            transform.LookAt(this.GetComponent<Canvas>().worldCamera.transform);

            time += Time.deltaTime;

            if (time >= 3.0f)
            {
                this.gameObject.SetActive(false);
            }
        }
    }

    public void Floating(string _message, bool _bool)
    {
        this.gameObject.SetActive(true);
        this.GetComponent<Canvas>().worldCamera =
            GameObject.FindWithTag("MainCamera").GetComponent<Camera>();

        isPlayerManager = false;
        floating = true;
        time = 0.0f;

        text.text = _message + "\n\n";
    }
}
