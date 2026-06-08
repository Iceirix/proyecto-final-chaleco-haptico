using UnityEngine;

public class GetComponentScript : MonoBehaviour
{
    public float shootTime;
    public AudioSource  audioSource ;

    private float counter;
    void Start()
    {
        audioSource = GetComponent<AudioSource>();
    }

    
    void Update()
    {
        counter += Time.deltaTime;

        if(Input.GetButtonUp("A") && counter >= shootTime) 
            { 

            counter =0;
            audioSource.Play(); 
        
            }
    }
}
