using UnityEngine;
using UnityEngine.UI;


public class REBOOTBTInputs : MonoBehaviour
{
    public static bool android;

    public static string[] dataArray;
    public static float primaryTrigger;
    public static float primaryTriggerMem;
    public static float secondaryTrigger;
    public static float secondaryTriggerMem;
    public static float tertiaryTrigger;
    public static float tertiaryTriggerMem;
    public static float x;
    public static float y;

    //public Text primaryTriggerText;
    private float mouseWheelCounter;
    
    private void Awake()
    {
        /*var devices = InputSystem.devices;
        inputControls = new InputControls();
        inputControls.AndroidControl.Enable();
        InputDevice[] inputDevicesArray = devices.ToArray();
        inputDevicesArrayString = new string[devices.Count];
        var control = InputSystem.FindControl("<AndroidGamepadWithDpadAxes>");

        for (int i = 0; i < inputDevicesArray.Length; i++)
        {
             inputDevicesArrayString[i] = inputDevicesArray[i].name + "\n";
        }
        
        primaryTriggerText.text = string.Join("/", inputDevicesArrayString );
        inputControls.AndroidControl.PrimaryTrigger.performed += ctx => primaryTrigger = ctx.ReadValue<float>();
        inputControls.AndroidControl.SecondaryTrigger.performed += ctx => secondaryTrigger = ctx.ReadValue<float>();
        inputControls.AndroidControl.TertiaryTrigger.performed += ctx => tertiaryTrigger = ctx.ReadValue<float>();
        inputControls.AndroidControl.GetX.performed += ctx => x = ctx.ReadValue<float>();
        inputControls.AndroidControl.GetY.performed += ctx => y = ctx.ReadValue<float>();
        inputControls.AndroidControl.GetX.canceled += ctx => x = 0;
        inputControls.AndroidControl.GetY.canceled += ctx => y = 0;*/

        if (Application.platform == RuntimePlatform.Android)
        {
            android = true;
        }

        else if(Application.platform == RuntimePlatform.WindowsEditor || Application.platform == RuntimePlatform.WindowsPlayer)
        {
            android = false;
            /*inputControls = new InputControls();
            inputControls.AndroidControl.Enable();
            inputControls.AndroidControl.PrimaryTrigger.performed += ctx => primaryTrigger = ctx.ReadValue<float>();
            inputControls.AndroidControl.SecondaryTrigger.performed += ctx => secondaryTrigger = ctx.ReadValue<float>();
            inputControls.AndroidControl.TertiaryTrigger.performed += ctx => tertiaryTrigger = ctx.ReadValue<float>();
            inputControls.AndroidControl.GetX.performed += ctx => x = ctx.ReadValue<float>();
            inputControls.AndroidControl.GetY.performed += ctx => y = ctx.ReadValue<float>();
            inputControls.AndroidControl.GetX.canceled += ctx => x = 0;
            inputControls.AndroidControl.GetY.canceled += ctx => y = 0;*/
        }

        /*if (android)
        {
            inputControls = new InputControls();
            inputControls.AndroidControl.PrimaryTrigger.performed += ctx => primaryTrigger = ctx.ReadValue<float>();
            inputControls.AndroidControl.SecondaryTrigger.performed += ctx => secondaryTrigger = ctx.ReadValue<float>();
            inputControls.AndroidControl.TertiaryTrigger.performed += ctx => tertiaryTrigger = ctx.ReadValue<float>();
            inputControls.AndroidControl.GetX.performed += ctx => x = ctx.ReadValue<float>();
            inputControls.AndroidControl.GetY.performed += ctx => y = ctx.ReadValue<float>();
            inputControls.AndroidControl.GetX.canceled += ctx => x = 0;
            inputControls.AndroidControl.GetY.canceled += ctx => y = 0;
        }*/
    }

    private void OnEnable()
    {
        if (android)
        {
            //inputControls.AndroidControl.Enable();
        }
        
    }

    private void OnDisable()
    {
        if (android)
        {
            //inputControls.AndroidControl.Disable();
        }
    }

    private void OnReceiveData(string data)
    {
        //entrada: primario, secundario(botón palanca), terciario, x, y,  
        //dataArray = data.Split(new char[] {','});
        //int.TryParse(dataArray[0], out primaryTrigger);
        //int.TryParse(dataArray[1], out secondaryTrigger);
        //int.TryParse(dataArray[2], out tertiaryTrigger);
        //float.TryParse(dataArray[3], out x);
        //float.TryParse(dataArray[4], out y);
    }

    private void Update()
    {
        
        
    }

