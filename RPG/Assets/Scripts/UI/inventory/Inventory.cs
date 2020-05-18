using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using System.Xml.Serialization;

//참고
//https://truecode.tistory.com/74?category=611884

public class Inventory : MonoBehaviour
{
    // 공개
    public List<GameObject> _lAllSlot;    // 모든 슬롯을 관리해줄 리스트.
    public RectTransform _invenRect;     // 인벤토리의 Rect
    public GameObject _originSlot;       // 오리지널 슬롯.

    public float _slotSize;              // 슬롯의 사이즈.
    public float _slotGap;               // 슬롯간 간격.
    public float _slotCountX;            // 슬롯의 가로 개수.
    public float _slotCountY;            // 슬롯의 세로 개수.

    // 비공개.
    private float _invenWidth;           // 인벤토리 가로길이.
    private float _invenHeight;          // 인밴토리 세로길이.

    public CanvasGroup m_inv;

    public bool m_isOpened = false;

    void Awake()
    {
        m_inv = GetComponent<CanvasGroup>();

        // 인벤토리 이미지의 가로, 세로 사이즈 셋팅.
        _invenWidth = (_slotCountX * _slotSize) + (_slotCountX * _slotGap) + _slotGap;
        _invenHeight = (_slotCountY * _slotSize) + (_slotCountY * _slotGap) + _slotGap;

        // 셋팅된 사이즈로 크기를 설정.
        _invenRect.SetSizeWithCurrentAnchors(RectTransform.Axis.Horizontal, _invenWidth); // 가로.
        _invenRect.SetSizeWithCurrentAnchors(RectTransform.Axis.Vertical, _invenHeight);  // 세로.

        // 슬롯 생성하기.
        for (int y = 0; y < _slotCountY; y++)
        {
            for (int x = 0; x < _slotCountX; x++)
            {
                // 슬롯을 복사한다.
                GameObject slot = Instantiate(_originSlot) as GameObject;
                // 슬롯의 RectTransform을 가져온다.
                RectTransform slotRect = slot.GetComponent<RectTransform>();
                // 슬롯의 자식인 투명이미지의 RectTransform을 가져온다.
                RectTransform item = slot.transform.GetChild(0).GetComponent<RectTransform>();

                slot.name = "slot_" + y + "_" + x;   // 슬롯 이름 설정.
                slot.transform.SetParent(transform); // 슬롯의 부모를 설정. (Inventory객체가 부모임.)

                // 슬롯이 생성될 위치 설정하기.
                slotRect.localPosition = new Vector3((_slotSize * x) + (_slotGap * (x + 1)) - 135,
                                                   -((_slotSize * y) + (_slotGap * (y + 1))) + 135,
                                                      0);

                // 슬롯의 자식인 투명이미지의 사이즈 설정하기.
                slotRect.localScale = Vector3.one;
                slotRect.SetSizeWithCurrentAnchors(RectTransform.Axis.Horizontal, _slotSize); // 가로
                slotRect.SetSizeWithCurrentAnchors(RectTransform.Axis.Vertical, _slotSize);   // 세로.

                // 슬롯의 사이즈 설정하기.
                item.SetSizeWithCurrentAnchors(RectTransform.Axis.Horizontal, _slotSize - _slotSize * 0.3f); // 가로.
                item.SetSizeWithCurrentAnchors(RectTransform.Axis.Vertical, _slotSize - _slotSize * 0.3f);   // 세로.

                // 리스트에 슬롯을 추가.
                _lAllSlot.Add(slot);
            }
        }

        // 빈 슬롯 = 슬롯의 숫자.
        //EmptySlot = AllSlot.Count;
        Invoke("Init", 0.01f);
    }

    void Init()
    {
        //ItemIO.Load(AllSlot);
    }

