using UnityEngine;

public class RigidBodyScript : MonoBehaviour
{
    public float Torque;


    void Start()
    {
         GetComponent<Rigidbody>().AddRelativeTorque(Vector3.right * Torque);

    }

}
