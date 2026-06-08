using Unity.VisualScripting;
using UnityEngine;

public class CubeHealthScript : MonoBehaviour
{
    public int health = 100;
       
    public void DamageCube(int damege_amount) 
        {  

            health -= damege_amount;

        if(health <=0)
            { 
            Destroy(gameObject);
             }
    
    }
}
        
