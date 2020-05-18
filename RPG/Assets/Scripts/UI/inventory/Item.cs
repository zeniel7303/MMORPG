using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Item : MonoBehaviour
{
    public enum TYPE { HP, MP }

    public TYPE _type;           // 아이템의 타입.
    public Sprite _defaultImg;   // 기본 이미지.
    public int _maxCount;        // 겹칠수 있는 최대 숫자.


    void AddItem()
    {
        // 싱글톤을 이용해서 인벤토리 스크립트를 가져온다.
        Inventory iv = ObjManager.Call().IV;

        // 아이템 획득에 실패할 경우.
        if (!iv.AddItem(this))
            Debug.Log("아이템이 가득 찼습니다.");
        else // 아이템 획득에 성공할 경우.
            gameObject.SetActive(false); // 아이템을 비활성화 시켜준다.
    }

    // 충돌체크
    void OnTriggerEnter(Collider _col)
    {
        // 플레이어와 충돌하면.
        if (_col.gameObject.tag == "Player")
            AddItem();
    }
}
