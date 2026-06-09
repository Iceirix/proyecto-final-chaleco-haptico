// ============================================================================
//  Peltier.ino - Golpe termico de la celda Peltier (antes electrodos).
//
//  La celda cuelga de un rele de 5 V controlado por TIP120. El comando PELT
//  (alias ELEC/REL) la enciende por PELTIER_PULSE_MS y se apaga sola, igual que
//  el solenoide pero con un pulso largo: una Peltier tarda segundos en producir
//  un cambio termico notable, por eso un pulso corto "no se activaba".
//
//  Por seguridad, si ENABLE_PELTIER esta en 0 (HardwareConfig.h) el firmware
//  reconoce el comando pero nunca cierra el rele fisicamente.
// ============================================================================

void InitPeltier()
{
  digitalWrite(PELTIER_RELAY_PIN, LOW);
  peltierActive = false;
  peltierEndMs = 0;
}

void UpdatePeltier()
{
  // Un nuevo golpe se ignora mientras hay uno en curso (no se reinicia el timer).
  if (peltierTrigger && !peltierActive)
  {
#if ENABLE_PELTIER
    peltierActive = true;
    peltierEndMs = millis() + PELTIER_PULSE_MS;
    digitalWrite(PELTIER_RELAY_PIN, HIGH);
#endif
  }
  peltierTrigger = false;

  if (peltierActive && (long)(millis() - peltierEndMs) >= 0)
  {
    digitalWrite(PELTIER_RELAY_PIN, LOW);
    peltierActive = false;
  }
}
