using UnityEngine;

public class RecoverAmmoScript : MonoBehaviour
{
    public int ammoToRecover;
    public AudioClip ammoClip;

    private void OnTriggerEnter(Collider other) {
        if (other.CompareTag("Player")) 
        {
            GetComponent<Collider>().enabled = false;
            GetComponent<MeshRenderer>().enabled = false;
            GetComponent<AudioSource>().PlayOneShot(ammoClip);
            other.GetComponent<AMMOScript>().RechargeAmmo(ammoToRecover);
            Destroy(gameObject, 5);
        }

    }



}
