using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerMovement : MonoBehaviour
{
    [HideInInspector]
    public PlayerManager myManager;

    public Transform playerMesh;

    public Tile myTile;

    public float moveSpeed = 3.5f;
    public float rotationSpeed = 4.0f;
    private Vector3 inputVec, targetDirection;

    private float sendTime;
    public float sendInterval = 0.2f;

    private bool isStop;

    private bool isMove_;
    public bool isMove
    {
        get { return isMove_; }
        set
        {
            if (isMove_ == value) return;

            isMove_ = value;

            SendPlayerPosition();

            sendTime = 0.01f;
        }
    }

    public bool isSceneMove;

    // Start is called before the first frame update
    void Start()
    {
        Init();
    }

    public void Init()
    {
        playerMesh = GameObject.Find("Mesh").transform;
        myTile = PathFinder.GetTile(this.transform.position);

        isMove = false;
        isStop = false;
        isSceneMove = false;

        myManager.playerAnimation.CharacterMove(false);

        myManager.unitInfo.state = STATE.IDLE;
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void Stay()
    {
        isMove = false;
        isStop = true;
    }

    public void Rollback()
    {
        isStop = false;
    }

    public void CharacterMove()
    {
        if (isStop || isSceneMove 
            || !ServerManager.Instance.GetIsConnect()
            || GameManager.Instance.chattingUI.isActivated) return;

        //float z = Input.GetAxisRaw("Horizontal");
        float x = Input.GetAxisRaw("Vertical");
        inputVec = new Vector3(0, 0, x);
        Vector3 dir = new Vector3(0, 0, x) * moveSpeed * Time.deltaTime;

        this.transform.Translate(dir);

        if (x != 0 /*|| z != 0*/)
        {
            myManager.playerAnimation.CharacterMove(true);
            isMove = true;

            if (myManager.unitInfo.state == STATE.IDLE)
                myManager.unitInfo.state = STATE.MOVE;
        }
        else
        {
            myManager.playerAnimation.CharacterMove(false);
            isMove = false;

            if (myManager.unitInfo.state == STATE.MOVE)
                myManager.unitInfo.state = STATE.IDLE;
        }

        UpdateMovement();
    }

    private void RotationTowardMovementDirection()
    {
        if (myManager.unitInfo.state == STATE.ATTACK)
            return;

        if (inputVec != Vector3.zero &&
            targetDirection != Vector3.zero)
        {
            playerMesh.rotation =
                Quaternion.Slerp(playerMesh.rotation,
                Quaternion.LookRotation(targetDirection),
                Time.deltaTime * rotationSpeed);
        }
    }

    public void TargetLookAtMesh(Vector3 _target)
    {
        playerMesh.rotation =
            Quaternion.Euler(
                playerMesh.eulerAngles.x,
                GetDegree(this.transform.position, _target),
                playerMesh.eulerAngles.z);
    }

    float GetDegree(Vector3 _to, Vector3 _from)
    {
        return Mathf.Atan2(_to.x - _from.x, _to.z - _from.z) * 180.0f / Mathf.PI - 180.0f;
    }

    private void SendPositionUpdate()
    {
        if (isSceneMove) return;

        if (isMove)
        {
            sendTime += Time.deltaTime;

            if (sendTime >= sendInterval)
            {
                SendPlayerPosition();
                sendTime = 0.0f;
            }
        }
    }

    private void SendPlayerPosition()
    {
        Vector position = new Vector(
            this.transform.position.x, this.transform.position.z);
        Vector direction = new Vector(
           playerMesh.forward.x, playerMesh.forward.z);

        UserPositionPacket positionPacket;

        if(isMove_)
        {
            myManager.unitInfo.state = STATE.MOVE;

            //Debug.Log("Send_USERMOVE");
            positionPacket = new UserPositionPacket(SendCommand.C2Zone_USER_MOVE, 
                myManager.userInfo.userID, new Position(direction, position.x, position.z));
        }
        else
        {
            myManager.unitInfo.state = STATE.IDLE;

            //Debug.Log("Send_USERMOVE_FINISH");
            positionPacket = new UserPositionPacket(SendCommand.C2Zone_USER_MOVE_FINISH,
                myManager.userInfo.userID, new Position(direction, position.x, position.z));
        }

        ServerManager.Instance.SendData_ZoneServer(positionPacket.GetBytes());
    }

    private void HorizontalMovement()
    {
        float z = Input.GetAxisRaw("Horizontal");

        transform.Rotate(0, z * rotationSpeed, 0);
    }

    private void UpdateMovement()
    {
        Vector3 motion = inputVec;

        motion *= (Mathf.Abs(inputVec.x) == 1 && Mathf.Abs(inputVec.z) == 1) ? 0.7f : 1.0f;

        HorizontalMovement();
        RotationTowardMovementDirection();
        SendPositionUpdate();
    }
}
