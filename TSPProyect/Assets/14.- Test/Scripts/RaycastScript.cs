using UnityEngine;

public class RaycastScript : MonoBehaviour
{

    public int damage;
    public float timeToRecharge;
    public GameObject aim;

    private float counter;
    private Vector3 origin;
    private Vector3 direction;
    private RaycastHit raycastHit;


    void Start()
    {
        



    }

  
    void Update()
    {
        counter += Time.deltaTime; 
        

       if (Input.GetButtonDown("Fire1")&& counter >= timeToRecharge) 
            {
            Shoot();
             }

    }

    private void Shoot() 
        {
        counter = 0;
        origin = aim.transform.position;
        direction = aim.transform.forward;  
        if (Physics.Raycast(origin, direction, out raycastHit, 100f, LayerMask.GetMask("Enemy") ) ) 
        {
            raycastHit.collider.gameObject.GetComponent<CubeHealthScript>().DamageCube(damage); 
        

        }


        }

}
