using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

public class ChattingInputField : InputField
{
    protected override void Awake()
    {
        onEndEdit.AddListener(EndEnter);
    }

    public void SetOnEndEdit(UnityEngine.Events.UnityAction<string> call)
    {
        onEndEdit.AddListener(call);
    }

    public void EndEnter(string _value)
    {
        m_Text = "";
    }

    public override void OnDeselect(BaseEventData eventData)
    {
        this.DeactivateInputField();
        base.OnDeselect(eventData);
    }
}