    private void LateUpdate()
    {
        primaryTriggerMem = primaryTrigger;
        secondaryTriggerMem = secondaryTrigger;
        tertiaryTriggerMem = tertiaryTrigger;
    }

    /// /////////////////////////////Primary////////////////////////////
    public static bool GetPrimaryDown()
    {
        return Input.GetButtonDown("AndroidInputPrimary");
        /*if (android)
        {
            return Input.GetButtonDown("AndroidInputPrimary");
        }

        else
        {
            if (primaryTrigger == 1 && primaryTriggerMem == 0)
            {
                return true;
            }

            else
            {
                return false;
            }
        } */       
    }

    public static bool GetPrimaryUp()
    {
        if (android)
        {
            return Input.GetButtonUp("AndroidInputPrimary");
        }

        else
        {
            if (primaryTrigger == 0 && primaryTriggerMem == 1)
            {
                return true;
            }

            else
            {
                return false;
            }
        }        
    }

    public static bool GetPrimary()
    {
        return Input.GetButton("AndroidInputPrimary");
        /*if (android)
        {
            return Input.GetButton("AndroidInputPrimary");
        }

        else
        {
            if (primaryTrigger == 1)
            {
                return true;
            }

            else
            {
                return false;
            }
        }   */     
    }

    /// ////////////////////////////Secondary//////////////////////////
    public static bool GetSecondaryDown()
    {
        return Input.GetButtonDown("AndroidInputSecondary");
        /*if (android)
        {
            return Input.GetButtonDown("AndroidInputSecondary");
        }

        else
        {
            if (secondaryTrigger == 1 && secondaryTriggerMem == 0)
            {
                return true;
            }

            else
            {
                return false;
            }
        }*/        
    }

    public static bool GetSecondaryUp()
    {
        if (android)
        {
            return Input.GetButtonUp("AndroidInputSecondary");
        }

        else
        {
            if (secondaryTrigger == 0 && secondaryTriggerMem == 1)
            {
                return true;
            }

            else
            {
                return false;
            }
        }
    }

    public static bool GetSecondary()
    {
        return Input.GetButton("AndroidInputSecondary");
        /*if (android)
        {
            return Input.GetButton("AndroidInputSecondary");
        }

        else
        {
            if (secondaryTrigger == 1)
            {
                return true;
            }

            else
            {
                return false;
            }
        }   */     
    }

    ///////////////////////////////Terciary//////////////////////////
    public static bool GetTertiaryDown()
    {
        return Input.GetButtonDown("AndroidInputTertiary");
        /*if (android)
        {
            return Input.GetButtonDown("AndroidInputTertiary");
        }

        else
        {
            if (tertiaryTrigger == 1 && tertiaryTriggerMem == 0)
            {
                return true;
            }

            else
            {
                return false;
            }
        }  */      
    }

    public static bool GetTertiaryUp()
    {
        if (android)
        {
            return Input.GetButtonUp("AndroidInputTertiary");
        }

        else
        {
            if (tertiaryTrigger == 0 && tertiaryTriggerMem == 1)
            {
                return true;
            }

            else
            {
                return false;
            }
        }
    }

    public static bool GetTertiary()
    {
        return Input.GetButton("AndroidInputTertiary");
        /*if (android)
        {
            return Input.GetButton("AndroidInputTertiary");
        }

        else
        {
            if (tertiaryTrigger == 1)
            {
                return true;
            }

            else
            {
                return false;
            }
        }  */     
    }

    public static bool GetNorth()
    {
        return Input.GetButtonDown("AndroidInputNorth");
    }
    public static bool GetSouth()
    {
        return Input.GetButton("AndroidInputSouth");        
    }
    public static bool GetEast()
    {
        return Input.GetButtonDown("AndroidInputEast");
    }
    public static bool GetWest()
    {
        return Input.GetButtonDown("AndroidInputWest");
    }
    public static bool GetR1()
    {
        return Input.GetButtonDown("AndroidInputR1");
    }
    public static bool GetL1()
    {
        return Input.GetButtonDown("AndroidInputL1");
    }
    public static bool GetStart()
    {
        return Input.GetButtonDown("AndroidInputStart");
    }

    public static bool GetSelect()
    {
        return Input.GetButtonDown("AndroidInputSelect");
    }
    ///////////////////////////////Joystick//////////////////////////
    public static float GetX()
    {
        return Input.GetAxis("AndroidLeftAxisX");
    }

    public static float GetY()
    {
        return -Input.GetAxis("AndroidLeftAxisY");
    }

    public static float GetX2()
    {
        return Input.GetAxis("AndroidRightAxisX");
    }

    public static float GetY2()
    {
        return Input.GetAxis("AndroidRightAxisY");              
    }       
}
