// ============================================================================
//  Solenoid.ino - Pulso del solenoide del guante para retroceso.
// ============================================================================

const unsigned long SOLENOID_PULSE_MS = 100;
const unsigned long SOLENOID_COOLDOWN_MS = 160;

void InitSolenoid()
{
  digitalWrite(SOLENOID_PIN, LOW);
  solenoidActive = false;
  solenoidReadyMs = 0;
}

void UpdateSolenoid()
{
  unsigned long now = millis();

  if (solenoidTrigger && !solenoidActive && (long)(now - solenoidReadyMs) >= 0)
  {
    solenoidActive = true;
    solenoidEndMs = now + SOLENOID_PULSE_MS;
    digitalWrite(SOLENOID_PIN, HIGH);
  }
  solenoidTrigger = false;

  if (solenoidActive && (long)(now - solenoidEndMs) >= 0)
  {
    digitalWrite(SOLENOID_PIN, LOW);
    solenoidActive = false;
    solenoidReadyMs = now + SOLENOID_COOLDOWN_MS;
  }
}
