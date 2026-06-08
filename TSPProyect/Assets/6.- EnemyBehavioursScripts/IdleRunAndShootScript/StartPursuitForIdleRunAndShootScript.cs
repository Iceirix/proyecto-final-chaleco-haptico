using UnityEngine;

public class StartPursuitForIdleRunAndShootScript : MonoBehaviour
{    private void OnTriggerEnter(Collider other) 
    {
        if (other.CompareTag("Player")) 
        { 
            GetComponentInParent<IdleRunAndShootScript>().startPursuit = true;
        }
    }
}

