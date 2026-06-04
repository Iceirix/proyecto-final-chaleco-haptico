// ============================================================================
//  VibrationMotors.ino - Control de 4 zonas vibratorias del chaleco.
//
//  Cada zona tiene dos modos:
//    - PWM continuo: M1..M4 enviados por Unity, 0..255.
//    - Toque de dano: DMG=1..4, patron corto no bloqueante.
// ============================================================================

const int PWM_MAX_DUTY = 255;

const unsigned long TAP_ON_MS = 80;
const unsigned long TAP_OFF_MS = 60;
const int TAP_PULSES = 3;

const int motorPins[4] = { MOTOR1_PIN, MOTOR2_PIN, MOTOR3_PIN, MOTOR4_PIN };

struct DamagePattern
{
  bool active;
  int pulsesLeft;
  bool onPhase;
  unsigned long phaseEndMs;
};

DamagePattern dmg[4];

void InitVibrationMotors()
{
  for (int i = 0; i < 4; i++)
  {
    pinMode(motorPins[i], OUTPUT);
    WriteMotor(i, 0);
    dmg[i] = { false, 0, false, 0 };
  }
}

void StartDamageTap(int zone)
{
  if (zone < 1 || zone > 4) return;

  int i = zone - 1;
  dmg[i].active = true;
  dmg[i].pulsesLeft = TAP_PULSES;
  dmg[i].onPhase = true;
  dmg[i].phaseEndMs = millis() + TAP_ON_MS;
  WriteMotor(i, PWM_MAX_DUTY);
}

void UpdateVibrationMotors()
{
  if (damageZoneTrigger >= 1 && damageZoneTrigger <= 4)
  {
    StartDamageTap(damageZoneTrigger);
    damageZoneTrigger = 0;
  }

  unsigned long now = millis();

  for (int i = 0; i < 4; i++)
  {
    if (dmg[i].active)
    {
      if ((long)(now - dmg[i].phaseEndMs) >= 0)
      {
        if (dmg[i].onPhase)
        {
          dmg[i].onPhase = false;
          dmg[i].phaseEndMs = now + TAP_OFF_MS;
          dmg[i].pulsesLeft--;
          WriteMotor(i, 0);
          if (dmg[i].pulsesLeft <= 0)
          {
            dmg[i].active = false;
          }
        }
        else
        {
          dmg[i].onPhase = true;
          dmg[i].phaseEndMs = now + TAP_ON_MS;
          WriteMotor(i, PWM_MAX_DUTY);
        }
      }
    }
    else
    {
      WriteMotor(i, motorTarget[i]);
    }
  }
}

void WriteMotor(int index, int duty)
{
  if (index < 0 || index >= 4) return;
  if (duty < 0) duty = 0;
  if (duty > 255) duty = 255;
  analogWrite(motorPins[index], duty);
}
