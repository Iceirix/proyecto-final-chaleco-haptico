void InitializeAccelerometer()
{
  Wire.beginTransmission(0x69); // Direccion de la IMU
  Wire.write(0x7E); // (registro) encender el acelerometro 
  Wire.write(0x11); // (instruccion) modo normal del acelerometro
  Wire.endTransmission();
  delay(100);
}

void ConfiguraAccelerometer()
{
  Wire.beginTransmission(0x69); // Direccion de la IMU
  Wire.write(0x40); // (registro) velocidad del acelerometro 
  Wire.write(0x0C); // (instruccion) 400 Hz, mediciones por segundo
  Wire.endTransmission();
  delay(100);

  Wire.beginTransmission(0x69); // Direccion de la IMU
  Wire.write(0x41); // (registro) rango del acelerometro
  Wire.write(0x03); // (instruccion) rango de 2G
  Wire.endTransmission();
  delay(100);
}

void ReadAccelerometer()
{
  Wire.beginTransmission(0x69); // Direccion de la IMU
  Wire.write(0x12); // (registro) registro base del acelerometro
  Wire.endTransmission(false); // false para evitar que cierre del todo la comunicacion

  Wire.requestFrom(0x69, 6); // 3 ejes, se piden 2 bytes por eje

  if(Wire.available() == 6)
  {
    ax = (int16_t)(Wire.read() | (Wire.read() << 8));
    ay = (int16_t)(Wire.read() | (Wire.read() << 8));
    az = (int16_t)(Wire.read() | (Wire.read() << 8));

    // valores de aceleracion lineal
    xAccelerometer = ax / 16384.0; // En G's
    yAccelerometer = ay / 16384.0; // En G's
    zAccelerometer = az / 16384.0; // En G's
    EulerAngles();
    FilterAccelerometer();
  }
}

void EulerAngles()
{
  xAccAngle = atan(-xAccelerometer / sqrt(pow(yAccelerometer, 2) + pow(zAccelerometer, 2))) * radToDeg;
  yAccAngle = atan(yAccelerometer / sqrt(pow(xAccelerometer, 2) + pow(zAccelerometer, 2))) * radToDeg;
}

void FilterAccelerometer()
{
  xAccAngleFil = alfa * xAccAngleFil + (1 - alfa) * xAccAngle;
  yAccAngleFil = alfa * yAccAngleFil + (1 - alfa) * yAccAngle;
}

