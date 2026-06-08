using UnityEngine;
using UnityEngine.AI;
public class NaveMeshAgentScript : MonoBehaviour
{
    public GameObject destiny;
    public NavMeshAgent naveMeshAgent;





    void Start()
    {
       naveMeshAgent = GetComponent<NavMeshAgent>();




    }

   
    void Update()
    {
       naveMeshAgent.SetDestination(destiny.transform.position);
        



    }
}
