// ============================================================================
//  ElectrodeRelay.ino - Pulso del rele que alimenta los electrodos.
//
//  Por seguridad, ENABLE_ELECTRODE_RELAY esta en 0 en HardwareConfig.h. Con ese
//  valor el firmware reconoce el comando, pero nunca activa fisicamente el rele.
// ============================================================================

const unsigned long ELECTRODE_PULSE_MS = 80;

void InitElectrodeRelay()
{
  digitalWrite(ELECTRODE_RELAY_PIN, LOW);
  electrodeActive = false;
}

void UpdateElectrodeRelay()
{
  if (electrodeTrigger && !electrodeActive)
  {
#if ENABLE_ELECTRODE_RELAY
    electrodeActive = true;
    electrodeEndMs = millis() + ELECTRODE_PULSE_MS;
    digitalWrite(ELECTRODE_RELAY_PIN, HIGH);
#endif
  }
  electrodeTrigger = false;

  if (electrodeActive && (long)(millis() - electrodeEndMs) >= 0)
  {
    digitalWrite(ELECTRODE_RELAY_PIN, LOW);
    electrodeActive = false;
  }
}
