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

    // Dano sin direccion conocida: vibran las 4 zonas a la vez.
    public void DamagePlayer(int damageAmount)
    {
        ApplyDamage(damageAmount, 15);
    }

    // Dano direccional: recibe la posicion del atacante en coordenadas de
    // mundo y vibra solo las zonas del chaleco que miran hacia el.
    public void DamagePlayer(int damageAmount, Vector3 attackerWorldPosition)
    {
        ApplyDamage(damageAmount, ComputeHitMask(attackerWorldPosition));
    }

    // Convierte la posicion del atacante en la mascara de zonas del comando
    // HIT= del firmware: bit0=M1 frente-izq, bit1=M2 frente-der,
    // bit2=M3 espalda-izq, bit3=M4 espalda-der. Sectores de 90 grados:
    // frente = ambos motores frontales, atras = ambos traseros,
    // costado = el motor frontal y el trasero de ese lado.
    private int ComputeHitMask(Vector3 attackerWorldPosition)
    {
        Vector3 local = transform.InverseTransformPoint(attackerWorldPosition);
        float angle = Mathf.Atan2(local.x, local.z) * Mathf.Rad2Deg; // 0 = frente, +90 = derecha

        if (angle > -45f && angle <= 45f) { return 1 | 2; }   // frente: M1+M2
        if (angle > 45f && angle <= 135f) { return 2 | 8; }   // derecha: M2+M4
        if (angle > -135f && angle <= -45f) { return 1 | 4; } // izquierda: M1+M3
        return 4 | 8;                                          // espalda: M3+M4
    }

    private void ApplyDamage(int damageAmount, int hitMask)
    {
        health -= damageAmount;

        health = Mathf.Clamp(health, 0, 100);

        // Un solo paquete con golpe + vida: con TCP NoDelay sale inmediato y el
        // firmware ademas usa HP= para los pulsos de calor por vida baja.
        WIFIConnectionScript.SendData("HIT=" + hitMask + ";HP=" + health);

        UpdateLifeBar();
        audioSource.PlayOneShot(hitClip);

        if (health <= 0)
        {
            print("Muri� el jugador");
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
