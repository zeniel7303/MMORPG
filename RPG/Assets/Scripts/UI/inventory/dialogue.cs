using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class dialogue : MonoBehaviour
{
    public CanvasGroup m_dial;
    public Text m_name;
    public Text m_mainText;
    public string m_npcName = "npc1";

    public GameObject m_questCheckWindow;
    public GameObject m_questButton;
    public Text m_questTitle;
    public Text m_questText;

    public PlayerManager m_player;

    public npcDialogue _npcDialogue;

    // Start is called before the first frame update
    void Start()
    {
        m_dial = GetComponent<CanvasGroup>();

        m_questCheckWindow.SetActive(false);
        m_questButton.SetActive(false);

        OnDialogueOpen(false);
    }

    // Update is called once per frame
    void Update()
    {
        ////npc 텍스트 출력
        //if(m_npcName == "npc1")
        //{
        //    m_name.text = _npcDialogue.lNdc[0].Name;
        //    m_mainText.text = _npcDialogue.lNdc[0].Dial1 + _npcDialogue.lNdc[0].Dial2;
        //}
        //else if(m_npcName == "npc2")
        //{
        //    m_name.text = _npcDialogue.lNdc[1].Name;
        //    m_mainText.text = _npcDialogue.lNdc[1].Dial1 + _npcDialogue.lNdc[1].Dial2;
        //}
    }

    public void OnDialogueOpen(bool isOpened)
    {
        m_dial.alpha = (isOpened) ? 1.0f : 0.0f;
        m_dial.interactable = isOpened;
        m_dial.blocksRaycasts = isOpened;
        m_player.isTalk = isOpened;
    }

    public void CloseClick()
    {
        Debug.Log("닫기 눌림");
        OnDialogueOpen(false);
        m_questCheckWindow.SetActive(false);
        m_questButton.SetActive(false);
        m_player.isTalk = false;
        m_player.unitInfo.state = STATE.IDLE;

        Cursor.lockState = CursorLockMode.Locked;//마우스 커서 고정
        Cursor.visible = false;//마우스 커서 보이기
    }

    public void QuestClick()
    {
        Debug.Log("퀘스트창 눌림");

        m_questCheckWindow.SetActive(true);
    }

    public void OkClick()
    {
        m_questCheckWindow.SetActive(false);

        //_myQuest.Save();
    }

    public void NoClick()
    {
        m_questCheckWindow.SetActive(false);
    }
}
