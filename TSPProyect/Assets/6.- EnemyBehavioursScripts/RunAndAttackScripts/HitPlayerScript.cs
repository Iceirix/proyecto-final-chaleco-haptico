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
            // Se pasa la posicion del enemigo para que el chaleco vibre solo
            // del lado por el que llego el golpe.
            other.gameObject.GetComponent<PlayersHealthScript>().DamagePlayer(damageToPlayer, transform.position);
        }

    }

}
