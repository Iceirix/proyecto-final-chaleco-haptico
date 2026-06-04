// ============================================================================
//  ChalecoHaptico.ino - Firmware para chaleco haptico + guante de control
//  Plataforma: ESP32-WROOM-32 DevKit de 38 pines en la PCB TSM 2026-II.
//
//  Lee:
//    - 2 joysticks analogicos
//    - sensor de flexion como gatillo
//    - botones PB1/PB2 para salto y cambio de arma
//    - IMU BMI160 por I2C
//
//  Actua:
//    - solenoide de retroceso en el guante
//    - 4 motores vibradores PWM en el chaleco
//    - barra de vida NeoPixel
//    - rele de electrodos, bloqueado por software hasta validacion fisica
//
//  Comunica:
//    - WiFi TCP, ESP32 como Access Point en 192.175.5.1:80 (Unity)
//    - Dashboard web embebido en http://192.175.5.1:81/ (navegador)
// ============================================================================

#include <WiFi.h>
#include <Wire.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <Adafruit_NeoPixel.h>
#include "HardwareConfig.h"
#include "GlobalState.h"

const unsigned long SEND_PERIOD_MS = 20; // 50 Hz
unsigned long lastSendMs = 0;

// 0 = silencio, 1 = estado general a 5 Hz, 2 = agrega IMU cruda.
#define DEBUG_VERBOSE 1
const unsigned long DEBUG_PERIOD_MS = 200;
unsigned long lastDebugMs = 0;

void setup()
{
  Serial.begin(115200);

  InitPinout();
  InitInputs();
  InitVibrationMotors();
  InitSolenoid();
  InitElectrodeRelay();
  InitHealthBar();

  InitI2CBus();
  InitIMU();

  ConfigureAP();
  InitDashboard();
}

void loop()
{
  dnsServer.processNextRequest();
  TryAcceptClient();

  ReadInputs();
  ReadIMU();
  ReceiveCommandsFromUnity();
  ReceiveCommandsFromSerial();

  UpdateVibrationMotors();
  UpdateSolenoid();
  UpdateElectrodeRelay();
  UpdateHealthBar();

  unsigned long now = millis();
  if (now - lastSendMs >= SEND_PERIOD_MS)
  {
    lastSendMs = now;
    SendStateToUnity();
  }

  // El dashboard se actualiza despues de enviar a Unity para no retrasar la
  // trama del videojuego. Lee los mismos globales, asi que no hay coste extra
  // de muestreo y la latencia hacia Unity se mantiene.
  UpdateDashboard();

  if (now - lastDebugMs >= DEBUG_PERIOD_MS)
  {
    lastDebugMs = now;
    DebugPrintAll();
  }
}

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
