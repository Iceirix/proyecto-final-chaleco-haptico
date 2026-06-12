// ============================================================================
//  ProtocolReceive.ino - Parser de comandos entrantes desde Unity.
//
//  Formato recomendado, terminado en salto de linea:
//    M1=0;M2=0;M3=0;M4=0;SOL=0;HP=100;DMG=1;PELT=0
//
//  El mismo formato tambien se puede escribir manualmente en el monitor Serial
//  para probar actuadores sin interrumpir la comunicacion normal por WiFi.
//
//  M1..M4: PWM continuo de las 4 zonas del chaleco.
//  SOL: pulso de solenoide para retroceso (Unity manda SOL=1 por cada disparo).
//  HP: vida 0..100 para NeoPixel. Tambien dispara los pulsos automaticos de la
//      Peltier cuando baja de PELTIER_LOW_HP_THRESHOLD (ver Peltier.ino).
//  DMG: golpe haptico en una sola zona 1..4 + flash rojo (legado).
//  HIT: golpe haptico direccional, bitmask 1..15 de zonas simultaneas
//       (bit0=M1 frente-izq, bit1=M2 frente-der, bit2=M3 espalda-izq,
//        bit3=M4 espalda-der) + flash rojo. Ej: HIT=3 = golpe de frente.
//  PELT: golpe termico de la celda Peltier (pulso largo). Solo actua si
//        ENABLE_PELTIER=1. ELEC y REL son alias de PELT por compatibilidad.
// ============================================================================

String wifiRxBuffer = "";
String serialRxBuffer = "";

int ClampInt(int v, int lo, int hi)
{
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

int ClampByte(int v)
{
  return ClampInt(v, 0, 255);
}

void ReceiveCommandsFromUnity()
{
  if (!(client && client.connected())) return;

  while (client.available())
  {
    char c = client.read();
    if (c == '\n' || c == '\r')
    {
      if (wifiRxBuffer.length() > 0)
      {
        ProcessRxLine(wifiRxBuffer);
        wifiRxBuffer = "";
      }
    }
    else
    {
      wifiRxBuffer += c;
      if (wifiRxBuffer.length() > 200) wifiRxBuffer = "";
    }
  }
}

void ReceiveCommandsFromSerial()
{
  while (Serial.available())
  {
    char c = Serial.read();
    if (c == '\n' || c == '\r')
    {
      if (serialRxBuffer.length() > 0)
      {
        ProcessRxLine(serialRxBuffer);
        Serial.print("CMD OK: ");
        Serial.println(serialRxBuffer);
        serialRxBuffer = "";
      }
    }
    else
    {
      serialRxBuffer += c;
      if (serialRxBuffer.length() > 200)
      {
        serialRxBuffer = "";
        Serial.println("CMD descartado: linea demasiado larga");
      }
    }
  }
}

void ProcessRxLine(const String& line)
{
  int start = 0;
  int len = line.length();
  while (start < len)
  {
    int sep = line.indexOf(';', start);
    if (sep < 0) sep = len;

    String token = line.substring(start, sep);
    token.trim();
    if (token.length() > 0) ApplyToken(token);

    start = sep + 1;
  }
}

void ApplyToken(const String& token)
{
  int eq = token.indexOf('=');
  if (eq <= 0) return;

  String key = token.substring(0, eq);
  String val = token.substring(eq + 1);
  key.trim();
  key.toUpperCase();
  val.trim();

  int v = val.toInt();

  if      (key == "M1")   motorTarget[0] = ClampByte(v);
  else if (key == "M2")   motorTarget[1] = ClampByte(v);
  else if (key == "M3")   motorTarget[2] = ClampByte(v);
  else if (key == "M4")   motorTarget[3] = ClampByte(v);
  else if (key == "SOL")  solenoidTrigger = (v != 0);
  else if (key == "HP")   healthPercent = ClampInt(v, 0, 100);
  else if (key == "PELT") peltierTrigger = (v != 0);
  else if (key == "ELEC") peltierTrigger = (v != 0); // alias: la salida ahora es Peltier
  else if (key == "REL")  peltierTrigger = (v != 0); // alias de compatibilidad
  else if (key == "DMG")
  {
    if (v >= 1 && v <= 4)
    {
      damageZoneMask |= (1 << (v - 1));
      damageFlashTrigger = true;
    }
  }
  else if (key == "HIT")
  {
    if (v >= 1 && v <= 15)
    {
      damageZoneMask |= v;
      damageFlashTrigger = true;
    }
  }
}
