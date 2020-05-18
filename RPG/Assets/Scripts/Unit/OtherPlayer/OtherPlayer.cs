using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class OtherPlayer : Unit
{
    [HideInInspector]
    public UserInfo userInfo;

    private Transform myTransform;
    private Transform playerTransform;

    private Animator animator;
    public bool isMove;
    public bool isMoveFinish;

    public bool isVisible;

    private Vector2 targetPosition;
    private Vector3 targetDirection;

    public float moveSpeed = 3.0f;

    public GameObject HUD;
    public EnemyInfoUI infoUI;

    public FloatingChatting floatingChattingPrefab;
    private FloatingChatting floatingChatting;

    public GameObject userMesh;

    //this.transform.forward 부분 전부 임시 주석처리함(Test위함)

    private void Awake()
    {
        HUD = transform.Find("HUD").gameObject;
        HUD.GetComponent<Canvas>().worldCamera =
            GameObject.FindWithTag("MainCamera").GetComponent<Camera>();

        floatingChatting = FloatingChatting.Instantiate(floatingChattingPrefab);
        floatingChatting.transform.SetParent(this.transform);
        floatingChatting.transform.localPosition = Vector3.zero;

        animator = this.GetComponent<Animator>();
        myTransform = this.transform;
        isMove = false;
    }

    // Update is called once per frame
    void Update()
    {
        if(isVisible)
        {
            if (!userMesh.activeSelf)
            {
                userMesh.SetActive(true);
                GameManager.Instance.mapManager.AddVisiblePlayer(this);
            }
        }
        else
        {
            if (userMesh.activeSelf)
            {
                userMesh.SetActive(false);
                GameManager.Instance.mapManager.RemoveVisiblePlayer(this);
            }
        }

        /*if (Vector3.SqrMagnitude(myTransform.position - playerTransform.position) >
            40.0f * 40.0f)
        {
            if (userMesh.activeSelf)
            {
                userMesh.SetActive(false);
                GameManager.Instance.mapManager.RemoveVisiblePlayer(this);
            }
        }
        else
        {
            if (!userMesh.activeSelf)
            {
                userMesh.SetActive(true);
                GameManager.Instance.mapManager.AddVisiblePlayer(this);
            }
        }*/

        infoUI.transform.LookAt(HUD.GetComponent<Canvas>().worldCamera.transform);

        infoUI.enemyPosition.text =
            string.Format("{0} / {1}", this.transform.position.x, this.transform.position.z);

        infoUI.enemyTargetPosition.text =
            string.Format("{0} / {1}", targetPosition.x, targetPosition.y);

        if (!isMove)
        {
            return;
        }

        this.transform.Translate(Vector3.forward * moveSpeed * Time.deltaTime);

        if (Vector2.SqrMagnitude(new Vector2(myTransform.position.x, myTransform.position.z)
            - targetPosition) < 0.1f)
        {
            Vector3 characterPosition = myTransform.position;
            characterPosition.x = targetPosition.x;
            characterPosition.z = targetPosition.y;
            myTransform.position = characterPosition;

            this.transform.forward = targetDirection;

            animator.SetBool("Move", false);
            isMove = false;
        }
    }

    public void Init(UnitInfo _unitInfo, UserInfo _userInfo, Transform _playerTransform)
    {
        playerTransform = _playerTransform;

        unitInfo = _unitInfo;
        userInfo = _userInfo;

        string playerName = userInfo.name;
        infoUI.enemyName.text = playerName;
        infoUI.index.text = _userInfo.userID.ToString();

        Position syncPosition = unitInfo.position;

        Vector3 respawnPosition = new Vector3(
             syncPosition.position.x, 1.0f, syncPosition.position.z);

        myTransform.position = respawnPosition;

        this.transform.forward = new Vector3(syncPosition.direction.x, 0.0f, syncPosition.direction.z);

        userMesh.SetActive(false);
        isVisible = false;
    }

    public void Release(UnitInfo _unitInfo, UserInfo _userInfo)
    {
        animator.SetTrigger("Revive");

        unitInfo = _unitInfo;

        Position syncPosition = unitInfo.position;

        Vector3 respawnPosition = new Vector3(
             syncPosition.position.x, 1.0f, syncPosition.position.z);

        myTransform.position = respawnPosition;

        if (Vector3.SqrMagnitude(myTransform.position - playerTransform.position) >
            40.0f * 40.0f)
        {
            userMesh.SetActive(false);
            GameManager.Instance.mapManager.RemoveVisiblePlayer(this);
        }
        else
        {
            userMesh.SetActive(true);
            GameManager.Instance.mapManager.AddVisiblePlayer(this);
        }
    }

    public void BeginMove(Vector _position, Vector _direction)
    {
        //Debug.Log("StartMove");

        unitInfo.state = STATE.MOVE;

        Vector2 compareVector = new Vector2(myTransform.position.x, myTransform.position.z);
        Vector2 pathPosition = new Vector2(_position.x, _position.z);

        if (Vector2.SqrMagnitude(compareVector - pathPosition) > 10.0f * 10.0f)
        {
            Vector3 respawnPosition = new Vector3(
                pathPosition.x, 10.0f, pathPosition.y);

            return;
        }

        Vector3 position = myTransform.position;
        position.x = _position.x;
        position.z = _position.z;

        Vector3 direction = new Vector3(_direction.x, 0.0f, _direction.z);
        Vector3 nextPosition = position + (direction * 1.5f * moveSpeed);

        //Debug.Log("targetDirection : " + direction);

        targetPosition = new Vector2(nextPosition.x, nextPosition.z);

        TargetLookAt(new Vector3(targetPosition.x, this.transform.position.y, targetPosition.y));

        animator.SetBool("Move", true);
        isMove = true;
    }

    public void EndMove(Vector _position, Vector _direction)
    {
        //Debug.Log("EndMove");

        targetPosition = new Vector2(_position.x, _position.z);
        targetDirection = new Vector3(_direction.x, 0.0f, _direction.z);

        //Debug.Log("targetDirection : " + targetDirection);

        if (Vector2.SqrMagnitude(new Vector2(myTransform.position.x, myTransform.position.z)
            - targetPosition) < 0.1f)
        {
            this.transform.forward = targetDirection;

            Vector3 characterPosition = myTransform.position;
            characterPosition.x = targetPosition.x;
            characterPosition.z = targetPosition.y;
            myTransform.position = characterPosition;

            unitInfo.state = STATE.IDLE;

            animator.SetBool("Move", false);
            isMove = false;
        }
        else
        {
            TargetLookAt(new Vector3(targetPosition.x, this.transform.position.y, targetPosition.y));
        }
    }

    public void Attack(Vector _direction)
    {
        targetDirection = new Vector3(_direction.x, 0.0f, _direction.z);
        this.transform.forward = targetDirection;

        animator.SetTrigger("BasicAttack");
    }

    public void Hit(int _damage)
    {

    }

    public void Death()
    {
        animator.SetTrigger("Death");

        GameManager.Instance.mapManager.RemoveVisiblePlayer(this);
    }

    public void Chatting(string _chatting)
    {
        floatingChatting.Floating(_chatting, true);
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
