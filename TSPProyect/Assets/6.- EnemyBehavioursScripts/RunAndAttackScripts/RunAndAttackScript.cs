using System.Runtime.CompilerServices;
using UnityEngine;
using UnityEngine.AI;

public class RunAndAttackScript : MonoBehaviour {

    public bool isAttacking;
    public bool closeToAttack;
    public int damageToPlayer;
    public float minSpeed;
    public float maxSpeed;
    public AudioClip stepClip;

    private GameObject player;
    private AudioSource audioSource;
    private NavMeshAgent navMeshAgent;
    private Animator animator;

    void Start() {
        animator = GetComponent<Animator>();
        audioSource = GetComponent<AudioSource>();
        navMeshAgent = GetComponent<NavMeshAgent>();
        navMeshAgent.speed = Random.Range(minSpeed, maxSpeed);
        player = GameObject.FindGameObjectWithTag("Player");

    }

    void Update() 
    {
        if (!closeToAttack && !isAttacking) 
        {
            RunTowardsPlayer();
        }
        else if (closeToAttack)
        {
           Attack();        
        }
    }

    private void RunTowardsPlayer() 
    {
        navMeshAgent.isStopped = false;
        animator.SetInteger("AnimState", 0);
        navMeshAgent.SetDestination(player.transform.position);

    }
    private void Attack()
    { 
        navMeshAgent.isStopped = true;
        animator.SetInteger("AnimState", 1);
    }

    public void AttackStarted() 
    { 
        isAttacking = true; 

    }

    public void AttackStopped() 
    {
        isAttacking = false;

    }

    public void StepSound() 
    {
        audioSource.PlayOneShot(stepClip);
    
    }

    private void OnTriggerEnter(Collider other) 
    {
        if (other.CompareTag("Player"))    
        { 
            closeToAttack = true;
        
        }
    }

    private void OnTriggerExit(Collider other) 
    {
        if (other.CompareTag("Player")) 
        {
            closeToAttack = false;

        }
    }

}




