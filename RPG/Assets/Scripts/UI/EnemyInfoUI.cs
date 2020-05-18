using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class EnemyInfoUI : MonoBehaviour
{
    public Slider enemyHPBar;
    public Text enemyHPText;
    public Text enemyName;

    public Text enemyPosition;
    public Text enemyTargetPosition;
    public Text index;

    public GameObject HeadUpPosition;

    // Use this for initialization
    void Start()
    {
        
    }

    //HP게이지를 업데이트 하고, HUD위치에 맞게 HP게이지를 그리게 한다
    //HUD객체에서 월드상의 좌표값을 얻어온다.
    void Update()
    {
        this.transform.position = HeadUpPosition.transform.position;
    }
}