    // 아이템을 넣기위해 모든 슬롯을 검사.
    public bool AddItem(Item item)
    {
        // 슬롯에 총 개수.
        int slotCount = _lAllSlot.Count;

        // 넣기위한 아이템이 슬롯에 존재하는지 검사.
        for (int i = 0; i < slotCount; i++)
        {
            // 그 슬롯의 스크립트를 가져온다.
            Slot slot = _lAllSlot[i].GetComponent<Slot>();

            // 슬롯이 비어있으면 통과.
            if (!slot.isSlots())
                continue;

            // 슬롯에 존재하는 아이템의 타입과 넣을려는 아이템의 타입이 같고.
            // 슬롯에 존재하는 아이템의 겹칠수 있는 최대치가 넘지않았을 때. (true일 때)
            if (slot.ItemReturn()._type == item._type && slot.ItemMax(item))
            {
                // 슬롯에 아이템을 넣는다.
                slot.AddItem(item);
                return true;
            }
        }

        // 빈 슬롯에 아이템을 넣기위한 검사.
        for (int i = 0; i < slotCount; i++)
        {
            Slot slot = _lAllSlot[i].GetComponent<Slot>();

            // 슬롯이 비어있지 않으면 통과
            if (slot.isSlots())
                continue;

            slot.AddItem(item);
            return true;
        }

        // 위에 조건에 해당되는 것이 없을 때 아이템을 먹지 못함.
        return false;
    }

    // 거리가 가까운 슬롯의 반환.
    public Slot NearDisSlot(Vector3 Pos)
    {
        float Min = 10000f;
        int Index = -1;

        int Count = _lAllSlot.Count;
        for (int i = 0; i < Count; i++)
        {
            Vector2 sPos = _lAllSlot[i].transform.GetChild(0).position;
            float Dis = Vector2.Distance(sPos, Pos);

            if (Dis < Min)
            {
                Min = Dis;
                Index = i;
            }
        }

        if (Min > _slotSize)
            return null;

        return _lAllSlot[Index].GetComponent<Slot>();
    }

    // 아이템 옮기기 및 교환.
    public void Swap(Slot slot, Vector3 Pos)
    {
        Slot FirstSlot = NearDisSlot(Pos);

        // 현재 슬롯과 옮기려는 슬롯이 같으면 함수 종료.
        if (slot == FirstSlot || FirstSlot == null)
        {
            slot.UpdateInfo(true, slot._slot.Peek()._defaultImg);
            return;
        }

        // 가까운 슬롯이 비어있으면 옮기기.
        if (!FirstSlot.isSlots())
        {
            Swap(FirstSlot, slot);
        }
        // 교환.
        else
        {
            int Count = slot._slot.Count;
            Item item = slot._slot.Peek();
            Stack<Item> temp = new Stack<Item>();

            {
                for (int i = 0; i < Count; i++)
                    temp.Push(item);

                slot._slot.Clear();
            }

            Swap(slot, FirstSlot);

            {
                Count = temp.Count;
                item = temp.Peek();

                for (int i = 0; i < Count; i++)
                    FirstSlot._slot.Push(item);

                FirstSlot.UpdateInfo(true, temp.Peek()._defaultImg);
            }
        }
    }

    // 1: 비어있는 슬롯, 2: 안 비어있는 슬롯.
    void Swap(Slot xFirst, Slot oSecond)
    {
        int Count = oSecond._slot.Count;
        Item item = oSecond._slot.Peek();

        for (int i = 0; i < Count; i++)
        {
            if (xFirst != null)
                xFirst._slot.Push(item);
        }

        if (xFirst != null)
            xFirst.UpdateInfo(true, oSecond.ItemReturn()._defaultImg);

        oSecond._slot.Clear();
        oSecond.UpdateInfo(false, oSecond._defaultImg);
    }

    void Update()
    {
        if (Input.GetKeyDown(KeyCode.I))
        {
            if (m_isOpened)
            {
                m_isOpened = false;

                Cursor.lockState = CursorLockMode.Locked;
                Cursor.visible = false;
            }
            else
            {
                m_isOpened = true;

                Cursor.lockState = CursorLockMode.None;
                Cursor.visible = true;
            }
        }

        OnInventoryOpen(m_isOpened);
    }

    public void OnInventoryOpen(bool isOpened)
    {
        m_inv.alpha = (isOpened) ? 1.0f : 0.0f;
        m_inv.interactable = isOpened;
        m_inv.blocksRaycasts = isOpened;
    }
}
