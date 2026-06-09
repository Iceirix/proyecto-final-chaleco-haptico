// ============================================================================
//  Pinout.ino - Configuracion inicial de pines.
//
//  Las constantes fisicas estan en HardwareConfig.h para que Arduino IDE las vea
//  antes de concatenar las pestanas .ino del sketch.
//
//  Restricciones relevantes de la PCB:
//  - GPIO34, GPIO35, GPIO36 y GPIO39 son solo entrada.
//  - GPIO9 y GPIO10 pertenecen a la flash del ESP32-WROOM; no se usan para MP3.
//  - GPIO0 es pin de arranque. La salida SOL puede requerir desconectar el
//    solenoide/base TIP120 durante carga o arranque si impide boot normal.
// ============================================================================

void InitPinout()
{
  pinMode(PB1_PIN, INPUT_PULLDOWN);
  pinMode(PB2_PIN, INPUT_PULLDOWN);
  pinMode(PB3_PIN, INPUT_PULLDOWN);
  pinMode(PB4_PIN, INPUT_PULLDOWN);

  pinMode(SOLENOID_PIN, OUTPUT);
  digitalWrite(SOLENOID_PIN, LOW);

  pinMode(PELTIER_RELAY_PIN, OUTPUT);
  digitalWrite(PELTIER_RELAY_PIN, LOW);
}

void InitI2CBus()
{
  Wire.begin(IMU_SDA_PIN, IMU_SCL_PIN);
}
