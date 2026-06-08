using UnityEngine;

public class SelectWeaponScript : MonoBehaviour
{
    public bool canSelect;    
    public bool selectingWeaponFinnished;
    public int weaponIndex;
    public float dividerAngle;
    public float selectorDistance;
    public AudioClip changeWeaponClip;
    public GameObject selectionHUD;    
    public GameObject[] weapons;
    public GameObject[] weaponsIndicators;

    private bool selectingWeapon;
    private bool changeWeaponButton;    
    private int previousSelectedWeapon;
    private float angle;
    private float finalAngle;
    private float middleMousePositionX;
    private float middleMousePositionY;
    private Vector3 mousePosition;
    public Vector2 currentMousePosition;
    private Animator animator;
    private AudioSource audioSource;

    void Start()
    {
        selectionHUD.SetActive(false);
        animator = GetComponent<Animator>();
        middleMousePositionX = Screen.width / 2;
        middleMousePositionY = Screen.height / 2;
        audioSource = GetComponent<AudioSource>();
        weaponsIndicators[weaponIndex].SetActive(true);
    }

    public void ActivateSelector()
    {
        canSelect = true;
    }

    void Update()
    {
        Inputs();

        if (changeWeaponButton && canSelect)
        {
            selectingWeaponFinnished = false;
            selectingWeapon = !selectingWeapon;
            selectionHUD.SetActive(selectingWeapon);            

            if (selectingWeapon)
            {
                Cursor.lockState = CursorLockMode.Confined;                

               GetComponent<VRHeadScript>().canRotate = false;
            }

            else
            {
                if (previousSelectedWeapon != weaponIndex)
                {
                    ChangeWeapon();
                }
            }            
        }

        if (selectingWeapon)
        {
            SelectWeapon();
        }
    }

    public void Inputs()
    {
        mousePosition = Input.mousePosition;
        changeWeaponButton = Input.GetButtonDown("Fire3");
    }    

    private void SelectWeapon()
    {
        currentMousePosition = new Vector2(mousePosition.x - middleMousePositionX, mousePosition.y - middleMousePositionY).normalized;
        
        angle = Vector3.Angle(currentMousePosition, Vector3.up);

        if (Vector3.Cross(currentMousePosition, Vector3.up).z >= 0)
        {
            finalAngle = angle;
        }

        else
        {
            finalAngle = 360 - angle;
        }

        weaponIndex = (int)(finalAngle / dividerAngle);

        if (previousSelectedWeapon != weaponIndex)
        {
            foreach (GameObject weaponIndicator in weaponsIndicators)
            {
                weaponIndicator.SetActive(false);
            }

            if (weaponIndex < weapons.Length)
            {
                weaponsIndicators[weaponIndex].SetActive(true);
            }
        }
    }

    private void ChangeWeapon()
    {
        GetComponent<VRHeadScript>().canRotate = true;
        

        if (!audioSource.isPlaying)
        {
            audioSource.PlayOneShot(changeWeaponClip);
        }        

        previousSelectedWeapon = weaponIndex;
        animator.SetInteger("AnimState", 3);
        Cursor.lockState = CursorLockMode.Locked;
    }

    private void AnimationKeyFrame()
    {   
        foreach (GameObject weapon in weapons)
        {
            weapon.SetActive(false);
        }

        weapons[weaponIndex].SetActive(true);
    }

    public void AnimationStopped() // Indica que ya terminó de realizar el cambio de arma (terminó animación)
    {
        selectingWeaponFinnished = true;
        animator.SetBool("ChangeWeaponFinnished", true); //5
    }
}
