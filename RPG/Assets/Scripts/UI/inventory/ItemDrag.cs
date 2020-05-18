using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class ItemDrag : MonoBehaviour
{

    public Transform _img;   // 빈 이미지 객체.

    private Image _emptyImg; // 빈 이미지.
    private Slot _slot;      // 현재 슬롯에 스크립트

    void Start()
    {
        // 현재 슬롯의 스크립트를 가져온다.
        _slot = GetComponent<Slot>();
        // 빈 이미지 객체를 태그를 이용하여 가져온다.
        _img = GameObject.FindGameObjectWithTag("DragImg").transform;
        // 빈 이미지 객체가 가진 Image컴포넌트를 가져온다.
        _emptyImg = _img.GetComponent<Image>();
    }

    public void Down()
    {
        // 슬롯에 아이템이 없으면 함수종료.
        if (!_slot.isSlots())
            return;

        // 아이템 사용시.
        if (Input.GetMouseButtonDown(1))
        {
            _slot.ItemUse();
            return;
        }

        // 빈 이미지 객체를 활성화 시킨다.
        _img.gameObject.SetActive(true);

        // 빈 이미지의 사이즈를 변경한다.(해상도가 바뀔경우를 대비.)
        float Size = _slot.transform.GetComponent<RectTransform>().sizeDelta.x;
        _emptyImg.rectTransform.SetSizeWithCurrentAnchors(RectTransform.Axis.Horizontal, Size);
        _emptyImg.rectTransform.SetSizeWithCurrentAnchors(RectTransform.Axis.Vertical, Size);

        // 빈 이미지의 스프라이트를 슬롯의 스프라이트로 변경한다.
        _emptyImg.sprite = _slot.ItemReturn()._defaultImg;
        // 빈 이미지의 위치를 마우스위로 가져온다.
        _img.transform.position = Input.mousePosition;
        // 슬롯의 아이템 이미지를 없애준다.
        _slot.UpdateInfo(true, _slot._defaultImg);
        // 슬롯의 텍스트 숫자를 없애준다.
        _slot._text.text = "";
    }

    public void Drag()
    {
        // isImg플래그가 false이면 슬롯에 아이템이 존재하지 않는 것이므로 함수 종료.
        if (!_slot.isSlots())
            return;

        _img.transform.position = Input.mousePosition;
    }

    public void DragEnd()
    {
        // isImg플래그가 false이면 슬롯에 아이템이 존재하지 않는 것이므로 함수 종료.
        if (!_slot.isSlots())
            return;

        // 싱글톤을 이용해서 인벤토리의 스왑함수를 호출(현재 슬롯, 빈 이미지의 현재 위치.)
        ObjManager.Call().IV.Swap(_slot, _img.transform.position);
        //slot = null;
    }

    public void Up()
    {
        // isImg플래그가 false이면 슬롯에 아이템이 존재하지 않는 것이므로 함수 종료.
        if (!_slot.isSlots())
            return;

        // 빈 이미지 객체 비활성화.
        _img.gameObject.SetActive(false);
        // 슬롯의 아이템 이미지를 복구 시킨다.
        _slot.UpdateInfo(true, _slot._slot.Peek()._defaultImg);
    }
}
