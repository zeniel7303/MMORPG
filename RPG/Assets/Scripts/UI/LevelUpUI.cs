using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class LevelUpUI : MonoBehaviour
{
    public Text LevelText;
    private float time;

    private bool isActivated;

    private void Awake()
    {
        this.gameObject.SetActive(false);
        isActivated = false;

        time = 0.0f;
    }

    public void LevelUp()
    {
        isActivated = true;
        this.gameObject.SetActive(true);

        int currentLevel = PlayerManager.instance.unitInfo.level;

        LevelText.text = string.Format("{0} → {1}", currentLevel - 1, currentLevel);
    }

    private void Update()
    {
        if (!isActivated) return;

        time += Time.deltaTime;

        if(time >= 3.0f)
        {
            time = 0;

            this.gameObject.SetActive(false);
            isActivated = false;
        }
    }
}