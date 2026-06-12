using UnityEngine;
using UnityEngine.AI;

public class IdleRunAndShootScript : MonoBehaviour
{
    public bool startPursuit;
    public int damageToPlayer;
    public float minSpeed;
    public float maxSpeed;
    public float rotationSpeed;
    public AudioClip stepClip;
    public AudioClip shootClip;
    public GameObject shootPoint;


    private bool shootPlayer;
    private Vector3 origin;
    private Vector3 direction;
    private Vector3 targetPosition;
    private RaycastHit raycastHit;
    private GameObject player;
    private AudioSource audioSource;
    private NavMeshAgent navMeshAgent;
    private Animator animator;

    void Start() 
    {
        animator = GetComponent<Animator>();
        audioSource = GetComponent<AudioSource>();
        navMeshAgent = GetComponent<NavMeshAgent>();
        navMeshAgent.speed = Random.Range(minSpeed, maxSpeed);
        player = GameObject.FindGameObjectWithTag("Player");

    }


    private void Update() 
    {
        if (!startPursuit) // Espera a que el jugador se acerque.
        {
            Idle();
            return;
        } 
        
        else if (!shootPlayer) // Corre detrás del jugador
        {
            RunTowardsPlayer();
            print("siguiendo");
        } 
        
        else if (shootPlayer) // Ataca al jugador
        {
            Attack();
        }
    }

    private void Idle() 
    {
        navMeshAgent.isStopped = true;
        animator.SetInteger("AnimState", 0);
    }

    private void RunTowardsPlayer() 
    {
        navMeshAgent.isStopped = false;
        animator.SetInteger("AnimState", 1);
        navMeshAgent.SetDestination(player.transform.position);
        print(player.transform.position);
    }

    private void Attack() 
    {
        navMeshAgent.isStopped = true;
        animator.SetInteger("AnimState", 3);
        targetPosition = player.transform.position - transform.position;
        Quaternion newRotation = Quaternion.LookRotation(targetPosition);
        transform.rotation = Quaternion.Lerp(transform.rotation, newRotation, rotationSpeed*0.001f);
    }

    public void StepSound() 
    {
        audioSource.PlayOneShot(stepClip);

    }

    private void OnTriggerEnter(Collider other) 
    {
        if (other.CompareTag("Player"))
        {
            shootPlayer = true;
        }
    }

    private void OnTriggerExit(Collider other) 
    {
        if (other.CompareTag("Player"))
        {
            shootPlayer = false;
        }
    }

    public void Shoot() 
    {
        audioSource.PlayOneShot(shootClip);
        origin = shootPoint.transform.position;
        direction = shootPoint.transform.forward;

        if (Physics.Raycast(origin, direction, out raycastHit, 100f, LayerMask.GetMask("Player"))) 
        {
            // Se pasa el origen del disparo para que el chaleco vibre solo del
            // lado por el que llego el balazo.
            raycastHit.collider.GetComponent<PlayersHealthScript>().DamagePlayer(damageToPlayer, origin);
        }
    }
}