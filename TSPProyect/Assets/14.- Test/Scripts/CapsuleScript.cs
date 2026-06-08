using UnityEngine;

public class CapsuleScript : MonoBehaviour
{
    public AudioSource audioSource;
    public GameObject cube;
    void Start()
    {
        cube = GameObject.FindGameObjectWithTag("Cube");
        audioSource = GetComponent<AudioSource>();
    }

    
    void Update()
    {
        if (Input.GetButtonDown("Fire1")) 
            {
             cube.GetComponent<CubeHealthScript>().DamageCube(10);
            audioSource.Play();
        }
    }
}