using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class Teleport : MonoBehaviour
{
    public string   sceneName;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    private void OnTriggerEnter(Collider other)
    {
        //SceneManager.LoadScene(sceneName);

        if(other.tag == "Player")
        {
            Debug.Log("포탈에 닿음");

            if (sceneName == "Village")
            {
                FieldNumPacket EnterVillagePacket =
                    new FieldNumPacket(SendCommand.C2Zone_ENTER_VILLAGE, 1);

                ServerManager.Instance.SendData_ZoneServer(EnterVillagePacket.GetBytes());
            }
            else if (sceneName == "Field")
            {
                FieldNumPacket EnterFieldPacket =
                    new FieldNumPacket(SendCommand.C2Zone_ENTER_FIELD, 2);

                ServerManager.Instance.SendData_ZoneServer(EnterFieldPacket.GetBytes());
            }
            else if (sceneName == "TestScene")
            {
                FieldNumPacket EnterTestFieldPacket =
                    new FieldNumPacket(SendCommand.C2Zone_ENTER_TESTFIELD, 3);

                ServerManager.Instance.SendData_ZoneServer(EnterTestFieldPacket.GetBytes());
            }
        }
    }
}
