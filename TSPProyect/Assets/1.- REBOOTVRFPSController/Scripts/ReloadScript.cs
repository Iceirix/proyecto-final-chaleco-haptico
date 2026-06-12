using UnityEngine;

// Recarga manual de municion con el boton PB2 del guante (WIFIConnectionScript.Button2).
//
// El juego no tiene cargador: la municion es un pool unico 0..100 en AMMOScript.
// Para que la recarga no sea municion infinita, es una "recarga tactica" con
// cooldown: cada pulsacion recupera 'ammoToRecover' y luego hay que esperar
// 'cooldownSeconds'. Confirmacion haptica: un pulso de solenoide (SOL=1), que el
// firmware temporiza solo.
//
// Va en el mismo GameObject que AMMOScript (el Player). Fire2 sirve como
// alternativa de teclado/mouse para probar en PC sin el guante.
public class ReloadScript : MonoBehaviour
{
    public int ammoToRecover = 30;
    public float cooldownSeconds = 3f;
    public int maxAmmo = 100;
    public AudioClip reloadClip;       // opcional: sonido de recarga
    public bool sendHapticPulse = true; // pulso de solenoide al recargar

    private AMMOScript ammoScript;
    private AudioSource audioSource;
    private float cooldownCounter;
    private bool button2Mem;

    private void Start()
    {
        ammoScript = GetComponent<AMMOScript>();
        if (ammoScript == null) { ammoScript = GetComponentInChildren<AMMOScript>(); }

        audioSource = GetComponent<AudioSource>();

        // Permite recargar de inmediato al iniciar.
        cooldownCounter = cooldownSeconds;
    }

    private void Update()
    {
        cooldownCounter += Time.deltaTime;

        // Flanco de subida: del guante (Button2) o de Fire2 para pruebas en PC.
        bool button2 = WIFIConnectionScript.Button2;
        bool pressed = (button2 && !button2Mem) || Input.GetButtonDown("Fire2");
        button2Mem = button2;

        if (pressed)
        {
            TryReload();
        }
    }

    private void TryReload()
    {
        if (cooldownCounter < cooldownSeconds) { return; }   // aun en cooldown
        if (ammoScript == null) { return; }                  // sin sistema de municion
        if (ammoScript.ammoAmount >= maxAmmo) { return; }    // cargador lleno

        ammoScript.RechargeAmmo(ammoToRecover);
        cooldownCounter = 0f;

        if (audioSource != null && reloadClip != null) { audioSource.PlayOneShot(reloadClip); }

        // Confirmacion haptica: el solenoide da un "chunk" tactil de recarga.
        if (sendHapticPulse) { WIFIConnectionScript.SendData("SOL=1"); }
    }
}
