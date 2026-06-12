// ============================================================================
//  Inputs.ino - Lectura de botones, joysticks y sensor de flexion.
// ============================================================================

void ReadInputs()
{
  jump = digitalRead(JUMP_PIN);
  pb2Reserved = digitalRead(PB2_PIN);
  pb3Reserved = digitalRead(PB3_PIN);
  pb4Reserved = digitalRead(PB4_PIN);

  // Eje X invertido en ambos joysticks: el hardware quedo cableado al reves
  // y movia izquierda/derecha en sentido contrario al esperado.
  vrx1 = -NormalizeJoystick(analogRead(JOY1_X_PIN));
  vry1 = NormalizeJoystick(analogRead(JOY1_Y_PIN));
  vrx2 = -NormalizeJoystick(analogRead(JOY2_X_PIN));
  vry2 = NormalizeJoystick(analogRead(JOY2_Y_PIN));

  ApplyDeadzone();

  flexValue = analogRead(FLEX_PIN);
  UpdateShootFromFlex();
}

float NormalizeJoystick(int raw)
{
  return mapFloat((float)raw, 0.0f, 4095.0f, 1.0f, -1.0f);
}

void ApplyDeadzone()
{
  if (fabsf(vrx1) < JOY_DEADZONE) vrx1 = 0.0f;
  if (fabsf(vry1) < JOY_DEADZONE) vry1 = 0.0f;
  if (fabsf(vrx2) < JOY_DEADZONE) vrx2 = 0.0f;
  if (fabsf(vry2) < JOY_DEADZONE) vry2 = 0.0f;
}

void UpdateShootFromFlex()
{
#if FLEX_TRIGGER_WHEN_BELOW
  if (!shoot && flexValue <= flexTriggerThreshold)
  {
    shoot = true;
  }
  else if (shoot && flexValue >= flexTriggerThreshold + FLEX_TRIGGER_HYSTERESIS)
  {
    shoot = false;
  }
#else
  if (!shoot && flexValue >= flexTriggerThreshold)
  {
    shoot = true;
  }
  else if (shoot && flexValue <= flexTriggerThreshold - FLEX_TRIGGER_HYSTERESIS)
  {
    shoot = false;
  }
#endif
}
