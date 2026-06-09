// ============================================================================
//  VibrationMotors.ino - Control de 4 zonas vibratorias del chaleco.
//
//  Cada zona tiene dos modos:
//    - PWM continuo: M1..M4 enviados por Unity, 0..255.
//    - Toque de dano: DMG=1..4, patron corto no bloqueante.
// ============================================================================

const int PWM_MAX_DUTY = 255;

// Duty maximo real (0..255) que mantiene el promedio entregado en
// MOTOR_RATED_VOLTAGE. Con pack 8 V y motor 3 V => 255*3/8 ~= 96. Se recalcula
// en InitVibrationMotors a partir de los voltajes de HardwareConfig.h, asi que
// cambiar BATTERY_CELL_VOLTAGE basta para reajustar el limite.
int motorMaxDuty = PWM_MAX_DUTY;

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

// Calcula el duty maximo seguro segun los voltajes configurados. Si el pack no
// supera al motor no hay recorte. Se llama una vez al arrancar.
void ComputeMotorMaxDuty()
{
  if (BATTERY_PACK_VOLTAGE <= MOTOR_RATED_VOLTAGE)
  {
    motorMaxDuty = PWM_MAX_DUTY;
    return;
  }
  int d = (int)lroundf((float)PWM_MAX_DUTY * MOTOR_RATED_VOLTAGE / BATTERY_PACK_VOLTAGE);
  if (d < 1) d = 1;
  if (d > PWM_MAX_DUTY) d = PWM_MAX_DUTY;
  motorMaxDuty = d;
}

void InitVibrationMotors()
{
  ComputeMotorMaxDuty();
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
  activeDamageZone = zone;  // visible en el dashboard mientras dura el patron
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
            if (activeDamageZone == i + 1) activeDamageZone = 0;
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
  if (duty > PWM_MAX_DUTY) duty = PWM_MAX_DUTY;
  // Guarda la intensidad realmente aplicada (0..255, antes del recorte de
  // voltaje) para que el dashboard muestre la salida viva, incluidos los toques
  // de dano y no solo el valor de los sliders (motorTarget).
  motorLevel[index] = duty;
  // Recorta el promedio a MOTOR_RATED_VOLTAGE: la intensidad pedida (0..255) se
  // mapea al rango fisico seguro (0..motorMaxDuty). Aplica tambien a los toques
  // de dano, que llaman aqui con PWM_MAX_DUTY y tampoco deben pasar de 3 V.
  int scaled = (int)((long)duty * motorMaxDuty / PWM_MAX_DUTY);
  analogWrite(motorPins[index], scaled);
}
