using UnityEngine;

public class HitPlayerScript : MonoBehaviour
{
    private int damageToPlayer;

    private void Start() 
        
    {
        damageToPlayer  = GetComponentInParent<RunAndAttackScript>().damageToPlayer;

    }

    private void OnTriggerEnter(Collider other) {
        if (other.CompareTag("Player")) {
            other.gameObject.GetComponent<PlayersHealthScript>().DamagePlayer(damageToPlayer);
        }

    }

}
