using UnityEngine;

public class FSPControllerScript : MonoBehaviour
{
    public bool android;
    public bool canWalk = true;
    public float speed;
    public float speedMeM;
    public float runSpeed; //2   
    public float gravity = 9.81f;
    public float jumpSpeed;
    public float x, z;
    public Vector3 moveVector;
    public Vector3 gravityVector;

    private bool isGrounded;
    private bool primaryDown;
    private bool jump;
    private int isRunning; //2    

    private Animator animator;
    private CharacterController character;

    private void OnEnable()
    {
        speedMeM = speed;
    }

    void Start()
    {        
        android = REBOOTBTInputs.android;

        if (android)
        {
            Cursor.lockState = CursorLockMode.Locked;
            Cursor.visible = false;
        }      
        
        animator = GetComponent<Animator>();
        character = GetComponent<CharacterController>();       
    }    

    void Update()
    {
        if(REBOOTBTInputs.android)
        {
            x = WIFIConnectionScript.J1X;
            z = WIFIConnectionScript.J1Y;

            jump = WIFIConnectionScript.Jump;
            primaryDown = REBOOTBTInputs.GetPrimary();
        }

        else
        {
            x = WIFIConnectionScript.J1X;
            z = WIFIConnectionScript.J1Y;

            jump = WIFIConnectionScript.Jump;
            primaryDown = Input.GetButton("Fire1") || REBOOTBTInputs.GetPrimary();
        }

        isGrounded = character.isGrounded;        
    }

    private void FixedUpdate()
    {
        Move();
    }   

    private void Move()
    {
        moveVector = transform.right * x + transform.forward * z;

        if(canWalk)
        {
            character.Move(moveVector.normalized * speed * Time.deltaTime * 10);
        }        
        
        if(moveVector == Vector3.zero && !primaryDown) //Idle
        {
            animator.SetInteger("AnimState", 0);
        }

        if (moveVector != Vector3.zero && !primaryDown && canWalk) //Run
        {
            animator.SetInteger("AnimState", 1);
        }

        if (jump == true && isGrounded)
        {
            gravityVector.y = jumpSpeed / 20;
        }

        if (!isGrounded)
        {
            gravityVector.y -= gravity * Time.deltaTime * Time.deltaTime; 
        }

        character.Move(gravityVector);
    }
}
