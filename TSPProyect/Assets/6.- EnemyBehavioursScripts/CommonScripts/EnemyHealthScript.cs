using UnityEngine.AI;
using UnityEngine;

public class EnemyHealthScript : MonoBehaviour
{
    public int enemyHealth;
    public Collider collider;
    public AudioClip dieClip;
    public Collider handCollider;

    private Animator animator;
    private AudioSource audioSource;

    private void Start() 
    {
         animator = GetComponent<Animator>();
        audioSource = GetComponent<AudioSource>();  
    }

    public void DamageEnemy(int damage) 
    {
        enemyHealth -= damage;
        if (enemyHealth <= 0) 
        {
            Die();
        }
    }


    private void Die() 
    {
        collider.enabled= false;
        animator.SetInteger("AnimState", 2);
        audioSource.PlayOneShot(dieClip);
        GetComponent<NavMeshAgent>().enabled = false;

        if (GetComponent<RunAndAttackScript>()!=null) 
        {
            handCollider.enabled = false;
            GetComponent<RunAndAttackScript>().enabled = false;
        }

        if (GetComponent<IdleRunAndShootScript>() != null) 
        {
            GetComponent<IdleRunAndShootScript>().enabled = false;
        }

        Destroy(gameObject, 3);
    }
}