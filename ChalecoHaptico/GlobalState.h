#ifndef GLOBAL_STATE_H
#define GLOBAL_STATE_H

// WiFi AP + TCP.
const char* AP_SSID = "ESP32_Chaleco";
const char* AP_PASSWORD = "123456789";
const int AP_MAX_CONNECTIONS = 8;  // estaciones: PC con Unity + varios celulares en el dashboard
WiFiServer server(80);
WiFiClient client;

// DNS captivo: cualquier nombre que el celular consulte (incluido "chaleco" y
// las pruebas de captive portal) se resuelve a la IP del AP. Asi el usuario
// puede tipear directamente "chaleco" en el navegador.
DNSServer dnsServer;

// Dashboard HTTP (servidor independiente para no interferir con Unity).
#if ENABLE_DASHBOARD
WiFiServer dashboardServer(DASHBOARD_PORT);
WiFiClient dashboardClients[DASHBOARD_MAX_STREAMS]; // streams SSE persistentes, uno por dispositivo
uint8_t dashboardStreamStrikes[DASHBOARD_MAX_STREAMS] = {0}; // tramas seguidas sin poder escribir (cliente zombi)
// Peticiones HTTP (pagina, /cmd) en slots keep-alive: el navegador reusa una
// sola conexion para todos los comandos. Abrir una conexion por comando (como
// antes) agotaba los PCB TCP de lwIP al arrastrar un slider y lwIP terminaba
// matando conexiones activas, incluido el stream SSE.
WiFiClient dashboardReqClients[DASHBOARD_MAX_REQ];
String dashboardRxBuffer[DASHBOARD_MAX_REQ];
String dashboardMethod[DASHBOARD_MAX_REQ];
String dashboardPath[DASHBOARD_MAX_REQ];
unsigned long dashboardReqLastMs[DASHBOARD_MAX_REQ] = {0};
unsigned long dashboardLastMs = 0;
#endif

// Inputs.
bool jump = false;
bool shoot = false;
bool pb2Reserved = false;   // PB2 libre: reservado para accion futura
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

// Cambio de arma por giro del brazo (roll de la IMU). Gesto "inclinar y volver
// al centro": al superar +UMBRAL avanza un arma, al pasar -UMBRAL retrocede, y
// el gesto se rearma solo cuando el roll regresa a la banda central. Asi cada
// giro cuenta como un solo cambio. weaponIndex es la fuente de verdad y se
// envia a Unity como indice absoluto 0..WEAPON_COUNT-1 (ver ProtocolSend.ino).
// La direccion del giro se invierte con WEAPON_ROLL_INVERT en HardwareConfig.h.
int weaponIndex = 0;
bool weaponGestureArmed = true;
const int   WEAPON_COUNT = 4;                 // armas en la rueda de Unity
const float WEAPON_ROLL_THRESHOLD = 45.0f;    // grados de roll para contar un cambio
const float WEAPON_ROLL_REARM = 20.0f;        // banda central (grados) para rearmar el gesto

int16_t axRaw = 0, ayRaw = 0, azRaw = 0;
int16_t gxRaw = 0, gyRaw = 0, gzRaw = 0;
float accX = 0.0f, accY = 0.0f, accZ = 0.0f;
float gyrX = 0.0f, gyrY = 0.0f, gyrZ = 0.0f;
float accAngleX = 0.0f, accAngleY = 0.0f;
float accAngleXFil = 0.0f, accAngleYFil = 0.0f;
unsigned long imuLastUs = 0;

// Comandos de Unity.
int motorTarget[4] = {0, 0, 0, 0};   // PWM continuo pedido por los sliders/Unity (intensidad 0..255)
int motorLevel[4] = {0, 0, 0, 0};    // intensidad realmente escrita al pin (incluye toques de dano); es lo que ve el dashboard
int activeDamageZone = 0;            // zona 1..4 con un toque de dano en curso (0 = ninguno); para el flash del dashboard
int healthPercent = 100;
bool solenoidTrigger = false;
bool peltierTrigger = false;
int damageZoneTrigger = 0;
bool damageFlashTrigger = false;

// Estado de actuadores usado tambien por Debug.ino.
bool solenoidActive = false;
unsigned long solenoidEndMs = 0;
unsigned long solenoidReadyMs = 0;

bool peltierActive = false;
unsigned long peltierEndMs = 0;

#endif
