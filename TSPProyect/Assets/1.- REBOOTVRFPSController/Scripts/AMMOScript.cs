using UnityEngine.UI;
using UnityEngine;

public class AMMOScript : MonoBehaviour
{
    public int ammoAmount;
    public Image ammoBarImage;

    private void Start() 
        
    {
       UpdateAmmoBar();
        
    }

    public void UseAMMO(int ammoToUse) 
        
    {
        ammoAmount -= ammoToUse;
        ammoAmount = Mathf.Clamp(ammoAmount, 0, 100);
        UpdateAmmoBar();
    }
public void RechargeAmmo(int ammoToRecharge) 
        
    {
        ammoAmount += ammoToRecharge;
        ammoAmount = Mathf.Clamp(ammoAmount, 0, 100);
        UpdateAmmoBar();
    }
    private void UpdateAmmoBar() 
        
    {
        ammoBarImage.fillAmount = ammoAmount / 100f;
        
    }
    


}
