using UnityEngine;

public class InstantiateScript : MonoBehaviour
{

    public GameObject prefab;
    public int maxEnemies = 10;
    public GameObject[] spawnPoints;
    private int counter;
    private int spawnPointsIndex;
    public float firstTime = 2f;
    public float constantTime = 3f;

    

    void Start()
    {

        InvokeRepeating("InstantiateEnemy", firstTime,constantTime);


    }

   
    private void InstantiateEnemy() 
    {
        spawnPointsIndex = Random.Range(0,spawnPoints.Length);
        Instantiate(prefab, spawnPoints[spawnPointsIndex].transform.position, Quaternion.identity);
        counter++;


        if (counter >= maxEnemies)            
        {
            Destroy(gameObject);
                
        }
    }
}
