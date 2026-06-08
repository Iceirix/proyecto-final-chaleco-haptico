using UnityEngine;

public class RecoverHealthScript : MonoBehaviour
{
    public int healthToRecover;
    public AudioClip healthClip;

    private void OnTriggerEnter(Collider other) 
    {
        if (other.CompareTag("Player")) 
            
        {
            GetComponent<Collider>().enabled = false;
            GetComponent<MeshRenderer>(). enabled = false;
            GetComponent<AudioSource>().PlayOneShot(healthClip);
            other.GetComponent<PlayersHealthScript>().RecoverHealth(healthToRecover);
            Destroy(gameObject, 5);
        }

    }



}
