using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class StatUI : MonoBehaviour
{
    public Text nameValue;
    public Text hpValue;
    public Text mpValue;
    public Text atkValue;
    public Text defValue;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        nameValue.text = PlayerManager.instance.userInfo.name;
        hpValue.text = string.Format("{0} / {1}", PlayerManager.instance.unitInfo.hp.currentValue, PlayerManager.instance.unitInfo.hp.maxValue);
        mpValue.text = string.Format("{0} / {1}", PlayerManager.instance.unitInfo.mp.currentValue, PlayerManager.instance.unitInfo.mp.maxValue);
        atkValue.text = PlayerManager.instance.unitInfo.atk.ToString();
        defValue.text = PlayerManager.instance.unitInfo.def.ToString();
    }
}