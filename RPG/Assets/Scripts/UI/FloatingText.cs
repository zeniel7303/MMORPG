using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class FloatingText : MonoBehaviour
{
    public Text FloatTextPrint;
    public Canvas canvas;

    float moveSpeed;
    float destroyTime;

    Vector3 vector;

    public void print(string Text, bool isEnemy)
    {
        if(isEnemy)
        {
            FloatTextPrint.color = Color.red;
        }
        else
        {
            FloatTextPrint.color = Color.white;
        }

        FloatTextPrint.text = string.Format(" {0}", Text);
    }

    private void Start()
    {
        canvas = this.transform.GetComponent<Canvas>();
        canvas.worldCamera = GameObject.FindWithTag("MainCamera").GetComponent<Camera>();

        moveSpeed = 0.01f; //위로 움직이는 속도값

        destroyTime = 1.2f; //몇초 후 삭제 될건지
    }

    void Update()
    {
        //vector.Set(FloatTextPrint.transform.position.x, FloatTextPrint.transform.position.y
        //    + (moveSpeed + Time.deltaTime), FloatTextPrint.transform.position.z);

        //FloatTextPrint.transform.position = vector;

        transform.LookAt(canvas.worldCamera.transform);

        FloatTextPrint.transform.position = new Vector3(FloatTextPrint.transform.position.x,
            FloatTextPrint.transform.position.y + (moveSpeed + Time.deltaTime),
            FloatTextPrint.transform.position.z);

        destroyTime -= Time.deltaTime;

        if (destroyTime <= 0)
        {
            Destroy(this.gameObject);
        }
    }
}
