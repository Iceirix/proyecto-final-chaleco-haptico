using UnityEngine.UI;
using UnityEngine;

public class VRHeadScript : MonoBehaviour
{
    public bool canRotate = true;
    public float smoothRotation = 0.004f; //3
    public float mouseSensitivity;
    public GameObject cameraCentral;
   	public GameObject vrHead;
    public GameObject vrBody;

    private float angle1;
    private float angle2;
    private float mouseX;
    private float mouseY;
    private float rotationSpeedX = 0.0f; //3
    private float rotationSpeedY = 0.0f; //3
    private float rightLeftRotation;
    private float upDownRotation;
    private VRMovementScript vrMovementScript;

    private void Start()
    {
        Cursor.lockState = CursorLockMode.Locked; //Bloque el cursor al centro
        Cursor.visible = false; //Hace invisible el cursor
        
    }

    

    void Update()
    {
        if(canRotate)
        {
            mouseX = WIFIConnectionScript.J2X * mouseSensitivity;
            mouseY = WIFIConnectionScript.J2Y * mouseSensitivity;
            rightLeftRotation += mouseX;
            upDownRotation -= mouseY;
            upDownRotation = Mathf.Clamp(upDownRotation, -70, 70);
        }           
    }

    private void FixedUpdate()
    {
        RotateLeftRight();
        RotateUpDown();
    }

    private void RotateLeftRight()
    {
        if(canRotate)
        {
            angle1 = Mathf.SmoothDampAngle(transform.eulerAngles.y, rightLeftRotation, ref rotationSpeedY, smoothRotation); //3
            transform.rotation = Quaternion.Euler(0, angle1, 0); //2
        }       
    }

    private void RotateUpDown()
    {
        if (canRotate)
        {
            angle2 = Mathf.SmoothDampAngle(vrHead.transform.eulerAngles.x, upDownRotation, ref rotationSpeedX, smoothRotation); //3
            vrHead.transform.localRotation = Quaternion.Euler(angle2, 0, 0); //2
        }
    }
}
