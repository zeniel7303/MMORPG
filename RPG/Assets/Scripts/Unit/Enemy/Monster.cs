using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public struct MonsterInfo
{
    public int index;
    public ushort monsterType;
    
    public STATE state;
    public ZeroToMax hp;
    public Vector position;
}

public class Monster : MonoBehaviour
{
    [HideInInspector]
    public MonsterInfo monsterInfo;

    private Transform myTransform;
    private Transform playerTransform;

    private Animator animator;
    private Rigidbody rigidbody;

    public GameObject monsterMesh;

    public GameObject HUD;
    public GameObject DamagePivot;

    public EnemyInfoUI monsterInfoUI;

    public List<Vector2> pathList;
    public float moveSpeed;

    public float playerDistance;

    public bool isDeath = false;
    public bool isInHitInterval = false;

    public float HitInterval = 0.0f;

    public bool isVisible;

    // Start is called before the first frame update
    private void Awake()
    {
        //HUD = transform.Find("HUD").gameObject;
        HUD.GetComponent<Canvas>().worldCamera = 
            GameObject.FindWithTag("MainCamera").GetComponent<Camera>();

        DamagePivot = transform.Find("DamagePivot").gameObject;

        animator = this.GetComponent<Animator>();
        rigidbody = this.GetComponent<Rigidbody>();
        myTransform = this.transform;

        playerDistance = 0.0f;
    }

    // Update is called once per frame
    void Update()
    {
        if(isVisible)
        {
            if (!monsterMesh.activeSelf)
            {
                monsterMesh.gameObject.SetActive(true);
                GameManager.Instance.mapManager.AddVisibleMonster(this);
            }
        }
        else
        {
            if (monsterMesh.activeSelf)
            {
                monsterMesh.gameObject.SetActive(false);
                GameManager.Instance.mapManager.RemoveVisibleMonster(this);
            }
        }

        if (monsterInfo.hp.currentValue < 0) monsterInfo.hp.currentValue = 0;

        //UI부분
        monsterInfoUI.transform.LookAt(HUD.GetComponent<Canvas>().worldCamera.transform);

        monsterInfoUI.enemyHPBar.maxValue = monsterInfo.hp.maxValue;
        monsterInfoUI.enemyHPBar.value = monsterInfo.hp.currentValue;
        monsterInfoUI.enemyHPText.text =
            string.Format("{0} / {1}", monsterInfo.hp.currentValue, monsterInfo.hp.maxValue);

        if(isDeath)
        {
            if (animator.GetCurrentAnimatorStateInfo(0).IsName("Death") &&
                animator.GetCurrentAnimatorStateInfo(0).normalizedTime >= 0.9f)
            {
                this.gameObject.SetActive(false);
            }

            return;
        }

        if(isInHitInterval)
        {
            HitInterval += Time.deltaTime;

            if(HitInterval >= 3.0f)
            {
                HitInterval = 0.0f;
                isInHitInterval = false;
            }
        }

        if (this.transform.position.y <= -5.0f)
        {
            rigidbody.useGravity = false;   

            Vector3 vec = new Vector3(monsterInfo.position.x,
                                  1.0f, monsterInfo.position.z);

            this.transform.position = vec;

            rigidbody.useGravity = true;
        }

        Moving();
    }

    public void Init(MonsterInfo _info, Transform _playerTransform)
    {
        playerTransform = _playerTransform;

        monsterInfo = _info;

        Vector3 respawnPosition = new Vector3(
             monsterInfo.position.x, 1.0f, monsterInfo.position.z);

        myTransform.position = respawnPosition;

        monsterMesh.SetActive(false);
        isVisible = false;

        monsterInfoUI.index.text = monsterInfo.index.ToString();
    }

    public void Release(MonsterInfo _info)
    {
        this.gameObject.SetActive(true);

        monsterInfo = _info;

        Vector3 respawnPosition = new Vector3(
             monsterInfo.position.x, 1.0f, monsterInfo.position.z);

        myTransform.position = respawnPosition;

        if (Vector3.SqrMagnitude(myTransform.position - playerTransform.position) >
            40.0f * 40.0f)
        {
            monsterMesh.SetActive(false);
            GameManager.Instance.mapManager.RemoveVisibleMonster(this);
        }
        else
        {
            monsterMesh.SetActive(true);
            GameManager.Instance.mapManager.AddVisibleMonster(this);
        }

        isDeath = false;
    }

    public void PathListClear()
    {
        pathList.Clear();
    }

    public void Move(Vector _position)
    {
        Vector2 targetPositon = new Vector2(_position.x, _position.z);

        if (pathList.Count == 0)
            TargetLookAt(new Vector3(targetPositon.x, this.transform.position.y, targetPositon.y));

        pathList.Add(targetPositon);
    }

    public void Moving()
    {
        if (pathList.Count == 0)
        {
            animator.SetBool("Move", false);
            return;
        }

        animator.SetBool("Move", true);

        Vector2 movePosition = Vector2.MoveTowards(
           new Vector2(myTransform.position.x, myTransform.position.z), pathList[0],
           moveSpeed * Time.deltaTime);

        Vector3 position = myTransform.position;
        position.x = movePosition.x;
        position.z = movePosition.y;
        myTransform.position = position;

        if (Vector2.SqrMagnitude(
                pathList[0] - movePosition) < 0.1f)
        {
            pathList.RemoveAt(0);

            if (pathList.Count != 0)
                TargetLookAt(new Vector3(pathList[0].x, this.transform.position.y, pathList[0].y));
        }
    }

    public void Hit(int _damage, int _userIndex)
    {
        //Debug.Log("Hit 함수 진입");

        monsterInfo.hp.currentValue -= _damage;

        //Debug.Log(_damage.ToString() + "데미지 입음");

        UIManager.Instance.SetFloatingText(DamagePivot, _damage.ToString(), true);

        //Debug.Log("데미지 텍스트 출력");

        if (PlayerManager.instance.userInfo.userID == _userIndex)
        {
            //이펙트같은거
        }

        if (monsterInfo.hp.currentValue <= 0)
        {
            Debug.Log("사망");

            Death();

            return;
        }

        //계속 Hit 애니메이션 출력하는걸 방지하기위함.
        if (!isInHitInterval)
        {
            isInHitInterval = true;

            animator.SetTrigger("Hit");
        }
    }

    public void Death()
    {
        animator.SetTrigger("Death");

        GameManager.Instance.mapManager.RemoveVisibleMonster(this);
        PathListClear();

        isDeath = true;
    }

    public void Attack(Vector3 _from)
    {
        animator.SetTrigger("Attack");

        TargetLookAt(_from);
    }

    public void TargetLookAt(Vector3 _from)
    {
        myTransform.rotation =
            Quaternion.Euler(
                myTransform.eulerAngles.x,
                GetDegree(myTransform.position, _from),
                myTransform.eulerAngles.z);
    }

    float GetDegree(Vector3 _to, Vector3 _from)
    {
        return Mathf.Atan2(_to.x - _from.x, _to.z - _from.z) * 180.0f / Mathf.PI - 180.0f;
    }
}
