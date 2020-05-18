using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerAttackManage : MonoBehaviour
{
    [HideInInspector]
    public PlayerManager myManager;

    public Transform myMesh;

    [HideInInspector]
    public List<Monster> selectedMonstersList;
    [HideInInspector]
    public Monster selectedMonster;

    float attackCoolTime;
    public bool isInAttackCoolTime;

    public void Init(PlayerManager _player)
    {
        myManager = _player;

        myMesh = GameObject.Find("Mesh").transform;

        attackCoolTime = 0.0f;
        isInAttackCoolTime = false;
    }

    void Update()
    {
        if (isInAttackCoolTime) attackCoolTime += Time.deltaTime;
        if (attackCoolTime >= 2) isInAttackCoolTime = false;

        if (myManager.unitInfo.state != STATE.ATTACK) return;

        AnimatorStateInfo animatorInfo = 
            myManager.playerAnimation.animator.GetCurrentAnimatorStateInfo(0);

        if (animatorInfo.IsName("BasicAttack"))
        {
            if(animatorInfo.normalizedTime >= 0.99f)
            {
                myManager.unitInfo.state = STATE.IDLE;
                myManager.playerMovement.Rollback();
            }
        }
    }

    public void Attack()
    {
        myManager.unitInfo.state = STATE.ATTACK;
        isInAttackCoolTime = true;

        myManager.playerMovement.Stay();
        myManager.playerAnimation.AnimationPlay("BasicAttack");

        BasicAttack();
    }

    public void BasicAttack()
    {
        MapManager mapManager = GameManager.Instance.mapManager;

        if (mapManager == null) return;

        //메쉬 정면으로 해서 정면 보게함.
        myMesh.localEulerAngles = Vector3.zero;
        Vector direction = new Vector(
           myMesh.forward.x, myMesh.forward.z);

        selectedMonstersList = mapManager.CheckMonsterInRange
            (180.0f, 2.0f);

        if (selectedMonstersList.Count == 0)
        {
            Debug.Log("헛공격");

            UserAttackPacket userAttackPacket =
                    new UserAttackPacket(SendCommand.C2Zone_USER_ATTACK_FAILED,
                    myManager.userInfo.userID, 0, direction);

            ServerManager.Instance.SendData(userAttackPacket.GetBytes());

            return;
        }
        else
        {
            for (int i = 0; i < selectedMonstersList.Count; i++)
            {
                UserAttackPacket userAttackPacket =
                    new UserAttackPacket(SendCommand.C2Zone_USER_ATTACK_MONSTER,
                    myManager.userInfo.userID, selectedMonstersList[i].monsterInfo.index, direction);

                ServerManager.Instance.SendData(userAttackPacket.GetBytes());
            }
        }
    }

    public void ResetSelectedMonstersList()
    {
        selectedMonstersList.Clear();
    }
}