void InitializeGyroscope()
{
  Wire.beginTransmission(0x69); // Direccion de la IMU
  Wire.write(0x7E); // Registro CMD
  Wire.write(0x11); // modo normal giroscopio
  Wire.endTransmission();
  delay(100);
}

void ConfigureGyroscope()
{
  Wire.beginTransmission(0x69);
  Wire.write(0x42); // velocidad de giro
  Wire.write(0x0C); // 400 Hz
  Wire.endTransmission();
  delay(100);

  Wire.beginTransmission(0x69);
  Wire.write(0x43); // rango del giro
  Wire.write(0x00); // rango de 2000°/s
  Wire.endTransmission();
  delay(100);
}

void ReadGyroscope()
{
  Wire.beginTransmission(0x69);
  Wire.write(0x0C); // registro base
  Wire.endTransmission(false); // false para evitar que cierre del todo la comunicacion

  Wire.requestFrom(0x69, 6); // 3 ejes, se piden 2 bytes por eje

  if(Wire.available() == 6)
  {
    gx = (int16_t)(Wire.read() | (Wire.read() << 8)); // << es corrimiento
    gy = (int16_t)(Wire.read() | (Wire.read() << 8));
    gz = (int16_t)(Wire.read() | (Wire.read() << 8));

    xRotation = gx / 16.4; // En °/s
    yRotation = gy / 16.4; // En °/s
    zRotation = gz / 16.4; // En °/s

    xGyr += (xRotation * dt);
    yGyr += (yRotation * dt);
    zGyr += (zRotation * dt);
  }
}