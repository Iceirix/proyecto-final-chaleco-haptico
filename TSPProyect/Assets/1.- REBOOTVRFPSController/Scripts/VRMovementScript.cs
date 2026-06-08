using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class VRMovementScript : MonoBehaviour
{
    private bool UIOn;
    private float Ax;
    private float Ay;
    private float Az;
    //////////////Acelerómetro/////////////
    private float pitchAcc;
    private float pitchAccFil;
    //////////////Giroscopio////////////////
    private float pitchGyr;
    private float yawRateGyr;
    /// //////////Yaw FInal////////////////
    public float yawFin;
    //////////////PitchFinal////////////////
    public float pitchFin;
    //////////////Interfaz////////////////
    public Text AxText;
    public Text AyText;
    public Text AzText;
    public Text rollText;
    public Text yawText;
    public Text input;
    public GameObject UI;
    public GameObject openButton;
    public Slider alfa;
    public Slider beta;
    
    void Start()
    {
        UIOn = false;
        Input.gyro.enabled = true;
        Screen.orientation = ScreenOrientation.LandscapeLeft;
        Cursor.visible = false;
        Cursor.lockState = CursorLockMode.Locked;
        alfa.value = 0.8f;
        beta.value = 0.6f;

        if (REBOOTBTInputs.android)
        {
            openButton.SetActive(true);
        }

        else
        {
            openButton.SetActive(false);
        }
    }

    void FixedUpdate()
    {
        Ax = Input.acceleration.x;
        Ay = Input.acceleration.y;
        Az = Input.acceleration.z;
        pitchGyr = -Input.gyro.rotationRateUnbiased.x * Time.deltaTime * Mathf.Rad2Deg;
        yawRateGyr = -Input.gyro.rotationRateUnbiased.y * Mathf.Rad2Deg;
        pitchAcc = -1*Mathf.Atan((Ay) / Mathf.Sqrt(Mathf.Pow(Ax, 2) + Mathf.Pow(Az, 2))) * Mathf.Rad2Deg;
        
        if(Input.acceleration.z < 0)
        {
            pitchAcc = -1*(pitchAcc - 90);
        }

        else
        {
            pitchAcc = -1*(90 - pitchAcc);
        }

        pitchAccFil = alfa.value * pitchAccFil + (1 - alfa.value) * pitchAcc;
        pitchFin = beta.value * (pitchFin + pitchGyr) + (1 - beta.value) * (pitchAccFil);
        yawFin = yawRateGyr * Time.deltaTime + yawFin;
        AxText.text = Ax.ToString("0.00");
        AyText.text = Ay.ToString("0.00");
        AzText.text = Az.ToString("0.00");
        rollText.text = pitchFin.ToString("0.00");
        yawText.text = yawFin.ToString("0.00");

        if(REBOOTBTInputs.android)
        {
            input.text = string.Join(".", REBOOTBTInputs.dataArray);
        }        
    }

    public void OnSettingsPressed()
    {
        UIOn = !UIOn;
        UI.SetActive(UIOn);
    }

    public void OnStartButtonPressed()
    {
        SceneManager.LoadScene(2, LoadSceneMode.Single);
        UI.SetActive(false);
    }
}
