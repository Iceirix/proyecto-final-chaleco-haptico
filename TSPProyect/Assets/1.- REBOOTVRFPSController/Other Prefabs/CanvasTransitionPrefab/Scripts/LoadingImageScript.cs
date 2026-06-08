using UnityEngine;
using UnityEngine.UI;

public class LoadingImageScript : MonoBehaviour
{
    public Text loadingText1;
    public Text loadingText2;
    public Text loadingTextCentral;
    public AudioClip loadingClip;     

    private AudioSource audioSource;

    void Start()
    {
        audioSource = GetComponent<AudioSource>();        
    }    

    public void LoadingSound()
    {
        audioSource.PlayOneShot(loadingClip);

        if (REBOOTBTInputs.android)
        {
            loadingText1.gameObject.SetActive(true);
            loadingText2.gameObject.SetActive(true);
            loadingTextCentral.gameObject.SetActive(false);
        }

        else
        {
            loadingTextCentral.gameObject.SetActive(true);
        }
    }
}
