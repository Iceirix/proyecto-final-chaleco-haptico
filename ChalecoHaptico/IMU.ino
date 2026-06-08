// ============================================================================
//  IMU.ino — Driver minimo del BMI160 (acelerometro + giroscopio) por I2C
//
//  Direccion del sensor: 0x69 (BMI160 con SA0 = Vcc).
//  Ejes: X y Y producen roll y pitch via filtro complementario.
//  El yaw (rotacion alrededor de Z) no se puede estimar sin magnetometro,
//  por lo que se omite.
//
//  IMPORTANTE: el sketch original "Arduino IDE/IMU/" tenia un bug en el
//  filtro complementario (multiplicaba donde debia sumar). Aqui esta corregido.
// ============================================================================

const uint8_t IMU_ADDR = 0x69;

// Constantes de conversion
const float RAD_TO_DEG_F = 57.2957795f;
const float ACC_LSB_PER_G   = 16384.0f;  // rango +/- 2g
const float GYR_LSB_PER_DPS = 16.4f;     // rango +/- 2000 deg/s

// Factores de filtro
const float ALPHA_ACC = 0.6f;   // suavizado del angulo del acelerometro
const float BETA_COMP = 0.8f;   // peso del giroscopio en el filtro complementario

void InitIMU()
{
  // Despertar acelerometro y giroscopio (modo normal)
  WriteRegister(0x7E, 0x11); // CMD: acc_set_pmu_mode = normal
  delay(50);
  WriteRegister(0x7E, 0x15); // CMD: gyr_set_pmu_mode = normal
  delay(80);

  // Acelerometro: 400 Hz, rango +/- 2g
  WriteRegister(0x40, 0x0C);
  WriteRegister(0x41, 0x03);

  // Giroscopio: 400 Hz, rango +/- 2000 deg/s
  WriteRegister(0x42, 0x0C);
  WriteRegister(0x43, 0x00);

  delay(50);
  imuLastUs = micros();
}

void ReadIMU()
{
  // Calcula dt real entre lecturas (en segundos)
  unsigned long now = micros();
  float dt = (now - imuLastUs) / 1000000.0f;
  imuLastUs = now;
  if (dt <= 0.0f || dt > 0.2f) dt = 0.001f; // protege ante saltos raros

  ReadAccelerometer();
  ReadGyroscope();

  // Angulos derivados del acelerometro (en grados)
  accAngleX = atan2f(-accX, sqrtf(accY * accY + accZ * accZ)) * RAD_TO_DEG_F;
  accAngleY = atan2f( accY, sqrtf(accX * accX + accZ * accZ)) * RAD_TO_DEG_F;

  // Suavizado IIR del angulo del acelerometro
  accAngleXFil = ALPHA_ACC * accAngleXFil + (1.0f - ALPHA_ACC) * accAngleX;
  accAngleYFil = ALPHA_ACC * accAngleYFil + (1.0f - ALPHA_ACC) * accAngleY;

  // Filtro complementario: integra giroscopio y corrige con acelerometro.
  // Formula correcta:  ang = beta * (ang + gyro*dt) + (1 - beta) * accAng
  rollAngle  = BETA_COMP * (rollAngle  - gyrX * dt) + (1.0f - BETA_COMP) * accAngleXFil;
  pitchAngle = BETA_COMP * (pitchAngle + gyrY * dt) + (1.0f - BETA_COMP) * accAngleYFil;

  UpdateWeaponFromRoll();
}

// Cambio de arma por giro del brazo. Gesto "inclinar y volver al centro":
// mientras el gesto este armado, pasar +UMBRAL avanza un arma y pasar -UMBRAL
// retrocede. Tras contar un cambio el gesto se desarma y solo vuelve a armarse
// cuando el roll regresa a la banda central (|roll| < REARM). Asi un giro
// cuenta como un solo cambio, sin ciclado descontrolado al mantener la mano
// girada. El indice da la vuelta (wrap) sobre WEAPON_COUNT armas.
void UpdateWeaponFromRoll()
{
#if WEAPON_ROLL_INVERT
  const int dir = -1;
#else
  const int dir = 1;
#endif

  if (weaponGestureArmed)
  {
    if (rollAngle >= WEAPON_ROLL_THRESHOLD)
    {
      weaponIndex = (weaponIndex + dir + WEAPON_COUNT) % WEAPON_COUNT;
      weaponGestureArmed = false;
    }
    else if (rollAngle <= -WEAPON_ROLL_THRESHOLD)
    {
      weaponIndex = (weaponIndex - dir + WEAPON_COUNT) % WEAPON_COUNT;
      weaponGestureArmed = false;
    }
  }
  else if (fabsf(rollAngle) < WEAPON_ROLL_REARM)
  {
    weaponGestureArmed = true;
  }
}

void ReadAccelerometer()
{
  Wire.beginTransmission(IMU_ADDR);
  Wire.write(0x12); // registro base del acelerometro
  Wire.endTransmission(false);
  Wire.requestFrom((int)IMU_ADDR, 6);

  if (Wire.available() == 6)
  {
    axRaw = (int16_t)(Wire.read() | (Wire.read() << 8));
    ayRaw = (int16_t)(Wire.read() | (Wire.read() << 8));
    azRaw = (int16_t)(Wire.read() | (Wire.read() << 8));
    accX = axRaw / ACC_LSB_PER_G;
    accY = ayRaw / ACC_LSB_PER_G;
    accZ = azRaw / ACC_LSB_PER_G;
  }
}

void ReadGyroscope()
{
  Wire.beginTransmission(IMU_ADDR);
  Wire.write(0x0C); // registro base del giroscopio
  Wire.endTransmission(false);
  Wire.requestFrom((int)IMU_ADDR, 6);

  if (Wire.available() == 6)
  {
    gxRaw = (int16_t)(Wire.read() | (Wire.read() << 8));
    gyRaw = (int16_t)(Wire.read() | (Wire.read() << 8));
    gzRaw = (int16_t)(Wire.read() | (Wire.read() << 8));
    gyrX = gxRaw / GYR_LSB_PER_DPS;
    gyrY = gyRaw / GYR_LSB_PER_DPS;
    gyrZ = gzRaw / GYR_LSB_PER_DPS;
  }
}

// Helper interno para escribir un byte a un registro del BMI160
void WriteRegister(uint8_t reg, uint8_t value)
{
  Wire.beginTransmission(IMU_ADDR);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}
