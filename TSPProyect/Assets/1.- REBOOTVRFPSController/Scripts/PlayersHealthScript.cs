using UnityEngine.UI;
using UnityEngine;

public class PlayersHealthScript : MonoBehaviour
{
    public int health;
    public Image lifeBarImage;
    public AudioClip hitClip;


    private AudioSource audioSource;

    private void Start() 
        
    {
        UpdateLifeBar();
        audioSource=GetComponent<AudioSource>();
    }

    public void DamagePlayer(int damageAmount)
    {
        health -= damageAmount;

        health = Mathf.Clamp(health, 0, 100);

        WIFIConnectionScript.SendData("HP=" + health);
        WIFIConnectionScript.SendData(
        "M1=100;M2=100;M3=100;M4=100"
    );

        UpdateLifeBar();
        audioSource.PlayOneShot(hitClip);

        if (health <= 0)
        {
            print("Murió el jugador");
        }
    }
    public void RecoverHealth(int healthToRecover)
    {
        health += healthToRecover;

        health = Mathf.Clamp(health, 0, 100);

        WIFIConnectionScript.SendData("HP=" + health);

        UpdateLifeBar();
    }
    private void UpdateLifeBar() 
    { 
       lifeBarImage.fillAmount = health / 100f;    
    
    }


}
