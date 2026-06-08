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
    private Transform aimTransform;
    private ParticleSystem muzzleFlareParticles;


    private void Start()

    {
       audioSource = GetComponent<AudioSource>();
       animator = GetComponentInParent<Animator>();
        ammoScript = GetComponentInParent<AMMOScript>();
        selectWeaponScript = GetComponentInParent<SelectWeaponScript>();
        ResolveAim();

        // Cachea el sistema de particulas del fogonazo si esta asignado. Si la
        // referencia 'muzzleFlare' falta en el inspector, simplemente no hay
        // fogonazo (en vez de lanzar UnassignedReferenceException al disparar).
        if (muzzleFlare != null)
        {
            muzzleFlareParticles = muzzleFlare.GetComponent<ParticleSystem>();
        }
    }

    // Resuelve el punto de mira para el raycast. Si 'aim' no se asigno en el
    // inspector, cae a Camera.main, luego a cualquier camara de la escena y,
    // como ultimo recurso, al transform del arma. Asi nunca se lanza
    // UnassignedReferenceException al disparar (esa excepcion pausaba el juego
    // con "Error Pause" activado).
    private void ResolveAim()
    {
        if (aim != null)
        {
            aimTransform = aim.transform;
            return;
        }

        Camera cam = Camera.main;
        if (cam == null) { cam = FindObjectOfType<Camera>(); }

        if (cam != null)
        {
            aimTransform = cam.transform;
            Debug.LogWarning(name + ": variable 'aim' sin asignar en WeaponScript; usando la camara '" + cam.name + "' como mira. Asignala en el inspector para una punteria exacta.", this);
        }
        else
        {
            aimTransform = transform;
            Debug.LogWarning(name + ": variable 'aim' sin asignar y sin camara en escena; usando el transform del arma como mira.", this);
        }
    }

    private void Update() 
    {
        // Disparo por sensor de flexion del guante (WIFIConnectionScript.Shoot),
        // con Fire1 como alternativa para pruebas con teclado/mouse en PC.
        primaryDown = Input.GetButton("Fire1") || WIFIConnectionScript.Shoot;
        counter += Time.deltaTime;

        if(primaryDown&&counter >= timeToRecharge && selectWeaponScript.selectingWeaponFinnished ) 
        {
            Shoot();
            print("Disparo");
        }
    }

    private void Shoot()
    {
        // Sin munición suficiente no se dispara. Si no hay AMMOScript asignado,
        // no bloqueamos el disparo (solo se omite el descuento de munición).
        if (ammoScript != null && ammoScript.ammoAmount < ammoRequired) { return; }

        if (aimTransform == null) { ResolveAim(); }

        counter = 0;
        origin = aimTransform.position;
        direction = aimTransform.forward;

        // Cada referencia opcional se usa solo si esta asignada: asi una falta en
        // el inspector degrada el efecto (sin animacion/sonido/fogonazo) en vez
        // de lanzar una excepcion que pausaria el juego.
        if (animator != null) { animator.SetTrigger("Shoot"); }
        if (audioSource != null && shootingClip != null) { audioSource.PlayOneShot(shootingClip); }
        if (muzzleFlareParticles != null) { muzzleFlareParticles.Play(); }
        if (ammoScript != null) { ammoScript.UseAMMO(ammoRequired); }

        if (Physics.Raycast(origin, direction, out raycastHit, 100f, LayerMask.GetMask("Enemy")))
        {
            EnemyHealthScript enemyHealth = raycastHit.collider.GetComponent<EnemyHealthScript>();
            if (enemyHealth != null) { enemyHealth.DamageEnemy(damageToEnemy); }
        }
    }

}
