#ifndef GLOBAL_STATE_H
#define GLOBAL_STATE_H

// WiFi AP + TCP.
const char* AP_SSID = "ESP32_Chaleco";
const char* AP_PASSWORD = "123456789";
WiFiServer server(80);
WiFiClient client;

// DNS captivo: cualquier nombre que el celular consulte (incluido "chaleco" y
// las pruebas de captive portal) se resuelve a la IP del AP. Asi el usuario
// puede tipear directamente "chaleco" en el navegador.
DNSServer dnsServer;

// Dashboard HTTP (servidor independiente para no interferir con Unity).
#if ENABLE_DASHBOARD
WiFiServer dashboardServer(DASHBOARD_PORT);
WiFiClient dashboardClient;      // stream SSE persistente
WiFiClient dashboardReqClient;   // peticiones HTTP cortas (pagina, /cmd)
String dashboardRxBuffer = "";
String dashboardMethod = "";
String dashboardPath = "";
bool dashboardStreaming = false;
unsigned long dashboardLastMs = 0;
#endif

// Inputs.
bool jump = false;
bool shoot = false;
bool changeWeapon = false;
bool pb3Reserved = false;
bool pb4Reserved = false;

float vrx1 = 0.0f, vry1 = 0.0f;
float vrx2 = 0.0f, vry2 = 0.0f;
int flexValue = 0;
int flexRestValue = 0;
int flexTriggerThreshold = 0;

const float JOY_DEADZONE = 0.20f;
const int FLEX_TRIGGER_DELTA = 600;       // sin uso: umbral ahora es fijo
const int FLEX_TRIGGER_THRESHOLD = 1000;  // umbral fijo del ADC validado en bench
const int FLEX_TRIGGER_HYSTERESIS = 150;

// IMU.
float rollAngle = 0.0f;
float pitchAngle = 0.0f;

// Recarga por giro del brazo (roll de la IMU). Analoga al gatillo por flex:
// al superar el umbral se activa la recarga y se libera con histeresis para
// evitar rebotes. Ajusta estos dos valores aqui tras probar el guante; la
// direccion del gesto (subir o bajar el roll) se elige con RELOAD_WHEN_ABOVE
// en HardwareConfig.h.
bool reload = false;
const float RELOAD_ROLL_THRESHOLD = 60.0f;    // grados; angulo de roll que dispara la recarga
const float RELOAD_ROLL_HYSTERESIS = 15.0f;   // grados de retorno para soltar la recarga

int16_t axRaw = 0, ayRaw = 0, azRaw = 0;
int16_t gxRaw = 0, gyRaw = 0, gzRaw = 0;
float accX = 0.0f, accY = 0.0f, accZ = 0.0f;
float gyrX = 0.0f, gyrY = 0.0f, gyrZ = 0.0f;
float accAngleX = 0.0f, accAngleY = 0.0f;
float accAngleXFil = 0.0f, accAngleYFil = 0.0f;
unsigned long imuLastUs = 0;

// Comandos de Unity.
int motorTarget[4] = {0, 0, 0, 0};
int healthPercent = 100;
bool solenoidTrigger = false;
bool electrodeTrigger = false;
int damageZoneTrigger = 0;
bool damageFlashTrigger = false;

// Estado de actuadores usado tambien por Debug.ino.
bool solenoidActive = false;
unsigned long solenoidEndMs = 0;
unsigned long solenoidReadyMs = 0;

bool electrodeActive = false;
unsigned long electrodeEndMs = 0;

#endif
