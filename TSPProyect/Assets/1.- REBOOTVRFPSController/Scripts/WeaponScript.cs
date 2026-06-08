using UnityEngine;
using static UnityEngine.UI.Image;

public class WeaponScript : MonoBehaviour
{
    public int ammoRequired;
    public int damageToEnemy;
    public float timeToRecharge;
    public AudioClip shootingClip;
    public GameObject muzzleFlare;
    public GameObject aim;

    private bool primaryDown;
    private float counter;
    private Vector3 origin;
    private Vector3 direction;
    private RaycastHit raycastHit;
    private Animator animator;
    private AudioSource audioSource;
    private AMMOScript ammoScript;
    private SelectWeaponScript selectWeaponScript;


    private void Start() 
        
    {
       audioSource = GetComponent<AudioSource>();
       animator = GetComponentInParent<Animator>();    
        ammoScript = GetComponentInParent<AMMOScript>();
        selectWeaponScript = GetComponentInParent<SelectWeaponScript>();
    }

    private void Update() 
    {
        primaryDown = Input.GetButton("Fire1");
        counter += Time.deltaTime;

        if(primaryDown&&counter >= timeToRecharge && selectWeaponScript.selectingWeaponFinnished ) 
        {
            Shoot();
            print("Disparo");
        }
    }

    private void Shoot() 
    {
        if (ammoScript.ammoAmount >= ammoRequired) 
        {

            counter = 0;
            origin = aim.transform.position;
            direction = aim.transform.forward;
            animator.SetTrigger("Shoot");
            audioSource.PlayOneShot(shootingClip);
            muzzleFlare.GetComponent<ParticleSystem>().Play();
            ammoScript.UseAMMO(ammoRequired);

            if (Physics.Raycast(origin, direction, out raycastHit, 100f, LayerMask.GetMask("Enemy"))) 
            {
                raycastHit.collider.gameObject.GetComponent<EnemyHealthScript>().DamageEnemy(damageToEnemy);

            }
        }

    }

}
