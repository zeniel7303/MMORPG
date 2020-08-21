using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using System.Text;

public class ChattingInfo
{
    public string id;
    public string text;
    public bool isWhisper;
    public bool isMe;

    public ChattingInfo(string _id, string _text, bool _isWhisper, bool _isMe)
    {
        id = _id;
        text = _text;
        isWhisper = _isWhisper;
        isMe = _isMe;
    }
}

public class Chatting : MonoBehaviour
{
    private CircBuf<ChattingInfo> chattingQueue;

    public ChattingInputField inputField;

    private Text[] textArray;

    public RectTransform chattingSpace;

    public Text textPrefabs;

    public int maxChattingSize;
    private int nowChattingCount;

    public bool isActivated;

    // Start is called before the first frame update
    void Start()
    {
        chattingQueue = new CircBuf<ChattingInfo>();
        chattingQueue.Init(maxChattingSize, new ChattingInfo("", "", false, false));

        textArray = new Text[maxChattingSize];

        for (int i = 0; i < maxChattingSize; i++)
        {
            textArray[i] = Text.Instantiate(textPrefabs);
            textArray[i].transform.SetParent(chattingSpace);
            textArray[i].transform.localScale = Vector3.one;
            textArray[i].text = "";
        }

        chattingSpace.localPosition = new Vector3(0, 28);

        nowChattingCount = 0;

        inputField.SetOnEndEdit(ChattingInput);

        isActivated = false;

        GameManager.Instance.chattingUI = this;
    }

    private void Update()
    {
        //Debug.Log(isActivated);

        if (Input.GetKeyDown(KeyCode.Return))
        {
            if(isActivated == true)
            {
                inputField.DeactivateInputField();
                isActivated = false;
            }
            else if (isActivated == false)
            {
                inputField.ActivateInputField();

                isActivated = true;
            }

            //inputField.ActivateInputField();
        }
    }

    public void ChattingInput(string _value)
    {
        if (_value == "")
            return;

        int index = _value.IndexOf("/w");

        if(index == 0)
        {
            _value += "\0";

            string[] stringArray = _value.Split(' ');
            string newString = _value.Remove(0, stringArray[0].Length + stringArray[1].Length + 2);

            ChattingPacket_Whisper chattingPacket_Whisper =
                 new ChattingPacket_Whisper(PlayerManager.instance.userInfo.userID,
                stringArray[1], PlayerManager.instance.userInfo.name, newString);

            ServerManager.Instance.SendData_ZoneServer(chattingPacket_Whisper.GetBytes());

            return;
        }

        _value += "\0";

        ChattingPacket chattingPacket =
                 new ChattingPacket(PlayerManager.instance.userInfo.userID,
                 PlayerManager.instance.userInfo.name, _value);

        ServerManager.Instance.SendData_ZoneServer(chattingPacket.GetBytes());
    }

    public void SystemChattingInput(string _value)
    {
        if (_value == "") return;

        chattingQueue.Enqueue(new ChattingInfo("System ", _value, false, false));

        ShowChatting();

        nowChattingCount++;

        if (nowChattingCount >= 12)
        {
            chattingSpace.localPosition = new Vector3(0, (nowChattingCount - 12) * 28 + 28);
        }
    }

    public void ChattingInput(int _userIndex, string _id, string _value)
    {
        if (_value == "") return;

        string userName;
        if (_userIndex == PlayerManager.instance.userInfo.userID)
        {
            userName = PlayerManager.instance.userInfo.name;
        }
        //테스트용
        else if (_userIndex >= 10000)
        {
            userName = _id;
        }
        else if (GameManager.Instance.mapManager == null ||
            !GameManager.Instance.mapManager.otherPlayersDic.ContainsKey(_userIndex))
        {
            return;
        }
        else
        {
            userName = _id;
        }

        chattingQueue.Enqueue(new ChattingInfo(userName, _value, false, false));

        ShowChatting();

        nowChattingCount++;

        if (nowChattingCount >= 12)
        {
            chattingSpace.localPosition = new Vector3(0, (nowChattingCount - 12) * 28 + 28);
        }
    }

    public void ChattingInput_Whisper(int _userIndex, string _targetId, string _id, string _value)
    {
        if (_value == "") return;

        string userName;
        if (_userIndex == PlayerManager.instance.userInfo.userID)
        {
            userName = _targetId;

            chattingQueue.Enqueue(new ChattingInfo(userName, _value, true, true));
        }
        else if (GameManager.Instance.mapManager == null ||
            !GameManager.Instance.mapManager.otherPlayersDic.ContainsKey(_userIndex))
        {
            return;
        }
        else
        {
            userName = _id;

            chattingQueue.Enqueue(new ChattingInfo(userName, _value, true, false));
        }

        ShowChatting();

        nowChattingCount++;

        if (nowChattingCount >= 12)
        {
            chattingSpace.localPosition = new Vector3(0, (nowChattingCount - 12) * 28 + 28);
        }
    }

    //public void InputfieldRefocus()
    //{
    //    inputField.Select();
    //    inputField.ActivateInputField();
    //}

    public void ShowChatting()
    {
        for (int i = 0; i < maxChattingSize; i++)
        {
            if (chattingQueue.GetQueueParameter(i).id == "")
                continue;

            if (chattingQueue.GetQueueParameter(i).isWhisper)
            {
                if(chattingQueue.GetQueueParameter(i).isMe)
                {
                    textArray[i].text = string.Format("<color=#50fd50>[{0}]</color> << {1}",
                    chattingQueue.GetQueueParameter(i).id,
                    chattingQueue.GetQueueParameter(i).text);
                }
                else
                {
                    textArray[i].text = string.Format("<color=#50fd50>[{0}]</color> >> {1}",
                    chattingQueue.GetQueueParameter(i).id,
                    chattingQueue.GetQueueParameter(i).text);
                }
            }
            else
            {
                textArray[i].text = string.Format("[{0}] : {1}",
                chattingQueue.GetQueueParameter(i).id,
                chattingQueue.GetQueueParameter(i).text);
            }
        }
    }

    public void IsActivatedTrue()
    {
        isActivated = true;
    }

    public void IsActivatedFalse()
    {
        isActivated = false;
    }
}
