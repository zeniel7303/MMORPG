using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerAnimation : MonoBehaviour
{
    public Animator animator;

    PlayerManager myManager;

    // Start is called before the first frame update
    void Start()
    {
        if (animator == null)
        {
            animator = this.GetComponent<Animator>();
        }
    }

    public void Init(PlayerManager _player)
    {
        myManager = _player;
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void AnimationPlay(string _name)
    {
        animator.SetTrigger(_name);
    }

    public void CharacterMove(bool _isMove)
    {
        animator.SetBool("Move", _isMove);
    }

    public void AttackStart()
    {
        //PlayerManager.instance.bIsAttack = true;
        animator.SetTrigger("BaseAttack");
    }

    public void AttackFinish()
    {
        //PlayerManager.instance.bIsAttack = false;
    }

    public AnimatorStateInfo GetAnimationState()
    {
        AnimatorStateInfo animatorInfo = animator.GetCurrentAnimatorStateInfo(0);

        return animatorInfo;
    }

    public int AnimationCompleteProgress(string _animationName)
    {
        AnimatorStateInfo animatorInfo = animator.GetCurrentAnimatorStateInfo(0);

        if (animatorInfo.IsName(_animationName))
        {
            if (animatorInfo.normalizedTime >= 0.95f)
            {
                return 2;
            }
            return 1;
        }
        return 0;
    }

    public bool AttackAnimationProgress(string _animationName, float _interval)
    {
        AnimatorStateInfo animatorInfo = animator.GetCurrentAnimatorStateInfo(0);

        if (animatorInfo.IsName(_animationName))
        {
            // Debug.Log("Attack!");
            if (animatorInfo.normalizedTime >= _interval)
            {
                return true;
            }
        }

        return false;
    }
}
