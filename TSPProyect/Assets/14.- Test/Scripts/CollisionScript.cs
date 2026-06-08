using UnityEngine;

public class CollisionScript : MonoBehaviour
{

    private void OnCollisionEnter(Collision collision)
        
    {
        print("Cochó");    

    }
    private void OnCollisionStay(Collision collision)
        
    {
        print("Sigue Cochando");

    }
    private void OnCollisionExit(Collision collision)
        
    {
        print("Dejó de chocar");

    }

   
}
